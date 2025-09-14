cmake_minimum_required(VERSION 4.0)
project(MiniVM)

set(CMAKE_CXX_STANDARD 20)

add_executable(clazz_parser main.cpp
        class_parser.cpp
        class_parser.h
)
