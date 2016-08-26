cmake_minimum_required(VERSION 2.8.11)
project(Jogo_da_velha)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.c)
add_executable(Jogo_da_velha ${SOURCE_FILES})
