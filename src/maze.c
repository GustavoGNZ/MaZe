#include "../include/bitboard.h"
#include "../include/ataques.h"
#include "../include/globals.h"
#include <stdio.h>
#include <stdlib.h>

// posicoes fen para teste
#define posicaoVazia "8/8/8/8/8/8/8/8 w - - 0 1"
#define posicaoInicial "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define posicaoTeste "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1"
#define RUYLOPEZ "r1bqkbnr/pppp1ppp/2n5/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 3 3"

// FENs para testar en passant
#define enPassantBranco "rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3"
#define enPassantPreto "rnbqkbnr/pppp1ppp/8/8/3Pp3/8/PPP1PPPP/RNBQKBNR b KQkq d3 0 3"


int main()
{
    u64 tabuleiro = 0ULL;

    runEngine();
    // Testando com uma posição que tenha promoção de peão branco
    parseFEN("r3k2r/pPppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    printTabuleiro();

    // Exemplo de uso das macros de backup
    estado_jogo backup;

    lances listaLances[1];

    gerar_lances(listaLances);
    printListaLances(listaLances);

    for (int i = 0; i < listaLances->contador; i++)
    {
        int lance = listaLances->lances[i];
        SALVAR_ESTADO(backup);

        fazer_lance(lance, todosLances, backup);
        printTabuleiro();
        getchar();
        RESTAURAR_ESTADO(backup);

    }
    return 0;
}
                                                                                                                        