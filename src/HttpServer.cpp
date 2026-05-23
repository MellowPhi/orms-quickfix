#include "HttpServer.h"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <iostream>
#include <optional>
#include <sstream>
#include <string_view>
#include <vector>

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

namespace {
#ifdef _WIN32
bool initializeSockets() {
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}
void cleanupSockets() {
    WSACleanup();
}
void closeSocket(socket_t sock) {
    closesocket(sock);
}
#else
bool initializeSockets() {
    return true;
}
void cleanupSockets() {
}
void closeSocket(socket_t sock) {
    close(sock);
}
#endif

std::string trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return {};
    }
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string toLowerCopy(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}
} // namespace

HttpServer::HttpServer(OrderController& controller)
    : controller_(controller) {
}

HttpServer::~HttpServer() {
    stop();
}

bool HttpServer::start(unsigned short port) {
    if (running_) {
        return false;
    }

    if (!initializeSockets()) {
        std::cerr << "Failed to initialize sockets." << std::endl;
        return false;
    }

    listenSocket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket_ == INVALID_SOCKET) {
        std::cerr << "Failed to create listen socket." << std::endl;
        cleanupSockets();
        return false;
    }

    int yes = 1;
    setsockopt(listenSocket_, SOL_SOCKET, SO_REUSEADDR,
               reinterpret_cast<const char*>(&yes), sizeof(yes));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (::bind(listenSocket_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "Failed to bind REST server socket." << std::endl;
        closeSocket(listenSocket_);
        cleanupSockets();
        return false;
    }

    if (::listen(listenSocket_, SOMAXCONN) < 0) {
        std::cerr << "Failed to listen on REST server socket." << std::endl;
        closeSocket(listenSocket_);
        cleanupSockets();
        return false;
    }

    running_ = true;
    acceptThread_ = std::thread(&HttpServer::acceptLoop, this);
    return true;
}

void HttpServer::stop() {
    if (!running_) {
        return;
    }

    running_ = false;
    if (listenSocket_ != INVALID_SOCKET) {
#ifdef _WIN32
        shutdown(listenSocket_, SD_BOTH);
#else
        shutdown(listenSocket_, SHUT_RDWR);
#endif
        closeSocket(listenSocket_);
        listenSocket_ = INVALID_SOCKET;
    }

    if (acceptThread_.joinable()) {
        acceptThread_.join();
    }

    cleanupSockets();
}

bool HttpServer::isRunning() const {
    return running_;
}

void HttpServer::acceptLoop() {
    while (running_) {
        sockaddr_in clientAddr{};
        socklen_t clientAddrLen = sizeof(clientAddr);
        socket_t clientSocket = ::accept(listenSocket_, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            continue;
        }
        std::thread(&HttpServer::handleClient, this, clientSocket).detach();
    }
}

void HttpServer::handleClient(socket_t clientSocket) {
    std::string request;
    if (!readRequest(clientSocket, request)) {
        closeSocket(clientSocket);
        return;
    }

    std::istringstream requestStream(request);
    std::string requestLine;
    std::getline(requestStream, requestLine);
    if (!requestLine.empty() && requestLine.back() == '\r') {
        requestLine.pop_back();
    }

    std::istringstream requestLineStream(requestLine);
    std::string method;
    std::string target;
    std::string version;
    requestLineStream >> method >> target >> version;

    if (method != "POST" || target != "/order") {
        sendResponse(clientSocket, 404, "{\"error\":\"not found\"}");
        closeSocket(clientSocket);
        return;
    }

    std::string headerLine;
    std::string contentType;
    size_t contentLength = 0;
    while (std::getline(requestStream, headerLine) && headerLine != "\r" && !headerLine.empty()) {
        if (headerLine.back() == '\r') {
            headerLine.pop_back();
        }
        auto colon = headerLine.find(':');
        if (colon == std::string::npos) {
            continue;
        }
        std::string name = toLowerCopy(trim(headerLine.substr(0, colon)));
        std::string value = trim(headerLine.substr(colon + 1));
        if (name == "content-type") {
            contentType = toLowerCopy(value);
        } else if (name == "content-length") {
            contentLength = static_cast<size_t>(std::stoull(value));
        }
    }

    std::string body;
    if (contentLength > 0) {
        body.reserve(contentLength);
        char ch;
        for (size_t i = 0; i < contentLength && requestStream.get(ch); ++i) {
            body.push_back(ch);
        }
        while (body.size() < contentLength) {
            char buffer[4096];
            int received = static_cast<int>(::recv(clientSocket, buffer, static_cast<int>(std::min(sizeof(buffer), contentLength - body.size())), 0));
            if (received <= 0) {
                break;
            }
            body.append(buffer, received);
        }
    }

    if (contentLength == 0) {
        sendResponse(clientSocket, 411, "{\"error\":\"missing content-length\"}");
        closeSocket(clientSocket);
        return;
    }

    if (contentType.find("application/json") == std::string::npos) {
        sendResponse(clientSocket, 415, "{\"error\":\"unsupported content type\"}");
        closeSocket(clientSocket);
        return;
    }

    auto orderOpt = parseOrderJson(body);
    if (!orderOpt) {
        sendResponse(clientSocket, 400, "{\"error\":\"invalid JSON body\"}");
        closeSocket(clientSocket);
        return;
    }

    const Order order = *orderOpt;
    bool success = controller_.placeOrder(order);
    std::ostringstream responseBody;
    responseBody << "{\"status\":\"" << (success ? "accepted" : "failed") << "\",";
    responseBody << "\"symbol\":\"" << order.symbol << "\",";
    responseBody << "\"quantity\":" << order.quantity;
    responseBody << "}";

    sendResponse(clientSocket, success ? 200 : 500, responseBody.str());
    closeSocket(clientSocket);
}

bool HttpServer::readRequest(socket_t clientSocket, std::string& request) {
    constexpr size_t bufferSize = 8192;
    request.clear();
    request.reserve(1024);

    while (true) {
        char buffer[bufferSize];
        int received = static_cast<int>(::recv(clientSocket, buffer, static_cast<int>(bufferSize), 0));
        if (received <= 0) {
            return !request.empty();
        }
        request.append(buffer, received);
        if (request.find("\r\n\r\n") != std::string::npos || request.size() > 65536) {
            return true;
        }
    }
}

bool HttpServer::sendResponse(socket_t clientSocket, int status, const std::string& body, const std::string& contentType) {
    std::ostringstream response;
    response << "HTTP/1.1 " << status << " ";
    switch (status) {
        case 200: response << "OK"; break;
        case 400: response << "Bad Request"; break;
        case 404: response << "Not Found"; break;
        case 411: response << "Length Required"; break;
        case 415: response << "Unsupported Media Type"; break;
        case 500: response << "Internal Server Error"; break;
        default: response << "Error"; break;
    }
    response << "\r\n";
    response << "Content-Type: " << contentType << "\r\n";
    response << "Content-Length: " << body.size() << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << body;

    const std::string responseString = response.str();
    size_t totalSent = 0;
    while (totalSent < responseString.size()) {
        int sent = static_cast<int>(::send(clientSocket, responseString.data() + totalSent,
                                          static_cast<int>(responseString.size() - totalSent), 0));
        if (sent <= 0) {
            return false;
        }
        totalSent += static_cast<size_t>(sent);
    }
    return true;
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
        auto start = pos + 1;
        while (start < body.size() && std::isspace(static_cast<unsigned char>(body[start]))) {
            start++;
        }
        auto end = start;
        while (end < body.size() && (std::isdigit(static_cast<unsigned char>(body[end])) || body[end] == '-')) {
            end++;
        }
        if (start == end) {
            return Optional<int>();
        }
        try {
            return Optional<int>(std::stoi(body.substr(start, end - start)));
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

std::string HttpServer::toLower(std::string value) const {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}
