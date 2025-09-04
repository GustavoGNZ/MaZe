#include "../include/bitboard.h"
#include "../include/ataques.h"
#include "../include/globals.h"
#include "../include/uci.h"
#include "../include/search.h"
#include "../include/evaluate.h"
#include <stdio.h>
#include <stdlib.h>

// posicoes fen para teste
#define posicaoVazia "8/8/8/8/8/8/8/8 w - - 0 1"
#define posicaoInicial "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define posicaoTeste "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "
#define RUYLOPEZ "r1bqkbnr/pppp1ppp/2n5/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 3 3"
#define killerTeste "6k1/3q1pp1/pp5p/1r5n/8/1P3PP1/PQ4BP/2R3K1 w - - 0 1"

// FENs para testar en passant
#define enPassantBranco "rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3"
#define enPassantPreto "rnbqkbnr/pppp1ppp/8/8/3Pp3/8/PPP1PPPP/RNBQKBNR b KQkq d3 0 3"

int main()
{
    u64 tabuleiro = 0ULL;

    runEngine();
    parseFEN(posicaoTeste);
    printTabuleiro();
    busca_lance(5);

    lances listaLances[1];
    gerar_lances(listaLances);
    // printListaLances(listaLances); // ruim no windows
    sort_moves(listaLances);
    print_move_scores();
    // printListaLances(listaLances);

    // uci_loop();

    return 0;
}
