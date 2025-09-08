#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/bitboard.h"
#include "../include/ataques.h"
#include "../include/globals.h"
#include "../include/uci.h"
#include "../include/search.h"
#include "../include/evaluate.h"

#define posicaoInicial "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

/**
 * @brief Função para analisar um lance no formato UCI
 *
 * Converte uma string de lance UCI (e2e4, g1f3, e7e8q, etc.) para
 * o formato interno do engine.
 *
 * @param string_lance String contendo o lance no formato UCI
 * @return Código do lance no formato interno ou -1 se inválido
 */
int parse_move(char *string_lance)
{
    lances listaLances[1];

    // Gera todos os lances legais para a posição atual
    gerar_lances(listaLances);

    // Calcula casa de origem (a1=0, b1=1, ..., h8=63)
    int origem = (string_lance[0] - 'a') + (string_lance[1] - '1') * 8;

    // Calcula casa de destino
    int destino = (string_lance[2] - 'a') + (string_lance[3] - '1') * 8;

    // Verifica se há promoção especificada no lance UCI
    char promocao_char = (strlen(string_lance) >= 5) ? string_lance[4] : '\0';

    for (int i = 0; i < listaLances->contador; i++)
    {
        int lance = listaLances->lances[i];
        if (get_origem(lance) == origem && get_destino(lance) == destino)
        {
            int promovida = get_peca_promovida(lance);

            // Se não há promoção no lance UCI e o lance também não tem promoção
            if (!promocao_char && !promovida)
            {
                return lance;
            }

            // Se há promoção no lance UCI, verifica se corresponde
            if (promocao_char && promovida)
            {
                if ((promovida == Q || promovida == q) && promocao_char == 'q')
                {
                    return lance;
                }
                if ((promovida == R || promovida == r) && promocao_char == 'r')
                {
                    return lance;
                }
                if ((promovida == B || promovida == b) && promocao_char == 'b')
                {
                    return lance;
                }
                if ((promovida == N || promovida == n) && promocao_char == 'n')
                {
                    return lance;
                }
            }
        }
    }
    return 0;
}

// uci
void parse_position(char *string_posicao)
{

    string_posicao += 9; // pula "position "

    char *token = string_posicao;

    if (strncmp(string_posicao, "startpos", 8) == 0)
    {
        parseFEN(posicaoInicial);
    }
    else
    {
        token = strstr(string_posicao, "fen"); // pula "fen "

        if (token == NULL)
        {
            parseFEN(posicaoInicial);
        }
        else
        {
            token += 4; // pula "fen "
            parseFEN(token);
        }
    }

    // Verifica se há o comando "moves"
    char *moves_token = strstr(string_posicao, "moves");
    if (moves_token != NULL)
    {
        moves_token += 6; // pula "moves "

        // Pula espaços iniciais
        while (*moves_token == ' ')
        {
            moves_token++;
        }

        while (*moves_token != '\0')
        {
            // Extrai o lance atual (até o próximo espaço ou fim da string)
            char lance_str[8] = {0}; // buffer para o lance
            int i = 0;

            // Copia caracteres do lance até encontrar espaço ou fim
            while (*moves_token != ' ' && *moves_token != '\0' && i < 7)
            {
                lance_str[i] = *moves_token;
                moves_token++;
                i++;
            }
            lance_str[i] = '\0'; // termina a string

            if (strlen(lance_str) >= 4) // lance válido deve ter pelo menos 4 caracteres
            {
                int lance = parse_move(lance_str);

                if (lance != 0)
                {
                    if (!fazer_lance(lance, todosLances, backup_global))
                    {
                        // printf("Erro ao executar lance: %s\n", lance_str);
                        break; // Erro ao executar lance
                    }
                }
                else
                {
                    // printf("Lance inválido: %s\n", lance_str);
                    break; // Lance inválido
                }
            }

            // Pula espaços para o próximo lance
            while (*moves_token == ' ')
            {
                moves_token++;
            }
        }
    }
}

void parse_go(char *string_go)
{
    int profundidade = -1;

    char *profundidade_atual = NULL;

    if ((profundidade_atual = strstr(string_go, "depth")))
    {
        profundidade = atoi(profundidade_atual + 6);
    }
    else
    {
        profundidade = 6; // valor padrão
    }

    busca_lance(profundidade);

    

}

void uci_loop()
{

    setbuf(stdout, NULL); // Desabilita buffering na saída padrão
    setbuf(stdin, NULL);  // Desabilita buffering na entrada padrão

    char comando[4096];
    while (1)
    {
        memset(comando, 0, sizeof(comando));

        fflush(stdout);

        if (fgets(comando, sizeof(comando), stdin) == NULL)
        {
            continue; // Se falhar ao ler, tenta novamente
        }

        // Remove nova linha do final do comando
        comando[strcspn(comando, "\n")] = 0;

        // Processa o comando
        if (strcmp(comando, "uci") == 0)
        {
            printf("id name MaZe 1.0\n");
            printf("id author GustavoGNZ\n");
            printf("uciok\n");
        }
        else if (strcmp(comando, "ucinewgame") == 0)
        {
            // Reinicia o jogo para uma nova partida
            parse_position("position startpos");
            // printTabuleiro();
        }
        else if (strncmp(comando, "isready", 7) == 0)
        {
            printf("readyok\n");
        }
        else if (strncmp(comando, "position", 8) == 0)
        {
            parse_position(comando);
            // printTabuleiro();
        }
        else if (strncmp(comando, "go", 2) == 0)
        {
            parse_go(comando);
        }
        else if (strncmp(comando, "quit", 4) == 0)
        {
            break;
        }
    }
}
