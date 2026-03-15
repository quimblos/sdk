cmake_minimum_required(VERSION 3.22.1)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" )

set(CMAKE_BUILD_TYPE Debug)
project (quimblos-sdk)

# Static Lib (lquimblos.a)

set(SOURCES
    src/engine.cpp
    src/hex.cpp
    src/memory.cpp
    src/operator.cpp
    src/parser.cpp
    src/runner.cpp
    src/static_checker.cpp
)

# WASM
if (DEFINED EMSCRIPTEN)

    add_executable(quimblos ${SOURCES} wasm.cpp)
    set(CMAKE_EXECUTABLE_SUFFIX ".js")

    target_include_directories(quimblos
        PRIVATE ${PROJECT_SOURCE_DIR}/include
    )

    set_target_properties(quimblos PROPERTIES COMPILE_FLAGS "-Os")
    set_target_properties(quimblos PROPERTIES LINK_FLAGS    "-Os -s STANDALONE_WASM -s EXPORT_ES6=1 -s ENVIRONMENT=web -lembind --no-entry")

# x86/x64
else()

    # Static Lib

    add_library(quimblos STATIC ${SOURCES})

    target_include_directories(quimblos
        PRIVATE ${PROJECT_SOURCE_DIR}/include
    )

    # Tests

    add_executable(test test/main.cpp)

    target_include_directories(test
        PRIVATE ${PROJECT_SOURCE_DIR}/include
    )

    target_link_libraries(test
        PUBLIC quimblos
    )
endif()