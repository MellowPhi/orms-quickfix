#pragma once

#include "OrderService.h"
#include <quickfix/Application.h>
#include <quickfix/MessageCracker.h>
#include <quickfix/fix42/ExecutionReport.h>

class OrderApplication : public FIX::Application, public FIX::MessageCracker {
public:
    explicit OrderApplication(OrderService& orderService);

    void onCreate(const FIX::SessionID& sessionID) override;
    void onLogon(const FIX::SessionID& sessionID) override;
    void onLogout(const FIX::SessionID& sessionID) override;
    void toAdmin(FIX::Message& message, const FIX::SessionID& sessionID) override;
    void fromAdmin(const FIX::Message& message, const FIX::SessionID& sessionID) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) override;
    void toApp(FIX::Message& message, const FIX::SessionID& sessionID) throw(FIX::DoNotSend) override;
    void fromApp(const FIX::Message& message, const FIX::SessionID& sessionID) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType) override;

    void onMessage(const FIX42::ExecutionReport& executionReport, const FIX::SessionID& sessionID);

private:
    OrderService& orderService_;
};
