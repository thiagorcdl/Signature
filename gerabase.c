#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char s[128];

int main(int argc, char *argv[]){
    int i, c, n, d;
    FILE *f;

    if(argc < 3){
        fprintf(stderr, "Usage: %s DIMENSIONS ARRAYS\n", argv[0]);
        return 1;
    }

    d = atoi(argv[1]);
    printf("%d\n", d);

    for(i = 2; i < argc; i++){
        f = fopen(argv[i], "r");
        if(!f){
            fprintf(stderr, "Failed to open %s.\n", argv[i]);
            return 2;
        }

        for(n = strlen(argv[i]) - 1; argv[i][n] != '_'; n--);
        argv[i][n] = 0;
        for(n--; argv[i][n] != 'a'; n--);
        c = atoi(argv[i] + n + 1);

        printf("%d\n", c);
        for(n = 0; n < d; n++){
            fscanf(f, "%s", s);
            printf("%s\n", s);
        }

        fclose(f);
    }

    return 0;
}
