#include "../include/ataques.h"
#include "../include/globals.h"
#include "../include/bitboard.h"

#include <stdio.h>
#include <string.h>

// Variável global para armazenar o estado do gerador de números aleatórios
unsigned int num_aleatorio = 1804289383;

// Gera um número aleatório usando xorshift32 (um gerador de números pseudo-aleatórios) foi criado assim para rodar em windows e linux pela diferenca na funcao rand() e random()
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
    // adicionar funcoes futuras que precisam ser inicializadas

    inicializarAtaquesPecas();
    // inicializarMagicNumbers(); ja estao gerados
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
void gerar_movimentos_peao_branco(int origem) {
    int destino = origem + 8;
    
    // Movimento simples
    if (destino < 64 && !getBit(ocupacoes[ambos], destino)) {
        if (origem >= a7 && origem <= h7) { // 7ª linha (promoção)
            printf("Peão promove %s%sq\n", casa_nome[origem], casa_nome[destino]);
            printf("Peão promove %s%sr\n", casa_nome[origem], casa_nome[destino]);
            printf("Peão promove %s%sb\n", casa_nome[origem], casa_nome[destino]);
            printf("Peão promove %s%sn\n", casa_nome[origem], casa_nome[destino]);
        } else {
            printf("Peao avança %s%s\n", casa_nome[origem], casa_nome[destino]);
            
            // Movimento duplo do peão (da 2ª linha)
            if (origem >= a2 && origem <= h2) {
                destino = origem + 16;
                if (destino < 64 && !getBit(ocupacoes[ambos], destino)) {
                    printf("Peao avança duplo %s%s\n", casa_nome[origem], casa_nome[destino]);
                }
            }
        }
    }
    
    // Capturas
    u64 ataques = tabela_ataques_peao[branco][origem] & ocupacoes[preto];
    while (ataques) {
        destino = getLeastBitIndex(ataques);
        
        if (origem >= a7 && origem <= h7) { // 7ª linha (promoção)
            printf("Peão captura e promove %s%sq\n", casa_nome[origem], casa_nome[destino]);
            printf("Peão captura e promove %s%sr\n", casa_nome[origem], casa_nome[destino]);
            printf("Peão captura e promove %s%sb\n", casa_nome[origem], casa_nome[destino]);
            printf("Peão captura e promove %s%sn\n", casa_nome[origem], casa_nome[destino]);
        } else {
            printf("Peão captura %s%s\n", casa_nome[origem], casa_nome[destino]);
        }
        
        clearBit(ataques, destino);
    }
    
    // En passant
    if (en_passant != -999 && origem >= a5 && origem <= h5) {
        if ((origem % 8 != 0 && en_passant == origem + 7) || 
            (origem % 8 != 7 && en_passant == origem + 9)) {
            printf("Peão captura en passant %s%s\n", casa_nome[origem], casa_nome[en_passant]);
        }
    }
}

// Função auxiliar para gerar movimentos de peões pretos
void gerar_movimentos_peao_preto(int origem) {
    int destino = origem - 8;
    
    // Movimento simples
    if (destino >= 0 && !getBit(ocupacoes[ambos], destino)) {
        if (origem >= a2 && origem <= h2) { // 2ª linha (promoção para peões pretos)
            printf("Peão promove %s%sq\n", casa_nome[origem], casa_nome[destino]);
            printf("Peão promove %s%sr\n", casa_nome[origem], casa_nome[destino]);
            printf("Peão promove %s%sb\n", casa_nome[origem], casa_nome[destino]);
            printf("Peão promove %s%sn\n", casa_nome[origem], casa_nome[destino]);
        } else {
            printf("Peao avança %s%s\n", casa_nome[origem], casa_nome[destino]);
            
            // Movimento duplo do peão (da 7ª linha para peões pretos)
            if (origem >= a7 && origem <= h7) {
                destino = origem - 16;
                if (destino >= 0 && !getBit(ocupacoes[ambos], destino)) {
                    printf("Peao avança duplo %s%s\n", casa_nome[origem], casa_nome[destino]);
                }
            }
        }
    }
    
    // Capturas
    u64 ataques = tabela_ataques_peao[preto][origem] & ocupacoes[branco];
    while (ataques) {
        destino = getLeastBitIndex(ataques);
        
        if (origem >= a2 && origem <= h2) { // 2ª linha (promoção para peões pretos)
            printf("Peão captura e promove %s%sq\n", casa_nome[origem], casa_nome[destino]);
            printf("Peão captura e promove %s%sr\n", casa_nome[origem], casa_nome[destino]);
            printf("Peão captura e promove %s%sb\n", casa_nome[origem], casa_nome[destino]);
            printf("Peão captura e promove %s%sn\n", casa_nome[origem], casa_nome[destino]);
        } else {
            printf("Peão captura %s%s\n", casa_nome[origem], casa_nome[destino]);
        }
        
        clearBit(ataques, destino);
    }
    
    // En passant
    if (en_passant != -999 && origem >= a4 && origem <= h4) {
        if ((origem % 8 != 0 && en_passant == origem - 9) || 
            (origem % 8 != 7 && en_passant == origem - 7)) {
            printf("Peão captura en passant %s%s\n", casa_nome[origem], casa_nome[en_passant]);
        }
    }
}

// Função auxiliar para gerar roque branco
void gerar_roque_branco() {
    // Roque pequeno
    if (roque & reiBranco_alaRei) {
        if (!getBit(ocupacoes[ambos], f1) && !getBit(ocupacoes[ambos], g1)) {
            if (!casaEstaAtacada(e1, preto) && !casaEstaAtacada(f1, preto) && !casaEstaAtacada(g1, preto)) {
                printf("Roque pequeno %s%s\n", casa_nome[e1], casa_nome[g1]);
            }
        }
    }
    
    // Roque grande
    if (roque & reiBranco_alaDama) {
        if (!getBit(ocupacoes[ambos], b1) && !getBit(ocupacoes[ambos], c1) && !getBit(ocupacoes[ambos], d1)) {
            if (!casaEstaAtacada(e1, preto) && !casaEstaAtacada(d1, preto) && !casaEstaAtacada(c1, preto)) {
                printf("Roque grande %s%s\n", casa_nome[e1], casa_nome[c1]);
            }
        }
    }
}

// Função auxiliar para gerar roque preto
void gerar_roque_preto() {
    // Roque pequeno
    if (roque & reiPreto_alaRei) {
        if (!getBit(ocupacoes[ambos], f8) && !getBit(ocupacoes[ambos], g8)) {
            if (!casaEstaAtacada(e8, branco) && !casaEstaAtacada(f8, branco) && !casaEstaAtacada(g8, branco)) {
                printf("Roque pequeno %s%s\n", casa_nome[e8], casa_nome[g8]);
            }
        }
    }
    
    // Roque grande
    if (roque & reiPreto_alaDama) {
        if (!getBit(ocupacoes[ambos], b8) && !getBit(ocupacoes[ambos], c8) && !getBit(ocupacoes[ambos], d8)) {
            if (!casaEstaAtacada(e8, branco) && !casaEstaAtacada(d8, branco) && !casaEstaAtacada(c8, branco)) {
                printf("Roque grande %s%s\n", casa_nome[e8], casa_nome[c8]);
            }
        }
    }
}

void gerar_lances() {
    for (int peca = P; peca <= k; peca++) {
        if (!bitboards[peca]) continue; // Pula se não há peças deste tipo
        
        if (lado_a_jogar == branco) {
            if (peca == P) {
                u64 bitboardCopia = bitboards[peca];
                while (bitboardCopia) {
                    int origem = getLeastBitIndex(bitboardCopia);
                    gerar_movimentos_peao_branco(origem);
                    clearBit(bitboardCopia, origem);
                }
            } else if (peca == K) {
                gerar_roque_branco();
                // TODO: Adicionar movimentos normais do rei depois
            }
        } else { // lado_a_jogar == preto
            if (peca == p) {
                u64 bitboardCopia = bitboards[peca];
                while (bitboardCopia) {
                    int origem = getLeastBitIndex(bitboardCopia);
                    gerar_movimentos_peao_preto(origem);
                    clearBit(bitboardCopia, origem);
                }
            } else if (peca == k) {
                gerar_roque_preto();
                // TODO: Adicionar movimentos normais do rei depois
            }
        }
    }
}