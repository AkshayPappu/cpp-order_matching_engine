# Order Matching Engine System

A complete order matching system consisting of a high-performance C++20 order matching engine and a Python-based order generation server.

## Project Overview

This system implements a complete order matching environment with two main components:

1. **Order Matching Engine** (C++20)
   - Processes and matches buy/sell orders in real-time
   - Implements a limit order book system
   - Connects to the order feed server via TCP socket
   - Matches orders based on price-time priority
   - Utilizes multithreading for high-performance order processing

2. **Order Generation Server** (Python)
   - Simulates a market data feed
   - Generates and sends random buy/sell orders
   - Provides a test environment for the matching engine

## Project Structure

```
.
├── order-matching-engine/     # C++ Order Matching Engine
│   ├── main.cpp              # Main program entry point
│   ├── OrderBook.hpp         # Order book implementation
│   ├── OrderReader.hpp       # Socket connection interface
│   └── OrderReader.cpp       # Order reading implementation
│
└── server/                   # Python Order Generation Server
    └── order_server.py      # Main server implementation
```

## Building and Running

### Order Matching Engine (C++)

Requirements:
- C++20 compatible compiler (e.g., GCC 10+)
- Make sure you're in the order-matching-engine directory

Build the engine:
```bash
cd order-matching-engine
g++ -std=c++20 -O2 -pthread main.cpp OrderReader.cpp -o engine
```

Run the engine:
```bash
./engine
```

Note: The engine uses multiple threads for order processing. The number of worker threads is currently set to 4 by default.

### Order Generation Server (Python)

Requirements:
- Python 3.x
- Required Python packages (install via pip):
```bash
cd server
pip install -r requirements.txt
```

Run the server:
```bash
python order_server.py
```

## System Architecture

### Order Matching Engine (C++)

#### Key Components

1. **Order Class**
   - Represents individual orders with:
     - Type (BUY/SELL)
     - Price
     - Quantity
     - Order ID
     - Timestamp

2. **OrderBook Class**
   - Maintains priority queues for buy/sell orders
   - Buy orders: highest price first
   - Sell orders: lowest price first
   - Methods for order management and matching

3. **OrderReader Class**
   - Handles TCP socket connection
   - Reads and parses incoming orders
   - Provides interface for main processing loop

4. **OrderQueue Class**
   - Thread-safe queue implementation
   - Manages order distribution to worker threads
   - Implements producer-consumer pattern
   - Handles graceful shutdown of worker threads

5. **Multithreaded Architecture**
   - Producer-Consumer pattern implementation
   - Main thread acts as producer:
     - Reads orders from socket
     - Adds orders to thread-safe queue
   - Multiple worker threads act as consumers:
     - Process orders from queue
     - Execute order matching logic
   - Synchronized logging with mutex protection
   - Graceful shutdown handling
   - Default configuration: 4 worker threads

### Order Generation Server (Python)

- Simulates a market data feed
- Generates random orders with realistic parameters
- Sends orders to the matching engine via TCP socket
- Configurable order generation parameters

## Configuration

### Order Matching Engine
- Default settings in `main.cpp`:
  - Host: 127.0.0.1 (localhost)
  - Port: 9000

### Order Generation Server
- Configuration options in `order_server.py`:
  - Order generation frequency
  - Price ranges
  - Quantity ranges
  - Order type distribution

## Error Handling

The system includes comprehensive error handling:

### Order Matching Engine
- Connection failures
- Socket communication errors
- Invalid order data
- Runtime exceptions

### Order Generation Server
- Connection management
- Data validation
- Error logging
- Graceful shutdown

## Testing the System

1. Start the order generation server:
```bash
cd server
python order_server.py
```

2. In a separate terminal, start the matching engine:
```bash
cd order-matching-engine
./engine
```

3. The server will generate orders that the engine will process and match

## Future Improvements

Potential enhancements for both components:

### Order Matching Engine
- Support for different order types
- Order cancellation
- Advanced matching algorithms
- Order book persistence
- Performance metrics
- REST API interface
- Unit and integration tests
- Dynamic thread pool sizing
- Thread affinity configuration
- Performance monitoring per thread
- Lock-free queue implementation

### Order Generation Server
- More sophisticated order generation algorithms
- Market simulation features
- Multiple instrument support
- Historical data replay
- Performance monitoring
- Web interface for control

## License

[Add your chosen license here]

## Author

Akshay Pappu 