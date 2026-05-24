#pragma once

#include <orms/Order.h>
#include <orms/OrderService.h>

class OrderController {
public:
    explicit OrderController(OrderService& orderService);
    bool placeOrder(const Order& order);

private:
    OrderService& orderService_;
};
