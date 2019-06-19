cmake_minimum_required(VERSION 2.8)

include_directories(src)
include_directories(flatbuffers/include)

set(Client_SRCS src/client.cpp)
set(Server_SRCS src/server.cpp)

add_executable(client ${Client_SRCS})
add_executable(server ${Server_SRCS})
