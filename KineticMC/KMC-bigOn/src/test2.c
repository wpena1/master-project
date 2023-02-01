#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void setPosition(DNODE defects[], int mean, int stdv, int bin)
{
    int range = mean*2;
    int step =(mean-stdv)/bin;
    int temp;
    float sum;
    float values[range];
    int number[bin*2+20];
    int numberofdf = 0;
    int num;
    int change = step;
    int i;
    int counter=0;
    int prev = 0;

    for(i = 0; i<=range; i++)
    {
        values[i]= gaussian(i,mean,stdv);
    }
    while(step<=(mean-stdv))
    {   sum = 0;
        for(i=prev; i<=step; i++)
        {
           temp = values[i];
           sum = sum+temp ;
        }
           num = roundf(sum);
           numberofdf = num + numberofdf;
           int j;
           for(j=0;j<=num;j++)
           {
               DNODE temp = defects[i];
               temp.z=prev + genrand_real1()*step;
               temp.y=genrand_real1()*20;
               temp.x=genrand_real1()*20;
               defects[i]=temp;
           }
           prev = step;
           if(step!=(mean-stdv)
           {step=change+step;}
           number[counter]= num;
           counter++;
    }
    int newchange = (stdv*2)/20;
    step = step + newchange;
    while(step<=(mean+stdv))
    {
        sum = 0;
        for(i=prev;i<=step;i++)
        {
            temp = values[i];
            sum = sum+temp;
        }
        num = roundf(sum);
        numberofdf = num + numberofdf;
        int j;
        for(j=0;j<=num;j++)
           {
               DNODE temp = defects[i];
               temp.z=prev + genrand_real1()*step;
               temp.y=genrand_real1()*20;
               temp.x=genrand_real1()*20;
               defects[i]=temp;
           }
        prev=step;
        if(step!=(mean+stdv)
        step = newchange + step;
        number[counter]=num;
        counter++;
    }

    step = step + change;
    while(step<=rangei)
    {
        sum = 0;
        for(i=prev;i<=step;i++)
        {
            temp = values[i];
            sum = sum+temp;
        }
        num = roundf(sum);
        numberofdf = num + numberofdf;
        int j;
        for(j=0;j<=num;j++)
           {
               DNODE temp = defects[i];
               temp.z=prev + genrand_real1()*step;
               temp.y=genrand_real1()*20;
               temp.x=genrand_real1()*20;
               defects[i]=temp;
           }
        prev=step;
        if(step!=(rangei)
        step = change + step;
        number[counter]=num;
        counter++;
    }

}
