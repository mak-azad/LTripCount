#include <stdio.h>
int L[100] = {0};
void logop(int i, int lcounter) {
    if (L[lcounter] == 0){
        printf("computed: %i %i\n", i, lcounter);
        L[lcounter] = 1;
    }
}
