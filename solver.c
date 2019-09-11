#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "game.h"
#include "MainAux.h"


/* This module implements the Backtrack algorithms.
 * it contains one deterministic and one non-deterministic implementation
 * of the back-tracking algorithm to solve a semi-full (or empty) board
 */






/* Takes a partially filled-in grid and attempts to assign values to
  all unassigned locations in a deterministic way (from 1 to 9), to meet the
  requirements for Sudoku solution (non-duplication across rows, columns, and boxes) */
int deterministicBackTracking(Cell** board, int row, int col){
	int value = 0;
	int nextIteration =0;

	/* If there is no unassigned location, we are done */
	if (row == C*R && col == 0){
		return 1; /* success! */
	}
	/* if the cell is fixed or already filled, go to the next cell */
	if(board[row][col].value != 0 || board[row][col].fixed == 1){
		if(col<(C*R-1)){
			nextIteration = deterministicBackTracking(board, row, col+1);
		}
		else if(col == C*R-1){
			nextIteration = deterministicBackTracking(board, row+1, 0);
		}
		if (nextIteration == 1){
			return 1;
		}
		else{
			return 0;
		}
	}
	else{
		/* set all the possible assignments to the cell (row,col) */
		setOptionalValues(board, row, col);
		/* as long as there are optional values to the cell: */
		while (board[row][col].numOfOptionalValues > 0){
			/* try to assign number (by order) to the current cell */
			value = board[row][col].optionalValues[0];
			board[row][col].value = value;
			fixOpptions(board, row, col, 0);

			/* and go to the next cell */
			if(col<(C*R-1)){
				nextIteration = deterministicBackTracking(board, row, col+1);
			}
			else if(col == C*R-1){
				nextIteration = deterministicBackTracking(board, row+1, 0);
			}
			/* if the next allocations worked and this one worked, return 1*/
			if (nextIteration == 1){
				return 1;
			}
			else{
			/* failure, unmake and move to the next number */
				board[row][col].value = 0;
			}
		}
		return 0;
	}
}

/* Takes a partially filled-in grid and attempts to assign values to
  all unassigned locations in a non-deterministic way (choosing between all the possible
  options randomly), to meet the requirements for Sudoku solution */
int nonDeterministicBackTracking(Cell** board, int row, int col){
    int value = 0;
    int chosenIndex = 0;
    int nextIteration;

    /* If there is no unassigned location, we are done */
    if (row == C*R && col == 0){
    	return 1; /* success! */
    }

    /* set all the possible assignments to the cell (row,col) */
    setOptionalValues(board, row, col);

    /* as long as there are optional values to the cell: */
    while (board[row][col].numOfOptionalValues > 0){
        /* if there is only one value option, assign it to the cell */
    	if(board[row][col].numOfOptionalValues == 1){
   	    	value = board[row][col].optionalValues[0];
    	}
    	else {
    		/* try to assign number (randomly chosen) to the current cell */
    		chosenIndex = rand()%board[row][col].numOfOptionalValues;
    		value = board[row][col].optionalValues[chosenIndex];
    	}
		board[row][col].value = value;
		fixOpptions(board, row, col, chosenIndex);

		/* and go to the next cell */
		if(col<(C*R-1)){
			nextIteration = nonDeterministicBackTracking(board, row, col+1);
		}
		else if(col == C*R-1){
			nextIteration = nonDeterministicBackTracking(board, row+1, 0);
		}
		/* if the next allocations worked and this one worked, return 1 */
		if (nextIteration == 1){
			return 1;
		}
		else{
		/* failure, unmake and move to the next number */
			board[row][col].value = 0;
		}
    }
    return 0;
}

