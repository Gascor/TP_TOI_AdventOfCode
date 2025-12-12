#include <stdio.h>
#include <stdlib.h>

/*
 * Compte le nombre de passages par 0 pendant UNE rotation,
 * sans simuler chaque cran.
 */
static long long count_passes_zero(int pos, char dir, long long dist)
{
    long long k0;

    if (dir == 'R') {
        // (pos + k) % 100 == 0
        k0 = (100 - pos) % 100;
        if (k0 == 0) k0 = 100;
    } else { // 'L'
        // (pos - k) % 100 == 0
        k0 = pos % 100;
        if (k0 == 0) k0 = 100;
    }

    if (dist < k0)
        return 0;

    return 1 + (dist - k0) / 100;
}