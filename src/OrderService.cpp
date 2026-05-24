#include "OrderService.h"
#include <functional>
#include <quickfix/Session.h>
#include <quickfix/fix42/NewOrderSingle.h>
#include <quickfix/Fields.h>
#include <quickfix/FieldTypes.h>
#include <iostream>

void OrderService::setSession(const FIX::SessionID& sessionID) {
    sessionId_ = sessionID;
    std::cout << "OrderService session set: " << sessionID << std::endl;
}

bool OrderService::placeOrder(const Order& order) {
    if (!sessionId_) {
        std::cerr << "No FIX session available to place order." << std::endl;
        return false;
    }

    // Validate side
    if (order.side != "BUY" && order.side != "SELL") {
        std::cerr << "Invalid order side. Must be 'BUY' or 'SELL'." << std::endl;
        return false;
    }

    // Validate quantity
    if (order.quantity <= 0) {
        std::cerr << "Invalid order quantity. Must be greater than 0." << std::endl;
        return false;
    }

    const char fixSide = (order.side == "BUY") ? FIX::Side_BUY : FIX::Side_SELL;

    FIX42::NewOrderSingle newOrder(
        FIX::ClOrdID(std::to_string(std::hash<std::string>{}(order.symbol + std::to_string(order.quantity)))),
        FIX::HandlInst('1'),
        FIX::Symbol(order.symbol),
        FIX::Side(fixSide),
        FIX::TransactTime(FIX::UtcTimeStamp()),
        FIX::OrdType(FIX::OrdType_MARKET));

    newOrder.set(FIX::OrderQty(order.quantity));

    bool result = FIX::Session::sendToTarget(newOrder, *sessionId_);
    if (!result) {
        std::cerr << "Failed to send order message." << std::endl;
    }
    return result;
}
