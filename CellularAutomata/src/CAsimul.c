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

void initializeGrid(CELL *grid[25][25][56]);
void evolvecells(DNODE *Dfnode[], CELL *grid[25][25][56]);
void write_traj(FILE *file);
float random_normal();
void initializedefects(DNODE* Dfnode[],CELL* grid[25][25][56]);
void moveinterstitials(DNODE* Dfnode[]);
void moveVacancies(CELL* vacancies[176]);

float Nitermax;
FILE *data;
FILE *output;
long Time;
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
CELL* grid[25][25][56];
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
    xyarea=115.0;
    Time = 0;
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
    CELL *vacancies[176];
    int j,k;
    //int count=0;
    for (i = 0;i<25;i++)
    {
        for (j = 0;j<25;j++)
        {
            for(k = 0 ;k<56;k++)
            {
                node = grid[i][j][k];
               // printf("state a %d \n", node->state);
                //printf(" a %2f, %2f, %2f\t", node->x,node->y,node->z);
                if(node->state==0 && count<176){ count++;
                vacancies[count-1] = node;}
            }
        }
    }
    totalNumofvcncy=count;
    //printf("count is %d\n", count);
    int printout=1;
    float fi = wi*expf(-(Emi/(Kb*T)));
    float fv = wv*expf(-(Emv/(Kb*T)));
    float f = fi+fv;
    float pick;
    printf("this is interstitial rate %20.15f \n", fi);
    printf("this is vacancy rate %20.15f \n", fv);
    for (i=0;i<Nitermax;i++)
    {
      // if(totalNumofvcncy==0){f=fi;}
    pick=genrand_real1()*f;
        if (pick>fi && pick<=f)
       {
    moveVacancies(vacancies);

       }
       else if (fi>=pick)
       {printf("this is ipick rate %20.15f \n", pick);
        moveinterstitials(Dfnode);}
        //printf("count is %d\n", count);
        evolvecells(Dfnode,grid);
    if (Time<100)
    {
        printout = 1;
    }
    else if (Time<1000)
    {
        printout = 10;
    }
    else if (Time<1000000)
    {
       printout = 100;
    }
    else if (Time<10000000)
    {
        printout = 1000;
    }
    else printout = 10000;

    if (Time%printout==0)
    {   output=fopen("outputca.txt","a");
            if(!output)
    {
        printf("error: file not opened\n");
        exit(-1);
    }
    write_traj(output);
    }
    Time=Time+1;
        //s = time(NULL);
        //init_genrand(s);
    }
    return 0;
}

float random_normal()
{
  return sqrtf(-2*log(genrand_real1())) * cos(2*3.1415925*genrand_real1());
}

void write_traj(FILE *file)
{
    int num=totalNumofintsl+totalNumofvcncy;
    fprintf(file, "%20d %10d %10d %10d %10d %10d %10d\n", Time, totalNumofintsl, totalNumofvcncy, totalNumofrcmtn, totalNumofsfcmv, totalNumofsfcmi, num);
    fclose(file);
    // printf("wrote file \n");
}

void moveinterstitials(DNODE* Dfnode[])
{   if(totalNumofintsl>0)
    {DNODE *node;
    int i;
    i=genrand_int31()%totalNumofintsl;
   // for(i=0;i<totalNumofintsl;i++)
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
            Dfnode[i]=Dfnode[totalNumofintsl-1];
            totalNumofintsl--;


    }
   // if (node->z > 300)
   // {
     //   node->z=(node->z-300)+105.0;
    //}

}
}
void moveVacancies(CELL* vacancies[])
{
    CELL *curr, *node;
    int i,j,k,d,f,c,countb ;
    float x,y,z;
    f=0;
    c=0;
    countb=0;
    if (totalNumofvcncy>0){
    for (i = 0;i<25;i++)
    {
        for (j = 0;j<25;j++)
        {
            for(k = 0 ;k<56;k++)
            {
                node = grid[i][j][k];
               // printf("state a %d \n", node->state);
                //printf(" a %2f, %2f, %2f\t", node->x,node->y,node->z);
                if(node->state==0){
                vacancies[countb] = node;
                countb++;}
            }
        }
    }
   // for(i = 0 ; i<countb; i++)
    //{
    /*    if(vacancies[i]->z<=5.431 && vacancies[i]->state==0)
    {
        totalNumofsfcmv++;
        printf("inside surface \n");
        vacancies[i]->state=1;
        vacancies[i]=vacancies[count-1];
        count--;
        totalNumofvcncy--;
        //f=1;
    }
    else */
   //     if (vacancies[i]->state==2)
   // {
    //    vacancies[i]=vacancies[countb-1];
       // printf("removing nonvacancie\n");
    //}
    //}
   // printf("this is count %d \n", count);
    //for (f=0;f<count;f++)
    //{
    d=genrand_int31()%countb;
    curr = vacancies[d];
   // printf("%d \n", d);
   // i=curr->i;
    //j=curr->j;
   // k=curr->k;
    x=curr->x;
    y=curr->y;
    z=curr->z;

    float x1,x2;
	float xb,yb,zb;
	do{
	x1 = 2*genrand_real1() - 1.0;
	x2 = 2*genrand_real1() - 1.0;} while((x1*x1 + x2*x2)>= 1);
	xb = (2*(x1)*sqrt(1-(x1*x1)-(x2*x2)))*jump;
	yb = (2*(x2)*sqrt(1-(x1*x1)-(x2*x2)))*jump;
	zb = (1-2*((x1*x1)+(x2*x2)))*jump;
	curr->x=x+xb;
	curr->y=y+yb;
	curr->z=z+zb;
	if(curr->x<0){curr->x=xyarea+curr->x;}
	else if(curr->x>xyarea){curr->x=curr->x-xyarea;}
	if(curr->y<0){curr->y=xyarea+curr->y;}
	else if(curr->y>xyarea){curr->y=curr->y-xyarea;}
	if(curr->z<0){curr->state=2;
                vacancies[d]=vacancies[countb-1];
                //count--;
                totalNumofvcncy--;
                totalNumofsfcmv++;}
   // else if(curr->z>300){curr->z = meanv+stdvv*random_normal();}
  if(curr->state==0)
                    {i = (int)floorf(curr->x/5.431);
                    j = (int)floorf(curr->y/5.431);
                    k = (int)floorf(curr->z/5.431);
                    c = grid[i][j][k]->state;
                    if (c==1)
                    {grid[i][j][k]->state=curr->state;
                    grid[i][j][k]->x=x;
                    grid[i][j][k]->y=y;
                    grid[i][j][k]->z=z;
                    curr->state=c;}}
                    /* int c = 0;//genrand_int31()%3;
                      // printf("move vacancy outside if %d %d %d %d %d \n", curr->id, f,i,j,k);
                        int n;
                        float a = 2*genrand_real1()-1;
                        if (a>0){n=(int)ceilf(a);}
                        else {n=(int)floorf(a);}
                       if(n<0)
                        {
                            if(i>0 && j>0 && k>0)
                       {
                     //  printf("move vacancy %d %d %d %d %d \n", curr->id, f,i,j,k);
                             if (c==0)
                            {if (grid[i][j][k+n]->state!=0)
                            {c=grid[i][j][k+n]->state;
                            grid[i][j][k+n]->state=curr->state;
                            curr->state=c;
                            vacancies[f]=grid[i][j][k+n];
                            f=176;
                            }
                            else if(grid[i][j+n][k]->state!=0)
                            {c=grid[i][j+n][k]->state;
                            grid[i][j+n][k]->state=curr->state;
                            curr->state=c;
                            vacancies[f]=grid[i][j+n][k];
                            f=176;
                            }
                             else if(grid[i+n][j][k]->state!=0)
                            {c=grid[i+n][j][k]->state;
                            grid[i+n][j][k]->state=curr->state;
                            curr->state=c;
                            vacancies[f]=grid[i+n][j][k];
                            f=176;
                            }
                            }
                            if (c==1)
                            {//else
                                 if (grid[i][j+n][k]->state!=0)
                            {c=grid[i][j+n][k]->state;
                            grid[i][j+n][k]->state=curr->state;
                            curr->state=c;
                            vacancies[f]=grid[i][j+n][k];
                            f=176;
                            }
                            else if (grid[i+n][j][k]->state!=0)
                            {c=grid[i+n][j][k]->state;
                            grid[i+n][j][k]->state=curr->state;
                            curr->state=c;
                            vacancies[f]=grid[i+n][j][k];
                            f=176;
                            }
                            else if (grid[i][j][k+n]->state!=0)
                            {c=grid[i][j][k+n]->state;
                            grid[i][j][k+n]->state=curr->state;
                            curr->state=c;
                            vacancies[f]=grid[i][j][k+n];
                            f=176;
                            }
                            }
                            if (c==2)
                            //else
                            {if  (grid[i+n][j][k]->state!=0)
                            {c=grid[i+n][j][k]->state;
                            grid[i+n][j][k]->state=curr->state;
                            curr->state=c;
                            vacancies[f]=grid[i+n][j][k];
                            f=176;
                            }
                            else if(grid[i][j+n][k]->state!=0)
                            {c=grid[i][j+n][k]->state;
                            grid[i][j+n][k]->state=curr->state;
                            curr->state=c;
                            vacancies[f]=grid[i][j+n][k];
                            f=176;
                            }
                            else if (grid[i][j][k+n]->state!=0)
                            {c=grid[i][j][k+n]->state;
                            grid[i][j][k+n]->state=curr->state;
                            curr->state=c;
                            vacancies[f]=grid[i][j][k+n];
                            f=176;
                            }}
                            }
                        }
                        else if (n>0)
                        {if(i<9 && j<9 && k<55)
                       {
                      // printf("move vacancy %d %d %d %d %d \n", curr->id, f,i,j,k);
                            if (c==0)
                            {if (grid[i][j][k+n]->state!=0)
                            {c=grid[i][j][k+n]->state;
                            grid[i][j][k+n]->state=curr->state;
                            curr->state=c;
                            vacancies[f]=grid[i][j][k+n];
                            f=176;
                            }
                            else if(grid[i][j+n][k]->state!=0)
                            {c=grid[i][j+n][k]->state;
                            grid[i][j+n][k]->state=curr->state;
                            curr->state=c;
                            vacancies[f]=grid[i][j+n][k];
                            f=176;
                            }
                             else if(grid[i+n][j][k]->state!=0)
                            {c=grid[i+n][j][k]->state;
                            grid[i+n][j][k]->state=curr->state;
                            curr->state=c;
                            vacancies[f]=grid[i+n][j][k];
                            f=176;
                            }
                            }
                            if (c==1)
                            {//else
                                 if (grid[i][j+n][k]->state!=0)
                            {c=grid[i][j+n][k]->state;
                            grid[i][j+n][k]->state=curr->state;
                            curr->state=c;
                            vacancies[f]=grid[i][j+n][k];
                            f=176;
                            }
                            else if (grid[i+n][j][k]->state!=0)
                            {c=grid[i+n][j][k]->state;
                            grid[i+n][j][k]->state=curr->state;
                            curr->state=c;
                            vacancies[f]=grid[i+n][j][k];
                            f=176;
                            }
                            else if (grid[i][j][k+n]->state!=0)
                            {c=grid[i][j][k+n]->state;
                            grid[i][j][k+n]->state=curr->state;
                            curr->state=c;
                            vacancies[f]=grid[i][j][k+n];
                            f=176;
                            }
                            }
                            if (c==2)
                            //else
                            {if  (grid[i+n][j][k]->state!=0)
                            {c=grid[i+n][j][k]->state;
                            grid[i+n][j][k]->state=curr->state;
                            curr->state=c;
                            vacancies[f]=grid[i+n][j][k];
                            f=176;
                            }
                            else if(grid[i][j+n][k]->state!=0)
                            {c=grid[i][j+n][k]->state;
                            grid[i][j+n][k]->state=curr->state;
                            curr->state=c;
                            vacancies[f]=grid[i][j+n][k];
                            f=176;
                            }
                            else if (grid[i][j][k+n]->state!=0)
                            {c=grid[i][j][k+n]->state;
                            grid[i][j][k+n]->state=curr->state;
                            curr->state=c;
                            vacancies[f]=grid[i][j][k+n];
                            f=176;
                            }}
                            }
                        } */

                    //printf("in loop \n");
                //}
    }
}
void initializedefects(DNODE* Dfnode[], CELL* grid[25][25][56])
{
    //DNODE *prev = Dfnode;
   // prev->next=NULL;
   DNODE *curr;
   CELL *currc;
   int i,j,k;
   int v,n;
   n=0;
   v=0;
   float x,y,z;
   while (v<176)
    {
        y=genrand_real1()*50.0;
        x=genrand_real1()*50.0;
        z=meanv + stdvv*random_normal();

     //  printf("num before %f \n", z);
        i=(int)floorf(x/5.431);
        j=(int)floorf(y/5.431);
        k=(int)floorf(z/5.431);
        //printf("num %d %d %d %d\n", i,j,k, n);
       // if(k>14 && k<25){n++;}
        //currc = grid[i][j][k];
        //currc->state=0;
        if(grid[i][j][k]->state!=0)
       {grid[i][j][k]->state = 0;
        grid[i][j][k]->x=x;
        grid[i][j][k]->y=y;
        grid[i][j][k]->z=z;
        v++;
        n++;}
        //totalNumofvcncy++;
    }
    for(i=0;i<Nu/2;i++)
    {
        curr = (DNODE *)malloc(sizeof(DNODE));
        curr->id = i;
        curr->type = 1;
        curr->Rj = genrand_real1();

            curr->z=meani + stdvi*random_normal();

        totalNumofintsl++;
        curr->x = genrand_real1()*50;
        curr->y = genrand_real1()*50;
        Dfnode[i] = curr;
    }
    fprintf(output,"%20s %10s %10s %10s %10s %10s %8s\n", "Time","Interstls","Vacancies", "Recmbtns", "VcncySfc", "IntlnSfc","defects");
    fclose(output);
    printf("defects initialized \n" );
}
void evolvecells(DNODE *Dfnode[], CELL *grid[25][25][56])
{
    CELL *curr;
    int i,j,k,d;
    float difference;
    int f=0;
    for (i = 1;i<24;i++)
    {
        for (j = 1;j<24;j++)
        {
            for(k = 1;k<55;k++)
            {
                curr = grid[i][j][k];
             //printf("inside grid %d, %d, %d, \n", curr->id,curr->state,k);

                /*if(curr->state==1)//lattice atom
                {//printf("inside grid lattice  %d\n",  curr->state);
                    for( d = 0;d<totalNumofintsl;d++)
                    {

                            if ((curr->x > Dfnode[d]->x && Dfnode[d]->x >= grid[i-1][j][k]->x)&&
                            (curr->y > Dfnode[d]->y && Dfnode[d]->y >= grid[i][j-1][k]->y)&&
                            (curr->z > Dfnode[d]->z && Dfnode[d]->z >= grid[i][j][k-1]->z))
                        {
                             difference = pow(((curr->x) - Dfnode[d]->x), 2) + pow(((curr->y) - Dfnode[d]->y), 2) +
                            pow(((curr->z) - Dfnode[d]->z), 2);
                            if(difference<(jump*jump) && (Dfnode[d]->Rj > 0.6))
                            {
                                curr->state=0;
                                totalNumofvcncy++;
                                //count++;
                                if(curr->z < 5.431)//vacancies at surface
                                {
                                    totalNumofsfcmv++;
                                    totalNumofvcncy--;
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

                else */if (curr->state==0)//vacancy
                {   //printf("inside grid vacancy \n");
                    //if (curr->z<5.431){curr->state=2;
                   // totalNumofsfcmv++;
                    //totalNumofvcncy--;}
                   // int cx, cy, cz;
                   // cy = 0;
                   // cz = 0;
                    for( d = 0;d<totalNumofintsl;d++)
                    {
                        //if( grid[i-1][j][k]->x<Dfnode[d]->x && Dfnode[d]->x <= grid[i+1][j][k]->x) cx = 1;
                        //if( grid[i][j-1][k]->y<Dfnode[d]->y && Dfnode[d]->y <= grid[i][j+1][k]->y) cy = 1;
                        //if( grid[i][j][k-1]->z<Dfnode[d]->z && Dfnode[d]->z <= grid[i][j][k+1]->z) cz = 1;
                  if (( grid[i-1][j][k]->x<Dfnode[d]->x && Dfnode[d]->x <= grid[i+1][j][k]->x) &&
                       ( grid[i][j-1][k]->y<Dfnode[d]->y && Dfnode[d]->y <= grid[i][j+1][k]->y) &&
                      ( grid[i][j][k-1]->z<Dfnode[d]->z && Dfnode[d]->z <= grid[i][j][k+1]->z))
                        {
                            difference = pow(((curr->x) - Dfnode[d]->x), 2) + pow(((curr->y) - Dfnode[d]->y), 2) +
                            pow(((curr->z) - Dfnode[d]->z), 2);
                            if(difference<16.0)
                            {
                                curr->state=2;
                                //f=1;
                                Dfnode[d]->type=2;
                                Dfnode[d]=Dfnode[totalNumofintsl-1];
                                d=totalNumofintsl;
                                totalNumofrcmtn++;
                                totalNumofintsl--;
                                totalNumofvcncy--;
                                printf("REcombination %d\n ", Time);
                                //count--;
                            }
                      }

                       // printf("looking for REcombination\n");
                    }

                    }
                }

            }
        }
    }
    //printf("evolved cells \n");
//}
void initializeGrid(CELL* grid[25][25][56])
{

    int i, j, k;
    //CELL* grid[3][3][4];
    float x = 5.431;
    float y = 5.431;
    float z = 0;
    int id = 1;
    CELL *currcell;

    for (i = 0;i<25;i++)
    {
        for (j = 0;j<25;j++)
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

