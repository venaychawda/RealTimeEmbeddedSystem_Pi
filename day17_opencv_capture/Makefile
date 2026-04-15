CXX = g++
CXXFLAGS = -Wall -g $(shell pkg-config --cflags opencv4 2>/dev/null)
LDFLAGS = $(shell pkg-config --libs opencv4 2>/dev/null) -lrt

all: cam_capture

cam_capture: cam_capture.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f cam_capture capture_*.jpg

.PHONY: all clean