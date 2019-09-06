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
	Move lastMove;
	Move nextMove;
}Move;

/* define a struct representing the sudoku board*/
typedef struct Game{
	Cell ** board;
	int n;
	int m;
	int markErrors;
	int numOfFilledCells;
	Move currentMove;
	int gameMode;
}Game;

/* set fixed cell according to numOfFilledCells variable &
 * solving the board using nonDeterministic BackTracking algorithm */
void setBoard(Cell ** board, int numOfFilledCells);

/* a command for exiting the game
 * we free the allocated memory using freeBoard and exiting */
void exitGame(Cell ** board);

Cell ** createGame();

/* allocating memory for new board of sudoku */
Cell ** initialBoard();

/* create a new board, fill if and set #n cells according to the user decision */
Cell ** createBoard();

/* create a copy of the board by allocating new memory for new board and copy all the data */
Cell ** copyBoard(Cell** board);

/* print the board in the required format */
void printBoard(Cell** board);

/* before exiting the game, we free the memory allocated to the board (2d array of Cell) */
void freeBoard(Cell ** board);

/* a command the user can put to set value to cell (row,col) */
void set(int row, int col, int value, Cell ** board);

/* a command the user can put to get a hint to a suitable value for cell (row,col)
 * we use the saves values from the board build to return the suitable value */
void hint(int row, int col, Cell ** board);

/* a command the user can put to vlidate that the board is solvable
 * we use deterministic back-tracking to check if it is solvable*/
void validate(Cell ** board);

/* a command the user can put while playing to restart the game */
void restart(Cell ** board);

/* in case we end the game and filled all the board,
 * we can only exit or restart */
void endGame(Cell ** board);

/* start the game and interactively apply the users commands */
void initMode();

#endif
