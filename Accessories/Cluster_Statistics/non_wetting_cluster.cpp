#include<iostream>
#include<cmath>
#include<cstdlib>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<string>
#include <math.h>

#include <cstdlib>
#include <algorithm>


using namespace std; 

int main (int argc , char * argv [])
{
//ifstream fin(argv[1]);
int NCHAR=128;
	char     filename[128], dummy[128+1];
	int      dummyInt;

const int max_cluster=1000000;
const double crival=0.001;


int nx,ny,nz;
int vtk_format;


const int e[18][3]=
{{1,0,0,},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1},{1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},{0,1,1},
{0,-1,1},{0,1,-1},{0,-1,-1},{1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1}};

char poreFileName[128];

char poreFileNameVTK[128];
char poreFileNameOut[128];
int mark,ii,jj,kk,loop,sum2;

int sum=0;
int sum1=0;
int pore_sum;
int sum3;
int sn[max_cluster];


int* sn2;
sn2=new int[max_cluster];

int phase_ind;
int exp_vtk;

int lnx,rnx,lny,rny,lnz,rnz;

for (int i=0;i<max_cluster;i++)
	sn[i]=0;


ifstream fins(argv[1]);
							fins.getline(dummy, NCHAR);
	fins >> poreFileName;				fins.getline(dummy, NCHAR);
	fins >> nx>> ny>>nz;				fins.getline(dummy, NCHAR);
	fins >> poreFileNameVTK;				fins.getline(dummy, NCHAR);
	fins >> vtk_format;				fins.getline(dummy, NCHAR);
	fins >> phase_ind;				fins.getline(dummy, NCHAR);
	fins >> lnx>>rnx;                                fins.getline(dummy, NCHAR);
	fins >> lny>>rny;                                fins.getline(dummy, NCHAR);
	fins >> lnz>>rnz;                                fins.getline(dummy, NCHAR);
	fins >> exp_vtk;				 fins.getline(dummy, NCHAR);
fins.close();	






int*** Solid;
double pore;


	FILE *ftest;
	ifstream fin;
	
	ftest = fopen(poreFileName, "r");

	if(ftest == NULL)
	{
		cout << "\n The pore geometry file (" << poreFileName <<
			") does not exist!!!!\n";
		cout << " Please check the file\n\n";

		exit(0);
	}
	fclose(ftest);

	fin.open(poreFileName);



	Solid = new int**[nx];
	
		
	for (int i=0; i<nx;i++)
	{
	       Solid[i] = new int*[ny];
	       for (int j=0;j<ny;j++)
	       {
	               Solid[i][j] = new int[nz];
	               for (int k=0;k<nz;k++)
	                       Solid[i][j][k] = 0;
	       }
	}
		
	cout<<endl;
	cout<<"=================================================================\n";
	cout<<"		    		Cluster Statistics Programme\n";
	cout<<"	Jianhui Yang - All right reserved	JAN2013	\n";
	cout<<"=================================================================\n";
	cout<<endl;
	cout<<"Start reading source geometry file"<<endl;

	
	if (vtk_format==1)
		{
		fin.getline(dummy, NCHAR);
		fin.getline(dummy, NCHAR);
		fin.getline(dummy, NCHAR);
		fin.getline(dummy, NCHAR);
		fin.getline(dummy, NCHAR);
		fin.getline(dummy, NCHAR);
		fin.getline(dummy, NCHAR);
		fin.getline(dummy, NCHAR);
		fin.getline(dummy, NCHAR);
		fin.getline(dummy, NCHAR);
		}



	for(int k=0 ; k<nz ; k++)				///*********
	for(int j=0 ; j<ny ; j++)
	for(int i=0 ; i<nx ; i++)				///*********


	//while (!fin.eof())                                        //**********
		{	
			//fin >> ci >> cj>> ck>>pore;
			fin >> pore;
			
			//if (pore == 0.0)	{Solid[ci-1][cj-1][ck-1] = 0;}
			if (pore > crival)	{Solid[i][j][k] = 1;        if ((i>=lnx) and (i<rnx) and (j>=lny) and (j<rny) and (k>=lnz) and (k<rnz)) sum++;}
			else
				if (pore < -crival) 
				{Solid[i][j][k] = -1;if ((i>=lnx) and (i<rnx) and (j>=lny) and (j<rny) and (k>=lnz) and (k<rnz))  {sum++;sum1++;}}
			
			
		
			
			
		}
		
	fin.close();
		
	pore_sum=sum;
	cout<<"porosity="<<double(pore_sum)/double((rnx-lnx)*(rny-lny)*(rnz-lnz))<<endl;
	cout<<"non wetting saturation="<<double(sum1)/double(sum)<<endl;
	cout<<"READING COMPLETE"<<endl;	
	

	sum3=0;

	sum=1;



	for (int i=lnx;i<rnx;i++)
		for (int j=lny;j<rny;j++)
			for (int k=lnz;k<rnz;k++)
			if (Solid[i][j][k]==phase_ind)
				{
				sum2=0;
				sum++;Solid[i][j][k]=-sum;sum2++;
				
				mark=1;
				while (mark>0)
				{
				mark=0;
				for (int ias=lnx;ias<rnx;ias++)
				for (int jas=lny;jas<rny;jas++)
				for (int kas=lnz;kas<rnz;kas++)

					if (Solid[ias][jas][kas]==-sum)
					for (int ls=0;ls<6;ls++)
					{
					ii=ias+e[ls][0];
					jj=jas+e[ls][1];
					kk=kas+e[ls][2];
				
					if ((ii>=lnx) and (ii<rnx) and (jj>=lny) and (jj<rny) and (kk>=lnz) and (kk<rnz))
						{
						if (Solid[ii][jj][kk]==phase_ind)
							{
							Solid[ii][jj][kk]=-sum;
							sum2++;mark=1;
							}
						}
					}
				
				
				if (sum-2>max_cluster)
					cout<<"WARNING: BEYOND MAX NUMBER OF CLUSTER"<<endl;
				sn[sum-2]=sum2;sn2[sum-2]=sum2;
				
				}
				cout<<sum<<"	"<<sum2<<endl;
				}
	
		cout<<"CLUSTER SEARCHING COMPLETE"<<endl;



int* capa;
int* capa2;
double* capa3;

int indi;
double sta_sum;
capa =  new int[sum-2];
capa2 = new int[sum-2];
capa3 = new double[sum-2];

sum3=0;

for (int i=0;i<sum-2;i++)
	capa2[i]=0;


for (int i=0;i<sum-1;i++)
	{
	indi=0;
		for (int j=0;j<sum3;j++)
			{
			if (sn[i]==capa[j])
				{capa2[j]++;indi=1;}
			}
	if (indi==0)
		{
		capa[sum3]=sn[i];
		capa2[sum3]++;
		sum3++;
		}
	
	}

for (int i=0;i<sum3;i++)
	{
	sta_sum=0.0;
	for (int j=0;j<sum3;j++)
		if (capa[j]>=capa[i])
			sta_sum=sta_sum+double(capa[j])*double(capa2[j])/double(pore_sum);
	capa3[i]=sta_sum;
	}



	//cout<<sum3<<endl;





		ostringstream name;
		name<<poreFileNameVTK;

		ofstream out;
		out.open(name.str().c_str());
	
		for (int i=0;i<sum3;i++)
		out<<capa[i]<<"		"<<capa2[i]<<"		"<<double(capa2[i])/double(pore_sum)<<"		"<<capa3[i]<<"		"<<double(capa[i])/double(pore_sum)<<endl;
		
		out.close();	
	






sort(sn2,sn2+sum-1);


//for (int i=0;i<=sum-2;i++)
//	cout<<i<<" "<<sn2[i]<<endl;


ostringstream name3;
		name3<<"Cluster_size_index_finder.txt";

		ofstream out3;
		out3.open(name3.str().c_str());
out3<<"Cluster size	index	"<<endl;
for (int i=0;i<=sum-2;i++)
	{
	for (int j=0;j<=sum-2;j++)
	if (sn[j]==sn2[i])
		{
		sn[j]=0;	
		out3<<sn2[i]<<"		"<<j+2<<endl;
		j=sum;
		}
	}		



	//cout<<sum<<"        @@@@@@@@@@@@"<<endl;
	
	int jumpnum,lsar;
	int* maparr1;
	int* maparr2;
if (exp_vtk==1)
	{
	        
	        jumpnum=(int)sum/4;
	        maparr1 = new int[sum+1];
	         maparr2 = new int[sum+1];
	        
	        for (int i=0;i<=sum;i++)
	                {maparr1[i]=0;maparr2[i]=0;}
	      lsar=1;
	        for (int i=1;i<=sum;i++)
	        {
	                  //lsar=i+jumpnum;
	                  while (maparr2[lsar]>0)
	                          {
	                                  lsar+=1;
	                                  if (lsar>sum)
	                                          lsar-=sum;
	                          }
	                          maparr1[i]=lsar;//cout<<lsar<<"        @@@"<<endl;
	                          maparr2[lsar]=i;
	                          lsar=lsar+jumpnum;
	                          if (lsar>sum)
	                                          lsar-=sum;
	        }
	        
	        
	ostringstream name2;
	name2<<"Processed_psi.vtk";
	
	

	out.open(name2.str().c_str());		
	out<<"# vtk DataFile Version 2.0"<<endl;
	out<<"J.Yang Lattice Boltzmann Simulation 3D Single Phase-Solid-Density"<<endl;
	out<<"ASCII"<<endl;
	out<<"DATASET STRUCTURED_POINTS"<<endl;
	out<<"DIMENSIONS         "<<rnx-lnx<<"         "<<rny-lny<<"         "<<rnz-lnz<<endl;       ///*********
	out<<"ORIGIN 0 0 0"<<endl;
	out<<"SPACING 1 1 1"<<endl;
	out<<"POINT_DATA     "<<(rnx-lnx)*(rny-lny)*(rnz-lnz)<<endl;				///*********
	out<<"SCALARS sample_scalars float"<<endl;
	out<<"LOOKUP_TABLE default"<<endl;
	for (int k=lnz;k<rnz;k++)
	for (int j=lny;j<rny;j++)
	for (int i=lnx;i<rnx;i++)
		
	//for (int k=0;k<nz;k++)
	//for (int j=0;j<ny;j++)
	//for (int i=0;i<nx;i++)
	        if (Solid[i][j][k]<0)
		out<<-maparr1[-Solid[i][j][k]]<<" ";
	        else
	                out<<Solid[i][j][k]<<" "; 

	
	out.close();
	}








	
}


