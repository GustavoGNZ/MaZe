#include "../include/search.h"
#include "../include/evaluate.h"
#include "../include/bitboard.h"
#include "../include/globals.h"
#include "../include/ataques.h"

int ply;

#define MAX_PLY 64

// Array para detectar repetição na linha de busca atual
u64 hash_linha_busca[MAX_PLY_BUSCA];

// Função para detectar repetição na linha de busca atual
int detectar_repeticao_busca() {
    u64 hash_atual = hash_posicao_simples();
    
    // Verificar se esta posição já apareceu na linha de busca atual
    for (int i = 0; i < ply; i++) {
        if (hash_linha_busca[i] == hash_atual) {
            return 1; // Repetição detectada
        }
    }
    return 0; // Não é repetição
}

int killer_moves[2][MAX_PLY];
int history_moves[12][MAX_PLY];

int pv_length[MAX_PLY];
int pv_table[MAX_PLY][MAX_PLY];

int follow_pv, score_pv;

void enable_pv_scoring(lances *listaLances) {
    follow_pv = 0;

    for(int i = 0; i < listaLances->contador; i++) {
        if(pv_table[0][ply] == listaLances->lances[i]) {
            follow_pv = 1;
            score_pv = 1;
            break;
        }
    }
}

/*
TABELA TRIANGULAR DA VARIAÇÃO PRINCIPAL (PV Table)

A tabela triangular da variação principal é uma estrutura de dados usada em motores de xadrez
para armazenar e recuperar a melhor sequência de lances encontrada durante a busca.

ESTRUTURA:
- É chamada "triangular" porque cada nível da busca armazena menos lances que o anterior
- No ply 0 (raiz): armazena toda a variação principal
- No ply 1: armazena a variação principal menos o primeiro lance
- No ply 2: armazena a variação principal menos os dois primeiros lances
- E assim por diante...

EXEMPLO DE ESTRUTURA:
pv_table[ply][ply_index] onde:
- ply: profundidade atual da busca (0 = raiz)
- ply_index: índice do lance na variação principal

ply 0: [e4, e5, Nf3, Nc6, Bb5, a6]  <- Variação completa
ply 1:     [e5, Nf3, Nc6, Bb5, a6]  <- Sem o primeiro lance
ply 2:         [Nf3, Nc6, Bb5, a6]  <- Sem os dois primeiros
ply 3:              [Nc6, Bb5, a6]  <- E assim por diante...

COMO FUNCIONA:
1. Quando encontramos um novo melhor lance, atualizamos a PV table
2. Copiamos a variação do nível filho e adicionamos o lance atual no início
3. O comprimento da variação diminui conforme descemos na árvore

VANTAGENS:
- Permite mostrar a melhor linha de jogo encontrada
- Útil para debugging e análise
- Melhora a ordenação de lances em buscas subsequentes
- Implementação eficiente em memória

*/

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

    int found_pv;

    int score;

    pv_length[ply] = ply;

    if (depth == 0)
    {
        return quiescence(alpha, beta);
    }

    if (ply > MAX_PLY - 1)
    {
        return evaluate();
    }

    // Detectar repetição na linha de busca
    if (detectar_repeticao_busca()) {
        return 0; // Empate por repetição
    }

    // Armazenar hash da posição atual na linha de busca
    hash_linha_busca[ply] = hash_posicao_simples();

    nos++;

    int em_cheque = casaEstaAtacada((lado_a_jogar == branco) ? getLeastBitIndex(bitboards[K]) : getLeastBitIndex(bitboards[k]), lado_a_jogar ^ 1);

    if (em_cheque)
    {
        depth++; // Extensão de profundidade se em cheque
    }

    int lances_legais = 0;



    lances listaLances[1];
    gerar_lances(listaLances);

    if(follow_pv){
        enable_pv_scoring(listaLances);
    }

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

        if(found_pv){
            score = -negamax(-alpha - 1, -alpha, depth - 1);

            if((score > alpha) && (score < beta)){
                score = -negamax(-beta, -alpha, depth - 1); // Re-search
            }
        } else {
            score = -negamax(-beta, -alpha, depth - 1);
        }

        RESTAURAR_ESTADO(backup_local); // Restaurar do backup local
        ply--;

        if (score >= beta)
        {
            if (get_captura(listaLances->lances[i]))
            {
                // Atualizar killer moves
                killer_moves[1][ply] = killer_moves[0][ply];
                killer_moves[0][ply] = listaLances->lances[i];
            }

            return beta; // Poda beta
        }

        if (score > alpha)
        {
            if (get_captura(listaLances->lances[i]))
            {
                history_moves[get_peca(listaLances->lances[i])][get_destino(listaLances->lances[i])] += 1;
            }

            alpha = score;

            found_pv = 1;

            // Atualizar a tabela PV
            pv_table[ply][ply] = listaLances->lances[i];

            for (int next_ply = ply + 1; next_ply < pv_length[ply + 1]; next_ply++)
            {
                pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];
            }
            pv_length[ply] = pv_length[ply + 1];
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

    return alpha;
}

void busca_lance(int depth)
{
    int score;
    nos = 0;
    follow_pv = 0;
    score_pv = 0;

    memset(killer_moves, 0, sizeof(killer_moves));
    memset(history_moves, 0, sizeof(history_moves));
    memset(pv_length, 0, sizeof(pv_length));
    memset(pv_table, 0, sizeof(pv_table));
    memset(hash_linha_busca, 0, sizeof(hash_linha_busca));

    // Para detectar mate em 1, precisa de pelo menos profundidade 2
    if (depth < 2)
    {
        depth = 2;
    }

    // Iterative deepening
    for (int i = 1; i <= depth; i++)
    {

        follow_pv = 1;
        score = negamax(-99999, 99999, i);

        printf("info score cp %d depth %d nodes %ld pv ", score, i, nos);

        for (int j = 0; j < pv_length[0]; j++)
        {
            printLance(pv_table[0][j]);
            printf(" ");
        }

        printf("\n");
    }

    printf("\n");

    printf("bestmove ");
    printLance(pv_table[0][0]); // é o melhor lance da linha principal
    printf("\n");
}



int score_move(int move)
{
    int capturada = get_captura(move);
    int promocao = get_peca_promovida(move);
    int destino = get_destino(move);
    int atacante = get_peca(move);

    int base_score = 0;

    if(score_pv){
        if(pv_table[0][ply] == move){

            score_pv = 0;
            return 10000; // Melhor lance da linha principal
        }
    }

    // Primeiro, calcular score da captura (se houver)
    if (capturada)
    {
        // É uma captura, vamos encontrar qual peça foi capturada
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

        return get_mvv_lva_score(atacante, pecaCapturada);
    }
    else
    {
        // killer moves e history moves

        if (killer_moves[0][ply] == move)
        {
            return 90; // Entre as piores capturas (1-9) e melhores capturas (21-1000)
        }
        else if (killer_moves[1][ply] == move)
        {
            return 80; // Segundo killer move com score menor
        }
        else
        {
            return history_moves[atacante][destino];
        }
    }

    return 0;
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