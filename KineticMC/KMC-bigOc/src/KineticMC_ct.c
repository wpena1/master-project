#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "mersenTwister.h"
#include "mersenTwister.c"

//Constant time algorithm
//Temperature is held constant.
//

//Defect node
typedef struct defnode
{
	float x,y,z;
	float rate;
	float Rj;
	int type;
	int id;
	struct defnode *next;
}DNODE;
//Methods
void initializedefects(DNODE *Dfnode);
int check_rec(DNODE *Dfnode);
float update_cf(DNODE *Dfnode);
void coe_jump(DNODE *defects, float rn);
void update_time(float rn);
void write_traj(FILE *file, DNODE *node);
void removedf(DNODE *node, int counter);
void setRate(DNODE *Dfnode);
float random_gaussian();
//Global variables
float xyarea;
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
int Niter=0;
int Nu;
int totalNumofintsl;
int totalNumofvcncy;
int totalNumofrcmtn=0;
int totalNumofsfcmi=0;
int totalNumofsfcmv=0;
float Lnk[2];
int numberOfRates;
float Rmaxv,Rmaxi;
unsigned long s;
int write = 0;

int main(int arg, char argc[])
{   //set time to 0
    Time = 0.0;
    int counter = 0;
    char line[14];
    double values[14];
    int i;
    data = fopen("data.txt","r");//open and read file
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
    defectnumber = values[0];//number of defects
    meanv = values[1];//vacancy mean z position in gaussian dist.
    meani = values[2];//interstitial mean z position in gaussian dist.
    stdvi = values[3];// interstitial standard deviation
    stdvv = values[4];// vacancy standard deviation
    T = values[5];//Temperature
    wi = values[6];//interstial migration prefactor
    Emi = values[7];//Energy barrier for interstitial
    wv = values[8];// vacancy migration prefactor
    Emv = values[9];//Energy barrier for vacancy
    Kb = 8.6173324E-5; // boltzman constant
    jump = values[11];// jump distance
    recr = values[12];// recombination radius
    Nitermax = values[13];// number of iterations
    numberOfRates = values[10];
    xyarea=50.0; //x y length
    recr=recr*recr;
    Nu=(int)(defectnumber);
    printf("Total number of defects: %d\n", Nu);
    Dfnode=(DNODE *)malloc(sizeof(DNODE));//initialize head node
    s = time(NULL);
    init_genrand(s);
    output = fopen("output2.txt","w");
    if(output==NULL)
    {
        printf("error: file not opened\n");
        exit(-1);
    }

    initializedefects(Dfnode);
   // printf("outside intitialize\n");
    //setRate(Dfnode);
    //Main loop
    float rn;
    clock_t begin, end;
    double time_spent;

    //Main loop
    begin = clock();
    for(i = 0;i<(int)Nitermax; i++)
    {
       setRate(Dfnode);
       rn = update_cf(Dfnode);
        coe_jump(Dfnode,rn);
        counterb = check_rec(Dfnode);
        removedf(Dfnode, counterb);
        update_time(rn);
        Niter++;
    }
    end = clock();
    time_spent = (double)(end-begin)/CLOCKS_PER_SEC;
    printf("Done %f\n", time_spent);
    return 0;
}

void initializedefects(DNODE *Dfnode)
{
    int i;
    int j=0;
    int k=0;
   // Lnk = (float *)malloc(sizeof(float)*numberOfRates);

    DNODE *prev = Dfnode;
    prev->next=NULL;
    DNODE *curr;
    for(i=0;i<Nu;i++)
    {
        curr = (DNODE *)malloc(sizeof(DNODE));
        curr->id = i;
        curr->type = i%2;
       if(i%2==0)
        {
            curr->z=meanv + stdvv*random_gaussian();
        }
        else
        {
            curr->z=meani + stdvi*random_gaussian();

        }

        curr->x = genrand_real1()*xyarea;
        curr->y = genrand_real1()*xyarea;
        curr->next=NULL;
        prev->next=curr;
        prev=prev->next;
    }
    int counter;
    totalNumofvcncy = Nu/2;
    printf("total number of vacancies %d \n", totalNumofvcncy);
    totalNumofintsl = Nu/2;
    printf("total number of interstitial %d \n", totalNumofintsl);
    fprintf(output,"%20s %10s %10s %10s %10s %10s %8s\n", "Time","Interstls","Vacancies", "Recmbtns", "VcncySfc", "IntlnSfc","defects");
    fclose(output);
    counter = check_rec(Dfnode);
    removedf(Dfnode, counter);
}

void write_traj(FILE *file, DNODE *node)
{
    int num=totalNumofintsl+totalNumofvcncy;
    fprintf(file, "%20.4f %10d %10d %10d %10d %10d %10d\n", Time, totalNumofintsl, totalNumofvcncy, totalNumofrcmtn, totalNumofsfcmv, totalNumofsfcmi, num);
    fclose(file);
}

float random_gaussian()
{
  return sqrtf(-2*log(genrand_real1())) * cos(2*3.1415925*genrand_real1());
}
//Check for recombination
int check_rec(DNODE *Dfnode)
{
    if(totalNumofintsl!=0 && totalNumofvcncy!=0)
    {
        DNODE *iptr[totalNumofintsl];//array of pointer nodes
        DNODE *vptr[totalNumofvcncy];
        DNODE *prev,*curr;
        prev=Dfnode;
        curr=prev->next;
        int i=0;
        int j=0;
        while(curr!=NULL)
        {
            if(curr->type==1)
            {iptr[i] = curr;
            i++;}
            if(curr->type==0)
            {vptr[j] = curr;
            j++;}
		curr = curr->next;
        }
    //defects are marked for removal by changing their id's
    j = 0;
    i = 0;
    float difference;
    int counter=0;
	while(j<totalNumofintsl)
	{
	    while(i<totalNumofvcncy)
		{
		    if (vptr[i]->id!=-1 && iptr[j]->id!=-1)
			{
			    difference = pow((iptr[j]->x - vptr[i]->x), 2) + pow((iptr[j]->y - vptr[i]->y), 2) +
                pow((iptr[j]->z - vptr[i]->z), 2);
                if(difference<(recr))
                {
                    iptr[j]->id=-1;
                    vptr[i]->id=-1;
                    counter=counter+2;
                    totalNumofrcmtn++;
                    i=0;
                    j++;
                }
                else
                    {i++;}
			}
			else
            {i++;}
		}
		j++;
	}
	return counter;}
	else
    {
        return 0;
    }

}
//remove marked defects
void removedf(DNODE *node, int counter)
{   if(Nu != 0)
    {int i;
    for(i=0;i<counter;i++)
    {
        DNODE *prev;
        prev = node;
        DNODE *curr ;
        curr = prev->next;
        while(curr!=NULL)
        {
            if(curr->id==-1)
            {
                if(curr->type==1)
                {totalNumofintsl--;}
                if(curr->type==0)
                {totalNumofvcncy--;}
                prev->next = curr->next;
                Nu--;
                free(curr);
                curr=NULL;
            }
            else
            {
                prev = curr;
                curr = curr->next;
            }
        }
    }

    }
}

void setRate(DNODE *Dfnode)
{   int i;
    DNODE *prev;
	prev = Dfnode;
	DNODE *curr ;
	curr = prev->next;
    while(curr!=NULL)
	{
		if(curr->type==1)
		{
		    curr->rate=wi*expf(-(Emi/(Kb*T)));
            curr = curr->next;
        }
		else
        {
            curr->rate=wv*expf(-(Emv/(Kb*T)));
            curr = curr->next;
        }
	}

           //Lnk[0]=wi*expf(-(Emi/(Kb*T)));
            //Lnk[1]=wv*expf(-(Emv/(Kb*T)));


}

float update_cf(DNODE *Dfnode)
{   //gets cummulative rate
    if(Nu!=0)
    {

        int j=0;
        float sum = 0;
        float sumb = 0;
        float sumc = 0;
        float temp;
        DNODE *prev;
        prev = Dfnode;
        DNODE *curr ;
        curr = prev->next;

        while(curr!=NULL)
        {   if(curr->type==0)
            {temp = curr->rate;
            curr->Rj=sum+temp;
            sum = sum + temp;}
            if(curr->type == 1)
            {
                temp = curr->rate;
                curr->Rj = sumb+temp;
                sumb = sumb + temp;
            }
            curr=curr->next;
        }
        Rmaxv = sum;
        Rmaxi = sumb;
        Lnk[0]=Rmaxi;
        Lnk[1]=Rmaxv;
        int i;
        if(totalNumofvcncy==0)
            {
                Lnk[1]=0;
            }

        for (i=0;i<numberOfRates;i++)
        {
            temp = Lnk[i];
            Lnk[i]= sumc+temp;
            sumc = sumc+temp;
        }
        if(totalNumofvcncy==0)
            {
                Lnk[1]=0;
            }
      //  printf("sum %f \n", sumc);
        return sumc;
  }
  else
  {
    return 0;
  }
}
// carry out event
void coe_jump(DNODE *defects, float RN)
{
    if(Nu != 0)
    {
        double u = genrand_real1();
        float uR = u*RN;
        DNODE *prev,*curr;
        DNODE *Adress[numberOfRates];
    int counter=0;
	prev = defects;
	DNODE *vptr[totalNumofvcncy];
	DNODE *iptr[totalNumofintsl];
	curr=prev->next;
    int i=0;
    int j =0;
    while(curr!=NULL)
        {
            if(curr->type==1)
            {iptr[i] = curr;
            i++;}
            if(curr->type==0)
            {vptr[j] = curr;
            j++;}
		curr = curr->next;
        }
    i=0;
        int found = -1;
        for(i=0;i<numberOfRates;i++)
        {

            if(i!=0)
            {
                if( Lnk[i]>=uR && Lnk[i-1]<uR)
                    found = i;
            }
            else
            {
                if(Lnk[i]>=uR)
                    found = i;
            }

        }
    //move defects

    float R;
    DNODE *temp;
    int selected = 0;
    int randA;
    double randB;

    if(found==1)
    {
        while(selected == 0)
       {randA = genrand_int31()%(totalNumofvcncy);
  //  printf("randA %d \n",randA );
       randB = genrand_real1();
       R = randB*Rmaxv;
       if (totalNumofvcncy<4)
        {
        temp=vptr[randA];
        selected = 1;

        }
       else if(vptr[randA]->Rj<=R)
       {
        selected = 1;
        temp = vptr[randA];
       }

      }

    }
    else if(found == 0)
    {
        while(selected == 0)
    {
         randA = genrand_int31()%(totalNumofintsl);
        randB = genrand_real1();
        R = randB*Rmaxi;
    if (totalNumofintsl<4)
        {
        temp=iptr[randA];
        selected = 1;

        }
    else if(iptr[randA]->Rj<=R)
    {
        selected = 1;
        temp = iptr[randA];
    }

    }
    }

	float xa = temp->x;
	float ya = temp->y;
	float za = temp->z;
	float x1;
	float x2;
	float xb,yb,zb;
	do{
	x1 = 2*genrand_real1() - 1.0;
	x2 = 2*genrand_real1() - 1.0;} while((x1*x1 + x2*x2)>= 1);

	xb = (2*(x1)*sqrt(1-(x1*x1)-(x2*x2)))*jump;
	yb = (2*(x2)*sqrt(1-(x1*x1)-(x2*x2)))*jump;
	zb = (1-2*((x1*x1)+(x2*x2)))*jump;
    //conditionals check for z=0 and wraparound
    temp->x = xa+xb;
    if(temp->x > xyarea)
    {
        temp->x=temp->x-xyarea;
    }
    if(temp->x<0.0)
    {
        temp->x=xyarea+temp->x;
    }
    temp->y = ya+yb;
    if(temp->y > xyarea)
    {
        temp->y=temp->y-xyarea;
    }
    if(temp->y<0.0)
    {
        temp->y=xyarea+temp->y;
    }
    temp->z = za+zb;
    if (temp->z<=0.0)
    {
        if(temp->type==1)
        {
            totalNumofsfcmi++;
        }
        if(temp->type==0)
        {
            totalNumofsfcmv++;
        }
        temp->id=-1;
        counter++;

    }

    removedf(defects, counter);
    if (Time<100)
    {
        write = 1;
    }
    else if (Time<1000)
    {
        write = 10;
    }
    else if (Time<100000)
    {
       write = 100;
    }
    else if (Time<1000000)
    {
        write = 1000;
    }
    else write = 10000;
    if(Niter%write==0)//
        {
            output=fopen("output2.txt","a");
            if(!output)
            {
                printf("error: file not opened\n");
                exit(-1);
            }
            write_traj(output, defects);
        }
    }
}
//updating time
void update_time(float rn)
{
    float u = genrand_real1();
    float deltat =  -1.0*log10(u)/rn; /*(1/rn)*log(1/u);*/
    Time = Time + deltat;
}

