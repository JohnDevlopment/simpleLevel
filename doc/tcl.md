# Tcl Scripting Commands

## CallProc
A pure Tcl procedure that accepts one or more arguments. The first argument is the name of a procedure or command. The following arguments are forwarded to the invoked procedure/command. Here is an example:

    set id [CallProc open example.txt r]

## level
A command linked to C function \_private\_LevelCommand in tcl.cc. The next argument is one of the following:

* background

### background
``background`` is a subcommand for manipulating the background layers and color in the level. it has the following options:

* color
* offsety

#### color
``color`` changes the background color. Its syntax is as follows: `level background color red green blue`. The red, green and blue values can be from 0 to 255. The alpha channel is 255 by default.

#### offsety
`offsety` changes the Y position of the selected background layer. Its syntax is as follows: `level background offsety bgnum value`. *bgnum* is a lookup into an array where the XY positions of the background layers are kept. *value* is just an integer that gets added to the selected background layer's Y position.

## Examples
    level background color 255 0 0
    level background offsety 1 352

## GetHash
A command linked to \_private\_GetHash. Here is the syntax: ``GetHash str max offset``. The return value is a hash of the *str* argument, a value no larger than *max* but added to by *offset*.

For levels, the hash parameters are: max=5000 offset=200.
