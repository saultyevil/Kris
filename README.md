# Kilo Text Editor

**WIP: currently only works as a text viewer!**

A simple text editor based on the Kilo text editor. Created by following a 
tutorial found [here](https://viewsourcecode.org/snaptoken/kilo/index.html),
thanks!

## Building and Dependencies

Currently, the editor can be built using CMake. The simplest way to build would
be to build in the directory,

```bash
$ cmake .
$ make
```

This will create the executable `kilo`. You will need a C compiler with support
for the C11 standard. An up-to-date version of `gcc` will be fine.

## Usage

Kilo can be invoked with or without an argument. If an argument is provided,
Kilo will attempt to open that file and display it to screen. If no argument is
provided, then an empty buffer will be loaded.
