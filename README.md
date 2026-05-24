# orms_cpp

A high-performance C++ Order Management System (OMS) that bridges REST/JSON requests to the FIX 4.2 protocol.

## Features
- **REST API**: Simple JSON interface for order placement.
- **FIX 4.2 Integration**: Uses QuickFIX for reliable financial messaging.
- **Field Normalization**: Support for multiple field aliases and case-insensitive inputs.
- **Health Monitoring**: Built-in health check endpoint.

## Project Layout
- `include/orms/` - Project header files
- `src/` - C++ implementation files
- `thirdparty/` - External libraries (e.g., `httplib`)
- `FIX42.xml` - FIX protocol dictionary
- `quickfix.cfg` - Session settings (Sender: `BANZAI`, Target: `FIXIMULATOR`)

## Dependencies
- **QuickFIX**: FIX protocol engine
- **nlohmann_json**: JSON for Modern C++
- **cpp-httplib**: Header-only HTTP server (included in `src/`)

## Build Instructions

### Prerequisites
Ensure `cmake`, `pkg-config`, and `quickfix` are installed.

```bash
# Ubuntu/Debian
sudo apt-get install libquickfix-dev nlohmann-json3-dev
```

### Build
```bash
cmake -S . -B build
cmake --build build
```

## Running the Demo

### 1. Start a FIX Simulator
The application is configured to connect to a simulator at `localhost:9878`. You can use [FIXIMULATOR](https://github.com/DmitryShtatnov/Fiximulator) or any other FIX acceptor.

### 2. Start the OMS
```bash
./build/orms_cpp
```

### 3. Place an Order
Use `curl` to send a Market Order:

```bash
curl -X POST http://localhost:8080/order \
  -H "Content-Type: application/json" \
  -d '{
    "symbol": "AAPL",
    "quantity": 100,
    "side": "BUY"
  }'
```

**Request Parameters:**
- `symbol` (or `ticker`): The instrument symbol (e.g., "MSFT").
- `quantity`: Positive integer.
- `side`: "BUY", "SELL", "B", or "S" (case-insensitive).

### 4. Check Health
```bash
curl http://localhost:8080/health
```

## API Reference

### POST `/order`
Submits a New Order Single (MsgType=D) to the FIX engine.

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `symbol` | string | Yes | Instrument ticker (alias: `ticker`) |
| `quantity` | int | Yes | Number of units |
| `side` | string | Yes | Direction: `BUY` or `SELL` (aliases: `B`, `S`) |

**Response Codes:**
- `200 OK`: Order successfully forwarded to FIX engine.
- `400 Bad Request`: Invalid JSON, missing fields, or validation error (e.g., non-positive quantity).
- `503 Service Unavailable`: FIX session is not logged on or connection is lost.

### GET `/health`
Returns the status of the HTTP server.

**Response Codes:**
- `200 OK`: Server is healthy.
