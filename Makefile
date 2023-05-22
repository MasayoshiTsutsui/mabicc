CFLAGS=-std=c11 -g -static

mabicc: mabicc.c

test: mabicc
	./test.sh

clean:
	rm -f mabicc *.o *~ tmp*

.PHONY: test clean