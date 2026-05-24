#include <orms/HttpServer.h>

#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

HttpServer::HttpServer(OrderController& controller, int port)
    : controller_(controller)
    , port_(port)
{
    registerRoutes();
}

void HttpServer::start()
{
    std::cout << "[HTTP] Listening on http://0.0.0.0:" << port_ << std::endl;
    server_.listen("0.0.0.0", port_);
}

void HttpServer::stop()
{
    server_.stop();
}

void HttpServer::registerRoutes()
{
    // POST /order
    server_.Post("/order", [this](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Content-Type", "application/json");

        json body;
        try {
            body = json::parse(req.body);
        } catch (const json::parse_error& e) {
            res.status = 400;
            res.body = json{{"error", std::string("Invalid JSON: ") + e.what()}}.dump();
            return;
        }

        // Accept either "symbol" or "ticker" for compatibility
        if (!(body.contains("symbol") || body.contains("ticker")) || !body.contains("quantity") || !body.contains("side")) {
            res.status = 400;
            res.body = json{{"error", "Missing required fields: symbol|ticker, quantity, side"}}.dump();
            return;
        }

        std::string symbol;
        try {
            if (body.contains("symbol")) symbol = body["symbol"].get<std::string>();
            else symbol = body["ticker"].get<std::string>();
        } catch (const json::exception& e) {
            res.status = 400;
            res.body = json{{"error", std::string("Type error: ") + e.what()}}.dump();
            return;
        }

        int quantity;
        std::string side;
        try {
            quantity = body["quantity"].get<int>();
            side = body["side"].get<std::string>();
        } catch (const json::exception& e) {
            res.status = 400;
            res.body = json{{"error", std::string("Type error: ") + e.what()}}.dump();
            return;
        }

        // Normalize side (case-insensitive)
        std::transform(side.begin(), side.end(), side.begin(), [](unsigned char c){ return static_cast<char>(std::toupper(c)); });
        if (side == "B") side = "BUY";
        if (side == "S") side = "SELL";
        if (side != "BUY" && side != "SELL") {
            res.status = 400;
            res.body = json{{"error", "Invalid side; must be BUY or SELL"}}.dump();
            return;
        }

        Order order{symbol, quantity, side};
        bool ok = controller_.placeOrder(order);
        if (!ok) {
            res.status = 503;
            res.body = json{{"error", "FIX session not logged on or failed to send"}}.dump();
            return;
        }

        res.status = 200;
        res.body = json{{"status", "accepted"}, {"symbol", symbol}, {"quantity", quantity}, {"side", side}}.dump();
    });

    // GET /health
    server_.Get("/health", [](const httplib::Request&, httplib::Response& res){
        res.set_header("Content-Type", "application/json");
        res.status = 200;
        res.body = json{{"status", "ok"}}.dump();
    });
}
