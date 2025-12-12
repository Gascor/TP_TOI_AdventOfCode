#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* Lecture simple de tout stdin dans un buffer */
static char *read_all_stdin(void) {
    size_t cap = 1 << 20;   /* 1 Mo au départ */
    size_t n = 0;
    char *buf = (char*)malloc(cap);
    if (!buf) { perror("malloc"); exit(1); }

    int c;
    while ((c = getchar()) != EOF) {
        if (n + 1 >= cap) {
            cap *= 2;
            buf = (char*)realloc(buf, cap);
            if (!buf) { perror("realloc"); exit(1); }
        }
        buf[n++] = (char)c;
    }
    buf[n] = '\0';
    return buf;
}

int main(void) {
    char *input = read_all_stdin();

    /* TODO : parser les intervalles + résoudre */
    /* Pour l’instant on affiche juste la taille lue */
    fprintf(stderr, "Lu %zu caractères\n", strlen(input));

    free(input);
    return 0;
}
