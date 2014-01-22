#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

int conf[256][256];
char num[1024];

int main(){
    int i, j, n, N;
    int c, o;

    memset(conf, 0, sizeof(conf));
    
    for(N = 0; scanf("%d %d", &c, &o) == 2; N++)
        conf[o][c]++;
    for(i=1, n=0; i < 30; i++){
        n+=conf[i][i];
        for(j=1; j < 30; j++)
            printf("%d ",conf[i][j]);
        printf("\n");
    }
    printf("%.8lf\n",(double) n/N);
      return 0;
}
