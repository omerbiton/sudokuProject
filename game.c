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
void freeGame(Game game){
	/*in case we passed NULL as the board */
	if(!game)
		return;
	freeBoard(game);
	clearNextMoves(game);
	clearPrevMoves(game);
	/* we free the game */
	free(game);
}
void freeBoard(Game game){
	int j;
	/* we free each calloc we made for the columns */
	for(j = 0; j< game.n*game.m; j++)
		free(game.board[j]);
	/* we free the calloc of the rows */
	free(game.board);
}

/* a command for exiting the game
 * we free the allocated memory using freeBoard and exiting */
void exitGame(Game game){
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
Cell ** createBoard(Game game){
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
void setMove(Game game, int row, int col, int value, int prevValue){
	Move move = (Move *)calloc(1, sizeof(Move));
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
	while(nextMove != NULL){
		nextMove = moveToClear.nextMove;
		free(moveToClear);
		moveToClear = nextMove;
	}
}
void clearPrevMoves(Game game){
	Move moveToClear = game.currentMove.prev;
	Move prevMove = game.currentMove.prev;
	while(moveToClear != NULL){
		prevMove = moveToClear.prevMove;
		free(moveToClear);
		moveToClear = nextMove;
	}
	free(game.currentMove);
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
	while(game.currentMove != NULL){
		move = game.currentMove;
		undo(game, 0);
		free(move);
	}
}


int loadBoard(Game* game, char* filePath){
	File *file;
	char mStr[20], nStr[20], numStr[20];
	int n, m, N, num, row=0, col=0, numOfFilledCells = 0 , numOfEmptyCells = 0, i = 0, fixedSign = 0;
	file = fopen(filePath, "r");
	if(file == NULL){ /* make sure we succeeded opening the file */
		return(0);
	}
	/* read the first two nums from the file and check it fits */
	if(fscanf(file, "%20s %20s", nStr, mStr) != 2 ){
		ErrorIncorectFormat();
		return(0);
	}
	else{
		n = atoi(nStr);
		m = atoi(mStr);
		if(n <= 0 || m <= 0){
			ErrorIncorectFormat();
			return(0);
		}
		N = n*m;
		else { /* read the file until you reach its end or you filled all the board */
			while(input != EOF || numOfFilledCells+numOfEmptyCells > N){
				/* initialize the array saving the next number and the fixedSign */
				for(i = 0; i<20 ; i++){
					numStr[i] = NULL;
				}
				fixedSign = 0;
				/* scan the next number to put in the board */
				fscanf(file, "%20s", numStr);
				/* case the next number is zero (so it's an empty cell and can't be fixed) */
				if(numStr == "0"){
					num = 0;
					/* increase the number of empty cells we filled */
					numOfEmptyCells++;
				}
				else{ /* check if the number we read is legal and if it has dot following it*/
					while(numStr[numLen] != NULL){
						numLen++;
					}
					if(numStr[numLen-1] == '.'){
						numStr[numLen-1] = NULL;
						fixedSign = 1;
					}
					num = atoi(numStr);
					if(num <= 0 || num > N){
						ErrorIncorectFormat();
						return(0);
					}
					else{
						numOfFilledCells++;
					}
				}
				game.board[row][col].value = num;
				if(fixedSign == 1){
					game.board[row][col].fixed = 1;
				}
			}
			/* in case the number of cells wasn't suitable to the board dimensions */
			if(numOfFilledCells+numOfEmptyCells != N){
				ErrorIncorectFormat();
				clearBoard(game);
				return(0);
			}
		}
		game->numOfFilledCells = numOfFilledCells;

	}
	fclose(filePath);

}

void saveBoard(Game game, char* filePath){
	int row, col, val;
	File *file;
	file = fopen(filePath, "w");
	if(game.mode == edit){
		if(validate(game, 0) == 0){
			Error();
			return;
		}
	}
	fprintf(file, "%d %d\n", game);
	for(row=0; row<game.n ;row++){
		for(col=0; col<game.m ;col++){
			fprintf(file, "%d", game.board[row][col].value);
			if(game.board[row][col].fixed == 1 || game.mode == edit){
				fprintf(file, ".");
			}
			if(col<game.m-1){
				fprintf(file, " ");
			}
		}
		fprintf(file, "\n");
	}
	fclose(file);
}




int validate(Game game, int printSign);

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

/* start the game and interactively apply the users commands
void initMode(){
	char input[256];
	int command[4] = {0};
	int *p = command;
	char strPath[256];
	char *path = strPath;
	 scan the user commands till EOF
	while (!feof(stdin)) {
		fflush(stdin);
		if (fgets(input, 256, stdin) != NULL) {
			parseUserInput(p, path, input);
			if (command[0] == 1 | command[0] == 2){ edit or solve command
				if(command[0] == 1 && commands[1] == 1){  user didn't enter path in solveMode
					printf("Error: invalid command\n");
					break;
				}
				controlGame(command, strPath);
			}
			if(command[0] == 17) exit command
				exitMode();
			if(command[0] == 5) blank line
				break;
			if(command[0] == 6){ otherwise
				printf("Error: invalid command\n");
				break;
			}
		}
	}
	 when reaching EOF, exit the game
	exitMode();
}*/

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








