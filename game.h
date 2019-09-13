#ifndef GAME_H_
#define GAME_H_

/* define a struct representing a cell in the sudoku board*/
typedef struct Cell{
	int value; /* the value we put in the cell */
	int fixed ; /* 1 or 0 if the cell is fixed or not (accordingly) */
	int savedValue; /* the value of the cell in the initialization of the game */
	int numOfOptionalValues; /* the number of possible values that can be allocated to the cell */
	int optionalValues[9]; /* an array with the possible values that can be allocated to the cell */
}Cell;

/* define a struct representing a move of the user in the game */
typedef struct Move{
	int row;
	int col;
	int value;
	int prevValue;
	struct Move *lastMove;
	struct Move *nextMove;
}Move;

/* define a struct representing the sudoku board*/
typedef struct Game{
	Cell ** board;
	int n;
	int m;
	int markErrors;
	int numOfFilledCells;
	Move currentMove;
	int mode;
}Game;

void freeGame(Game* game);

void freeBoard(Game* game);

void exitGame(Game* game);

Game* createGame();

Cell ** createBoard(Game* game);

void reset(Game game);

void printBoard(Game* game);

void set(Game game, int row, int col, int value, int printSign);

void hint(int row, int col, Cell ** board);

int validate(Game game, int printSign);

int isErrorneous(Game game);

void mark_errors(int markErrorNum, int* error);

void gameControl();



#endif
