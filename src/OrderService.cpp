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

    FIX42::NewOrderSingle newOrder(
        FIX::ClOrdID(std::to_string(std::hash<std::string>{}(order.symbol + std::to_string(order.quantity)))),
        FIX::HandlInst('1'),
        FIX::Symbol(order.symbol),
        FIX::Side(FIX::Side_BUY),
        FIX::TransactTime(FIX::UtcTimeStamp()),
        FIX::OrdType(FIX::OrdType_MARKET));

    newOrder.set(FIX::OrderQty(order.quantity));

    bool result = FIX::Session::sendToTarget(newOrder, *sessionId_);
    if (!result) {
        std::cerr << "Failed to send order message." << std::endl;
    }
    return result;
}
