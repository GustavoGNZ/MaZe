#include "../include/bitboard.h"

u64 bitboards[12];
u64 ocupacoes[3];

int lado_a_jogar;
int en_passant = -999; // Valor padrão para indicar que não há en passant
int roque = 0; // Valor padrão para indicar que não há roque

const char pecas_ascii[] = {
    'P', 'N', 'B', 'R', 'Q', 'K', // Peças brancas
    'p', 'n', 'b', 'r', 'q', 'k'  // Peças pretas
};

const char *pecas_unicode[] = {
    "♟", "♞", "♝", "♜", "♛", "♚", // Peças brancas
    "♙", "♘", "♗", "♖", "♕", "♔"  // Peças pretas
};

int pecas_char[] = {
    [P] = 'P', [N] = 'N', [B] = 'B', [R] = 'R', [Q] = 'Q', [K] = 'K', [p] = 'p', [n] = 'n', [b] = 'b', [r] = 'r', [q] = 'q', [k] = 'k'};

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

void printTabuleiro()
{
    for (int linha = 7; linha >= 0; linha--)
    {
        printf(" %d ", linha + 1);

        for (int coluna = 0; coluna < 8; coluna++)
        {
            int casa = linha * 8 + coluna;

            int peca = -1;

            for (int pecaTabuleiro = P; pecaTabuleiro <= k; pecaTabuleiro++)
            {
                if (getBit(bitboards[pecaTabuleiro], casa))
                {
                    peca = pecaTabuleiro;
                }
            }

            if (peca != -1)
            {
                #ifdef _WIN32
                                printf(" %c ", pecas_char[peca]);
                #else
                                printf(" %s ", pecas_unicode[peca]);
                #endif
            }
            else
            {
                printf(" . ");
            }
        }

        printf("\n");
    }
    printf("    a  b  c  d  e  f  g  h\n");

    printf("\n");

    printf("Turno das: %s\n", lado_a_jogar == branco ? "Brancas" : "Pretas");

    printf("En passant: %s\n", (en_passant != -999) ? casa_nome[en_passant] : "Nenhum");

    printf("Roque: ");
    // Exibe roques de forma compacta: KQkq (K=branco rei, Q=branco dama, k=preto rei, q=preto dama)
    printf("%c", (roque & reiBranco_alaRei)     ? 'K' : '-');
    printf("%c", (roque & reiBranco_alaDama)    ? 'Q' : '-');
    printf("%c", (roque & reiPreto_alaRei)      ? 'k' : '-');
    printf("%c", (roque & reiPreto_alaDama)     ? 'q' : '-');
    printf("\n");
}

void gerarPosicaoInicial()
{
    // Limpa os bitboards
    for (int i = 0; i < 12; i++)
    {
        bitboards[i] = 0ULL;
    }

    // Coloca as peças brancas
    setBit(bitboards[R], a1);
    setBit(bitboards[N], b1);
    setBit(bitboards[B], c1);
    setBit(bitboards[Q], d1);
    setBit(bitboards[K], e1);
    setBit(bitboards[B], f1);
    setBit(bitboards[N], g1);
    setBit(bitboards[R], h1);

    for (int i = a2; i <= h2; i++)
    {
        setBit(bitboards[P], i);
    }

    // Coloca as peças pretas
    setBit(bitboards[r], a8);
    setBit(bitboards[n], b8);
    setBit(bitboards[b], c8);
    setBit(bitboards[q], d8);
    setBit(bitboards[k], e8);
    setBit(bitboards[b], f8);
    setBit(bitboards[n], g8);
    setBit(bitboards[r], h8);

    for (int i = a7; i <= h7; i++)
    {
        setBit(bitboards[p], i);
    }

    // Inicializa ocupações
    ocupacoes[branco] = 0ULL;
    ocupacoes[preto] = 0ULL;
    
    for (int i = P; i <= K; i++)
        ocupacoes[branco] |= bitboards[i];

    for (int i = p; i <= k; i++)
        ocupacoes[preto] |= bitboards[i];

    ocupacoes[ambos] = ocupacoes[branco] | ocupacoes[preto];

    // Inicializa o lado a jogar
    lado_a_jogar = branco;
    en_passant = -999; // Nenhum en passant disponível
    roque = 0;
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
