bin_PROGRAMS= AudioTest NetworkTest

AudioTest_CPPFLAGS= -I$(top_srcdir)/include -I$(top_srcdir)/Tests -DUSE_STUB_H
AudioTest_SOURCES= AudioTest.cpp \
	../src/OgrianAudio.cpp \
	../src/OgrianConst.cpp \
	LogManager.cpp \
	stub.cpp

NetworkTest_CPPFLAGS= -I$(top_srcdir)/include -I$(top_srcdir)/Tests -DUSE_STUB_H
NetworkTest_SOURCES= NetworkTest.cpp \
	../src/OgrianConst.cpp \
	Multiplayer.cpp \
	LogManager.cpp \
	stub.cpp
