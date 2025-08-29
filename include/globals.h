// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include "bitboard.h"

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