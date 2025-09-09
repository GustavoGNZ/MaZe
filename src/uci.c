/*
 * Implementação do protocolo UCI (Universal Chess Interface)
 * 
 * Este arquivo implementa o protocolo UCI para comunicação com interfaces gráficas de xadrez.
 * Contém funções de parsing, utilitários de conversão de lances e o loop principal do UCI.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/bitboard.h"
#include "../include/ataques.h"
#include "../include/globals.h"
#include "../include/uci.h"
#include "../include/search.h"
#include "../include/evaluate.h"
#include "../include/aberturas.h"

// Posição inicial padrão em notação FEN
#define posicaoInicial "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

void lance_para_uci(int lance, char* buffer) {
    // Extract move components from internal format
    int origem = get_origem(lance);
    int destino = get_destino(lance);
    int promocao = get_peca_promovida(lance);
    
    // Create basic move string (e.g., "e2e4")
    sprintf(buffer, "%s%s", casa_nome[origem], casa_nome[destino]);
    
    // Add promotion character if this is a promotion move
    if (promocao) {
        char promo_char;
        switch (promocao) {
            case Q: case q: promo_char = 'q'; break;
            case R: case r: promo_char = 'r'; break;
            case B: case b: promo_char = 'b'; break;
            case N: case n: promo_char = 'n'; break;
            default: promo_char = '\0'; break;
        }
        if (promo_char) {
            int len = strlen(buffer);
            buffer[len] = promo_char;
            buffer[len + 1] = '\0';
        }
    }
}

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

    // Limpa o histórico da partida ao configurar nova posição
    num_lances_partida = 0;

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
                    // Adiciona o lance ao histórico da partida
                    if (num_lances_partida < MAX_LANCES_PARTIDA) {
                        historico_lances_partida[num_lances_partida] = lance;
                        num_lances_partida++;
                    }
                    
                    if (!fazer_lance(lance, todosLances, backup_global))
                    {
                        break; // Erro ao executar lance
                    }
                }
                else
                {
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
    int tempo_branco = -1, tempo_preto = -1;
    int inc_branco = 0, inc_preto = 0;
    int movetime = -1;
    int infinite = 0;

    char *token = NULL;

    // Parse depth
    if ((token = strstr(string_go, "depth")))
    {
        profundidade = atoi(token + 6);
    }

    // Parse wtime (tempo das brancas em ms)
    if ((token = strstr(string_go, "wtime")))
    {
        tempo_branco = atoi(token + 6);
    }

    // Parse btime (tempo das pretas em ms)  
    if ((token = strstr(string_go, "btime")))
    {
        tempo_preto = atoi(token + 6);
    }

    // Parse winc (incremento das brancas em ms)
    if ((token = strstr(string_go, "winc")))
    {
        inc_branco = atoi(token + 5);
    }

    // Parse binc (incremento das pretas em ms)
    if ((token = strstr(string_go, "binc")))
    {
        inc_preto = atoi(token + 5);
    }

    // Parse movetime (tempo fixo para o lance em ms)
    if ((token = strstr(string_go, "movetime")))
    {
        movetime = atoi(token + 9);
    }

    // Parse infinite
    if (strstr(string_go, "infinite"))
    {
        infinite = 1;
    }

    // PRIMEIRO: Consultar livro de aberturas
    if (livro_aberturas.inicializado) {
        int lance_livro = buscar_lance_abertura(historico_lances_partida, num_lances_partida);
        
        if (lance_livro != 0) {
            // Verificar se o lance é válido na posição atual
            lances listaLances[1];
            gerar_lances(listaLances);
            
            for (int i = 0; i < listaLances->contador; i++) {
                if (listaLances->lances[i] == lance_livro) {
                    printf("bestmove ");
                    printLance(lance_livro);
                    printf("\n");
                    fflush(stdout);
                    return;
                }
            }
            printf("info string Lance do livro inválido na posição atual\n");
        }
    }

    // Configurar controle de tempo
    if (infinite) {
        // Busca infinita - sem limite de tempo
        // controle_tempo.tempo_total = 0;
        profundidade = (profundidade == -1) ? 64 : profundidade;
    } else if (movetime > 0) {
        // Tempo fixo por lance
        // controle_tempo.tempo_total = movetime;
        // controle_tempo.incremento = 0;
        // controle_tempo.tempo_restante = movetime;
        profundidade = 64; // Buscar até o tempo esgotar
    } else if (tempo_branco > 0 || tempo_preto > 0) {
        // Controle de tempo normal
        // init_tempo(tempo_branco, tempo_preto, inc_branco, inc_preto);
        profundidade = 64; // Buscar até o tempo esgotar
    } else {
        // Sem parâmetros de tempo - usar profundidade padrão
        // controle_tempo.tempo_total = 0;
        profundidade = (profundidade == -1) ? 6 : profundidade;
    }

    // Se não encontrou no livro, faz busca normal
    busca_lance(profundidade);
}

void uci_loop()
{

    setbuf(stdout, NULL); // Desabilita buffering na saída padrão
    setbuf(stdin, NULL);  // Desabilita buffering na entrada padrão

    // Carrega o livro de aberturas na inicialização
    inicializar_livro_aberturas();

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
            // limpar_historico_partida();
            parse_position("position startpos");
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

