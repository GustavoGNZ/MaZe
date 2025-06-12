#ifndef ATAQUES_H
#define ATAQUES_H

#include "bitboard.h"

extern u64 tabela_ataques_peao[2][64];
extern u64 tabela_ataques_cavalo[64];

u64 gerar_ataque_peao(int lado, int casa);
u64 gerar_ataque_cavalo(int casa);
void gerar_ataques();

#endif
