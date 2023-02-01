#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "mersenTwister.h"
#include "mersenTwister.c"

//Logarithmic time algorithm:
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
	char *name;
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
DNODE *Dfnodeg;
int Niter=0;
int Nu;
int totalNumofintsl;
int totalNumofvcncy;
int totalNumofrcmtn=0;
int totalNumofsfcmi=0;
int totalNumofsfcmv=0;
//new variables
float *Lnk;
int *Cn;//contains number of events in each list.
int *AdressList;
int numberOfRates;
int write = 0;


unsigned long s;

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
    printf("got data \n");
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
    Dfnodeg=(DNODE *)malloc(sizeof(DNODE)*Nu);
    s = time(NULL);
    init_genrand(s);
    //output = fopen("output2.xyz","w");
    output = fopen("output2.xyz","w");
    if(output==NULL)
    {
        printf("error: file not opened\n");
        exit(-1);
    }
     printf("inside initialize\n");
    initializedefects(Dfnode);
    printf("outside initialize\n");
    setRate(Dfnode);
    clock_t begin, end;
    double time_spent;

    //Main loop
    begin= clock();
    for(i = 0;i<(int)Nitermax; i++)
    {
        setRate(Dfnode);
        float rn = update_cf(Dfnode);
        coe_jump(Dfnode,rn);
        counterb = check_rec(Dfnode);
        removedf(Dfnode, counterb);
        update_time(rn);
        Niter++;
        //printf("in loop %d.\n", i);
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
    Lnk = (float *)malloc(sizeof(float)*numberOfRates);
    Cn = (int *)malloc(sizeof(int)*numberOfRates);

    DNODE *prev = Dfnode;
    prev->next=NULL;
    DNODE *curr;
    for(i=0;i<Nu;i++)
    {
        curr = (DNODE *)malloc(sizeof(DNODE));
        curr->id = i+1;
        curr->type = i%2;
       if(i%2==0)
        {
            curr->z=meanv + stdvv*random_gaussian();
            curr->name = "Ar";

        }
        else
        {
            curr->z=meani + stdvi*random_gaussian();
            curr->name = "Si";
        }

        curr->x = genrand_real1()*xyarea;
        curr->y = genrand_real1()*xyarea;
        curr->next=NULL;
        prev->next=curr;
        Dfnodeg[i]=*curr;
        prev=prev->next;
    }

    int counter;
    totalNumofvcncy = Nu/2;
    printf("total number of vacancies %d \n", totalNumofvcncy);
    totalNumofintsl = Nu/2;
    printf("total number of interstitial %d \n", totalNumofintsl);
   // fprintf(output,"%20s %10s %10s %10s %10s %10s %8s\n", "Time","Interstls","Vacancies", "Recmbtns", "VcncySfc", "IntlnSfc","defects");
    //fprintf(output,"%i %s\n", totalNumofintsl+totalNumofvcncy, "atoms");
    //fprintd(output, "%6d %s", nu, "atoms");
    fclose(output);
   // printf("going to chek");
   write_traj(output,Dfnodeg);
    counter = check_rec(Dfnode);
   // printf("going to chek");
    removedf(Dfnode, counter);
    Cn[0] = totalNumofintsl;
    Cn[1] = totalNumofvcncy;
}

void write_traj(FILE *file, DNODE *node)
{
     fprintf(file, "%6d %s \n", (int) defectnumber, "atoms");
   // int num=totalNumofintsl+totalNumofvcncy;
   // fprintf(file, "%20.4f %10d %10d %10d %10d %10d %10d\n", Time, totalNumofintsl, totalNumofvcncy, totalNumofrcmtn, totalNumofsfcmv, totalNumofsfcmi, num);
   // fclose(file);
    DNODE curr;
    int i;
    for(i = 0;i<defectnumber;i++)
	{   curr=Dfnodeg[i];
        fprintf(file, "%6d %4s %13f %12f %12f \n", curr.id, curr.name, curr.x, curr.y, curr.z);
	}
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
                    int index = iptr[j]->id - 1;
                    int indexb = vptr[i]->id - 1;
                    Dfnodeg[index].x=0.0;
                    Dfnodeg[index].y=0.0;
                    Dfnodeg[index].z=0.0;
                    Dfnodeg[indexb].x=0.0;
                    Dfnodeg[indexb].y=0.0;
                    Dfnodeg[indexb].z=0.0;
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
	return counter;
	}
	else
    {
        return 0;
    }

}
//remove marked defects
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
	int i;
	for(i=0;i<numberOfRates;i++)
    {
        if(i==0)
        {
            Lnk[i]=wi*expf(-(Emi/(Kb*T)));
        }
        else
            Lnk[i]=wv*expf(-(Emv/(Kb*T)));
    }
    if(totalNumofvcncy==0)
            {
                Lnk[1]=0;
            }
}

float update_cf(DNODE *Dfnode)
{   //gets cummulative rate
    if(Nu!=0)
    {
        float i=1;
        int j=0;
        float sum = 0;
        float temp;
        Cn[0] = totalNumofintsl;
        Cn[1] = totalNumofvcncy;
        for (j=0;j<numberOfRates;j++)
        {
            temp = Lnk[j]*Cn[j];
            Lnk[j]=sum+temp;
            sum = sum+temp;
        }
        return sum;
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
    {   DNODE *iptr[totalNumofintsl];//array of pointer nodes
        DNODE *vptr[totalNumofvcncy];
        double u = genrand_real1();
        float uR = u*RN;
       // DNODE *rates[Nu]; //array of pointers
        DNODE *prev,*curr;
       //// prev=defects;
        //curr=prev->next;
        int i=0;
        int found =-1;
         if(totalNumofvcncy==0)
            {
                Lnk[1]=0;
            }
        while(i<numberOfRates||found==-1)
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
        i++;
        }

    //move defects
    int counter=0;
    i=0;
    int j =0;
    int defectcounter=0;
    prev=defects;
    curr=prev->next;
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
	DNODE *temp ;
	int rand_defect;
	if (found == 1){

	rand_defect=genrand_int31()%totalNumofvcncy;
	//printf("rand defecthh %d\n", rand_defect);
	temp = vptr[rand_defect];
	}
    else if(found == 0){

        rand_defect=genrand_int31()%totalNumofintsl;
       // printf("rand defect5 %d\n", rand_defect);
        temp = iptr[rand_defect];}

//printf("getting coordinates \n");
int index = temp->id - 1;
	float xa = temp->x;
	float ya = temp->y;
	float za = temp->z;
	float x1;
	float x2;
	float xb,yb,zb;
	//printf("got coordinates\n");
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
    Dfnodeg[index].x=temp->x;
    temp->y = ya+yb;
    if(temp->y > xyarea)
    {
        temp->y=temp->y-xyarea;
    }
    if(temp->y<0.0)
    {
        temp->y=xyarea+temp->y;
    }
    Dfnodeg[index].y=temp->y;
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


        Dfnodeg[index].z=0.0;
        Dfnodeg[index].name="Re";
        temp->id=-1;
        temp->name = "Re";
        counter++;

    }
    removedf(defects,counter);
   /*if (Time<100)
    {
        write = 1;
    }
    else if (Time<1000)
    {
        write = 1;
    }
    else if (Time<100000)
    {
       write = 1;
    }
    else if (Time<1000000)
    {
        write = 1;
    }
    else write = 100;*/
    if (Niter>=1000000)
    write = 1000;
    if (Niter<1000000)
    write = 1000;
    if(Niter%write==0)
        {
            output=fopen("output2.xyz","a");
            if(!output)
            {
                printf("error: file not opened\n");
                exit(-1);
            }
            write_traj(output, Dfnodeg);
        }
       // printf("end of coejump\n");
    }
}
//updating time
void update_time(float rn)
{
    float u = genrand_real1();
    float deltat = -1.0*log10(u)/rn;
    Time = Time + deltat;
}

