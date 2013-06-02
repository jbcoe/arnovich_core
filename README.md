
ARNOVICH CORE
==============

This is a simple library for holding some of the common functionality used across my various projects.
It consists mainly of a few simple types and helpers.

Since anything you put up on Github(or anywhere else) is <a href="http://en.wikipedia.org/wiki/Berne_Convention">by default copyrighted</a> I have added a BSD-3 license, just in case.

There is some auto-generated documentation at <a href="http://arnovich.github.io/core/">http://arnovich.github.io/core/</a>.

Features
--------

Things that you will find here:

* Variant type that can easily be shared and interfaced across C/C++/Python libraries
* A very simple debug outputter that works across C/C++/Python
* An even simpler push-pop-notification server for C/Python


TODOs
------

Things I should really get done

* fix debug call: should move out of header
* move XML serialisation of variant here from LCL
* server could be variant-based


HOW TO BUILD
-------------

For a clean build go to the root of the checkout and do
    [sudo] ./mk clean all BUILD=[ARCH]
where ARCH=LINUX64/OSX/CYGWIN/MINGW

To do a normal build leave out 'clean'.

To build docs do
    ./mk docs

[![githalytics.com alpha](https://cruel-carlota.pagodabox.com/9f544152c9bcee0bca43ee9a96ea72e8 "githalytics.com")](http://githalytics.com/arnovich/core)
