cmake_minimum_required(VERSION 3.0)
project(TIN)
set(CMAKE_CXX_STANDARD 14)

file(GLOB client_SOURCES "Client/*.h" "Client/*.cpp" "Common/*.cpp" "Common/*.h")
file(GLOB server_SOURCES "TicketServer/*.h" "TicketServer/*.cpp" "Common/*.cpp" "Common/*.h")

add_executable(client ${client_SOURCES})
add_executable(server ${server_SOURCES})
