#include "coroutine_wrappers.h"

namespace asio {
namespace internal {

void IOAction::HandleDone::operator()(
    error_code error, std::size_t bytes_transferred) {
  io_action->result_ = IOResult{error, bytes_transferred};
  handle.resume();
}

}  // namespace internal

Read::Read(tcp::socket& socket, boost::asio::mutable_buffer buffer)
    : socket_(socket), buffer_(buffer) {}

void Read::await_suspend(std::experimental::coroutine_handle<> handle) {
  boost::asio::async_read(
      socket_, boost::asio::buffer(buffer_), HandleDone{this, handle});
}

ReadSome::ReadSome(tcp::socket& socket, boost::asio::mutable_buffer buffer)
    : socket_(socket), buffer_(buffer) {}

void ReadSome::await_suspend(std::experimental::coroutine_handle<> handle) {
  socket_.async_read_some(
      boost::asio::buffer(buffer_), HandleDone{this, handle});
}

Write::Write(tcp::socket& socket, boost::asio::const_buffer buffer)
    : socket_(socket), buffer_(buffer) {}

void Write::await_suspend(std::experimental::coroutine_handle<> handle) {
  boost::asio::async_write(
      socket_, boost::asio::buffer(buffer_), HandleDone{this, handle});
}

WriteSome::WriteSome(tcp::socket& socket, boost::asio::const_buffer buffer)
    : socket_(socket), buffer_(buffer) {}

void WriteSome::await_suspend(std::experimental::coroutine_handle<> handle) {
  socket_.async_write_some(
      boost::asio::buffer(buffer_), HandleDone{this, handle});
}

Accept::Accept(tcp::acceptor& acceptor)
    : acceptor_(acceptor),
      result_{{}, tcp::socket{acceptor.get_io_service()}} {}

void Accept::await_suspend(std::experimental::coroutine_handle<> handle) {
  acceptor_.async_accept(
      result_.socket,
      [this, handle](error_code error) mutable {
        result_.error = error;
        handle.resume();
      });
}

}  // namespace asio
