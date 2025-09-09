// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include "bitboard.h"

// Estrutura para detecção de repetição
#define MAX_HISTORIA 1000
extern u64 historico_posicoes[MAX_HISTORIA];
extern int contador_historia;

// Funções para detecção de repetição
void adicionar_posicao_historia();
int posicao_repetida();
void limpar_historia();
u64 hash_posicao_simples();

extern u64 tabela_ataques_peao[2][64];
extern u64 tabela_ataques_cavalo[64];
extern u64 tabela_ataques_rei[64];

extern u64 mask_tabela_ataques_bispo[64];
extern u64 mask_tabela_ataques_torre[64];

extern u64 tabela_ataques_bispo[64][512];
extern u64 tabela_ataques_torre[64][4096];

extern const int bits_relevantes_bispo[64];
extern const int bits_relevantes_torre[64];

extern  u64 magics_bispo[64];
extern  u64 magics_torre[64];

// Variável global para backup do estado do jogo
extern estado_jogo backup_global;

// ===== LIVRO DE ABERTURAS =====
#define MAX_ABERTURAS 1000
#define MAX_LANCES_ABERTURA 20
#define MAX_LANCE_STRING 6

// Estrutura para uma abertura
typedef struct {
    char lances[MAX_LANCES_ABERTURA][MAX_LANCE_STRING]; // Array de strings de lances
    int num_lances;                                     // Número de lances na abertura
} abertura;

// Variáveis globais do livro de aberturas
extern abertura livro_aberturas[MAX_ABERTURAS];
extern int num_aberturas_carregadas;
extern int lances_jogados_partida;
extern char historico_lances_partida[MAX_LANCES_ABERTURA][MAX_LANCE_STRING];

// Funções do livro de aberturas
int carregar_livro_aberturas(const char* arquivo);
char* consultar_livro_aberturas();
void adicionar_lance_historico(const char* lance);
void limpar_historico_partida();
int lances_coincidem(const char* lance1, const char* lance2);

#endif