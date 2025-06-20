#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdio.h>

#define u64 unsigned long long

#define getBit(bitboard, casa) ((bitboard & (1ULL << casa)))
#define setBit(bitboard, casa) (bitboard |= (1ULL << casa))
#define clearBit(bitboard, casa) (bitboard &= ~(1ULL << casa))
#define getLeastBitIndex(bitboard) (__builtin_ctzll(bitboard)) // Retorna o índice do bit menos significativo

extern const char *casa_nome[]; // Mapeamento de casas do tabuleiro

// Função de utilitário
void printBitboard(u64 bitboard);
int contarBits(u64 bitboard);

// Enumerações para casas e lados
enum { a1, b1, c1, d1, e1, f1, g1, h1,
       a2, b2, c2, d2, e2, f2, g2, h2,
       a3, b3, c3, d3, e3, f3, g3, h3,
       a4, b4, c4, d4, e4, f4, g4, h4,
       a5, b5, c5, d5, e5, f5, g5, h5,
       a6, b6, c6, d6, e6, f6, g6, h6,
       a7, b7, c7, d7, e7, f7, g7, h7,
       a8, b8, c8, d8, e8, f8, g8, h8 };

enum { branco, preto };


#endif
