
ARNOVICH CORE LIB
==================


library for common functions and types

Features
--------


TODOs
------

* fix debug call: should move out of header
* move serialisation of variant here from LCL
* srv as variant-based


HOW TO BUILD
-------------

To do a clean build go to the root of the checkout and do
[sudo] ./mk clean all BUILD=<ARCH>
where ARCH=LINUX64/OSX/CYGWIN/MINGW

To do a normal build leave out 'clean'.

To build docs do
./mk docs

