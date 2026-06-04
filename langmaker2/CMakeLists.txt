cmake_minimum_required(VERSION 3.22.1)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" )

set(CMAKE_BUILD_TYPE Debug)
project (cpp-langmaker)

set(SYNTAX_SOURCES
    src/ebnf.cpp
    src/parser.cpp
    src/meta.cpp
)

set(SEMANTICS_SOURCES
    src/semantics.cpp
)

# Tool: Make Semantics Parser (make-semantics)

add_executable(make-semantics ${SYNTAX_SOURCES} tools/make_semantics_parser.cpp)

target_include_directories(make-semantics
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

add_custom_command(
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT ../src/semantics.cpp
  DEPENDS ../**/*
  COMMAND make-semantics
)

# Static Lib (llangmaker.a)

add_library(lang STATIC ${SYNTAX_SOURCES} ${SEMANTICS_SOURCES})

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

## CLI
#
#add_executable(langmaker src/cli.cpp)
#
#target_include_directories(langmaker
#    PRIVATE ${PROJECT_SOURCE_DIR}/include
#)
#
#target_link_libraries(langmaker
#    PUBLIC lang
#)
#
## Target
#
##add_subdirectory(target)