#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "game.h"
#include "MainAux.h"
#include "gurobi_c.h"

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


int ilpSolver(Game game){

	int dim = game.n*game.m;
	GRBenv *env = NULL;
	GRBmodel *model = NULL;
	int ind = (int*)calloc(dim, sizeof(int));
	double val = (double*)calloc(dim, sizeof(double));
	double lb = (double*)calloc(dim*dim*dim, sizeof(double));
	char vtype = (char*)calloc(dim*dim*dim, sizeof(char));
	char *names = (char**)calloc(dim*dim*dim, sizeof(char));
	char namestorage = (char*)malloc((dim+1)*dim*dim*dim, sizeof(char));
	char *cursor;
	int optimstatus;
	double objval;
	int i, j, v, ig, jg, count;
	int error = 0;

	/* create an empty model */
	cursor = namestorage;
	for(i = 0; i < dim; i++){
		for(j = 0; j < dim; j++){
			for(v = 1; v < dim+1; v++){
				if(game.board[i][j].value == v){
					lb[i*dim*dim+j*dim*dim+v] = 1;
				}
				else{
					lb[i*dim*dim+j*dim*dim+v] = 0;
				}
				vtype[i*dim*dim+j*dim*dim+v] = GRB_BINARY;
				names[i*dim*dim+j*dim*dim+v] = cursor;
				sprintf(names[i*dim*dim+j*dim*dim+v], "x_%d,%d,%d", i, j, v+1);
				cursor += strlen(names[i*dim*dim+j*dim*dim+v]+1);
			}
		}
	}
	/* create environment */
	error = GRBloadenv(&env, "sudoku.log");
	if (error) goto QUIT;
	/* create a new model */
	error = GRBnewmodel(env, &model, "sudoku", dim*dim*dim, NULL, lb, NULL, vtype, names);
	if (error) goto QUIT;
	/* each cell gets a value */
	for(i=0; i<dim; i++){
		for(i=0; i<dim; i++){
			for(i=0; i<dim; i++){
				ind[v] = i*dim*dim+j*dim*dim+v;
				val[v] = 1.0;
			}
			error = GRBaddconstr(model, dim, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) goto QUIT;
		}
	}
	/* each value must appear once in each row */
	for(i=0; i<dim; i++){
		for(i=0; i<dim; i++){
			for(i=0; i<dim; i++){
				ind[i] = i*dim*dim+j*dim*dim+v;
				val[i] = 1.0;
			}
			error = GRBaddconstr(model, dim, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) goto QUIT;
		}
	}
	/* each value must appear once in each col */
	for(i=0; i<dim; i++){
		for(i=0; i<dim; i++){
			for(i=0; i<dim; i++){
				ind[j] = i*dim*dim+j*dim*dim+v;
				val[j] = 1.0;
			}
			error = GRBaddconstr(model, dim, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) goto QUIT;
		}
	}
	/* each value must appear once in each subgrid */
	for(v=0; v<dim; v++){
		for(ig=0;  ig<game.n; ig++){
			for(jg=0; jg<game.m; jg++){
				count = 0;
				for(i= ig*game.m; i<(ig+1)*game.m; i++){
					for(j= jg*game.n; j<(jg+1)*game.n; i++){
						ind[count] = i*dim*dim+j*dim*dim+v;
						val[count] = 1.0;
					}
				}
			}
			error = GRBaddconstr(model, dim, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) goto QUIT;
		}
	}
	/* optimize model */
	error = GRBoptimize(model);
	if (error) goto QUIT;
	/* Write model to 'sudoku.lp' */
	error = GRBwrite(model, "sudoku.lp");
	if (error) goto QUIT;
	/* Capture solution information */
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) goto QUIT;
	error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
	if (error){
		exitILP();
	}
	/* if there is a solution to the game */
	if (optimstatus == GRB_OPTIMAL){
		/* free all calloc */
		free(ind);
		free(val);
		free(lb);
		free(vtype);
		free(names);
		free(namestorage);
		return 1;
	}
	/* if there is no solution to the game */
	else if (optimstatus == GRB_INF_OR_UNBD){
		/* free all calloc */
		free(ind);
		free(val);
		free(lb);
		free(vtype);
		free(names);
		free(namestorage);
		return 0;
	}
	/* if the optimization was stopped early and we didn't reach a solution*/
	else{
		/* free all calloc */
		free(ind);
		free(val);
		free(lb);
		free(vtype);
		free(names);
		free(namestorage);
		return 2;
	}
	/* Error reporting */
	QUIT:
	if (error) {
		printf("ERROR: %s\n", GRBgeterrormsg(env));
	    exit(1);
	}
	/* Free model */
	GRBfreemodel(model);
	/* Free environment */
	GRBfreeenv(env);
	/* free all calloc */
		free(ind);
		free(val);
		free(lb);
		free(vtype);
		free(names);
		free(namestorage);
	return 0;
}


void autofill(Game *game){
	int row, col, val, N = game->n*game ;
	if(isErroneous(game)){
		printf("ERROR: board is erroneous.\n");
		return;
	}
	/* find the optional values for every cell of the board */
	for(row = 0; row < N; row++){
		for(col = 0; col < N; col++){
			setOptionalValues(game->board, row, col);
		}
	}
	/* for all the cells that has only one value possible, autofill this value */
	for(row = 0; row < N; row++){
		for(col = 0; col < N; col++){
			if(game->board[row][col].numOfOptionalValues == 1){
				set(game, row, col, game->board[row][col].optionalValues[0], 0);
				printf("cell <%d,%d> was set to %d\n", row, col, game->board[row][col].optionalValues[0]);
			}
		}
	}
}




