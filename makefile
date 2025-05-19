all: main

main: main.o rand_malloc.o
	$(CC) -g -Wall -pedantic $^ -o $@

.c.o:
	$(CC) -g -Wall -pedantic -c $< -o $@

rand_malloc.o: rand_malloc.h
main.o: rand_malloc.h

clean:
	rm -f main *.o
