#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mersenTwister.h"
#include "mersenTwister.c"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif



double random_normal()
{
  return sqrt(-2*log(genrand_real1())) * cos(2*3.14159265*genrand_real1());
}

int main()
{
  unsigned long s;
  int array[0];
  s = time(NULL);
  init_genrand(s);
  int i;
  double rands[176];
  for (i=0; i<176; i++)
  rands[i] = 105 + 30*random_normal();
  int num = 0;
  int numb = 0;
  int numc = 0;
for(i=0;i<176;i++)
  {
	if(rands[i]<75)
	num++;
    if(135>rands[i]&&rands[i]>75)
	numb++;
	if(rands[i]>135)
	numc++;
	printf("%f\n", rands[i]);
  }
  printf("%d  %d  %d %d \n", num, numb, numc, array[0]);

  return 0;

}
