CXXFLAGS=-std=c++14 -g -static
SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)

mabicc: $(OBJS)
	$(CXX) -o mabicc $(OBJS) $(LDFLAGS)

$(OBJS): mabicc.hpp

test: mabicc
	./test.sh

clean:
	rm -f mabicc *.o *~ tmp*

.PHONY: test clean