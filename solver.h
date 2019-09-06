#ifndef SOLVER_H_
#define SOLVER_H_
#include "game.h"



/* Takes a partially filled-in grid and attempts to assign values to
  all unassigned locations in a deterministic way (from 1 to 9), to meet the
  requirements for Sudoku solution (non-duplication across rows, columns, and boxes) */
int deterministicBackTracking(Cell** board, int row, int col);


/* Takes a partially filled-in grid and attempts to assign values to
  all unassigned locations in a non-deterministic way (choosing between all the possible
  options randomly), to meet the requirements for Sudoku solution
  */
int nonDeterministicBackTracking(Cell** board, int row, int col);




#endif /* SOLVER_H_ */
