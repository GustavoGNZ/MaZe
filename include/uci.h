/**
 * @file uci.h
 * @brief Interface UCI (Universal Chess Interface) do engine MaZe
 * @author GustavoGNZ
 * @version 1.0
 * 
 * Este header contém as declarações para implementação do protocolo UCI,
 * permitindo comunicação com GUIs de xadrez e outros softwares compatíveis.
 * Inclui funções para:
 * - Parsing de comandos UCI
 * - Conversão entre formatos de lance
 * - Comunicação com interface gráfica
 * - Controle do engine via protocolo padrão
 */

#ifndef UCI_H
#define UCI_H

#include "bitboard.h"

// =============================================================================
// FUNÇÕES DE PARSING E CONVERSÃO
// =============================================================================

/**
 * @brief Analisa um lance no formato UCI
 * 
 * Converte uma string de lance no formato UCI (ex: "e2e4", "g1f3", "e7e8q")
 * para o formato interno do engine. Suporta movimentos normais, capturas,
 * roques e promoções.
 * 
 * Exemplos de formato UCI:
 * - "e2e4": Peão de e2 para e4
 * - "g1f3": Cavalo de g1 para f3  
 * - "e7e8q": Promoção de peão para dama
 * - "e1g1": Roque pequeno do rei branco
 * 
 * @param string_lance String contendo o lance no formato UCI
 * @return Código do lance no formato interno ou -1 se inválido
 */
int parse_move(char *string_lance);

/**
 * @brief Processa comando UCI "position"
 * 
 * Analisa e configura a posição do tabuleiro baseada no comando UCI.
 * Suporta tanto "position startpos" quanto "position fen <fen_string>".
 * Também processa a lista de movimentos se fornecida.
 * 
 * @param string_posicao String completa do comando position
 * @param backup Estrutura para backup do estado do jogo
 */
void parse_position(char *string_posicao, estado_jogo backup);

/**
 * @brief Processa comando UCI "go"
 * 
 * Analisa e executa o comando de busca UCI. Processa parâmetros como
 * tempo de busca, profundidade, número de nós e outros controles.
 * 
 * @param string_go String completa do comando go
 */
void parse_go(char *string_go);


#endif
