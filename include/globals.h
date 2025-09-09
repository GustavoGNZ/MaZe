/**
 * @file globals.h
 * @brief Declarações de variáveis globais e funções auxiliares da engine MaZe
 * @author GustavoGNZ
 * @version 1.0
 * 
 * Este header declara todas as variáveis globais da engine, incluindo
 * tabelas de ataques, magic numbers, histórico de posições e funções
 * para detecção de repetição.
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include "bitboard.h"

// =============================================================================
// SISTEMA DE HISTÓRICO E DETECÇÃO DE REPETIÇÃO
// =============================================================================

/**
 * @brief Tamanho máximo do histórico de posições
 */
#define MAX_HISTORIA 1000

/**
 * @brief Histórico de hashes de posições para detecção de repetição
 */
extern u64 historico_posicoes[MAX_HISTORIA];

/**
 * @brief Contador atual do histórico de posições
 */
extern int contador_historia;

/**
 * @brief Tamanho máximo do histórico de lances da partida
 */
#define MAX_LANCES_PARTIDA 200

/**
 * @brief Histórico de lances para integração com livro de aberturas
 */
extern int historico_lances_partida[MAX_LANCES_PARTIDA];

/**
 * @brief Número de lances jogados na partida atual
 */
extern int num_lances_partida;

// =============================================================================
// FUNÇÕES DE DETECÇÃO DE REPETIÇÃO
// =============================================================================

/**
 * @brief Adiciona a posição atual ao histórico
 * 
 * Calcula o hash da posição atual e armazena no histórico
 * para futura detecção de repetições.
 */
void adicionar_posicao_historia();

/**
 * @brief Verifica se a posição atual é repetição
 * 
 * @return 1 se posição foi repetida, 0 caso contrário
 */
int posicao_repetida();

/**
 * @brief Limpa todo o histórico de posições
 */
void limpar_historia();

/**
 * @brief Calcula hash simples da posição atual
 * 
 * @return Hash da posição para detecção de repetição
 */
u64 hash_posicao_simples();

// =============================================================================
// TABELAS DE ATAQUES PRÉ-CALCULADAS
// =============================================================================

/**
 * @brief Tabela de ataques de peões [lado][casa]
 */
extern u64 tabela_ataques_peao[2][64];

/**
 * @brief Tabela de ataques de cavalos por casa
 */
extern u64 tabela_ataques_cavalo[64];

/**
 * @brief Tabela de ataques de reis por casa
 */
extern u64 tabela_ataques_rei[64];

/**
 * @brief Máscaras de ataques para bispos (sem bloqueios)
 */
extern u64 mask_tabela_ataques_bispo[64];

/**
 * @brief Máscaras de ataques para torres (sem bloqueios)
 */
extern u64 mask_tabela_ataques_torre[64];

/**
 * @brief Tabela completa de ataques de bispos [casa][índice_ocupação]
 */
extern u64 tabela_ataques_bispo[64][512];

/**
 * @brief Tabela completa de ataques de torres [casa][índice_ocupação]
 */
extern u64 tabela_ataques_torre[64][4096];

// =============================================================================
// MAGIC BITBOARDS - DADOS PRÉ-CALCULADOS
// =============================================================================

/**
 * @brief Número de bits relevantes para magic bitboards de bispos
 */
extern const int bits_relevantes_bispo[64];

/**
 * @brief Número de bits relevantes para magic bitboards de torres
 */
extern const int bits_relevantes_torre[64];

/**
 * @brief Magic numbers para bispos em cada casa
 */
extern u64 magics_bispo[64];

/**
 * @brief Magic numbers para torres em cada casa
 */
extern u64 magics_torre[64];

// =============================================================================
// ESTADO DO JOGO
// =============================================================================

/**
 * @brief Variável global para backup do estado do jogo
 */
extern estado_jogo backup_global;

#endif