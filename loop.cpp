#include <stdio.h>
#include "loop.h"

void runLoop(int n)
{
    int sum = 0;
    for(int i = 1; i <= n; i++){
        sum += i;
    }
}