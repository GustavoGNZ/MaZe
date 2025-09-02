#include "../include/search.h"
#include "../include/evaluate.h"
#include "../include/bitboard.h"
#include "../include/globals.h"
#include "../include/ataques.h"

int ply;
int bestMove;

// The purpose of this search is to only evaluate "quiet" positions, or positions where there are no winning tactical moves to be made. This search is needed to avoid the horizon effect. -- chess programing .org
int quiescence(int alpha, int beta)
{
    int evaluation = evaluate();

    if (evaluation >= beta)
    {
        return beta; 
    }

    if (evaluation > alpha)
    {
        alpha = evaluation;
    }

    lances listaLances[1];
    gerar_lances(listaLances);

    for (int i = 0; i < listaLances->contador; i++)
    {
        estado_jogo backup_local; // Backup local para cada lance
        SALVAR_ESTADO(backup_local);
        ply++;

        if (fazer_lance(listaLances->lances[i], lancesCaptura, backup_local) == 0)
        {
            ply--;
            continue;
        }

        int score = -quiescence(-beta, -alpha);

        RESTAURAR_ESTADO(backup_local); // Restaurar do backup local
        ply--;

        if (score >= beta)
        {
            return beta; // Poda beta
        }

        if (score > alpha)
        {
            alpha = score;
        }
    }
    return alpha;
}

// variante minimax
int negamax(int alpha, int beta, int depth)
{
    if (depth == 0)
    {
        return quiescence(alpha, beta);
    }

    nos++;

    int lances_legais = 0;

    int em_cheque = casaEstaAtacada((lado_a_jogar == branco) ? getLeastBitIndex(bitboards[K]) : getLeastBitIndex(bitboards[k]), lado_a_jogar ^ 1);

    int bestMove_temporario = 0;
    int alpha_antigo = alpha;

    lances listaLances[1];
    gerar_lances(listaLances);

    for (int i = 0; i < listaLances->contador; i++)
    {
        estado_jogo backup_local; // Backup local para cada lance
        SALVAR_ESTADO(backup_local);
        ply++;

        if (fazer_lance(listaLances->lances[i], todosLances, backup_local) == 0)
        {
            ply--;
            continue;
        }

        lances_legais++;

        int score = -negamax(-beta, -alpha, depth - 1);

        RESTAURAR_ESTADO(backup_local); // Restaurar do backup local
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

    if (lances_legais == 0)
    {
        if (em_cheque)
        {
            return -99999 + ply; // cheque mate
        }
        else
        {
            return 0; // empate por afogamento
        }
    }

    if (alpha_antigo != alpha)
    {
        bestMove = bestMove_temporario;
    }

    return alpha;
}

void busca_lance(int depth)
{
    // printf("=== INICIANDO BUSCA COM PROFUNDIDADE %d ===\n", depth);
    // printf("Posicao inicial: score = %d\n", evaluate());

    // Para detectar mate em 1, precisa de pelo menos profundidade 2
    if (depth < 2)
    {
        depth = 2;
    }

    int score = negamax(-99999, 99999, depth);

    if (bestMove)
    {

        printf("info score cp %d depth %d nodes %ld pv \n", score, depth, nos);
        printf("bestmove ");
        printLance(bestMove);
        printf("\n");
    }
}