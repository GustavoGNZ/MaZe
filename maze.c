#include <stdio.h>  


#define u64 unsigned long long 

#define getBit(bitboard, casa) ((bitboard & (1ULL << casa))) // Verifica se o bit correspondente à casa está setado (ou seja, há uma peça nessa casa)
#define setBit(bitboard, casa) (bitboard |= (1ULL << casa)) // Define o bit correspondente à casa como 1 (ou seja, coloca uma peça nessa casa)
#define clearBit(bitboard, casa) (bitboard &= ~(1ULL << casa)) // Limpa o bit correspondente à casa (ou seja, remove a peça dessa casa)

// casa do tabuleiro
enum { a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8 };
// lados 
enum { branco, preto };

// [lado][casa]
u64 tabela_ataques_peao[2][64];

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




void gerar_ataques(){
    for(int casa = 0; casa < 64; casa++){
        tabela_ataques_peao[branco][casa] = gerar_ataque_peao(branco, casa);
        tabela_ataques_peao[preto][casa] = gerar_ataque_peao(preto, casa);
    }
}




// Imprime o bitboard que representa o tabuleiro de xadrez.
void printBitboard(u64 bitboard){

    for(int linha = 7; linha >= 0; linha--){
        printf(" %d ", linha + 1);

        for(int coluna = 0; coluna < 8; coluna++){
            int casa = linha * 8 + coluna;

            if(getBit(bitboard, casa)){
                printf(" 1 ");
            } else {
                printf(" 0 ");
            }
        }

        printf("\n");
    
    }
        printf("    a  b  c  d  e  f  g  h\n");
        printf("\n Bitboard: %lld\n", bitboard);

}


int main (){

    u64 bitboard = 0ULL;
    printBitboard(bitboard);
    gerar_ataques();



    return 0;
}