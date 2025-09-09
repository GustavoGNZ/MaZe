/**
 * @file search.h
 * @brief Sistema de busca da engine MaZe
 * @author GustavoGNZ
 * @version 1.0
 * 
 * Este header declara todas as funções e variáveis relacionadas ao
 * algoritmo de busca alfa-beta, incluindo variação principal (PV),
 * killer moves, history heuristic, quiescence search e ordenação de lances.
 */

#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"

// =============================================================================
// VARIÁVEIS GLOBAIS DE BUSCA
// =============================================================================

/**
 * @brief Profundidade atual da busca (ply)
 */
extern int ply;

/**
 * @brief Melhor lance encontrado na busca
 */
extern int bestMove;

/**
 * @brief Máxima profundidade de busca para arrays estáticos
 */
#define MAX_PLY_BUSCA 64

/**
 * @brief Array para detectar repetição na linha de busca atual
 */
extern u64 hash_linha_busca[MAX_PLY_BUSCA];

/**
 * @brief Tabela MVV-LVA para ordenação de capturas
 * 
 * Most Valuable Victim - Least Valuable Attacker
 * [peça_atacante][peça_vítima]
 */
extern int mvv_lva[12][12];

/**
 * @brief Tabela de killer moves [índice][profundidade]
 */
extern int killer_moves[2][64];

/**
 * @brief Tabela de history moves [peça][casa]
 */
extern int history_moves[12][64];

// =============================================================================
// FUNÇÕES DE VARIAÇÃO PRINCIPAL (PV)
// =============================================================================

/**
 * @brief Habilita pontuação para seguir a variação principal
 * 
 * Configura flags para dar prioridade aos lances da linha
 * principal durante a busca.
 * 
 * @param listaLances Lista de lances a verificar
 */
void enable_pv_scoring(lances *listaLances);

// =============================================================================
// ALGORITMOS DE BUSCA
// =============================================================================

/**
 * @brief Busca quiescence (busca de captura)
 * 
 * Estende a busca apenas para capturas para evitar o
 * "horizon effect" e melhorar a avaliação de posições táticas.
 * 
 * @param alpha Valor alfa para poda alfa-beta
 * @param beta Valor beta para poda alfa-beta
 * @return Avaliação da posição após capturas forçadas
 */
int quiescence(int alpha, int beta);

/**
 * @brief Algoritmo principal de busca Negamax com poda alfa-beta
 * 
 * Implementa o algoritmo Negamax com várias otimizações:
 * - Poda alfa-beta
 * - Variação principal (PV)
 * - Killer moves
 * - History heuristic
 * - Quiescence search
 * 
 * @param alpha Valor alfa para poda alfa-beta
 * @param beta Valor beta para poda alfa-beta
 * @param depth Profundidade restante de busca
 * @return Avaliação da posição
 */
int negamax(int alpha, int beta, int depth);

/**
 * @brief Função principal de busca iterativa
 * 
 * Executa busca com profundidade crescente (iterative deepening)
 * até a profundidade especificada, atualizando o melhor lance.
 * 
 * @param depth Profundidade máxima de busca
 */
void busca_lance(int depth);

// =============================================================================
// ORDENAÇÃO E PONTUAÇÃO DE LANCES
// =============================================================================

/**
 * @brief Calcula pontuação MVV-LVA para uma captura
 * 
 * Most Valuable Victim - Least Valuable Attacker:
 * prioriza capturas de peças valiosas com peças menos valiosas.
 * 
 * @param atacante Tipo da peça que captura
 * @param vitima Tipo da peça capturada
 * @return Pontuação da captura
 */
int get_mvv_lva_score(int atacante, int vitima);

/**
 * @brief Calcula pontuação de um lance para ordenação
 * 
 * Atribui pontuação baseada em:
 * - Variação principal
 * - Capturas (MVV-LVA)
 * - Killer moves
 * - History heuristic
 * 
 * @param move Lance a ser pontuado
 * @return Pontuação do lance
 */
int score_move(int move);

/**
 * @brief Ordena lista de lances por pontuação
 * 
 * Ordena lances em ordem decrescente de pontuação para
 * melhorar eficiência da poda alfa-beta.
 * 
 * @param listaLances Lista de lances a ordenar
 * @return Sempre retorna 0 (compatibilidade)
 */
int sort_moves(lances *listaLances);

/**
 * @brief Imprime pontuações dos lances (debug)
 * 
 * Função auxiliar para depuração que mostra as pontuações
 * atribuídas aos lances pela função de ordenação.
 */
void print_move_scores();

// =============================================================================
// FUNÇÕES AUXILIARES DE ORDENAÇÃO
// =============================================================================

/**
 * @brief Troca dois lances na lista
 * 
 * @param listaLances Lista de lances
 * @param move_scores Array de pontuações
 * @param i Índice do primeiro lance
 * @param j Índice do segundo lance
 */
void swap_moves(lances *listaLances, int *move_scores, int i, int j);

/**
 * @brief Função de partição para quicksort
 * 
 * @param listaLances Lista de lances
 * @param move_scores Array de pontuações
 * @param low Índice inferior
 * @param high Índice superior
 * @return Índice do pivô após partição
 */
int partition(lances *listaLances, int *move_scores, int low, int high);

/**
 * @brief Algoritmo quicksort para ordenação de lances
 * 
 * @param listaLances Lista de lances
 * @param move_scores Array de pontuações
 * @param low Índice inferior
 * @param high Índice superior
 */
void quicksort_moves(lances *listaLances, int *move_scores, int low, int high);

#endif