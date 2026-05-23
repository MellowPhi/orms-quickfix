#pragma once

#include "Order.h"
#include "OrderService.h"

class OrderController {
public:
    explicit OrderController(OrderService& orderService);
    bool placeOrder(const Order& order);

private:
    OrderService& orderService_;
};
