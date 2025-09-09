/**
 * @file aberturas.h
 * @brief Sistema de livro de aberturas para o MaZe
 * @author GustavoGNZ
 * @version 1.0
 * 
 * Este header contém as declarações para o sistema de livro de aberturas,
 * permitindo que a engine jogue variações diferentes a cada partida.
 */

#ifndef ABERTURAS_H
#define ABERTURAS_H

#include "bitboard.h"

// =============================================================================
// ESTRUTURAS DE DADOS
// =============================================================================

#define MAX_LANCES_ABERTURA 20  // Máximo de lances por linha de abertura
#define MAX_LINHAS_ABERTURA 200 // Máximo de linhas no arquivo de aberturas
#define MAX_NOME_ABERTURA 100   // Tamanho máximo do nome da abertura

/**
 * @brief Estrutura para armazenar uma linha de abertura
 */
typedef struct {
    char nome[MAX_NOME_ABERTURA];           // Nome da abertura (ex: "Ruy Lopez")
    char lances_str[MAX_LANCES_ABERTURA][6]; // Lances em notação UCI (ex: "e2e4")
    int lances_codificados[MAX_LANCES_ABERTURA]; // Lances no formato interno
    int num_lances;                          // Número de lances nesta linha
} linha_abertura_t;

/**
 * @brief Estrutura principal do livro de aberturas
 */
typedef struct {
    linha_abertura_t linhas[MAX_LINHAS_ABERTURA];
    int num_linhas;
    int inicializado;
} livro_aberturas_t;

// Variável global do livro
extern livro_aberturas_t livro_aberturas;

// =============================================================================
// FUNÇÕES PÚBLICAS
// =============================================================================

/**
 * @brief Inicializa o sistema de livro de aberturas
 * 
 * Carrega o arquivo aberturas.txt e processa todas as linhas,
 * convertendo os lances UCI para formato interno.
 * 
 * @return 1 se sucesso, 0 se erro
 */
int inicializar_livro_aberturas();

/**
 * @brief Busca um lance de abertura para a posição atual
 * 
 * Analisa a posição atual (histórico de lances) e busca no livro
 * de aberturas se há alguma linha que corresponde. Se encontrar,
 * retorna um lance aleatório entre as opções válidas.
 * 
 * @param historico_lances Array com os lances jogados até agora
 * @param num_lances_jogados Número de lances já jogados
 * @return Lance codificado se encontrado, 0 se não há lance no livro
 */
int buscar_lance_abertura(int historico_lances[], int num_lances_jogados);

/**
 * @brief Verifica se a engine ainda está "em livro"
 * 
 * Determina se a posição atual ainda corresponde a alguma linha
 * conhecida do livro de aberturas.
 * 
 * @param historico_lances Array com os lances jogados até agora
 * @param num_lances_jogados Número de lances já jogados
 * @return 1 se ainda está em livro, 0 caso contrário
 */
int esta_em_livro(int historico_lances[], int num_lances_jogados);

/**
 * @brief Obtém o número do lance atual baseado no FEN
 * 
 * Extrai o número do lance do FEN da posição atual.
 * Formato FEN: "... w KQkq - 0 5" -> lance 5
 * 
 * @return Número do lance atual (1, 2, 3, ...)
 */
int obter_numero_lance_atual();

/**
 * @brief Obtém informações detalhadas do lance atual
 * 
 * @param numero_lance Ponteiro para receber o número do lance
 * @param eh_brancas Ponteiro para receber se é vez das brancas (1) ou pretas (0)
 */
void obter_info_lance_atual(int* numero_lance, int* eh_brancas);

/**
 * @brief Libera recursos do livro de aberturas
 */
void finalizar_livro_aberturas();

// =============================================================================
// FUNÇÕES AUXILIARES
// =============================================================================

/**
 * @brief Converte lance UCI para formato interno
 * 
 * @param lance_uci String UCI (ex: "e2e4")
 * @return Lance codificado ou 0 se inválido
 */
int converter_uci_para_interno(const char* lance_uci);

/**
 * @brief Compara histórico com linha do livro
 * 
 * @param historico Array de lances jogados
 * @param num_jogados Número de lances jogados
 * @param linha Linha do livro para comparar
 * @return 1 se corresponde, 0 caso contrário
 */
int comparar_historico_com_linha(int historico[], int num_jogados, linha_abertura_t* linha);

#endif // ABERTURAS_H