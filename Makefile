CXX = g++
CXXFLAGS = -std=c++20
LDFLAGS = -lws2_32

SRCS = main.cpp src/serve_video.cpp src/query_parameters.cpp
OBJS = main.o src/serve_video.o src/query_parameters.o

TARGET = main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	rm -f $(OBJS)

video_server.o: video_server.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/serve_video.o: src/serve_video.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/query_parameters.o: src/query_parameters.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
