#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdio.h>

#define u64 unsigned long long

#define getBit(bitboard, casa) ((bitboard & (1ULL << casa)))
#define setBit(bitboard, casa) (bitboard |= (1ULL << casa))
#define clearBit(bitboard, casa) (bitboard &= ~(1ULL << casa))
#define getLeastBitIndex(bitboard) (__builtin_ctzll(bitboard)) // Retorna o índice do bit menos significativo

extern const char *casa_nome[]; // Mapeamento de casas do tabuleiro


// bitboards para peças
extern u64 bitboards[12]; // bitboards para cada tipo de peça 
extern u64 ocupacoes[3]; // Ocupações do tabuleiro (brancas, pretas e ambos)

extern int lado_a_jogar;
extern int en_passant;
extern int roque;

extern const char pecas_ascii[]; // Representação ASCII das peças
extern const char *pecas_unicode[]; // Representação Unicode das peças
extern int pecas_char[]; // Mapeamento de peças para caracteres

// Função utilitária
void printBitboard(u64 bitboard); // Imprime o bitboard que representa o tabuleiro de xadrez
void printTabuleiro();            // Imprime o tabuleiro a partir do bitboard
void printCasasAtacadasPeloLado(int lado);
void parseFEN(char *fen); // Analisador sintático de FEN (Forsyth-Edwards Notation)
int contarBits(u64 bitboard); // Conta o número de bits setados no bitboard

// Enumerações para casas e lados
enum { a1, b1, c1, d1, e1, f1, g1, h1,
       a2, b2, c2, d2, e2, f2, g2, h2,
       a3, b3, c3, d3, e3, f3, g3, h3,
       a4, b4, c4, d4, e4, f4, g4, h4,
       a5, b5, c5, d5, e5, f5, g5, h5,
       a6, b6, c6, d6, e6, f6, g6, h6,
       a7, b7, c7, d7, e7, f7, g7, h7,
       a8, b8, c8, d8, e8, f8, g8, h8 };

enum { branco, preto, ambos }; // lados
enum { peao, cavalo, bispo, torre, dama, rei }; // lidar com pecas em alto nivel (nao sao as pecas do bitboard)

/* Enumerações para roques

    0001 = rei branco ala rei 1
    0010 = rei branco ala dama 2
    0100 = rei preto ala rei 4
    1000 = rei preto ala dama 8

    0011 = rei branco ala rei e dama 3
    1100 = rei preto ala rei e dama 12 
    1111 = ambos os lados com roque 15
    0000 = nenhum roque

*/ 

enum { reiBranco_alaRei = 1, reiBranco_alaDama = 2, reiBranco_alaRei_alaDama = 3, 
       reiPreto_alaRei = 4, reiPreto_alaDama = 8, reiPreto_alaRei_alaDama = 12, ambosReisLadosRoque = 15}; // roques

enum { P, N, B, R, Q, K, p, n, b, r, q, k }; // peças no bitboard (maiúsculas para brancas e minúsculas para pretas)
// usando notacao em ingles por exemplo: P = Pawn, N = Knight, B = Bishop, R = Rook, Q = Queen, K = King



#endif
