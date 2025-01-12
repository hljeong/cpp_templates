CC = g++ -Wall -g -std=c++17

.PHONY: test cpp

test: cpp
	@ ./a.out

cpp: main.cc
	@ $(CC) main.cc
