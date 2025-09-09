/**
 * @file evaluate.h
 * @brief Sistema de avaliação de posições da engine MaZe
 * @author GustavoGNZ
 * @version 1.0
 * 
 * Este header declara todas as funções e variáveis relacionadas à
 * avaliação de posições, incluindo valores de material, tabelas
 * posicionais, estrutura de peões, mobilidade e segurança do rei.
 */

#ifndef EVALUATE_H
#define EVALUATE_H

#include "bitboard.h"

// =============================================================================
// VALORES E TABELAS DE AVALIAÇÃO
// =============================================================================

/**
 * @brief Valores de material para cada tipo de peça
 */
extern int material_score[12];

/**
 * @brief Tabela posicional para peões
 */
extern int pawn_table[64];

/**
 * @brief Tabela posicional para cavalos
 */
extern int knight_table[64];

/**
 * @brief Tabela posicional para bispos
 */
extern int bishop_table[64];

/**
 * @brief Tabela posicional para torres
 */
extern int rook_table[64];

/**
 * @brief Tabela posicional para damas
 */
extern int queen_table[64];

/**
 * @brief Tabela posicional para reis no meio-jogo
 */
extern int king_table_mg[64];

/**
 * @brief Tabela posicional para reis no final
 */
extern int king_table_eg[64];

// =============================================================================
// MÁSCARAS DE AVALIAÇÃO
// =============================================================================

/**
 * @brief Máscaras para colunas do tabuleiro
 */
extern u64 coluna_masks[64];

/**
 * @brief Máscaras para linhas do tabuleiro
 */
extern u64 linha_masks[64];

/**
 * @brief Máscaras para detecção de peões isolados
 */
extern u64 peao_isolado_masks[64];

/**
 * @brief Máscaras para detecção de peões passados brancos
 */
extern u64 peao_passado_branco_masks[64];

/**
 * @brief Máscaras para detecção de peões passados pretos
 */
extern u64 peao_passado_preto_masks[64];

/**
 * @brief Array para conversão de casa para linha
 */
extern int get_linha[64];

// =============================================================================
// PARÂMETROS DE ESTRUTURA DE PEÕES
// =============================================================================

/**
 * @brief Penalidade por peões dobrados
 */
extern int peao_dobrado_penalidade;

/**
 * @brief Penalidade por peões isolados
 */
extern int peao_isolado_penalidade;

/**
 * @brief Bônus por peão passado baseado na linha
 */
extern int peao_passado_bonus[8];

/**
 * @brief Bônus por torre em coluna semi-livre
 */
extern int coluna_semilivre_bonus;

/**
 * @brief Bônus por torre em coluna livre
 */
extern int coluna_livre_bonus;

/**
 * @brief Penalidade por rei em coluna livre
 */
extern int rei_coluna_livre_penalidade;

/**
 * @brief Penalidade por rei em coluna semi-livre
 */
extern int rei_coluna_semilivre_penalidade;

// =============================================================================
// PARÂMETROS DE MOBILIDADE
// =============================================================================

/**
 * @brief Bônus por movimento legal de cavalo
 */
extern int mobility_bonus_knight;

/**
 * @brief Bônus por movimento legal de bispo
 */
extern int mobility_bonus_bishop;

/**
 * @brief Bônus por movimento legal de torre
 */
extern int mobility_bonus_rook;

/**
 * @brief Bônus por movimento legal de dama
 */
extern int mobility_bonus_queen;

// =============================================================================
// PARÂMETROS DE SEGURANÇA DO REI
// =============================================================================

/**
 * @brief Bônus por peão protetor do rei
 */
extern int king_safety_pawn_shield_bonus;

/**
 * @brief Penalidade por linha/coluna aberta perto do rei
 */
extern int king_safety_open_file_penalty;

/**
 * @brief Penalidade por casa atacada ao redor do rei
 */
extern int king_safety_attacked_square_penalty;

/**
 * @brief Penalidade por rei diretamente atacado
 */
extern int king_safety_king_attacked_penalty;

// =============================================================================
// FUNÇÕES DE INICIALIZAÇÃO DE MÁSCARAS
// =============================================================================

/**
 * @brief Cria máscara para coluna e linha específicas
 * 
 * @param file Coluna (0-7)
 * @param rank Linha (0-7)
 * @return Bitboard com a máscara criada
 */
u64 set_file_rank_masks(int file, int rank);

/**
 * @brief Cria máscara para peão passado branco
 * 
 * @param file Coluna do peão
 * @param rank Linha do peão
 * @return Bitboard com casas que bloqueiam o peão
 */
u64 set_passed_pawn_mask_white(int file, int rank);

/**
 * @brief Cria máscara para peão passado preto
 * 
 * @param file Coluna do peão
 * @param rank Linha do peão
 * @return Bitboard com casas que bloqueiam o peão
 */
u64 set_passed_pawn_mask_black(int file, int rank);

/**
 * @brief Inicializa todas as máscaras de avaliação
 * 
 * Deve ser chamada uma vez durante a inicialização da engine
 * para preencher todas as máscaras de avaliação.
 */
void init_evaluation_masks();

// =============================================================================
// FUNÇÕES DE AVALIAÇÃO ESPECÍFICAS
// =============================================================================

/**
 * @brief Avalia vantagem material
 * 
 * @return Pontuação baseada na diferença de material
 */
int evaluate_material();

/**
 * @brief Avalia fatores posicionais
 * 
 * Inclui tabelas posicionais para cada tipo de peça.
 * 
 * @return Pontuação posicional
 */
int evaluate_positional();

/**
 * @brief Avalia estrutura de peões
 * 
 * Considera peões isolados, dobrados, passados e conectados.
 * 
 * @return Pontuação da estrutura de peões
 */
int evaluate_pawn_structure();

/**
 * @brief Avalia controle de colunas abertas
 * 
 * @return Pontuação baseada em colunas livres e semi-livres
 */
int evaluate_open_files();

/**
 * @brief Avalia mobilidade das peças
 * 
 * Conta movimentos legais disponíveis para cada peça.
 * 
 * @return Pontuação de mobilidade
 */
int evaluate_mobility();

/**
 * @brief Avalia segurança do rei
 * 
 * Considera proteção por peões, colunas abertas próximas
 * e casas atacadas ao redor do rei.
 * 
 * @return Pontuação de segurança do rei
 */
int evaluate_king_safety();

// =============================================================================
// FUNÇÃO PRINCIPAL DE AVALIAÇÃO
// =============================================================================

/**
 * @brief Função principal de avaliação da posição
 * 
 * Combina todos os fatores de avaliação (material, posicional,
 * estrutura de peões, mobilidade, segurança do rei) em uma
 * pontuação final da perspectiva do lado que deve jogar.
 * 
 * @return Avaliação da posição em centipawns
 */
int evaluate();

#endif