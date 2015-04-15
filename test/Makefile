libfiles = ../StartModule.cpp ../IrRc5.cpp
testfiles = TestSuite.cpp Stub.cpp
includedirs = -I. -I..
sourcefiles = $(libfiles) $(testfiles)
headerfiles = ../StartModule.h ../IrRc5.h Arduino.h

all: testbench

clean:
	rm -f testbench

testbench: $(sourcefiles) $(headerfiles)
	g++ $(includedirs) -Wno-write-strings -g -o testbench $(sourcefiles)
