#pragma once

#include "OrderController.h"
#include "Optional.h"
#include <atomic>
#include <string>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
using socket_t = SOCKET;
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
using socket_t = int;
static constexpr socket_t INVALID_SOCKET = -1;
#endif

class HttpServer {
public:
    explicit HttpServer(OrderController& controller);
    ~HttpServer();

    bool start(unsigned short port);
    void stop();
    bool isRunning() const;

private:
    void acceptLoop();
    void handleClient(socket_t clientSocket);
    bool readRequest(socket_t clientSocket, std::string& request);
    bool sendResponse(socket_t clientSocket, int status, const std::string& body,
                      const std::string& contentType = "application/json");
    Optional<Order> parseOrderJson(const std::string& body);
    std::string toLower(std::string value) const;

    OrderController& controller_;
    std::atomic<bool> running_{false};
    std::thread acceptThread_;
    socket_t listenSocket_{INVALID_SOCKET};
};
