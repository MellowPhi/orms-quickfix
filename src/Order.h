#pragma once

#include <string>

struct Order {
    std::string symbol;
    int quantity;
    std::string side; // "BUY" or "SELL"
};
