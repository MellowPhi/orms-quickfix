cmake_minimum_required(VERSION 3.15)
project(orms_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(QuickFIX REQUIRED IMPORTED_TARGET quickfix)

# Optional: find nlohmann_json (header-only)
find_package(nlohmann_json REQUIRED)

add_executable(orms_cpp
    src/main.cpp
    src/OrderApplication.cpp
    src/OrderController.cpp
    src/OrderService.cpp
    src/HttpServer.cpp
)

target_include_directories(orms_cpp PRIVATE ${QuickFIX_INCLUDE_DIRS} src)
target_link_libraries(orms_cpp PRIVATE PkgConfig::QuickFIX nlohmann_json::nlohmann_json)

if (WIN32)
    target_link_libraries(orms_cpp PRIVATE ws2_32)
endif()

install(TARGETS orms_cpp DESTINATION bin)
