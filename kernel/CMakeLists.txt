cmake_minimum_required(VERSION 3.22.1)
set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" )

set(CMAKE_BUILD_TYPE Debug)
project (quimblos-sdk)

set(SOURCES
    src/parser.cpp
    src/runner.cpp
    src/engine.cpp
    examples/2-accumulator.cpp
)

add_executable(sandbox ${SOURCES})

target_include_directories(sandbox
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)