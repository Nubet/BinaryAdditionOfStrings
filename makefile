all: main

main: main.o primlib.o rand_malloc.o
	gcc -g -Wall -pedantic $^ -o $@ -lSDL2_gfx `sdl2-config --libs` -lm

.c.o:
	gcc -g -Wall -pedantic `sdl2-config --cflags` -c $< -o $@

primlib.o: primlib.h
rand_malloc.o: rand_malloc.h
main.o: primlib.h rand_malloc.h

clean:
	rm -f main *.o
