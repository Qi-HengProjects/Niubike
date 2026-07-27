cmake_minimum_required(VERSION 4.3)
project(Niubike)

set(CMAKE_CXX_STANDARD 20)

add_executable(Niubike main.cpp
        Structures.h
        DatabaseEngine.h
        DatabaseEngine.cpp)
