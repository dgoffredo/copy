.PHONY: clean read-write

CXXFLAGS ?= -Wall -Wextra -Werror -pedantic -O3 -flto --std=c++20

read-write: read-write.o posix.o
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS)

