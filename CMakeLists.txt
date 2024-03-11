cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(magneta)

FILE(GLOB app_sources
    src/*.c
    src/bma400/*.c
    src/bm83/*.c
    src/bm83/*.cpp
)
target_sources(app PRIVATE ${app_sources})

target_include_directories(app PRIVATE
src
src/bma400
src/bm83
)
zephyr_library_include_directories(.)
