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

void freeGRBdata(int* ind, double* val, double* obj, char* vtype) {
	/*Free the arrays of values needed for the calculation of the gurobi functions.Used upon finish or upon error*/
	free(ind);
	free(val);
	free(obj);
	free(vtype);
}

int addConstraints(int m, int n, int* ind, double* val, int* filled, int amountFilled, GRBenv *env, GRBmodel *model, double* obj, char* vtype) {
	/*Add the constraints of the ILP model to the model
	 INPUT: int cols, rows - Integers representing the amount of columns and rows in a single block in the board.
	 int *ind - Array that holds the variable indices of non-zero values in constraints.
	 double *val - Array that holds the values for non-zero values in constraints.
	 int *filled - An array holding the already filled cells in the board, each cell take 3 spaces, for column, row and value of the cell.
	 int amount Filled - The amount of already filled cells in the game board.
	 Other variables - Holds information of the model for gurobi such as the enviroment, the model itself, the type of the variables and the objective function.
	 OUTPUT: The function returns (-1) on error and (0) on success.*/
	int col, row, value, i, j, error, N;
	N = n*m;
	/*Only one number per cell constraints*/
	for (col = 0; col < N; col++) {
		for (row = 0; row < N; row++) {
			for (value = 0; value < N; value++) {
				ind[value] = col * N * N + row * N + value;
				val[value] = 1;
			}
			error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
			/*constraint name is defaulted because we don't care what it's name is*/
			if (error) {
				printf("ERROR %d in cell constraints GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				freeGRBdata(ind, val, obj, vtype);
				return -1;
			}
		}
	}
	/*Same number only once per row constraints*/
	for (col = 0; col < N; col++) {
		for (value = 0; value < N; value++) {
			for (row = 0; row < N; row++) {
				ind[row] = col * N * N + row * N + value;
				val[row] = 1;
			}
			error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
			/*constraint name is defaulted because we don't care what it's name is*/
			if (error) {
				printf("ERROR %d in row constraints GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				freeGRBdata(ind, val, obj, vtype);
				return -1;
			}
		}
	}
	/*Same number only once per col constraints*/
	for (row = 0; row < N; row++) {
		for (value = 0; value < N; value++) {
			for (col = 0; col < N; col++) {
				ind[col] = col * N * N + row * N + value;
				val[col] = 1;
			}
			error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
			/*Constraint name is defaulted because we don't care what it's name is*/
			if (error) {
				printf("ERROR %d in col constraints GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				freeGRBdata(ind, val, obj, vtype);
				return -1;
			}
		}
	}
	/*Same number only one per block*/
	for (col = 0; col < m; col++) {/*block row index*/
		for (row = 0; row < n; row++) {/*block col index*/
			for (value = 0; value < n * m; value++) {/*cell number index*/
				for (i = 0; i < n; i++) {/*cell row index*/
					for (j = 0; j < m; j++) {/*cell col index*/
						ind[i * m + j] = (col * n + i) * N * N + (row * m + j) * N + value;
						val[i * m + j] = 1;
					}
				}
				error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
				/*constraint name is defaulted because we don't care what it's name is*/
				if (error) {
					printf("ERROR %d in block constraints GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
					freeGRBdata(ind, val, obj, vtype);
					return -1;
				}
			}
		}
	}
	/*Cells already filled constraints*/
	for (i = 0; col < amountFilled; i++) {/*data is in col row val triplets*/
		/*+0 is the col,+1 is the row,+2 is the value, we do -1 since indexing start from 0 and the value starts from 1*/
		ind[0] = filled[col * 3] * N + filled[(col * 3) + 1] * N * N + filled[(col * 3) + 2] - 1;
		val[0] = 1;
		error = GRBaddconstr(model, 1, ind, val, GRB_EQUAL, 1.0, NULL);
		/*constraint name is defaulted because we don't care what it's name is*/
		if (error) {
			printf("ERROR %d in filled constraints GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
			freeGRBdata(ind, val, obj, vtype);
			return -1;
		}
	}
	return 0;
}

int addVars(int m, int n, int* ind, double* val, double* obj, char* vtype, GRBenv *env, GRBmodel *model) {
	/* Adds variables to the model and set the variables to be binary.
	 INPUT: int cols, rows - Integers representing the amount of columns and rows in a single block in the board.
	 int *ind - Array that holds the variable indices of non-zero values in constraints.
	 double *val - Array that holds the values for non-zero values in constraints.
	 int *filled - An array holding the already filled cells in the board, each cell take 3 spaces, for column, row and value of the cell.
	 int amount Filled - The amount of already filled cells in the game board.
	 Other variables - Holds information of the model for gurobi such as the enviroment, the model itself, the type of the variables and the objective function.
	 OUTPUT: The function returns (-1) on error and (0) on success.*/
	int col, row, value, error, N;
	N = n*m;
	/*Set the variables to be binary*/
	for (col = 0; col < N; col++) {
		for (row = 0; row < N; row++) {
			for (value = 0; value < N; value++) {
				vtype[col * N * N + row * N + value] = GRB_BINARY;
			}
		}
	}
	/* Add variables to model */
	error = GRBaddvars(model, N * N * N, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
	if (error) {
		printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
		freeGRBdata(ind, val, obj, vtype);
		return -1;
	}
	/* Change objective sense to maximization */
	error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if (error) {
		printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		freeGRBdata(ind, val, obj, vtype);
		return -1;
	}
	/* Update the model - to integrate new variables */
	error = GRBupdatemodel(model);
	if (error) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		freeGRBdata(ind, val, obj, vtype);
		return -1;
	}
	return 0;
}

int findSol(int m, int n, int* filled, int amountFilled, double* sol) {
	GRBenv *env = NULL;
	GRBmodel *model = NULL;
	int N, error = 0;
	int* ind; /*Which variable are used in each constraint*/
	double* val; /*The coefficients of the constraints*/
	double *obj = { 0 };
	char* vtype; /*What type the variable will be (all will be binary)*/
	int optimstatus;
	N = n*m;
	/*allocation space for the arrays*/
	ind = (int*) calloc(N, sizeof(int));
	if(ind == NULL){
		printf("ERROR in calloc memory for the gurobi function.\n");
		return -1;
	}
	val = (double*) calloc(N, sizeof(double));
	if(val == NULL){
		printf("ERROR in calloc memory for the gurobi function.\n");
		free(ind);
		return -1;
	}
	vtype = (char*) calloc(N * N * N, sizeof(char));
	if(vtype == NULL){
		printf("ERROR in calloc memory for the gurobi function.\n");
		free(ind);
		free(val);
		return -1;
	}
	obj = (double*) calloc(N * N * N, sizeof(double));
	if(obj == NULL){
		printf("ERROR in calloc memory for the gurobi function.\n");
		free(ind);
		free(val);
		free(vtype);
		return -1;
	}
	/* Create environment - log file is mip1.log */
	error = GRBloadenv(&env, "mip1.log");
	if (error) {
		printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
		freeGRBdata(ind, val, obj, vtype);
		return -1;
	}

	/*Cancel log being written to console*/
	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) {
		printf("ERROR %d GRBsetintparam(): %s\n", error, GRBgeterrormsg(env));
		freeGRBdata(ind, val, obj, vtype);
		return -1;
	}

	/* Create an empty model named "mip1" */
	error = GRBnewmodel(env, &model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
	if (error) {
		printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
		freeGRBdata(ind, val, obj, vtype);
		return -1;
	}
	/*Sets the variables to be binary type*/
	if (addVars(m, n, ind, val, obj, vtype, env, model))
		return -1;

	/*Adds the constraints of the model*/
	if (addConstraints(m, n, ind, val, filled, amountFilled, env, model, obj, vtype))
		return -1;

	/*  Optimize model - need to call this before calculation  */
	error = GRBoptimize(model);
	if (error) {
		printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
		freeGRBdata(ind, val, obj, vtype);
		return -1;
	}

	/* Write model to 'mip1.lp' - this is not necessary but very helpful */
	error = GRBwrite(model, "mip1.lp");
	if (error) {
		printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
		freeGRBdata(ind, val, obj, vtype);
		return -1;
	}

	/* Get solution information - the status of the model: infeasible or feasible,which means there isn't a solution or there is a solution*/
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
		freeGRBdata(ind, val, obj, vtype);
		return -1;
	}
	if (optimstatus == GRB_INFEASIBLE) {/*Was there no solution?if so do this*/
		/* Free model and environment,and return that there was no solution to this board */
		GRBfreemodel(model);
		GRBfreeenv(env);
		freeGRBdata(ind, val, obj, vtype);
		return 0;
	}
	/* Get the solution - the assignment to each variable */
	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, N * N * N, sol);
	if (error) {
		printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
		freeGRBdata(ind, val, obj, vtype);
		return -1;
	}

	/* Free model and environment,and return that there was a solution to this board */
	GRBfreemodel(model);
	GRBfreeenv(env);
	freeGRBdata(ind, val, obj, vtype);
	return 1;/*found solution,and it's stored in sol*/
}
