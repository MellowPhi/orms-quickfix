#include "OrderController.h"
#include <iostream>

OrderController::OrderController(OrderService& orderService)
    : orderService_(orderService) {
}

bool OrderController::placeOrder(const Order& order) {
    std::cout << "Controller received order request: " << order.symbol << " x " << order.quantity << std::endl;
    bool success = orderService_.placeOrder(order);
    if (success) {
        std::cout << "Controller forwarded order to service." << std::endl;
    } else {
        std::cout << "Controller could not place order: service unavailable." << std::endl;
    }
    return success;
}
