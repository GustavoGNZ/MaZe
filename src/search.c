#include "../include/search.h"
#include "../include/evaluate.h"
#include "../include/bitboard.h"
#include "../include/globals.h"
#include "../include/ataques.h"

int ply;
int bestMove;

// TODO tabela mvv-lva

// variante minimax
int negamax(int alpha, int beta, int depth)
{
    if (depth == 0)
    {
        return evaluate();
    }

    nos++;
    lances listaLances[1];
    gerar_lances(listaLances);

    int em_cheque = casaEstaAtacada((lado_a_jogar == branco) ? getLeastBitIndex(bitboards[K]) : getLeastBitIndex(bitboards[k]), lado_a_jogar ^ 1);

    int lances_legais = 0;
    int bestMove_temporario = 0;
    int alpha_antigo = alpha;

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