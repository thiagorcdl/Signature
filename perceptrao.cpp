#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <math.h>
#include <string.h>
#include <set>
#include <algorithm>

#define EPS 1e-9

using namespace std;

typedef struct Char {
    int c;
    double *v;
    double d;
} Char;

Char base[1 << 12];
double characteristics[(1 << 12) + 1][256];
int D, N, K; // D = num carac, N = num objetos
double mind[256];
double maxd[256];
double bias = 0.0, pesos[256];
Char test;

inline int cmpf(double a, double b){
    if(a - b < EPS) return 0;
    return (a < b? -1 : 1);
}

inline int hardlim(double a){
    return (cmpf(a, 0.0) <= 0? 0 : 1);
}

int load_base(char *filename, int c1, int c2){
    uint64_t *min64 = (uint64_t *) mind;
    uint64_t *max64 = (uint64_t *) maxd;
    uint64_t *v;
    FILE *f = fopen(filename, "r");
    int i, n, q;
    set<int> livres, usados;

    if(!f)
        return 1;

    fscanf(f, "%d", &D);

    for(i = 0; i < D; i++)
        fscanf(f, "%" PRIX64 " %" PRIX64, min64 + i, max64 + i);


    for(i = 0; fscanf(f, "%d", &(base[i].c)) == 1 ; i++){
        base[i].v = characteristics[i];
        v = (uint64_t *) base[i].v;
        for(n = 0; n < D; n++)
            fscanf(f, "%" PRIX64, v + n);
        if(base[i].c != c1 && base[i].c != c2){
            i--;
            continue;
        }
        base[i].c = (base[i].c == c1? 0 : 1);
        livres.insert(base[i].c);
    }
    N = i;

    fclose(f);

    /* Previne catastrophic forgetting */
    for(i = 0; i < N; i++){
        do {
            if(!livres.size())
                livres.swap(usados);
            q = *(livres.begin());
            livres.erase(livres.begin());
            usados.insert(q);

            for(n = i; n < N; n++)
                if(base[n].c == q) break;
        } while(n == N);

        swap(base[i], base[n]);
    }

    return 0;
}

void learn(int obj){
    int i, err;
    double *v;  
    double sum;

    v = base[obj].v;
    sum = bias;
    for(i = 0; i < D; i++)
        sum += v[i] * pesos[i];

    err = base[obj].c - hardlim(sum);

    if(err){
        for(i=0;i<D;i++)
            pesos[i] += (double) err * v[i] ;
        bias += (double) err;
    }
}

int decide(char *s, Char *t){
    t->v = characteristics[N];
    uint64_t *v = (uint64_t *) t->v;
    FILE *f = fopen(s, "r");
    int i;
    double sum;

    if(!f)
        return -1;

    sum = bias;
    for(i = 0; i < D; i++){
        fscanf(f, "%" PRIX64, v + i);
        t->v[i] = (t->v[i] - mind[i]) / (maxd[i] - mind[i]);
        sum += t->v[i] * pesos[i];
    }
    fscanf(f, "%d", &(t->c));

    fclose(f);

    return hardlim(sum);
}

int main(int argc, char *argv[]){
    int i, n, d;
    int r[2];

    // ./perceptrao base c1 c2 garoto
    if(argc < 5){
        fprintf(stderr, "Usage: %s BASE CLASS1 CLASS2 CASE\n",
            argv[0]);
        return 1;
    }

    r[0] = atoi(argv[2]);
    r[1] = atoi(argv[3]);


    if(load_base(argv[1], r[0], r[1])){
        fprintf(stderr, "Could not open %s for reading.\n", argv[1]);
        return 2;
    }

    memset(pesos, 0, sizeof(pesos));

    for(i = 0; i < N; i++)
        for(n = 0; n < N; n++)
            learn(n);

    d = decide(argv[4], &test);
    if(d == -1){
        fprintf(stderr, "Failed to open %s for reading.\n", argv[4]);
        return 3;
    }
    printf("%d %d\n", r[d], test.c);

    return 0;
}
