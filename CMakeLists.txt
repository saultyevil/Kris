cmake_minimum_required(VERSION 3.10)
project(kilo C)

set(CMAKE_C_STANDARD 11)

add_executable(kilo src/main.c src/term.c src/kilo.h src/util.c src/editor.c
        src/init.c src/global_functions.h src/io.c src/key_input.c)