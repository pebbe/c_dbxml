.SUFFIXES:

INCLUDE_PATH = /net/aps/64/include
LIBRARY_PATH = /net/aps/64/lib

HERE = $(realpath .)

all: test libc_dbxml.so

libc_dbxml.so: c_dbxml.cc c_dbxml.h
	g++ -Wall -shared -fPIC -L$(LIBRARY_PATH) -I$(INCLUDE_PATH) -Wl,-rpath=$(LIBRARY_PATH) -o $@ $< -ldbxml

test: test.c c_dbxml.h libc_dbxml.so
	gcc -s -Wall -L. -Wl,-rpath=$(HERE) -o $@ $< -lc_dbxml
