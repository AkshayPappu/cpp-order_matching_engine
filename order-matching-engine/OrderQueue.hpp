#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include "OrderBook.hpp"

class OrderQueue { 
    private:
        std::queue<Order> queue_;
        std::mutex queue_mutex_;
        std::condition_variable queue_cv_;
        bool done_ = false;
    public:
        // constructor
        OrderQueue() = default;

        // add order
        void add(Order order) {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            queue_.push(order);
            queue_cv_.notify_one();
        }

        // pop order
        Order pop() {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            queue_cv_.wait(lock, [this]() { return !queue_.empty() || done_; });
            if (done_ && queue_.empty()) {
                throw std::runtime_error("Queue is empty and done flag is set");
            }
            Order order = queue_.front();
            queue_.pop();
            return order;
        }

        // set done
        void set_done() {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            done_ = true;
            queue_cv_.notify_all();
        }
};