#include "klnet.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>
#include <openssl/ssl.h>
#include <arpa/inet.h>
#include <array>

#include "klexcept.hpp"

namespace kl {

int connect_to_server(const Text& server, uint16_t port) {
  addrinfo hints;
  memset(&hints, 0, sizeof(addrinfo));
  hints.ai_flags = AI_ADDRCONFIG | AI_PASSIVE | AI_ALL | AI_NUMERICSERV;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  addrinfo* addresses = nullptr;
  auto callres = ::getaddrinfo(server.to_string().c_str(), std::to_string(port).c_str(), &hints, &addresses);
  if (0 != callres) {
    throw IOException(gai_strerror(callres));
  }

  addrinfo* candidate = addresses;
  const socklen_t buffer_size = 1024;
  std::array<char, buffer_size> buffer;
  for (auto p = addresses; p != nullptr; p = p->ai_next) {
    candidate = p;
    ::inet_ntop(p->ai_family, p->ai_addr, buffer.data(), buffer_size);
    if (p->ai_family == AF_INET6) {
      break;
    }
  }

  const int socketid = socket(candidate->ai_family, candidate->ai_socktype, candidate->ai_protocol);
  auto res = ::connect(socketid, candidate->ai_addr, candidate->ai_addrlen);
  freeaddrinfo(addresses);
  if (0 != res) {
    throw IOException::current_standard_error();
  }
  log("Connected...");
  return socketid;
}

TcpClient::TcpClient(const Text& server, uint16_t port) : PosixFileStream(connect_to_server(server, port)) {}

bool TcpClient::can_read() { return true; }
bool TcpClient::can_write() { return true; }
bool TcpClient::can_seek() { return false; }
bool TcpClient::can_timeout() { return true; }
size_t TcpClient::size() { throw OperationNotSupported("TcpClient::size()", ""); }
size_t TcpClient::position() { throw OperationNotSupported("TcpClient::size()", ""); }

bool TcpClient::data_available() {
  struct pollfd pfd = {.fd = m_fd, .events = POLLIN, .revents = 0};
  return ::poll(&pfd, 1, 0) > 0;
}

bool TcpClient::end_of_stream() { throw OperationNotSupported("TcpClient::end_of_stream()", ""); }
void TcpClient::flush() { throw OperationNotSupported("TcpClient::flush()", ""); }

TimeSpan TcpClient::read_timeout() {
  struct timeval timeout;
  socklen_t length = sizeof(timeout);
  if (getsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, &length) < 0) {
    return {};
  }
  return TimeSpan::from_timeval(timeout);
}

void TcpClient::set_read_timeout(TimeSpan ts) {
  struct timeval timeout = ts.timeval();
  setsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

TimeSpan TcpClient::write_timeout() {
  struct timeval timeout;
  socklen_t length = sizeof(timeout);
  if (getsockopt(m_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, &length) < 0) {
    return {};
  }
  return TimeSpan::from_timeval(timeout);
}

void TcpClient::set_write_timeout(TimeSpan ts) {
  struct timeval timeout = ts.timeval();
  setsockopt(m_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
}

class SSLHandler {
  SSL_CTX* m_ctx;
  SSLHandler() { m_ctx = SSL_CTX_new(SSLv23_method()); }
  ~SSLHandler() {
    SSL_CTX_free(m_ctx);
    m_ctx = nullptr;
  }

  SSL* create_ssl() { return SSL_new(m_ctx); }

public:
  SSLHandler(const SSLHandler&) = delete;
  SSLHandler(SSLHandler&&) = delete;
  SSLHandler& operator=(const SSLHandler&) = delete;
  SSLHandler& operator=(SSLHandler&&) = delete;

  static SSL* create() {
    static SSLHandler libhandler;
    return libhandler.create_ssl();
  }
};

struct SslClient::SslClientImpl {
  TcpClient m_client;
  SSL* m_ssl_handler;
  SslClientImpl(const Text& server, uint16_t port) : m_client(server, port), m_ssl_handler(SSLHandler::create()) {
    SSL_set_fd(m_ssl_handler, m_client.file_descriptor());
    auto res = SSL_connect(m_ssl_handler);
    if (res <= 0) {
      throw IOException("SSL Connect error "_t + std::to_string(SSL_get_error(m_ssl_handler, res)));
    }
  }
  SslClientImpl(const SslClientImpl&) = delete;
  SslClientImpl(SslClientImpl&&) = delete;
  SslClientImpl& operator=(const SslClientImpl&) = delete;
  SslClientImpl& operator=(SslClientImpl&&) = delete;
  ~SslClientImpl() { close(); }

  void close() {
    if (m_ssl_handler != nullptr) {
      SSL_shutdown(m_ssl_handler);
      m_ssl_handler = nullptr;
      m_client.close();
    }
  }

  size_t read(std::span<uint8_t> where) const {
    auto res = SSL_read(m_ssl_handler, where.data(), static_cast<int>(where.size()));
    if (res <= 0) {
      auto err = SSL_get_error(m_ssl_handler, res);
      if (err == SSL_ERROR_ZERO_RETURN) {
        return 0;
      }
      throw IOException("SSL Read error "_t + std::to_string(err));
    }
    return res;
  }

  void write(std::span<uint8_t> what) const {
    auto res = SSL_write(m_ssl_handler, what.data(), static_cast<int>(what.size()));
    if (res <= 0) {
      throw IOException("SSL Write error "_t + std::to_string(SSL_get_error(m_ssl_handler, res)));
    }
  }
};

SslClient::SslClient(const Text& server, uint16_t port)
    : m_impl(std::make_unique<SslClient::SslClientImpl>(server, port)) {}

SslClient::~SslClient() = default;

bool SslClient::can_read() { return true; }
bool SslClient::can_write() { return true; }
bool SslClient::can_seek() { return false; }
bool SslClient::can_timeout() { return true; }
size_t SslClient::read(std::span<uint8_t> where) { return m_impl->read(where); }
void SslClient::write(std::span<uint8_t> what) { m_impl->write(what); }

void SslClient::close() { m_impl->close(); }

TimeSpan SslClient::read_timeout() { return m_impl->m_client.read_timeout(); }
void SslClient::set_read_timeout(TimeSpan ts) { m_impl->m_client.set_read_timeout(ts); }
TimeSpan SslClient::write_timeout() { return m_impl->m_client.write_timeout(); }
void SslClient::set_write_timeout(TimeSpan ts) { m_impl->m_client.set_write_timeout(ts); }
} // namespace kl
