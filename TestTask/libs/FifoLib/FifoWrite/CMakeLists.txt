cmake_minimum_required(VERSION 3.0)

project(FifoWrite)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/include/FifoWrite/*.h)


add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC FifoBase)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME})

include("unittest.cmake")
