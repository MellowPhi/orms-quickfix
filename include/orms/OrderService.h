#pragma once

#include <orms/Order.h>
#include <orms/Optional.h>
#include <quickfix/SessionID.h>

class OrderService {
public:
    void setSession(const FIX::SessionID& sessionID);
    bool placeOrder(const Order& order);

private:
    Optional<FIX::SessionID> sessionId_;
};
