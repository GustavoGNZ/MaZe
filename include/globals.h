// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include "bitboard.h"

// Estrutura para detecção de repetição
#define MAX_HISTORIA 1000
extern u64 historico_posicoes[MAX_HISTORIA];
extern int contador_historia;

// Histórico de lances para livro de aberturas
#define MAX_LANCES_PARTIDA 200
extern int historico_lances_partida[MAX_LANCES_PARTIDA];
extern int num_lances_partida;

// Funções para detecção de repetição
void adicionar_posicao_historia();
int posicao_repetida();
void limpar_historia();
u64 hash_posicao_simples();

extern u64 tabela_ataques_peao[2][64];
extern u64 tabela_ataques_cavalo[64];
extern u64 tabela_ataques_rei[64];

extern u64 mask_tabela_ataques_bispo[64];
extern u64 mask_tabela_ataques_torre[64];

extern u64 tabela_ataques_bispo[64][512];
extern u64 tabela_ataques_torre[64][4096];

extern const int bits_relevantes_bispo[64];
extern const int bits_relevantes_torre[64];

extern  u64 magics_bispo[64];
extern  u64 magics_torre[64];

// Variável global para backup do estado do jogo
extern estado_jogo backup_global;

#endif