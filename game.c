#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "MainAux.h"
#include "parser.h"
#include "solver.h"
#include "game.h"


#define SEP "----------------------------------\n"  /*separator for printBoard*/
#define ErrorCalloc "Error: calloc has failed\n" /*error warning if calloc fails*/

/* before exiting the game, we free the memory allocated to the board (2d array of Cell) */
void freeGame(Game* game){
	/*in case we passed NULL as the board */
	if(!game)
		return;
	freeBoard(game);
	clearNextMoves(game);
	clearPrevMoves(game);
	/* we free the game */
	free(game);
}
void freeBoard(Game* game){
	int j;
	/* we free each calloc we made for the columns */
	for(j = 0; j< game.n*game.m; j++)
		free(game.board[j]);
	/* we free the calloc of the rows */
	free(game.board);
}

/* a command for exiting the game
 * we free the allocated memory using freeBoard and exiting */
void exitGame(Game* game){
	freeGame(game);
	printf("Exiting...\n");
	exit(0);
}

Game* createGame(){
	Game *game = (Game*)calloc(1, sizeof(Game));
	game.n = 3;
	game.m = 3;
	game.board = createBoard(game);
	game.mode = 0;
}

/* create a new board */
Cell ** createBoard(Game* game){
	int n = game.n;
	int m = game.m;
	int j;
	/* allocating n*m*sizeOf(int) bytes for the board rows */
	Cell** board = (Cell **)calloc(n*m, sizeof(Cell*));
	if(board == NULL){
		printf(ErrorCalloc);
		exit(0);
	}
	/* allocating n*m*sizeOf(int) bytes for the board column in each row */
	for(j=0; j<n*m; ++j){
		board[j] = (Cell *)calloc(n*m, sizeof(Cell));
		if(board[j] == NULL){
			printf(ErrorCalloc);
			exit(0);
		}
	}
	return board;
}

/* a command the user can put while playing to restart the game */
void reset(Game* game){
	Move move;
	while(game->currentMove != NULL){
		move = game->currentMove;
		undo(game, 0);
		free(move);
	}
}

/* print the board in the required format */
void printBoard(Game* game){
	int n = game.n;
	int m = game.m;
	int N = n*m;
	int row;
	int col;
    while(doubleNM != 0)
    {
        // n = n/10
    	doubleNM /= 10;
        ++maxSpacePerCell;
    }
	/* print each row*/
	for(row = 0; row<n*m; row++){
		/* before rows that are multiple of 3, print the separator row*/
		if(row%n ==0)
			printf(SEP);
		for(col = 0; col<n*m; col++){
			/* before cols that are multiple of 3, print the separator sign */
			if(col%m == 0)
				printf("| ");
			/* print the value of the cell according to if it is fixed or filled */
			if(game->board[row][col].fixed == 0){
				if(game->board[row][col].value != 0){
					printf(" %d ", board[row][col].value);
				}
				else{
					printf("   ");
				}
			}
			else{
				if(game->mode == 2) /*ignore fixed in edit mode*/
					printf(" %d ", game->board[row][col].value);
				else
					printf("%d. ", game->board[row][col].value);
			}
		}
		printf("|\n");
	}
	printf(SEP);
}

/* a command the user can put to set value to cell (row,col) */
void set(Game *game, int row, int col, int value, int printSign){
	/* check the cell is not fixed */
	if(game->board[row-1][col-1].fixed ==1){
		printf("Error: cell is fixed\n");
		return;
	}
	/* set the value to the suitable cell and print the board */
	if((value == 0)||(isSafe(game.board, row-1, col-1, value) == 1)){
		clearNextMoves(game);
		setMove(game, row, col, value, game->board[row-1][col-1].value);
		game->board[row-1][col-1].value = value;
		if(printSign == 1){
			printBoard(game->board);
		}
	}
	else
		printf("Error: value is invalid\n");
	/* check if the board is full. if it is, end the game */
	if(findUnassignedLocation(game.board) == 0){
		printf("Puzzle solved successfully\n");
		endGame(game.board);
	}
}


int validate(Game *game, int printSign){
	int ilpSolverRes;
	if(isErrorneous(game)){
		if (printSign){
			printf("The board is errorneous.\n")
		}
		return 0;
	}
	else{
		ilpSolverRes = ilpSolver(game);
		if(ilpSolverRes == 1){
			if(printSign){
				printf("The board is valid and solvable, you may continue.\n");
			}
			return 1;
		}
		else{
			if(printSign){
				printf("The board is not solvable.\n");
			}
			return 0;
		}
	}
}

int isErrorneous(Game *game){
	int row, col, i, j, val, errorMark = 0;
	int N = game->n*game->m;
	/* for each value from 1 to N, check there is no multiplicity */
	for(val = 1; val <= game->n*game->n; val++){
		for(row = 0; row < N, row++){
			if(instancesInRow(game, row, val) > 1){
				errorMark = 1;
			}
		}
		for(col = 0; col < N, col++){
			if(instancesInCol(game, col, val) > 1){
				errorMark = 1;
			}
		}
		for(i = 0; i < game->n; i++){
			for(j = 0; j < game->m; j++){
				if(instancesInBox(game, i*game->m, j*game->n, val) >1){
					errorMark = 1;
				}
			}
		}
	}
	return errorMark;
}

void mark_errors(int markErrorNum, int* error){
	if(markErrorNum == 0 | markErrorNum == 1)
		*error = markErrorNum;
	else
		printf("Error: mark_errors can get only 0 or 1\n");
}

void generate(Game *game, int x, int y){
	int row, col, val, N = game->n*game->m, i;
	/* if the board doesn't contain x empty cells */
	if(N*N-game->numOfFilledCells < x){
		pfintf("Error: the board does nor contain %d empty cells.\n", x);
		return;
	}
	/* fill x cells in the board. if there's a problem exit function */
	if(fillXCells(game, x) == 0){
		return;
	}
	filledCells = (int*) cealloc(game->numOfFilledCells * 3, sizeof(int));
	if(filledCelles == NULL){
		printf("ERROR: memory allocation error.\n");
		return NULL;
	}
	sol = (double*) calloc(N*N*N, sizeof(double));
	if(filledCelles == NULL){
		printf("ERROR: memory allocation error.\n");
		return NULL;
	}
	findFilledCells(game, filledCells);/* fills the filledCells array with the data of the game filled cells */
	solved = findSol(game->n, game->m, filledCells, game->numOfFilledCells, sol);
	/* if the board is unsolvable */
	if(!solved){
		/****************************************/
	}
	/* put the solution of the ILP in the game-board */
	for(i=0; i<N*N*N; ii=++){
		if(sol[i]==1){
			col = i/(N*N);
			row = (i-(col*N*N))/N;
			val = ((i-col*N*N)-row*N);
			game->board[row][col].value = val;
		}
	}
	clearFixedSigns(game, 1);
	/* choose Y cells to keep */
	for(i=0; i < y; i++){
		row = rand()%N;
		col = rand()%N;
		game->board[row][col].fixed = 1;
	}
	/* clear the rest of the cells */
	clearFixedSign(game, 3);
}

int fillXCells(Game *game, int x){
	int row, col, val, i=0, j, N = game->n*game->m, counter = 0;
	while(i<x){
		if(counter >= 1000){
			printf("ERROR: error in the puzzle generator, can't execute the operation\n");
			return 0;
		}
		row = rand()%N;
		col = rand()%N;
		/* if the cell is free (has 0 as value) */
		/* set all the possible assignments to the cell (row,col) */
		setOptionalValues(game->board, row, col);
		if(game->board[row][col].numOfOptionalValues > 0){
			if(game->board[row][col].numOfOptionalValues == 1){
				val = game->board[row][col].optionalValues[0];
			}
			else{
				j = rand()%game->board[row][col].numOfOptionalValues;
				val = game->board[row][col].optionalValues[j];
			}
			game->board[row][col].value = val;
			game->board[row][col].fixed = 2;
			i++;
		}
		/* if we chose a cell that has no optional value, restart the all process*/
		else {
			clearFixedSigns(game, 2);
			i=0;
			counter++;
		}
	}
	return 1;
}

/* goes throw all the board cells
 * case fixedNum == 1 : unfix the cell and set the fix field to 0
 * case fixedNum == 2 : clear the cell, set the value and the fix fields to 0
 * case fixedNum == 3 : set the value of the unfixed cells to 0*/
void clearFixedSigns(Game *game, int fixedNum){
	int row, col;
	for(row = 0; row < game->n; row++ ){
		for(col = 0; col < game->m; col++ ){
			if(game->board[row][col].fixed ==1 && fixedNum != 0){
				game->board[row][col].fixed = 0;
			}
			else if(game->board[row][col].fixed == 2 && fixedNum == 2){
				game->board[row][col].value = 0;
				game->board[row][col].fixed = 0;
			}
			else if(fixedNum == 3){
				if(game->board[row][col].fixed = 0){
					game->board[row][col].value = 0;
				}
			}

		}
	}
}

/*The function finds all the filled cells in the game board and return them in an array such that every cells take 3 spaces, x,y, and the value in this cell.
	 * INPUT: Game *game - A pointer to the game.
	 *        int *filledCells - A pointer to an int array in size 3*numOfFilledCells to put the filled cells of the game
	 * OUTPUT: An int array with all the filled cells found in the game board, each cell uses 3 spaces in the format x,y, and the current value inside the cell.*/
int* findFilledCells(Game *game, int *filledCells) {
	int N = game->m * game->n;

	for (row = 0; row < N; row++) {
		for (col = 0; col < N; col++) {
			if (game->board[j][i].value != 0) {
				filledCells[index * 3] = row;
				filledCells[index * 3 + 1] = col;
				filledCells[index * 3 + 2] = game->board[row][col].value;
			}
		}
	}
	return filled;
}

/*The function extract the value that should be in the cell <x,y> according to the solution found with the ILP module that is contained in sol.
	 * INPUT: double *sol - A double array in the size of (cols * rows)^3, where the solution found by the ILP is stored. Function should run only if the solution is valid.
	 *        int x,y - Integers representing the column and row of the cell we want to know it's value according to the solution sol.
	 *        int cols, rows - Integers representing the amount of columns and rows in each block of the game board.
	 * OUTPUT: An integer which should be in the cell <x,y> according to the solution sol.*/
int findval(double *sol, int x, int y, Game *game) {
	int i, N = game->n*game->m;
	for (i = 0; i < N; i++) {
		if (sol[x*N + y*N* N + i])/*If the location has 1 then the value in the cell in the valid board is the location-1*/
			return i + 1;
	}
	return -1;/*Value not found, there was no valid solution, shouldn't happen since the function runs only if a valid solution was found*/
}

/* a command the user can put to get a hint to a suitable value for cell (row,col)
 * we use the saves values from the board build to return the suitable value */
void hint(Game* game , int x , int y){
	double *sol;
	int *filledCells;
	int val, solved, N;
	N = game->n*game->m;
	/* allocate an */
	if(isErroneous(game)){
		printf("ERROR: board is erroneous.\n");
		return;
	}
	if(game->board[x][y].fixed){
		printf("ERROR: cell is fixed.\n");
		return;
	}
	if(game->board[x][y].value != 0){
		printf("ERROR: cell already contains a value.\n");
		return;
	}
	filledCells = (int*) cealloc(game->numOfFilledCells * 3, sizeof(int));
	if(filledCelles == NULL){
		printf("ERROR: memory allocation error.\n");
		return NULL;
	}
	sol = (double*) calloc(N*N*N, sizeof(double));
	if(filledCelles == NULL){
		printf("ERROR: memory allocation error.\n");
		return NULL;
	}
	findFilledCells(game, filledCells);/* fills the filledCells array with the data of the game filled cells */
	solved = findSol(game->n, game->m, filledCells, game->numOfFilledCells, sol);
	if (solved > 0) {/*Solution was found, we can give a hint*/
		val = findval(sol, x, y, cols, rows);
		printf("Hint: set cell to %d\n", val);
	} else if (!solved) {
		printf("Error: board is unsolvable\n");/*solved is 0 here so board is unsolveable*/
	}/*If we didn't enter the conditions above, we had an error in the Gurobi library and a message was printed*/
	free(filledCells);
	free(sol);
}

/* start the game and interactively apply the users commands */
void gameControl(){
	char input[256];
	int command[4] = {0};
	int markErrors = 1;
	int* error = &markErrors;
	int *p = command;
	char strPath[256];
	char *path = strPath;
	Game game =  createGame();
	/* scan the user commands till EOF */
	while (!feof(stdin)) {
		fflush(stdin);
		if (fgets(input, 1024, stdin) != NULL) {
			parseUserInput(p, path, input);
			switch (command[0]) {
			case 1: /*solve command */
				if(commands[1] == 1)
					printf("Error: invalid command, have to enter a path\n");
				else{
					/*should load an existing board*/
					game.mode=1;
				}
				break;
			case 2: /*edit command */
				if(markErrors == 0)
					markErrors = 1;
				if(commands[1] == 1){
					/*should load a new board 9x9*/
				}
				else{
					/*should load an existing board*/
				}
				game.mode=2;
				break;
			case 3: /*mark_errors command*/
				if(game.mode == 1)
					mark_errors(command[1], error);
				else
					printf("Error: invalid command\n");
				break;
			case 4: /*printBoard command*/
				if(game.mode != 0)
					printBoard(game);
				else
					printf("Error: invalid command\n");
				break;
			case 5: /*set command */
				if(game.mode != 0)
					set(game.board);
				break;
			case 6: /*validate command*/
				if(game.mode != 0)
					validate(game.board);
				else
					printf("Error: invalid command\n");
				break;
			case 7: /*guess command*/
				if(game.mode == 1)
					guess(game.board);
				else
					printf("Error: invalid command\n");
			case 8: /*generate command*/
				if(game.mode == 2)
					generate(game.board);
				else
					printf("Error: invalid command\n");
				break;
			case 9: /*undo command*/
				if(game.mode != 0)
					undo(game.board);
				else
					printf("Error: invalid command\n");
				break;
			case 10: /*redo command*/
				if(game.mode != 0)
					redo(game.board);
				else
					printf("Error: invalid command\n");
				break;
			case 11: /*save command*/
				if(game.mode != 0)
					save(game.board);
				else
					printf("Error: invalid command\n");
				break;
			case 12: /*hint command*/
				if(game.mode == 2)
					hint(command[2], command[1], game.board);
				else
					printf("Error: invalid command\n");
				break;
			case 13: /*guess_hint command*/
				if(game.mode == 2)
					hint(command[2], command[1], game.board);
				else
					printf("Error: invalid command\n");
				break;
			case 14: /*num_solutions command*/
				if(game.mode != 0)
					num_solutions(game.board);
				else
					printf("Error: invalid command\n");
				break;
			case 15: /*autofill command*/
				if(game.mode == 2)
					autofill(game.board);
				else
					printf("Error: invalid command\n");
				break;
			case 16: /*reset command*/
				if(game.mode != 0)
					reset(game.board);
				else
					printf("Error: invalid command\n");
				break;
			case 17: /*exit command*/
				exitGame(game.board);
				break;
			case 18: /*blank line */
				break;
			case 19: /*otherwise */
				printf("Error: invalid command\n");
				break;
			}
		}
	}
	/* when reaching EOF, exit the game */
	exitGame(game.board);
}

