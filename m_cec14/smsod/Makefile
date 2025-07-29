CXX      := g++
CXXFLAGS := -O3 -Wall -I/usr/local/include \
            -march=native -mtune=native \
            -flto \
            -funroll-loops \
            -fprefetch-loop-arrays

LDFLAGS  := -L/usr/local/lib -lgsl -lgslcblas -lm -flto


SRCS    := $(wildcard *.cpp)
OBJS    := $(SRCS:.cpp=.o)

TARGET  := main

.PHONY: all clean
all: $(TARGET)


$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

