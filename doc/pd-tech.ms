.RP no
.mso www.tmac
.so pdcolors.tmac
.nr pNH 0
\"	#@nh - temporarily disable hyphenation
.de @nh
.it 1 pd-trap
.nr pNH \\n.[.hy]
.nh
..
.de1 pd-trap
.if \\n[pNH] \{\
.  hy \\n[pNH]
.  nr pNH 0
.\}
..
\"	#@hr - create a horizontal line to the end of the line
.de1 @hr
.br
.ti 0
\h'|0'\l'\\n[.l]u-\\n[\\n[.ev]:pri]u\(ul'
..
\" #@ip [text ...] - indented pragraph with a list format; next line 
.de1 @ip
.par*start \\n[\\n[.ev]:ai] 0
.if \\n[.$] \{\
.  di pd-div
.  par*push-tag-env
\&\\$*
.  par*pop-tag-env
.  di
.  chop pd-div
.  ti -\\n[\\n[.ev]:ai]u
.  ie (\\n[dl] + 1n <= \\n[\\n[.ev]:ai]) \{\
.    DEVTAG-COL 1
\\*[pd-div]\h'|\\n[\\n[.ev]:ai]u'\c
.    DEVTAG-COL 2
.  \}
.  el \{\
.    DEVTAG-COL 1
\\*[pd-div]
.    DEVTAG-COL-NEXT 2
.    br
.  \}
.  rm pd-div
.\}
..
\"	#@error [args...] - print error
.de @error
.tm \\(.F: \\$*
..
\"	#FA type name [-const|+const] [type name ...] [tag] - define a function; tag turns into '...'
.de1 FA
.if (\\n[.$] > 1) \{\
.  nf
.  nr mE \\n[.f]
.  ds pd-result \,\fB\\$1\~\\$2\fR\/(
.  shift 2
.  ds \\$0:app
.  ds \\$0:pre
.  while \\n[.$] \{\
.    if '\\$1'-const' \{\
.      ie \\n[.color] \
.        as \\$0:pre " \fB\m[pdgreen]const\m[]\f[]\"
.      el \
.        as \\$0:pre " \fBconst\f[]\"
.      shift
.      continue
.    \}
.    if '\\$1'-static' \{\
.      ie \\n[.color] \
.        as \\$0:pre " \fB\m[pdred]static\m[]\f[]\"
.      el \
.        as \\$0:pre " \fBstatic\f[]\"
.      shift
.      continue
.    \}
.    if '\\$1'+const' \{\
.      ie \\n[.color] \
.        as \\$0:app " \fB\m[pdgreen]const\m[]\f[]\"
.      el \
.        as \\$0:app " \fBconst\f[]\"
.      shift
.      continue
.    \}
.    if '\\$1'...' \{\
.      as pd-result .\|.\|.
.      break
.    \}
.    as pd-result \,\fB\\$1\~\fI\\$2\fR
.    if (\\n[.$] > 2) .as pd-result \/,\~
.    shift 2
.  \}
.  as pd-result )
.  substring \\$0:pre 1 -1
.  substring \\$0:app 1 -1
.  if !'\\*[\\$0:pre]'' \
.    as \\$0:pre \0
.  @ip "\\*[\\$0:pre]\\*[pd-result] \\*[\\$0:app]"
.  ft R
.  fi
.  rm \\$0:pre
.  rm \\$0:app
.\}
..
\" if not in html mode, make aliases for commands that are provided under the www macro package
.do if !'\*[.T]'html' \{\
.  als HR @hr
.\}
." start document
.TL
Project Dewpsi Technical Document
.AU
John Russell
.do ds CH Page \\n[.pn]
.P1
.ND
.AB
In this document we will look at all the classes, functions, and namespaces that are used in Project Dewpsi, that \
require a detailed explanation.
To wit, commands are defined in this document with the type
.CW PDCommand ,
and textures are defined with the type
.CW PDTexture .
.AE
.nr GROWPS 3
.nr PSINCR 1.5p
.
.NH 1
Definitions
.PP The following terms are used in this document and are defined as follows:
.
.IP Command
An object that encapsulates a certain behavior that can be invoked by the program.
A command in this context is represented by a type,
.CW PDCommand .
PDCommand is a base class, so a pointer to it can refer to any of its deriviations.
.
.IP Object
One of two definitions fits depending on the context.
.RS
.IP 1.
In programming, "object" refers to an instantiation of a defined type, usually a class.
Like how
.CW "int a"
refers to an object of the type "int" named "a".
.IP 2.
In the context of Project Dewpsi and talking about objects, components, etc., an object refers \
to an in-game entity that moves itself across the screen or otherwise acts as its own "being".
Other programmers or gamers use the term "sprite".
.RE
.
.IP Texture
A data representation of graphics in a video game, either in a 3D enviroment or a 2D one, like \
in the case for Project Dewpsi.
Depending on the context, either PDTexture or _RefTexture is being referenced.
SDL_Texture will be explicitly referenced using its name.
.
.NH 1
Data Classes
.
.NH 2
PDCommand
.PP
A class that represents a command.
The class defines a single method, DoAction(), which returns
.CW void .
The class cannot be used on its own.
A class must derive from it and implement DoAction().
.
.PP
When the object manager is initialized,
.B RegisterCommands()
is called.
RegisterCommands() is an inline function defined in registerpdcommandtypes\.h that registers commands with \
the object manager.
For every type derived from PDCommand that exists, a
.CW PDCommandType
is used to create a hash ID for a given command builder, with which it is then put into a hash table.
That command builder can afterward be used to create a command of that specific type by calling
.B PDObjectManager::NewCommand() .
.
.PP
.B PDCommandBuilder :
A class that defines a Build() method and returns a pointer to PDCommand.
Derived classes will return pointers to deriviations of PDCommand.
.CW PDCommandTBuilder
is derived from PDCommandBuilder.
It is a class template whose sole parameter is a name of a type to return \
from its Build() method.
.PS
ellipse "PDCommandBuilder" width 1.5;
arrow down;
box "PDCommandTBuilder" width 1.7;
.PE
.
.NH 2
PDTexture
.PP
A class that centralizes the creation, modification, and destruction of this game's textures.
PDTexture has support for image rotation, flipping, and scaling, as well as alpha and color blending.
.
.PP
PDTexture works by keeping a pointer to
.B _RefTexture ,
an opaque structure that holds a pointer to SDL_Texture and a reference count.
Whenever a new PDTexture gets a reference to this structure, its internal reference count \
increments by one.
But when that same texture is deleted, the reference count goes down.
And if that count decrements to zero, the _RefTexture and its data are all freed from memory.
Because of all this, multiple PDTexture's can reference the same SDL_Texture, saving memory in the long run.
.
.NH 3
Member Functions
.
.FA PDTexture* NewTexture -static SDL_Texture* tex
Use this function to create a new PDTexture that keeps a reference to the SDL_Texture provided here.
The return value is a pointer to a PDTexture or
.CW NULL
upon failure; an error message is printed automatically in that case.
.
.FA PDTexture* NewTexture -static "const char*" file char useCk uint32_t ck
Use this function to create a new PDTexture from a file denoted by the string argument.
If
.I useCk
is non-zero, then
.I ck
is used as a colorkey value in the format
.I RRGGBB FF, 0x
where RR, GG, and BB all stand for the red, green, and blue components of the desired color key.
The return value is a pointer to a PDTexture or
.CW NULL
upon failure; an error message is printed automatically in that case.
.
.FA PDTexture* Clone
Use this function to clone a PDTexture, copying its internal data, and increment the reference count.
Because this function uses new, an exception will be thrown if memory allocation fails.
.
.FA void Free -static PDTexture* texture
Use this function to free a texture and clear its internal data.
Decrements the reference count; if zero, it causes the internal _RefTexture to be freed as well.
.
.HR
.PP
Functions for setting the scale, rotation, and flipping of the texture.
.
.FA void SetBlitSize "const Point<int>&" size
Sets the blitting width and height to the values found in
.I size .
.
.FA void ResetBlitSize
Resets the blitting width and height to the original dimensions of the texture.
.
.FA void SetRotation double angle
Sets the rotation angle of the texture in degrees.
The rotation is clockwise.
.
.FA void SetPivot SDL_Point* pivot
Sets the pivot point of rotation.
Accepts a pointer to SDL_Point, which has two integers: \fIx\f[] and \fIy\f[].
They denote a x and y position relative to the texture.
If the pointer is
.CW NULL ,
the pivot is reset to the center of the image.
.
.FA void DisableRotation
Disables rotation for the texture and resets the angle to zero.
.
.FA int SetBlendMode SDL_BlendMode mode
Sets the blend mode of the texture.
.
.FA void SetModulation int value
Sets the modulation for the active blend mode of the texture.
If alpha or additive blending is enabled,
.I value
is a value from 0 to 255 that specifies how solid or how much to ADD to the texture pixels, respectively.
If color modulation is enabled,
.I value
is a color value in the format
.I RRGGBB FF,
where RR, GG, and BB stand for the red, blue, and green components of a color to blend over the texture.
.
.HR
.PP
Accessor functions.
.
.FA SDL_BlendMode GetBlendMode
Get the blend mode of the texture.
.
.FA Point<int> GetBlitSize +const
Get a point representing the width and height of the texture's blitting rectangle.
