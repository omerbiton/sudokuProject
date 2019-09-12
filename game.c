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
void reset(Game game){
	Move move;
	while(game.currentMove != NULL){
		move = game.currentMove;
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
void set(Game game, int row, int col, int value, int printSign){
	/* check the cell is not fixed */
	if(game.board[row-1][col-1].fixed ==1){
		printf("Error: cell is fixed\n");
		return;
	}
	/* set the value to the suitable cell and print the board */
	if((value == 0)||(isSafe(game.board, row-1, col-1, value) == 1)){
		clearNextMoves(game);
		setMove(game, row, col, value, game.board[row-1][col-1].value);
		game.board[row-1][col-1].value = value;
		if(printSign == 1){
			printBoard(game.board);
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

/* a command the user can put to get a hint to a suitable value for cell (row,col)
 * we use the saves values from the board build to return the suitable value */
void hint(int row, int col, Cell ** board){
	printf("Hint: set cell to %d\n", board[row-1][col-1].savedValue);
}


int validate(Game game, int printSign){
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

int isErrorneous(Game game){
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

void mark_errors(int markErrorNum, int* error){
	if(markErrorNum == 0 | markErrorNum == 1)
		*error = markErrorNum;
	else
		printf("Error: mark_errors can get only 0 or 1\n");
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

