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
extern int get_linha[64];

extern int peao_dobrado_penalidade;
extern int peao_isolado_penalidade;
extern int peao_passado_bonus[8];// Bônus por avanço do peão passado
extern int coluna_semilivre_bonus;
extern int coluna_livre_bonus;
extern int rei_coluna_livre_penalidade;
extern int rei_coluna_semilivre_penalidade;

// Parâmetros de mobilidade
extern int mobility_bonus_knight;
extern int mobility_bonus_bishop;
extern int mobility_bonus_rook;
extern int mobility_bonus_queen;

// Parâmetros de segurança do rei
extern int king_safety_pawn_shield_bonus;
extern int king_safety_open_file_penalty;
extern int king_safety_attacked_square_penalty;
extern int king_safety_king_attacked_penalty;


u64 set_file_rank_masks(int file, int rank);
u64 set_passed_pawn_mask_white(int file, int rank);
u64 set_passed_pawn_mask_black(int file, int rank);

void init_evaluation_masks();

// Funções de avaliação específicas
int evaluate_material();
int evaluate_positional();
int evaluate_pawn_structure();
int evaluate_open_files();
int evaluate_mobility();
int evaluate_king_safety();

// Função principal de avaliação
int evaluate();

#endif