CC = g++
CPPFLAGS = -Wall -O2 -pipe -fno-plt -fPIC

default: cpusim

cpusim: cpusim.cpp
	$(CC) $(CPPFLAGS) *.cpp -o cpusim

.PHONY: clean
clean:
	rm -f cpusim
