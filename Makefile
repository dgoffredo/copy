.PHONY: clean read-write

CXXFLAGS ?= -Wall -Wextra -Werror -pedantic -O3 -flto

read-write: read-write.o posix.o
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS)

