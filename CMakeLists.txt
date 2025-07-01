cmake_minimum_required(VERSION 3.18)
project(p2p_client VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # clang-tools için

# ▸ Uyarılar (isteğe bağlı sertlik)
add_compile_options(-Wall -Wextra -Wpedantic)

# ▸ vcpkg'den gelen libdatachannel paketi
#    Paket adı 'rtc'; hedefi 'rtc::rtc'
find_package(LibDataChannel CONFIG REQUIRED)
find_package(libpqxx CONFIG REQUIRED)
find_package(cpr REQUIRED)



# ▸ Uygulama
add_executable(p2p_client src/main.cpp)
target_link_libraries(p2p_client PRIVATE LibDataChannel::LibDataChannel)
target_link_libraries(p2p_client PRIVATE libpqxx::pqxx)
target_link_libraries(p2p_client PRIVATE cpr::cpr)

# ▸ Linux'ta pthread'i ekstra isterse (genelde rtc::rtc getirir)
find_package(Threads REQUIRED)
target_link_libraries(p2p_client PRIVATE Threads::Threads)

