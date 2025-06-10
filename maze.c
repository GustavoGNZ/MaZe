#include <stdio.h>  


#define u64 unsigned long long 

enum { a1, a2, a3, a4, a5, a6, a7, a8,
       b1, b2, b3, b4, b5, b6, b7, b8,
       c1, c2, c3, c4, c5, c6, c7, c8,
       d1, d2, d3, d4, d5, d6, d7, d8,
       e1, e2, e3, e4, e5, e6, e7, e8,
       f1, f2, f3, f4, f5, f6, f7, f8,
       g1, g2, g3, g4, g5, g6, g7, g8,
       h1, h2, h3, h4, h5, h6, h7, h8 };


#define getBit(bitboard, casa) ((bitboard & (1ULL << casa))) // Verifica se o bit correspondente à casa está setado (ou seja, há uma peça nessa casa)
#define setBit(bitboard, casa) (bitboard |= (1ULL << casa)) // Define o bit correspondente à casa como 1 (ou seja, coloca uma peça nessa casa)
#define clearBit(bitboard, casa) (bitboard &= ~(1ULL << casa)) // Limpa o bit correspondente à casa (ou seja, remove a peça dessa casa)



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




    return 0;
}