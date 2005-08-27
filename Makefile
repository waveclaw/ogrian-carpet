# Copyright 2005 Jeremiah D. Powell <jdpowell@mingle.waveclaw.net>
# Licensed under the GNU GPL
# Note: fmod for linux has been copied to /opt before running make

CC             =/usr/bin/gcc
CXX            =$(CC)
LD             =$(CC)
CLEANER        =/bin/rm -rf
COPY           =/bin/cp

# -W -> warning flags, -I -> extra includes for <include> style
# -g -> include info for debuggers, -pg -> include info for profilers
EXTRA_CFLAGS  :=-I./ -I../ -Wall -Wimplicit -Wunused -Wmissing-prototypes -g -pg
OGRE_CFLAGS   :=$(shell pkg-config --cflags OGRE)
RACKNET_CFLAGS:=$(shell pkg-config --cflags raknet)
# Note: fmod for linux has been copied to /opt before running make
FMOD_CFLAGS    =-I/opt/fmodapi374linux/api/inc
CXXFLAGS       =$(OGRE_CFLAGS) $(RACKNET_CFLAGS) $(FMOD_CFLAGS) $(EXTRA_CFLAGS)

OGRE_LIBS     :=$(shell pkg-config --libs OGRE)
RACKNET_LIBS   =/usr/lib/libraknet.so
# Note: fmod for linux has been copied to /opt before running make
FMOD_LIBS     :=/opt/fmodapi374linux/api/libfmod-3.74.so
LDFLAGS        =$(OGRE_LIBS) $(RACKNET_LIBS) $(FMOD_LIBS)

.PHONY: clean fix_gui
.DEFAULT: all

all: fix_gui *.cpp
	$(CC) $(CXXFLAGS) *.cpp $(LDFLAGS) 
	$(COPY) a.out Ogrian/Ogrian

# remove the offending OGRE 1.0 files 
fix_gui:
	-$(CLEANER) OgrianGui.cpp OgrianGui.h OgrianGuiFrameListener.h 

# clean the installed and uninstalled temp files
clean: 
	-$(CLEANER) t *~ *.~ *.o *.bak *.temp *.tmp Ogrian/Ogrian

# for GNU compatability
distclean: clean

# for 'make RandomCppFileWithOutItsCppExtension'
%: %.cpp
	$(CC) $(CXXFLAGS) -c $<
