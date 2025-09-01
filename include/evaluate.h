// evaluate.h
#ifndef EVALUATE_H
#define EVALUATE_H

#include "bitboard.h"

extern int material_score[12];

extern int pawn_table[64];
extern int knight_table[64];
extern int bishop_table[64];
extern int rook_table[64];
extern int queen_table[64];
extern int king_table_mg[64];
extern int king_table_eg[64];


// Funções de avaliação específicas
int evaluate_material();
int evaluate_piece_position();

// Função principal de avaliação
int evaluate();

#endif