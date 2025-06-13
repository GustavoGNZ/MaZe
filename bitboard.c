#include "bitboard.h"

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
