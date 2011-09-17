
# default build config
BUILD ?= OSX
# DEBUG build on as default
DEBUG ?= 1

# where to put the build files
BUILDDIR ?= build
# where to put the test files
TESTDIR ?= test

# the name of the lib
LIBNAME=arnovich_core

# source dirs and files
CORE_SRCLIB=src/lib/
CORE_TMPLIB=$(BUILDDIR)/src/lib/
SRCBINLIB=src/bin/
TMPBINLIB=$(BUILDDIR)/src/bin/
include Makefile.files

# dependencies and objects
CORE_BUILDEXT = $(BUILD).DEBUG$(DEBUG)
OBJFILES = $(subst $(CORE_SRCLIB),$(CORE_TMPLIB),$(CORE_SRCFILES:.c=.${CORE_BUILDEXT}.o))
DEPFILES = $(subst $(CORE_SRCLIB),$(CORE_TMPLIB),$(CORE_SRCFILES:.c=.d))
SRCBINFILES=$(wildcard $(SRCBINLIB)*.c)
DEPBINFILES = $(subst $(SRCBINLIB),$(TMPBINLIB),$(SRCBINFILES:.c=.d))

# some common build stuff
MKDIR=mkdir -p
CC=/usr/bin/g++
LN=/usr/bin/g++
PRELIB=lib
PYLIB_EXT=

# OSX build
ifeq (${BUILD},OSX)
	BUILDLIB=/usr/lib/
	BUILDBIN=$(TESTDIR)/bin/
	CORE_CCARG= -x c -I include/ -I ./ -c -fPIC -Wall -I /usr/include/python2.7/ 
	CORE_CCARGBIN= -x c -I include/ -I ./ -Wall -L$(BUILDLIB) -l$(LIBNAME)
	LNARG=-lpython2.7 -dynamiclib -Wl,-v -dylib -Wl,-dynamic
	EXTLIB=dylib
endif

# LINUX 64-bit build
ifeq (${BUILD},LINUX64)
	BUILDLIB=/usr/lib64/
	BUILDBIN=$(TESTDIR)/bin/
	CORE_CCARG= -x c -I include/ -I ./ -c -fPIC -Wall -I /usr/include/python2.7/ 
	CORE_CCARGBIN= -x c -I include/ -I ./ -Wall -L$(BUILDLIB) -l$(LIBNAME)
	LNARG=-lpython2.7 -Wl,-v -shared
	EXTLIB=so
endif

# CYGWIN build
ifeq (${BUILD},CYGWIN)
	BUILDLIB=/usr/lib/
	BUILDBIN=$(TESTDIR)/bin/
	CORE_CCARG= -x c -I include/ -I ./ -c -Wall -I /usr/include/python2.7/
	CORE_CCARGBIN= -x c -I include/ -I ./ -Wall -L$(BUILDLIB) -l$(LIBNAME)
	LNARG=-lpython2.7 -Wl,-v -shared
	EXTLIB=dll.a
endif

# MINGW build - not sure it works anymore
ifeq (${BUILD},MINGW)
	BUILDBIN=$(TESTDIR)\\bin\\
	BUILDLIB=.\\
	CC=gcc
	LN=gcc
	CORE_CCARG= -x c -I include/ -I ./ -c -Wall -I "E:\\Python26\\include" 
	CORE_CCARGBIN= -x c -I include/ -I ./ -Wall -L$(BUILDLIB) -l$(LIBNAME)
	LNARG=-L"E:\\Python26\\libs" -lpython26 -Wl,-v -shared
	EXTLIB=dll
	PYLIB_EXT=--compiler=mingw32
endif

ifeq (${DEBUG},1)
	CORE_CCARGBIN += -g -DDEBUG=${DEBUG}
	CORE_CCARG += -g  -DDEBUG=${DEBUG}
else
	ifeq (${DEBUG},2)
		CORE_CCARGBIN += -g
		CORE_CCARG += -g
	else
		CORE_CCARGBIN += -O1
		CORE_CCARG += -O1
	endif
endif


#### various targets

all: depends lib pylib

.PHONY: docs
docs:
	$(MKDIR) docs
	doxygen core.doxy

pylib:
	python src/pylib/setup.py build $(PYLIB_EXT)
	python src/pylib/setup.py install

clean:
	rm -rf $(BUILDDIR)
	rm -rf $(BUILDBIN)

depends: $(DEPFILES) $(DEPBINFILES)

lib: $(BUILDLIB)${PRELIB}${LIBNAME}.$(EXTLIB)
	
include Makefile.rules

include $(DEPFILES)
include $(DEPBINFILES)

$(BUILDLIB)${PRELIB}${LIBNAME}.$(EXTLIB): $(OBJFILES)
	$(MKDIR) $(BUILDLIB)
	$(LN) $(LNARG) -o $@ $(OBJFILES) $(LNARG)

