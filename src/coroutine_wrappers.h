#pragma once

#include <boost/asio.hpp>
#include <experimental/coroutine>
#include <type_traits>
#include <variant>

using boost::asio::ip::tcp;
using boost::system::error_code;

// Wraps a result so that it can fail with a boost::system::error_code instead.
// Example usage:
//   Result<int> result = Foo();
//   if (result) {
//     DoSomethingWith(result.Get());
//   } else {
//     std::cerr << "An error occurred: " << result.Error().message() << "\n";
//   }
template <typename T,
          typename = std::enable_if_t<!std::is_same_v<T, error_code>>>
class Result {
 public:
  Result() = default;
  Result(error_code error) : contents_(error) {}
  Result(T value) : contents_(std::move(value)) {}
  operator bool() const { return std::holds_alternative<T>(contents_); }
  T& Get() { return std::get<T>(contents_); }
  error_code Error() {
    if (auto* error = std::get_if<error_code>(&contents_)) {
      return *error;
    } else {
      return {};
    }
  }

 private:
  std::variant<error_code, T> contents_;
};

namespace internal {

class IOAction {
 public:
  bool await_ready() const { return false; }
  Result<std::size_t> await_resume() { return std::move(result_); }

 protected:
  struct HandleDone {
    IOAction* io_action;
    std::experimental::coroutine_handle<> handle;
    void operator()(error_code error, std::size_t bytes_transferred);
  };

 private:
  Result<std::size_t> result_;
};

}  // namespace internal

class ReadSome : public internal::IOAction {
 public:
  ReadSome(tcp::socket& socket, boost::asio::mutable_buffer buffer);
  void await_suspend(std::experimental::coroutine_handle<> handle);

 private:
  tcp::socket& socket_;
  boost::asio::mutable_buffer buffer_;
};

class WriteSome : public internal::IOAction {
 public:
  WriteSome(tcp::socket& socket, boost::asio::const_buffer buffer);
  void await_suspend(std::experimental::coroutine_handle<> handle);

 private:
  tcp::socket& socket_;
  boost::asio::const_buffer buffer_;
};

class Accept {
 public:
  explicit Accept(tcp::acceptor& acceptor);
  bool await_ready() const { return false; }
  void await_suspend(std::experimental::coroutine_handle<> handle);
  Result<tcp::socket> await_resume() { return std::move(result_); }

 private:
  tcp::acceptor& acceptor_;
  tcp::socket socket_;
  Result<tcp::socket> result_;
};

struct Task {
  struct promise_type {
    auto get_return_object() { return Task{}; }
    auto initial_suspend() { return std::experimental::suspend_never{}; }
    auto final_suspend() { return std::experimental::suspend_never{}; }
    void unhandled_exception() { std::terminate(); }
    void return_void() {}
  };
};
