bin_PROGRAMS= AudioTest NetworkTest SpinBufferTest

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

SpinBufferTest_CPPFLAGS= -I$(top_srcdir)/include -I$(top_srcdir)/Tests -I$(top_srcdir) -DDEBUG
SpinBufferTest_SOURCES= SpinBufferTest.cpp ../src/SpinBuffer.cpp
