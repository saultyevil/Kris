cmake_minimum_required(VERSION 3.10)
project(kris C)

set(CMAKE_C_STANDARD 11)

add_executable(kris src/kris.c src/term.c src/kris.h src/util.c src/editor.c
        src/init.c src/io.c src/keys.c src/lines.c src/highlight.c src/find.c src/syntax.h)