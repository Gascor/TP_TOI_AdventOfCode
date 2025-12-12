#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint64_t a, b;
} Interval;

/* Petit vecteur dynamique (fait maison) */
typedef struct {
    Interval *v;
    size_t n, cap;
} Vec;

static void vec_push(Vec *x, Interval it) {
    if (x->n == x->cap) {
        x->cap = x->cap ? x->cap * 2 : 16;
        x->v = (Interval*)realloc(x->v, x->cap * sizeof(Interval));
        if (!x->v) { perror("realloc"); exit(1); }
    }
    x->v[x->n++] = it;
}

/* Parse la ligne du sujet : "a-b,c-d,..." */
static Vec parse_intervals(const char *s) {
    Vec v = (Vec){0};

    /* On duplique car strtok modifie la chaîne */
    char *tmp = strdup(s);
    if (!tmp) { perror("strdup"); exit(1); }

    const char *delim = ", \t\r\n";
    char *saveptr = NULL;
    char *tok = strtok_r(tmp, delim, &saveptr);

    while (tok) {
        char *dash = strchr(tok, '-');
        if (!dash) {
            fprintf(stderr, "Token invalide (pas de '-'): %s\n", tok);
            exit(1);
        }
        *dash = '\0';

        uint64_t a = strtoull(tok, NULL, 10);
        uint64_t b = strtoull(dash + 1, NULL, 10);
        if (a > b) { uint64_t t = a; a = b; b = t; }

        vec_push(&v, (Interval){a, b});
        tok = strtok_r(NULL, delim, &saveptr);
    }

    free(tmp);
    return v;
}

static int cmp_interval(const void *p, const void *q) {
    const Interval *a = (const Interval*)p;
    const Interval *b = (const Interval*)q;
    if (a->a < b->a) return -1;
    if (a->a > b->a) return 1;
    if (a->b < b->b) return -1;
    if (a->b > b->b) return 1;
    return 0;
}

/* Fusionne les intervalles qui se chevauchent (ou adjacents) */
static Vec merge_intervals(Vec in) {
    Vec out = (Vec){0};
    if (in.n == 0) return out;

    qsort(in.v, in.n, sizeof(Interval), cmp_interval);

    Interval cur = in.v[0];
    for (size_t i = 1; i < in.n; i++) {
        Interval nx = in.v[i];
        if (nx.a <= cur.b + 1) {
            if (nx.b > cur.b) cur.b = nx.b;
        } else {
            vec_push(&out, cur);
            cur = nx;
        }
    }
    vec_push(&out, cur);
    return out;
}

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
    Vec raw = parse_intervals(input);
    free(input);

    Vec merged = merge_intervals(raw);
    free(raw.v);

    fprintf(stderr, "Intervalles après fusion : %zu\n", merged.n);

    free(merged.v);
    return 0;
}

