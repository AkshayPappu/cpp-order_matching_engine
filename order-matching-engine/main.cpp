#include "OrderReader.hpp"
#include "OrderMatcher.hpp"
#include <iostream>

int main() {
    std::string host = "127.0.0.1";  // match your test server
    int port = 9000;

    try {
        // Create components
        OrderReader reader(host, port);
        OrderMatcher matcher;

        // Connect to order feed
        std::cout << "Connecting to order feed at " << host << ":" << port << "..." << std::endl;
        reader.connect();
        std::cout << "Connected!" << std::endl;

        while (true) {
            try {
                // Read next order from socket
                Order order = reader.read_next_order();

                // Log received order
                std::cout << "Received Order → "
                          << ((order.type() == OrderType::BUY) ? "BUY" : "SELL")
                          << " | Price: " << order.price()
                          << " | Qty: " << order.quantity()
                          << " | ID: " << order.id()
                          << " | Timestamp: " << order.timestamp()
                          << std::endl;

                // Pass to matcher
                matcher.match_order(order);

                // Optional: print trades or order book state here
                // e.g., matcher.print_summary();

            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                break;
            }
        }

        reader.disconnect();
        std::cout << "Disconnected from order feed." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}