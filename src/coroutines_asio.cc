#include "coroutine_wrappers.h"

#include <array>
#include <iostream>

asio::Task Echo(asio::tcp::socket socket) {
  std::array<char, 1024> buffer;
  while (true) {
    asio::IOResult result;
    result = co_await asio::ReadSome(socket, boost::asio::buffer(buffer));
    if (result.error) {
      std::cerr << "Error when reading: " << result.error.message() << "\n";
      break;
    }
    std::cout << "Processing " << result.bytes_transferred << " byte(s).\n";
    result = co_await asio::WriteSome(
        socket, boost::asio::buffer(buffer.data(), result.bytes_transferred));
    if (result.error) {
      std::cerr << "Error when writing: " << result.error.message() << "\n";
      break;
    }
  }
}

asio::Task Serve(asio::tcp::acceptor acceptor) {
  for (asio::ConnectionResult result = co_await asio::Accept(acceptor);
       !result.error;
       result = co_await asio::Accept(acceptor)) {
    std::cout << "Accepted connection from " << result.socket.remote_endpoint()
              << ".\n";
    Echo(std::move(result.socket));
  }
}

int main() {
  boost::asio::io_service io_service;
  asio::tcp::acceptor acceptor(
      io_service, asio::tcp::endpoint(asio::tcp::v4(), 17994));
  Serve(std::move(acceptor));
  io_service.run();
}
