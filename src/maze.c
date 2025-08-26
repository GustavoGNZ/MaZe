#include "../include/bitboard.h"
#include "../include/ataques.h"
#include "../include/globals.h"
#include <stdio.h>
#include <stdlib.h>

// posicoes fen para teste
#define posicaoVazia "8/8/8/3Q4/4p3/8/8/8 w - - 0 1"
#define posicaoInicial "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define posicaoTeste "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define RUYLOPEZ "r1bqkbnr/pppp1ppp/2n5/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 3 3"

// FENs para testar en passant
#define enPassantBranco "rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3"
#define enPassantPreto "rnbqkbnr/pppp1ppp/8/8/3Pp3/8/PPP1PPPP/RNBQKBNR b KQkq - 0 3"


int main()
{
    u64 tabuleiro = 0ULL;

    runEngine();
    // Testando com uma posição que tenha promoção de peão branco
    // parseFEN("r3k2r/pPppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    parseFEN(posicaoInicial);
    printTabuleiro();

    // Exemplo de uso das macros de backup
    estado_jogo backup;
    
    // Salva o estado atual
    SALVAR_ESTADO(backup);
    printf("Estado salvo!\n\n");

    lances listaLances[1];
    gerar_lances(listaLances);
    // printListaLances(listaLances);
    
    // Simula uma alteração no estado (exemplo: mudança de lado)
    printf("\nSimulando mudança de estado...\n");
    parseFEN(posicaoVazia);
    printTabuleiro();

    // Restaura o estado original
    RESTAURAR_ESTADO(backup);
    printf("Estado restaurado!\n");
    printTabuleiro();

    return 0;
}
                                                                                                                        