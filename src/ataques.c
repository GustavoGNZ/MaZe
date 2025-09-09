/**
 * @file ataques.c
 * @brief Sistema de geração de ataques e lances da engine MaZe
 * @author GustavoGNZ
 * @version 1.0
 * 
 * Este arquivo contém a implementação completa do sistema de geração de ataques
 * e lances, incluindo magic bitboards, geração de números aleatórios, cálculo
 * de ataques para todas as peças e geração de movimentos legais.
 */

#include "../include/ataques.h"
#include "../include/globals.h"
#include "../include/bitboard.h"
#include "../include/evaluate.h"

#include <stdio.h>
#include <string.h>
#include <sys/time.h>

// =============================================================================
// FUNÇÕES DE TEMPO E NÚMEROS ALEATÓRIOS
// =============================================================================

/**
 * @brief Obtém o tempo atual em milissegundos
 * 
 * Função multiplataforma para obter timestamp em milissegundos,
 * usada para medição de performance e seeds aleatórias.
 * 
 * @return Tempo atual em milissegundos
 */
int get_tempo_milisegundos()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

/**
 * @brief Estado global do gerador de números aleatórios
 */
unsigned int num_aleatorio = 1804289383;

/**
 * @brief Gera um número aleatório de 32 bits usando xorshift32
 * 
 * Implementação de gerador pseudo-aleatório xorshift32 para garantir
 * comportamento consistente entre Windows e Linux (diferenças em rand()).
 * 
 * @return Número aleatório de 32 bits
 */
unsigned int gerarNumeroAleatorio32bits()
{
    unsigned int num = num_aleatorio;

    // xorshift32
    num ^= num << 13;
    num ^= num >> 17;
    num ^= num << 5;

    num_aleatorio = num; // Atualiza o estado global

    return num;
};

// Gera um número aleatório de 64 bits combinando quatro números aleatórios
u64 gerarNumeroAleatorio64bits()
{

    u64 n1, n2, n3, n4;

    n1 = (u64)gerarNumeroAleatorio32bits() & 0xFFFF;
    n2 = (u64)gerarNumeroAleatorio32bits() & 0xFFFF;
    n3 = (u64)gerarNumeroAleatorio32bits() & 0xFFFF;
    n4 = (u64)gerarNumeroAleatorio32bits() & 0xFFFF;

    // Combina os quatro números de 16 bits em um único número de 64 bits
    u64 numero_aleatorio = n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);

    return numero_aleatorio;
}
// Gera o magic number
u64 gerarMagicNumber()
{
    return gerarNumeroAleatorio64bits() & gerarNumeroAleatorio64bits() & gerarNumeroAleatorio64bits();
}

u64 encontrarMagicNumber(int casa, int bits_relevantes, int peca)
{
    u64 ocupacoes[4096];      // Array para armazenar ocupações
    u64 ataques[4096];        // Array para armazenar ataques
    u64 ataques_usados[4096]; // Array para armazenar ataques usados

    u64 ataque_peca;

    if (peca == bispo)
    {
        ataque_peca = gerarAtaquesBispo(casa);
    }
    else if (peca == torre)
    {
        ataque_peca = gerarAtaquesTorre(casa);
    }
    else
    {
        printf("Peca invalida para encontrar magic number\n");
        return 0ULL;
    }

    int indices_ocupacao = 1 << bits_relevantes;

    for (int i = 0; i < indices_ocupacao; i++)
    {
        // Gera uma ocupação aleatória
        ocupacoes[i] = set_occupancy(i, bits_relevantes, ataque_peca);

        // Calcula o ataque para a ocupação atual
        if (peca == bispo)
        {
            ataques[i] = gerarAtaquesBispoComBloqueio(casa, ocupacoes[i]);
        }
        else if (peca == torre)
        {
            ataques[i] = gerarAtaquesTorreComBloqueio(casa, ocupacoes[i]);
        }
    }

    // 1048576 = 2^20 escolhido arbitrariamente para o tamanho do array de ataques usados
    for (int j = 0; j < 100000000; j++)
    {

        u64 magic_number = gerarMagicNumber();

        if (contarBits((ataque_peca * magic_number) & 0xFF00000000000000) < 6)
        {
            continue; // Se o magic number não gera ataques relevantes, ignora
        }

        memset(ataques_usados, 0ULL, sizeof(ataques_usados)); // Limpa o array de ataques usados

        int indice, erro;

        for (indice = 0, erro = 0; !erro && indice < indices_ocupacao; indice++)
        {
            // Calcula o índice de ataque usando o magic number
            int index_magico = (int)((ocupacoes[indice] * magic_number) >> (64 - bits_relevantes));

            // Verifica se o ataque já foi usado
            if (ataques_usados[index_magico] == 0ULL)
            {
                ataques_usados[index_magico] = ataques[indice]; // Marca o ataque como usado
            }
            else if (ataques_usados[index_magico] != ataques[indice])
            {
                erro = 1; // Se o ataque já foi usado e é diferente, invalida o magic number
            }
        }
        // funciona
        if (!erro)
            return magic_number;
    }

    return 0ULL; // Se não encontrou um magic number válido, retorna 0
}

// gera magic numbers hard coded para bispo e torre (a ordem importa pois a aleatoriadade eh pseudo-aleatória)
void inicializarMagicNumbers()
{
    // Inicializa os magic numbers para bispo e torre
    for (int casa = 0; casa < 64; casa++)
    {
        magics_torre[casa] = encontrarMagicNumber(casa, bits_relevantes_torre[casa], torre);
    }
    for (int casa = 0; casa < 64; casa++)
    {
        magics_bispo[casa] = encontrarMagicNumber(casa, bits_relevantes_bispo[casa], bispo);
    }
}

// Função auxiliar para verificar se uma posição está dentro do tabuleiro
int estaDentroDoTabuleiro(int linha, int coluna)
{
    return (linha >= 0 && linha < 8 && coluna >= 0 && coluna < 8);
};

// Verifica se a próxima casa na direção especificada é a última da direção
int ehUltimaCasaNaDirecao(int linha, int coluna, int deslocamento_linha, int deslocamento_coluna)
{
    int proxima_linha = linha + deslocamento_linha;
    int proxima_coluna = coluna + deslocamento_coluna;

    return !estaDentroDoTabuleiro(proxima_linha, proxima_coluna);
}

// Marca os ataques direcionais para bispo e torre
void marcarAtaquesDirecionais(u64 *ataque, int linha, int coluna, int deslocamentos[][2])
{
    for (int i = 0; i < 4; i++)
    {
        int nova_linha = linha;
        int nova_coluna = coluna;

        while (1)
        {
            int proxima_linha = nova_linha + deslocamentos[i][0];
            int proxima_coluna = nova_coluna + deslocamentos[i][1];

            if (!estaDentroDoTabuleiro(proxima_linha, proxima_coluna))
            {
                break;
            }

            // Verifica se a próxima casa já é a última da direção
            if (ehUltimaCasaNaDirecao(proxima_linha, proxima_coluna, deslocamentos[i][0], deslocamentos[i][1]))
            {
                break;
            }

            int nova_casa = proxima_linha * 8 + proxima_coluna;
            setBit(*ataque, nova_casa);

            nova_linha = proxima_linha;
            nova_coluna = proxima_coluna;
        }
    }
}

void marcarAtaquesDirecionaisComBloqueio(u64 *ataque, int linha, int coluna, int deslocamentos[][2], int num_direcoes, u64 bitboard_ocupacao)
{
    for (int i = 0; i < num_direcoes; i++)
    {
        int nova_linha = linha;
        int nova_coluna = coluna;

        while (1)
        {
            int proxima_linha = nova_linha + deslocamentos[i][0];
            int proxima_coluna = nova_coluna + deslocamentos[i][1];

            if (!estaDentroDoTabuleiro(proxima_linha, proxima_coluna))
            {
                break;
            }

            int nova_casa = proxima_linha * 8 + proxima_coluna;

            setBit(*ataque, nova_casa);

            if (getBit(bitboard_ocupacao, nova_casa))
            {
                // Casa ocupada: não pode continuar, nem marca
                break;
            }

            nova_linha = proxima_linha;
            nova_coluna = proxima_coluna;
        }
    }
}

// Gera os ataques pré-definidos para as peças
void inicializarAtaquesPecas()
{
    for (int casa = 0; casa < 64; casa++)
    {
        // peões
        tabela_ataques_peao[branco][casa] = gerarAtaquesPeao(branco, casa);
        tabela_ataques_peao[preto][casa] = gerarAtaquesPeao(preto, casa);

        // cavalo
        tabela_ataques_cavalo[casa] = gerarAtaquesCavalo(casa);

        // rei
        tabela_ataques_rei[casa] = gerarAtaquesRei(casa);

        // bispo e torre
        inicializarAtaquesPecasDeslizantes(bispo);
        inicializarAtaquesPecasDeslizantes(torre);
    }
}

void inicializarAtaquesPecasDeslizantes(int peca)
{
    for (int casa = 0; casa < 64; casa++)
    {

        mask_tabela_ataques_bispo[casa] = gerarAtaquesBispo(casa);
        mask_tabela_ataques_torre[casa] = gerarAtaquesTorre(casa);

        u64 ataque_mask;

        ataque_mask = (peca == bispo) ? mask_tabela_ataques_bispo[casa] : mask_tabela_ataques_torre[casa];

        int bits_relevantes = contarBits(ataque_mask);

        int indices_ocupacao = 1 << bits_relevantes;

        for (int i = 0; i < indices_ocupacao; i++)
        {

            if (peca == bispo)
            {
                u64 ocupacao = set_occupancy(i, bits_relevantes, ataque_mask);

                int indice_magico = (ocupacao * magics_bispo[casa]) >> (64 - bits_relevantes);

                tabela_ataques_bispo[casa][indice_magico] = gerarAtaquesBispoComBloqueio(casa, ocupacao);
            }
            else if (peca == torre)
            {
                u64 ocupacao = set_occupancy(i, bits_relevantes, ataque_mask);

                int indice_magico = (ocupacao * magics_torre[casa]) >> (64 - bits_relevantes);

                tabela_ataques_torre[casa][indice_magico] = gerarAtaquesTorreComBloqueio(casa, ocupacao);
            }
        }
    }
}

u64 obterAtaquesBispo(int casa, u64 ocupacao)
{

    ocupacao &= mask_tabela_ataques_bispo[casa];
    ocupacao *= magics_bispo[casa];
    ocupacao >>= (64 - bits_relevantes_bispo[casa]);

    return tabela_ataques_bispo[casa][ocupacao]; // Retorna o ataque correspondente ao índice calculado
}

u64 obterAtaquesTorre(int casa, u64 ocupacao)
{

    ocupacao &= mask_tabela_ataques_torre[casa];
    ocupacao *= magics_torre[casa];
    ocupacao >>= (64 - bits_relevantes_torre[casa]);

    return tabela_ataques_torre[casa][ocupacao]; // Retorna o ataque correspondente ao índice calculado
}

u64 obterAtaquesDama(int casa, u64 ocupacao)
{
    return (obterAtaquesBispo(casa, ocupacao) | obterAtaquesTorre(casa, ocupacao)); // A dama ataca como bispo e torre
}

// Gera o bitboard de ataques possíveis de um peão a partir da casa fornecida
u64 gerarAtaquesPeao(int lado, int casa)
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
u64 gerarAtaquesCavalo(int casa)
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

        if (estaDentroDoTabuleiro(nova_linha, nova_coluna))
        {
            int nova_casa = nova_linha * 8 + nova_coluna;
            setBit(ataque, nova_casa); // Marca a casa como atacada
        }
    }

    return ataque;
}

// Gera o bitboard de ataques possíveis de um rei a partir da casa fornecida
u64 gerarAtaquesRei(int casa)
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

        if (estaDentroDoTabuleiro(nova_linha, nova_coluna))
        {
            int nova_casa = nova_linha * 8 + nova_coluna;
            setBit(ataque, nova_casa); // Marca a casa como atacada
        }
    }

    return ataque;
}

// Gera o bitboard de ataques possíveis de um bispo a partir da casa fornecida
u64 gerarAtaquesBispo(int casa)
{

    u64 ataque = 0ULL;

    int linha = casa / 8;  // linha (0 = primeira, 7 = oitava)
    int coluna = casa % 8; // coluna (0 = 'a', 7 = 'h')

    // Deslocamentos diagonais
    int deslocamentos[4][2] = {
        {1, 1}, {1, -1}, {-1, -1}, {-1, 1}};

    marcarAtaquesDirecionais(&ataque, linha, coluna, deslocamentos);

    return ataque;
}

// Gera o bitboard de ataques possíveis de uma torre a partir da casa fornecida
u64 gerarAtaquesTorre(int casa)
{

    u64 ataque = 0ULL;

    int linha = casa / 8;  // linha (0 = primeira, 7 = oitava)
    int coluna = casa % 8; // coluna (0 = 'a', 7 = 'h')

    // Deslocamentos verticais e horizontais
    int deslocamentos[4][2] = {
        {1, 0}, {0, 1}, {-1, 0}, {0, -1}};

    marcarAtaquesDirecionais(&ataque, linha, coluna, deslocamentos);

    return ataque;
}

// Gera o bitboard de ataques possíveis de um bispo considerando peças que bloqueiam
u64 gerarAtaquesBispoComBloqueio(int casa, u64 bitboard_ocupacao)
{
    u64 ataque = 0ULL;
    int linha = casa / 8;
    int coluna = casa % 8;

    int deslocamentos[4][2] = {
        {1, 1}, {1, -1}, {-1, -1}, {-1, 1}};

    marcarAtaquesDirecionaisComBloqueio(&ataque, linha, coluna, deslocamentos, 4, bitboard_ocupacao);
    return ataque;
}

// Gera o bitboard de ataques possíveis de uma torre considerando peças que bloqueiam
u64 gerarAtaquesTorreComBloqueio(int casa, u64 bitboard_ocupacao)
{
    u64 ataque = 0ULL;
    int linha = casa / 8;
    int coluna = casa % 8;

    int deslocamentos[4][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    marcarAtaquesDirecionaisComBloqueio(&ataque, linha, coluna, deslocamentos, 4, bitboard_ocupacao);
    return ataque;
}

u64 set_occupancy(int index, int qtde_bits, u64 mask)
{
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

void runEngine()
{

    inicializarAtaquesPecas();
    init_evaluation_masks();

}

int casaEstaAtacada(int casa, int lado)
{

    if ((lado == branco) && (tabela_ataques_peao[preto][casa] & bitboards[P]))
        return 1;

    if ((lado == preto) && (tabela_ataques_peao[branco][casa] & bitboards[p]))
        return 1;

    if (tabela_ataques_cavalo[casa] & ((lado == branco) ? bitboards[N] : bitboards[n]))
        return 1;

    if (obterAtaquesBispo(casa, ocupacoes[ambos]) & ((lado == branco) ? bitboards[B] : bitboards[b]))
        return 1;

    if (obterAtaquesTorre(casa, ocupacoes[ambos]) & ((lado == branco) ? bitboards[R] : bitboards[r]))
        return 1;

    if (obterAtaquesDama(casa, ocupacoes[ambos]) & ((lado == branco) ? bitboards[Q] : bitboards[q]))
        return 1;

    if (tabela_ataques_rei[casa] & ((lado == branco) ? bitboards[K] : bitboards[k]))
        return 1;

    return 0;
}

// Função auxiliar para gerar movimentos de peões brancos
void gerar_movimentos_peao_branco(int origem, lances *listaLances)
{
    int destino = origem + 8;

    // Movimento simples
    if (destino < 64 && !getBit(ocupacoes[ambos], destino))
    {
        if (origem >= a7 && origem <= h7)
        { // 7ª linha (promoção)
            adicionarLance(listaLances, codificar_lance(origem, destino, P, Q, 0, 0, 0, 0));
            adicionarLance(listaLances, codificar_lance(origem, destino, P, R, 0, 0, 0, 0));
            adicionarLance(listaLances, codificar_lance(origem, destino, P, B, 0, 0, 0, 0));
            adicionarLance(listaLances, codificar_lance(origem, destino, P, N, 0, 0, 0, 0));
        }
        else
        {
            adicionarLance(listaLances, codificar_lance(origem, destino, P, 0, 0, 0, 0, 0));

            // Movimento duplo do peão (da 2ª linha)
            if (origem >= a2 && origem <= h2)
            {
                destino = origem + 16;
                if (destino < 64 && !getBit(ocupacoes[ambos], destino))
                {
                    adicionarLance(listaLances, codificar_lance(origem, destino, P, 0, 0, 1, 0, 0));
                }
            }
        }
    }

    // Capturas
    u64 ataques = tabela_ataques_peao[branco][origem] & ocupacoes[preto];
    while (ataques)
    {
        destino = getLeastBitIndex(ataques);

        if (origem >= a7 && origem <= h7)
        { // 7ª linha (promoção)
            adicionarLance(listaLances, codificar_lance(origem, destino, P, Q, 1, 0, 0, 0));
            adicionarLance(listaLances, codificar_lance(origem, destino, P, R, 1, 0, 0, 0));
            adicionarLance(listaLances, codificar_lance(origem, destino, P, B, 1, 0, 0, 0));
            adicionarLance(listaLances, codificar_lance(origem, destino, P, N, 1, 0, 0, 0));
        }
        else
        {
            adicionarLance(listaLances, codificar_lance(origem, destino, P, 0, 1, 0, 0, 0));
        }

        clearBit(ataques, destino);
    }

    // En passant
    if (en_passant != -999 && origem >= a5 && origem <= h5)
    {
        if ((origem % 8 != 0 && en_passant == origem + 7) ||
            (origem % 8 != 7 && en_passant == origem + 9))
        {
            adicionarLance(listaLances, codificar_lance(origem, en_passant, P, 0, 1, 0, 1, 0));
        }
    }
}

// Função auxiliar para gerar movimentos de peões pretos
void gerar_movimentos_peao_preto(int origem, lances *listaLances)
{
    int destino = origem - 8;

    // Movimento simples
    if (destino >= 0 && !getBit(ocupacoes[ambos], destino))
    {
        if (origem >= a2 && origem <= h2)
        { // 2ª linha (promoção para peões pretos)
            adicionarLance(listaLances, codificar_lance(origem, destino, p, q, 0, 0, 0, 0));
            adicionarLance(listaLances, codificar_lance(origem, destino, p, r, 0, 0, 0, 0));
            adicionarLance(listaLances, codificar_lance(origem, destino, p, b, 0, 0, 0, 0));
            adicionarLance(listaLances, codificar_lance(origem, destino, p, n, 0, 0, 0, 0));
        }
        else
        {
            adicionarLance(listaLances, codificar_lance(origem, destino, p, 0, 0, 0, 0, 0));

            // Movimento duplo do peão (da 7ª linha para peões pretos)
            if (origem >= a7 && origem <= h7)
            {
                destino = origem - 16;
                if (destino >= 0 && !getBit(ocupacoes[ambos], destino))
                {
                    adicionarLance(listaLances, codificar_lance(origem, destino, p, 0, 0, 1, 0, 0));
                }
            }
        }
    }

    // Capturas
    u64 ataques = tabela_ataques_peao[preto][origem] & ocupacoes[branco];
    while (ataques)
    {
        destino = getLeastBitIndex(ataques);

        if (origem >= a2 && origem <= h2)
        { // 2ª linha (promoção para peões pretos)
            adicionarLance(listaLances, codificar_lance(origem, destino, p, q, 1, 0, 0, 0));
            adicionarLance(listaLances, codificar_lance(origem, destino, p, r, 1, 0, 0, 0));
            adicionarLance(listaLances, codificar_lance(origem, destino, p, b, 1, 0, 0, 0));
            adicionarLance(listaLances, codificar_lance(origem, destino, p, n, 1, 0, 0, 0));
        }
        else
        {
            adicionarLance(listaLances, codificar_lance(origem, destino, p, 0, 1, 0, 0, 0));
        }

        clearBit(ataques, destino);
    }

    // En passant
    if (en_passant != -999 && origem >= a4 && origem <= h4)
    {
        if ((origem % 8 != 0 && en_passant == origem - 9) ||
            (origem % 8 != 7 && en_passant == origem - 7))
        {
            adicionarLance(listaLances, codificar_lance(origem, en_passant, p, 0, 1, 0, 1, 0));
        }
    }
}

// Função auxiliar para gerar roque branco
void gerar_roque_branco(lances *listaLances)
{
    // Roque pequeno
    if (roque & reiBranco_alaRei)
    {
        if (!getBit(ocupacoes[ambos], f1) && !getBit(ocupacoes[ambos], g1))
        {
            if (!casaEstaAtacada(e1, preto) && !casaEstaAtacada(f1, preto) && !casaEstaAtacada(g1, preto))
            {
                adicionarLance(listaLances, codificar_lance(e1, g1, K, 0, 0, 0, 0, 1));
            }
        }
    }

    // Roque grande
    if (roque & reiBranco_alaDama)
    {
        if (!getBit(ocupacoes[ambos], b1) && !getBit(ocupacoes[ambos], c1) && !getBit(ocupacoes[ambos], d1))
        {
            if (!casaEstaAtacada(e1, preto) && !casaEstaAtacada(d1, preto) && !casaEstaAtacada(c1, preto))
            {
                adicionarLance(listaLances, codificar_lance(e1, c1, K, 0, 0, 0, 0, 1));
            }
        }
    }
}

// Função auxiliar para gerar roque preto
void gerar_roque_preto(lances *listaLances)
{
    // Roque pequeno
    if (roque & reiPreto_alaRei)
    {
        if (!getBit(ocupacoes[ambos], f8) && !getBit(ocupacoes[ambos], g8))
        {
            if (!casaEstaAtacada(e8, branco) && !casaEstaAtacada(f8, branco) && !casaEstaAtacada(g8, branco))
            {
                adicionarLance(listaLances, codificar_lance(e8, g8, k, 0, 0, 0, 0, 1));
            }
        }
    }

    // Roque grande
    if (roque & reiPreto_alaDama)
    {
        if (!getBit(ocupacoes[ambos], b8) && !getBit(ocupacoes[ambos], c8) && !getBit(ocupacoes[ambos], d8))
        {
            if (!casaEstaAtacada(e8, branco) && !casaEstaAtacada(d8, branco) && !casaEstaAtacada(c8, branco))
            {
                adicionarLance(listaLances, codificar_lance(e8, c8, k, 0, 0, 0, 0, 1));
            }
        }
    }
}

// Função genérica para gerar movimentos de peças "grandes" (cavalos, bispos, torres, damas)
void gerar_movimentos_pecaGrande(int origem, int lado, int tipo_peca, lances *listaLances)
{
    u64 ataques;

    // Obtém os ataques baseado no tipo da peça
    switch (tipo_peca)
    {
    case N:
    case n:
        ataques = tabela_ataques_cavalo[origem];
        break;
    case B:
    case b:
        ataques = obterAtaquesBispo(origem, ocupacoes[ambos]);
        break;
    case R:
    case r:
        ataques = obterAtaquesTorre(origem, ocupacoes[ambos]);
        break;
    case Q:
    case q:
        ataques = obterAtaquesDama(origem, ocupacoes[ambos]);
        break;
    case K:
    case k:
        ataques = tabela_ataques_rei[origem];
        break;
    default:
        return; // Tipo de peça inválido
    }

    // Remove casas ocupadas pelo próprio lado
    ataques &= ~ocupacoes[lado];
    u64 ocupacao_oponente = (lado == branco) ? ocupacoes[preto] : ocupacoes[branco];

    while (ataques)
    {
        int destino = getLeastBitIndex(ataques);

        if (getBit(ocupacao_oponente, destino))
        {
            adicionarLance(listaLances, codificar_lance(origem, destino, tipo_peca, 0, 1, 0, 0, 0));
        }
        else
        {
            adicionarLance(listaLances, codificar_lance(origem, destino, tipo_peca, 0, 0, 0, 0, 0));
        }

        clearBit(ataques, destino);
    }
}

int fazer_lance(int lance, int flag, estado_jogo backup)
{

    if (flag == todosLances)
    {

        SALVAR_ESTADO(backup);

        int origem = get_origem(lance);
        int destino = get_destino(lance);
        int peca = get_peca(lance);
        int promocao = get_peca_promovida(lance);
        int captura = get_captura(lance);
        int movimento_duplo = get_double_push(lance);
        int en_passant_flag = get_en_passant(lance);
        int roque_flag = get_roque(lance);

        clearBit(bitboards[peca], origem);
        setBit(bitboards[peca], destino);

        if (captura)
        {
            int pecaInicial, pecaFinal;
            if (lado_a_jogar == branco)
            {
                pecaInicial = p;
                pecaFinal = k;
            }
            else
            {
                pecaInicial = P;
                pecaFinal = K;
            }
            for (int peca = pecaInicial; peca <= pecaFinal; peca++)
            {
                if (getBit(bitboards[peca], destino))
                {
                    clearBit(bitboards[peca], destino);
                    break;
                }
            }
        }

        if (promocao)
        {
            if ( lado_a_jogar == branco) {
                clearBit(bitboards[P], destino);
            } else {
                clearBit(bitboards[p], destino);
            }

            setBit(bitboards[promocao], destino);
        }

        if(en_passant_flag) {
            if (lado_a_jogar == branco) {
                clearBit(bitboards[p], destino - 8);
            } else {
                clearBit(bitboards[P], destino + 8);
            }
        }   

        en_passant = -999;

        if (movimento_duplo)
        {
            if (lado_a_jogar == branco)
            {
                en_passant = origem + 8;
            }
            else
            {
                en_passant = origem - 8;
            }
        }

        if (roque_flag)
        {
            if (lado_a_jogar == branco)
            {
                // Roque pequeno
                if (destino == g1)
                {
                    clearBit(bitboards[R], h1);
                    setBit(bitboards[R], f1);
                }
                // Roque grande
                else if (destino == c1)
                {
                    clearBit(bitboards[R], a1);
                    setBit(bitboards[R], d1);
                }
            }
            else
            {
                // Roque pequeno
                if (destino == g8)
                {
                    clearBit(bitboards[r], h8);
                    setBit(bitboards[r], f8);
                }
                // Roque grande
                else if (destino == c8)
                {
                    clearBit(bitboards[r], a8);
                    setBit(bitboards[r], d8);
                }
            }
        }

        //roque
        roque &= roque_permissoes[origem];
        roque &= roque_permissoes[destino];

        // ocupacoes
        memset(ocupacoes, 0ULL, sizeof(ocupacoes));
        for (int peca = P; peca <= k; peca++){
            ocupacoes[peca <= K ? branco : preto] |= bitboards[peca];
        }
        ocupacoes[ambos] |= ocupacoes[branco];
        ocupacoes[ambos] |= ocupacoes[preto];

        // mudar lado a jogar
        lado_a_jogar ^= 1; 

        // verificar se o rei do lado que acabou de jogar ficou em cheque
        if(casaEstaAtacada((lado_a_jogar == branco) ? getLeastBitIndex(bitboards[k]) : getLeastBitIndex(bitboards[K]), lado_a_jogar)) {
            // o rei ficou em cheque, desfaz o lance
            RESTAURAR_ESTADO(backup);
            return 0;
        } else {
            return 1;
        }

    }
    else
    {
        if (get_captura(lance))
        {
            fazer_lance(lance, todosLances, backup);
        }
        else
        {
            return 0;
        }
    }


}

void gerar_lances(lances *listaLances)
{
    listaLances->contador = 0;

    for (int peca = P; peca <= k; peca++)
    {
        if (!bitboards[peca])
            continue; // Pula se não há peças deste tipo

        if (lado_a_jogar == branco)
        {
            if (peca == P)
            {
                u64 bitboardCopia = bitboards[peca];
                while (bitboardCopia)
                {
                    int origem = getLeastBitIndex(bitboardCopia);
                    gerar_movimentos_peao_branco(origem, listaLances);
                    clearBit(bitboardCopia, origem);
                }
            }
            else if (peca == K)
            {
                gerar_roque_branco(listaLances);
                u64 bitboardCopia = bitboards[peca];
                while (bitboardCopia)
                {
                    int origem = getLeastBitIndex(bitboardCopia);
                    gerar_movimentos_pecaGrande(origem, branco, peca, listaLances);
                    clearBit(bitboardCopia, origem);
                }
            }
            else if (peca == N || peca == B || peca == R || peca == Q )
            {
                u64 bitboardCopia = bitboards[peca];
                while (bitboardCopia)
                {
                    int origem = getLeastBitIndex(bitboardCopia);
                    gerar_movimentos_pecaGrande(origem, branco, peca, listaLances);
                    clearBit(bitboardCopia, origem);
                }
            }
        }
        else
        { // lado_a_jogar == preto
            if (peca == p)
            {
                u64 bitboardCopia = bitboards[peca];
                while (bitboardCopia)
                {
                    int origem = getLeastBitIndex(bitboardCopia);
                    gerar_movimentos_peao_preto(origem, listaLances);
                    clearBit(bitboardCopia, origem);
                }
            }
            else if (peca == k)
            {
                gerar_roque_preto(listaLances);
                u64 bitboardCopia = bitboards[peca];
                while (bitboardCopia)
                {
                    int origem = getLeastBitIndex(bitboardCopia);
                    gerar_movimentos_pecaGrande(origem, preto, peca, listaLances);
                    clearBit(bitboardCopia, origem);
                }
            }
            else if (peca == n || peca == b || peca == r || peca == q)
            {
                u64 bitboardCopia = bitboards[peca];
                while (bitboardCopia)
                {
                    int origem = getLeastBitIndex(bitboardCopia);
                    gerar_movimentos_pecaGrande(origem, preto, peca, listaLances);
                    clearBit(bitboardCopia, origem);
                }
            }
        }
    }
}
