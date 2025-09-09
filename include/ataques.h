/**
 * @file ataques.h
 * @brief Sistema de geração de ataques e lances da engine MaZe
 * @author GustavoGNZ
 * @version 1.0
 * 
 * Este header declara todas as funções relacionadas à geração de ataques,
 * magic bitboards, números aleatórios e geração de movimentos legais.
 * Inclui desde funções básicas de utilidade até o sistema completo
 * de magic bitboards para peças deslizantes.
 */

#ifndef ATAQUES_H
#define ATAQUES_H

#include "bitboard.h"

// =============================================================================
// INICIALIZAÇÃO E CONTROLE DA ENGINE
// =============================================================================

/**
 * @brief Inicializa todos os sistemas da engine
 * 
 * Função principal de inicialização que configura todas as tabelas
 * de ataques, máscaras de avaliação e sistemas necessários para
 * o funcionamento da engine.
 */
void runEngine();

// =============================================================================
// FUNÇÕES DE TEMPO E NÚMEROS ALEATÓRIOS
// =============================================================================

/**
 * @brief Obtém o tempo atual em milissegundos
 * 
 * Função multiplataforma para obter timestamp preciso,
 * usado para medição de performance e seeds aleatórias.
 * 
 * @return Tempo atual em milissegundos
 */
int get_tempo_milisegundos();

/**
 * @brief Gera número aleatório de 32 bits usando xorshift32
 * 
 * Implementação consistente entre plataformas para geração
 * de números pseudo-aleatórios, evitando diferenças entre
 * implementações de rand() em diferentes sistemas.
 * 
 * @return Número aleatório de 32 bits
 */
unsigned int gerarNumeroAleatorio32bits();

/**
 * @brief Gera número aleatório de 64 bits
 * 
 * Combina quatro números de 32 bits para criar um número
 * aleatório de 64 bits adequado para magic numbers.
 * 
 * @return Número aleatório de 64 bits
 */
u64 gerarNumeroAleatorio64bits();

// =============================================================================
// SISTEMA DE MAGIC BITBOARDS
// =============================================================================

/**
 * @brief Inicializa magic numbers para todas as casas (DEPRECATED)
 * 
 * Função para gerar magic numbers dinamicamente. Atualmente não
 * é usada pois os valores estão hard-coded para performance.
 * Mantida para referência histórica.
 */
void inicializarMagicNumbers();

/**
 * @brief Gera um magic number candidato
 * 
 * Cria um número com densidade de bits adequada para
 * ser testado como magic number.
 * 
 * @return Magic number candidato
 */
u64 gerarMagicNumber();

/**
 * @brief Encontra magic number válido para uma casa específica
 * 
 * Testa magic numbers até encontrar um que produza mapeamento
 * perfeito para todas as ocupações possíveis da peça na casa.
 * 
 * @param casa Casa do tabuleiro (0-63)
 * @param bits_relevantes Número de bits relevantes para a casa
 * @param peca Tipo da peça (bispo ou torre)
 * @return Magic number válido ou 0 se não encontrado
 */
u64 encontrarMagicNumber(int casa, int bits_relevantes, int peca);

// =============================================================================
// FUNÇÕES AUXILIARES DE TABULEIRO
// =============================================================================

/**
 * @brief Verifica se uma coordenada está dentro do tabuleiro
 * 
 * @param linha Linha da coordenada (0-7)
 * @param coluna Coluna da coordenada (0-7)
 * @return 1 se dentro do tabuleiro, 0 caso contrário
 */
int estaDentroDoTabuleiro(int linha, int coluna);

/**
 * @brief Verifica se é a última casa na direção especificada
 * 
 * @param linha Linha atual
 * @param coluna Coluna atual
 * @param deslocamento_linha Deslocamento na linha
 * @param deslocamento_coluna Deslocamento na coluna
 * @return 1 se é última casa, 0 caso contrário
 */
int ehUltimaCasaNaDirecao(int linha, int coluna, int deslocamento_linha, int deslocamento_coluna);

// =============================================================================
// GERAÇÃO DE ATAQUES DIRECIONAIS
// =============================================================================

/**
 * @brief Marca ataques direcionais sem considerar bloqueios
 * 
 * @param ataque Ponteiro para bitboard de ataques a ser preenchido
 * @param linha Linha da peça
 * @param coluna Coluna da peça
 * @param deslocamentos Array de direções [linha, coluna]
 */
void marcarAtaquesDirecionais(u64 *ataque, int linha, int coluna, int deslocamentos[][2]);

/**
 * @brief Marca ataques direcionais considerando bloqueios
 * 
 * @param ataque Ponteiro para bitboard de ataques a ser preenchido
 * @param linha Linha da peça
 * @param coluna Coluna da peça
 * @param deslocamentos Array de direções [linha, coluna]
 * @param num_direcoes Número de direções a processar
 * @param bitboard_ocupacao Bitboard com posições ocupadas
 */
void marcarAtaquesDirecionaisComBloqueio(u64 *ataque, int linha, int coluna, int deslocamentos[][2], int num_direcoes, u64 bitboard_ocupacao);

// =============================================================================
// INICIALIZAÇÃO DE TABELAS DE ATAQUES
// =============================================================================

/**
 * @brief Inicializa todas as tabelas de ataques pré-calculadas
 * 
 * Preenche tabelas para peões, cavalos, reis e peças deslizantes.
 * Deve ser chamada uma vez durante a inicialização da engine.
 */
void inicializarAtaquesPecas();

/**
 * @brief Inicializa tabelas para peças deslizantes (bispo/torre)
 * 
 * @param peca Tipo da peça (bispo ou torre)
 */
void inicializarAtaquesPecasDeslizantes(int peca);

// =============================================================================
// GERAÇÃO DE ATAQUES POR TIPO DE PEÇA
// =============================================================================

/**
 * @brief Gera ataques de peão para uma casa específica
 * 
 * @param lado Lado do peão (branco ou preto)
 * @param casa Casa do peão (0-63)
 * @return Bitboard com casas atacadas pelo peão
 */
u64 gerarAtaquesPeao(int lado, int casa);

/**
 * @brief Gera ataques de cavalo para uma casa específica
 * 
 * @param casa Casa do cavalo (0-63)
 * @return Bitboard com casas atacadas pelo cavalo
 */
u64 gerarAtaquesCavalo(int casa);

/**
 * @brief Gera ataques de rei para uma casa específica
 * 
 * @param casa Casa do rei (0-63)
 * @return Bitboard com casas atacadas pelo rei
 */
u64 gerarAtaquesRei(int casa);

/**
 * @brief Gera máscara de ataques de bispo (sem bloqueios)
 * 
 * @param casa Casa do bispo (0-63)
 * @return Bitboard com máscara de ataques
 */
u64 gerarAtaquesBispo(int casa);

/**
 * @brief Gera máscara de ataques de torre (sem bloqueios)
 * 
 * @param casa Casa da torre (0-63)
 * @return Bitboard com máscara de ataques
 */
u64 gerarAtaquesTorre(int casa);

/**
 * @brief Gera ataques de bispo considerando bloqueios
 * 
 * @param casa Casa do bispo (0-63)
 * @param bitboard_ocupacao Bitboard com posições ocupadas
 * @return Bitboard com ataques reais do bispo
 */
u64 gerarAtaquesBispoComBloqueio(int casa, u64 bitboard_ocupacao);

/**
 * @brief Gera ataques de torre considerando bloqueios
 * 
 * @param casa Casa da torre (0-63)
 * @param bitboard_ocupacao Bitboard com posições ocupadas
 * @return Bitboard com ataques reais da torre
 */
u64 gerarAtaquesTorreComBloqueio(int casa, u64 bitboard_ocupacao);

// =============================================================================
// CONSULTA RÁPIDA DE ATAQUES (MAGIC BITBOARDS)
// =============================================================================

/**
 * @brief Obtém ataques de bispo usando magic bitboards
 * 
 * @param casa Casa do bispo (0-63)
 * @param ocupacao Bitboard com ocupação atual
 * @return Bitboard com ataques do bispo
 */
u64 obterAtaquesBispo(int casa, u64 ocupacao);

/**
 * @brief Obtém ataques de torre usando magic bitboards
 * 
 * @param casa Casa da torre (0-63)
 * @param ocupacao Bitboard com ocupação atual
 * @return Bitboard com ataques da torre
 */
u64 obterAtaquesTorre(int casa, u64 ocupacao);

/**
 * @brief Obtém ataques de dama (bispo + torre)
 * 
 * @param casa Casa da dama (0-63)
 * @param ocupacao Bitboard com ocupação atual
 * @return Bitboard com ataques da dama
 */
u64 obterAtaquesDama(int casa, u64 ocupacao);

// =============================================================================
// FUNÇÕES AUXILIARES DE MAGIC BITBOARDS
// =============================================================================

/**
 * @brief Define ocupação baseada em índice e máscara
 * 
 * Converte um índice em uma configuração específica de ocupação
 * baseada na máscara de bits relevantes.
 * 
 * @param index Índice da ocupação
 * @param qtde_bits Quantidade de bits relevantes
 * @param mask Máscara de bits relevantes
 * @return Bitboard representando a ocupação
 */
u64 set_occupancy(int index, int qtde_bits, u64 mask);

// =============================================================================
// VERIFICAÇÃO DE ATAQUES
// =============================================================================

/**
 * @brief Verifica se uma casa está sendo atacada por um lado
 * 
 * @param casa Casa a verificar (0-63)
 * @param lado Lado que pode estar atacando
 * @return 1 se casa está atacada, 0 caso contrário
 */
int casaEstaAtacada(int casa, int lado);

// =============================================================================
// GERAÇÃO DE MOVIMENTOS
// =============================================================================

/**
 * @brief Gera movimentos de peão branco
 * 
 * @param origem Casa de origem do peão
 * @param listaLances Lista onde adicionar os lances gerados
 */
void gerar_movimentos_peao_branco(int origem, lances *listaLances);

/**
 * @brief Gera movimentos de peão preto
 * 
 * @param origem Casa de origem do peão
 * @param listaLances Lista onde adicionar os lances gerados
 */
void gerar_movimentos_peao_preto(int origem, lances *listaLances);

/**
 * @brief Gera movimentos de roque para brancas
 * 
 * @param listaLances Lista onde adicionar os lances de roque
 */
void gerar_roque_branco(lances *listaLances);

/**
 * @brief Gera movimentos de roque para pretas
 * 
 * @param listaLances Lista onde adicionar os lances de roque
 */
void gerar_roque_preto(lances *listaLances);

/**
 * @brief Gera movimentos para peças grandes (cavalo, bispo, torre, dama, rei)
 * 
 * @param origem Casa de origem da peça
 * @param lado Lado da peça (branco ou preto)
 * @param tipo_peca Tipo da peça
 * @param listaLances Lista onde adicionar os lances gerados
 */
void gerar_movimentos_pecaGrande(int origem, int lado, int tipo_peca, lances *listaLances);

/**
 * @brief Gera todos os lances legais para a posição atual
 * 
 * Função principal que gera todos os movimentos possíveis
 * para o lado que deve jogar na posição atual.
 * 
 * @param listaLances Lista onde armazenar todos os lances gerados
 */
void gerar_lances(lances *listaLances);

/**
 * @brief Executa um lance no tabuleiro
 * 
 * Aplica um lance ao estado atual do tabuleiro, atualizando
 * todas as estruturas de dados necessárias. Pode ser desfeito
 * posteriormente usando o backup fornecido.
 * 
 * @param lance Lance a ser executado
 * @param flag Tipo de lance (todos os lances ou apenas capturas)
 * @param backup Estado do jogo para possível desfazer
 * @return 1 se lance foi executado com sucesso, 0 se inválido
 */
int fazer_lance(int lance, int flag, estado_jogo backup);

#endif
