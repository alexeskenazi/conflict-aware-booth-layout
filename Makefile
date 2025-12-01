CXX = g++
CXXFLAGS = -std=c++17 -Wall

SOURCES = model.cpp io.cpp score.cpp greedy.cpp localsearch.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: layout sanity

layout: main.cpp $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o layout main.cpp $(OBJECTS)

sanity: tests/sanity.cpp $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o sanity tests/sanity.cpp $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f layout sanity $(OBJECTS)

.PHONY: all clean
