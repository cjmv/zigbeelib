# ZigBeeLib compilation file
# $Header$
# $Author: cvicente $
# $Date: 2013-07-30 17:47:08 +0100 (Tue, 30 Jul 2013) $
# $Revision: 100 $

#vpath %.h include
#vpath %.cpp src
#vpath %.o src
VPATH = src:include

CPPFLAGS=-Iinclude/ -fPIC
LDFLAGS=
#LDLIBS=-lpthread -lrt

objects := $(patsubst %.cpp,%.o,$(wildcard src/*.cpp))

libZBAPICpp.a: $(objects)
	ar rcs $@ $(objects)

clean:
	rm -f $(objects) libZBAPICpp.a

install:
	mkdir /usr/local/lib/ZBAPICpp
	cp libZBAPICpp.a /usr/local/lib/ZBAPICpp/
	mkdir /usr/local/include/ZBAPICpp
	cp include/* /usr/local/include/ZBAPICpp/
