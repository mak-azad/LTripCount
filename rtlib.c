#include <stdio.h>
#include <stdlib.h>
int L[100] = {0};


void logop(int i, int lcounter, int loc, char *s) {
    FILE *fp = fopen("loopinfo.txt", "a");
    if (L[lcounter] == 0){
        fprintf(fp, "%i %i %i %s\n", loc, i, lcounter, s);
        L[lcounter] = 1;
    }
}
