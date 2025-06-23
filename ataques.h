#ifndef ATAQUES_H
#define ATAQUES_H

#include "bitboard.h"

// Variaveis globais para armazenar os ataques pré-definidos de cada peça
extern u64 tabela_ataques_peao[2][64];
extern u64 tabela_ataques_cavalo[64];
extern u64 tabela_ataques_rei[64];
extern u64 tabela_ataques_bispo[64];
extern u64 tabela_ataques_torre[64];
extern const int bits_relevantes_bispo[64];
extern const int bits_relevantes_torre[64];
extern const u64 magics_bispo[64];
extern const u64 magics_torre[64];

// Funcoes de geração de números aleatórios
unsigned int gerarNumeroAleatorio32bits();
u64 gerarNumeroAleatorio64bits();
u64 gerarMagicNumber();
u64 find_magic_number(int casa, int bits_relevantes, int peca);
void init_magic_numbers();

// Verifica se a casa está dentro do tabuleiro
int dentro_do_tabuleiro(int linha, int coluna);
int eh_ultima_casa_na_direcao(int linha, int coluna, int deslocamento_linha, int deslocamento_coluna);

// Marca os ataques direcionais para peças como bispo e torre
void marcar_ataque_direcional(u64 *ataque, int linha, int coluna, int deslocamentos[][2]);
void marcar_ataque_direcional_tempo_real(u64 *ataque, int linha, int coluna, int deslocamentos[][2], int num_direcoes, u64 bitboard_ocupacao);


// Gera os ataques pré-definidos para as peças
void gerar_ataques_pecas();
u64 gerar_ataque_peao(int lado, int casa);
u64 gerar_ataque_cavalo(int casa);
u64 gerar_ataque_rei(int casa);
u64 gerar_ataque_bispo(int casa);
u64 gerar_ataque_torre(int casa);
u64 gerar_ataque_bispo_tempo_real(int casa, u64 bitboard);
u64 gerar_ataque_torre_tempo_real(int casa, u64 bitboard);
u64 set_occupancy(int index, int qtde_bits, u64 mask);


#endif
