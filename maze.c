#include "bitboard.h"
#include "ataques.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    u64 mainBitboard = 0ULL;    

    init_magic_numbers(); // hard code magic numbers para bispo e torre                                                                                                                                                      
    for(int square = 0; square < 64; square++)
    {
        printf("0x%llxULL,\n", magics_torre[square]);

    }

    printf("\n\n\n");

    for(int square = 0; square < 64; square++)
    {
        printf("0x%llxULL,\n", magics_bispo[square]);
    }                                                                                                                                                                                                                       

    return 0;
}
                                                                                                                        