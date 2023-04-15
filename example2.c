#include <stdio.h>

void loop1 (){
    int sum = 0;
    for(int i = 0; i < 10; i++)
    {
        sum += i;
    }
    
}

void loop2(){
    int sum = 0;
    for(int i = 0; i < 5; i++)
    {
        sum += i;
    }
    
}


int main(int argc, const char** argv) {
    int num;
    num = 20;
    loop1();
    loop2();
    return 0;
}
