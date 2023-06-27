CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

mabicc: $(OBJS)
	$(CC) -o mabicc $(OBJS) $(LDFLAGS)

$(OBJS): mabicc.h

test: mabicc
	./test.sh

clean:
	rm -f mabicc *.o *~ tmp*

.PHONY: test clean