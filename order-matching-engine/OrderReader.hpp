#pragma once
#include <string>
#include "OrderBook.hpp"
#include <boost/asio.hpp>

class OrderReader {
    private:
        std::string host_;
        int port_;
        boost::asio::io_context io_context_;
        boost::asio::ip::tcp::socket socket_;
        bool connected_;
    public:
        // constructor
        OrderReader(const std::string& host, int port);

        // destructor
        ~OrderReader();

        // connect to server
        void connect();

        // read next order
        Order read_next_order();

        // close connection
        void disconnect();
};