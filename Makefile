CXXFLAGS=-std=c++11 -g -static
SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)

mabicc: $(OBJS)
	$(CC) -o mabicc $(OBJS) $(LDFLAGS)

$(OBJS): mabicc.hpp

test: mabicc
	./test.sh

clean:
	rm -f mabicc *.o *~ tmp*

.PHONY: test clean