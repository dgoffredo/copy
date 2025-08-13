.PHONY: clean read-write

CXXFLAGS ?= -Wall -Wextra -Werror -pedantic -O3 -flto --std=c++20

read-write: read-write.o posix.o
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS)

clean:
	find . -maxdepth 1 -type f \( -name '*.o' -o -name read-write \) -print0 | xargs -0 rm	
