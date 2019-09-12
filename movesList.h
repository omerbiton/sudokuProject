#ifndef MOVELIST_H_
#define MOVELIST_H_


void setMove(Game* game, int row, int col, int value, int prevValue);

void clearNextMoves(Game game);

void clearPrevMoves(Game game);

void undo(Game game, int printSign);

void redo(Game game, int printSign);

#endif
