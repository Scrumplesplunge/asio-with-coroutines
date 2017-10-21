#include <array>
#include <boost/asio.hpp>
#include <iostream>
#include <memory>

using boost::asio::ip::tcp;

class Session {
 public:
  static void Create(tcp::socket socket) {
    auto self = std::make_shared<Session>(std::move(socket));
    Session* session = self.get();
    session->WaitForReceipt(std::move(self));
  }

  Session(tcp::socket socket)
      : socket_(std::move(socket)) {}

 private:
  void WaitForReceipt(std::shared_ptr<Session> self) {
    socket_.async_read_some(
        boost::asio::buffer(buffer_.data(), buffer_.size()),
        [this, self = std::move(self)](
            boost::system::error_code error_code, std::size_t bytes_read) {
          HandleReceipt(std::move(self), error_code, bytes_read);
        });
  }

  void HandleReceipt(std::shared_ptr<Session> self,
                     boost::system::error_code error_code,
                     std::size_t bytes_read) {
    if (error_code) {
      std::cerr << "Error when reading: " << error_code.message() << "\n";
    } else {
      std::cout << "Processing " << bytes_read << " byte(s).\n";
      boost::asio::async_write(
          socket_, boost::asio::buffer(buffer_.data(), bytes_read),
          [this, self = std::move(self)](
              boost::system::error_code error_code, std::size_t bytes_written) {
            HandleSent(std::move(self), error_code, bytes_written);
          });
    }
  }

  void HandleSent(std::shared_ptr<Session> self,
                  boost::system::error_code error_code,
                  std::size_t /* bytes_written */) {
    if (error_code) {
      std::cerr << "Error when writing: " << error_code.message() << "\n";
    } else {
      WaitForReceipt(std::move(self));
    }
  }

  tcp::socket socket_;
  std::array<char, 1024> buffer_;
};

class Server {
 public:
  Server()
      : acceptor_(io_service_, tcp::endpoint(tcp::v4(), 17994)),
        socket_(io_service_) {}

  void Run() {
    WaitForConnection();
    io_service_.run();
  }

 private:
  void WaitForConnection() {
    acceptor_.async_accept(
        socket_,
        [this](boost::system::error_code error_code) {
          HandleAccept(error_code);
        });
  }

  void HandleAccept(boost::system::error_code error_code) {
    if (error_code) {
      std::cerr << "Error when accepting connection: "
                << error_code.message() << "\n";
    } else {
      Session::Create(std::move(socket_));
      WaitForConnection();
    }
  }

  boost::asio::io_service io_service_;
  tcp::acceptor acceptor_;
  tcp::socket socket_;
};

int main() {
  Server server;
  server.Run();
}
