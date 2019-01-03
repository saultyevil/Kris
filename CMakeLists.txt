cmake_minimum_required(VERSION 3.10)
project(kris C)

set(CMAKE_C_STANDARD 11)

add_executable(kris src/main.c src/term.c src/kris.h src/util.c src/editor.c
        src/init.c src/global_functions.h src/io.c src/key_input.c src/line_ops.c src/highlight.c src/find.c)