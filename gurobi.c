/* Source file which contains the functions that we use in order to use Gurobi for solving boards using ILP. Includes the following functions:
 * free_stuffs - A function that frees data allocated in the findSol function.
 * addConstraints - A function that adds the needed constraints for the model.
 * addVars - A function that adds the variables needed for the model.
 * findSol - A function that checks if we have a solution for the board provided and returns the solution to the board in the parameter sol.*/

#include <stdlib.h>
#include <stdio.h>
#include "game.h"
#include "gurobi.h"
#include "MainAux.h"

void free_stuffs(int* ind, double* val, double* obj, char* vtype) {
	/*Free the arrays of values needed for the calculation of the gurobi functions.Used upon finish or upon error*/
	free(ind);
	free(val);
	free(obj);
	free(vtype);
}

int addConstraints(int cols, int rows, int* ind, double* val, int* filled, int amountFilled, GRBenv *env, GRBmodel *model, double* obj, char* vtype) {
	/*Add the constraints of the ILP model to the model
	 INPUT: int cols, rows - Integers representing the amount of columns and rows in a single block in the board.
	 int *ind - Array that holds the variable indices of non-zero values in constraints.
	 double *val - Array that holds the values for non-zero values in constraints.
	 int *filled - An array holding the already filled cells in the board, each cell take 3 spaces, for column, row and value of the cell.
	 int amount Filled - The amount of already filled cells in the game board.
	 Other variables - Holds information of the model for gurobi such as the enviroment, the model itself, the type of the variables and the objective function.
	 OUTPUT: The function returns (-1) on error and (0) on success.*/
	int i, j, k, l, a, error;
	/*Only one number per cell constraints*/
	for (i = 0; i < cols * rows; i++) {
		for (j = 0; j < cols * rows; j++) {
			for (k = 0; k < cols * rows; k++) {
				ind[k] = i * cols * rows * cols * rows + j * cols * rows + k;
				val[k] = 1;
			}
			error = GRBaddconstr(model, cols * rows, ind, val, GRB_EQUAL, 1.0,
			NULL);/*constraint name is defaulted because we dont
			 care what it's name is*/
			if (error) {
				printf("ERROR %d in cell constraints GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				free_stuffs(ind, val, obj, vtype);
				return -1;
			}
		}
	}
	/*Same number only once per row constraints*/
	for (i = 0; i < cols * rows; i++) {
		for (k = 0; k < cols * rows; k++) {
			for (j = 0; j < cols * rows; j++) {
				ind[j] = i * cols * rows * cols * rows + j * cols * rows + k;
				val[j] = 1;
			}
			error = GRBaddconstr(model, cols * rows, ind, val, GRB_EQUAL, 1.0,
			NULL);/*constraint name is defaulted because we dont
			 care what it's name is*/
			if (error) {
				printf("ERROR %d in row constraints GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				free_stuffs(ind, val, obj, vtype);
				return -1;
			}
		}
	}
	/*Same number only once per col constraints*/
	for (j = 0; j < cols * rows; j++) {
		for (k = 0; k < cols * rows; k++) {
			for (i = 0; i < cols * rows; i++) {
				ind[i] = i * cols * rows * cols * rows + j * cols * rows + k;
				val[i] = 1;
			}
			error = GRBaddconstr(model, cols * rows, ind, val, GRB_EQUAL, 1.0,
			NULL);/*Constraint name is defaulted because we dont
			 care what it's name is*/
			if (error) {
				printf("ERROR %d in col constraints GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				free_stuffs(ind, val, obj, vtype);
				return -1;
			}
		}
	}
	/*Same number only one per block*/
	for (i = 0; i < cols; i++) {/*block row index*/
		for (j = 0; j < rows; j++) {/*block col index*/
			for (k = 0; k < rows * cols; k++) {/*cell number index*/
				for (l = 0; l < rows; l++) {/*cell row index*/
					for (a = 0; a < cols; a++) {/*cell col index*/
						ind[l * cols + a] = (i * rows + l) * cols * rows * cols * rows + (j * cols + a) * cols * rows + k;
						val[l * cols + a] = 1;
					}
				}
				error = GRBaddconstr(model, cols * rows, ind, val, GRB_EQUAL, 1.0, NULL);/*constraint name is defaulted because we dont
				 care what it's name is*/
				if (error) {
					printf("ERROR %d in block constraints GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
					free_stuffs(ind, val, obj, vtype);
					return -1;
				}
			}
		}
	}

	/*Cells already filled constraints*/

	for (i = 0; i < amountFilled; i++) {/*data is in col row val triplets*/
		ind[0] = filled[i * 3] * cols * rows + filled[(i * 3) + 1] * cols * rows * cols * rows + filled[(i * 3) + 2] - 1;/*+0 is the col,+1 is the row,+2 is the value, we do -1 since indexing start from 0 and the value starts from 1*/
		val[0] = 1;
		error = GRBaddconstr(model, 1, ind, val, GRB_EQUAL, 1.0, NULL);/*constraint name is defaulted because we dont
		 care what it's name is*/
		if (error) {
			printf("ERROR %d in filled constraints GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
			free_stuffs(ind, val, obj, vtype);
			return -1;
		}
	}
	return 0;
}

int addVars(int cols, int rows, int* ind, double* val, double* obj, char* vtype, GRBenv *env, GRBmodel *model) {
	/* Adds variables to the model and set the variables to be binary.
	 INPUT: int cols, rows - Integers representing the amount of columns and rows in a single block in the board.
	 int *ind - Array that holds the variable indices of non-zero values in constraints.
	 double *val - Array that holds the values for non-zero values in constraints.
	 int *filled - An array holding the already filled cells in the board, each cell take 3 spaces, for column, row and value of the cell.
	 int amount Filled - The amount of already filled cells in the game board.
	 Other variables - Holds information of the model for gurobi such as the enviroment, the model itself, the type of the variables and the objective function.
	 OUTPUT: The function returns (-1) on error and (0) on success.*/
	int i, j, k, error;
	/*Set the variables to be binary*/
	for (i = 0; i < cols * rows; i++) {
		for (j = 0; j < cols * rows; j++) {
			for (k = 0; k < cols * rows; k++) {
				vtype[i * cols * rows * cols * rows + j * cols * rows + k] =
				GRB_BINARY;
			}
		}
	}

	/* Add variables to model */
	error = GRBaddvars(model, cols * rows * cols * rows * cols * rows, 0, NULL,
	NULL, NULL, obj, NULL, NULL, vtype,
	NULL);
	if (error) {
		printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
		free_stuffs(ind, val, obj, vtype);
		return -1;
	}

	/* Change objective sense to maximization */
	error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if (error) {
		printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		free_stuffs(ind, val, obj, vtype);
		return -1;
	}

	/* Update the model - to integrate new variables */

	error = GRBupdatemodel(model);
	if (error) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		free_stuffs(ind, val, obj, vtype);
		return -1;
	}
	return 0;
}

int findSol(int cols, int rows, int* filled, int amountFilled, double* sol) {
	GRBenv *env = NULL;
	GRBmodel *model = NULL;
	int error = 0;
	int* ind;/*Which variable are used in each constraint*/
	double* val;/*The coefficients of the constraints*/
	double *obj = { 0 };
	char* vtype;/*What type the variable will be (all will be binary)*/
	int optimstatus;
	/*allocation space for the arrays*/
	ind = (int*) calloc(cols * rows, sizeof(int));
	checkInitalize(ind, "calloc");
	val = (double*) calloc(cols * rows, sizeof(double));
	checkInitalize(val, "calloc");
	vtype = (char*) calloc(cols * rows * cols * rows * cols * rows, sizeof(char));
	checkInitalize(vtype, "calloc");
	obj = (double*) calloc(cols * rows * cols * rows * cols * rows, sizeof(double));
	checkInitalize(obj, "calloc");
	checkAllocatedMemory(ind, "calloc");
	val = (double*) calloc(cols * rows, sizeof(double));
	checkAllocatedMemory(val, "calloc");
	vtype = (char*) calloc(cols * rows * cols * rows * cols * rows, sizeof(char));
	checkAllocatedMemory(vtype, "calloc");
	obj = (double*) calloc(cols * rows * cols * rows * cols * rows, sizeof(double));
	checkAllocatedMemory(obj, "calloc");
	/* Create environment - log file is mip1.log */
	error = GRBloadenv(&env, "mip1.log");
	if (error) {
		printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
		free_stuffs(ind, val, obj, vtype);
		return -1;
	}

	/*Cancel log being written to console*/
	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) {
		printf("ERROR %d GRBsetintparam(): %s\n", error, GRBgeterrormsg(env));
		free_stuffs(ind, val, obj, vtype);
		return -1;
	}

	/* Create an empty model named "mip1" */
	error = GRBnewmodel(env, &model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
	if (error) {
		printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
		free_stuffs(ind, val, obj, vtype);
		return -1;
	}
	/*Sets the variables to be binary type*/
	if (addVars(cols, rows, ind, val, obj, vtype, env, model))
		return -1;

	/*Adds the constraints of the model*/
	if (addConstraints(cols, rows, ind, val, filled, amountFilled, env, model, obj, vtype))
		return -1;

	/*  Optimize model - need to call this before calculation  */
	error = GRBoptimize(model);
	if (error) {
		printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
		free_stuffs(ind, val, obj, vtype);
		return -1;
	}

	/* Write model to 'mip1.lp' - this is not necessary but very helpful */
	error = GRBwrite(model, "mip1.lp");
	if (error) {
		printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
		free_stuffs(ind, val, obj, vtype);
		return -1;
	}

	/* Get solution information - the status of the model: infeasible or feasible,which means there isn't a solution or there is a solution*/
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
		free_stuffs(ind, val, obj, vtype);
		return -1;
	}
	if (optimstatus == GRB_INFEASIBLE) {/*Was there no solution?if so do this*/
		/* Free model and environment,and return that there was no solution to this board */
		GRBfreemodel(model);
		GRBfreeenv(env);
		free_stuffs(ind, val, obj, vtype);
		return 0;
	}
	/* Get the solution - the assignment to each variable */
	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, cols * rows * cols * rows * cols * rows, sol);
	if (error) {
		printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
		free_stuffs(ind, val, obj, vtype);
		return -1;
	}

	/* Free model and environment,and return that there was a solution to this board */
	GRBfreemodel(model);
	GRBfreeenv(env);
	free_stuffs(ind, val, obj, vtype);
	return 1;/*found solution,and it's stored in sol*/
}
