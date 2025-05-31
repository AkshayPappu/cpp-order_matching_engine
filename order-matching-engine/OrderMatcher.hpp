#pragma once
#include <chrono>
#include "OrderBook.hpp"
#include "TradeBook.hpp"
#include "IdGenerator.hpp"

class OrderMatcher {
    private:
        OrderBook order_book_;
        TradeBook trade_book_;
        RandomIdGenerator id_generator_;
    public:
        // constructor
        OrderMatcher() = default;

        // match order
        void match_order(Order order) {
            // store current time
            uint64_t current_time = std::chrono::system_clock::now().time_since_epoch().count();

            if (order.type() == OrderType::BUY) {
                while (order_book_.has_sell_order()) {
                    Order sell_order = order_book_.pop_top_sell_order();

                    // check if sell order price is greater than buy order price
                    if (sell_order.price() > order.price()) {
                        order_book_.add_order(sell_order);
                        break;
                    }

                    double max_match_quantity = std::min(order.quantity(), sell_order.quantity());

                    // record trade
                    trade_book_.add_trade(Trade(sell_order.price() * max_match_quantity, max_match_quantity, id_generator_.generate(), order.id(), sell_order.id(), current_time));

                    // update both order quantities
                    order.set_quantity(order.quantity() - max_match_quantity);
                    sell_order.set_quantity(sell_order.quantity() - max_match_quantity);

                    // handle exit cases
                    if (sell_order.quantity() > 0) {
                        order_book_.add_order(sell_order);
                        break;
                    }

                    if (order.quantity() == 0) {
                        break;
                    }
                }

                // if quantity remaining, add buy order to order book
                if (order.quantity() > 0) {
                    order_book_.add_order(order);
                }
            } else {
                while (order_book_.has_buy_order()) {
                    Order buy_order = order_book_.pop_top_buy_order();


                    // check if buy order price is less than sell order price
                    if (buy_order.price() < order.price()) {
                        order_book_.add_order(buy_order);
                        break;
                    }

                    double max_match_quantity = std::min(order.quantity(), buy_order.quantity());

                    // record trade
                    trade_book_.add_trade(Trade(buy_order.price() * max_match_quantity, max_match_quantity, id_generator_.generate(), buy_order.id(), order.id(), current_time));

                    // update both order quantities
                    order.set_quantity(order.quantity() - max_match_quantity);
                    buy_order.set_quantity(buy_order.quantity() - max_match_quantity);
                    
                    // handle exit cases
                    if (buy_order.quantity() > 0) {
                        order_book_.add_order(buy_order);
                        break;
                    }

                    if (order.quantity() == 0) {
                        break;
                    }
                }

                // if quantity remaining, add sell order to order book
                if (order.quantity() > 0) {
                    order_book_.add_order(order);
                }
            }
        }

};