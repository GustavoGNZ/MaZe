#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdio.h>
#include <string.h> // Para memcpy

#define u64 unsigned long long

#define getBit(bitboard, casa) ((bitboard & (1ULL << casa)))
#define setBit(bitboard, casa) (bitboard |= (1ULL << casa))
#define clearBit(bitboard, casa) (bitboard &= ~(1ULL << casa))
#define getLeastBitIndex(bitboard) (__builtin_ctzll(bitboard)) // Retorna o índice do bit menos significativo

#define codificar_lance(origem, destino, peca, peca_promovida, captura, double_push, en_passant, roque) \
    (origem | (destino << 6) | (peca << 12) | (peca_promovida << 16) | (captura << 20) | (double_push << 21) | (en_passant << 22) | (roque << 23))

#define get_origem(lance) (lance & 0x3F)
#define get_destino(lance) ((lance >> 6) & 0x3F)
#define get_peca(lance) ((lance >> 12) & 0xF)
#define get_peca_promovida(lance) ((lance >> 16) & 0xF)
#define get_captura(lance) ((lance & 0x100000) )
#define get_double_push(lance) ((lance & 0x200000) )
#define get_en_passant(lance) ((lance & 0x400000) )
#define get_roque(lance) ((lance & 0x800000) )

extern const char *casa_nome[]; // Mapeamento de casas do tabuleiro


// bitboards para peças
extern u64 bitboards[12]; // bitboards para cada tipo de peça 
extern u64 ocupacoes[3]; // Ocupações do tabuleiro (brancas, pretas e ambos)

extern int lado_a_jogar;
extern int en_passant;
extern int roque;
extern int roque_permissoes[64];


extern const char pecas_ascii[]; // Representação ASCII das peças
extern const char *pecas_unicode[]; // Representação Unicode das peças
extern int pecas_char[]; // Mapeamento de peças para caracteres
extern const char pecas_promocao[]; // Peças de promoção

typedef struct {
    int lances[256]; // array para armazenar os lances gerados
    int contador; // contador de lances gerados

} lances;

// Função utilitária
void printBitboard(u64 bitboard); // Imprime o bitboard que representa o tabuleiro de xadrez
void printTabuleiro();            // Imprime o tabuleiro a partir do bitboard
void printCasasAtacadasPeloLado(int lado);
void parseFEN(char *fen); // Analisador sintático de FEN (Forsyth-Edwards Notation)
int contarBits(u64 bitboard); // Conta o número de bits setados no bitboard
void printLance(int lance); // Imprime um lance codificado
void printListaLances(lances *listaLances); // Imprime a lista de lances
void adicionarLance(lances *listaLances, int lance);
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

    K = 1
    Q = 2
    KQ = 3
    k = 4
    Kk = 5
    Qk = 6
    KQk = 7
    q = 8
    Kq = 9
    Qq = 10
    KQq = 11
    kq = 12
    Kkq = 13
    Qkq = 14
    KQkq = 15
    

    0001 = rei branco ala rei 1
    0010 = rei branco ala dama 2

    0011 = rei branco ala rei e dama 3

    0100 = rei preto ala rei 4
    1000 = rei preto ala dama 8

    1100 = rei preto ala rei e dama 12 

    1111 = ambos os lados com roque 15
    0000 = nenhum roque

*/


enum { reiBranco_alaRei = 1, reiBranco_alaDama = 2, reiBranco_alaRei_alaDama = 3, 
       reiPreto_alaRei = 4, reiPreto_alaDama = 8, reiPreto_alaRei_alaDama = 12, ambosReisLadosRoque = 15}; // roques

enum { P, N, B, R, Q, K, p, n, b, r, q, k }; // peças no bitboard (maiúsculas para brancas e minúsculas para pretas)
// usando notacao em ingles por exemplo: P = Pawn, N = Knight, B = Bishop, R = Rook, Q = Queen, K = King

enum { todosLances, lancesCaptura}; // tipos de lances para busca


/* formato binario para movimentos

    0000 0000 0000 0000 0011 1111 origem 6bits hex 0x3F
    0000 0000 0000 1111 1100 0000 destino 6bits hex 0xFC0
    0000 0000 1111 0000 0000 0000 peca 4bits hex 0xF000
    0000 1111 0000 0000 0000 0000 peca promovida 4bits hex 0xF000
    0001 0000 0000 0000 0000 0000 flag captura 1bit hex 0x100000 
    0010 0000 0000 0000 0000 0000 flag double push 1bit hex 0x200000
    0100 0000 0000 0000 0000 0000 flag en passant 1bit hex 0x400000
    1000 0000 0000 0000 0000 0000 flag roque 1bit hex 0x800000

*/

// Estrutura para backup do estado do jogo
// Como usar: declare na main ou função relevante: estado_jogo backup;
// salvar estado: SALVAR_ESTADO(backup);
// restaurar estado: RESTAURAR_ESTADO(backup);
typedef struct {
    u64 bitboards_backup[12];  // Backup dos bitboards das peças
    u64 ocupacoes_backup[3];   // Backup das ocupações
    int lado_a_jogar_backup;   // Backup do lado a jogar
    int en_passant_backup;     // Backup do en passant
    int roque_backup;          // Backup dos direitos de roque
} estado_jogo;

// Macros para salvar e restaurar o estado do jogo
#define SALVAR_ESTADO(backup) do { \
    memcpy((backup).bitboards_backup, bitboards, sizeof(bitboards)); \
    memcpy((backup).ocupacoes_backup, ocupacoes, sizeof(ocupacoes)); \
    (backup).lado_a_jogar_backup = lado_a_jogar; \
    (backup).en_passant_backup = en_passant; \
    (backup).roque_backup = roque; \
} while(0)

#define RESTAURAR_ESTADO(backup) do { \
    memcpy(bitboards, (backup).bitboards_backup, sizeof(bitboards)); \
    memcpy(ocupacoes, (backup).ocupacoes_backup, sizeof(ocupacoes)); \
    lado_a_jogar = (backup).lado_a_jogar_backup; \
    en_passant = (backup).en_passant_backup; \
    roque = (backup).roque_backup; \
} while(0)


#endif
