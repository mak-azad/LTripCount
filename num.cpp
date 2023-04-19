#include "num.h"

Num::Num() : num(0) {}
Num::Num(int n): num(n) {}

int Num::getNum(){
    int sum = 0;
    for(int i = 1; i <=2; i++)
    {                 
           sum += i;
    }

      for(int k = 0; k < 10; k++)
      {
            sum += k;
      }

      
    return num+sum;
}