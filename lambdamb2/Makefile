# Makefile for NVMe benchmark
.PHONY: module
module: lambdamb2

# Compiler and flags
CFLAGS=--std=c++20 -O3 -g
CXX=g++-12

# The target to compile
lambdamb2: lambdamb2.cpp
	$(CXX) $(CFLAGS) $^ -o $@ 

# Clean rule to remove compiled files
clean:
	rm -f lambdamb2