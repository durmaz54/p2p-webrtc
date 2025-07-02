cmake_minimum_required(VERSION 3.18)
project(p2p_client VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

add_compile_options(-Wall -Wextra -Wpedantic)

find_package(LibDataChannel REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(p2p_client src/main.cpp)
target_link_libraries(p2p_client LibDataChannel::LibDataChannel)
target_link_libraries(p2p_client nlohmann_json::nlohmann_json)


