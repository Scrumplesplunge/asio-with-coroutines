#include "coroutine_wrappers.h"

namespace internal {

void IOAction::HandleDone::operator()(error_code error,
                                      std::size_t bytes_transferred) {
  if (error) {
    io_action->result_ = error;
  } else {
    io_action->result_ = bytes_transferred;
  }
  handle.resume();
}

}  // namespace internal

ReadSome::ReadSome(tcp::socket& socket, boost::asio::mutable_buffer buffer)
    : socket_(socket), buffer_(buffer) {}

void ReadSome::await_suspend(std::experimental::coroutine_handle<> handle) {
  socket_.async_read_some(boost::asio::buffer(buffer_),
                          HandleDone{this, handle});
}

WriteSome::WriteSome(tcp::socket& socket, boost::asio::const_buffer buffer)
    : socket_(socket), buffer_(buffer) {}

void WriteSome::await_suspend(std::experimental::coroutine_handle<> handle) {
  socket_.async_write_some(boost::asio::buffer(buffer_),
                           HandleDone{this, handle});
}

Accept::Accept(tcp::acceptor& acceptor)
    : acceptor_(acceptor), socket_(acceptor.get_io_service()) {}

void Accept::await_suspend(std::experimental::coroutine_handle<> handle) {
  acceptor_.async_accept(socket_, [this, handle](error_code error) mutable {
    if (error) {
      result_ = error;
    } else {
      result_ = std::move(socket_);
    }
    handle.resume();
  });
}
