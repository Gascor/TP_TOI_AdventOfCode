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

int main(void)
{
    int pos = 50;                 // position initiale
    long long q1 = 0;             // question 1
    long long q2 = 0;             // question 2

    char dir;
    long long dist;

    /* Lecture depuis stdin : R39, L20, ... */
    while (scanf(" %c%lld", &dir, &dist) == 2) {

        /* Question 2 : passages par 0 pendant la rotation */
        q2 += count_passes_zero(pos, dir, dist);

        /* Mise à jour de la position finale */
        long long d = dist % 100;

        if (dir == 'R') {
            pos = (pos + d) % 100;
        } else { // 'L'
            pos = (pos - d) % 100;
            if (pos < 0) pos += 100;
        }

        /* Question 1 : finit sur 0 */
        if (pos == 0)
            q1++;
    }

    /* Résultats */
    printf("%lld\n%lld\n", q1, q2);
    return 0;
}
