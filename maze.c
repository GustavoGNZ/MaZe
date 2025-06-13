#include "bitboard.h"
#include "ataques.h"

int main()
{

    u64 bitboard = 0ULL;
    printBitboard(bitboard);
    gerar_ataques_pecas();
    printBitboard(tabela_ataques_torre[b2]);
    printBitboard(tabela_ataques_bispo[g1]);

    return 0;
}
