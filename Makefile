.PHONY: clean run-tetris

tetris: tetris.c initializeTetris.c
	cc tetris.c initializeTetris.c -o tetris -lncurses

run-tetris: tetris
	./tetris

clean:
	rm -f tetris
