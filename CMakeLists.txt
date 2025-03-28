cmake_minimum_required(VERSION 3.30)
project(NESEmu)

set(CMAKE_CXX_STANDARD 20)

add_executable(NESEmu main.cpp
        NES.cpp
        NES.hpp
        CPU.cpp
        CPU.hpp
        SystemBus.cpp
        SystemBus.hpp
        Cartridge.cpp
        Cartridge.hpp)
