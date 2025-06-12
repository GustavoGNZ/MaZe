#include "ataques.h"

u64 tabela_ataques_peao[2][64];
u64 tabela_ataques_cavalo[64];
u64 tabela_ataques_rei[64];


u64 gerar_ataque_peao(int lado, int casa) {
    u64 ataque = 0ULL;

    // Se o peão branco está na 8ª linha (rank 8) ou o peão preto está na 1ª linha (rank 1), não pode atacar nada
    if ((lado == branco && casa / 8 == 7) || (lado == preto && casa / 8 == 0)) {
        return 0ULL;
    }

    if (lado == branco) {
        if (casa % 8 != 0) { // Não está na coluna 'a'
            ataque |= (1ULL << (casa + 7)); // Ataque para a esquerda
        }
        if (casa % 8 != 7) { // Não está na coluna 'h'
            ataque |= (1ULL << (casa + 9)); // Ataque para a direita
        }
    } else {
        if (casa % 8 != 0) { // Não está na coluna 'a'
            ataque |= (1ULL << (casa - 7)); // Ataque para a esquerda
        }
        if (casa % 8 != 7) { // Não está na coluna 'h'
            ataque |= (1ULL << (casa - 9)); // Ataque para a direita
        }
    }

    return ataque;
}

// Gera o bitboard de ataques possíveis de um cavalo a partir da casa fornecida (usando linha e coluna)
u64 gerar_ataque_cavalo(int casa) {
    u64 ataque = 0ULL;
    int linha = casa / 8; // linha (0 = primeira, 7 = oitava)
    int coluna = casa % 8; // coluna (0 = 'a', 7 = 'h')

    // Todos os 8 possíveis movimentos do cavalo (deslocamento em linha, coluna)
    int deslocamentos[8][2] = {
        {  2,  1 }, {  1,  2 }, { -1,  2 }, { -2,  1 },
        { -2, -1 }, { -1, -2 }, {  1, -2 }, {  2, -1 }
    };

    // Para cada possível movimento, verifica se está dentro do tabuleiro
    for (int i = 0; i < 8; i++) {
        int nova_linha = linha + deslocamentos[i][0];
        int nova_coluna = coluna + deslocamentos[i][1];
        if (nova_linha >= 0 && nova_linha < 8 && nova_coluna >= 0 && nova_coluna < 8) {
            int nova_casa = nova_linha * 8 + nova_coluna;
            ataque |= (1ULL << nova_casa); // Marca a casa como atacada
        }
    }

    return ataque;
}

u64 gerar_ataque_rei(int casa) {
    u64 ataque = 0ULL;
    int linha = casa / 8; // linha (0 = primeira, 7 = oitava)
    int coluna = casa % 8; // coluna (0 = 'a', 7 = 'h')

    // Todos os 8 possíveis movimentos do rei (deslocamento em linha, coluna)
    int deslocamentos[8][2] = {
        {  1,  0 }, {  1,  1 }, {  0,  1 }, { -1,  1 },
        { -1,  0 }, { -1, -1 }, {  0, -1 }, {  1, -1 }
    };

    // Para cada possível movimento, verifica se está dentro do tabuleiro
    for (int i = 0; i < 8; i++) {
        int nova_linha = linha + deslocamentos[i][0];
        int nova_coluna = coluna + deslocamentos[i][1];
        if (nova_linha >= 0 && nova_linha < 8 && nova_coluna >= 0 && nova_coluna < 8) {
            int nova_casa = nova_linha * 8 + nova_coluna;
            ataque |= (1ULL << nova_casa); // Marca a casa como atacada
        }
    }

    return ataque;
}


// Gera os ataques pré-definidos para peões e cavalos
void gerar_ataques(){
    for(int casa = 0; casa < 64; casa++){
        tabela_ataques_peao[branco][casa] = gerar_ataque_peao(branco, casa);
        tabela_ataques_peao[preto][casa] = gerar_ataque_peao(preto, casa);
        tabela_ataques_cavalo[casa] = gerar_ataque_cavalo(casa);
        tabela_ataques_rei[casa] = gerar_ataque_rei(casa);
    }
}
