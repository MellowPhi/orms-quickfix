#include "HttpServer.h"
#include "OrderApplication.h"
#include "OrderController.h"
#include "OrderService.h"
#include "Order.h"
#include <quickfix/SocketInitiator.h>
#include <quickfix/FileStore.h>
#include <quickfix/FileLog.h>
#include <iostream>
#include <thread>

int main(int argc, char* argv[]) {
    const std::string configFile = "quickfix.cfg";

    try {
        FIX::SessionSettings settings(configFile);
        FIX::FileStoreFactory storeFactory(settings);
        FIX::FileLogFactory logFactory(settings);

        OrderService orderService;
        OrderController orderController(orderService);
        OrderApplication application(orderService);
        HttpServer httpServer(orderController, 8080);
        FIX::SocketInitiator initiator(application, storeFactory, settings, logFactory);

        // Start HTTP server on background thread
        std::thread serverThread([&httpServer](){ httpServer.start(); });

        initiator.start();
        std::cout << "QuickFIX C++ initiator started." << std::endl;
        std::cout << "REST endpoint available at http://localhost:8080/order" << std::endl;
        std::cout << "Press Enter to stop." << std::endl;
        std::cin.get();

        httpServer.stop();
        if (serverThread.joinable()) serverThread.join();

        initiator.stop();
    } catch (const std::exception& ex) {
        std::cerr << "Error starting QuickFIX application: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
