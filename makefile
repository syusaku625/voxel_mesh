CXX=g++

all: a.out

a.out: main.cpp
	$(CXX) main.cpp

clean:
	rm -f a.out

