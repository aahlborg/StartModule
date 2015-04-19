libfiles = ../StartModule.cpp ../IrRc5.cpp
testfiles = TestSuite.cpp Stub.cpp
sourcefiles = $(libfiles) $(testfiles)
headerfiles = ../StartModule.h ../IrRc5.h Arduino.h

includedirs = -I. -I..
defines = -DTEST

all: testbench

clean:
	rm -f testbench

testbench: $(sourcefiles) $(headerfiles)
	g++ $(includedirs) $(defines) -Wall -Wno-write-strings -g -o testbench $(sourcefiles)
