#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/bitboard.h"
#include "../include/ataques.h"
#include "../include/globals.h"
#include "../include/uci.h"

/**
 * @brief Função para analisar um lance no formato UCI
 * 
 * Converte uma string de lance UCI (e2e4, g1f3, e7e8q, etc.) para
 * o formato interno do engine.
 * 
 * @param string_lance String contendo o lance no formato UCI
 * @return Código do lance no formato interno ou -1 se inválido
 */
int parse_move(char *string_lance){
    lances listaLances[1];

    // Gera todos os lances legais para a posição atual
    gerar_lances(listaLances);

    // Calcula casa de origem (a1=0, b1=1, ..., h8=63)
    int origem = (string_lance[0] - 'a') + (string_lance[1] - '1') * 8;
    
    // Calcula casa de destino
    int destino = (string_lance[2] - 'a') + (string_lance[3] - '1') * 8;

    printf("origem: %s, destino: %s\n", casa_nome[origem], casa_nome[destino]);

    // TODO: Implementar busca na lista de lances legais
    // TODO: Verificar se o lance é válido
    // TODO: Retornar o lance codificado ou -1 se inválido
    
    return -1; // Por enquanto retorna -1 (implementação incompleta)
}
