#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <math.h>
#include <string.h>

#define min(a, b) (a < b? a : b)
#define max(a, b) (a > b? a : b)

typedef struct Char {
    int c;
    double *v;
    double d;
} Char;

Char base[1 << 12];
double characteristics[(1 << 12) + 1][256];
int D, N, K;
double min[256];
double max[256];
int tem[256];
Char test;

int load_base(char *filename){
    uint64_t *min64 = (uint64_t *) min;
    uint64_t *max64 = (uint64_t *) max;
    uint64_t *v;
    FILE *f = fopen(filename, "r");
    int i, n;

    if(!f)
        return 1;

    fscanf(f, "%d", &D);

    for(i = 0; i < D; i++)
        fscanf(f, "%" PRIX64 " %" PRIX64, min64 + i, max64 + i);


    for(i = 0; fscanf(f, "%d", &(base[i].c)) == 1; i++){
        base[i].v = characteristics[i];
        v = (uint64_t *) base[i].v;
        for(n = 0; n < D; n++)
            fscanf(f, "%" PRIX64, v + n);
    }
    N = i;

    fclose(f);

    return 0;
}

int load_test(char *s, Char *t){
    t->v = characteristics[N];
    uint64_t *v = (uint64_t *) t->v;
    FILE *f = fopen(s, "r");
    int i;

    if(!f)
        return 1;

    for(i = 0; i < D; i++){
        fscanf(f, "%" PRIX64, v + i);
        t->v[i] = (t->v[i] - min[i]) / (max[i] - min[i]);
    }
    fscanf(f, "%d", &(t->c));

    fclose(f);

    return 0;
}

double dist(const Char *a, const Char *b){
    double d = 0.0;
    int i;

    for(i = 0; i < D; i++)
        d += (a->v[i] - b->v[i]) * (a->v[i] - b->v[i]);

    return sqrt(d);
}

int sorteia(Char *a, Char *b){
    return (a->d < b->d? -1 : 1);
}

int main(int argc, char *argv[]){
    int i, n;

    if(argc < 4){
        fprintf(stderr, "Usage: %s BASE K CASE\n", argv[0]);
        return 1;
    }

    K = atoi(argv[2]);

    if(load_base(argv[1])){
        fprintf(stderr, "Could not open %s for reading.\n", argv[1]);
        return 2;
    }

    for(i = 3; i < argc; i++){
        if(load_test(argv[i], &test)){
            fprintf(stderr, "Could not open %s for reading.\n", argv[i]);
            return 3;
        }

        for(n = 0; n < N; n++)
            base[n].d = dist(&test, base+n);


        qsort(base, N, sizeof(Char), (__compar_fn_t) sorteia);
        for(n = 0; n < K; n++)
            printf("%d ", base[n].c);
        printf("%d\n", test.c);
    }

    return 0;
}
