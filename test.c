#include <stdio.h>
int main(int argc, const char** argv) {
    int i,j, sum = 0, num;
    scanf("%d", &num);
    for(i = 1; i <=num; i++)
    {
          sum += i;
    }

    for(j = 1; j <=10; j++)
    {
          sum += j;
    }
      
    return 0;
}
