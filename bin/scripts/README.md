# Tcl Scripting Commands

## CallProc
A pure Tcl procedure that accepts one or more arguments. The first argument is the name of a procedure or command. The following arguments are forwared to the invoked procedure/command. Here is an example:

    set id [CallProc open example.txt r]

## level
A command linked to C function _private_LevelCommand in tcl.cc. The next argument is one of the following:

* background
* test

### background

``background`` is a subcommand for manipulating the background layers and color in the level. it has the following options:

* color
* test

**color**

``color`` changes the background color. It accepts three additional arguments: red, green, and blue. All three options are the red green and blue components of a color; each can be 0-255.

**test**

``test`` does not do anything.

## Examples

    level background color 255 0 0
    level background test
    level test
