CXX = g++
CXXFLAGS = -std=c++20
LDFLAGS = -lws2_32

SRCDIR := src
SRCS := main.cpp $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(SRCS:.cpp=.o)

CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra

# Your executable
TARGET := main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Pattern rule for compiling .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET) 

