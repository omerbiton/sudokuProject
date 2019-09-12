
/* Header file which contains the functions that we use in order to use Gurobi for solving boards using ILP. Including the following functions:
 * findSol - The function needs the amount of columns and rows that are in a block in the current game, an array representing all the currently
 *           filled cells in the board, the number of cells that are filled in the board, and a pointer to a double array that will hold the
 *           solution found for the board. The format of the filled cells should be 3 spaces for each cell, representing the column, row and value in the cell.
 *           The function will update the double array with the solution if any was found, and will return (1) if a solution was found, (0) if
 *           no solution was found, or (-1) if an error occurred (and an appropriate message will be printed). No changes will be made to the game board.*/

#ifndef GUROBIFUNC_H_
#define GUROBIFUNC_H_

int findSol(int cols, int rows, int* filled, int amountFilled, double* sol);
/*The function fills the board for ILP functions that need it, and keeps the solution in the sol array provided.
 * INPUT: int cols, rows - Integers representing the amount of columns and rows in a single block in the board.
 *        int *filled - An array holding the already filled cells in the board, each cell take 3 spaces, for column, row and value of the cell.
 *        int amount Filled - The amount of already filled cells in the game board.
 *        double *sol - An array that will hold The values of the cells in the solution for the board. For every cell there will be rows*cols spaces in the array allocated to it.
 *                      The value of the cell in the solution will be its index from the rows*cols options.
 OUTPUT: The function returns (1) if there was a solution to the board, (0) if there wasn't and (-1) if an error occured.*/

#endif /* GUROBIFUNC_H_ */
