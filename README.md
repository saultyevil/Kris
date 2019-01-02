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

## VT100 Control Sequences

```
The following is a summary of the VT100 control sequences.

Cursor Up	                            ESC A
Cursor Down                          	    ESC B
Cursor Right	                            ESC C
Cursor Left	                            ESC D
Select Special Graphics character set	    ESC F
Select ASCII character set	            ESC G
Cursor to home	                            ESC H
Reverse line feed	                    ESC I
Erase to end of screen	                    ESC J
Erase to end of line	                    ESC K
Direct cursor address	                    ESC Y l c	(see note 1)
Identify	                            ESC Z	(see note 2)
Enter alternate keypad mode	            ESC =
Exit alternate keypad mode	            ESC >
Enter ANSI mode	                            ESC <

NOTE 1: Line and column numbers for direct cursor address are single
        character codes whose values are the desired number plus 378.
        Line and column numbers start at 1.
NOTE 2: Response to ESC Z is ESC / Z.
```