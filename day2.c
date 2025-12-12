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

/* --- Outils pour calculs un peu grands (somme peut dépasser uint64) --- */
static unsigned __int128 u128_pow10(int k) {
    unsigned __int128 r = 1;
    for (int i = 0; i < k; i++) r *= 10;
    return r;
}

static unsigned __int128 u128_ceil_div(unsigned __int128 a, unsigned __int128 b) {
    return (a + b - 1) / b;
}

static unsigned __int128 u128_sum_arith(unsigned __int128 lo, unsigned __int128 hi) {
    unsigned __int128 n = hi - lo + 1;
    return (lo + hi) * n / 2;
}

/* Somme des nombres x = m*M dans [A,B]
 * avec m composé de k chiffres (pas de zéro en tête)
 */
static unsigned __int128 sum_scaled_in_range(
    uint64_t A64,
    uint64_t B64,
    int k,
    unsigned __int128 M
) {
    unsigned __int128 A = (unsigned __int128)A64;
    unsigned __int128 B = (unsigned __int128)B64;

    unsigned __int128 mmin = u128_pow10(k - 1);
    unsigned __int128 mmax = u128_pow10(k) - 1;

    unsigned __int128 lo = u128_ceil_div(A, M);
    unsigned __int128 hi = B / M;

    if (lo < mmin) lo = mmin;
    if (hi > mmax) hi = mmax;
    if (lo > hi) return 0;

    return M * u128_sum_arith(lo, hi);
}

/* Q1 : nombre invalide si le motif est répété exactement 2 fois (mm) */
static unsigned __int128 solve_q1_interval(uint64_t A, uint64_t B) {
    int maxD = digits_u64(B);
    unsigned __int128 total = 0;

    for (int k = 1; 2 * k <= maxD; k++) {
        /* x = m * (10^k + 1) => mm */
        unsigned __int128 M = u128_pow10(k) + 1;
        total += sum_scaled_in_range(A, B, k, M);
    }

    return total;
}

/* Affichage de __int128 sans se compliquer la vie */
static void print_u128(unsigned __int128 x) {
    if (x == 0) { putchar('0'); return; }
    char buf[64];
    int i = 0;
    while (x > 0) {
        buf[i++] = (char)('0' + (unsigned)(x % 10));
        x /= 10;
    }
    while (i--) putchar(buf[i]);
}

static int digits_u64(uint64_t x) {
    int d = 1;
    while (x >= 10) { x /= 10; d++; }
    return d;
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

    unsigned __int128 sum1 = 0;

    for (size_t i = 0; i < merged.n; i++) {
        sum1 += solve_q1_interval(merged.v[i].a, merged.v[i].b);
    }

    print_u128(sum1);
    putchar('\n');

    free(merged.v);
    return 0;
}

