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
#define RUYLOPEZ "r1bqkbnr/pppp1ppp/2n5/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 3 3"
#define killerTeste "6k1/3q1pp1/pp5p/1r5n/8/1P3PP1/PQ4BP/2R3K1 w - - 0 1"

// FENs para testar en passant
#define enPassantBranco "rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3"
#define enPassantPreto "rnbqkbnr/pppp1ppp/8/8/3Pp3/8/PPP1PPPP/RNBQKBNR b KQkq d3 0 3"
#define repeticao "2r3k1/R7/8/1R6/8/8/P4KPP/8 w - - 0 40"
#define peaoDobrado "8/8/1p6/8/P7/P7/P7/8 w - - 0 1"
#define testeSegurancaRei "rn1qkbnr/ppp2ppp/4p3/1b1p4/6B1/5N2/PP3PPP/RNBQK2R w kq - 0 1"

int main()
{
    u64 tabuleiro = 0ULL;

    runEngine();
    
    // lances listaLances[1];
    // gerar_lances(listaLances);
    // sort_moves(listaLances);

    uci_loop();

    return 0;
}
