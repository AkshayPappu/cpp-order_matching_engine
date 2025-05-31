#include "OrderReader.hpp"
#include "OrderBook.hpp"
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <iostream>
#include <stdexcept>
#include <regex>

class OrderReaderException : public std::runtime_error {
public:
    explicit OrderReaderException(const std::string& msg) : std::runtime_error(msg) {}
};

OrderReader::OrderReader(const std::string& host, int port)
    : host_(host), port_(port), socket_(io_context_), connected_(false) {
    if (port <= 0 || port > 65535) {
        throw OrderReaderException("Invalid port number");
    }
}

void OrderReader::connect() {
    try {
        if (connected_) {
            throw OrderReaderException("Already connected");
        }

        boost::asio::ip::tcp::resolver resolver(io_context_);
        auto endpoints = resolver.resolve(host_, std::to_string(port_));
        
        boost::system::error_code ec;
        boost::asio::connect(socket_, endpoints, ec);
        
        if (ec) {
            throw OrderReaderException("Connection failed: " + ec.message());
        }
        
        connected_ = true;
    } catch (const boost::system::system_error& e) {
        throw OrderReaderException("Connection error: " + std::string(e.what()));
    }
}

Order OrderReader::read_next_order() {
    if (!connected_) {
        throw OrderReaderException("Not connected");
    }

    try {
        // create buffer
        boost::asio::streambuf buffer;
        boost::system::error_code ec;
        
        // Set up timeout
        boost::asio::steady_timer timeout(io_context_, std::chrono::seconds(5));
        timeout.async_wait([this](const boost::system::error_code& error) {
            if (!error) {
                socket_.cancel();
            }
        });
            
        size_t bytes_read = boost::asio::read_until(socket_, buffer, '\n', ec);
        timeout.cancel();
        
        if (ec) {
            if (ec == boost::asio::error::operation_aborted) {
                throw OrderReaderException("Read operation timed out");
            }
            if (ec == boost::asio::error::eof) {
                throw OrderReaderException("Connection closed by peer");
            }
            throw OrderReaderException("Read error: " + ec.message());
        }
        
        if (bytes_read == 0) {
            throw OrderReaderException("Empty message received");
        }

        // parse the line
        std::istream is(&buffer);
        std::string line;
        std::getline(is, line);

        // Validate line format using regex
        static const std::regex order_pattern(R"(^(BUY|SELL),(\d+\.?\d*),(\d+\.?\d*),(\d+),(\d+)$)");
        std::smatch matches;
        
        if (!std::regex_match(line, matches, order_pattern)) {
            throw OrderReaderException("Invalid order format: " + line);
        }

        // Parse values with validation
        OrderType type = (matches[1] == "BUY") ? OrderType::BUY : OrderType::SELL;
        double price = std::stod(matches[2]);
        double quantity = std::stod(matches[3]);
        uint64_t id = std::stoull(matches[4]);
        uint64_t timestamp = std::stoull(matches[5]);

        // Validate business rules
        if (price <= 0) {
            throw OrderReaderException("Invalid price: must be positive");
        }
        if (quantity <= 0) {
            throw OrderReaderException("Invalid quantity: must be positive");
        }
        if (id == 0) {
            throw OrderReaderException("Invalid order ID: cannot be zero");
        }

        return Order(type, price, quantity, id, timestamp);
    } catch (const std::exception& e) {
        throw OrderReaderException("Error reading order: " + std::string(e.what()));
    }
}

void OrderReader::disconnect() {
    if (!connected_) {
        return;
    }

    try {
        boost::system::error_code ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        if (ec) {
            std::cerr << "Warning: Error during socket shutdown: " << ec.message() << std::endl;
        }
        
        socket_.close(ec);
        if (ec) {
            std::cerr << "Warning: Error closing socket: " << ec.message() << std::endl;
        }
        
        connected_ = false;
    } catch (const std::exception& e) {
        std::cerr << "Error during disconnect: " << e.what() << std::endl;
        connected_ = false;
    }
}

OrderReader::~OrderReader() {
    disconnect();
}
