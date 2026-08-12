CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Werror -O2
SRCS     := $(wildcard src/*.cpp)
OBJS     := $(SRCS:.cpp=.o)
HEADERS  := $(wildcard src/*.h)

# Library objects exclude main.o: the test runner has its own main(), and
# linking both would be a duplicate-symbol error.
LIBOBJS  := $(filter-out src/main.o,$(OBJS))

graphviz: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

src/%.o: src/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

TESTSRC := $(wildcard tests/*.cpp)
TESTOBJ := $(TESTSRC:.cpp=.o)

test_runner: $(TESTOBJ) $(LIBOBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

tests/%.o: tests/%.cpp $(HEADERS) tests/test_harness.h
	$(CXX) $(CXXFLAGS) -Isrc -c -o $@ $<

test: test_runner
	./test_runner

clean:
	rm -f graphviz test_runner src/*.o tests/*.o

.PHONY: test clean
