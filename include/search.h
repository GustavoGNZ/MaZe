// search.h
#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"

extern int ply;
extern int bestMove;

// Array para detectar repetição na linha de busca atual
#define MAX_PLY_BUSCA 64
extern u64 hash_linha_busca[MAX_PLY_BUSCA];

// Tabela MVV-LVA para ordenação de capturas
extern int mvv_lva[12][12];
extern int killer_moves[2][64];
extern int history_moves[12][64];

void enable_pv_scoring(lances *listaLances);
int quiescence(int alpha, int beta);
int negamax(int alpha, int beta, int depth);
void busca_lance(int depth);
int get_mvv_lva_score(int atacante, int vitima);
int score_move(int move);
int sort_moves(lances *listaLances);
void print_move_scores();

// Funções auxiliares para quicksort
void swap_moves(lances *listaLances, int *move_scores, int i, int j);
int partition(lances *listaLances, int *move_scores, int low, int high);
void quicksort_moves(lances *listaLances, int *move_scores, int low, int high);

#endif