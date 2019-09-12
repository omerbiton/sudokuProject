<<<<<<< HEAD
=======
<<<<<<< HEAD
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "MainAux.h"
#include "parser.h"
#include "solver.h"
#include "game.h"



/* creating new move and add it to the moves list*/
void setMove(Game* game, int row, int col, int value, int prevValue){
	Move move = (Move *)calloc(1, sizeof(Move));
	move.row = row;
	move.col = col;
	move.value = value;
	move.prevValue = prevValue;
	move.lastMove = game.currentMove;
	move.nextMove = NULL;
	game.currentMove = move;
}

void clearNextMoves(Game game){
	Move nextMove = game.currentMove.nextMove;
	Move moveToClear = game.currentMove.nextMove;
	while(nextMove != NULL){
		nextMove = moveToClear.nextMove;
		free(moveToClear);
		moveToClear = nextMove;
	}
}
void clearPrevMoves(Game game){
	Move moveToClear = game.currentMove.prev;
	Move prevMove = game.currentMove.prev;
	while(moveToClear != NULL){
		prevMove = moveToClear.prevMove;
		free(moveToClear);
		moveToClear = nextMove;
	}
	free(game.currentMove);
}

void undo(Game game, int printSign){
	/* if there was no move done yet */
	if(game.currentMove == NULL){
		printf(ErrorUndo);
	}
	else{
		int row = game.currentMove.row;
		int col = game.currentMove.col;
		int value = game.currentMove.prevValue;
		set(game, row, col, prevValue, printSign);
		game.currentMove = game.currentMove.lastMove;
	}
}

void redo(Game game, int printSign){
	if(game.currentMove.nextMove == NULL){
		printf(ErrorRedo);
	}
	else{
		int row = game.currentMove.nextMove.row;
		int col = game.currentMove.nextMove.col;
		int value = game.currentMove.nextMove.value;
		set(game, row, col, prevValue, printSign);
		game.currentMove = game.currentMove.nextMove;
	}
}


