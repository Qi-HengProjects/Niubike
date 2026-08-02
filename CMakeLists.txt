cmake_minimum_required(VERSION 3.20)
project(Niubike)

set(CMAKE_CXX_STANDARD 20)

add_executable(Niubike
        main.cpp
        DatabaseEngine.cpp
        DatabaseEngine.h
        Structures.h
        UserInterface.h
        UserInterface.h
        UserInterface.cpp
        Helpers.h
        Helpers.cpp
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
)