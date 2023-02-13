ifeq ($(OS),Windows_NT)
	uname_S := Windows
else
	uname_S := $(shell uname -s)
endif

ifeq ($(uname_S), Windows)
	target = lib/libfoo.dll
endif
ifeq ($(uname_S), Linux)
	target = lib/libfoo.so
endif

all:
	rm -rf lib
	mkdir lib
	gcc -c -Wall -Werror -fpic ClipxApi.cpp ClipX.cpp 
	gcc -shared -o $(target) ClipxApi.o ClipX.o
	rm *.o


