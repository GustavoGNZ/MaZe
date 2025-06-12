#include "bitboard.h"
#include "ataques.h"

int main() {


    u64 bitboard = 0ULL;
    printBitboard(bitboard);
    gerar_ataques_pecas();
    printBitboard(tabela_ataques_torre[b2]);

    
    
    return 0;
}
