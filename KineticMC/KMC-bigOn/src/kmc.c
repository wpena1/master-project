#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct defnode
{
	int x,y,z;
	float rate;
	int type;
	struct defnode *next;
}DNODE;

typedef struct ratenode
{
	float rj;
	struct ratenode *next;
}RNODE;

int Niter,Nitermax;
int N;
DNODE interstitial[N/2], vacancy[N/2];
int wi,wv;
int Emi,Emv;
float Kb;
int T;
float time;
RNODE Rates[372];
DNODE Defects[372];


int main(int argc, char argv[])
{
	//allocate defects into array
	N=372;
	DNODE *defects = malloc(N*sizeof(*defects));
	DNODE  *root = malloc(sizeof(*root));
	int i=0;
	for(i=0;i<N;i++)
	{
		defects[i].x=;
		defects[i].y=;
		defects[i].z=;
		if (i%2==0){
		defects[i].type=0;
		interstitial[i]=defects[i];//defects[i].rate=wi*exp(-Emi/(Kb*T));
		}
		else {
		defects[i].type=1;
		//defects[i].rate=wc*exp(-Emv/(Kb*T));
		vacancy[i]= defects[i];
		}
		defects[i].next=defects[i+1];
	}
	//mark for removal
	int j = 0;
	for(j = 0;j<N/2;j++)
	{	for(i = 0;i<N/2;i++)
		{	if (vacancy[i].rate!=-1)
			{difference = sqrt((pow(interstitial[j].x-vacancy[i].x),2) + pow((interstitial[j].y-vacancy[i].y),2) +
			pow((interstitial[j].z-vacancy[i].z),2));
			if(difference<=4)
			{interstitial[j].rate=-1;
			vacancy[i].rate=-1;
			j++;}
			}
		}
	}

	root->next=defects;
	DNODE *prev = root;
	DNODE *next = root->next;
	while(*next!=0)
	{
		if(*next.rate!=-1)
		{
			prev=next;
			next=*next.next;
		}
		else
		{
			*prev.next = *next.next;
			next=*next.next;
		}
	}
	for(i=0;i<N;i++)
	{if (i%2==0){
		defects[i].rate=wi*exp(-Emi/(Kb*T));
		}
		else {
		efects[i].rate=wc*exp(-Emv/(Kb*T));
		}
	}

	RNODE *rates = malloc(N*sizeof(*rates));
	double sum = 0;
	for(i=0;i<N;i++)
	{
		temp = defects[i].rate;
		rates[i].rj=sum + temp;
		sum = sum + temp;
	}
	double RN = sum;
	double u = genrand_real2(void);
	double uR = u*RN;
	int first = 0;
	int last = N;
	int middle = (first + last)/2;
	int found;
	while(first <= last)
	{
		if( rates[middle].rj < uR)
		{	if (uR<=rates[middle+1])
			{found = middle + 1;
			break;}
			first = middle + 1;
		}
		else if (rates[middle].rj > uR)
		{
			if(rates[middle-1].rj<uR)
			{found = middle;
			break;}
			last=middle - 1;
		}
		else
			{found = middle;
			break;}
		middle  = (first + last) /2;
	}

	DNODE temp = Defects[found];
	double x = temp.x;
	double y = temp.y;
	double z = temp.z;



}




	//initialize positions
