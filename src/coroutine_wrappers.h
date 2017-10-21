#pragma once

#include <boost/asio.hpp>
#include <experimental/coroutine>

namespace asio {

using boost::asio::ip::tcp;
using boost::system::error_code;

struct IOResult {
  error_code error;
  std::size_t bytes_transferred;
};

namespace internal {

class IOAction {
 public:
  bool await_ready() const { return false; }
  IOResult await_resume() { return std::move(result_); }
 protected:
  struct HandleDone {
    IOAction* io_action;
    std::experimental::coroutine_handle<> handle;
    void operator()(error_code error, std::size_t bytes_transferred);
  };
 private:
  IOResult result_;
};

}  // namespace internal

class Read : public internal::IOAction {
 public:
  Read(tcp::socket& socket, boost::asio::mutable_buffer buffer);
  void await_suspend(std::experimental::coroutine_handle<> handle);
 private:
  tcp::socket& socket_;
  boost::asio::mutable_buffer buffer_;
};

class ReadSome : public internal::IOAction {
 public:
  ReadSome(tcp::socket& socket, boost::asio::mutable_buffer buffer);
  void await_suspend(std::experimental::coroutine_handle<> handle);
 private:
  tcp::socket& socket_;
  boost::asio::mutable_buffer buffer_;
};

class Write : public internal::IOAction {
 public:
  Write(tcp::socket& socket, boost::asio::const_buffer buffer);
  void await_suspend(std::experimental::coroutine_handle<> handle);
 private:
  tcp::socket& socket_;
  boost::asio::const_buffer buffer_;
};

class WriteSome : public internal::IOAction {
 public:
  WriteSome(tcp::socket& socket, boost::asio::const_buffer buffer);
  void await_suspend(std::experimental::coroutine_handle<> handle);
 private:
  tcp::socket& socket_;
  boost::asio::const_buffer buffer_;
};

struct ConnectionResult {
  error_code error;
  tcp::socket socket;
};

class Accept {
 public:
  explicit Accept(tcp::acceptor& acceptor);
  bool await_ready() const { return false; }
  void await_suspend(std::experimental::coroutine_handle<> handle);
  ConnectionResult await_resume() { return std::move(result_); }
 private:
  tcp::acceptor& acceptor_;
  ConnectionResult result_;
};

struct Task {
  struct promise_type;
  using handle = std::experimental::coroutine_handle<promise_type>;
  struct promise_type {
    static auto get_return_object_on_allocation_failure() { return Task{}; }
    auto get_return_object() { return Task{}; }
    auto initial_suspend() { return std::experimental::suspend_never{}; }
    auto final_suspend() { return std::experimental::suspend_never{}; }
    void unhandled_exception() { std::terminate(); }
    void return_void() {}
    auto yield_value(int /* value */) {
      return std::experimental::suspend_never{};
    }
  };
};

}  // namespace asio
