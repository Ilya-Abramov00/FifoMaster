
cmake_minimum_required(VERSION 3.5)

project(FifoLib LANGUAGES CXX)


add_subdirectory(FifoF)
add_subdirectory(Server)
add_subdirectory(Client)

add_library(${PROJECT_NAME} INTERFACE)

target_link_libraries(${PROJECT_NAME} INTERFACE Server  Client)


include("unittest.cmake")