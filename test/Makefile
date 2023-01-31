CXX=g++
SANFLAGS=-fsanitize=address -fsanitize=leak -fsanitize=undefined
CXXFLAGS := -Wall -I/../src/graph/ -std=c++20 -g -O2 $(SANFLAGS)

# SRCDIR=../src/
BUILDDIR=./build/

exam: main
	$(CXX) $(SANFLAGS) $(BUILDDIR)main.o -o a.out

main:
	$(CXX) $(CXXFLAGS) -c -o $(BUILDDIR)main.o $@.cpp

.PHONY: clean
clean:
	rm *.out
	rm $(BUILDDIR)*.o