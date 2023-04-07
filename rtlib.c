#include <stdio.h>
int L[100] = {0};
void logop(int i, int lcounter, int loc, char *s) {
    if (L[lcounter] == 0){
        printf("%i %i %i %s\n", loc, i, lcounter, s);
        L[lcounter] = 1;
    }
}
