 twim
=========
Vim style TwinCat file editor


Debian/Ubuntu Dependencies
==========================
libuuid-dev
libncurses-dev
libxml2-dev

installation&build
================
$sudo apt-get install libuuid-dev libncurses-dev libxml2-dev
$make


TC specific commands
====================
:ml	    - list methods
:md	    - delete method
:mn	    - create new method
:is	    - insert statement
:id         - insert declaration

q or escape - cancel dialog

CTRL+w w     - switch between declaration/implementation
CTRL+w + UP/DOWN - same as above

:! command   - execute command
:!>command   - execute command and insert output to buffer
