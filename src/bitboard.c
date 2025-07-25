#include "../include/bitboard.h"

u64 bitboards[12];


u64 ocupacoes[3];

int lado_a_jogar = -999; 
int en_passant = -999;
int roque = -999; 

const char pecas_ascii[] = {
    'P', 'N', 'B', 'R', 'Q', 'K', // Peças brancas
    'p', 'n', 'b', 'r', 'q', 'k'  // Peças pretas
};

const char *pecas_unicode[] = {
    "♟", "♞", "♝", "♜", "♛", "♚", // Peças brancas
    "♙", "♘", "♗", "♖", "♕", "♔"  // Peças pretas
};

int pecas_char[] = {
    [P] = 'P', [N] = 'N', [B] = 'B', [R] = 'R', [Q] = 'Q', [K] = 'K',
    [p] = 'p', [n] = 'n', [b] = 'b', [r] = 'r', [q] = 'q', [k] = 'k'
};


const char *casa_nome[] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"};

// Imprime o bitboard que representa o tabuleiro de xadrez.
void printBitboard(u64 bitboard)
{

    for (int linha = 7; linha >= 0; linha--)
    {
        printf(" %d ", linha + 1);

        for (int coluna = 0; coluna < 8; coluna++)
        {
            int casa = linha * 8 + coluna;

            if (getBit(bitboard, casa))
            {
                printf(" 1 ");
            }
            else
            {
                printf(" 0 ");
            }
        }

        printf("\n");
    }
    printf("    a  b  c  d  e  f  g  h\n");
    printf("\n Bitboard: %lld\n", bitboard);
}

int contarBits(u64 bitboard)
{
    int count = 0;

    while (bitboard != 0)
    {
        count += bitboard & 1; // Verifica o bit menos significativo
        bitboard >>= 1;        // Desloca para a direita
    }

    return count;
}

