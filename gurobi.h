
/* Header file which contains the functions that we use in order to use Gurobi for solving boards using ILP. Including the following functions:
 * findSol - The function needs the amount of columns and rows that are in a block in the current game, an array representing all the currently
 *           filled cells in the board, the number of cells that are filled in the board, and a pointer to a double array that will hold the
 *           solution found for the board. The format of the filled cells should be 3 spaces for each cell, representing the column, row and value in the cell.
 *           The function will update the double array with the solution if any was found, and will return (1) if a solution was found, (0) if
 *           no solution was found, or (-1) if an error occurred (and an appropriate message will be printed). No changes will be made to the game board.*/

#ifndef GUROBIFUNC_H_
#define GUROBIFUNC_H_

void freeGRBdata(int* ind, double* val, double* obj, char* vtype);

int addConstraints(int m, int n, int* ind, double* val, int* filled, int amountFilled, GRBenv *env, GRBmodel *model, double* obj, char* vtype);

int addVars(int m, int n, int* ind, double* val, double* obj, char* vtype, GRBenv *env, GRBmodel *model);

int findSol(int m, int n, int* filled, int amountFilled, double* sol);


#endif /* GUROBIFUNC_H_ */
