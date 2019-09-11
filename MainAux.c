#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"

#define R 3
#define C 3

/* MainAux Module
 	• The auxiliary functions are placed inside this module. Those are functions that
		do not belong to any of the other modules.
	• support the following functions:
		• usedInRow - check if number in row
		• usedInCol - check if number in column
		• usedInBox - check if number in box
		• isSafe - check if we can put a number in cell
		• setOptionalValues - set all the optinal values for a cell
		• fixOpptions - Removes from the optional values array the element in index 'chosenIndex'
		• updateStoredSolution - update the saved options of the board in case the user would ask for a hint


*/

/* Returns 1 if an assigned entry in the specified row matches the given number
 * or 0 else */
int instancesInRow(Game *game, int row, int value){
	int col = 0;
	int numOfInstances = 0;
	int* errorCells = (int*)calloc(game->n*game->m, sizeof(int));
	/* go over all the columns of the row and count how many times value appears */
    for (col = 0; col < game->n*game->m; col++) {
        if (game->board[row][col].value == value){
        	errorCells[col] = 1;
        	game->board[row][col].error = 0;
        	numOfInstances++;
        }
    }
    if(numOfInstances > 1 ){
    	for(col = 0; col < game->n*game->m, col++){
    		if(errorCells[col] == 1){
    			game->board[row][col].error = 1;
    		}
    	}
    }
    free(errorCells);
    return numOfInstances;
}

/* Returns 1 if an assigned entry in the specified col matches the given number
 * or 0 else */
int instancesInCol(Game game, int col, int value){
	int row,  numOfInstances = 0;
	int errorCells = (int*) calloc(game.n*game.m, sizeof(int));
	/* go over all the columns of the row and count how many times value appears */
    for (row = 0; row < game.n*game.m; row++){
        if (board[row][col].value == value){
        	errorCells[row] = 1;
			board[row][col].error = 0;
			numOfInstances++;
        }
    }
    if(numOfInstances > 1 ){
		for(row = 0; row < game.n*game.m, row++){
			if(errorCells[row] == 1){
				game.board[row][col].error = 1;
			}
		}
	}
    free(errorCells);
    return numOfInstances;
}

/* Returns 1 if an assigned entry in the specified 3x3 box matches the given number
 * or 0 else */
int instancesInBox(Game game, int boxStartRow, int boxStartCol, int value){
	int row, col, i, numOfInstances = 0;
	int errorCells = (int*) calloc(game.n*game.m, sizeof(int));
	/* iterate all the cells in the box and counts how many time value appears*/
    for (row = 0; row < game.n; row++){
        for (col = 0; col < game.m; col++){
            if (board[row+boxStartRow][col+boxStartCol].value == value){
            	errorCells[row*game.m+col] = 1;
            	board[row+boxStartRow][col+boxStartCol].error = 0;
            	numOfInstances++;
            }
        }
    }
    if(numOfInstances > 1 ){
   		for(i = 0; i < game.n*game.m, i++){
   			if(errorCells[i] == 1){
   				game.board[(i/game.m)+boxStartRow][(i%game.m)boxStartCol].error = 1;
   			}
   		}
   	}
    free(errorCells);
    return numOfInstances;
}

/* Returns 1 if it will be legal to assign num to the given row,col location
 * or 0 if not */
int isSafe(Game game, int row, int col, int val){
    /* Check if 'val' is not already placed in current row,
       current column and current 3x3 box */
    return instancesInRow(game, row, val) == 0 &&
    		instancesInCol(game, col, val) == 0 &&
			instancesInBox(game, row - row%R , col - col%C, val) == 0;
}

void checkAllocatedMemory(void *pointer, char *cmd) {
	if (pointer == NULL) {
		printf("Error: %s has failed (memory allocation error)\n", cmd);
		exit(0);
	}
}


/* fill the array of the optional values of cell (row,col)
 * and save the number of optional values */
void setOptionalValues(Cell** board, int row, int col){
	int index = 0;
	int num;
	/* check if its safe to put each of the values between 1 to 9 in cell (row,col)
	 * if it is add this value to the optionalValues array */
	for(num = 1; num <= (C*R); num++){
		if (isSafe(board, row, col, num) != 0){
			board[row][col].optionalValues[index] = num;
			index++;
		}
	}
	/* save the number of optional values */
	board[row][col].numOfOptionalValues = index;
}

/* Removes from the optional values array the element in index 'chosenIndex'
 * and decrease by 1 the number of optional values */
void fixOpptions(Cell** board, int row, int col, int chosenIndex){
	/* promote each value in the indexes passing the chosen index by 1*/
	for(; chosenIndex <= board[row][col].numOfOptionalValues; chosenIndex++){
		board[row][col].optionalValues[chosenIndex] = board[row][col].optionalValues[(chosenIndex+1)];
	}
	/* decrease by 1 the number of optional values */
	board[row][col].numOfOptionalValues-=1;
}



/* Searches the grid to find an entry that is still unassigned (left to right and head
 * to bottom. If found, 1 is returned. If no unassigned entries remain, 0 is returned. */
int findUnassignedLocation(Cell** board){
	int row;
	int col;
	for (row =0; row < C*R; row++){
	        for (col = 0; col < C*R; col++){
	            if ((board[row][col].fixed == 0) && (board[row][col].value == 0)){
	            	return 1;
	            }
	        }
	}
	return 0;
}

/* after running a deterministic back-tracking algorithm to validate if the board is solvable,
 * update the saved options of the board in case the user would ask for a hint */
void updateStoredSolution(Cell **cpBaord, Cell **board){
	int row;
	int col;
	for(row = 0; row < R*C; row++){
		for(col = 0; col < R*C; col++){
			/* save in each cell in the board the value of the new solution in the saved value field */
			board[row][col].savedValue = cpBaord[row][col].value;
		}
	}
}

