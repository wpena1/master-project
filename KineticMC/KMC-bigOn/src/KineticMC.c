#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "mersenTwister.h"
#include "mersenTwister.c"

// linear time algorithm
//Temperature is held constant.
//
typedef struct defnode
{
	double x,y,z;
	double rate;
	double Rj;
	int type;
	int id;
	struct defnode *next;
}DNODE;
//Functions
void initializedefects(DNODE *Dfnode);//initializes defects
int check_rec(DNODE *Dfnode);//checks for recombinations
double update_cf(DNODE *Dfnode);//updates rates of events
void coe_jump(DNODE *defects, double rn);//carries out event
void update_time(double rn);// updates time
void write_traj(FILE *file, DNODE *node);// writes to a file the state of the system
void removedf(DNODE *node, int counter);// removes events no longer on the system
void setRate(DNODE *Dfnode);// set rates of events
double random_normal();// converts a normal distribution to a gaussian distribution

double xyarea;// area of simulation box in the x and y dimensions
double Nitermax;// maximum number of iterations
FILE *data;// file containing pertaining information for simulation
FILE *output;// output file
double Time;
double Emv;//energy barrier for vacancy in eV
double Emi;//energy barrier for interstitial in eV
double Kb;// boltzman constant
double T;//temperature
double wi;//interstitial migration prefactor
double wv;//vacancy migration prefactor
double recr;// recombination radius
double jump;// jump distance
double defectnumber;// number of defects
double meani;// mean of gaussian distribution of interstitials
double meanv;//mean of gaussian distribution of vacancies
double stdvi;// standard deviation of distribution of interstitials
double stdvv;// standard deviation of distribution of vacancy
DNODE *Dfnode;// pointer to DNODE
//RNODE *Rnode;
int Niter=0;
int Nu;
int totalNumofintsl;
int totalNumofvcncy;
int totalNumofrcmtn=0;
int totalNumofsfcmi=0;
int totalNumofsfcmv=0;
int ex = 1;
int write = 0;
unsigned long s;

int main(int arg, char argc[])
{
    Time = 0.0;//set time to zero
    int counter = 0;
    char line[14];
    double values[14];
    int i;
    data = fopen("data.txt","r");// read data form data file
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
    printf("kb %20.15f. \n", Kb);
    jump = values[11];
    recr = values[12];
    Nitermax = values[13];
    xyarea=115.0;
    Nu=(int)(defectnumber);
    printf("this is nu %d\n", Nu);
    Dfnode=(DNODE *)malloc(sizeof(DNODE));
    s = time(NULL);
    init_genrand(s);
    output = fopen("output2.txt","w");
    if(output==NULL)
    {
        printf("error: file not opened\n");
        exit(-1);
    }

    initializedefects(Dfnode);//initialize defects

    for(i = 0;i<(int)Nitermax; i++)// KMC loop
    {
        setRate(Dfnode);//set rates
        double rn = update_cf(Dfnode);//calculate cummulative rate
       // printf("before\n");
        coe_jump(Dfnode,rn);//carry out event
      //  printf("after\n");
        counterb = check_rec(Dfnode);//check for recombination
        removedf(Dfnode, counterb);//removed recombined defects
        update_time(rn);//update  Time
        Niter++;// increase number of iterations
       // if(Niter>90000000)
      //{
          // printf(" made it here %d\n", i);
      // }
    }
    return 0;// finish program

}
// initialize defects
void initializedefects(DNODE *Dfnode)
{
    int i;
    int j=0;
    int k=0;
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
            curr->z=meanv + stdvv*random_normal();//convert normal to gaussian distribution
        }
        else
        {
            curr->z=meani + stdvi*random_normal();
        }

        curr->x = genrand_real1()*20;//assign random x and y position
        curr->y = genrand_real1()*20;
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

void write_traj(FILE *file, DNODE *node)//write output
{

    int num=totalNumofintsl+totalNumofvcncy;
    fprintf(file, "%20.4f %10d %10d %10d %10d %10d %10d\n", Time, totalNumofintsl, totalNumofvcncy, totalNumofrcmtn, totalNumofsfcmv, totalNumofsfcmi, num);

    fclose(file);

}

double random_normal()//return random number from normal distribution
{
  return sqrt(-2*log(genrand_real1())) * cos(2*3.1415925*genrand_real1());
}

int check_rec(DNODE *Dfnode)
{   if(totalNumofintsl!=0 && totalNumofvcncy!=0)
   {
    DNODE *iptr[totalNumofintsl];
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
    j = 0;
    i = 0;
    double difference;
    int counter=0;

	while(j<totalNumofintsl)
	{
	    while(i<totalNumofvcncy)
		{
		    if (vptr[i]->id!=-1 && iptr[j]->id!=-1)
			{
			    difference = pow(abs(iptr[j]->x - vptr[i]->x), 2) + pow(abs(iptr[j]->y - vptr[i]->y), 2) +
                pow(abs(iptr[j]->z - vptr[i]->z), 2);
                if(difference<16.0)
                {
        //
                    iptr[j]->id=-1;
                    vptr[i]->id=-1;
                    counter=counter+2;
                    totalNumofrcmtn++;
                    i=totalNumofvcncy;
                    j=totalNumofintsl;
                }

                else
                    {i++;
                    }
			}
			else i++;
		}
		j++;
	}

	return counter;}
	else
    {
    return 0;}

}
void removedf(DNODE *node, int counter)
{
    int i;
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

void setRate(DNODE *Dfnode)
{
    DNODE *prev;
	prev = Dfnode;
	DNODE *curr ;
	curr = prev->next;

    while(curr!=NULL)
	{
		if(curr->type==1)
		{
		    curr->rate=wi*exp(-(Emi/(Kb*T)));
            curr = curr->next;

        }
		else
            {

            curr->rate=wv*exp(-(Emv/(Kb*T)));
            curr = curr->next;

            }
	}
}

double update_cf(DNODE *Dfnode)
{   if(Nu!=0)
    {double i=1;
    int j=0;
    double sum = 0;
    double temp;
    DNODE *prev;
	prev = Dfnode;
	DNODE *curr ;
	curr = prev->next;

	while(curr!=NULL)
	{
	    //if(curr->type==1)
        temp = curr->rate;
        curr->Rj=sum+temp;
        sum = sum + temp;
		curr=curr->next;
	}

    return sum;
}
}

void coe_jump(DNODE *defects, double RN)
{
    if(Nu!=0)
    {double u = genrand_real1();
	double uR = u*RN;
	DNODE *rates[Nu];
	DNODE *prev,*curr;
	prev=defects;
	curr=prev->next;
	int i=0;
    while(curr!=NULL)
	{   //if(curr->type==1)
	    rates[i]=curr;
		i++;
		curr = curr->next;
	}
	int first = 0;
	int last = Nu - 1;
	int middle = (first + last)/2;
	int found;
	while(first <= last)
	{
		if( rates[middle]->Rj < uR)
		{
		    first = middle + 1;
		    if (uR<=rates[middle+1]->Rj)
			{
			    found = middle + 1;
                break;
            }

		}
		else if (rates[middle]->Rj == uR)
		{   if(middle==0)
           {
               found=middle;
               break;
           }
			else
            {if(rates[middle-1]->Rj<uR && rates[middle]->Rj <= uR)
			{
			    found = middle;
                break;
            }
            }

		}
		else
			{
			    last = middle - 1;
            if (middle==0)
            {
                found=middle;
                break;
            }
            else{
			if(rates[middle-1]->Rj < uR && uR<=rates[middle]->Rj)
            {
                found = middle;
                break;
            }
            }


			}
		middle  = (first + last)/2;

	}

    int counter=0;
	DNODE *temp = rates[found];
	double xa = temp->x;
	double ya = temp->y;
	double za = temp->z;
	double x1;
	double x2;
	double xb,yb,zb;
	do{
	x1 = 2*genrand_real1() - 1.0;
	x2 = 2*genrand_real1() - 1.0;} while((x1*x1 + x2*x2)>= 1);
	xb = (2*(x1)*sqrt(1-(x1*x1)-(x2*x2)))*jump;
	yb = (2*(x2)*sqrt(1-(x1*x1)-(x2*x2)))*jump;
	zb = (1-2*((x1*x1)+(x2*x2)))*jump;
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

    removedf(defects,counter);
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
    else {write = 10000;
    //printf("%d\n", Niter);
    }
    if(Niter%write==0){
    output=fopen("output2.txt","a");
    if(!output)
    {
        printf("error: file not opened\n");
        exit(-1);
    }
   // printf("%d\n", Niter);
    write_traj(output, defects);

    }
   }
}
void update_time(double rn)
{
    double u = genrand_real1();
    double deltat = -1.0*log10(u)/rn;
    Time = Time + deltat;
}
