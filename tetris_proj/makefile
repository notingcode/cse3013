target=tetris
source=tetris.o queue.o

$(target) : $(source)
	gcc -o $(target) $(source) -lncurses

tetris.o : tetris.c tetris.h queue.c queue.h
	gcc -c tetris.c

queue.o : queue.c queue.h
	gcc -c queue.c

clean:
	rm $(target) *.o
