cmake_minimum_required(VERSION 3.22)
project(translator)

set(CMAKE_CXX_STANDARD 14)

add_executable(translator launch.cpp translator.cpp translator.h variable_table.h const_table.h lexeme.h variable_table.cpp)
