CXX = g++
CXXFLAGS = -std=c++17 -Wall

SOURCES = model.cpp io.cpp score.cpp greedy.cpp localsearch.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: layout test

layout: main.cpp $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o layout main.cpp $(OBJECTS)

test: test.cpp $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o test test.cpp $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f layout test $(OBJECTS)

.PHONY: all clean
