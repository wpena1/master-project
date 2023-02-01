#include<stdio.h>
#include<conio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "mersenTwister.h"
#include "mersenTwister.c"


typedef struct unitcell
{
	int i, j, k;
	int id;
	int state;
	float x,y,z;

}CELL;

typedef struct defnode
{
	float x,y,z;
	int type;
	int id;
	float Rj;
	struct defnode *next;
}DNODE;

void initializeGrid(CELL *grid[10][10][56]);
void evolvecells(DNODE *Dfnode[], CELL *grid[10][10][56]);
void write_traj(FILE *file);
float random_normal();
void initializedefects(DNODE* Dfnode[],CELL* grid[10][10][56]);
void moveinterstitials(DNODE* Dfnode[]);

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
float xyarea;
int Niter=0;
int Nu;
int totalNumofintsl=0;
int totalNumofvcncy=0;
int totalNumofpoint;
int totalNumofrcmtn=0;
int totalNumofsfcmi=0;
int totalNumofsfcmv=0;
unsigned long s;
DNODE* Dfnode[176];
CELL* grid[10][10][56];
int count=0;

int main(int arg, char argc[])
{   Time = 0.0;// DNODE node = malloc(sizeof(node));
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
    printf("this is nu %d\n", Nu);
    //Dfnode=(DNODE *)malloc(sizeof(DNODE));
    xyarea=54;
   // DNODE* Dfnode[Nu/2];
    //CELL* grid[10][10][56];

    s = time(NULL);
    init_genrand(s);
    output = fopen("outputca.txt","w");
    if(output==NULL)
    {
        printf("error: file not opened\n");
        exit(-1);
    }

    initializeGrid(grid);
    initializedefects(Dfnode,grid);
    CELL *node;
    int j,k;
    //int count=0;
    for (i = 0;i<10;i++)
    {
        for (j = 0;j<10;j++)
        {
            for(k = 0 ;k<56;k++)
            {
                node = grid[i][j][k];
               // printf("state a %d \n", node->state);
                //printf(" a %2f, %2f, %2f\t", node->x,node->y,node->z);
                if(node->state==0) count++;
            }
        }
    }
    totalNumofvcncy=count;
    //printf("count is %d\n", count);
    int printout;
    for (i=0;i<Nitermax;i++)
    {
        printout=1000;
        moveinterstitials(Dfnode);

        //printf("count is %d\n", count);
        evolvecells(Dfnode,grid);
    if(i<1000) printout=100;

    if ((int)Time%printout==0)
    {   output=fopen("outputca.txt","a");
            if(!output)
    {
        printf("error: file not opened\n");
        exit(-1);
    }
    write_traj(output);
    }
        Time=Time+1;
    }

}

float random_normal()
{
  return sqrtf(-2*log(genrand_real1())) * cos(2*3.1415925*genrand_real1());
}

void write_traj(FILE *file)
{
    int num=totalNumofintsl+totalNumofvcncy;
    fprintf(file, "%20.4f %10d %10d %10d %10d %10d %10d\n", Time, totalNumofintsl, totalNumofvcncy, totalNumofrcmtn, totalNumofsfcmv, totalNumofsfcmi, num);
    fclose(file);
    // printf("wrote file \n");
}

void moveinterstitials(DNODE* Dfnode[])
{
    DNODE *node;
    int i;
    i=(int)(genrand_real1()*totalNumofintsl);
    //printf("index of defect %d \n", i);
    node = Dfnode[i];
    float x1;
	float x2;
	float xb,yb,zb;
	do{
	x1 = 2*genrand_real1() - 1.0;
	x2 = 2*genrand_real1() - 1.0;} while((x1*x1 + x2*x2)>= 1);
	xb = (2*(x1)*sqrt(1-(x1*x1)-(x2*x2)))*jump;
	yb = (2*(x2)*sqrt(1-(x1*x1)-(x2*x2)))*jump;
	zb = (1-2*((x1*x1)+(x2*x2)))*jump;
    node->x=node->z + xb;
    node->y=node->y + yb;
    node->z=node->z + zb;
    if(node->x > xyarea)
    {
        node->x=node->x-xyarea;
    }
    if(node->x<0.0)
    {
        node->x=xyarea+node->x;
    }
    if(node->y > xyarea)
    {
        node->y=node->y-xyarea;
    }
    if(node->y<0.0)
    {
        node->y=xyarea+node->y;
    }
    if (node->z<=0.0)
    {

            totalNumofsfcmi++;
            totalNumofintsl--;
            Dfnode[i]=Dfnode[totalNumofintsl];

    }
    if (node->z > 304)
    {
        node->z=(node->z-304)+105.0;
    }
}
void initializedefects(DNODE* Dfnode[], CELL* grid[10][10][56])
{
    //DNODE *prev = Dfnode;
   // prev->next=NULL;
   DNODE *curr;
   CELL *currc;
   int i,j,k;
   int v;
   float x,y,z;
   for (v = 0;v<185;v++)
    {
        y=genrand_real1()*xyarea;
        x=genrand_real1()*xyarea;
        z=meanv + stdvv*random_normal();
       // printf("num before %f \n", x);
        i=(int)(x/5.431);
        j=(int)(y/5.431);
        k=(int)(z/5.431);
        //printf("num %d %d %d \n", i,j,k);
        //currc = grid[i][j][k];
        //currc->state=0;
        grid[i][j][k]->state = 0;
        //totalNumofvcncy++;
    }
    for(i=0;i<Nu/2;i++)
    {
        curr = (DNODE *)malloc(sizeof(DNODE));
        curr->id = i;
        curr->Rj = genrand_real1();

            curr->z=meani + stdvi*random_normal();

        totalNumofintsl++;
        curr->x = genrand_real1()*xyarea;
        curr->y = genrand_real1()*xyarea;
        Dfnode[i] = curr;
    }
    fprintf(output,"%20s %10s %10s %10s %10s %10s %8s\n", "Time","Interstls","Vacancies", "Recmbtns", "VcncySfc", "IntlnSfc","defects");
    fclose(output);
    printf("defects initialized \n" );
}
void evolvecells(DNODE *Dfnode[], CELL *grid[10][10][56])
{
    CELL *curr;
    int i,j,k,d;
    float difference;
    for (i = 1;i<10;i++)
    {
        for (j = 1;j<10;j++)
        {
            for(k = 1;k<56;k++)
            {
                curr = grid[i][j][k];
             //printf("inside grid %d, %d, %d, \n", curr->id,curr->state,k);

                if(curr->state==1)//lattice atom
                {//printf("inside grid lattice  %d\n",  curr->state);
                    for( d = 0;d<totalNumofintsl;d++)
                    {

                            if ((curr->x > Dfnode[d]->x && Dfnode[d]->x >= grid[i-1][j][k]->x)&&
                            (curr->y > Dfnode[d]->y && Dfnode[d]->y >= grid[i][j-1][k]->y)&&
                            (curr->z > Dfnode[d]->z && Dfnode[d]->z >= grid[i][j][k-1]->z))
                        {
                             difference = pow(((curr->x) - Dfnode[d]->x), 2) + pow(((curr->y) - Dfnode[d]->y), 2) +
                            pow(((curr->z) - Dfnode[d]->z), 2);
                            if(difference<(jump*jump) && (Dfnode[d]->Rj > 0.9))
                            {
                                curr->state=0;
                                //count++;
                                if(curr->z < 5.431)//vacancies at surface
                                {
                                    totalNumofsfcmv++;
                               }
                               // else {totalNumofsfcmv++;}
                               printf("energy float %f index %d \n", Dfnode[d]->Rj, d);
                               float x1;
	float x2;
	float xb,yb,zb;
	do{
	x1 = 2*genrand_real1() - 1.0;
	x2 = 2*genrand_real1() - 1.0;} while((x1*x1 + x2*x2)>= 1);
	xb = (2*(x1)*sqrt(1-(x1*x1)-(x2*x2)))*jump;
	yb = (2*(x2)*sqrt(1-(x1*x1)-(x2*x2)))*jump;
	zb = (1-2*((x1*x1)+(x2*x2)))*jump;
	Dfnode[d]->x=Dfnode[d]->x+xb;
	Dfnode[d]->y=Dfnode[d]->y+yb;
	Dfnode[d]->z=Dfnode[d]->z+zb;

                                d=totalNumofintsl;
                            }
                       }

                    }
                }

                else if (curr->state==0)//vacancy
                {   //printf("inside grid vacancy \n");
                    for( d = 0;d<totalNumofintsl;d++)
                    {
                        if ((curr->x > Dfnode[d]->x && Dfnode[d]->x >= grid[i-1][j][k]->x)&&
                            (curr->y > Dfnode[d]->y && Dfnode[d]->y >= grid[i][j-1][k]->y)&&
                            (curr->z > Dfnode[d]->z && Dfnode[d]->z >= grid[i][j][k-1]->z))
                        {
                            difference = pow(((curr->x) - Dfnode[d]->x), 2) + pow(((curr->y) - Dfnode[d]->y), 2) +
                            pow(((curr->z) - Dfnode[d]->z), 2);
                            if(difference<16.0)
                            {
                                curr->state=2;
                                totalNumofrcmtn++;
                                totalNumofintsl--;
                                totalNumofvcncy--;
                                Dfnode[d]=Dfnode[totalNumofintsl];
                                d=totalNumofintsl;
                                printf("REcombination\n");
                                //count--;
                            }
                        }
                       // printf("looking for REcombination\n");
                    }
                }

            }
        }
    }
    //printf("evolved cells \n");
}
void initializeGrid(CELL* grid[10][10][56])
{

    int i, j, k;
    //CELL* grid[3][3][4];
    float x = 5.431;
    float y = 5.431;
    float z = 0;
    int id = 1;
    CELL *currcell;

    for (i = 0;i<10;i++)
    {
        for (j = 0;j<10;j++)
        {
            for(k = 0 ;k<56;k++)
            {

                currcell = (CELL *)malloc(sizeof(CELL));
                currcell->i=i;
                currcell->j=j;
                currcell->k=k;
                currcell->x=x;
                currcell->y=y;
                currcell->z=z+5.431;
                currcell->id = id;
                currcell->state=1;
                grid[i][j][k]= currcell;
                z=z+5.431;
                id++;
            }
            z=0;
            y=y+5.431;

        }
        y=5.431;
        x=x+5.431;
    }
    printf("initialized grid \n");
   /* CELL *node;
    for (i = 0;i<3;i++)
    {
        for (j = 0;j<3;j++)
        {
            for(k = 0 ;k<3;k++)
            {
                node = grid[i][j][k];
                printf("%2f, %2f, %2f\t", node->x,node->y,node->z);
            }
            printf("\n");
        }
        printf("\n");
    }*/

}

