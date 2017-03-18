#
# Simple Makefile
#
all: test demo

test: Build/testception
	./Build/testception

demo: Build/demo
	./Build/demo

Build/demo: test/demo.cpp selftest.hpp
	mkdir -p Build
	c++ -std=c++11 -I. -Wall -Werror -g -DDEBUG -o Build/demo test/demo.cpp

Build/testception: test/tcmain.cpp test/testception.cpp selftest.hpp
	mkdir -p Build
	c++ -std=c++11 -I. -Wall -Werror -g -DDEBUG -o Build/testception test/tcmain.cpp test/testception.cpp

clean:
	rm -rf Build
