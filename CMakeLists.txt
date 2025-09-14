cmake_minimum_required(VERSION 4.0)
project(clazz_parser)

set(CMAKE_CXX_STANDARD 20)

add_library(clazz_parser
        class_parser.cpp
        class_parser.h
)

add_executable(parser_main main.cpp)
target_link_libraries(parser_main PRIVATE clazz_parser)