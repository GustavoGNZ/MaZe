#ifndef ATAQUES_H
#define ATAQUES_H

#include "bitboard.h"

// Funcoes de geração de números aleatórios
unsigned int gerarNumeroAleatorio32bits();
u64 gerarNumeroAleatorio64bits();
void inicializarMagicNumbers();
u64 gerarMagicNumber();
u64 encontrarMagicNumber(int casa, int bits_relevantes, int peca);


// Verifica se a casa está dentro do tabuleiro
int estaDentroDoTabuleiro(int linha, int coluna);
int ehUltimaCasaNaDirecao(int linha, int coluna, int deslocamento_linha, int deslocamento_coluna);

// Marca os ataques direcionais para peças como bispo e torre
void marcarAtaquesDirecionais(u64 *ataque, int linha, int coluna, int deslocamentos[][2]);
void marcarAtaquesDirecionaisComBloqueio(u64 *ataque, int linha, int coluna, int deslocamentos[][2], int num_direcoes, u64 bitboard_ocupacao);


// Gera os ataques pré-definidos para as peças
void inicializarAtaquesPecas();
void inicializarAtaquesPecasDeslizantes(int peca);

u64 gerarAtaquesPeao(int lado, int casa);
u64 gerarAtaquesCavalo(int casa);
u64 gerarAtaquesRei(int casa);
u64 gerarAtaquesBispo(int casa);
u64 gerarAtaquesTorre(int casa);

u64 gerarAtaquesBispoComBloqueio(int casa, u64 bitboard_ocupacao);
u64 gerarAtaquesTorreComBloqueio(int casa, u64 bitboard_ocupacao);

u64 obterAtaquesBispo(int casa, u64 ocupacao);
u64 obterAtaquesTorre(int casa, u64 ocupacao);
u64 obterAtaquesDama(int casa, u64 ocupacao); 

u64 set_occupancy(int index, int qtde_bits, u64 mask);


#endif
