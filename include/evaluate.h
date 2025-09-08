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

// 64 casas do tabuleiro
extern u64 coluna_masks[64];    
extern u64 linha_masks[64];

extern u64 peao_isolado_masks[64];
extern u64 peao_passado_branco_masks[64];
extern u64 peao_passado_preto_masks[64];


u64 set_file_rank_masks(int file, int rank);
u64 set_passed_pawn_mask_white(int file, int rank);
u64 set_passed_pawn_mask_black(int file, int rank);

void init_evaluation_masks();

// Funções de avaliação específicas
int evaluate_material();
int evaluate_positional();

// Função principal de avaliação
int evaluate();

#endif