#ifndef MAINAUX_H_
#define MAINAUX_H_
#include "game.h"

#define R 3
#define C 3

/* after running a deterministic back-tracking algorithm to validate if the board is solvable,
 * update the saved options of the board in case the user would ask for a hint */
void updateStoredSolution(Cell **cpBaord, Cell **board);

/* Returns 1 if an assigned entry in the specified row matches the given number
 * or 0 else */
int usedInRow(Cell** board, int row, int num);

/* Returns 1 if an assigned entry in the specified col matches the given number
 * or 0 else */
int usedInCol(Cell** board, int col, int num);

/* Returns 1 if an assigned entry in the specified 3x3 box matches the given number
 * or 0 else */
int usedInBox(Cell** board, int boxStartRow, int boxStartCol, int num);

/* Searches the grid to find an entry that is still unassigned (left to right and head
 * to bottom. If found, 1 is returned. If no unassigned entries remain, 0 is returned. */
int findUnassignedLocation(Cell** board);



/* Returns 1 if it will be legal to assign num to the given row,col location
 * or 0 if not */
int isSafe(Cell** board, int row, int col, int num);

/* fill the array of the optional values of cell (row,col)
 * and save the number of optional values */
void setOptionalValues(Cell** board, int row, int col);

/* Removes from the optional values array the element in index 'chosenIndex'
 * and decrease by 1 the number of optional values */
void fixOpptions(Cell** board, int row, int col, int chosenIndex);

#endif
