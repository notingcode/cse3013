target=tetris
source=tetris.o

$(target) : $(source)
	gcc -g -o $(target) $(source) -lncurses

tetris.o : tetris.c tetris.h
	gcc -g -c tetris.c

clean:
	rm $(target) *.o
