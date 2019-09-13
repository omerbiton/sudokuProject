#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "MainAux.h"
#include "parser.h"
#include "solver.h"
#include "game.h"

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
	checkAllocatedMemory("calloc", game);
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
	checkAllocatedMemory("calloc", board);
	if(board == NULL){
		printf(ErrorCalloc);
		exit(0);
	}
	/* allocating n*m*sizeOf(int) bytes for the board column in each row */
	for(j=0; j<n*m; ++j){
		board[j] = (Cell *)calloc(n*m, sizeof(Cell));
		checkAllocatedMemory("calloc", board[j]);
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
void printBoard(Game *game) {
	int i, j;
	if(game.mode == init){
		printf("Error: invalid command\n");
	}else{

		for (i = 0; i < game->m * game->n; i++) {
			if (i % game->rows == 0) {
				for (j = 0; j < game->n * game->m * 4 + game->n + 1; j++) {
					printf("-");
				}
				printf("\n");
			}
			for (j = 0; j < game->n * game->m; j++) {
				if (j == 0) {
					printf("|");
				}
				printf(" ");
				if (game->board[j][i].value == 0) {
					printf("   ");
				} else {
					printf("%2d", game->board[j][i].value);
					if (game->board[j][i].fixed)
						printf(".");
					else if ((game->board[j][i].error) && ((game->markErrors) || (game->mode == edit))) /*should add error check for the cell*/
						printf("*");
					else
						printf(" ");
				}
				if (j % game->m == game->n - 1)
					printf("|");
			}
			printf("\n");
		}
		for (j = 0; j < game->n * game->m * 4 + game->n + 1; j++) {
			printf("-");
		}
		printf("\n");
	}
}

/* a command the user can put to set value to cell (row,col) */
void set(Game *game, int row, int col, int value, int printSign){
	if(game.mode == init){
		printf("Error: invalid command\n");
	}else{
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
}


int validate(Game *game, int printSign){
	int ilpSolverRes;
	if(game.mode == init){
		printf("Error: invalid command\n");
	}
	else{
		if(isErrorneous(game)){
			if (printSign){
				printf("The board is errorneous.\n");
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
}
void undo(Game game, int printSign){
	/* if there was no move done yet */
	if(game.currentMove == NULL){
		printf(ErrorUndo);
	}
	else{
		int row = game.currentMove.row;
		int col = game.currentMove.col;
		int value = game.currentMove.prevValue;
		set(game, row, col, prevValue, printSign);
		game.currentMove = game.currentMove.lastMove;
	}
}

void redo(Game game, int printSign){
	if(game.currentMove.nextMove == NULL){
		printf(ErrorRedo);
	}
	else{
		int row = game.currentMove.nextMove.row;
		int col = game.currentMove.nextMove.col;
		int value = game.currentMove.nextMove.value;
		set(game, row, col, prevValue, printSign);
		game.currentMove = game.currentMove.nextMove;
	}
}

int isErrorneous(Game *game){
	int row, col, i, j, val;
	int N = game->n*game->m;
	/* for each value from 1 to N, check if there is multiplicity in each row, col, box */
	for(val = 1; val <= game->n*game->m; val++){
		for(row = 0; row < N; row++){
			if(instancesInRow(game, row, val) > 1){
				return 1;
			}
		}
		for(col = 0; col < N; col++){
			if(instancesInCol(game, col, val) > 1){
				return 1;
			}
		}
		for(i = 0; i < game->n; i++){
			for(j = 0; j < game->m; j++){
				if(instancesInBox(game, i*game->m, j*game->n, val) >1){
					return 1;
				}
			}
		}
	}
	return 0;
}

void mark_errors(int markErrorNum, Game* game){
	if(game.mode != solve)
		printf("Error: invalid command\n");
	else if(markErrorNum == 0 | markErrorNum == 1)
		game.markErrors = markErrorNum;
	else
		printf("Error: mark_errors can get only 0 or 1\n");
}

void generate(Game *game, int x, int y){
	int row, col, val, N = game->n*game->m, i;
	if(N*N-game->numOfFilledCells < x){ 	/* if the board doesn't contain x empty cells */
		printf("Error: the board does not contain %d empty cells.\n", x);
		return;
	}
	if(fillXCells(game, x) == 0){ 	/* fill x cells in the board. if there's a problem exit function */
		return;
	}
	filledCells = (int*)calloc(game->numOfFilledCells * 3, sizeof(int));
	checkAllocatedMemory("calloc", filledCells);

	sol = (double*)calloc(N*N*N, sizeof(double));
	checkAllocatedMemory("calloc", sol);

	findFilledCells(game, filledCells); /* fills the filledCells array with the data of the game filled cells */
	solved = findSol(game->n, game->m, filledCells, game->numOfFilledCells, sol);
	if(!solved){/* if the board is unsolvable */
		/**/
	}

	for(i=0; i < N*N*N; i++){ /* put the solution of the ILP in the game-board */
		if(sol[i] == 1){
			col = i/(N*N);
			row = (i-(col*N*N))/N;
			val = ((i-col*N*N)-row*N);
			game->board[row][col].value = val;
		}
	}
	clearFixedSigns(game, 1);
	for(i=0; i < y; i++){	/* choose Y cells to keep */
		row = rand()%N;
		col = rand()%N;
		game->board[row][col].fixed = 1;
	}
	clearFixedSign(game, 3);	/* clear the rest of the cells */
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

int* findFilledCells(Game *game, int *numOfFilled) {
	/*The function finds all the filled cells in the game board and return them in an array such that every cells take 3 spaces, x,y, and the value in this cell.
	 * INPUT: gameState *metaBoard - A pointer to a gameState with allocated board and with valid values.
	 *        int *amountFilled - A pointer to an integer that will be updated with the amount of filled cells the function have found.
	 * OUTPUT: An int array with all the filled cells found in the game board, each cell uses 3 spaces in the format x,y, and the current value inside the cell.*/
	int *filled = { 0 };
	int row, col;
	int N = game->m * game->n;
	for (row = 0; row < N; row++) {
		for (col = 0; N; col++) {
			if (game->board[row][col].value != 0) {
				filled = (int*) realloc(filled, ((*numOfFilled) + 1) * 3 * sizeof(int));
				checkAllocatedMemory(filled, "realloc");
				filled[(*numOfFilled) * 3] = row;
				filled[(*numOfFilled) * 3 + 1] = col;
				filled[(*numOfFilled) * 3 + 2] = game->board[row][col].value;
				(*numOfFilled)++;
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
	filledCells = (int*) calloc(game->numOfFilledCells * 3, sizeof(int));
	checkAllocatedMemory("calloc", filledCells);
	sol = (double*)calloc(N*N*N, sizeof(double));
	checkAllocatedMemory("calloc", sol);
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
void solve(int command[], Game *game, char *path){
	int success;
	if(commands[1] == 1)/*no path in command*/
		printf("Error: invalid command, have to enter a path\n");
	else{
		success = loadBoard(game, path);
		if(success == 1)
			game.mode = solve;
	}
}
void edit(int command[], Game *game, char *path){
	int success;
	if(commands[1] == 1) /*no path in command*/
		createBoard(game);
	else{
		success = loadBoard(game, path);
		if(success == 1)
			game.mode = edit;
	}
}
void save(int command[],Game *game, char *path) {
	if(commands[1] == 1){/*no path in command*/
		printf("Error: invalid command, have to enter a path\n");
		return 0;
	}
	if (game->mode == edit) {
		if (isErroneous(game)) {
			printf("Error: board contains erroneous values\n");
			return;
		} else if (!validate(game)) {
			printf("Error: board validation failed\n");
			return;
		}
	}
	saveToFile(game, path);
}
void guess(Game * game, int threshold){
	if(game.mode != solve){
		printf("Error: invalid command\n");
	}
	else{
}
int num_solutions(Game* game){
	int numSolutions;
	if(game.mode == init)
		printf("Error: invalid command\n");
	else if(isErrorneous(game))
		printf("Error: board contains erroneous values\n");
	else{
		numSolutions = backTracking(game);
		return numSolutions;
	}
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
			case solve:
				solve(p, game, path);
				break;
			case edit:
				edit(p, game, path);
				break;
			case mark_errors:
				mark_errors(command[1], error);
				break;
			case printBoard:
				printBoard(game);
				break;
			case set:
				set(game);
				break;
			case validate:
				validate(game);
				break;
			case guess:
				guess(game, command[1]);
				break;
			case generate:
				if(game.mode == edit)
					generate(game);
				else
					printf("Error: invalid command\n");
				break;
			case undo:
				if(game.mode != init)
					undo(game);
				else
					printf("Error: invalid command\n");
				break;
			case redo:
				if(game.mode != init)
					redo(game);
				else
					printf("Error: invalid command\n");
				break;
			case save:
				if(game.mode != init)
					save(game, path);
				else
					printf("Error: invalid command\n");
				break;
			case hint:
				if(game.mode == edit)
					hint(command[2], command[1], game);
				else
					printf("Error: invalid command\n");
				break;
			case guess_hint:
				if(game.mode == edit)
					hint(command[2], command[1], game);
				else
					printf("Error: invalid command\n");
				break;
			case num_solutions:
				if(game.mode != init)
					num_solutions(game);
				else
					printf("Error: invalid command\n");
				break;
			case autofill:
				if(game.mode == edit)
					autofill(game);
				else
					printf("Error: invalid command\n");
				break;
			case reset:
				if(game.mode != init)
					reset(game);
				else
					printf("Error: invalid command\n");
				break;
			case exit:
				exitGame(game);
				break;
			case blank_line:
				break;
			case otherwise:
				printf("Error: invalid command\n");
				break;
			}
		}
	}
	/* when reaching EOF, exit the game */
	exitGame(game.board);
}

