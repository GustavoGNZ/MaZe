#include "../include/aberturas.h"
#include "../include/uci.h"
#include "../include/globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Variável global do livro de aberturas
livro_aberturas_t livro_aberturas = {0};

/**
 * @brief Inicializa o sistema de livro de aberturas
 */
int inicializar_livro_aberturas() {
    FILE *arquivo;
    char linha[512];
    char nome_abertura_atual[MAX_NOME_ABERTURA] = "Desconhecida";
    
    // Inicializar o random seed
    srand(time(NULL));
    
    livro_aberturas.num_linhas = 0;
    livro_aberturas.inicializado = 0;
    
    // Tentar abrir o arquivo de aberturas
    arquivo = fopen("aberturas.txt", "r");
    if (arquivo == NULL) {
        printf("info string Aviso: arquivo aberturas.txt nao encontrado\n");
        return 0;
    }
    
    printf("info string Carregando livro de aberturas...\n");
    
    while (fgets(linha, sizeof(linha), arquivo) && livro_aberturas.num_linhas < MAX_LINHAS_ABERTURA) {
        // Remover quebra de linha
        linha[strcspn(linha, "\n")] = 0;
        
        // Ignorar linhas vazias
        if (strlen(linha) == 0) continue;
        
        // Processar comentários (nomes de aberturas)
        if (linha[0] == '#') {
            // Extrair nome da abertura
            char *ptr = linha + 1;
            while (*ptr == ' ') ptr++; // Pular espaços
            if (strlen(ptr) > 0) {
                strncpy(nome_abertura_atual, ptr, MAX_NOME_ABERTURA - 1);
                nome_abertura_atual[MAX_NOME_ABERTURA - 1] = '\0';
            }
            continue;
        }
        
        // Processar linha de lances
        linha_abertura_t *linha_atual = &livro_aberturas.linhas[livro_aberturas.num_linhas];
        strcpy(linha_atual->nome, nome_abertura_atual);
        linha_atual->num_lances = 0;
        
        // Parsear lances na linha
        char *token = strtok(linha, " ");
        while (token != NULL && linha_atual->num_lances < MAX_LANCES_ABERTURA) {
            // Copiar lance UCI
            strncpy(linha_atual->lances_str[linha_atual->num_lances], token, 5);
            linha_atual->lances_str[linha_atual->num_lances][5] = '\0';
            
            // Não converter para formato interno ainda - fazer na hora da comparação
            linha_atual->lances_codificados[linha_atual->num_lances] = 0;
            
            linha_atual->num_lances++;
            token = strtok(NULL, " ");
        }
        
        if (linha_atual->num_lances > 0) {
            livro_aberturas.num_linhas++;
        }
    }
    
    fclose(arquivo);
    
    livro_aberturas.inicializado = 1;
    printf("info string Livro carregado: %d linhas de abertura\n", livro_aberturas.num_linhas);
    
    return 1;
}

/**
 * @brief Converte lance UCI para formato interno
 */
int converter_uci_para_interno(const char* lance_uci) {
    if (strlen(lance_uci) < 4) return 0;
    
    // Salvar estado atual
    estado_jogo backup;
    SALVAR_ESTADO(backup);
    
    // Usar a função parse_move existente
    int lance = parse_move((char*)lance_uci);
    
    // Restaurar estado
    RESTAURAR_ESTADO(backup);
    
    return lance;
}

/**
 * @brief Obtém o número do lance atual baseado no FEN
 */
int obter_numero_lance_atual() {
    // O número do lance está diretamente relacionado ao número de lances jogados
    // Para as brancas: num_lances_partida é ímpar (lance 1, 3, 5...)
    // Para as pretas: num_lances_partida é par (lance 2, 4, 6...)
    
    // Retorna qual o número do próximo lance a ser jogado
    return num_lances_partida + 1;
}

/**
 * @brief Obtém informações detalhadas do lance atual
 */
void obter_info_lance_atual(int* numero_lance, int* eh_brancas) {
    // Determina se é vez das brancas ou pretas
    *eh_brancas = (num_lances_partida % 2 == 0) ? 1 : 0;
    
    // Calcula o número do lance no formato "lance completo"
    // Lance 1 = 1º das brancas, Lance 2 = 1º das pretas
    // Lance 3 = 2º das brancas, Lance 4 = 2º das pretas, etc.
    *numero_lance = num_lances_partida + 1;
}

/**
 * @brief Converte lance interno para string UCI
 */
void lance_interno_para_uci(int lance, char* uci_str) {
    int origem = get_origem(lance);
    int destino = get_destino(lance);
    int promocao = get_peca_promovida(lance);
    
    // Converter coordenadas para notação UCI
    char origem_str[3] = {(origem % 8) + 'a', (origem / 8) + '1', '\0'};
    char destino_str[3] = {(destino % 8) + 'a', (destino / 8) + '1', '\0'};
    
    strcpy(uci_str, origem_str);
    strcat(uci_str, destino_str);
    
    // Adicionar promoção se houver
    if (promocao) {
        switch (promocao) {
            case Q: strcat(uci_str, "q"); break;
            case R: strcat(uci_str, "r"); break;
            case B: strcat(uci_str, "b"); break;
            case N: strcat(uci_str, "n"); break;
        }
    }
}

/**
 * @brief Compara histórico com linha do livro usando strings UCI
 */
int comparar_historico_com_linha_uci(int historico[], int num_jogados, linha_abertura_t* linha) {
    // CORREÇÃO CRÍTICA: O histórico deve coincidir EXATAMENTE com o início da linha
    // Se temos mais lances jogados do que a linha tem, não pode coincidir
    if (num_jogados > linha->num_lances) {
        return 0;
    }
    
    // Compara todos os lances jogados com o início da linha
    for (int i = 0; i < num_jogados; i++) {
        char lance_uci[8];
        lance_interno_para_uci(historico[i], lance_uci);
        
        if (strcmp(lance_uci, linha->lances_str[i]) != 0) {
            return 0;
        }
    }
    
    return 1;
}

/**
 * @brief Busca um lance de abertura para a posição atual
 */
int buscar_lance_abertura(int historico_lances[], int num_lances_jogados) {
    if (!livro_aberturas.inicializado) {
        return 0;
    }
    
    // Arrays para armazenar lances candidatos e suas informações
    int candidatos[MAX_LINHAS_ABERTURA];
    char nomes_aberturas[MAX_LINHAS_ABERTURA][MAX_NOME_ABERTURA];
    int num_candidatos = 0;
    
    // Buscar todas as linhas que correspondem ao histórico atual
    for (int i = 0; i < livro_aberturas.num_linhas; i++) {
        linha_abertura_t *linha = &livro_aberturas.linhas[i];
        
        // Verificar se esta linha corresponde ao histórico E tem um próximo lance
        if (comparar_historico_com_linha_uci(historico_lances, num_lances_jogados, linha) &&
            linha->num_lances > num_lances_jogados) {
            
            // Converter o próximo lance UCI para formato interno
            int lance_candidato = converter_uci_para_interno(linha->lances_str[num_lances_jogados]);
            int ja_existe = 0;
            
            for (int j = 0; j < num_candidatos; j++) {
                if (candidatos[j] == lance_candidato) {
                    ja_existe = 1;
                    break;
                }
            }
            
            if (!ja_existe) {
                candidatos[num_candidatos] = lance_candidato;
                strncpy(nomes_aberturas[num_candidatos], linha->nome, MAX_NOME_ABERTURA - 1);
                nomes_aberturas[num_candidatos][MAX_NOME_ABERTURA - 1] = '\0';
                num_candidatos++;
            }
        }
    }
    
    // Se não há candidatos, sair do livro
    if (num_candidatos == 0) {
        return 0;
    }
    
    // Escolher um lance aleatório entre os candidatos
    int indice_escolhido = rand() % num_candidatos;
    
    printf("info string Lance escolhido do livro: %s (%d opcoes disponiveis)\n", 
           nomes_aberturas[indice_escolhido], num_candidatos);
    
    return candidatos[indice_escolhido];
}

/**
 * @brief Verifica se a engine ainda está "em livro"
 */
int esta_em_livro(int historico_lances[], int num_lances_jogados) {
    if (!livro_aberturas.inicializado) {
        return 0;
    }
    
    // Verificar se alguma linha ainda corresponde ao histórico
    for (int i = 0; i < livro_aberturas.num_linhas; i++) {
        linha_abertura_t *linha = &livro_aberturas.linhas[i];
        
        if (comparar_historico_com_linha_uci(historico_lances, num_lances_jogados, linha) &&
            linha->num_lances > num_lances_jogados) {
            return 1;
        }
    }
    
    return 0;
}

/**
 * @brief Libera recursos do livro de aberturas
 */
void finalizar_livro_aberturas() {
    livro_aberturas.inicializado = 0;
    livro_aberturas.num_linhas = 0;
}
