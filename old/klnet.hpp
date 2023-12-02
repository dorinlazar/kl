#pragma once
#include "klio.hpp"
#include "kltime.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

namespace kl {

class TcpClient : public PosixFileStream {
public:
  TcpClient(const Text& server, uint16_t port);
  TcpClient(const TcpClient&) = delete;
  TcpClient(TcpClient&&) = delete;
  TcpClient& operator=(const TcpClient&) = delete;
  TcpClient& operator=(TcpClient&&) = delete;
  ~TcpClient() override = default;

public: // capabilities
  bool can_read() override;
  bool can_write() override;
  bool can_seek() override;
  bool can_timeout() override;

public: // properties
  size_t size() override;
  size_t position() override;

public: // operations
  bool data_available() override;
  bool end_of_stream() override;
  void flush() override;

public:
  TimeSpan read_timeout();
  void set_read_timeout(TimeSpan);
  TimeSpan write_timeout();
  void set_write_timeout(TimeSpan);
};

class SslClient final : public Stream {
  struct SslClientImpl;
  std::unique_ptr<SslClientImpl> m_impl;

public:
  SslClient(const Text& server, uint16_t port);
  SslClient(const SslClient&) = delete;
  SslClient(SslClient&&) = delete;
  SslClient& operator=(const SslClient&) = delete;
  SslClient& operator=(SslClient&&) = delete;
  ~SslClient() override;

public: // capabilities
  bool can_read() override;
  bool can_write() override;
  bool can_seek() override;
  bool can_timeout() override;

public: // operations
  size_t read(std::span<uint8_t> where) override;
  void write(std::span<uint8_t> what) override;

  void close() override;

public:
  TimeSpan read_timeout();
  void set_read_timeout(TimeSpan);
  TimeSpan write_timeout();
  void set_write_timeout(TimeSpan);
};

class TcpServer {
public:
  explicit TcpServer(uint16_t port);
  TcpServer(const TcpServer&) = delete;
  TcpServer(TcpServer&&) = delete;
  TcpServer& operator=(const TcpServer&) = delete;
  TcpServer& operator=(TcpServer&&) = delete;
  ~TcpServer();

  std::unique_ptr<Stream> accept();
};
} // namespace kl
