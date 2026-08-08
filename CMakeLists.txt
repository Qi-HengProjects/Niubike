cmake_minimum_required(VERSION 3.20)
project(Niubike)

set(CMAKE_CXX_STANDARD 20)

add_executable(Niubike
        main.cpp
        DatabaseEngine.cpp
        DatabaseEngine.h
        Structures.h
        MainMenu.h
        MainMenu.cpp
        Helpers.h
        Helpers.cpp
        RentalModule.h
        RentalModule.cpp
        PaymentModule.h
        PaymentModule.cpp
        ReturnModule.h
        ReturnModule.cpp
        History.cpp
        History.h
)