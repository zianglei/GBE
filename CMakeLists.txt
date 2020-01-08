cmake_minimum_required(VERSION 3.15)
project(GBE)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(GBE
        src/main.cpp
        src/gameboy.cpp
        src/cpu.cpp
        src/register.cpp
        src/cartridge.cpp
        src/mmu.cpp)


add_executable(cartridge_test
        test/test-cartridge.cpp)