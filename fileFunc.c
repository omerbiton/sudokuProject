#include "MainAux.h"
#include "parser.h"
#include "solver.h"
#include "game.h"

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
		game->n = n;
		game->m = m;
		if(n <= 0 || m <= 0){
			ErrorIncorectFormat();
			fclose(filePath);
			return(0);
		}
		N = n*m;
		 /* read the file until you reach its end or you filled all the board */
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
					fclose(filePath);
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
				fclose(filePath);
				return(0);
			}
		}
		game->numOfFilledCells = numOfFilledCells;
	fclose(filePath);

}

void saveToFile(Game *game, char* filePath){
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
