ifeq ($(OS),Windows_NT)
	uname_S := Windows
else
	uname_S := $(shell uname -s)
endif

ifeq ($(uname_S), Windows)
	target = lib/libclipx.dll
endif
ifeq ($(uname_S), Linux)
	target = lib/libclipx.so
endif

all:
	rm -rf lib
	mkdir lib
	gcc -c -Wall -Werror -fpic ClipxApi.cpp ClipX.cpp 
	gcc -shared -o $(target) ClipxApi.o ClipX.o
	cp ClipxApi.h ClipX.hpp lib
	rm *.o


