#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "mersenTwister.h"
#include "mersenTwister.c"

//
//
typedef struct defnode
{
	float x;//,y,z;
	int type;
	int id;
	struct defnode *next;
	struct defnode *before;
}DNODE;

typedef struct unitcell
{
	int i, j, k;
	int id;
	struct unitcell *top;
	struct unitcell *bottom;
	struct unitcell *right;
	struct unitcell *left;
}CELL;

void initializedefects(DNODE *Dfnode);
void evolvecells(DNODE *Dfnode);
void write_traj(FILE *file, DNODE *node);
float random_normal();

float Nitermax;
FILE *data;
FILE *output;
float Time;
float Emv;
float Emi;
float Kb;
float T;
float wi;
float wv;
float recr;
float jump;
float defectnumber;
float meani;
float meanv;
float stdvi;
float stdvv;
DNODE *Dfnode;
//RNODE *Rnode;
int Niter=0;
int Nu;
int totalNumofintsl;
int totalNumofvcncy;
int totalNumofpoint;
int totalNumofrcmtn=0;
int totalNumofsfcmi=0;
int totalNumofsfcmv=0;
unsigned long s;

int main(int arg, char argc[])
{
    Time = 0.0;// DNODE node = malloc(sizeof(node));
    int counter = 0;
    char line[14];
    double values[14];
    int i;
    data = fopen("data.txt","r");
    if(!data)
    {
        printf("error: file not opened\n");
        exit(-1);
    }

    while(fgets(line, sizeof(line), data)!=NULL)
    {

        values[counter]=atof(line);
        counter++;
    }
    fclose(data);
    int counterb;
    defectnumber = values[0];
    meanv = values[1];
    meani = values[2];
    stdvi = values[3];
    stdvv = values[4];
    T = values[5];
    wi = values[6];
    Emi = values[7];
    wv = values[8];
    Emv = values[9];
    Kb = 8.6173324E-5;
    //printf("kb %20.15f. \n", Kb);
    jump = values[11];
    recr = values[12];
    Nitermax = values[13];
    Nu=(int)(defectnumber);
    //printf("this is nu %d\n", Nu);
    Dfnode=(DNODE *)malloc(sizeof(DNODE));
    s = time(NULL);
    init_genrand(s);
    output = fopen("outputca.txt","w");
    if(output==NULL)
    {
        printf("error: file not opened\n");
        exit(-1);
    }
    initializedefects(Dfnode);

    for(i = 0;i<(int)Nitermax; i++)
    {
    if((totalNumofintsl > 0) && (totalNumofvcncy>0))l,
   {

   evolvecells(Dfnode);
    output=fopen("outputca.txt","a");
    if(!output)
    {
        printf("error: file not opened\n");
        exit(-1);
    }
    write_traj(output, Dfnode);
    printf("in loop. \n");

    }}
    return 0;
}
void initializedefects(DNODE *Dfnode)
{
    int i;
    float j=0;
    //int k=0;
    DNODE *prev = Dfnode;
    prev->next=NULL;
    prev->before = NULL;
    DNODE *curr;
    for(i=0;i<Nu;i++)
    {
        curr = (DNODE *)malloc(sizeof(DNODE));
        curr->id = i;
        curr->type = genrand_int31()%3;
       if(curr->type == 0)//vacancy
        {   if (i==0){
            curr->x = 0;
            j=curr->x;}
            else {curr->x=j+5.431;
                j=curr->x;
                }
                totalNumofvcncy++;
        }
        else if(curr->type == 1)//latticepoint
        {
            if (i==0){
            curr->x = 0;
            j=curr->x;}
            else {curr->x=j+5.431;
                j=curr->x;
                }
                totalNumofpoint++;

        }
        else if (curr->type == 2)//interstitial
        {
            curr->x = meani + stdvi*random_normal();
            totalNumofintsl++;
        }
        curr->next=NULL;
        prev->next=curr;
        curr->before = prev;
        prev=prev->next;
        fprintf(output,"%20s %10s %10s %10s %10s %10s %8s\n", "Time","Interstls","Vacancies", "Recmbtns", "VcncySfc", "IntlnSfc","defects");
        fclose(output);
    }
}
void evolvecells(DNODE *Dfnode)
{   DNODE *prev,*curr,*next,*before;
	prev=Dfnode;
	curr=prev->next;
	next = curr->next;
	before = prev;
	int i=0;
	printf("outside. \n");
    while(curr!=NULL)
	{
	    before = curr->before;
        next = curr->next;
        if (i > 0)
	  {

	    if (curr->type == 0)
        {

            if ((before->type == 2) && (abs(curr->x - before->x) <= recr))
               {curr->type = 1;
               totalNumofvcncy--;
                totalNumofintsl--;
                totalNumofrcmtn++;
                }
            else if ((next->type == 2)&& (abs(curr->x - next->x) <= recr))
            {
                curr->type = 1;
                totalNumofvcncy--;
                totalNumofintsl--;
            }
            else if(before->type == 1 && next->type == 0)
            {
                //move with probability
                    before->type = 0;
                    curr->type = 1;
                    if (before->x <= 0)
                       {totalNumofvcncy--;
                       totalNumofsfcmv++;}
            }
            else if(next->type == 1 && before->type == 0)
            {
                next->type = 0;
                curr->type = 1;
                 if(next->x <= 0)
                    {totalNumofvcncy--;
                    totalNumofsfcmv++;}
            }
            else if (next->type == 1 && before->type == 1)
            {
                int choose;
                choose = genrand_int31()%2;
                if(choose == 0)
                    { before->type = 0;
                    curr->type = 1;
                    if (before->x <= 0)
                       {totalNumofvcncy--;
                        totalNumofsfcmv++;} }//move to the left with probability
                else {//move to the right with probability
                     next->type = 0;
                    curr->type = 1;
                    if(next->x <= 0)
                    {totalNumofvcncy--;
                    totalNumofsfcmv++;
                        }
                    }
            }
        }
      /*  else if (curr->type == 1)
        {
            //move with probability consider rate of vacancy moving
            if (next->type == 1 && before->type == 0)
                //move to the left with probability

            else if (next->type == 0 && before->type == 1)
                //move to the right with probability
        }*/
        else if (curr->type == 2)
        {   //move with probability
            curr->x = curr->x + (2*jump*genrand_real1()-jump);
            if (curr->x <=0)
               {totalNumofintsl--;
               totalNumofsfcmi++;
               }

        }
	  }
        curr = curr->next;
        i=1;
	}
    printf("outside loop. \n");
}
float random_normal()
{
  return sqrtf(-2*log(genrand_real1())) * cos(2*3.1415925*genrand_real1());
}
void write_traj(FILE *file, DNODE *node)
{
    int num=totalNumofintsl+totalNumofvcncy;
    fprintf(file, "%20.4f %10d %10d %10d %10d %10d %10d\n", Time, totalNumofintsl, totalNumofvcncy, totalNumofrcmtn, totalNumofsfcmv, totalNumofsfcmi, num);
    fclose(file);
}
void intitializegrid(CELL Grid[][][])
{
    CELL *currcell;
    currcell = (CELL *)malloc(sizeof(CELL));

    for(i=0;i<xyarea/5;i++)
    {
        for(j=0;j<xyarea/5;j++)
        {
            for(k=0;k<300/5;k++)
            {
                currcell->i=i;
                currcell->j=j;
                currcell->k=k;
                Grid[i][j][k]=currcell;
            }
        }
    }
}
