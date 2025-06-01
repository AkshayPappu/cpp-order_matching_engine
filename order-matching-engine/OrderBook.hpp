#pragma once
#include <queue>
#include <cstdint>
#include <mutex>

enum class OrderType { BUY, SELL };

class Order {
    private:
        OrderType type_;
        double price_;
        double quantity_;
        uint64_t id_;
        uint64_t timestamp_;

    public:
        // constructors
        Order() : type_(OrderType::BUY), price_(0), quantity_(0), id_(0), timestamp_(0) {}
        Order(OrderType type, double price, double quantity, uint64_t id, uint64_t timestamp)
            : type_(type), price_(price), quantity_(quantity), id_(id), timestamp_(timestamp) {}
        
        // getters
        OrderType type() const { return type_; }
        double price() const { return price_; }
        double quantity() const { return quantity_; }
        uint64_t id() const { return id_; }
        double value() const { return price_ * quantity_; }
        uint64_t timestamp() const { return timestamp_; }

        // setters
        void set_price(double price) { price_ = price; }
        void set_quantity(double quantity) { quantity_ = quantity; }
        void set_id(uint64_t id) { id_ = id; }
        void set_timestamp(uint64_t timestamp) { timestamp_ = timestamp; }
        void set_type(OrderType type) { type_ = type; }
};

// comparator for buy orders
struct BuyOrderComparator {
    bool operator()(const Order& a, const Order& b) const {
        return a.price() < b.price();
    }
};

// comparator for sell orders
struct SellOrderComparator {
    bool operator()(const Order& a, const Order& b) const {
        return a.price() > b.price();
    }
};

class OrderBook {
    private:
        std::priority_queue<Order, std::vector<Order>, BuyOrderComparator> buy_orders_;
        std::priority_queue<Order, std::vector<Order>, SellOrderComparator> sell_orders_;
        std::mutex buy_mutex_;
        std::mutex sell_mutex_;

    public:
        // constructor
        OrderBook() = default;

        // add order
        void add_order(Order order) {
            if (order.type() == OrderType::BUY) {
                std::lock_guard<std::mutex> lock(buy_mutex_);
                buy_orders_.push(order);
            } else {
                std::lock_guard<std::mutex> lock(sell_mutex_);
                sell_orders_.push(order);
            }
        };

        // pop top buy order
        Order pop_top_buy_order() {
            std::lock_guard<std::mutex> lock(buy_mutex_);
            if (!buy_orders_.empty()) {
                Order order = buy_orders_.top();
                buy_orders_.pop();
                return order;
            }
            
            return Order(OrderType::BUY, -1, -1, -1, -1);
        };

        // pop top sell order
        Order pop_top_sell_order() {
            std::lock_guard<std::mutex> lock(sell_mutex_);
            if (!sell_orders_.empty()) {
                Order order = sell_orders_.top();
                sell_orders_.pop();
                return order;
            }
            
            return Order(OrderType::SELL, -1, -1, -1, -1);
        };

        // check if has sell order
        bool has_sell_order() { 
            std::lock_guard<std::mutex> lock(sell_mutex_);
            return !sell_orders_.empty(); 
        }

        // check if has buy order
        bool has_buy_order() { 
            std::lock_guard<std::mutex> lock(buy_mutex_);
            return !buy_orders_.empty(); 
        }
};