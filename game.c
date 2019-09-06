#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "MainAux.h"
#include "parser.h"
#include "solver.h"
#include "game.h"


#define SEP "----------------------------------\n"  /*seParator for printBoard*/
#define ErrorCalloc "Error: calloc has faild\n" /*error warning if calloc fails*/

/* Game Module
 	• represents the current game status
		• The game status – i.e., the digits in each cell
		• Stored solution
	• Allow game status to change:
		• Apply next move
	• Provides information
		• Check if a certain move is valid
		• Print the current board
	• support the following functions:
		• setBoard - solving the board and fill numOfFilledCells cells
		• freeBoard - free the memory allocated
		• initialBoard - building a new board
		• createBoard - using setBoard and initialBoard
		• restart - starting a new game
		• validate - check if the board is solvable
		• exit - exit the game
		• hint - giving a hint for a specific cell in the board
		• set - set a cell
		• copyBoard - copy the current board
		• startGame - managing the game
		• endGame - manage the game after puzzle solved successfully
		• printBoard - printing the current board
 */




/* set fixed cell according to numOfFilledCells variable &
 * solving the board using nonDeterministic BackTracking algorithm */
void setBoard(Cell ** board, int numOfFilledCells){
	int j=0;
	int i = 0;
	int count = 0;
	int row=0;
	int col=0;
	/*fill the empty board with non deterministic back-tracking algorithm*/
	nonDeterministicBackTracking(board, 0, 0);
	/*save the values of the full board and clear the board*/
	for(row = 0; row < R*C; row++){
		for(col = 0; col < R*C; col++){
			board[row][col].savedValue = board[row][col].value;
			board[row][col].value = 0;
		}
	}
	/*set numOfFilledCells as fixed values*/
	while (count < numOfFilledCells) {
			j = rand()%(C*R);
			i = rand()%(C*R);
			if (board[i][j].fixed == 0) {
				count++;
				board[i][j].value = board[i][j].savedValue;
				board[i][j].fixed = 1;
			}
	}
}

/* before exiting the game, we free the memory allocated to the board (2d array of Cell) */
void freeBoard(Cell ** board){
	int j;
	/*in case we passed NULL as the board */
	if(!board)
		return;
	/* we free each calloc we made for the columns */
	for(j = 0; j< C*R; j++)
		free(board[j]);
	/* we free the calloc of the rows */
	free(board);
}

/* a command for exiting the game
 * we free the allocated memory using freeBoard and exiting */
void exitGame(Cell ** board){
	freeBoard(board);
	printf("Exiting...\n");
	exit(0);
}

/* allocating memory for new board of sudoku */
Cell ** initialBoard(){
	int j;
	/* allocating C*R*sizeOf(int) bytes for the board rows */
	Cell** board = (Cell **)calloc(R*C, sizeof(Cell*));
	if(board == NULL){
		printf(ErrorCalloc);
		exit(0);
	}
	/* allocating C*R*sizeOf(int) bytes for the board column in each row */
	for(j=0; j<R*C; ++j){
		board[j] = (Cell *)calloc(R*C, sizeof(Cell));
		if(board[j] == NULL){
			printf(ErrorCalloc);
			exit(0);
		}
	}
	return board;
}

/* create a new board, fill if and set #n cells according to the user decision */
Cell ** createBoard(){
	int scanResult;
	int numOfFilledCells = -1;
	/* allocate memory for the board */
	Cell ** board = initialBoard();
	/* get from the user the number of cells he wants to fix */
	printf("Please enter the number of cells to fill [0-80]:\n");
	while ((scanResult = scanf("%d", &numOfFilledCells)) == 1) {
		if (numOfFilledCells < 0 || numOfFilledCells >80){
			printf("Error: invalid number of cells to fill (should be between 0 and 80)\n");
			printf("Please enter the number of cells to fill [0-80]:\n");
		}
		else{
			/* fill the board and fix numOfFilledCells cells */
			setBoard(board, numOfFilledCells);
			printBoard(board);
			return board;
		}
	}
	/* in case the user entered wrong input for the numOfFilledCells */
	if (scanResult != EOF){
		printf("Error: not a number\n");
	}
	exitGame(board);
	return board;
}


/* a command the user can put while playing to restart the game */
void restart(Cell ** board){
	/* free the memory allocated to the board */
	freeBoard(board);
	/* start a new game */
	initMode();
}

/* start the game and interactively apply the users commands */
void initMode(){
	char input[1024];
	int command[4] = {0};
	int *p = command;
	char *path
	/* scan the user commands till EOF */
	while (!feof(stdin)) {
		fflush(stdin);
		if (fgets(input, 1024, stdin) != NULL) {
			parseUserInput(p, input);
			switch (command[0]) {
			case 1: /*solve command */
				solveMode(command[1]);
				break;
			case 2: /*edit command */
				editMode(command[1]);
				break;
			case 17: /*exit command*/
				exitMode();
				break;
			case 5: /*blank line */
				break;
			case 6: /*otherwise */
				printf("Error: invalid command\n");
				break;
			}
		}
	}
	/* when reaching EOF, exit the game */
	exitMode();
}


/* start the game and interactively apply the users commands */
void editMode(int commandType, FILE* ifp){
	char input[1024];
	int command[4] = {0};
	int *p = command;
	Cell ** board = NULL;
	/* create a new board for the new game */
	board = createBoard(board);
	/* scan the user commands till EOF */
	if()


	while (!feof(stdin)) {
		fflush(stdin);
		if (fgets(input, 1024, stdin) != NULL) {
			parseUserInput(p, input);
			switch (command[0]) {
			case 1: /*solve command */
				freeBoard(board);
				solve(command[2], command[1], command[3]);
				break;
			case 2: /*edit command */
				freeBoard(board);
				edit(command[2], command[1]);
				break;
			case 4: /*printBoard command*/
				printBoard(board);
				break;
			case 5: /*set command */
				set(board);
				break;
			case 6: /*validate command*/
				validate(board);
				break;
			case 8: /*generate command*/
				generate(board);
				break;
			case 9: /*undo command*/
				undo(board);
				break;
			case 10: /*redo command*/
				redo(board);
				break;
			case 11: /*save command*/
				save(board);
				break;
			case 14: /*num_solutions command*/
				num_solutions(board);
				break;
			case 16: /*reset command*/
				reset(board);
				break;
			case 17: /*exit command*/
				exitGame(board);
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
	exitGame(board);
}

void exitMode(){
	char input[1024];
	int command[4] = {0};
	int *p = command;
	Cell ** board = NULL;
	/* create a new board for the new game */
	board = createBoard(board);
	/* scan the user commands till EOF */
	while (!feof(stdin)) {
		fflush(stdin);
		if (fgets(input, 1024, stdin) != NULL) {
			parseUserInput(p, input);
			switch (command[0]) {
			case 0: /*set command */
				set(command[2], command[1], command[3], board);
				break;
			case 1: /*hint command */
				hint(command[2], command[1], board);
				break;
			case 2: /*validate command*/
				validate(board);
				break;
			case 3: /*restart command */
				restart(board);
				break;
			case 4: /*exit command*/
				exitGame(board);
				break;
			case 5: /*blank line */
				break;
			case 6: /*otherwise */
				printf("Error: invalid command\n");
				break;
			}
		}
	}
	/* when reaching EOF, exit the game */
	exitGame(board);
}

/* in case we end the game and filled all the board,
 * we can only exit or restart */
void endGame(Cell** board) {
	char input[1024];
	int command[4] = {0};
	int *p = command;
	while (!feof(stdin)) {
		fflush(stdin);
		if (fgets(input, 1024, stdin) != NULL) {
			parseUserInput(p, input);
			switch (command[0]) {
			case 0: /*set command */
				printf("Error: invalid command\n");
				break;
			case 1: /*hint command */
				printf("Error: invalid command\n");
				break;
			case 2: /*validate command*/
				printf("Error: invalid command\n");
				break;
			case 3: /*restart command */
				restart(board);
				break;
			case 4: /*exit command */
				exitGame(board);
				break;
			case 5: /*blank line */
				break;
			case 6:/*otherwise */
				printf("Error: invalid command\n");
				break;
			}
		}
	}
	/* when reaching EOF, exit the game */
	exitGame(board);
}

/* create a copy of the board by allocating new memory for new board and copy all the data */
Cell ** copyBoard(Cell** board){
	int row;
	int col;
	/* allocating C*R*sizeOf(int) bytes for the board rows */
	Cell** cpBaord = (Cell **)calloc(R*C, sizeof(Cell*));
	if(cpBaord == NULL){
		printf("%s",ErrorCalloc);
		freeBoard(board);
		exit(0);
	}
	for(row = 0; row < R*C; row++){
		/* allocating C*R*sizeOf(int) bytes for the board rows */
		cpBaord[row] = (Cell *)calloc(R*C, sizeof(Cell));
		if(cpBaord[row] == NULL){
			printf("%s",ErrorCalloc);
			freeBoard(board);
			exit(0);
		}
		/* copy the data from cell (row, col) of the original board to the new allocated board */
		for(col = 0; col < R*C; col++){
			cpBaord[row][col].value = board[row][col].value;
			if(cpBaord[row][col].value != 0)
				cpBaord[row][col].fixed = 1;
		}
	}
	return cpBaord;
}

/* print the board in the required format */
void printBoard(Cell** board){
	int row;
	int col;
	/* print each row*/
	for(row = 0; row<R*C; row++){
		/* before rows that are multiple of 3, print the separator row*/
		if(row%R ==0)
			printf(SEP);
		for(col = 0; col<R*C; col++){
			/* before cols that are multiple of 3, print the separator sign */
			if(col%C == 0)
				printf("| ");
			/* print the value of the cell according to if it is fixed or filled */
			if(board[row][col].fixed == 0){
				if(board[row][col].value != 0){
					printf(" %d ", board[row][col].value);
				}
				else{
					printf("   ");
				}
			}
			else{
				printf(".%d ", board[row][col].value);
			}
		}
		printf("|\n");
	}
	printf(SEP);
}

/* a command the user can put to set value to cell (row,col) */
void set(int row, int col, int value, Cell ** board){
	/* check the cell is not fixed */
	if(board[row-1][col-1].fixed ==1){
		printf("Error: cell is fixed\n");
		return;
	}
	/* set the value to the suitable cell and print the board */
	if((value == 0)||(isSafe(board, row-1, col-1, value) == 1)){
		board[row-1][col-1].value = value;
		printBoard(board);
	}
	else
		printf("Error: value is invalid\n");
	/* check if the board is full. if it is, end the game */
	if(findUnassignedLocation(board) == 0){
		printf("Puzzle solved successfully\n");
		endGame(board);
	}
}

/* a command the user can put to get a hint to a suitable value for cell (row,col)
 * we use the saves values from the board build to return the suitable value */
void hint(int row, int col, Cell ** board){
	printf("Hint: set cell to %d\n", board[row-1][col-1].savedValue);
}

/* a command the user can put to vlidate that the board is solvable
 * we use deterministic back-tracking to check if it is solvable*/
void validate(Cell ** board){
	/* create a copy of the board to pass to the deterministic back-tracking algorithm */
	Cell ** cpBaord = copyBoard(board);
	/* ran the algorithm and check if it succeeded */
	if (deterministicBackTracking(cpBaord, 0, 0) == 1){
		printf("Validation passed: board is solvable\n");
		updateStoredSolution(cpBaord, board);
	}
	else
		printf("Validation failed: board is unsolvable\n");
	/* free the memory allocated to the copy of the board */
	freeBoard(cpBaord);
}







