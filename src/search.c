#include "../include/search.h"
#include "../include/evaluate.h"
#include "../include/bitboard.h"
#include "../include/globals.h"
#include "../include/ataques.h"

int ply;
int bestMove;

/**
 * TABELA MVV-LVA (Most Valuable Victim - Least Valuable Attacker)
 *
 * Esta tabela é usada para ordenar lances de captura de forma eficiente.
 * O princípio é simples: priorizar capturas onde a peça capturada (vítima)
 * tem alto valor e a peça que captura (atacante) tem baixo valor.
 *
 * Formato: mvv_lva[atacante][vítima] = score
 *
 * Índices das peças:
 * 0=P(peão branco), 1=N(cavalo branco), 2=B(bispo branco), 3=R(torre branca), 4=Q(dama branca), 5=K(rei branco)
 * 6=p(peão preto), 7=n(cavalo preto), 8=b(bispo preto), 9=r(torre preto), 10=q(dama preta), 11=k(rei preto)
 *
 * Exemplos práticos:
 * - mvv_lva[0][10] = 95 → Peão branco captura Dama preta (Melhor: vítima valiosa, atacante barato)
 * - mvv_lva[4][6] = 91  → Dama branca captura peão preto (Pior: vítima barata, atacante caro)
 * - mvv_lva[1][9] = 53  → Cavalo branco captura Torre preta (Bom: vítima cara, atacante médio)
 * - mvv_lva[3][7] = 33  → Torre branca captura Cavalo preto (Médio: ambos têm valor médio)
 *
 * Fórmula: (valor_vítima * 10) - valor_atacante
 * Valores das peças: P/p=1, N/n=3, B/b=3, R/r=5, Q/q=9, K/k=0(não capturável)
 */

// Tabela MVV-LVA pré-calculada [atacante][vítima]
// Incluindo capturas da mesma cor (valores baixos) e rei como vítima mais valiosa (valor 999)
int mvv_lva[12][12] = {
    // Atacante: P (Peão branco) - índice 0                    P   N   B   R   Q   K   p   n   b   r   q   k
    {4, 2, 2, 1, 1, 99, 9, 29, 29, 49, 89, 999}, // P vs todas
    // Atacante: N (Cavalo branco) - índice 1
    {4, 2, 2, 1, 1, 97, 7, 27, 27, 47, 87, 997}, // N vs todas
    // Atacante: B (Bispo branco) - índice 2
    {4, 2, 2, 1, 1, 97, 7, 27, 27, 47, 87, 997}, // B vs todas
    // Atacante: R (Torre branca) - índice 3
    {4, 2, 2, 1, 1, 95, 5, 25, 25, 45, 85, 995}, // R vs todas
    // Atacante: Q (Dama branca) - índice 4
    {4, 2, 2, 1, 1, 91, 1, 21, 21, 41, 81, 991}, // Q vs todas
    // Atacante: K (Rei branco) - índice 5
    {9, 7, 7, 5, 1, 100, 10, 30, 30, 50, 90, 1000}, // K vs todas

    // Atacante: p (peão preto) - índice 6                     P   N   B   R   Q   K   p   n   b   r   q   k
    {9, 29, 29, 49, 89, 999, 4, 2, 2, 1, 1, 99}, // p vs todas
    // Atacante: n (cavalo preto) - índice 7
    {7, 27, 27, 47, 87, 997, 4, 2, 2, 1, 1, 97}, // n vs todas
    // Atacante: b (bispo preto) - índice 8
    {7, 27, 27, 47, 87, 997, 4, 2, 2, 1, 1, 97}, // b vs todas
    // Atacante: r (torre preta) - índice 9
    {5, 25, 25, 45, 85, 995, 4, 2, 2, 1, 1, 95}, // r vs todas
    // Atacante: q (dama preta) - índice 10
    {1, 21, 21, 41, 81, 991, 4, 2, 2, 1, 1, 91}, // q vs todas
    // Atacante: k (rei preto) - índice 11
    {10, 30, 30, 50, 90, 1000, 9, 7, 7, 5, 1, 100} // k vs todas
};

/**
 * Exemplos de uso da tabela:
 *
 * CAPTURAS OPONENTES (valores altos):
 * Melhor captura: mvv_lva[0][11] = 999 → Peão captura Rei inimigo (máximo valor)
 * Segunda melhor: mvv_lva[0][10] = 89  → Peão captura Dama inimiga
 * Pior captura:   mvv_lva[4][6] = 1    → Dama captura peão inimigo
 *
 * CAPTURAS MESMA COR (valores baixos, para evitar problemas):
 * mvv_lva[0][5] = 99   → Peão "captura" Rei próprio (valor alto mas menor que inimigo)
 * mvv_lva[0][4] = 1    → Peão "captura" Dama própria (valor baixo)
 * mvv_lva[4][0] = 4    → Dama "captura" Peão próprio (valor baixo)
 *
 * Ordem de prioridade para capturas (maior score = melhor):
 * 1. Qualquer peça × Rei inimigo (scores 991-1000) [situação teórica]
 * 2. Qualquer peça × Dama inimiga (scores 81-89)
 * 3. Qualquer peça × Torre inimiga (scores 41-49)
 * 4. Qualquer peça × Cavalo/Bispo inimigo (scores 21-29)
 * 5. Qualquer peça × Peão inimigo (scores 1-9)
 * 6. Rei próprio × qualquer peça (scores 91-100) [situação teórica]
 * 7. Outras capturas da mesma cor (scores 1-9) [situação teórica]
 *
 * Nota: Capturas da mesma cor nunca acontecem no jogo real, mas ter valores
 * consistentes evita problemas de ordenação e divisão por zero.
 */

/**
 * @brief Obtém o score MVV-LVA para uma captura
 * @param atacante Tipo da peça atacante (0-11)
 * @param vitima Tipo da peça vítima (0-11)
 * @return Score MVV-LVA (maior = melhor captura)
 */
int get_mvv_lva_score(int atacante, int vitima)
{
    if (atacante < 0 || atacante >= 12 || vitima < 0 || vitima >= 12)
    {
        return 0; // Captura inválida
    }
    return mvv_lva[atacante][vitima];
}

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

    sort_moves(listaLances);

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

    int em_cheque = casaEstaAtacada((lado_a_jogar == branco) ? getLeastBitIndex(bitboards[K]) : getLeastBitIndex(bitboards[k]), lado_a_jogar ^ 1);

    if (em_cheque)
    {
        depth++; // Extensão de profundidade se em cheque
    }

    int lances_legais = 0;
    int bestMove_temporario = 0;
    int alpha_antigo = alpha;

    lances listaLances[1];
    gerar_lances(listaLances);

    sort_moves(listaLances);

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
    // Para detectar mate em 1, precisa de pelo menos profundidade 2
    if (depth < 2)
    {
        depth = 2;
    }

    nos = 0; // Reset contador de nós
    int score = negamax(-99999, 99999, depth);

    if (bestMove)
    {
        printf("info score cp %d depth %d nodes %ld\n", score, depth, nos);
        printf("bestmove ");
        printLance(bestMove);

        // Detectar e reportar mate
        if (score >= 99990)
        {
            int mate_in = (99999 - score) / 2 + 1;
        }
        else if (score <= -99990)
        {
            int mate_in = (99999 + score) / 2 + 1;
        }

        printf("\n");
    }
    else
    {
        printf("Nenhum lance encontrado!\n");
    }
}

int score_move(int move)
{
    int capturada = get_captura(move);
    int promocao = get_peca_promovida(move);
    int base_score = 0;

    // Primeiro, calcular score da captura (se houver)
    if (capturada)
    {
        // É uma captura, vamos encontrar qual peça foi capturada
        int destino = get_destino(move);
        int atacante = get_peca(move);
        int pecaCapturada = 12; // Valor inválido por padrão

        // Determinar qual peça está sendo capturada no destino
        int pecaInicial, pecaFinal;
        if (lado_a_jogar == branco)
        {
            // Brancas jogam, capturam peças pretas
            pecaInicial = p;
            pecaFinal = k;
        }
        else
        {
            // Pretas jogam, capturam peças brancas
            pecaInicial = P;
            pecaFinal = K;
        }

        // Encontrar qual peça está no destino
        for (int peca = pecaInicial; peca <= pecaFinal; peca++)
        {
            if (getBit(bitboards[peca], destino))
            {
                pecaCapturada = peca;
                break;
            }
        }

        // Se encontrou a peça capturada, usa score MVV-LVA
        if (pecaCapturada != 12)
        {
            base_score = get_mvv_lva_score(atacante, pecaCapturada);
        }
        else
        {
            base_score = 1; // Captura mas não encontrou a peça (erro)
        }
    }

    // // Segundo, adicionar bonus de promoção (se houver)
    // if (promocao)
    // {
    //     if (capturada) {
    //         // Promoção com captura: bonus pequeno (só para desempate)
    //         switch (promocao)
    //         {
    //             case Q: // Dama
    //             case q:
    //                 base_score += 4; // Maior bonus para desempate
    //                 break;
    //             case R: // Torre
    //             case r:
    //                 base_score += 3;
    //                 break;
    //             case B: // Bispo
    //             case b:
    //                 base_score += 2;
    //                 break;
    //             case N: // Cavalo
    //             case n:
    //                 base_score += 1; // Menor bonus (às vezes cavalo é melhor)
    //                 break;
    //             default:
    //                 break;
    //         }
    //     }
    // }

    return base_score;
}

void print_move_scores()
{
    lances listaLances[1];
    gerar_lances(listaLances);

    if (listaLances->contador == 0)
    {
        return;
    }

    for (int i = 0; i < listaLances->contador; i++)
    {
        int move = listaLances->lances[i];
        int score = score_move(move);
        printLance(move);
        printf(" Score: %d\n", score);
    }
}

/**
 * @brief Função auxiliar para trocar dois elementos nos arrays de lances e scores
 */
void swap_moves(lances *listaLances, int *move_scores, int i, int j)
{
    // Trocar lances
    int temp_move = listaLances->lances[i];
    listaLances->lances[i] = listaLances->lances[j];
    listaLances->lances[j] = temp_move;

    // Trocar scores
    int temp_score = move_scores[i];
    move_scores[i] = move_scores[j];
    move_scores[j] = temp_score;
}

/**
 * @brief Particiona o array para quicksort (ordena do maior para o menor score)
 */
int partition(lances *listaLances, int *move_scores, int low, int high)
{
    int pivot = move_scores[high]; // Usa o último elemento como pivot
    int i = low - 1;               // Índice do menor elemento

    for (int j = low; j < high; j++)
    {
        // Se o elemento atual é MAIOR que o pivot (queremos ordem decrescente)
        if (move_scores[j] > pivot)
        {
            i++;
            swap_moves(listaLances, move_scores, i, j);
        }
    }

    swap_moves(listaLances, move_scores, i + 1, high);
    return i + 1;
}

/**
 * @brief Implementação recursiva do quicksort para ordenar lances por score
 */
void quicksort_moves(lances *listaLances, int *move_scores, int low, int high)
{
    if (low < high)
    {
        int pi = partition(listaLances, move_scores, low, high);

        // Recursivamente ordenar elementos antes e depois da partição
        quicksort_moves(listaLances, move_scores, low, pi - 1);
        quicksort_moves(listaLances, move_scores, pi + 1, high);
    }
}

int sort_moves(lances *listaLances)
{
    if (listaLances->contador <= 1)
    {
        return 1; // Não precisa ordenar se tem 0 ou 1 lance
    }

    // Calcular scores para todos os lances
    int move_scores[listaLances->contador];
    for (int i = 0; i < listaLances->contador; i++)
    {
        move_scores[i] = score_move(listaLances->lances[i]);
    }

    // Ordenação por quicksort (muito mais eficiente que bubble sort)
    // Ordena do maior score para o menor (melhores capturas primeiro)
    quicksort_moves(listaLances, move_scores, 0, listaLances->contador - 1);

    return 1;
}