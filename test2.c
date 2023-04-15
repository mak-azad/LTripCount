#include <stdio.h>
#include <stdlib.h>

#define N ((1 << 25)+10)
#define I (1 << 25)
#define J (1 << 25)
#define K (1 << 25)
volatile long * volatile a;
volatile long * volatile b;
volatile long * volatile c;

volatile long long x=0;
volatile long delta;

void __attribute__((noinline)) Dead(){
        int i;
        for(i = 0 ; i < I; i++){
                        a[i] = i;
        }
}

void __attribute__((noinline)) Kill(){
        int i;
        for(i = 0 ; i < J; i++){
                        b[i] = i;
        }
}


void __attribute__((noinline)) Use(){
        int i;
        for(i = 0 ; i < K; i++){
                        x += a[i] + b[i];
        }
}



void __attribute__((noinline))Run(void){
        a = malloc(N * sizeof(long));
        b = malloc(N * sizeof(long));
        //c = malloc(N * sizeof(long));
       // printf("Alloc-> %llx, %llx, %llx\n", a, b, c);
        Dead();
        Kill();
        Use();
       
        
}





int main(int argc, char **argv)
{
        Run();
        free(a);
        free(b);
        free(c);
        return 0;
}

