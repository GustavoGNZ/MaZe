// search.h
#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"

extern int ply;
extern int bestMove;

int negamax(int alpha, int beta, int depth);
void busca_lance(int depth);


#endif