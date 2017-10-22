#include "coroutine_wrappers.h"

#include <array>
#include <iostream>

Task RunEchoSession(tcp::socket socket) {
  std::array<char, 1024> buffer;
  while (true) {
    auto result = co_await ReadSome(socket, boost::asio::buffer(buffer));
    if (!result) {
      std::cerr << "Error when reading: " << result.Error().message() << "\n";
      break;
    }
    std::cout << "Processing " << result.Get() << " byte(s).\n";
    result = co_await WriteSome(
        socket, boost::asio::buffer(buffer.data(), result.Get()));
    if (!result) {
      std::cerr << "Error when writing: " << result.Error().message() << "\n";
      break;
    }
  }
}

Task AcceptConnections(tcp::acceptor acceptor) {
  while (true) {
    Result<tcp::socket> ressult = co_await Accept(acceptor);
    if (result) {
      RunEchoSession(std::move(result.Get()));
    } else {
      std::cerr << "Error accepting connection: " << result.Error().message()
                << "\n";
      break;
    }
  }
}

int main() {
  boost::asio::io_service io_service;
  tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 17994));
  AcceptConnections(std::move(acceptor));
  io_service.run();
}
