# orms_cpp

This folder contains a C++ QuickFIX implementation

## Project layout

- `CMakeLists.txt` - build configuration
- `quickfix.cfg` - FIX session settings
- `src/Order.h` - order DTO equivalent
- `src/OrderController.h/cpp` - controller layer
- `src/OrderService.h/cpp` - service layer that constructs and sends FIX NewOrderSingle messages
- `src/OrderApplication.h/cpp` - QuickFIX application and Execution Report handler
- `src/main.cpp` - REST server launcher and controller integration
- `src/HttpServer.h/cpp` - minimal REST server exposing `POST /order`

## Dependicies
- Use FIXIMLATOR as a mock FX Simulator
- Any REST client or curl to POST order

## Build

Make sure `cmake` and QuickFIX are installed and available in your environment.

```bash
cd cpp
cmake -S . -B build
cmake --build build
```

## Run

```bash
cd cpp\build
./orms_cpp
```

The program will start a QuickFIX initiator using `quickfix.cfg` and expose a REST endpoint.

### REST endpoint

- POST `http://localhost:8080/order`
- Request body JSON: `{"symbol":"AAPL","quantity":10}`
- Response body JSON includes order status and echo fields

Press Enter in the console to shut down the server.
