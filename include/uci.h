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

// =============================================================================
// FUNÇÕES DE COMUNICAÇÃO UCI (FUTURAS IMPLEMENTAÇÕES)
// =============================================================================

#endif
