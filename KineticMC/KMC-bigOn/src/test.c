#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<string.h>

typedef struct defnode
{
	int x,y,z;
	float rate;
	int type;
    struct   DNODE *next;
}DNODE;

void settype(DNODE *defects);
void testf(DNODE *adress);
void removedf(DNODE *node, int counter);
double N = 100.0;
FILE *data;
FILE *output;
int test;
int main(int argc, char argv[])
{

    int counter = 0;
    char *lines[14];
    char line[14];
    double values[14];
    int i;
    output=fopen("put.txt","w");
    if(!output)
    {
        printf("error: file not opened\n");
        exit(-1);
    }
    data = fopen("data.txt","r");
    if(!data)
    {
        printf("error: file not opened\n");
        exit(-1);
    }
   // printf("something here\n");
    while(fgets(line, sizeof(line), data)!=NULL)
    {
     //   printf("value %s\n",line);
        lines[counter]=line;
        values[counter]=atof(line);
      //  printf("still in loop %f\n",values[counter]);
        //fprintf(output,"writing random text2.\n");
        counter++;
    }
    fclose(data);
   // fclose(output);
    double K;
    for (i = 0; i<counter; i++)
    {   K = values[i];
    //    printf("value in double %f.\n",  K);
    }
    test = 567;
    DNODE *defects =(DNODE *) malloc((N+1)*sizeof(DNODE));
    DNODE *prev = &defects[0];
    for(i=0;i<(N+1);i++)
    {
        //DNODE curr = *prev;
        prev->type=i%2;
        prev->rate=i-1;
        if(i!=100)
        {
        prev->next=&defects[i+1];
        prev=&defects[i+1];}
        else{
            prev->next=NULL;
        }
    }
    DNODE temp;
    for(i=0;i<(N+1);i++)
    {  // if(i!=100)
        //temp = *(defects[i]).next;
       // printf("this is type %d. and this is rate %f and this is type of next %d.\n", defects[i].type, defects[i].rate, temp.type);
        fprintf(output, "this is type %d. and this is rate %f. i: %d \n", defects[i].type, defects[i].rate,i);
    }
    //settype(defects);
    int c =0;
    for(i=1;i<(N+1);i++)
    {   if(i%2==0)
        {defects[i].rate = -2.0;
        c++;}
    }
    printf("this is N %f.\n", N);
    removedf(defects, c);
    printf("this is N %f.\n", N);
    for(i=0;i<(N+1);i++)
    {  // if(i!=100)
        //temp = *(defects[i]).next;
       // printf("this is type %d. and this is rate %f and this is type of next %d.\n", defects[i].type, defects[i].rate, temp.type);
        fprintf(output, "this is type %d. and this is rate %f. i: %d \n", defects[i].type, defects[i].rate,i);
    }

    //DNODE *prev;
	prev = &defects[0];
	DNODE *curr ;
	curr = prev->next;
	int h = 0;
    while(curr!=NULL)
	{
	   // prev = curr;
		curr = curr->next;
		h++;
		printf("contigo peruuuuu %d.\n",h);
	}

   /* for(i=1;i<counter;i++)
    {
        DNODE *prev=&defects[0];
        DNODE *curr = prev->next;
        while(curr!=NULL)
        {
        if (curr->rate ==1)
        {
            prev->next = curr->next;
            free(*curr);
            N--;
        }
        else{}


        }
    }*/
    /*for(i=0;i<N;i++)
    {

            printf("printing %d.\n", i);

    }*/
    return 0;
}

void settype(DNODE *defects)
{   int i;
    int j=0;

    DNODE vacancy[(int)(N/2)];
    DNODE *adress;
    //printf("gets here 1\n");
    for(i=0;i<100;i++)
    {  // printf("gets here 2 %d\n", i);
        if(i%2==0)
        {defects[i].type = 1;
        defects[i].rate = -1;}
        else
        {defects[i].type=0;
        vacancy[j]=defects[i];
        j++;}
    }
    test = 333;
    printf("gets here\n");
    for(i=0;i<N;i++)
    {
        printf("this is type %d. after function\n", defects[i].type);
    }
    printf(" 1 this is test %d.\n", test);
    adress = vacancy;
    testf(adress);
    removedf(defects, 50);
    DNODE temp;
    for(i=0;i<N;i++)
    {  // if(i!=99)
        //temp = *defects[i].next;
        //fprintf(output,"this is type %d. and this is rate %f and this is type of next %d.\n", defects[i].type, defects[i].rate, temp.type);
    }
    fclose(output);
   // printf("This is N %f.\n", N);
}
void testf(DNODE *adress)
{
    int i;
    for(i=0;i<50;i++)
    {
     printf("this is type %d. of vacancy.\n", adress[i].type);
    }
    printf("this is test 2 %d.\n",test);
}

void removedf(DNODE *node, int counter)
{   int i;
    //int j;

    for(i=0;i<counter;i++)
    {
    //j=0;
	DNODE *prev;
	prev = &node[0];
	DNODE *curr ;
	curr = prev->next;
    while(curr!=NULL)
	{
		if(curr->rate==-2.0)
		{   printf("rate was -1. %f\n", N);
            prev->next = curr->next;
            N--;
			//free(curr);;
			break;
			//N--;
		}
		else{
		prev = curr;
		curr = curr->next;
		//printf("contigo peruuuuu.\n");
		}
	}
	free(curr);
    }
   // N=N - counter;
}



