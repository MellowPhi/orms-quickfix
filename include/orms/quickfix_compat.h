#pragma once

// Wrapper to suppress C++17 dynamic exception specification errors in QuickFIX
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc++17-compat"

#include <quickfix/SocketInitiator.h>
#include <quickfix/FileStore.h>
#include <quickfix/FileLog.h>

#pragma GCC diagnostic pop
