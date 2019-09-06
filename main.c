#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"



/* generates the all game */
int main(int argc, char *argv[]){
	/* set the seed for the random function from the main arguments */
	char* seedInput = argv[argc-1];
	int seed = atoi(seedInput);
	setbuf(stdout, NULL);
	srand(seed);
	/* start the game */
	initMode();

	return 0;
}
