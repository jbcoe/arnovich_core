
BUILD ?= OSX
DEBUG ?= 1

BUILDDIR ?= build
TESTDIR ?= test

CORE_SRCLIB=src/lib/
CORE_TMPLIB=$(BUILDDIR)/src/lib/
SRCBINLIB=src/bin/
TMPBINLIB=$(BUILDDIR)/src/bin/
MKDIR=mkdir -p

include Makefile.files

CORE_BUILDEXT = $(BUILD).DEBUG$(DEBUG)
OBJFILES = $(subst $(CORE_SRCLIB),$(CORE_TMPLIB),$(CORE_SRCFILES:.c=.${CORE_BUILDEXT}.o))

DEPFILES = $(subst $(CORE_SRCLIB),$(CORE_TMPLIB),$(CORE_SRCFILES:.c=.d))

SRCBINFILES=$(wildcard $(SRCBINLIB)*.c)
DEPBINFILES = $(subst $(SRCBINLIB),$(TMPBINLIB),$(SRCBINFILES:.c=.d))

CORE_CCARG= -x c -I include/ -I ./ -c -fPIC -Wall -I /usr/include/python2.6/ 
CORE_CCARGD=$(CORE_CCARG)
LNARG=-lpython2.6 -dynamiclib -Wl,-v -dylib -Wl,-dynamic

CC=/usr/bin/g++
LN=/usr/bin/g++
EXTLIB=dylib
PRELIB=lib
DELIM=/

HERE=/usr/
BUILDLIB=$(HERE)lib$(DELIM)
BUILDBIN=$(TESTDIR)$(DELIM)bin$(DELIM)

NAME=arnovich_core

CORE_CCARGBIN= -x c -I include/ -I ./ -Wall -L$(BUILDLIB) -l$(NAME)

PYLIB_EXT=

ifeq (${BUILD},LINUX64)
	BUILDLIB=$(HERE)lib64$(DELIM)
	LNARG=-lpython2.6 -Wl,-v -shared
	EXTLIB=so
endif

ifeq (${BUILD},CYGWIN)
	CORE_CCARG= -x c -I include/ -I ./ -c -Wall -I /usr/include/python2.6/
	BUILDLIB=$(HERE)lib$(DELIM)
	LNARG=-lpython2.6 -Wl,-v -shared
	EXTLIB=dll.a
endif

ifeq (${BUILD},WIN32)
	CC=gcc
	LN=gcc
	CORE_CCARG= -x c -I include/ -I ./ -c -Wall -I "E:\\Python26\\include" 
	BUILDLIB=.\\
	LNARG=-L"E:\\Python26\\libs" -lpython26 -Wl,-v -shared
	EXTLIB=dll
	PYLIB_EXT=--compiler=mingw32
endif

ifeq (${DEBUG},1)
	CORE_CCARGBIN += -g -DDEBUG=${DEBUG}
	CORE_CCARG += -g  -DDEBUG=${DEBUG}
	CORE_CCARGD += -DDEBUG=${DEBUG}
else
	ifeq (${DEBUG},2)
		CORE_CCARGBIN += -g
		CORE_CCARG += -g
	else
		CORE_CCARGBIN += -O1
		CORE_CCARG += -O1
	endif
endif

all: depends lib

docs:
	doxygen core.doxy

pylib:
	python src/pylib/setup.py build $(PYLIB_EXT)
	python src/pylib/setup.py install

clean:
	rm -rf $(BUILDDIR)
	rm -rf $(BUILDBIN)

cleanobjs:
	find $(CORE_TMPLIB) -name '*.o' -exec rm {} \;
	find $(TMPBINLIB) -name '*.o' -exec rm {} \;

cleandeps:
	find $(CORE_TMPLIB) -name '*.d' -exec rm {} \;
	find $(TMPBINLIB) -name '*.d' -exec rm {}  \;
	
depends: $(DEPFILES) $(DEPBINFILES)

lib: $(BUILDLIB)${PRELIB}${NAME}.$(EXTLIB)
	
$(TMPBINLIB)%.d: $(SRCBINLIB)%.c
	@echo "re-depending: $<"
	mkdir -p $(@D)
	@set -e; rm -f $@; \
	$(CC) -MM $(CORE_CCARGD) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(BUILDBIN)\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include Makefile.rules

include $(DEPFILES)
include $(DEPBINFILES)

$(BUILDLIB)${PRELIB}${NAME}.$(EXTLIB): $(OBJFILES)
	$(MKDIR) $(BUILDLIB)
	$(LN) $(LNARG) -o $@ $(OBJFILES) $(LNARG)
