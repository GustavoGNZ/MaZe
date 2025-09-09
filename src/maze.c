/**
 * @file maze.c
 * @brief Arquivo principal da engine de xadrez MaZe
 * @author GustavoGNZ
 * @version 1.0
 * 
 * Este arquivo contém o ponto de entrada principal da engine MaZe,
 * incluindo definições de posições FEN para teste e inicialização
 * dos sistemas da engine.
 */

// =============================================================================
// INCLUDES
// =============================================================================

#include "../include/bitboard.h"
#include "../include/ataques.h"
#include "../include/globals.h"
#include "../include/uci.h"
#include "../include/search.h"
#include "../include/evaluate.h"
#include <stdio.h>
#include <stdlib.h>

// =============================================================================
// FUNÇÃO PRINCIPAL
// =============================================================================

/**
 * @brief Função principal da engine MaZe
 * 
 * Inicializa todos os sistemas necessários da engine e entra no loop
 * principal do protocolo UCI para comunicação com interfaces gráficas.
 * 
 * @return 0 em caso de execução bem-sucedida
 */
int main()
{
    runEngine();  // Inicializa sistemas da engine (ataques, avaliação, etc.)
    uci_loop();   // Entra no loop principal UCI
    
    return 0;
}
