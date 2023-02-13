ifeq ($(OS),Windows_NT)
	uname_S := Windows
else
	uname_S := $(shell uname -s)
endif

ifeq ($(uname_S), Windows)
	target = lib/clipX.dll
endif
ifeq ($(uname_S), Linux)
	target = lib/clipX.so
endif

all:
	g++ -L../src/lib -Wall -o test cpp_example.cpp -lclipx -pthread


