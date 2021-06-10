#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void swap (int *x, int *y)
{
    int temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

int factorial(int i)
{
   if(i == 0)
      return 1;

   return i * factorial(i-1);

}
 
void permute(int *a, int i, int n, long int *count, int *list) 
{
   int j, k; 
   if (i == n){

     for(k = 0; k <= n; k++)
       *(list + 5 * *count + k) = *(a+k);

     *count = *count + 1;

     //printf("%ld\n", *count);
     printf("%i%i%i%i%i\n", *a, *(a+1), *(a+2), *(a+3), *(a+4));
     //printf("%i%i%i%i%i%i%i%i%i\n", *a, *(a+1), *(a+2), *(a+3), *(a+4), *(a+5), *(a+6), *(a+7), *(a+8));
   }else
   {
        for (j = i; j <= n; j++)
       {
          swap((a + i), (a + j));
          permute(a, i + 1, n, count, list);
          swap((a + i), (a + j)); //backtrack
       }
   }
} 
 
int main()
{
   int array[] = {1, 2, 3, 4, 5};
   int *list;
   int x, y;
   list = (int*) malloc(120 * 5 * sizeof(int));
   long int count = 0;
   int len;
   len = 4;
   permute(array, 0, len, &count, list);

   for (x = 0; x < 120; x++)
   {
      for(y = 0; y < 5; y++)
         printf("%i", *(list + x * 5 + y));

      printf(" %i\n", x);
   }  
   return 0;

   free(list);
}