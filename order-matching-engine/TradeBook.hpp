#pragma once
#include <vector>
#include <cstdint>
#include <iostream>

class Trade {
    private:
        double price_;
        double quantity_;
        uint64_t id_;
        uint64_t buyer_id_;
        uint64_t seller_id_;
        uint64_t timestamp_;
    public:
        // constructor
        Trade(double price, double quantity, uint64_t id, uint64_t buyer_id, uint64_t seller_id, uint64_t timestamp)
            : price_(price), quantity_(quantity), id_(id), buyer_id_(buyer_id), seller_id_(seller_id), timestamp_(timestamp) {}
        
        // getters
        double price() const { return price_; }
        double quantity() const { return quantity_; }
        uint64_t id() const { return id_; }
        uint64_t buyer_id() const { return buyer_id_; }
        uint64_t seller_id() const { return seller_id_; }
        uint64_t timestamp() const { return timestamp_; }
        double value() const { return price_ * quantity_; }
};

class TradeBook {
    private:
        std::vector<Trade> trades_;
    public:
        // constructor
        TradeBook() = default;

        // add trade
        void add_trade(Trade trade) {
            trades_.push_back(trade);
            std::cout << "Trade Made: " << "Buyer ID: " << trade.buyer_id() << " Seller ID: " << trade.seller_id() << " Price: " << trade.price() << " Quantity: " << trade.quantity() << std::endl;
        };

        // get trades
        std::vector<Trade> getTrades() const { return trades_; };
};