#ifndef ATAQUES_H
#define ATAQUES_H

#include "bitboard.h"

extern u64 tabela_ataques_peao[2][64];
extern u64 tabela_ataques_cavalo[64];
extern u64 tabela_ataques_rei[64];
extern u64 tabela_ataques_bispo[64];
extern u64 tabela_ataques_torre[64];
extern const int bits_relevantes_bispo[64];
extern const int bits_relevantes_torre[64];


u64 gerar_ataque_peao(int lado, int casa);
u64 gerar_ataque_cavalo(int casa);
u64 gerar_ataque_rei(int casa);
u64 gerar_ataque_bispo(int casa);
u64 gerar_ataque_torre(int casa);
u64 gerar_ataque_bispo_tempo_real(int casa, u64 bitboard);
u64 gerar_ataque_torre_tempo_real(int casa, u64 bitboard);
unsigned int gerarNumeroAleatorio();
u64 set_occupancy(int index, int qtde_bits, u64 mask);
void gerar_ataques_pecas();

#endif
