cmake_minimum_required(VERSION 3.5)

project(PipeServer LANGUAGES CXX)
set(UNITTEST ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/binaries)
add_executable(PipeServer Server.cpp)

project(PiperClient LANGUAGES CXX)
set(UNITTEST ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/binaries)
add_executable(PiperClient Client.cpp)




