#include "ataques.h"

u64 tabela_ataques_peao[2][64];
u64 tabela_ataques_cavalo[64];
u64 tabela_ataques_rei[64];
u64 tabela_ataques_bispo[64];
u64 tabela_ataques_torre[64];

// gerado usando um for que percorre todas as casas do tabuleiro e conta os bits de cada retorno da funcao gerar_ataque_bispo e gerar_ataque_torre
const int bits_relevantes_bispo[64] = {
    6, 5, 5, 5, 5, 5, 5, 6, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 7, 9, 9, 7, 5, 5, 
    5, 5, 7, 9, 9, 7, 5, 5, 
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    6, 5, 5, 5, 5, 5, 5, 6
};

const int bits_relevantes_torre[64] = {
    12, 11, 11, 11, 11, 11, 11, 12, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    12, 11, 11, 11, 11, 11, 11, 12   
};

// Gera o bitboard de ataques possíveis de um peão a partir da casa fornecida
u64 gerar_ataque_peao(int lado, int casa)
{

    u64 ataque = 0ULL;
    u64 bitboard = 0ULL;

    setBit(bitboard, casa); // Marca a casa atual do peão

    // Se o peão branco está na 8ª linha (rank 8) ou o peão preto está na 1ª linha (rank 1), não pode atacar nada
    if ((lado == branco && casa / 8 == 7) || (lado == preto && casa / 8 == 0))
    {
        return 0ULL;
    }

    if (lado == branco)
    {
        if (casa % 8 != 0)
        {                             // Não está na coluna 'a'
            setBit(ataque, casa + 7); // Ataque para a esquerda
        }
        if (casa % 8 != 7)
        {                             // Não está na coluna 'h'
            setBit(ataque, casa + 9); // Ataque para a direita
        }
    }
    else
    {
        if (casa % 8 != 0)
        {                             // Não está na coluna 'a'
            setBit(ataque, casa - 9); // Ataque para a esquerda
        }
        if (casa % 8 != 7)
        {                             // Não está na coluna 'h'
            setBit(ataque, casa - 7); // Ataque para a direita
        }
    }

    return ataque;
}

// Gera o bitboard de ataques possíveis de um cavalo a partir da casa fornecida
u64 gerar_ataque_cavalo(int casa)
{

    u64 ataque = 0ULL;
    u64 bitboard = 0ULL;

    setBit(bitboard, casa); // Marca a casa atual do peão

    int linha = casa / 8;  // linha (0 = primeira, 7 = oitava)
    int coluna = casa % 8; // coluna (0 = 'a', 7 = 'h')

    // Todos os 8 possíveis movimentos do cavalo (deslocamento em linha, coluna)
    int deslocamentos[8][2] = {
        {2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};

    // Para cada possível movimento, verifica se está dentro do tabuleiro
    for (int i = 0; i < 8; i++)
    {
        int nova_linha = linha + deslocamentos[i][0];
        int nova_coluna = coluna + deslocamentos[i][1];
        if (nova_linha >= 0 && nova_linha < 8 && nova_coluna >= 0 && nova_coluna < 8)
        {
            int nova_casa = nova_linha * 8 + nova_coluna;
            setBit(ataque, nova_casa); // Marca a casa como atacada
        }
    }

    return ataque;
}

// Gera o bitboard de ataques possíveis de um rei a partir da casa fornecida
u64 gerar_ataque_rei(int casa)
{

    u64 ataque = 0ULL;
    u64 bitboard = 0ULL;

    setBit(bitboard, casa); // Marca a casa atual do peão

    int linha = casa / 8;  // linha (0 = primeira, 7 = oitava)
    int coluna = casa % 8; // coluna (0 = 'a', 7 = 'h')

    // Todos os 8 possíveis movimentos do rei (deslocamento em linha, coluna)
    int deslocamentos[8][2] = {
        {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};

    // Para cada possível movimento, verifica se está dentro do tabuleiro
    for (int i = 0; i < 8; i++)
    {
        int nova_linha = linha + deslocamentos[i][0];
        int nova_coluna = coluna + deslocamentos[i][1];
        if (nova_linha >= 0 && nova_linha < 8 && nova_coluna >= 0 && nova_coluna < 8)
        {
            int nova_casa = nova_linha * 8 + nova_coluna;
            setBit(ataque, nova_casa); // Marca a casa como atacada
        }
    }

    return ataque;
}

// Gera o bitboard de ataques possíveis de um bispo a partir da casa fornecida
u64 gerar_ataque_bispo(int casa)
{

    u64 ataque = 0ULL;

    int linha = casa / 8;  // linha (0 = primeira, 7 = oitava)
    int coluna = casa % 8; // coluna (0 = 'a', 7 = 'h')

    // Deslocamentos diagonais
    int deslocamentos[4][2] = {
        {1, 1}, {1, -1}, {-1, -1}, {-1, 1}};

    // Para cada direção diagonal
    for (int i = 0; i < 4; i++)
    {
        int nova_linha = linha;
        int nova_coluna = coluna;

        // Continua movendo na direção até o final do tabuleiro
        while (1)
        {
            int proxima_linha = nova_linha + deslocamentos[i][0];
            int proxima_coluna = nova_coluna + deslocamentos[i][1];

            // Se sair do tabuleiro, para
            if (proxima_linha < 0 || proxima_linha >= 8 || proxima_coluna < 0 || proxima_coluna >= 8)
            {
                break;
            }

            // Marca a casa como atacada, mas NÃO adiciona o último bit (não marca a última casa da diagonal)
            // Então, só marca se a próxima casa NÃO for a última da diagonal
            int depois_linha = proxima_linha + deslocamentos[i][0];
            int depois_coluna = proxima_coluna + deslocamentos[i][1];
            if (depois_linha < 0 || depois_linha >= 8 || depois_coluna < 0 || depois_coluna >= 8)
            {
                // Próxima casa é a última da diagonal, então não marca
                break;
            }

            int nova_casa = proxima_linha * 8 + proxima_coluna;
            setBit(ataque, nova_casa); // Marca a casa como atacada

            nova_linha = proxima_linha;
            nova_coluna = proxima_coluna;
        }
    }

    return ataque;
}

// Gera o bitboard de ataques possíveis de uma torre a partir da casa fornecida
u64 gerar_ataque_torre(int casa)
{

    u64 ataque = 0ULL;

    int linha = casa / 8;  // linha (0 = primeira, 7 = oitava)
    int coluna = casa % 8; // coluna (0 = 'a', 7 = 'h')

    // Deslocamentos verticais e horizontais
    int deslocamentos[4][2] = {
        {1, 0}, {0, 1}, {-1, 0}, {0, -1}};

    // Para cada direção
    for (int i = 0; i < 4; i++)
    {
        int nova_linha = linha;
        int nova_coluna = coluna;

        // Continua movendo na direção até o final do tabuleiro
        while (1)
        {
            int proxima_linha = nova_linha + deslocamentos[i][0];
            int proxima_coluna = nova_coluna + deslocamentos[i][1];

            // Se sair do tabuleiro, para
            if (proxima_linha < 0 || proxima_linha >= 8 || proxima_coluna < 0 || proxima_coluna >= 8)
            {
                break;
            }

            // Não adiciona a última casa possível
            int depois_linha = proxima_linha + deslocamentos[i][0];
            int depois_coluna = proxima_coluna + deslocamentos[i][1];
            if (depois_linha < 0 || depois_linha >= 8 || depois_coluna < 0 || depois_coluna >= 8)
            {
                // Próxima casa é a última da linha/coluna, então não marca
                break;
            }

            int nova_casa = proxima_linha * 8 + proxima_coluna;
            setBit(ataque, nova_casa); // Marca a casa como atacada

            nova_linha = proxima_linha;
            nova_coluna = proxima_coluna;
        }
    }

    return ataque;
}

u64 gerar_ataque_bispo_tempo_real(int casa, u64 bitboard)
{

    u64 ataque = 0ULL;

    int linha = casa / 8;  // linha (0 = primeira, 7 = oitava)
    int coluna = casa % 8; // coluna (0 = 'a', 7 = 'h')

    // Deslocamentos diagonais
    int deslocamentos[4][2] = {
        {1, 1}, {1, -1}, {-1, -1}, {-1, 1}};

    // Para cada direção diagonal
    for (int i = 0; i < 4; i++)
    {
        int nova_linha = linha;
        int nova_coluna = coluna;

        // Continua movendo na direção até o final do tabuleiro
        while (1)
        {
            int proxima_linha = nova_linha + deslocamentos[i][0];
            int proxima_coluna = nova_coluna + deslocamentos[i][1];

            // Se sair do tabuleiro, para
            if (proxima_linha < 0 || proxima_linha >= 8 || proxima_coluna < 0 || proxima_coluna >= 8)
            {
                break;
            }

            int nova_casa = proxima_linha * 8 + proxima_coluna;
            if (getBit(bitboard, nova_casa))
            {
                // Se a casa já está ocupada, para
                break;
            }
            else
            {

                setBit(ataque, nova_casa); // Marca a casa como atacada
            }
            nova_linha = proxima_linha;
            nova_coluna = proxima_coluna;
        }
    }

    return ataque;
}

u64 gerar_ataque_torre_tempo_real(int casa, u64 bitboard)
{

    u64 ataque = 0ULL;

    int linha = casa / 8;  // linha (0 = primeira, 7 = oitava)
    int coluna = casa % 8; // coluna (0 = 'a', 7 = 'h')

    // Deslocamentos verticais e horizontais
    int deslocamentos[4][2] = {
        {1, 0}, {0, 1}, {-1, 0}, {0, -1}};

    // Para cada direção
    for (int i = 0; i < 4; i++)
    {
        int nova_linha = linha;
        int nova_coluna = coluna;

        // Continua movendo na direção até o final do tabuleiro
        while (1)
        {
            int proxima_linha = nova_linha + deslocamentos[i][0];
            int proxima_coluna = nova_coluna + deslocamentos[i][1];

            // Se sair do tabuleiro, para
            if (proxima_linha < 0 || proxima_linha >= 8 || proxima_coluna < 0 || proxima_coluna >= 8)
            {
                break;
            }

            int nova_casa = proxima_linha * 8 + proxima_coluna;
            if (getBit(bitboard, nova_casa))
            {
                // Se a casa já está ocupada, para
                break;
            }
            else
            {

                setBit(ataque, nova_casa); // Marca a casa como atacada
            }
            nova_linha = proxima_linha;
            nova_coluna = proxima_coluna;
        }
    }

    return ataque;
}

// Gera os ataques pré-definidos para as peças
void gerar_ataques_pecas()
{
    for (int casa = 0; casa < 64; casa++)
    {
        // peões
        tabela_ataques_peao[branco][casa] = gerar_ataque_peao(branco, casa);
        tabela_ataques_peao[preto][casa] = gerar_ataque_peao(preto, casa);

        // cavalo
        tabela_ataques_cavalo[casa] = gerar_ataque_cavalo(casa);

        // rei
        tabela_ataques_rei[casa] = gerar_ataque_rei(casa);

        // bispo e torre
        tabela_ataques_bispo[casa] = gerar_ataque_bispo(casa);
        tabela_ataques_torre[casa] = gerar_ataque_torre(casa);
    }
}

/**
 * @brief Gera um bitboard de ocupação baseado em um índice, quantidade de bits e uma máscara.
 *
 * Esta função cria um bitboard (ocupação) mapeando os bits do índice fornecido
 * para as posições dos bits setados na máscara. Para cada bit setado na máscara,
 * se o bit correspondente no índice estiver setado, a função seta esse bit no bitboard de ocupação.
 *
 * @param index        Índice representando a variação de ocupação (geralmente de 0 até 2^qtde_bits - 1).
 * @param qtde_bits    Quantidade de bits relevantes a considerar da máscara.
 * @param mask         Máscara de bitboard indicando as casas relevantes.
 * @return u64         Bitboard de ocupação resultante.
 */
u64 set_occupancy(int index, int qtde_bits, u64 mask){
    u64 occupancy = 0ULL;
    for (int i = 0; i < qtde_bits; i++)
    {
        int casa = getLeastBitIndex(mask);
        
        clearBit(mask, casa); // Remove a casa do mask

        if (index & (1ULL << i))
        {
            setBit(occupancy, casa); // Marca a casa no occupancy
        }
    }
    return occupancy;
}

