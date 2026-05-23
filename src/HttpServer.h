#pragma once

#include "OrderController.h"
#include "Optional.h"
#include "httplib.h"
#include <memory>
#include <thread>

class HttpServer {
public:
    explicit HttpServer(OrderController& controller);
    ~HttpServer();

    bool start(unsigned short port);
    void stop();
    bool isRunning() const;

private:
    Optional<Order> parseOrderJson(const std::string& body);

    OrderController& controller_;
    std::unique_ptr<httplib::Server> server_;
    std::thread serverThread_;
    bool running_{false};
};
