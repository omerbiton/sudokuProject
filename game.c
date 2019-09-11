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
void freeGame(Game *game){
	/*in case we passed NULL as the board */
	if(!game)
		return;
	freeBoard(game);
	clearNextMoves(game);
	clearPrevMoves(game);
	/* we free the game */
	free(game);
}
void freeBoard(Game *game){
	int j;
	/* we free each calloc we made for the columns */
	for(j = 0; j< game.n*game.m; j++)
		free(game.board[j]);
	/* we free the calloc of the rows */
	free(game.board);
}

/* a command for exiting the game
 * we free the allocated memory using freeBoard and exiting */
void exitGame(Game *game){
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
Cell ** createBoard(Game *game){
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

/* creating new move and add it to the moves list*/
void setMove(Game *game, int row, int col, int value, int prevValue){
	Move move = (Move *)calloc(1, sizeof(Move));
	move.row = row;
	move.col = col;
	move.value = value;
	move.prevValue = prevValue;
	move.lastMove = game.currentMove;
	move.nextMove = NULL;
	game.currentMove = move;
}

void clearNextMoves(Game *game){
	Move nextMove = game.currentMove.nextMove;
	Move moveToClear = game.currentMove.nextMove;
	while(nextMove != NULL){
		nextMove = moveToClear.nextMove;
		free(moveToClear);
		moveToClear = nextMove;
	}
}
void clearPrevMoves(Game *game){
	Move moveToClear = game.currentMove.prev;
	Move prevMove = game.currentMove.prev;
	while(moveToClear != NULL){
		prevMove = moveToClear.prevMove;
		free(moveToClear);
		moveToClear = nextMove;
	}
	free(game.currentMove);
}

void undo(Game *game, int printSign){
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

/* a command the user can put while playing to restart the game */
void reset(Game game){
	Move move;
	while(game.currentMove != NULL){
		move = game.currentMove;
		undo(game, 0);
		free(move);
	}
}




/* a command the user can put to get a hint to a suitable value for cell (row,col)
 * we use the saves values from the board build to return the suitable value */
void hint(int x, int y, gameState *metaBoard, Game *game) {
	int rows = metaBoard->rows, cols = metaBoard->cols;
	double *sol;
	int* filledCells;
	int numOfFilled = 0;
	int val, solved;
	if (game->board[x][y].fixed)
		printf("Error: cell is fixed\n");
	else if (game->board[x][y].value)
		printf("Error: cell already contains a value\n");
	else {
		sol = (double*) calloc(cols * rows * cols * rows * cols * rows, sizeof(double));
		checkAllocatedMemory(sol, "calloc");
		filledCells = findFilledCells(game, &numOfFilled);/*Get the already filled cells from the board*/
		solved = findSol(cols, rows, filledCells, numOfFilled, sol);
		if (solved > 0) {/*Solution was found, we can give a hint*/
			val = findval(sol, x, y, cols, rows);
			printf("Hint: set cell to %d\n", val);
		} else if (!solved) {
			printf("Error: board is unsolvable\n");/*solved is 0 here so board is unsolveable*/
		}/*If we didn't enter the conditions above, we had an error in the Gurobi library and a message was printed*/
		free(filledCells);
		free(sol);
	}
}

int* findFilledCells(Game *game, int *numOfFilled) {
	/*The function finds all the filled cells in the game board and return them in an array such that every cells take 3 spaces, x,y, and the value in this cell.
	 * INPUT: gameState *metaBoard - A pointer to a gameState with allocated board and with valid values.
	 *        int *amountFilled - A pointer to an integer that will be updated with the amount of filled cells the function have found.
	 * OUTPUT: An int array with all the filled cells found in the game board, each cell uses 3 spaces in the format x,y, and the current value inside the cell.*/
	int *filled = { 0 };
	int i, j;
	int N = game->m * game->n;
	for (i = 0; i < N; i++) {
		for (j = 0; N; j++) {
			if (game->board[j][i].value != 0) {
				filled = (int*) realloc(filled, ((*numOfFilled) + 1) * 3 * sizeof(int));
				checkAllocatedMemory(filled, "realloc");
				filled[(*numOfFilled) * 3] = j;
				filled[(*numOfFilled) * 3 + 1] = i;
				filled[(*numOfFilled) * 3 + 2] = game->board[j][i].value;
				(*numOfFilled)++;
			}
		}
	}
	return filled;
}

int validate(Game game, int printSign){
	int ilpSolverRes;
	if(isErrorneous(game)){
		if (printSign){
			printf("The board is erroneous.\n")
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
	int N = game.n*game.m;
	/* for each value from 1 to N, check there is no multiplicity */
	for(val = 1; val <= game.n*game.n; val++){
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
		for(i = 0; i < game.n; i++){
			for(j = 0; j < game.m; j++){
				if(instancesInBox(game, i*game.m, j*game.n, val) >1){
					errorMark = 1;
				}
			}
		}
	}
	return errorMark;
}




void printBoard(Game *game) {
	int i, j;
	Cell ** board = game->board;
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


/* a command the user can put to set value to cell (row,col) */
void set(Game *game, int row, int col, int value, int printSign){
	int isSafe;
	int prevValue = game.board[row][col].value;
	/* check the cell is not fixed */
	if(game.board[row][col].fixed  == 1 && game.mode == solve){
		printf("Error: cell is fixed\n");
		return;
	}
	if(game.board[row][col].value  == value){
		return;
	}
	clearNextMoves(game);
	setMove(game, row, col, value, game.board[row-1][col-1].value);
	game.board[row][col].value = value;

	if(game.board[row][col].error == 1)
		isSafe = isSafe(game.board, row-1, col-1, prevValue);
	isSafe = isSafe(game.board, row-1, col-1, value);

	if(printSign == 1){
		printBoard(game);
	}

	/* check if the board is full. if it is, end the game */
	if(findUnassignedLocation(game.board) == 0){
		printf("Puzzle solved successfully\n");
		endGame(game.board);
	}
}

/* a command the user can put to get a hint to a suitable value for cell (row,col)
 * we use the saves values from the board build to return the suitable value */
void hint(int row, int col, Cell ** board){
	printf("Hint: set cell to %d\n", board[row-1][col-1].savedValue);
}


void mark_errors(int markErrorNum, int* error){
	if(markErrorNum == 0 | markErrorNum == 1)
		*error = markErrorNum;
	else
		printf("Error: mark_errors can get only 0 or 1\n");
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
				if(game.mode == solve)
					mark_errors(command[1], error);
				else
					printf("Error: invalid command\n");
				break;
			case printBoard:
				if(game.mode != init)
					printBoard(game);
				else
					printf("Error: invalid command\n");
				break;
			case set:
				if(game.mode != init)
					set(game);
				break;
			case validate:
				if(game.mode != init)
					validate(game);
				else
					printf("Error: invalid command\n");
				break;
			case guess:
				if(game.mode == solve)
					guess(game);
				else
					printf("Error: invalid command\n");
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
					save(game);
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

