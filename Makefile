CFLAGS = -std=c99 -pedantic -Wall -Wextra -O3
DBGFLAGS = -std=c99 -pedantic -Wall -Wextra -g3 -DDEBUG

CC = cc

all: cube.o debugcube.o

cube.s: clean
	${CC} ${CFLAGS} -c -S -o cube.s cube.c

cube.o: clean
	${CC} ${CFLAGS} -c -o cube.o cube.c

debugcube.o: clean
	${CC} ${DBGFLAGS} -c -o debugcube.o cube.c

clean:
	rm -rf *.o

test: debugcube.o
	CUBETYPE=${CUBETYPE} TEST=${TEST} ./test/test.sh

benchmark: cube.o
	CUBETYPE=${CUBETYPE} ./benchmark/bench.sh

.PHONY: all clean test benchmark
