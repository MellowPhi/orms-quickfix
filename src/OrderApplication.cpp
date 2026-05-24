#include <orms/OrderApplication.h>
#include <quickfix/Session.h>
#include <quickfix/Exceptions.h>
#include <iostream>

OrderApplication::OrderApplication(OrderService& orderService)
    : orderService_(orderService) {
}

void OrderApplication::onCreate(const FIX::SessionID& sessionID) {
    std::cout << "Session created: " << sessionID << std::endl;
}

void OrderApplication::onLogon(const FIX::SessionID& sessionID) {
    std::cout << "Logon successful: " << sessionID << std::endl;
    orderService_.setSession(sessionID);
}

void OrderApplication::onLogout(const FIX::SessionID& sessionID) {
    std::cout << "Logout: " << sessionID << std::endl;
}

void OrderApplication::toAdmin(FIX::Message& message, const FIX::SessionID& sessionID) {
    std::cout << "ToAdmin: " << message << std::endl;
}

void OrderApplication::fromAdmin(const FIX::Message& message, const FIX::SessionID& sessionID) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) {
    std::cout << "FromAdmin: " << message << std::endl;
}

void OrderApplication::toApp(FIX::Message& message, const FIX::SessionID& sessionID) throw(FIX::DoNotSend) {
    std::cout << "ToApp: " << message << std::endl;
}

void OrderApplication::fromApp(const FIX::Message& message, const FIX::SessionID& sessionID) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType) {
    crack(message, sessionID);
}

void OrderApplication::onMessage(const FIX42::ExecutionReport& executionReport, const FIX::SessionID& sessionID) {
    try {
        FIX::OrderID orderID;
        FIX::ExecType execType;
        executionReport.get(orderID);
        executionReport.get(execType);
        std::cout << "Received ExecutionReport for order: " << orderID.getValue() << std::endl;
        std::cout << "Execution type: " << execType.getValue() << std::endl;
    } catch (const FIX::FieldNotFound& e) {
        std::cerr << "Missing field in ExecutionReport: " << e.what() << std::endl;
    }
}
