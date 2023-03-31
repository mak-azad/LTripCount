#include <stdio.h>
int main(int argc, const char** argv) {
    int i,j,k,l, sum = 0, num;
    // //scanf("%d", &num);

    // four nested loops
     for(i = 1; i <=2; i++)
     { 
         sum += i;
           for(l = 1; l <= 4; l++)
             {
                 sum += l;
                  for(j = 1; j <= 3; j++)
                  {
                     sum += j;
                       for(int k = 0; k < 10; k++)
                      {
                          sum += k;
                      }
                  }
             }
     }



     for(i = 1; i <=2; i++)
     {                 
           sum += i;
     }

      for(int k = 0; k < 10; k++)
      {
            sum += k;
      }
   
    //     //print(outer count)   
    
  

   
      
    return 0;
}
