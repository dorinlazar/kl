#pragma once

#include <cstddef>
#include <vector>
#include <memory>
#include <span>
#include "kltext.hpp"
#include "klexcept.hpp"

namespace kl {

// Inspired by System.IO.Stream: https://docs.microsoft.com/en-us/dotnet/api/system.io.stream?view=net-5.0
class Stream {
public:
  Stream() = default;
  Stream(const Stream&) = delete;
  Stream(Stream&&) = delete;
  Stream& operator=(const Stream&) = delete;
  Stream& operator=(Stream&&) = delete;
  virtual ~Stream() = default;

public: // capabilities
  virtual bool can_read();
  virtual bool can_write();
  virtual bool can_seek();
  virtual bool can_timeout();

public: // properties
  virtual size_t size();
  virtual size_t position();

public: // operations
  virtual size_t read(std::span<uint8_t> where);
  virtual void write(std::span<uint8_t> what);

  virtual void seek(size_t offset);
  virtual bool data_available();
  virtual bool end_of_stream();
  virtual void flush();

  virtual void close();
};

class StreamReader {
  size_t m_offset = 0, m_read_size = 0;
  constexpr static size_t ReaderBufferSize = 4096;
  std::array<uint8_t, ReaderBufferSize> m_buffer;
  Stream* m_stream;

public:
  explicit StreamReader(Stream* stream);
  Stream* stream() const;
  size_t read(std::span<uint8_t> where);
  Text read_line();
  Text read_all();
  bool end_of_stream();
};

class StreamWriter {
  Stream* m_stream;

public:
  explicit StreamWriter(Stream* stream);
  Stream* stream() const;
  void write(std::span<uint8_t> what);
  void write(const Text& what);
  void write_line(const Text& what);
  void write(const TextChain& what);
  void flush();
};

enum class FileOpenMode { ReadOnly, WriteOnly, ReadWrite, AppendRW, TruncateRW };

class PosixFileStream : public Stream {
protected:
  int m_fd = -1;
  bool m_regular = false;

public:
  explicit PosixFileStream(int fd);
  PosixFileStream(const PosixFileStream&) = delete;
  PosixFileStream(PosixFileStream&&) = delete;
  PosixFileStream& operator=(const PosixFileStream&) = delete;
  PosixFileStream& operator=(PosixFileStream&&) = delete;
  ~PosixFileStream() override;

public: // properties
  size_t size() override;
  size_t position() override;

public: // operations
  size_t read(std::span<uint8_t> where) override;
  void write(std::span<uint8_t> what) override;

  void seek(size_t offset) override;
  bool data_available() override;
  void flush() override;
  bool end_of_stream() override;

  void close() override;
  int file_descriptor() const;
};

class FileStream final : public PosixFileStream {
  FileOpenMode m_mode;

public:
  FileStream(const Text& filename, FileOpenMode mode);
  FileStream(const FileStream&) = delete;
  FileStream(FileStream&&) = delete;
  FileStream& operator=(const FileStream&) = delete;
  FileStream& operator=(FileStream&&) = delete;
  ~FileStream() override = default;

public: // capabilities
  bool can_read() override;
  bool can_write() override;
  bool can_seek() override;
};

} // namespace kl
