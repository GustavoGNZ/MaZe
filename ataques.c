#include "ataques.h"
#include <stdio.h>
#include <string.h>

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

// gerado usando init_magic_numbers() 
const u64 magics_bispo[64] = {
    0x10c4048800000082ULL,
    0x10800100021400ULL,
    0x1420810000a00000ULL,
    0x4a80040000042ULL,
    0x4800a09006000810ULL,
    0x8143010101002000ULL,
    0x540004900470302ULL,
    0x908088161ULL,
    0x10026001364808ULL,
    0x800380000088812ULL,
    0x8048a000080000ULL,
    0xa012028400108002ULL,
    0x200044000e020008ULL,
    0x504010a040410720ULL,
    0x8000001b84044881ULL,
    0x800000118c01000ULL,
    0x244661004c080821ULL,
    0x22240820802201eULL,
    0x10008180800400ULL,
    0x940000900081824ULL,
    0x1800012898000018ULL,
    0x5010200000641000ULL,
    0x62040981c0001032ULL,
    0x21000401290040ULL,
    0x100e000202108000ULL,
    0x704840810202ULL,
    0x9558000000081ULL,
    0x4002000c80840040ULL,
    0x20d2000200040800ULL,
    0x821011000000ULL,
    0x14001808180231ULL,
    0x8012481000210800ULL,
    0x910009408028010ULL,
    0x80021408687ULL,
    0x400009201000210ULL,
    0x300000480010008ULL,
    0x108002a100101ULL,
    0x901200524402a00ULL,
    0x1000c00220604880ULL,
    0x100000c010140ULL,
    0x410041240190800ULL,
    0x1804000b00810100ULL,
    0x2400400200b010ULL,
    0x50001002200c020ULL,
    0x1820282000348420ULL,
    0x1110204002440080ULL,
    0x1400440000ac2238ULL,
    0x28c804440208004ULL,
    0x402cc680a028c010ULL,
    0xa00018800004ULL,
    0x8400001208010100ULL,
    0x1020408a00844408ULL,
    0x5200c28160214800ULL,
    0xa1229804ULL,
    0x5005002820000ULL,
    0x8530400011240200ULL,
    0x8000004210c00002ULL,
    0x40080286082ULL,
    0x8000000000442220ULL,
    0x1000000200840251ULL,
    0x500020122820600ULL,
    0x100301210608ULL,
    0x840400580000048ULL,
    0x2ac0402982000040ULL,
};


// gerado usando init_magic_numbers() 
const u64 magics_torre[64] = {
    0x780a080080080056ULL,
    0x4000440621811200ULL,
    0xc00000e00a000500ULL,
    0x6400411200444402ULL,
    0x2840122001050000ULL,
    0x402800018080192ULL,
    0x140008048000100ULL,
    0x9000200289008000ULL,
    0xa00880900000eaULL,
    0x10040080124022ULL,
    0x8061840001108005ULL,
    0x200520402014140ULL,
    0x1400063040000ULL,
    0x400032040004ULL,
    0x895a0000120ULL,
    0x8e00102ULL,
    0x1013748002001002ULL,
    0x9000988000000010ULL,
    0x44501200012088c4ULL,
    0x20670412015020ULL,
    0x84b0080800100009ULL,
    0x9040200020002110ULL,
    0xa00000004c21100ULL,
    0x8004012800a0000ULL,
    0x5010040002558250ULL,
    0x800a204000110010ULL,
    0x8302420c01084200ULL,
    0x8110888004010ULL,
    0x4c0088200004c241ULL,
    0x8040200508000010ULL,
    0x8009005282000428ULL,
    0x2804140008080ULL,
    0x408240100a000120ULL,
    0x40810004022a830ULL,
    0x3002054040004005ULL,
    0x300004090020800ULL,
    0x840048102080000ULL,
    0x8004000c2024090ULL,
    0x61200000080c000aULL,
    0x41480210000420ULL,
    0x830000b5008004ULL,
    0x2280000040410040ULL,
    0x80004001020000ULL,
    0x1000100010010000ULL,
    0x5201400820a6002ULL,
    0x604000800001280ULL,
    0x114220200104005ULL,
    0xa0001200000c020ULL,
    0x110c820900042450ULL,
    0x2015002405000ULL,
    0x2605400040003000ULL,
    0x200020200202ULL,
    0x9600810020002009ULL,
    0xa000000002001200ULL,
    0x4a8008094201100ULL,
    0xc2008000000060ULL,
    0x80b1428200040040ULL,
    0xc8408402000048ULL,
    0x18224400002d0480ULL,
    0x2040000906002001ULL,
    0x1410020c0104ULL,
    0x81861cc01800000ULL,
    0x1208215000040ULL,
    0x228005040004ULL,
};


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
u64 gerarMagicNumber(){
    return gerarNumeroAleatorio64bits() & gerarNumeroAleatorio64bits() & gerarNumeroAleatorio64bits();
}

u64 find_magic_number(int casa, int bits_relevantes, int peca){
    u64 ocupacoes[4096]; // Array para armazenar ocupações
    u64 ataques[4096]; // Array para armazenar ataques
    u64 ataques_usados[4096]; // Array para armazenar ataques usados

    u64 ataque_peca;

    if(peca == bispo){
        ataque_peca = gerar_ataque_bispo(casa);
    } else if(peca == torre){
        ataque_peca = gerar_ataque_torre(casa);
    } else {
        printf("Peca invalida para encontrar magic number\n");
        return 0ULL;
    }

    int indices_ocupacao = 1 << bits_relevantes; 

    for(int i = 0; i < indices_ocupacao; i++) {
        // Gera uma ocupação aleatória
        ocupacoes[i] = set_occupancy(i, bits_relevantes, ataque_peca);

        // Calcula o ataque para a ocupação atual
        if(peca == bispo){
            ataques[i] = gerar_ataque_bispo_tempo_real(casa, ocupacoes[i]);
        } else if(peca == torre){
            ataques[i] = gerar_ataque_torre_tempo_real(casa, ocupacoes[i]);
        }
   
    }

    // 1048576 = 2^20 escolhido arbitrariamente para o tamanho do array de ataques usados
    for(int j = 0; j < 1048576; j++){

        u64 magic_number = gerarMagicNumber();


        if(contarBits((ataque_peca * magic_number) & 0xFF00000000000000) < 6) {
            continue; // Se o magic number não gera ataques relevantes, ignora
        }

        memset(ataques_usados, 0ULL, sizeof(ataques_usados)); // Limpa o array de ataques usados

        int indice, valido;

        for(indice = 0, valido = 0; !valido && indice < indices_ocupacao; indice++) {
            // Calcula o índice de ataque usando o magic number
            int index = (ocupacoes[indice] * magic_number) >> (64 - bits_relevantes);

            // Verifica se o ataque já foi usado
            if (ataques_usados[index] == 0ULL) {
                ataques_usados[index] = ataques[indice]; // Marca o ataque como usado
                valido = 1; // Marca como válido
            } else if (ataques_usados[index] != ataques[indice]) {
                valido = 0; // Se o ataque já foi usado e é diferente, invalida o magic number
            }
        }
        if (valido) {
            return magic_number;
        }
        return 0ULL; // Se não encontrou um magic number válido, retorna 0
    }

}

// gera magic numbers hard coded para bispo e torre
void init_magic_numbers() {
    // Inicializa os magic numbers para bispo e torre
    for (int casa = 0; casa < 64; casa++) {
        u64 magic_bispo = find_magic_number(casa, bits_relevantes_bispo[casa], bispo);
        u64 magic_torre = find_magic_number(casa, bits_relevantes_torre[casa], torre);

            printf(" 0x%llxULL,\n",magic_bispo);
            printf(" 0x%llxULL,\n",magic_torre);

    }
}

// Função auxiliar para verificar se uma posição está dentro do tabuleiro
int dentro_do_tabuleiro(int linha, int coluna) {
    return (linha >= 0 && linha < 8 && coluna >= 0 && coluna < 8);
};

// Verifica se a próxima casa na direção especificada é a última da direção
int eh_ultima_casa_na_direcao(int linha, int coluna, int deslocamento_linha, int deslocamento_coluna) {
    int proxima_linha = linha + deslocamento_linha;
    int proxima_coluna = coluna + deslocamento_coluna;

    return !dentro_do_tabuleiro(proxima_linha, proxima_coluna);
}

// Marca os ataques direcionais para bispo e torre
void marcar_ataque_direcional(u64 *ataque, int linha, int coluna, int deslocamentos[][2]) {
    for (int i = 0; i < 4; i++) {
        int nova_linha = linha;
        int nova_coluna = coluna;

        while (1) {
            int proxima_linha = nova_linha + deslocamentos[i][0];
            int proxima_coluna = nova_coluna + deslocamentos[i][1];

            if (!dentro_do_tabuleiro(proxima_linha, proxima_coluna)) {
                break;
            }

            // Verifica se a próxima casa já é a última da direção
            if (eh_ultima_casa_na_direcao(proxima_linha, proxima_coluna, deslocamentos[i][0], deslocamentos[i][1])) {
                break;
            }

            int nova_casa = proxima_linha * 8 + proxima_coluna;
            setBit(*ataque, nova_casa);

            nova_linha = proxima_linha;
            nova_coluna = proxima_coluna;
        }
    }
}

void marcar_ataque_direcional_tempo_real(u64 *ataque, int linha, int coluna, int deslocamentos[][2], int num_direcoes, u64 bitboard_ocupacao) {
    for (int i = 0; i < num_direcoes; i++) {
        int nova_linha = linha;
        int nova_coluna = coluna;

        while (1) {
            int proxima_linha = nova_linha + deslocamentos[i][0];
            int proxima_coluna = nova_coluna + deslocamentos[i][1];

            if (!dentro_do_tabuleiro(proxima_linha, proxima_coluna)) {
                break;
            }

            int nova_casa = proxima_linha * 8 + proxima_coluna;

            if (getBit(bitboard_ocupacao, nova_casa)) {
                // Casa ocupada: não pode continuar, nem marca
                break;
            }

            setBit(*ataque, nova_casa);
            nova_linha = proxima_linha;
            nova_coluna = proxima_coluna;
        }
    }
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
        
        if (dentro_do_tabuleiro(nova_linha, nova_coluna))
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
        
        if (dentro_do_tabuleiro(nova_linha, nova_coluna))
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

    marcar_ataque_direcional(&ataque, linha, coluna, deslocamentos);

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

    marcar_ataque_direcional(&ataque, linha, coluna, deslocamentos);

    return ataque;
}
// Gera o bitboard de ataques possíveis de uma torre considerando peças que bloqueiam
u64 gerar_ataque_bispo_tempo_real(int casa, u64 bitboard_ocupacao) {
    u64 ataque = 0ULL;
    int linha = casa / 8;
    int coluna = casa % 8;

    int deslocamentos[4][2] = {
        {1, 1}, {1, -1}, {-1, -1}, {-1, 1}
    };

    marcar_ataque_direcional_tempo_real(&ataque, linha, coluna, deslocamentos, 4, bitboard_ocupacao);
    return ataque;
}

// Gera o bitboard de ataques possíveis de um bispo considerando peças que bloqueiam
u64 gerar_ataque_torre_tempo_real(int casa, u64 bitboard_ocupacao) {
    u64 ataque = 0ULL;
    int linha = casa / 8;
    int coluna = casa % 8;

    int deslocamentos[4][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };

    marcar_ataque_direcional_tempo_real(&ataque, linha, coluna, deslocamentos, 4, bitboard_ocupacao);
    return ataque;
}

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




