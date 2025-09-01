#include "../include/search.h"
#include "../include/evaluate.h"
#include "../include/bitboard.h"
#include "../include/globals.h"
#include "../include/ataques.h"

int ply;
int bestMove;
// variante minimax
int negamax(int alpha, int beta, int depth)
{
    if (depth == 0)
    {
        return evaluate();
    }

    nos++;
    int bestMove_temporario;
    int alpha_antigo = alpha;
    lances listaLances[1];

    gerar_lances(listaLances);

    for (int i = 0; i < listaLances->contador; i++)
    {
        SALVAR_ESTADO(backup_global);
        ply++;

        if (fazer_lance(listaLances->lances[i], todosLances, backup_global) == 0)
        {
            ply--;
            continue;
        }

        int score = -negamax(-beta, -alpha, depth - 1);
        RESTAURAR_ESTADO(backup_global);
        ply--;

        if (score >= beta)
        {
            return beta; // Poda beta
        }

        if (score > alpha)
        {
            alpha = score;
            if (ply == 0)
            {
                bestMove_temporario = listaLances->lances[i];
            }
        }
    }
    if (alpha_antigo != alpha)
    {
        // Atualiza a melhor jogada
        bestMove = bestMove_temporario;
    }

    return alpha;
}

void busca_lance(int depth){
    printf("Iniciando busca com profundidade %d\n", depth);
    printf("Posicao inicial: score = %d\n", evaluate());
    
    int score = negamax(-99999, 99999, depth);

    printf("Melhor score: %d\n", score);
    printf("bestmove ");
    printLance(bestMove);
    printf("\n");
}