#include <stdio.h>
#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#define min(a, b) (a < b? a : b)
#define max(a, b) (a > b? a : b)

typedef struct Char {
    int c;
    double v[256];
} Char;

Char chars[1 << 12];
int D, N;
double min[256];
double max[256];

int read_char(Char *c){
    int i;
    uint64_t a;
    double *d = (double *) &a;

    scanf("%d", &(c->c));
    for(i = 0; i < D; i++){
        if(scanf("%" PRIX64, &a) != 1)
            return 1;
        c->v[i] = *d;
    }

    return 0;
}

void normalize(Char *c){
    int i;

    for(i = 0; i < D; i++)
        c->v[i] = (c->v[i] - min[i])/(max[i] - min[i]);
}

int main(){
    int i, n;
    uint64_t *min64 = (uint64_t *) min;
    uint64_t *max64 = (uint64_t *) max;
    uint64_t *v;

    scanf("%d", &D);

    for(N = 0; !read_char(chars + N); N++);

    for(i = 0; i < D; i++)
        min[i] = 1e100, max[i] = -1e100;

    for(i = 0; i < N; i++)
        for(n = 0; n < D; n++){
            min[n] = min(min[n], chars[i].v[n]);
            max[n] = max(max[n], chars[i].v[n]);
        }

    for(i = 0; i < N; i++)
        normalize(chars + i);

    printf("%d\n", D);
    for(i = 0; i < D; i++)
        printf("%016" PRIX64 " %016" PRIX64 "\n", min64[i], max64[i]);

    for(i = 0; i < N; i++){
        printf("%d\n", chars[i].c);
        v = (uint64_t *) chars[i].v;
        for(n = 0; n < D; n++)
            printf("%016" PRIX64 "\n", v[n]);
    }

    return 0;
}
