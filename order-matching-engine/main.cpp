#include "OrderReader.hpp"
#include "OrderMatcher.hpp"
#include "OrderQueue.hpp"
#include <iostream>
#include <thread>
#include <atomic>

// Global flag for graceful shutdown
std::atomic<bool> should_stop(false);
std::mutex print_mutex;

// Worker thread function that processes orders from the queue
void process_orders(OrderQueue& queue, OrderMatcher& matcher) {
    while (!should_stop) {
        try {
            Order order = queue.pop();
            
            // Log processing order
            {  
                std::lock_guard<std::mutex> lock(print_mutex);
                std::cout << "Processing Order → " << "(Thread ID: " << std::this_thread::get_id() << ") "
                        << ((order.type() == OrderType::BUY) ? "BUY" : "SELL")
                        << " | Price: " << order.price()
                        << " | Qty: " << order.quantity()
                        << " | ID: " << order.id()
                        << " | Timestamp: " << order.timestamp()
                        << std::endl;
            }

            // Match the order
            matcher.match_order(order);
            
            // Optional: print trades or order book state here
            // e.g., matcher.print_summary();
            
        } catch (const std::exception& e) {
            if (!should_stop) {  // Only log errors if we're not shutting down
                std::lock_guard<std::mutex> lock(print_mutex);
                std::cerr << "Error processing order: " << e.what() << std::endl;
            }
        }
    }
}

int main() {
    std::string host = "127.0.0.1";  // match your test server
    int port = 9000;

    try {
        // Create components
        OrderReader reader(host, port);
        OrderMatcher matcher;
        OrderQueue queue;

        // Start the worker thread
        int num_threads = 4;
        std::vector<std::thread> workers;
        for (int i = 0; i < num_threads; ++i) {
            workers.emplace_back(process_orders, std::ref(queue), std::ref(matcher));
        }

        // Connect to order feed
        std::cout << "Connecting to order feed at " << host << ":" << port << "..." << std::endl;
        reader.connect();
        std::cout << "Connected!" << std::endl;

        while (!should_stop) {
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

                // Add to queue for processing
                queue.add(order);

            } catch (const std::exception& e) {
                std::cerr << "Error reading order: " << e.what() << std::endl;
                break;
            }
        }

        // Signal worker thread to stop
        should_stop = true;
        queue.set_done();
        
        // Wait for worker thread to finish
        for (auto& worker : workers) {
            worker.join();
        }

        reader.disconnect();
        std::cout << "Disconnected from order feed." << std::endl;

    } catch (const std::exception& e) {
        should_stop = true;  // Ensure worker thread stops on fatal error
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}