# orms_cpp

A high-performance C++ Order Management System (OMS) that bridges REST/JSON requests to the FIX 4.2 protocol.

## Running the Demo

### 1. Start a FIX Simulator
The application is configured to connect to a simulator at `localhost:9878`. You can use [FIXIMULATOR](https://github.com/DmitryShtatnov/Fiximulator) or any other FIX acceptor.

### 2. Start the OMS
```bash
./build/orms_cpp
```

```bash
❯ ./build/orms_cpp
Session created: FIX.4.2:BANZAI->FIXIMULATOR
[HTTP] Listening on http://0.0.0.0:8080
QuickFIX C++ initiator started.
REST endpoint available at http://localhost:8080/order
Press Enter to stop.
ToAdmin: 8=FIX.4.29=7835=A34=149=BANZAI52=20260524-15:52:52.04956=FIXIMULATOR98=0108=30141=Y10=063
FromAdmin: 8=FIX.4.29=7835=A34=149=FIXIMULATOR52=20260524-15:52:52.06256=BANZAI98=0108=30141=Y10=058
Logon successful: FIX.4.2:BANZAI->FIXIMULATOR
OrderService session set: FIX.4.2:BANZAI->FIXIMULATOR
Controller received order request: AAPL x 100
ToApp: 8=FIX.4.29=13435=D34=249=BANZAI52=20260524-15:53:06.22356=FIXIMULATOR11=187037968135696780321=138=10040=154=255=AAPL60=20260524-15:53:0610=061
Controller forwarded order to service.
FromAdmin: 8=FIX.4.29=6035=034=249=FIXIMULATOR52=20260524-15:53:22.84456=BANZAI10=225
ToAdmin: 8=FIX.4.29=6035=034=349=BANZAI52=20260524-15:53:36.09956=FIXIMULATOR10=233
FromAdmin: 8=FIX.4.29=6035=034=349=FIXIMULATOR52=20260524-15:53:53.83956=BANZAI10=234
ToAdmin: 8=FIX.4.29=6035=034=449=BANZAI52=20260524-15:54:06.13256=FIXIMULATOR10=220
FromAdmin: 8=FIX.4.29=6035=034=449=FIXIMULATOR52=20260524-15:54:23.84156=BANZAI10=226
Logout: FIX.4.2:BANZAI->FIXIMULATOR
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


<img width="1224" height="934" alt="image" src="https://github.com/user-attachments/assets/c065ab9c-3eda-45f5-a6ef-49c7a55378e6" />
FIXIMULATOR recieves new order FIX message.

```bash
<20260524-16:04:45, FIX.4.2:FIXIMULATOR->BANZAI, event> (Session FIX.4.2:FIXIMULATOR->BANZAI schedule is daily, 00:00:00 UTC - 00:00:00 UTC (daily, 00:00:00 UTC - 00:00:00 UTC))
<20260524-16:04:45, FIX.4.2:FIXIMULATOR->BANZAI, event> (Created session: FIX.4.2:FIXIMULATOR->BANZAI)
May 25, 2026 2:04:45 AM quickfix.mina.acceptor.AbstractSocketAcceptor startAcceptingConnections
INFO: Listening for connections at 0.0.0.0/0.0.0.0:9878
May 25, 2026 2:04:52 AM quickfix.mina.acceptor.AcceptorIoHandler sessionCreated
INFO: MINA session created: /127.0.0.1:47124
<20260524-16:04:52, FIX.4.2:FIXIMULATOR->BANZAI, incoming> (8=FIX.4.29=7835=A34=149=BANZAI52=20260524-16:04:52.84456=FIXIMULATOR98=0108=30141=Y10=064)
<20260524-16:04:52, FIX.4.2:FIXIMULATOR->BANZAI, event> (Accepting session FIX.4.2:FIXIMULATOR->BANZAI from /127.0.0.1:47124)
<20260524-16:04:52, FIX.4.2:FIXIMULATOR->BANZAI, event> (Acceptor heartbeat set to 30 seconds)
<20260524-16:04:52, FIX.4.2:FIXIMULATOR->BANZAI, event> (Logon contains ResetSeqNumFlag=Y, resetting sequence numbers to 1)
<20260524-16:04:52, FIX.4.2:FIXIMULATOR->BANZAI, event> (Received logon request)
<20260524-16:04:52, FIX.4.2:FIXIMULATOR->BANZAI, outgoing> (8=FIX.4.29=7835=A34=149=FIXIMULATOR52=20260524-16:04:52.86456=BANZAI98=0108=30141=Y10=066)
<20260524-16:04:52, FIX.4.2:FIXIMULATOR->BANZAI, event> (Responding to logon request)
<20260524-16:04:53, FIX.4.2:FIXIMULATOR->BANZAI, incoming> (8=FIX.4.29=13435=D34=249=BANZAI52=20260524-16:04:53.17956=FIXIMULATOR11=187037968135696780321=138=10040=154=255=AAPL60=20260524-16:04:5310=069)
SecurityID: null
IDSource: null
<20260524-16:05:23, FIX.4.2:FIXIMULATOR->BANZAI, outgoing> (8=FIX.4.29=6035=034=249=FIXIMULATOR52=20260524-16:05:23.84456=BANZAI10=224)
<20260524-16:05:23, FIX.4.2:FIXIMULATOR->BANZAI, incoming> (8=FIX.4.29=6035=034=349=BANZAI52=20260524-16:05:23.84856=FIXIMULATOR10=229)
```

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
