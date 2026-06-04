cmake_minimum_required(VERSION 3.22.1)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" )

set(CMAKE_BUILD_TYPE Debug)
project (cpp-langmaker)

# Static Lib (llangmaker.a)

set(SOURCES
    src/ebnf.cpp
    src/parser.cpp
    src/meta.cpp
)

add_library(lang STATIC ${SOURCES})

target_include_directories(lang
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

# Tests

add_executable(test test/lib/main.cpp)

target_include_directories(test
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(test
    PUBLIC lang
)

# CLI

add_executable(langmaker src/cli.cpp)

target_include_directories(langmaker
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(langmaker
    PUBLIC lang
)

# Target

#add_subdirectory(target)