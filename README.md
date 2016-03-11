Euclid
======

Euclid is a toy game engine, written in C++14, with Lua for the light
lifting. The provided renderer is for retro FPS style games, and under the
hood it uses something like the technique used by the Build engine's
renderer.

Requirements
------------

- Windows
- VS2015
- Cygwin, for packaging

Also uses SDL2 and Lua 5.3, but those are included and built for you

Usage
-----

In cygwin, from within the root directory, run  ./script/package.sh

Copy the package directory elsewhere to start a game.

Your game will need a game.cfg, specifying:

- startscript
- startstate

startscript will be loaded and run in full, and should pull in other
files/systems as needed. startstate should be the name of a class (created
with the `CreateNewClass()` function) which may be used in the state machine. It
must implement `Update()` and `Render()` at a minimum.

If startscript is edited and saved, the game will reload it and run it. It
should be written in such a way that this does not break anything! Other
files may be given this behaviour, by loading them with
`Game.LoadAndWatchFile("foo")`

~~For further help, see the API documention~~ doesn't exist yet, see the
source, or the editor's lua source code!

Directory Structure
-------------------

### engine

This is the root for the core euclid project

### extern

Any externally required libraries, SDL2, Lua, SDL2_image, etc

### fonts

Font files used to build the bitmapped fonts

### tools

External tools, CBFG is the tool used to make the bitmapped font textures

### script

Utility scripts, for code statistics. Run package.sh from the root directory
to build all configurations, and to create a package using the Shipping
config

### package

Results of package.sh end up here

### content

the Lua parts of the engine. Files from here will be moved into the package
directory, along with the necessary binary files

TODO
----

- The structure of package/ is not ideal
- API documentation doesn't exist!
