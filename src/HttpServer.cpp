#include "HttpServer.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <thread>

HttpServer::HttpServer(OrderController& controller)
    : controller_(controller), server_(std::make_unique<httplib::Server>()) {
}

HttpServer::~HttpServer() {
    stop();
}

bool HttpServer::start(unsigned short port) {
    if (running_) {
        return false;
    }

    // Register POST /order endpoint
    server_->Post("/order", [this](const httplib::Request& req, httplib::Response& res) {
        // Check content type
        if (!req.has_header("Content-Type") ||
            req.get_header_value("Content-Type").find("application/json") == std::string::npos) {
            res.set_content("{\"error\":\"unsupported content type\"}", "application/json");
            res.status = 415;
            return;
        }

        // Parse JSON body
        auto orderOpt = parseOrderJson(req.body);
        if (!orderOpt.has_value()) {
            res.set_content("{\"error\":\"invalid JSON body\"}", "application/json");
            res.status = 400;
            return;
        }

        // Place order and build response
        const Order order = orderOpt.value();
        bool success = controller_.placeOrder(order);

        std::ostringstream responseBody;
        responseBody << "{\"status\":\"" << (success ? "accepted" : "failed") << "\",";
        responseBody << "\"symbol\":\"" << order.symbol << "\",";
        responseBody << "\"quantity\":" << order.quantity << "}";

        res.set_content(responseBody.str(), "application/json");
        res.status = success ? 200 : 500;
    });

    // Register catch-all for 404
    server_->set_default_headers({{"Server", "ORMS-CPP/1.0"}});

    running_ = true;
    // Run server in background thread
    serverThread_ = std::thread([this, port]() {
        server_->listen("0.0.0.0", port);
    });

    // Give server a moment to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return running_;
}

void HttpServer::stop() {
    if (!running_) {
        return;
    }
    running_ = false;
    if (server_) {
        server_->stop();
    }
    if (serverThread_.joinable()) {
        serverThread_.join();
    }
}

bool HttpServer::isRunning() const {
    return running_;
}

Optional<Order> HttpServer::parseOrderJson(const std::string& body) {
    auto parseString = [&](const std::string& field) -> Optional<std::string> {
        const std::string quoted = '"' + field + '"';
        auto pos = body.find(quoted);
        if (pos == std::string::npos) {
            return Optional<std::string>();
        }
        pos = body.find(':', pos + quoted.size());
        if (pos == std::string::npos) {
            return Optional<std::string>();
        }
        pos = body.find('"', pos + 1);
        if (pos == std::string::npos) {
            return Optional<std::string>();
        }
        auto end = body.find('"', pos + 1);
        if (end == std::string::npos) {
            return Optional<std::string>();
        }
        return Optional<std::string>(body.substr(pos + 1, end - pos - 1));
    };

    auto parseInt = [&](const std::string& field) -> Optional<int> {
        const std::string quoted = '"' + field + '"';
        auto pos = body.find(quoted);
        if (pos == std::string::npos) {
            return Optional<int>();
        }
        pos = body.find(':', pos + quoted.size());
        if (pos == std::string::npos) {
            return Optional<int>();
        }
        auto numStart = pos + 1;
        while (numStart < body.size() && std::isspace(static_cast<unsigned char>(body[numStart]))) {
            numStart++;
        }
        auto numEnd = numStart;
        while (numEnd < body.size() && (std::isdigit(static_cast<unsigned char>(body[numEnd])) || body[numEnd] == '-')) {
            numEnd++;
        }
        if (numStart == numEnd) {
            return Optional<int>();
        }
        try {
            return Optional<int>(std::stoi(body.substr(numStart, numEnd - numStart)));
        } catch (...) {
            return Optional<int>();
        }
    };

    auto symbol = parseString("symbol");
    auto quantity = parseInt("quantity");
    if (!symbol.has_value() || !quantity.has_value()) {
        return Optional<Order>();
    }
    return Optional<Order>(Order{symbol.value(), quantity.value()});
}
