#pragma once

#include <orms/OrderController.h>
#include <httplib.h>
#include <nlohmann/json.hpp>

class HttpServer {
public:
    explicit HttpServer(OrderController& controller, int port = 8080);

    // Blocks the calling thread (call from a std::thread to run in background)
    void start();
    void stop();

private:
    void registerRoutes();

    OrderController& controller_;
    int port_;
    httplib::Server server_;
};
