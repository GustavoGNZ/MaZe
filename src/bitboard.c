#include "../include/bitboard.h"
#include "../include/ataques.h"
#include "../include/evaluate.h"
#include <string.h>

u64 bitboards[12];
u64 ocupacoes[3];

int lado_a_jogar;
int en_passant = -999; // Valor padrão para indicar que não há en passant
int roque = 0; // Valor padrão para indicar que não há roque

int roque_permissoes[64] = {
    13,15,15,15,12,15,15,14,  // Linha 1: a1=13(remove Q), e1=12(remove KQ), h1=14(remove K)
    15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,
    7,15,15,15,3,15,15,11    // Linha 8: a8=7(remove q), e8=3(remove kq), h8=11(remove k)
};

const char pecas_ascii[] = {
    'P', 'N', 'B', 'R', 'Q', 'K', // Peças brancas
    'p', 'n', 'b', 'r', 'q', 'k'  // Peças pretas
};

const char *pecas_unicode[] = {
    "♟", "♞", "♝", "♜", "♛", "♚", // Peças brancas
    "♙", "♘", "♗", "♖", "♕", "♔"  // Peças pretas
};

int pecas_char[] = {
    [P] = 'P', [N] = 'N', [B] = 'B', [R] = 'R', [Q] = 'Q', [K] = 'K', [p] = 'p', [n] = 'n', [b] = 'b', [r] = 'r', [q] = 'q', [k] = 'k'};

const char *casa_nome[] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"};

const char pecas_promocao[] = {[Q] = 'q', [R] = 'r', [B] = 'b', [N] = 'n', [q] = 'q', [r] = 'r', [b] = 'b', [n] = 'n'}; // Peças de promoção para peões

long nos;

// Imprime o bitboard que representa o tabuleiro de xadrez.
void printBitboard(u64 bitboard)
{

    for (int linha = 7; linha >= 0; linha--)
    {
        printf(" %d ", linha + 1);

        for (int coluna = 0; coluna < 8; coluna++)
        {
            int casa = linha * 8 + coluna;

            if (getBit(bitboard, casa))
            {
                printf(" 1 ");
            }
            else
            {
                printf(" 0 ");
            }
        }

        printf("\n");
    }
    printf("    a  b  c  d  e  f  g  h\n");
    printf("\n Bitboard: %lld\n", bitboard);
}

void printTabuleiro()
{
    for (int linha = 7; linha >= 0; linha--)
    {
        printf(" %d ", linha + 1);

        for (int coluna = 0; coluna < 8; coluna++)
        {
            int casa = linha * 8 + coluna;

            int peca = -1;

            for (int pecaTabuleiro = P; pecaTabuleiro <= k; pecaTabuleiro++)
            {
                if (getBit(bitboards[pecaTabuleiro], casa))
                {
                    peca = pecaTabuleiro;
                }
            }

            if (peca != -1)
            {
                #ifdef _WIN32
                                printf(" %c ", pecas_char[peca]);
                #else
                                printf(" %s ", pecas_unicode[peca]);
                #endif
            }
            else
            {
                printf(" . ");
            }
        }

        printf("\n");
    }
    printf("    a  b  c  d  e  f  g  h\n");

    printf("\n");

    printf("Turno das: %s\n", lado_a_jogar == branco ? "Brancas" : "Pretas");

    printf("En passant: %s\n", (en_passant != -999) ? casa_nome[en_passant] : "Nenhum");

    printf("Roque: ");
    // Exibe roques de forma compacta: KQkq (K=branco rei, Q=branco dama, k=preto rei, q=preto dama)
    // printf("%d\n", roque);
    // printf("%d\n", roque & reiBranco_alaRei);
    // printf("%d\n", roque & reiBranco_alaDama);
    // printf("%d\n", roque & reiPreto_alaRei);
    // printf("%d\n", roque & reiPreto_alaDama);


    printf("%c", (roque & reiBranco_alaRei)     ? 'K' : '-');
    printf("%c", (roque & reiBranco_alaDama)    ? 'Q' : '-');
    printf("%c", (roque & reiPreto_alaRei)      ? 'k' : '-');
    printf("%c", (roque & reiPreto_alaDama)     ? 'q' : '-');
    printf("\n");

    printf("Score: %d\n", evaluate());
}

void printCasasAtacadasPeloLado(int lado){
    printf("\n");
    
    // loop sobre as linhas do tabuleiro (de 7 para 0, igual ao printTabuleiro)
    for (int linha = 7; linha >= 0; linha--)
    {
        // loop sobre as colunas do tabuleiro
        for (int coluna = 0; coluna < 8; coluna++)
        {
            // inicializa casa
            int casa = linha * 8 + coluna;
            
            // imprime números das linhas
            if (!coluna)
                printf("  %d ", linha + 1); // linha + 1 para mostrar 8, 7, 6...
            
            // verifica se a casa atual está atacada ou não
            printf(" %d", casaEstaAtacada(casa, lado) ? 1 : 0);
        }
        
        // imprime nova linha a cada linha
        printf("\n");
    }
    
    // imprime colunas
    printf("\n     a b c d e f g h\n\n");
}

// Analisador sintático de FEN (Forsyth-Edwards Notation).
// Esta função deve ser implementada para interpretar a notação FEN e configurar o tabuleiro 
void parseFEN(char *fen){
    // Inicializa os bitboards e ocupações vazios e variaveis de estado do jogo
    memset(bitboards, 0ULL, sizeof(bitboards));
    memset(ocupacoes, 0ULL, sizeof(ocupacoes));
    lado_a_jogar = branco;
    en_passant = -999;
    roque = 0;

    // Cria mapeamento reverso usando seu pecas_char[] existente
    int char_para_peca[256];
    memset(char_para_peca, -1, sizeof(char_para_peca));
    
    // Usa seu array existente para criar o mapeamento reverso
    for(int i = P; i <= k; i++) {
        char_para_peca[pecas_char[i]] = i;
    }

    // 1. Parse das peças (FEN começa da linha 8 e vai até linha 1)
    for(int linha = 7; linha >= 0; linha--) {
        for(int coluna = 0; coluna < 8; coluna++) {
            int casa = linha * 8 + coluna;
            
            if(*fen == '/') {
                fen++; // Pula o separador de linha
                coluna--; // Compensa o incremento do loop
                continue;
            }
            
            if(*fen >= '1' && *fen <= '8') {
                // Número representa casas vazias
                int casas_vazias = *fen - '0';
                coluna += casas_vazias - 1; // -1 porque o loop já incrementa
                fen++;
            }
            else if(char_para_peca[*fen] != -1) {
                // É uma peça válida
                int peca = char_para_peca[*fen];
                setBit(bitboards[peca], casa);
                fen++;
            }
            else {
                fen++; // Pula caracteres não reconhecidos
            }
        }
    }
    
    // Pula espaço após as peças
    if(*fen == ' ') fen++;
    
    // 2. Parse do lado a jogar
    if(*fen == 'w') {
        lado_a_jogar = branco;
    } else if(*fen == 'b') {
        lado_a_jogar = preto;
    }
    fen++;
    
    // Pula espaço
    if(*fen == ' ') fen++;
    
    // 3. Parse do roque
    if(*fen != '-') {
        while(*fen != ' ' && *fen != '\0') {
            if(*fen == 'K') roque |= reiBranco_alaRei;
            else if(*fen == 'Q') roque |= reiBranco_alaDama;
            else if(*fen == 'k') roque |= reiPreto_alaRei;
            else if(*fen == 'q') roque |= reiPreto_alaDama;
            fen++;
        }
    } else {
        fen++; // Pula o '-'
    }
    
    // Pula espaço
    if(*fen == ' ') fen++;
    
    // 4. Parse do en passant
    if(*fen != '-') {
        // Converte notação algébrica (ex: "e3") para índice de casa
        int coluna = *fen - 'a';  // 'a' = 0, 'b' = 1, etc.
        fen++;
        int linha = *fen - '1';   // '1' = 0, '2' = 1, etc.
        fen++;
        en_passant = linha * 8 + coluna;
    } else {
        en_passant = -999;
        fen++; // Pula o '-'
    }
    
    // Atualizar ocupações
    for(int i = P; i <= K; i++) {
        ocupacoes[branco] |= bitboards[i];
    }
    for(int i = p; i <= k; i++) {
        ocupacoes[preto] |= bitboards[i];
    }
    ocupacoes[ambos] = ocupacoes[branco] | ocupacoes[preto];
}

int contarBits(u64 bitboard)
{
    int count = 0;

    while (bitboard != 0)
    {
        count += bitboard & 1; // Verifica o bit menos significativo
        bitboard >>= 1;        // Desloca para a direita
    }

    return count;
}

void adicionarLance(lances *listaLances, int lance) {
    if (listaLances->contador < 256) { // Verifica se há espaço na lista
        listaLances->lances[listaLances->contador] = lance;
        listaLances->contador++;
    }
}
// uci
void printLance(int lance){
    printf("%s%s%c", casa_nome[get_origem(lance)], casa_nome[get_destino(lance)], get_peca_promovida(lance) ? pecas_promocao[get_peca_promovida(lance)] : ' ');
}

void printListaLances(lances *listaLances) {
    
    if(listaLances->contador == 0){
        printf("Nao ha lances gerados\n");
        return;
    }

    printf("\n");
    printf("┌─────┬──────┬─────┬─────┬──────┬─────────┬────────────┬──────────┐\n");
    printf("│ No. │ Move │ Peca│ Cap │ Duplo│ En Pass │  Castling  │  Promocao│\n");
    printf("├─────┼──────┼─────┼─────┼──────┼─────────┼────────────┼──────────┤\n");

    for (int i = 0; i < listaLances->contador; i++) {
        int lance = listaLances->lances[i];
        char promocao = get_peca_promovida(lance) ? pecas_promocao[get_peca_promovida(lance)] : ' ';
        
        printf("│%4d │ %s%s │  %c  │  %c  │  %c   │   %c     │     %c      │    %c     │\n", 
               i + 1,
               casa_nome[get_origem(lance)], 
               casa_nome[get_destino(lance)], 
               pecas_ascii[get_peca(lance)],
               get_captura(lance) ? 'S' : 'N',
               get_double_push(lance) ? 'S' : 'N',
               get_en_passant(lance) ? 'S' : 'N',
               get_roque(lance) ? 'S' : 'N',
               promocao);
    }
    
    printf("└─────┴──────┴─────┴─────┴──────┴─────────┴────────────┴──────────┘\n");
    printf("Total de lances: %d\n", listaLances->contador);
}

void perft(int profundidade) {
    if (profundidade == 0) {
        nos++;
        return;
    }

    lances listaLances;

    gerar_lances(&listaLances);

    for (int i = 0; i < listaLances.contador; i++) {
        int lance = listaLances.lances[i];

        // Salva o estado atual
        estado_jogo backup;
        SALVAR_ESTADO(backup);

        if (!fazer_lance(lance, todosLances, backup)) {
            continue; // Lance inválido, tenta o próximo
        }

        perft(profundidade - 1);

        // Restaura o estado anterior
        RESTAURAR_ESTADO(backup);
    }

}   

void perft_teste(int profundidade) {
    if (profundidade == 0) {
        nos++;
        return;
    }

    lances listaLances;

    gerar_lances(&listaLances);

    int inicio = get_tempo_milisegundos();

    for (int i = 0; i < listaLances.contador; i++) {
        int lance = listaLances.lances[i];

        // Salva o estado atual
        estado_jogo backup;
        SALVAR_ESTADO(backup);

        if (!fazer_lance(lance, todosLances, backup)) {
            continue; // Lance inválido, tenta o próximo
        }

        long nos_antes = nos;

        perft(profundidade - 1);

        long nos_antigos = nos - nos_antes;

        // Restaura o estado anterior
        RESTAURAR_ESTADO(backup);
        printLance(lance);
        printf("nos : %ld\n", nos_antigos);

    }

    printf("\nProfundidade: %d\n", profundidade);
    printf("Nos: %ld\n", nos);
    printf("Tempo: %d ms\n", get_tempo_milisegundos() - inicio);
    printf("\n");

}   
