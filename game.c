#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "MainAux.h"
#include "parser.h"
#include "solver.h"
#include "game.h"


#define SEP "----------------------------------\n"  /*separator for printBoard*/
#define ErrorCalloc "Error: calloc has faild\n" /*error warning if calloc fails*/

/* before exiting the game, we free the memory allocated to the board (2d array of Cell) */
void freeBoard(Game game){
	int j;
	/*in case we passed NULL as the board */
	if(!game)
		return;
	/* we free each calloc we made for the columns */
	for(j = 0; j< game.n*game.m; j++)
		free(game.board[j]);
	/* we free the calloc of the rows */
	free(game.board);
}

/* a command for exiting the game
 * we free the allocated memory using freeBoard and exiting */
void exitGame(Game game){
	freeBoard(game);
	printf("Exiting...\n");
	exit(0);
}

/* allocating memory for new board of 9*9 sudoku */
Cell ** allocateMemForStandardBoard(){
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

/* creating new move and add it to the moves list*/
void setMove(Game game, int row, int col, int value, int prevValue){
	Move *move = (Move *)calloc(1, sizeof(Move));
	move.row = row;
	move.col = col;
	move.value = value;
	move.prevValue = prevValue;
	move.lastMove = game.currentMove;
	move.nextMove = NULL;
	game.currentMove = move;
}

void clearNextMoves(Game game){
	Move nextMove = game.currentMove.nextMove;
	Move moveToClear = game.currentMove.nextMove;
	while(nextMove != Null){
		nextMove = moveToClear.nextMove;
		free(moveToClear);
		moveToClear = nextMove;
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

/* a command the user can put while playing to restart the game */
void reset(Game game){
	Move move;
	while(game.currentMove != Null){
		move = game.currentMove;
		undo(game, 0);
		free(move);
	}
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
void set(Game game, int row, int col, int value, int printSign){
	/* check the cell is not fixed */
	if(board[row-1][col-1].fixed ==1){
		printf("Error: cell is fixed\n");
		return;
	}
	/* set the value to the suitable cell and print the board */
	if((value == 0)||(isSafe(board, row-1, col-1, value) == 1)){
		clearNextMoves(game);
		setMove(game, row, col, value, board[row-1][col-1].value);
		board[row-1][col-1].value = value;
		if(printSign == 1){
			printBoard(board);
		}
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

/* start the game and interactively apply the users commands */
void initMode(){
	char input[256];
	int command[4] = {0};
	int *p = command;
	char strPath[256];
	char *path = str;
	/* scan the user commands till EOF */
	while (!feof(stdin)) {
		fflush(stdin);
		if (fgets(input, 256, stdin) != NULL) {
			parseUserInput(p, path, input);
			if (command[0] == 1 | command[0] == 2) /*edit or solve command*/
				controlGame(command, strPath)
			if(command[0] == 17) /*exit command*/
				exitMode();
			if(command[0] == 5) /*blank line */
				break;
			if(command[0] == 6){ /*otherwise */
				printf("Error: invalid command\n");
				break;
			}
		}
	}
	/* when reaching EOF, exit the game */
	exitMode();
}

/* start the game and interactively apply the users commands */
void controlGame(int[] commands, char[] strPath){
	char input[256];
	int command[4] = {0};
	int *p = command;
	char strPath[256];
	char *path = str;
	Game game = (Game*)calloc(1, sizeof(Game));

	if(commands[0] == 1){ /*solveMode*/
		if(commands[1] == 1) /* user didnt enter path */
			printf("Error: invalid command\n");
		else{
			game.modeNum = 1;
		}
	}

	if(commands[0] == 2){ /*editMode*/
		if(commands[1] == 1)
			/*should load a new board 9x9*/
		else{
		FILE* ifp = fopen(strPath, "r");
		/*should load an exsisint board*/
		}
		game.modeNum = 2;
	}
	/* scan the user commands till EOF */
	while (!feof(stdin)) {
		fflush(stdin);
		if (fgets(input, 1024, stdin) != NULL) {
			parseUserInput(p, path, input);
			switch (command[0]) {
			case 1: /*solve command */
				if(commands[1] == 1)
					printf("Error: invalid command\n");
				else
					game.modeNum = 1;
				break;
			case 2: /*edit command */
				if(commands[1] == 1)
					/*should load a new board 9x9*/
				else{
					FILE* ifp = fopen(strPath, "r");
					/*should load an exsiting board*/
				}
				game.modeNum = 2;
				break;
			case 3: /*mark_errors command*/
				if(game.modeNum = 2)
					mark_errors(game.board);
				else
					printf("Error: invalid command\n");
				break;
			case 4: /*printBoard command*/
				printBoard(game.board);
				break;
			case 5: /*set command */
				set(game.board);
				break;
			case 6: /*validate command*/
				validate(game.board);
				break;
			case 7: /*guess command*/
				if(game.modeNum = 2)
					generate(game.board);
				else
					/*printError*/
			case 8: /*generate command*/
				generate(game.board);
				break;
			case 9: /*undo command*/
				undo(game.board);
				break;
			case 10: /*redo command*/
				redo(game.board);
				break;
			case 11: /*save command*/
				save(game.board);
				break;
			case 12: /*hint command*/
				if(game.modeNum = 2)
					hint(command[2], command[1], game.board);
				else
					printf("Error: invalid command\n");
				break;
			case 13: /*guess_hint command*/
				if(game.modeNum = 2)
					hint(command[2], command[1], game.board);
				else
					printf("Error: invalid command\n");
				break;
			case 14: /*num_solutions command*/
				num_solutions(bogame.boardard);
				break;
			case 15: /*autofill command*/
				if(game.modeNum = 2)
					autofill(game.board);
				else
					printf("Error: invalid command\n");
				break;
			case 16: /*reset command*/
				reset(game.board);
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
	exitGame(board);
}

void solveMode(Game game){
	char input[1024];
	int command[4] = {0};
	int *p = command;
	char str[2048];
	char *path = str;
	Cell ** board = NULL;
	/* create a new board for the new game */
	board = createBoard(board);
	/* scan the user commands till EOF */
	while (!feof(stdin)) {
		fflush(stdin);
		if (fgets(input, 1024, stdin) != NULL) {
			parseUserInput(p,path, input);
			switch (command[0]) {
			case 0: /*set command */
				set(game, command[2], command[1], command[3], 1);
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






