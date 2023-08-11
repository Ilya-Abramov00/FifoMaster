cmake_minimum_required(VERSION 3.0)

project(FifoF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(FifoBase)
add_subdirectory(FifoRead)
add_subdirectory(FifoWrite)
add_subdirectory(Fifo)

add_library(${PROJECT_NAME} INTERFACE)

target_link_libraries(${PROJECT_NAME} INTERFACE Fifo)

include("unittest.cmake")

