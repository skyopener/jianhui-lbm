#include<iostream>
#include<cmath>
#include<cstdlib>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<string>
#include<math.h> 
# include "mpi.h"



#define MAXN 100
#define eps 1e-12
#define zero(x) (fabs(x)<eps)

struct mat{
    int n,m;
    double data[MAXN][MAXN];
};



//D3Q19 STANDARD LATTICE MRT LATTICE BOLTZMANN METHOD
//UNIFORM MESH, LATTICE VELOCITY 1


using namespace std;        
const int Q=19;          


double u_max,u_ave,gx,gy,gz,porosity;

//----------
double s_e;
double s_eps;
double s_q;
//----------

double s_v;
double q_p;

int cl,cr;

int EI;


int Count,NX,NY,NZ;

int mirX=0;
int mirY=0;
int mirZ=0;
int mir=0;


double M[19][19]=
{{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
{-1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1},
{1,-2,-2,-2,-2,-2,-2,1,1,1,1,1,1,1,1,1,1,1,1},
{0,1,-1,0,0,0,0,1,-1,1,-1,1,-1,1,-1,0,0,0,0},
{0,-2,2,0,0,0,0,1,-1,1,-1,1,-1,1,-1,0,0,0,0},
{0,0,0,1,-1,0,0,1,-1,-1,1,0,0,0,0,1,-1,1,-1},
{0,0,0,-2,2,0,0,1,-1,-1,1,0,0,0,0,1,-1,1,-1},
{0,0,0,0,0,1,-1,0,0,0,0,1,-1,-1,1,1,-1,-1,1},
{0,0,0,0,0,-2,2,0,0,0,0,1,-1,-1,1,1,-1,-1,1},
{0,2,2,-1,-1,-1,-1,1,1,1,1,1,1,1,1,-2,-2,-2,-2},
{0,-2,-2,1,1,1,1,1,1,1,1,1,1,1,1,-2,-2,-2,-2},
{0,0,0,1,1,-1,-1,1,1,1,1,-1,-1,-1,-1,0,0,0,0},
{0,0,0,-1,-1,1,1,1,1,1,1,-1,-1,-1,-1,0,0,0,0},
{0,0,0,0,0,0,0,1,1,-1,-1,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,-1,-1},
{0,0,0,0,0,0,0,0,0,0,0,1,1,-1,-1,0,0,0,0},
{0,0,0,0,0,0,0,1,-1,1,-1,-1,1,-1,1,0,0,0,0},
{0,0,0,0,0,0,0,-1,1,1,-1,0,0,0,0,1,-1,1,-1},
{0,0,0,0,0,0,0,0,0,0,0,1,-1,-1,1,-1,1,1,-1}};

double M_c[19];

double MI[19][19];







double m[19];
double meq[19];





double uMax,c,Re,dx,dy,Lx,Ly,dt,rho0,P0,tau_f,niu,error,SFx,SFy,reso;


void tests();

void init_Sparse_read_rock_parallel( int*, int*);

void init(double*, double**, double**,double**, double**, double**, double*, double*,double*, double*, double*, double***,int*);

void collision(double*,double** ,double** ,double**,double**,double**,  double* ,double* , double*, double*, double*, int* ,int***,int* ,int*);

void comput_macro_variables(double* ,double**,double** ,double** ,double** ,double**, double**, double*, double*,double*, double*, double* ,int* ,int***,double***);

double Error(double** ,double** ,double*, double*);

void boundary_velocity(int,double,int , double ,int ,double ,int ,double ,int ,double ,int ,double ,double** ,double**,double* ,double** ,int*** );

void boundary_pressure(int ,double ,int , double ,int ,double ,int ,double ,int ,double ,int ,double ,double** ,double**,double** ,double* ,int*** );

void Solute_Constant_BC(int,double,int, double,int ,double ,int ,double ,int ,double ,int,double ,double**,int***,double*,double**,double**);

void Solute_ZeroFlux_BC(int,double, int,double,int,double,int,double,int,double,int,double,double**,int***,double*,double**,double**);

void output_velocity(int ,double* ,double** ,int ,int ,int ,int ,int*** );

void output_density(int ,double* ,int ,int ,int ,int ,int*** );	

void Geometry(int*** );	

void output_velocity_b(int ,double* ,double** ,int ,int ,int ,int ,int*** );

void output_psi(int ,double* ,int ,int ,int ,int ,int*** );

void output_psi_b(int ,double* ,int ,int ,int ,int ,int*** );

void output_density_b(int ,double* ,int ,int ,int ,int ,int*** );	

void Geometry_b(int*** );

double Comput_Perm(double** u,double*,int,int*);

double S[19];

void Comput_MI(double[19][19], double[19][19]);

void Statistical_psi(double*,int, int***);

int inverse(mat &a);

double feq(int,double, double[3]);

double feq_psi(int,double, double[3]);

void Suppliment(int*,int***);

void Backup(int ,double*, double*, double**, double**, double**);

void Backup_init(double* , double** , double** ,double** , double**, double**, double* , double* ,double*, double*, double*, char[128], char[128], char[128], char[128], char[128]);

void psi_reset(double**,double** , double* , double* , double*** , int* );

void Parallelize_Geometry();


const int e[19][3]=
{{0,0,0},{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1},{1,1,0},{-1,-1,0},{1,-1,0},{-1,1,0},{1,0,1},
{-1,0,-1},{1,0,-1},{-1,0,1},{0,1,1},{0,-1,-1},{0,1,-1},{0,-1,1}};

double elat[19][3]=
{{0,0,0},{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1},{1,1,0},{-1,-1,0},{1,-1,0},{-1,1,0},{1,0,1},
{-1,0,-1},{1,0,-1},{-1,0,1},{0,1,1},{0,-1,-1},{0,1,-1},{0,-1,1}};


const double w[19]={1.0/3.0,1.0/18.0,1.0/18.0,1.0/18.0,1.0/18.0,1.0/18.0,1.0/18.0,1.0/36.0,1.0/36.0,1.0/36.0,1.0/36.0,1.0/36.0,1.0/36.0,1.0/36.0,1.0/36.0,1.0/36.0,1.0/36.0,1.0/36.0,1.0/36.0};


//========*************************===============
//int LR[19]={0,2,1,4,3,6,5,10,9,8,7,14,13,12,11,18,17,16,15};
const int LR[19]={0,2,1,4,3,6,5,8,7,10,9,12,11,14,13,16,15,18,17};
const int FRP[19]={0,0,0,0,0,0,0,1,0,2,0,3,0,4,0,0,0,0,0};
const int FLN[19]={0,0,0,0,0,0,0,0,1,0,2,0,3,0,4,0,0,0,0};
const int RP[5]={1,7,9,11,13};
const int LN[5]={2,8,10,12,14};
const int SYN[5]={3,7,10,15,17};
const int SYP[5]={4,8,9,16,18};
const int SZN[5]={5,11,14,15,18};
const int SZP[5]={6,12,13,16,17};
const int SXN[5]={1,7,9,11,13};
const int SXP[5]={2,8,10,12,14};

//=========================================


int n,nx_l,n_max,in_BC,PerDir,freRe,freDe,freVe,frePsi,Par_Geo,Par_nx,Par_ny,Par_nz;
int Zoom,lattice_v,Sub_BC_psi,Sub_Dis,freDis,ini_psi;


int wr_per,pre_xp,pre_xn,pre_yp,pre_yn,pre_zp,pre_zn,fre_backup;
int vel_xp,vel_xn,vel_yp,vel_yn,vel_zp,vel_zn,Sub_BC,Out_Mode,mode_backup_ini;
double p_xp,p_xn,p_yp,p_yn,p_zp,p_zn,niu_l,niu_s,tau_s;
double inivx,inivy,inivz,v_xp,v_xn,v_yp,v_yn,v_zp,v_zn;
double error_perm,S_l,gxs,gys,gzs,Gravity;
double Buoyancy_parameter=1.0;
double ref_psi,c_s,c_s2,dx_input,dt_input,lat_c;
double Dispersion=0;
double disp_pre=0;

int sol_c_xp,sol_c_xn,sol_c_yp,sol_c_yn,sol_c_zp,sol_c_zn;
double c_xp,c_xn,c_yp,c_yn,c_zp,c_zn;

int sol_zf_xp,sol_zf_xn,sol_zf_yp,sol_zf_yn,sol_zf_zp,sol_zf_zn;
double zf_xp,zf_xn,zf_yp,zf_yn,zf_zp,zf_zn,psi_total;
int par_per_x,par_per_y,par_per_z,per_xp,per_xn,per_yp,per_yn,per_zp,per_zn;

char outputfile[128]="./";
int NCHAR=128;
	char     filename[128], dummy[128+1],filenamepsi[128], backup_rho[128], backup_velocity[128],backup_psi[128], backup_f[128], backup_g[128];
	int      dummyInt;
	
	

int*** Solid;
double*** Psi_local;

	
int main(int argc , char *argv [])
{	

MPI :: Init (argc , argv );
MPI_Status status ;

double start , finish,remain;

int rank = MPI :: COMM_WORLD . Get_rank ();
int para_size=MPI :: COMM_WORLD . Get_size ();

int dif,th,tm,ts;

int tse,the,tme;
double elaps;


double Per_l[3],Per_g[3];
double v_max;




	MPI_Barrier(MPI_COMM_WORLD);
	start = MPI_Wtime();


	if (rank==0)
	{
	ifstream fin(argv[1]);
	                                                fin.getline(dummy, NCHAR);
	fin >> filename;				fin.getline(dummy, NCHAR);
	fin >> filenamepsi;				fin.getline(dummy, NCHAR);
	fin >> NX >> NY >> NZ;				fin.getline(dummy, NCHAR);
	fin >> n_max;					fin.getline(dummy, NCHAR);
	fin >> reso;					fin.getline(dummy, NCHAR);
	fin >> in_BC;					fin.getline(dummy, NCHAR);
	fin >> Buoyancy_parameter >>ref_psi;		fin.getline(dummy, NCHAR);
	fin >> Gravity;					fin.getline(dummy, NCHAR);
	fin >> gx >> gy >> gz;				fin.getline(dummy, NCHAR);
	fin >> pre_xp >> p_xp >> pre_xn >> p_xn;	fin.getline(dummy, NCHAR);
	fin >> pre_yp >> p_yp >> pre_yn >> p_yn;	fin.getline(dummy, NCHAR);
	fin >> pre_zp >> p_zp >> pre_zn >> p_zn;	fin.getline(dummy, NCHAR);
	fin >> vel_xp >> v_xp >> vel_xn >> v_xn;	fin.getline(dummy, NCHAR);
	fin >> vel_yp >> v_yp >> vel_yn >> v_yn;	fin.getline(dummy, NCHAR);
	fin >> vel_zp >> v_zp >> vel_zn >> v_zn;	fin.getline(dummy, NCHAR);
	fin >> sol_c_xp >> c_xp >> sol_c_xn >> c_xn;	fin.getline(dummy, NCHAR);
	fin >> sol_c_yp >> c_yp >> sol_c_yn >> c_yn;	fin.getline(dummy, NCHAR);
	fin >> sol_c_zp >> c_zp >> sol_c_zn >> c_zn;	fin.getline(dummy, NCHAR);
	fin >> sol_zf_xp >> zf_xp >> sol_zf_xn >> zf_xn;	fin.getline(dummy, NCHAR);
	fin >> sol_zf_yp >> zf_yp >> sol_zf_yn >> zf_yn;	fin.getline(dummy, NCHAR);
	fin >> sol_zf_zp >> zf_zp >> sol_zf_zn >> zf_zn;	fin.getline(dummy, NCHAR);
	fin >> niu;					fin.getline(dummy, NCHAR);
	fin >> niu_s;					fin.getline(dummy, NCHAR);
	fin >> inivx >> inivy >> inivz;			fin.getline(dummy, NCHAR);
							        fin.getline(dummy, NCHAR);
	fin >> wr_per;					fin.getline(dummy, NCHAR);
	fin >> PerDir;					fin.getline(dummy, NCHAR);
	fin >> freRe;					fin.getline(dummy, NCHAR);
	fin >> Out_Mode;				fin.getline(dummy, NCHAR);
	fin >> freVe;					fin.getline(dummy, NCHAR);
	fin >> freDe;					fin.getline(dummy, NCHAR);
	fin >> frePsi;					fin.getline(dummy, NCHAR);
	fin >> Sub_Dis >> freDis;                fin.getline(dummy, NCHAR);
							fin.getline(dummy, NCHAR);
	fin >> lattice_v >> dx_input >> dt_input;	fin.getline(dummy, NCHAR);
	fin >> outputfile;				fin.getline(dummy, NCHAR);
	fin >> Sub_BC;					fin.getline(dummy, NCHAR);
	fin >> Sub_BC_psi;				fin.getline(dummy, NCHAR);
	fin >> ini_psi;                                  fin.getline(dummy, NCHAR);
	fin >> par_per_x >> par_per_y >>par_per_z;	fin.getline(dummy, NCHAR);
	fin >> per_xp >> per_xn;			fin.getline(dummy, NCHAR);
	fin >> per_yp >> per_yn;			fin.getline(dummy, NCHAR);
	fin >> per_zp >> per_zn;			fin.getline(dummy, NCHAR);
	                                                        fin.getline(dummy, NCHAR);
	fin >> fre_backup;                        	fin.getline(dummy, NCHAR);
	fin >>mode_backup_ini;                		fin.getline(dummy, NCHAR);
//	fin >> backup_rho;                        	fin.getline(dummy, NCHAR);
//	fin >> backup_velocity;                		fin.getline(dummy, NCHAR);
//	fin >> backup_psi;                        	fin.getline(dummy, NCHAR);
//	fin >> backup_f;                        	fin.getline(dummy, NCHAR);
//	fin >> backup_g;                        	fin.getline(dummy, NCHAR);
	
	
	fin.close();
	
	//cout<<inivy<<"    asdfa "<<endl;
	NX=NX-1;NY=NY-1;NZ=NZ-1;
	}

	MPI_Bcast(&filename,128,MPI_CHAR,0,MPI_COMM_WORLD);MPI_Bcast(&Buoyancy_parameter,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&NX,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&NY,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&NZ,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&n_max,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&reso,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&in_BC,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&gx,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&gy,1,MPI_DOUBLE,0,MPI_COMM_WORLD);MPI_Bcast(&gz,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&ref_psi,1,MPI_DOUBLE,0,MPI_COMM_WORLD);


	MPI_Bcast(&pre_xp,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&p_xp,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&pre_xn,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&p_xn,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&pre_yp,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&p_yp,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&pre_yn,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&p_yn,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&pre_zp,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&p_zp,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&pre_zn,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&p_zn,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&vel_xp,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&v_xp,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&vel_xn,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&v_xn,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&vel_yp,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&v_yp,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&vel_yn,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&v_yn,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&vel_zp,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&v_zp,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&vel_zn,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&v_zn,1,MPI_DOUBLE,0,MPI_COMM_WORLD);

	MPI_Bcast(&inivx,1,MPI_DOUBLE,0,MPI_COMM_WORLD);MPI_Bcast(&inivy,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&inivz,1,MPI_DOUBLE,0,MPI_COMM_WORLD);MPI_Bcast(&wr_per,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&PerDir,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&freRe,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&freVe,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&freDe,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&Gravity,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&outputfile,128,MPI_CHAR,0,MPI_COMM_WORLD);

	MPI_Bcast(&Sub_BC,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&Out_Mode,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&niu,1,MPI_DOUBLE,0,MPI_COMM_WORLD);MPI_Bcast(&niu_s,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
      	
	MPI_Bcast(&frePsi,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&fre_backup,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&backup_rho,128,MPI_CHAR,0,MPI_COMM_WORLD);MPI_Bcast(&backup_velocity,128,MPI_CHAR,0,MPI_COMM_WORLD);
	MPI_Bcast(&mode_backup_ini,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&backup_psi,128,MPI_CHAR,0,MPI_COMM_WORLD);
	MPI_Bcast(&backup_f,128,MPI_CHAR,0,MPI_COMM_WORLD);MPI_Bcast(&backup_g,128,MPI_CHAR,0,MPI_COMM_WORLD);

	MPI_Bcast(&sol_c_xp,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&c_xp,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&sol_c_xn,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&c_xn,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&sol_c_yp,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&c_yp,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&sol_c_yn,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&c_yn,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&sol_c_zp,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&c_zp,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&sol_c_zn,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&c_zn,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&sol_zf_xp,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&zf_xp,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&sol_zf_xn,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&zf_xn,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&sol_zf_yp,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&zf_yp,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&sol_zf_yn,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&zf_yn,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&sol_zf_zp,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&zf_zp,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&sol_zf_zn,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&zf_zn,1,MPI_DOUBLE,0,MPI_COMM_WORLD);

	MPI_Bcast(&lattice_v,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&dx_input,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&dt_input,1,MPI_DOUBLE,0,MPI_COMM_WORLD);MPI_Bcast(&Sub_BC_psi,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&Sub_Dis,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&freDis,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&ini_psi,1,MPI_INT,0,MPI_COMM_WORLD);

	MPI_Bcast(&par_per_x,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&par_per_y,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&par_per_z,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&per_yp,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&per_xp,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&per_yn,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&per_zp,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&per_zn,1,MPI_INT,0,MPI_COMM_WORLD);MPI_Bcast(&per_xn,1,MPI_INT,0,MPI_COMM_WORLD);





int U_max_ref=0;
mirX=0;
mirY=0;
mirZ=0;
mir=1;
Zoom=1;

if (mirX==1)
	NX=NX*2+1;
if (mirY==1)
	NY=NY*2+1;
if (mirZ==1)
	NZ=NZ*2+1;


if (Zoom>1)
	{	
	NX=(NX+1)*Zoom-1;
	NY=(NY+1)*Zoom-1;
	NZ=(NZ+1)*Zoom-1;
	}

//if (Zoom>1)	
//	reso=reso/Zoom;





	double* Permia;
	double* rho;
	double* rho_r;
	double* rhor;
	double** u;
	double**f;
	double**fg;
	double**F;
	double**Fg;
	double**u0;
	int* SupInv;
	double* forcex;
	double* forcey;
	double* forcez;
	
	int*  Sl;
	int*  Sr;
	
	
	
	
	 Parallelize_Geometry();
	
	
	Sl = new int[(NY+1)*(NZ+1)];
	Sr = new int[(NY+1)*(NZ+1)];

//	Psi_rank= new float[nx_l*(NY+1)*(NZ+1)];
//	Solid_rank= new int[nx_l*(NY+1)*(NZ+1)];
	
/*	

        Solid = new int**[nx_l];
	Psi_local = new double**[nx_l];
	for (int i=0;i<nx_l;i++)
		{
		Solid[i] = new int*[NY+1];
		Psi_local[i] = new double*[NY+1];
			for (int j=0;j<=NY;j++)
			{
			Solid[i][j]= new int[NZ+1];
			Psi_local[i][j] = new double[NZ+1];
			}
		}
*/
		MPI_Barrier(MPI_COMM_WORLD);
		

	init_Sparse_read_rock_parallel(Sl,Sr);
	MPI_Barrier(MPI_COMM_WORLD);
	
	
	//***************************************************
	//WARRING: SPARSE MATRIX STARTS FROM INDEX 1 NOT 0!!!
	//***************************************************

	rho = new double[Count+1];
	rho_r = new double[Count+1];
	
	Permia = new double[3];
	rhor = new double[Count+1];
	
	forcex = new double[Count+1];
	forcey = new double[Count+1];
	forcez = new double[Count+1];
	
	
	u = new double*[Count+1];
	u[0] = new double[(Count+1)*3];
	        for (int i=1;i<=Count;i++)
	              u[i] = u[i-1]+3;
	      
	      
	f = new double*[Count+1];
	f[0] =new double[(Count+1)*19];
	        for (int i=1;i<=Count;i++)
	                f[i] = f[i-1]+19;
	        
	        
	F = new double*[Count+1];
	F[0] =new double[(Count+1)*19];
	for (int i=1;i<=Count;i++)
		F[i] = F[i-1]+19;
	
	
	fg = new double*[Count+1];
	fg[0] =new double[(Count+1)*19];
	        for (int i=1;i<=Count;i++)
	                fg[i] = fg[i-1]+19;
	        
	        
	Fg = new double*[Count+1];
	Fg[0] =new double[(Count+1)*19];
	for (int i=1;i<=Count;i++)
		Fg[i] = Fg[i-1]+19;
	
	
	u0 = new double*[Count+1];
	u0[0] = new double[(Count+1)*3];
	        for (int i=1;i<=Count;i++)
		u0[i] = u0[i-1]+3;
	
	SupInv = new int[Count+1];

	/*for (int i=0;i<=Count;i++)
		{
		u[i] = new double[3];
		f[i] = new double[19];
		u0[i] = new double[3];
		F[i] = new double[19];
		fg[i] = new double[19];
		Fg[i] = new double[19];
		}
		
		*/

	Comput_MI(M,MI);
	
	Suppliment(SupInv,Solid);

	MPI_Barrier(MPI_COMM_WORLD);
	
	if (Out_Mode==1)
		Geometry(Solid);
	else
		Geometry_b(Solid);

	if (mode_backup_ini==0)
	        init(rho,u,f,fg,F,Fg,rho_r,rhor,forcex,forcey,forcez,Psi_local,SupInv);
	else
	        Backup_init(rho, u, f,fg,F,Fg,rho_r, rhor, forcex,forcey,forcez,backup_rho, backup_velocity, backup_psi,backup_f,backup_g);


if (rank==0)
{
        cout<<endl;
        cout<<"INITIALIZATION COMPLETED"<<endl;
        cout<<endl;
}



//========================================================
char FileName[128];strcpy(FileName,outputfile);
char FileName2[128];strcpy(FileName2,outputfile);
char FileName3[128];strcpy(FileName3,outputfile);
char FileName4[128];strcpy(FileName4,outputfile);

strcat(FileName,"Results.txt");
strcat(FileName2,"Permeability.txt");
strcat(FileName4,"Velocity_ave_max.txt");
strcat(FileName3,"bodyforce.txt");
//========================================================



ofstream fins;	
	fins.open(FileName,ios::trunc);
	fins.close();

if (wr_per==1)
	{
	fins.open(FileName2,ios::trunc);
	fins.close();
	}


	fins.open(FileName3,ios::out);
	fins.close();

	fins.open(FileName4,ios::out);
	fins.close();
	
	for(n=0;n<=n_max;n++)
	{
	
	

	collision(rho,u,f,F,fg,Fg,rho_r,rhor,forcex,forcey,forcez,SupInv,Solid,Sl,Sr);

	
	if ((1-pre_xp)*(1-pre_xn)*(1-pre_yp)*(1-pre_yn)*(1-pre_zp)*(1-pre_zn)==0)
		boundary_pressure(pre_xp,p_xp,pre_xn,p_xn,pre_yp,p_yp,pre_yn,p_yn,pre_zp,p_zp,pre_zn,p_zn,f,F,u,rho,Solid);

	
	if ((1-vel_xp)*(1-vel_xn)*(1-vel_yp)*(1-vel_yn)*(1-vel_zp)*(1-vel_zn)==0)
		boundary_velocity(vel_xp,v_xp,vel_xn,v_xn,vel_yp,v_yp,vel_yn,v_yn,vel_zp,v_zp,vel_zn,v_zn,f,F,rho,u,Solid);

	if ((1-sol_c_xp)*(1-sol_c_xn)*(1-sol_c_yp)*(1-sol_c_yn)*(1-sol_c_zp)*(1-sol_c_zn)==0)
	Solute_Constant_BC(sol_c_xp,c_xp,sol_c_xn,c_xn,sol_c_yp,c_yp,sol_c_yn,c_yn,sol_c_zp,c_zp,sol_c_zn,c_zn,Fg,Solid,rho_r,u,fg);
	
	if ((1-sol_zf_xp)*(1-sol_zf_xn)*(1-sol_zf_yp)*(1-sol_zf_yn)*(1-sol_zf_zp)*(1-sol_zf_zn)==0)
	Solute_ZeroFlux_BC(sol_zf_xp,zf_xp,sol_zf_xn,zf_xn,sol_zf_yp,zf_yp,sol_zf_yn,zf_yn,sol_zf_zp,zf_zp,sol_zf_zn,zf_zn,Fg,Solid,rho_r,u,fg);
  		 
		comput_macro_variables(rho,u,u0,f,F,fg,Fg,rho_r,rhor,forcex,forcey,forcez,SupInv,Solid,Psi_local);
 
	if (n==ini_psi)
	        psi_reset(u,fg,rho_r,rhor,Psi_local,SupInv);

	
	
	if(n%freRe==0)
		{       
			
		
			if (rank==0)
			{
			finish = MPI_Wtime();
			ofstream fin(FileName,ios::out);
			
			fin<<"The"<<n-freRe<<"th computation result:"<<endl;
			fin<<"The Maximum velocity is: "<<setprecision(6)<<u_max<<"   Re="<<Re<<endl;
			fin<<"Peclet Number="<<u_max*dx/niu_s<<"     Courant Number="<<u_max*dt/dx<<endl;
			fin<<"The max relative error of velocity is: "
				<<setiosflags(ios::scientific)<<error<<endl;
				fin<<"The averaged velocity is: "<<u_ave<<endl;
			fin<<"The permiability is: "<<Permia[0]*reso*reso*1000<<", "<<Permia[1]*reso*reso*1000<<", "<<Permia[2]*reso*reso*1000<<endl;
			fin<<"The relative error of permiability computing is: "<<error_perm<<endl;
			fin<<"Elapsed time is "<< the<<"h"<<tme<<"m"<<tse<<"s"<<endl;
			fin<<"The expected completion time is "<<th<<"h"<<tm<<"m"<<ts<<"s"<<endl;
			fin<<endl;
			fin.close();
			}

			error=Error(u,u0,&u_max,&u_ave);
			if (u_max>=10.0)	U_max_ref+=1;
			
			error_perm=Comput_Perm(u,Permia,PerDir,SupInv);
			
			if (rank==0)
			{
			finish = MPI_Wtime();
			ofstream fin(FileName,ios::app);
			fin<<"The"<<n<<"th computation result:"<<endl;

			Re=u_ave*(NY+1)/(1.0/3.0*(1/s_v-0.5));
		//=============================================================================================
		        remain=(n_max-n)*((finish-start)/n);
			th=int(remain/3600);
			tm=int((remain-th*3600)/60);
			ts=int(remain-(th*3600+tm*60));

			elaps=finish-start;
			the=int(elaps/3600);
			tme=int((elaps-the*3600)/60);
			tse=int(elaps-(the*3600+tme*60));
		//==============================================================================================
		        fin<<"The Maximum velocity is: "<<setprecision(6)<<u_max<<"   Re="<<Re<<endl;
			fin<<"Peclet Number="<<u_max*dx/niu_s<<"     Courant Number="<<u_max*dt/dx<<endl;
			fin<<"The max relative error of velocity is: "
				<<setiosflags(ios::scientific)<<error<<endl;
				fin<<"The averaged velocity is: "<<u_ave<<endl;
			fin<<"The permiability is: "<<Permia[0]*reso*reso*1000<<", "<<Permia[1]*reso*reso*1000<<", "<<Permia[2]*reso*reso*1000<<endl;
			fin<<"The relative error of permiability computing is: "<<error_perm<<endl;
			fin<<"Elapsed time is "<< the<<"h"<<tme<<"m"<<tse<<"s"<<endl;
			fin<<"The expected completion time is "<<th<<"h"<<tm<<"m"<<ts<<"s"<<endl;
			fin<<endl;
			fin.close();
		//==============================================================================================


		if (wr_per==1)
			{
			ofstream finfs(FileName2,ios::app);
			switch(PerDir)
				{
				case 1:
				finfs<<Permia[0]*reso*reso*1000<<" "<<error_perm<<endl;break;
				case 2:
				finfs<<Permia[1]*reso*reso*1000<<" "<<error_perm<<endl;break;
				case 3:
				finfs<<Permia[2]*reso*reso*1000<<" "<<error_perm<<endl;break;
				default:
				finfs<<Permia[0]*reso*reso*1000<<" "<<error_perm<<endl;break;
				}
			finfs.close();
			}
	
		
			//==========for bodyforce output===========
			//ofstream finf3(FileName3,ios::app);
			//finf3<<gx<<endl;
			//finf3.close();
			
			ofstream finf4(FileName4,ios::app);
			finf4<<u_ave<<" "<<u_max<<" "<<error<<endl;
			finf4.close();
		

			//cout<<"The"<<n<<"th computation result:"<<endl;
		//=============================================================================================
			//cout<<"The permiability is: "<<Permia[0]*reso*reso*1000<<", "<<Permia[1]*reso*reso*1000<<", "<<Permia[2]*reso*reso*1000<<endl;
			//cout<<"The relative error of permiability computing is: "<<error_perm<<endl;
		//==============================================================================================

		//==============================================================================================
			cout<<"The"<<n<<"th computation result:"<<endl;
			cout<<"The Density of point(NX/2,NY/2,NZ/2) is: "<<setprecision(6)
				<<rho[Solid[(nx_l/2)][NY/2][NZ/2]]<<endl;
			cout<<"The permiability is: "<<Permia[0]*reso*reso*1000<<", "<<Permia[1]*reso*reso*1000<<", "<<Permia[2]*reso*reso*1000<<endl;
			cout<<"The Maximum velocity is: "<<setprecision(6)<<u_max<<"   Re="<<Re<<endl;
			cout<<"Peclet Number="<<u_max*dx/niu_s<<"     Courant Number="<<u_max*dt/dx<<endl;
			
			cout<<"The max relative error of uv is: "
				<<setiosflags(ios::scientific)<<error<<endl;
			
			cout<<"The relative error of permiability computing is: "<<error_perm<<endl;
			cout<<"Elapsed time is "<< the<<"h"<<tme<<"m"<<tse<<"s"<<endl;
			cout<<"The expected completion time is "<<th<<"h"<<tm<<"m"<<ts<<"s"<<endl;
			cout<<endl;
			}
			
			if ((freDe>=0) and (n%freDe==0))
				if (Out_Mode==1)
					output_density(n,rho,mirX,mirY,mirZ,mir,Solid);
				else
					output_density_b(n,rho,mirX,mirY,mirZ,mir,Solid);

			if ((freVe>=0) and (n%freVe==0))
				if (Out_Mode==1)
					output_velocity(n,rho,u,mirX,mirY,mirZ,mir,Solid);
				else
					output_velocity_b(n,rho,u,mirX,mirY,mirZ,mir,Solid);
			
			//===================================	
			if ((frePsi>=0) and (n%frePsi==0))
				if (Out_Mode==1)
					output_psi(n,rho_r,mirX,mirY,mirZ,mir,Solid);
				else
					output_psi_b(n,rho_r,mirX,mirY,mirZ,mir,Solid);
				
			if ((freDis>=0) and (n%freDis==0) and (Sub_Dis>0))	
			                {
					Statistical_psi(rho_r,n,Solid);
					//==========for bodyforce output===========
					if ((rank==0) and (n>ini_psi))
					{
					ofstream finf3(FileName3,ios::app);
					finf3<<Dispersion<<"  "<<disp_pre<<endl;
					finf3.close();
					}
					//=========================================
					}
		
			//===================================
			
			
			if ((fre_backup>=0) and (n%fre_backup==0)  and (n>0))
			        Backup(n,rho,rho_r,u,f,fg);
			
			
			if(error!=error) {cout<<"PROGRAM STOP"<<endl;break;};
			if(U_max_ref>=5) {cout<<"PROGRAM STOP DUE TO HIGH VELOCITY"<<endl;break;}
		}	
	}


	MPI_Barrier(MPI_COMM_WORLD);
	if (fre_backup>=0)
			        Backup(n_max,rho,rho_r,u,f,fg);
			
/*			
	for (int i=0;i<=Count;i++)
		{
		delete [] u[i];
		delete [] u0[i];
		delete [] f[i];
		delete [] F[i];
		delete [] fg[i];
		delete [] Fg[i];
		}
	
	delete [] f;
	delete [] fg;
	delete [] Fg;
	delete [] u;
	delete [] F;
	delete [] u0;
	delete [] rho;
	delete [] rho_r;
	
	delete [] rhor;
	
	delete [] forcex;
	delete [] forcey;
	delete [] forcez;
	delete [] SupInv;

	delete [] Sl;
	delete [] Sr;

	delete [] Permia;
//	delete [] Count;
*/

	finish = MPI_Wtime();
	
	
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank==0)
			{
    			cout<<"Elapsed time is "<< the<<"h"<<tme<<"m"<<tse<<"s"<<endl;
    			cout<<"Accuracy: "<<MPI_Wtick()<<" Second"<<endl;
			ofstream fin(FileName,ios::app);
			fin<<"Elapsed time is "<< the<<"h"<<tme<<"m"<<tse<<"s"<<endl;
			fin<<"Accuracy: "<<MPI_Wtick()<<" Second"<<endl;
			}

	MPI :: Finalize ();

	
}


int inverse(mat &a){
    double t;
    int i,j,k,is[MAXN],js[MAXN];
    if(a.n!=a.m) return 0;
    for(k=0;k<a.n;k++){
        for(t=0,i=k;i<a.n;i++)
            for(j=k;j<a.n;j++)
                if(fabs(a.data[i][j])>t)
                    t=fabs(a.data[is[k]=i][js[k]=j]);
        if(zero(t)) return 0;
        if(is[k]!=k)
            for(j=0;j<a.n;j++)
                t=a.data[k][j],a.data[k][j]=a.data[is[k]][j],a.data[is[k]][j]=t;
        if(js[k]!=k)
            for(i=0;i<a.n;i++)
                t=a.data[i][k],a.data[i][k]=a.data[i][js[k]],a.data[i][js[k]]=t;
        a.data[k][k]=1/a.data[k][k];
        for(j=0;j<a.n;j++)
            if(j!=k)
                a.data[k][j]*=a.data[k][k];
        for(i=0;i<a.n;i++)
            if(i!=k)
                for(j=0;j<a.n;j++)
                    if(j!=k)
                        a.data[i][j]-=a.data[i][k]*a.data[k][j];
        for(i=0;i<a.n;i++)
            if(i!=k)
                a.data[i][k]*=-a.data[k][k];
    }
    for(k=a.n-1;k>=0;k--){
        for(j=0;j<a.n;j++)
            if(js[k]!=k)
                t=a.data[k][j],a.data[k][j]=a.data[js[k]][j],a.data[js[k]][j]=t;
        for(i=0;i<a.n;i++)
            if(is[k]!=k)
                t=a.data[i][k],a.data[i][k]=a.data[i][is[k]],a.data[i][is[k]]=t;
    }
    return 1;
}


void Comput_MI(double M[19][19], double MI[19][19])
{

double mim[19][19];

mat a;
    int i,j;
    int n_s=19;
        for(int i=0;i<n_s;i++)
            for(int j=0;j<n_s;j++)
                a.data[i][j]=M[i][j];
	a.m=a.n=n_s;
        if(inverse(a))
            for(int i=0;i<n_s;i++)
                for(int j=0;j<n_s;j++)
                    MI[i][j]=a.data[i][j];
               
            
        else
            puts("NO");


}





void Parallelize_Geometry()
{
        int rank = MPI :: COMM_WORLD . Get_rank ();
	int mpi_size=MPI :: COMM_WORLD . Get_size ();
	MPI_Status status;
	MPI_Request request;

int nx_g[mpi_size];
int disp[mpi_size];
int pore;
float pore2;
int loc_por[NX+1];
int sum=0;
int sum2=0;
double ave_nx;
int nx_pre,nx_aft,n_i,sum_nx;

//====================
int* Solid_rank0;
float* Psi_rank0;
int* recv_solid;
float* recv_psi;

int bufsize[mpi_size];
int bufloc[mpi_size];
//=====================


	for (int i=0;i<=NX;i++)
	        loc_por[i]=0;
	
	if (par_per_x==0)
		{per_xn=0;per_xp=NX;}
	if (par_per_y==0)
		{per_yn=0;per_yp=NY;}
	if (par_per_z==0)
		{per_zn=0;per_zp=NZ;}
	
if (rank==0)
{
	FILE *ftest;
	ifstream fin;
	
	ftest = fopen(filename, "r");

	if(ftest == NULL)
	{
		cout << "\n The pore geometry file (" << filename <<
			") does not exist!!!!\n";
		cout << " Please check the file\n\n";

		exit(0);
	}
	fclose(ftest);

	Solid_rank0 = new int[(NX+1)*(NY+1)*(NZ+1)];
	
	fin.open(filename);
	for(int k=0 ; k<=NZ ; k++)
	for(int j=0 ; j<=NY ; j++)
	for(int i=0 ; i<=NX ; i++)
	
	{
		while(true)
		{	
			fin >> pore;
			if( pore == 0.0 || pore == 1.0) break;
		}
		
		Solid_rank0[i*(NY+1)*(NZ+1)+j*(NZ+1)+k]=pore;
		if (pore==0)
		        {
		                sum+=1;
		                loc_por[i]+=1;
		                if ((i>=per_xn) and (i<=per_xp) and (j>=per_yn) and (j<=per_yp) and (k>=per_zn) and (k<=per_zp))
		                        sum2+=1;
		        }
	}
	fin.close();
}

        MPI_Bcast(loc_por,NX+1,MPI_INT,0,MPI_COMM_WORLD);
        MPI_Bcast(&sum,1,MPI_INT,0,MPI_COMM_WORLD);
        
   
         
	nx_pre=0;nx_aft=0;sum_nx=0;
	ave_nx=(double)sum/(mpi_size);
	disp[0]=0;
	bufloc[0]=0;   //===========
	porosity=(double)sum2/((per_xp-per_xn+1)*(per_yp-per_yn+1)*(per_zp-per_zn+1));
	
	
	for (int i=0;i<mpi_size-1;i++)
	        {
	        nx_pre=0;nx_aft=0;n_i=0;
	        while (nx_aft<ave_nx*(i+1))
	                {
	                nx_pre=nx_aft;
	                nx_aft+=loc_por[n_i];
	                n_i+=1;
	                }
	        if ((double)(nx_aft-ave_nx*(i+1))>(double)(ave_nx*(i+1)-nx_pre))
	                disp[i+1]=n_i-1;
	        else
	                disp[i+1]=n_i;
	        
	        nx_g[i]=disp[i+1]-disp[i];
	  
	        //======================
	        bufsize[i]=nx_g[i]*(NY+1)*(NZ+1);
	        bufloc[i+1]=bufloc[i]+bufsize[i];
	        //=======================
	        
	        }
	  
	  nx_g[mpi_size-1]=(NX+1)-disp[mpi_size-1];
	  bufsize[mpi_size-1]=nx_g[mpi_size-1]*(NY+1)*(NZ+1);//======
	  
	  nx_l=nx_g[rank];
	  
	

//===========================================	  
	    Solid = new int**[nx_l];
	Psi_local = new double**[nx_l];
	for (int i=0;i<nx_l;i++)
		{
		Solid[i] = new int*[NY+1];
		Psi_local[i] = new double*[NY+1];
			for (int j=0;j<=NY;j++)
			{
			Solid[i][j]= new int[NZ+1];
			Psi_local[i][j] = new double[NZ+1];
			}
		}
	  
	  
		recv_solid = new int[nx_l*(NY+1)*(NZ+1)];
		recv_psi = new float[nx_l*(NY+1)*(NZ+1)];
//========================================
		

MPI_Barrier(MPI_COMM_WORLD);

        
        MPI_Scatterv(Solid_rank0,bufsize,bufloc,MPI_INT,recv_solid,nx_l*(NY+1)*(NZ+1),MPI_INT,0,MPI_COMM_WORLD);

	cout<<"GEOMETRY INPUT FILE PARTITIONING FOR PARALLEL READING DONE   Processor No."<<rank<<endl;
	//cout<<endl;




    
 
 
 if (rank==0)
{       
        FILE *ftest;
	ftest = fopen(filenamepsi, "r");
	ifstream fin;
	if(ftest == NULL)
	{
		cout << "\n The Concentration file (" << filenamepsi <<
			") does not exist!!!!\n";
		cout << " Please check the file\n\n";

		exit(0);
	}
	fclose(ftest);

	Psi_rank0 = new float[(NX+1)*(NY+1)*(NZ+1)];
	
	fin.open(filenamepsi);
	for(int k=0 ; k<=NZ ; k++)
	for(int j=0 ; j<=NY ; j++)
	for(int i=0 ; i<=NX ; i++)
	
	{
	
			fin >> pore2;
			Psi_rank0[i*(NY+1)*(NZ+1)+j*(NZ+1)+k]=pore2;
	
	
	}
	fin.close();
	
}


	
MPI_Barrier(MPI_COMM_WORLD);	
	
        MPI_Scatterv(Psi_rank0,bufsize,bufloc,MPI_FLOAT,recv_psi,nx_l*(NY+1)*(NZ+1),MPI_FLOAT,0,MPI_COMM_WORLD);
	        
	        
	 cout<<"CONCENTRATION FILE PARTITIONING FOR PARALLEL READING DONE   Processor No."<<rank<<endl;
//	cout<<endl;
   
MPI_Barrier(MPI_COMM_WORLD);		        
	        
	        
	if (rank==0)
{	
//        for (int i=0;i<nx_l;i++)
//	                for (int j=0;j<=NY;j++)
//	                for (int k=0;k<=NZ;k++)
//	                {
//	                        recv_solid[i*(NY+1)*(NZ+1)+j*(NZ+1)+k]=Solid_rank0[i*(NY+1)*(NZ+1)+j*(NZ+1)+k];
//	                        recv_psi[i*(NY+1)*(NZ+1)+j*(NZ+1)+k]=Psi_rank0[i*(NY+1)*(NZ+1)+j*(NZ+1)+k];
//	                }
	       delete [] Solid_rank0;	
	       delete [] Psi_rank0;  
	
	       
	
}

	//-------------------------
	double psi_total_g[mpi_size];
	psi_total=0;
	//-------------------------

	for (int i=0;i<nx_l;i++)
	                for (int j=0;j<=NY;j++)
	                for (int k=0;k<=NZ;k++)
	                {
	                Solid[i][j][k]=recv_solid[i*(NY+1)*(NZ+1)+j*(NZ+1)+k];
	                Psi_local[i][j][k]=recv_psi[i*(NY+1)*(NZ+1)+j*(NZ+1)+k];
			if (Solid[i][j][k]==0)
	                	psi_total+=recv_psi[i*(NY+1)*(NZ+1)+j*(NZ+1)+k];
	                }

	MPI_Barrier(MPI_COMM_WORLD);	
	           
	MPI_Gather(&psi_total,1,MPI_DOUBLE,psi_total_g,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(psi_total_g,mpi_size,MPI_DOUBLE,0,MPI_COMM_WORLD);


	psi_total=0;
	for (int i=0;i<mpi_size;i++)
		psi_total+=psi_total_g[i];

	//cout<<psi_total<<"  "<<rank<<endl;;


	 delete [] recv_psi;
	 delete [] recv_solid;
       
}



void init_Sparse_read_rock_parallel(int* Sl,int* Sr)
{	
MPI_Status status[4] ;
MPI_Request request[4];

	
	
	int rank = MPI :: COMM_WORLD . Get_rank ();
	int mpi_size=MPI :: COMM_WORLD . Get_size ();

bool mark;
int kk,ip,jp,kp,mean_l,mpi_test,s_c;

int pore;
double pore2;

	

	int* Sl_send;
	int* Sr_send;
	Sl_send = new int[(NY+1)*(NZ+1)];
	Sr_send = new int[(NY+1)*(NZ+1)];

	Count=1;




for(int i=0;i<nx_l;i++)	
	for(int j=0;j<=NY;j++)
		for(int k=0;k<=NZ;k++)
		{
		      
			if (Solid[i][j][k]==0)
				{
				Solid[i][j][k]=Count;
				Count++;
				}
			else
			{
				Solid[i][j][k]=0;
			}

		}

	
	
	Count-=1;
	
	cl=0;cr=0;	
	
	for (int j=0;j<=NY;j++)
		for (int k=0;k<=NZ;k++)
		{
			
			if (Solid[0][j][k]>0)
				{
				cl++;
				Sl_send[j*(NZ+1)+k]=cl;
				}
			else
				Sl_send[j*(NZ+1)+k]=0;
			

		
			if (Solid[nx_l-1][j][k]>0)
				{
				cr++;
				Sr_send[j*(NZ+1)+k]=cr;
				}
			else
				Sr_send[j*(NZ+1)+k]=0;
			
		}




if (rank==0)
		{
		MPI_Isend(Sr_send, (NY+1)*(NZ+1), MPI_INT, rank+1, rank*2+1, MPI_COMM_WORLD,&request[0]);
      		MPI_Isend(Sl_send, (NY+1)*(NZ+1), MPI_INT, mpi_size-1, rank*2, MPI_COMM_WORLD,&request[1]);
		MPI_Irecv(Sr , (NY+1)*(NZ+1), MPI_INT, rank+1, (rank+1)*2, MPI_COMM_WORLD,&request[2]);		
      		MPI_Irecv(Sl, (NY+1)*(NZ+1), MPI_INT, mpi_size-1, (mpi_size-1)*2+1, MPI_COMM_WORLD,&request[3] );
		
		}
		else
		if (rank==mpi_size-1)
			{
			MPI_Isend(Sl_send, (NY+1)*(NZ+1), MPI_INT, rank-1, rank*2, MPI_COMM_WORLD,&request[0]);
      			MPI_Isend(Sr_send, (NY+1)*(NZ+1), MPI_INT, 0, rank*2+1, MPI_COMM_WORLD,&request[1]);
			MPI_Irecv(Sl, (NY+1)*(NZ+1), MPI_INT, rank-1, (rank-1)*2+1, MPI_COMM_WORLD,&request[2] );
      			MPI_Irecv(Sr, (NY+1)*(NZ+1), MPI_INT, 0, 0, MPI_COMM_WORLD,&request[3]);
			
			}
			else
			{
			MPI_Isend(Sl_send, (NY+1)*(NZ+1), MPI_INT, rank-1, rank*2, MPI_COMM_WORLD,&request[0]);
      			MPI_Isend(Sr_send, (NY+1)*(NZ+1), MPI_INT, rank+1, rank*2+1, MPI_COMM_WORLD,&request[1]);
			MPI_Irecv(Sl, (NY+1)*(NZ+1), MPI_INT, rank-1, (rank-1)*2+1, MPI_COMM_WORLD,&request[2]);
      			MPI_Irecv(Sr, (NY+1)*(NZ+1), MPI_INT, rank+1, (rank+1)*2, MPI_COMM_WORLD,&request[3]);
			
			};

	
	MPI_Waitall(4,request, status);
	MPI_Testall(4,request,&mpi_test,status);


	

	delete [] Sl_send;
	delete [] Sr_send;		
}

void Suppliment(int* SupInv,int*** Solid)
{

	int rank = MPI :: COMM_WORLD . Get_rank ();
	int mpi_size=MPI :: COMM_WORLD . Get_size ();

	for(int i=0;i<nx_l;i++)	
		for(int j=0;j<=NY;j++)
			for(int k=0;k<=NZ;k++)	
			if (Solid[i][j][k]>0)
				SupInv[abs(Solid[i][j][k])]=i*(NY+1)*(NZ+1)+j*(NZ+1)+k;


}



void init(double* rho, double** u, double** f,double** fg, double** F, double** Fg, double* rho_r, double* rhor, double* forcex, double* forcey, double* forcez, double*** Psi_local, int* SupInv)
{	
      


	
	double usqr,vsqr;
	double c2,c4;
	
	
	rho0=1.0;dt=1.0/Zoom;dx=1.0/Zoom;
 
	if (lattice_v==1)
		{dx=dx_input;dt=dt_input;}

	lat_c=dx/dt;
	c_s=lat_c/sqrt(3);
	c_s2=lat_c*lat_c/3;

	c2=lat_c*lat_c;c4=c2*c2;
	
	//niu=niu;
	tau_f=niu/(c_s2*dt)+0.5;
	//tau_f=3.0*niu/dt+0.5;
	//tau_s=3.0*niu_s/dt+0.5;
	tau_s=niu_s/(c_s2*dt)+0.5;

	s_v=1/tau_f;
        
	double pr,eu; //raduis of the obstacles
       
	double s_other=8*(2-s_v)/(8-s_v);
	double u_tmp[3];





	if (lattice_v==1)
	{
	
	M_c[0]=1.0;
	M_c[1]=lat_c*lat_c;
	M_c[2]=lat_c*lat_c*lat_c*lat_c;
	M_c[3]=lat_c;
	M_c[4]=lat_c*lat_c*lat_c;
	M_c[5]=lat_c;
	M_c[6]=lat_c*lat_c*lat_c;
	M_c[7]=lat_c;	
	M_c[8]=lat_c*lat_c*lat_c;
	M_c[9]=lat_c*lat_c;
	M_c[10]=lat_c*lat_c*lat_c*lat_c;
	M_c[11]=lat_c*lat_c;
	M_c[12]=lat_c*lat_c*lat_c*lat_c;
	M_c[13]=lat_c*lat_c;
	M_c[14]=lat_c*lat_c;
	M_c[15]=lat_c*lat_c;
	M_c[16]=lat_c*lat_c*lat_c;
	M_c[17]=lat_c*lat_c*lat_c;
	M_c[18]=lat_c*lat_c*lat_c;

	for (int i=0;i<19;i++)
		for (int j=0;j<3;j++)
		elat[i][j]=e[i][j]*lat_c;
	
	for (int i=0;i<19;i++)
		for (int j=0;j<19;j++)
		M[i][j]*=M_c[i];

	Comput_MI(M,MI);

	}

//	for (int i=0;i<19;i++)
//		for (int j=0;j<3;j++)
//		elat[i][j]=e[i][j]*lat_c;
  //      cout<<elat[13][0]<<" "<<elat[13][1]<<" "<<elat[13][2]<<endl;        

	S[0]=0;
	S[1]=s_v;
	S[2]=s_v;
	S[3]=0;
	S[4]=s_other;
	S[5]=0;
	S[6]=s_other;
	S[7]=0;
	S[8]=s_other;
	S[9]=s_v;
	S[10]=s_v;
	S[11]=s_v;
	S[12]=s_v;
	S[13]=s_v;
	S[14]=s_v;
	S[15]=s_v;
	S[16]=s_other;
	S[17]=s_other;
	S[18]=s_other;


	

	
	for (int i=1;i<=Count;i++)	
			
		{
			u[i][0]=inivx;
			u[i][1]=inivy;
			u[i][2]=inivz;
			u_tmp[0]=u[i][0];
			u_tmp[1]=u[i][1];
			u_tmp[2]=u[i][2];
			rho_r[i]=Psi_local[(int)(SupInv[i]/((NY+1)*(NZ+1)))][(int)((SupInv[i]%((NY+1)*(NZ+1)))/(NZ+1))][SupInv[i]%(NZ+1)];
		
			rho[i]=1.0;
			//rho_r[i]=psi[i];
			
			rhor[i]=0;
			
			//cout<<u[i][0]<<" "<<u[i][1]<<" "<<u[i][2]<<endl;

			
			forcex[i]=gx;
			forcey[i]=gy+Gravity*Buoyancy_parameter*rho_r[i];
			forcez[i]=gz;

			//INITIALIZATION OF m and f

			for (int lm=0;lm<19;lm++)
			{
			f[i][lm]=feq(lm,rho[i],u_tmp);
			eu=elat[lm][0]*u[i][0]+elat[lm][1]*u[i][1]+elat[lm][2]*u[i][2];
			fg[i][lm]=w[lm]*rho_r[i]*(1+3*eu/c2);
			F[i][lm]=f[i][lm];Fg[i][lm]=fg[i][lm];
			//fg[i][lm]=w[mi]*rho_r[ci]*(1+3*eu/c2+4.5*eu*eu/c4-1.5*uu/c2);
			}
                	

			
		
		

	}

	

	 	
}


double feq(int k,double rho, double u[3])
{

	double ux,uy,uz;
	double eu,uv,feq;
        double c2,c4,ls;
	double rho_0=1.0;
	
	c2=lat_c*lat_c;c4=c2*c2;
	eu=(elat[k][0]*u[0]+elat[k][1]*u[1]+elat[k][2]*u[2]);
	uv=(u[0]*u[0]+u[1]*u[1]+u[2]*u[2]);// WITH FORCE TERM:GRAVITY IN X DIRECTION
	feq=w[k]*rho*(1.0+3.0*eu/c2+4.5*eu*eu/c4-1.5*uv/c2);

	
			ux=u[0];
			uy=u[1];
			uz=u[2];

	for(int s=0;s<19;s++)
		meq[s]=0;
			meq[0]=rho;meq[3]=rho_0*ux;meq[5]=rho_0*uy;meq[7]=rho_0*uz;
			meq[1]=rho_0*(ux*ux+uy*uy+uz*uz);
			meq[9]=rho_0*(2*ux*ux-uy*uy-uz*uz);
			meq[11]=rho_0*(uy*uy-uz*uz);
			meq[13]=rho_0*ux*uy;meq[14]=rho_0*uy*uz;
			meq[15]=rho_0*ux*uz;

	feq=0;
	for (int j=0;j<19;j++)
		feq+=MI[k][j]*meq[j];		





	return feq;

}


double feq_psi(int k,double rho, double u[3])
{

	double ux,uy,uz;
	double eu,uv,feq;
        double c2,c4,ls;
	double rho_0=1.0;
	
	c2=lat_c*lat_c;c4=c2*c2;
	eu=(elat[k][0]*u[0]+elat[k][1]*u[1]+elat[k][2]*u[2]);
	uv=(u[0]*u[0]+u[1]*u[1]+u[2]*u[2]);
	feq=w[k]*rho*(1.0+3.0*eu/c2+4.5*eu*eu/c4-1.5*uv/c2);
	return feq;
}


void collision(double* rho,double** u,double** f,double** F, double** fg, double** Fg, double* rho_r, double* rhor, double* forcex,double* forcey, double* forcez,int* SupInv,int*** Solid,int* Sl, int* Sr)
{
	MPI_Status status2[8] ;
	MPI_Request request2[8];
	int mpi_test2;
	
	int rank = MPI :: COMM_WORLD . Get_rank ();
	int mpi_size=MPI :: COMM_WORLD . Get_size ();

	const double c_l=lat_c;	

double g_r[19];
double rho_0=1.0;
double lm0,lm1,cc,sum,uu;
double ux,uy,uz,nx,ny,nz;
double usqr,vsqr,eu,ef,cospsi,s_other;
double F_hat[19],GuoF[19],f_eq[19],u_tmp[3];
double m_l[19],m_inv_l[19];
int i,j,m;
int interi,interj,interk,ip,jp,kp;
double c2,c4;

	c2=lat_c*lat_c;c4=c2*c2;

	int* Gcl = new int[mpi_size];
	int* Gcr = new int[mpi_size];

	
	MPI_Gather(&cl,1,MPI_INT,Gcl,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(Gcl,mpi_size,MPI_INT,0,MPI_COMM_WORLD);

	MPI_Gather(&cr,1,MPI_INT,Gcr,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(Gcr,mpi_size,MPI_INT,0,MPI_COMM_WORLD);

	
	double* sendl_s;
	double* sendr_s;
	
	
	double* sendl;
	double* sendr;

	double* recvl= new double[Gcl[rank]*5];
	double* recvr = new double[Gcr[rank]*5];


	
	double* recvl_s= new double[Gcl[rank]*5];
	double* recvr_s = new double[Gcr[rank]*5];
	

if (rank==0)
		{
		
		sendl_s = new double[Gcr[mpi_size-1]*5];
		sendr_s = new double[Gcl[rank+1]*5];

		sendl = new double[Gcr[mpi_size-1]*5];
		sendr = new double[Gcl[rank+1]*5];


		for (int ka=0;ka<Gcr[mpi_size-1];ka++)
				for (int kb=0;kb<5;kb++)
				{sendl[ka*5+kb]=-1;sendl_s[ka*5+kb]=-1;}
		                
		 for (int ka=0;ka<Gcl[rank+1];ka++)
		                for (int kb=0;kb<5;kb++)
				{sendr[ka*5+kb]=-1;sendr_s[ka*5+kb]=-1;}
  
		
		
		}	
		else
		if (rank==mpi_size-1)
			{
			
			sendl_s = new double[Gcr[rank-1]*5];
			sendr_s = new double[Gcl[0]*5];
			
			sendl = new double[Gcr[rank-1]*5];
			sendr = new double[Gcl[0]*5];


			for (int ka=0;ka<Gcr[rank-1];ka++)
				for (int kb=0;kb<5;kb++)
				{sendl[ka*5+kb]=-1;sendl_s[ka*5+kb]=-1;}
		                

		        for (int ka=0;ka<Gcl[0];ka++)
				for (int kb=0;kb<5;kb++)
			   	{sendr[ka*5+kb]=-1;sendr_s[ka*5+kb]=-1;}
			
			
			}
			else
			{
			
			sendl_s = new double[Gcr[rank-1]*5];
			sendr_s = new double[Gcl[rank+1]*5];
			
			sendl = new double[Gcr[rank-1]*5];
			sendr = new double[Gcl[rank+1]*5];

			for (int ka=0;ka<Gcr[rank-1];ka++)
				for (int kb=0;kb<5;kb++)
				{sendl[ka*5+kb]=-1;sendl_s[ka*5+kb]=-1;}

		        for (int ka=0;ka<Gcl[rank+1];ka++)
				for (int kb=0;kb<5;kb++)
				{sendr[ka*5+kb]=-1;sendr_s[ka*5+kb]=-1;}
			
			}


		
MPI_Barrier(MPI_COMM_WORLD);


	for(int ci=1;ci<=Count;ci++)	
	

		{	
				i=(int)(SupInv[ci]/((NY+1)*(NZ+1)));
				j=(int)((SupInv[ci]%((NY+1)*(NZ+1)))/(NZ+1));
				m=(int)(SupInv[ci]%(NZ+1));   

			forcex[ci]=gx;
			forcey[ci]=gy+Gravity*Buoyancy_parameter*(rho_r[ci]-ref_psi);
			forcez[ci]=gz;
		
			//=================FORCE TERM_GUO=========================================
/*
//=====================================
			for (int k=0;k<19;k++)
			{	
			lm0=((elat[k][0]-u[ci][0])*forcex[ci]+(elat[k][1]-u[ci][1])*forcey[ci]+(elat[k][2]-u[ci][2])*forcez[ci])/c_s2;
			lm1=(elat[k][0]*u[ci][0]+elat[k][1]*u[ci][1]+elat[k][2]*u[ci][2])*(elat[k][0]*forcex[ci]+elat[k][1]*forcey[ci]+elat[k][2]*forcez[ci])/(c_s2*c_s2);
			GuoF[k]=w[k]*(lm0+lm1);
			//GuoF[k]=0.0;
			}
//=======================================
*/


//==========================
lm0=((+0.000*c_l-u[ci][0])*forcex[ci]+(+0.000*c_l-u[ci][1])*forcey[ci]+(+0.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(+0.000*c_l*u[ci][0]+(+0.000*c_l)*u[ci][1]+(+0.000*c_l)*u[ci][2])*(+0.000*c_l*forcex[ci]+(+0.000*c_l)*forcey[ci]+(+0.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[0]=w[0]*(lm0+lm1);

lm0=((+1.000*c_l-u[ci][0])*forcex[ci]+(+0.000*c_l-u[ci][1])*forcey[ci]+(+0.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(+1.000*c_l*u[ci][0]+(+0.000*c_l)*u[ci][1]+(+0.000*c_l)*u[ci][2])*(+1.000*c_l*forcex[ci]+(+0.000*c_l)*forcey[ci]+(+0.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[1]=w[1]*(lm0+lm1);

lm0=((-1.000*c_l-u[ci][0])*forcex[ci]+(+0.000*c_l-u[ci][1])*forcey[ci]+(+0.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(-1.000*c_l*u[ci][0]+(+0.000*c_l)*u[ci][1]+(+0.000*c_l)*u[ci][2])*(-1.000*c_l*forcex[ci]+(+0.000*c_l)*forcey[ci]+(+0.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[2]=w[2]*(lm0+lm1);

lm0=((+0.000*c_l-u[ci][0])*forcex[ci]+(+1.000*c_l-u[ci][1])*forcey[ci]+(+0.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(+0.000*c_l*u[ci][0]+(+1.000*c_l)*u[ci][1]+(+0.000*c_l)*u[ci][2])*(+0.000*c_l*forcex[ci]+(+1.000*c_l)*forcey[ci]+(+0.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[3]=w[3]*(lm0+lm1);

lm0=((+0.000*c_l-u[ci][0])*forcex[ci]+(-1.000*c_l-u[ci][1])*forcey[ci]+(+0.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(+0.000*c_l*u[ci][0]+(-1.000*c_l)*u[ci][1]+(+0.000*c_l)*u[ci][2])*(+0.000*c_l*forcex[ci]+(-1.000*c_l)*forcey[ci]+(+0.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[4]=w[4]*(lm0+lm1);

lm0=((+0.000*c_l-u[ci][0])*forcex[ci]+(+0.000*c_l-u[ci][1])*forcey[ci]+(+1.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(+0.000*c_l*u[ci][0]+(+0.000*c_l)*u[ci][1]+(+1.000*c_l)*u[ci][2])*(+0.000*c_l*forcex[ci]+(+0.000*c_l)*forcey[ci]+(+1.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[5]=w[5]*(lm0+lm1);

lm0=((+0.000*c_l-u[ci][0])*forcex[ci]+(+0.000*c_l-u[ci][1])*forcey[ci]+(-1.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(+0.000*c_l*u[ci][0]+(+0.000*c_l)*u[ci][1]+(-1.000*c_l)*u[ci][2])*(+0.000*c_l*forcex[ci]+(+0.000*c_l)*forcey[ci]+(-1.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[6]=w[6]*(lm0+lm1);

lm0=((+1.000*c_l-u[ci][0])*forcex[ci]+(+1.000*c_l-u[ci][1])*forcey[ci]+(+0.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(+1.000*c_l*u[ci][0]+(+1.000*c_l)*u[ci][1]+(+0.000*c_l)*u[ci][2])*(+1.000*c_l*forcex[ci]+(+1.000*c_l)*forcey[ci]+(+0.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[7]=w[7]*(lm0+lm1);

lm0=((-1.000*c_l-u[ci][0])*forcex[ci]+(-1.000*c_l-u[ci][1])*forcey[ci]+(+0.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(-1.000*c_l*u[ci][0]+(-1.000*c_l)*u[ci][1]+(+0.000*c_l)*u[ci][2])*(-1.000*c_l*forcex[ci]+(-1.000*c_l)*forcey[ci]+(+0.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[8]=w[8]*(lm0+lm1);

lm0=((+1.000*c_l-u[ci][0])*forcex[ci]+(-1.000*c_l-u[ci][1])*forcey[ci]+(+0.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(+1.000*c_l*u[ci][0]+(-1.000*c_l)*u[ci][1]+(+0.000*c_l)*u[ci][2])*(+1.000*c_l*forcex[ci]+(-1.000*c_l)*forcey[ci]+(+0.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[9]=w[9]*(lm0+lm1);

lm0=((-1.000*c_l-u[ci][0])*forcex[ci]+(+1.000*c_l-u[ci][1])*forcey[ci]+(+0.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(-1.000*c_l*u[ci][0]+(+1.000*c_l)*u[ci][1]+(+0.000*c_l)*u[ci][2])*(-1.000*c_l*forcex[ci]+(+1.000*c_l)*forcey[ci]+(+0.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[10]=w[10]*(lm0+lm1);

lm0=((+1.000*c_l-u[ci][0])*forcex[ci]+(+0.000*c_l-u[ci][1])*forcey[ci]+(+1.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(+1.000*c_l*u[ci][0]+(+0.000*c_l)*u[ci][1]+(+1.000*c_l)*u[ci][2])*(+1.000*c_l*forcex[ci]+(+0.000*c_l)*forcey[ci]+(+1.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[11]=w[11]*(lm0+lm1);

lm0=((-1.000*c_l-u[ci][0])*forcex[ci]+(+0.000*c_l-u[ci][1])*forcey[ci]+(-1.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(-1.000*c_l*u[ci][0]+(+0.000*c_l)*u[ci][1]+(-1.000*c_l)*u[ci][2])*(-1.000*c_l*forcex[ci]+(+0.000*c_l)*forcey[ci]+(-1.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[12]=w[12]*(lm0+lm1);

lm0=((+1.000*c_l-u[ci][0])*forcex[ci]+(+0.000*c_l-u[ci][1])*forcey[ci]+(-1.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(+1.000*c_l*u[ci][0]+(+0.000*c_l)*u[ci][1]+(-1.000*c_l)*u[ci][2])*(+1.000*c_l*forcex[ci]+(+0.000*c_l)*forcey[ci]+(-1.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[13]=w[13]*(lm0+lm1);

lm0=((-1.000*c_l-u[ci][0])*forcex[ci]+(+0.000*c_l-u[ci][1])*forcey[ci]+(+1.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(-1.000*c_l*u[ci][0]+(+0.000*c_l)*u[ci][1]+(+1.000*c_l)*u[ci][2])*(-1.000*c_l*forcex[ci]+(+0.000*c_l)*forcey[ci]+(+1.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[14]=w[14]*(lm0+lm1);

lm0=((+0.000*c_l-u[ci][0])*forcex[ci]+(+1.000*c_l-u[ci][1])*forcey[ci]+(+1.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(+0.000*c_l*u[ci][0]+(+1.000*c_l)*u[ci][1]+(+1.000*c_l)*u[ci][2])*(+0.000*c_l*forcex[ci]+(+1.000*c_l)*forcey[ci]+(+1.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[15]=w[15]*(lm0+lm1);

lm0=((+0.000*c_l-u[ci][0])*forcex[ci]+(-1.000*c_l-u[ci][1])*forcey[ci]+(-1.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(+0.000*c_l*u[ci][0]+(-1.000*c_l)*u[ci][1]+(-1.000*c_l)*u[ci][2])*(+0.000*c_l*forcex[ci]+(-1.000*c_l)*forcey[ci]+(-1.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[16]=w[16]*(lm0+lm1);

lm0=((+0.000*c_l-u[ci][0])*forcex[ci]+(+1.000*c_l-u[ci][1])*forcey[ci]+(-1.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(+0.000*c_l*u[ci][0]+(+1.000*c_l)*u[ci][1]+(-1.000*c_l)*u[ci][2])*(+0.000*c_l*forcex[ci]+(+1.000*c_l)*forcey[ci]+(-1.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[17]=w[17]*(lm0+lm1);

lm0=((+0.000*c_l-u[ci][0])*forcex[ci]+(-1.000*c_l-u[ci][1])*forcey[ci]+(+1.000*c_l-u[ci][2])*forcez[ci])/c_s2;
lm1=(+0.000*c_l*u[ci][0]+(-1.000*c_l)*u[ci][1]+(+1.000*c_l)*u[ci][2])*(+0.000*c_l*forcex[ci]+(-1.000*c_l)*forcey[ci]+(+1.000*c_l)*forcez[ci])/(c_s2*c_s2);
GuoF[18]=w[18]*(lm0+lm1);

//====================


			
			//=====================equilibrium of moment=================================
			ux=u[ci][0];
			uy=u[ci][1];
			uz=u[ci][2];
			
			for(int k=0;k<19;k++)
				meq[k]=0;

			
			
			meq[0]=rho[ci];meq[3]=rho_0*ux;meq[5]=rho_0*uy;meq[7]=rho_0*uz;
			meq[1]=rho_0*(ux*ux+uy*uy+uz*uz);
			meq[9]=rho_0*(2*ux*ux-uy*uy-uz*uz);
			meq[11]=rho_0*(uy*uy-uz*uz);
			meq[13]=rho_0*ux*uy;
			meq[14]=rho_0*uy*uz;
			meq[15]=rho_0*ux*uz;
			
			
			



			//============FOR G COMPLEX BOUNDARY===============================================
			uu=u[ci][0]*u[ci][0]+u[ci][1]*u[ci][1]+u[ci][2]*u[ci][2];
			//==================================================================================



//==========================
m_l[0]=+1.000*1.0*f[ci][0]+1.00000000000000000*1.0*f[ci][1]+1.00000000000000000*1.0*f[ci][2]+1.00000000000000000*1.0*f[ci][3]+1.00000000000000000*1.0*f[ci][4]+1.00000000000000000*1.0*f[ci][5]+1.00000000000000000*1.0*f[ci][6]+1.00000000000000000*1.0*f[ci][7]+1.00000000000000000*1.0*f[ci][8]+1.00000000000000000*1.0*f[ci][9]+1.00000000000000000*1.0*f[ci][10]+1.00000000000000000*1.0*f[ci][11]+1.00000000000000000*1.0*f[ci][12]+1.00000000000000000*1.0*f[ci][13]+1.00000000000000000*1.0*f[ci][14]+1.00000000000000000*1.0*f[ci][15]+1.00000000000000000*1.0*f[ci][16]+1.00000000000000000*1.0*f[ci][17]+1.00000000000000000*1.0*f[ci][18];

F_hat[0]=+1.000*1.0*GuoF[0]+1.00000000000000000*1.0*GuoF[1]+1.00000000000000000*1.0*GuoF[2]+1.00000000000000000*1.0*GuoF[3]+1.00000000000000000*1.0*GuoF[4]+1.00000000000000000*1.0*GuoF[5]+1.00000000000000000*1.0*GuoF[6]+1.00000000000000000*1.0*GuoF[7]+1.00000000000000000*1.0*GuoF[8]+1.00000000000000000*1.0*GuoF[9]+1.00000000000000000*1.0*GuoF[10]+1.00000000000000000*1.0*GuoF[11]+1.00000000000000000*1.0*GuoF[12]+1.00000000000000000*1.0*GuoF[13]+1.00000000000000000*1.0*GuoF[14]+1.00000000000000000*1.0*GuoF[15]+1.00000000000000000*1.0*GuoF[16]+1.00000000000000000*1.0*GuoF[17]+1.00000000000000000*1.0*GuoF[18];

F_hat[0]*=(1-0.5*S[0]);
m_l[0]=m_l[0]-S[0]*(m_l[0]-meq[0])+dt*F_hat[0];
//=======================================

m_l[1]=-1.00000000000000000*c_l*c_l*f[ci][0]+0.00000000000000000*c_l*c_l*f[ci][1]+0.00000000000000000*c_l*c_l*f[ci][2]+0.00000000000000000*c_l*c_l*f[ci][3]+0.00000000000000000*c_l*c_l*f[ci][4]+0.00000000000000000*c_l*c_l*f[ci][5]+0.00000000000000000*c_l*c_l*f[ci][6]+1.00000000000000000*c_l*c_l*f[ci][7]+1.00000000000000000*c_l*c_l*f[ci][8]+1.00000000000000000*c_l*c_l*f[ci][9]+1.00000000000000000*c_l*c_l*f[ci][10]+1.00000000000000000*c_l*c_l*f[ci][11]+1.00000000000000000*c_l*c_l*f[ci][12]+1.00000000000000000*c_l*c_l*f[ci][13]+1.00000000000000000*c_l*c_l*f[ci][14]+1.00000000000000000*c_l*c_l*f[ci][15]+1.00000000000000000*c_l*c_l*f[ci][16]+1.00000000000000000*c_l*c_l*f[ci][17]+1.00000000000000000*c_l*c_l*f[ci][18];

F_hat[1]=-1.00000000000000000*c_l*c_l*GuoF[0]+0.00000000000000000*c_l*c_l*GuoF[1]+0.00000000000000000*c_l*c_l*GuoF[2]+0.00000000000000000*c_l*c_l*GuoF[3]+0.00000000000000000*c_l*c_l*GuoF[4]+0.00000000000000000*c_l*c_l*GuoF[5]+0.00000000000000000*c_l*c_l*GuoF[6]+1.00000000000000000*c_l*c_l*GuoF[7]+1.00000000000000000*c_l*c_l*GuoF[8]+1.00000000000000000*c_l*c_l*GuoF[9]+1.00000000000000000*c_l*c_l*GuoF[10]+1.00000000000000000*c_l*c_l*GuoF[11]+1.00000000000000000*c_l*c_l*GuoF[12]+1.00000000000000000*c_l*c_l*GuoF[13]+1.00000000000000000*c_l*c_l*GuoF[14]+1.00000000000000000*c_l*c_l*GuoF[15]+1.00000000000000000*c_l*c_l*GuoF[16]+1.00000000000000000*c_l*c_l*GuoF[17]+1.00000000000000000*c_l*c_l*GuoF[18];

F_hat[1]*=(1-0.5*S[1]);
m_l[1]=m_l[1]-S[1]*(m_l[1]-meq[1])+dt*F_hat[1];
//=======================================

m_l[2]=+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][0]-2.00000000000000000*c_l*c_l*c_l*c_l*f[ci][1]-2.00000000000000000*c_l*c_l*c_l*c_l*f[ci][2]-2.00000000000000000*c_l*c_l*c_l*c_l*f[ci][3]-2.00000000000000000*c_l*c_l*c_l*c_l*f[ci][4]-2.00000000000000000*c_l*c_l*c_l*c_l*f[ci][5]-2.00000000000000000*c_l*c_l*c_l*c_l*f[ci][6]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][7]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][8]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][9]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][10]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][11]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][12]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][13]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][14]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][15]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][16]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][17]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][18];

F_hat[2]=+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[0]-2.00000000000000000*c_l*c_l*c_l*c_l*GuoF[1]-2.00000000000000000*c_l*c_l*c_l*c_l*GuoF[2]-2.00000000000000000*c_l*c_l*c_l*c_l*GuoF[3]-2.00000000000000000*c_l*c_l*c_l*c_l*GuoF[4]-2.00000000000000000*c_l*c_l*c_l*c_l*GuoF[5]-2.00000000000000000*c_l*c_l*c_l*c_l*GuoF[6]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[7]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[8]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[9]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[10]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[11]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[12]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[13]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[14]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[15]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[16]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[17]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[18];

F_hat[2]*=(1-0.5*S[2]);
m_l[2]=m_l[2]-S[2]*(m_l[2]-meq[2])+dt*F_hat[2];
//=======================================

m_l[3]=+0.00000000000000000*c_l*f[ci][0]+1.00000000000000000*c_l*f[ci][1]-1.00000000000000000*c_l*f[ci][2]+0.00000000000000000*c_l*f[ci][3]+0.00000000000000000*c_l*f[ci][4]+0.00000000000000000*c_l*f[ci][5]+0.00000000000000000*c_l*f[ci][6]+1.00000000000000000*c_l*f[ci][7]-1.00000000000000000*c_l*f[ci][8]+1.00000000000000000*c_l*f[ci][9]-1.00000000000000000*c_l*f[ci][10]+1.00000000000000000*c_l*f[ci][11]-1.00000000000000000*c_l*f[ci][12]+1.00000000000000000*c_l*f[ci][13]-1.00000000000000000*c_l*f[ci][14]+0.00000000000000000*c_l*f[ci][15]+0.00000000000000000*c_l*f[ci][16]+0.00000000000000000*c_l*f[ci][17]+0.00000000000000000*c_l*f[ci][18];

F_hat[3]=+0.00000000000000000*c_l*GuoF[0]+1.00000000000000000*c_l*GuoF[1]-1.00000000000000000*c_l*GuoF[2]+0.00000000000000000*c_l*GuoF[3]+0.00000000000000000*c_l*GuoF[4]+0.00000000000000000*c_l*GuoF[5]+0.00000000000000000*c_l*GuoF[6]+1.00000000000000000*c_l*GuoF[7]-1.00000000000000000*c_l*GuoF[8]+1.00000000000000000*c_l*GuoF[9]-1.00000000000000000*c_l*GuoF[10]+1.00000000000000000*c_l*GuoF[11]-1.00000000000000000*c_l*GuoF[12]+1.00000000000000000*c_l*GuoF[13]-1.00000000000000000*c_l*GuoF[14]+0.00000000000000000*c_l*GuoF[15]+0.00000000000000000*c_l*GuoF[16]+0.00000000000000000*c_l*GuoF[17]+0.00000000000000000*c_l*GuoF[18];

F_hat[3]*=(1-0.5*S[3]);
m_l[3]=m_l[3]-S[3]*(m_l[3]-meq[3])+dt*F_hat[3];
//=======================================

m_l[4]=+0.00000000000000000*c_l*c_l*c_l*f[ci][0]-2.00000000000000000*c_l*c_l*c_l*f[ci][1]+2.00000000000000000*c_l*c_l*c_l*f[ci][2]+0.00000000000000000*c_l*c_l*c_l*f[ci][3]+0.00000000000000000*c_l*c_l*c_l*f[ci][4]+0.00000000000000000*c_l*c_l*c_l*f[ci][5]+0.00000000000000000*c_l*c_l*c_l*f[ci][6]+1.00000000000000000*c_l*c_l*c_l*f[ci][7]-1.00000000000000000*c_l*c_l*c_l*f[ci][8]+1.00000000000000000*c_l*c_l*c_l*f[ci][9]-1.00000000000000000*c_l*c_l*c_l*f[ci][10]+1.00000000000000000*c_l*c_l*c_l*f[ci][11]-1.00000000000000000*c_l*c_l*c_l*f[ci][12]+1.00000000000000000*c_l*c_l*c_l*f[ci][13]-1.00000000000000000*c_l*c_l*c_l*f[ci][14]+0.00000000000000000*c_l*c_l*c_l*f[ci][15]+0.00000000000000000*c_l*c_l*c_l*f[ci][16]+0.00000000000000000*c_l*c_l*c_l*f[ci][17]+0.00000000000000000*c_l*c_l*c_l*f[ci][18];

F_hat[4]=+0.00000000000000000*c_l*c_l*c_l*GuoF[0]-2.00000000000000000*c_l*c_l*c_l*GuoF[1]+2.00000000000000000*c_l*c_l*c_l*GuoF[2]+0.00000000000000000*c_l*c_l*c_l*GuoF[3]+0.00000000000000000*c_l*c_l*c_l*GuoF[4]+0.00000000000000000*c_l*c_l*c_l*GuoF[5]+0.00000000000000000*c_l*c_l*c_l*GuoF[6]+1.00000000000000000*c_l*c_l*c_l*GuoF[7]-1.00000000000000000*c_l*c_l*c_l*GuoF[8]+1.00000000000000000*c_l*c_l*c_l*GuoF[9]-1.00000000000000000*c_l*c_l*c_l*GuoF[10]+1.00000000000000000*c_l*c_l*c_l*GuoF[11]-1.00000000000000000*c_l*c_l*c_l*GuoF[12]+1.00000000000000000*c_l*c_l*c_l*GuoF[13]-1.00000000000000000*c_l*c_l*c_l*GuoF[14]+0.00000000000000000*c_l*c_l*c_l*GuoF[15]+0.00000000000000000*c_l*c_l*c_l*GuoF[16]+0.00000000000000000*c_l*c_l*c_l*GuoF[17]+0.00000000000000000*c_l*c_l*c_l*GuoF[18];

F_hat[4]*=(1-0.5*S[4]);
m_l[4]=m_l[4]-S[4]*(m_l[4]-meq[4])+dt*F_hat[4];
//=======================================

m_l[5]=+0.00000000000000000*c_l*f[ci][0]+0.00000000000000000*c_l*f[ci][1]+0.00000000000000000*c_l*f[ci][2]+1.00000000000000000*c_l*f[ci][3]-1.00000000000000000*c_l*f[ci][4]+0.00000000000000000*c_l*f[ci][5]+0.00000000000000000*c_l*f[ci][6]+1.00000000000000000*c_l*f[ci][7]-1.00000000000000000*c_l*f[ci][8]-1.00000000000000000*c_l*f[ci][9]+1.00000000000000000*c_l*f[ci][10]+0.00000000000000000*c_l*f[ci][11]+0.00000000000000000*c_l*f[ci][12]+0.00000000000000000*c_l*f[ci][13]+0.00000000000000000*c_l*f[ci][14]+1.00000000000000000*c_l*f[ci][15]-1.00000000000000000*c_l*f[ci][16]+1.00000000000000000*c_l*f[ci][17]-1.00000000000000000*c_l*f[ci][18];

F_hat[5]=+0.00000000000000000*c_l*GuoF[0]+0.00000000000000000*c_l*GuoF[1]+0.00000000000000000*c_l*GuoF[2]+1.00000000000000000*c_l*GuoF[3]-1.00000000000000000*c_l*GuoF[4]+0.00000000000000000*c_l*GuoF[5]+0.00000000000000000*c_l*GuoF[6]+1.00000000000000000*c_l*GuoF[7]-1.00000000000000000*c_l*GuoF[8]-1.00000000000000000*c_l*GuoF[9]+1.00000000000000000*c_l*GuoF[10]+0.00000000000000000*c_l*GuoF[11]+0.00000000000000000*c_l*GuoF[12]+0.00000000000000000*c_l*GuoF[13]+0.00000000000000000*c_l*GuoF[14]+1.00000000000000000*c_l*GuoF[15]-1.00000000000000000*c_l*GuoF[16]+1.00000000000000000*c_l*GuoF[17]-1.00000000000000000*c_l*GuoF[18];

F_hat[5]*=(1-0.5*S[5]);
m_l[5]=m_l[5]-S[5]*(m_l[5]-meq[5])+dt*F_hat[5];
//=======================================

m_l[6]=+0.00000000000000000*c_l*c_l*c_l*f[ci][0]+0.00000000000000000*c_l*c_l*c_l*f[ci][1]+0.00000000000000000*c_l*c_l*c_l*f[ci][2]-2.00000000000000000*c_l*c_l*c_l*f[ci][3]+2.00000000000000000*c_l*c_l*c_l*f[ci][4]+0.00000000000000000*c_l*c_l*c_l*f[ci][5]+0.00000000000000000*c_l*c_l*c_l*f[ci][6]+1.00000000000000000*c_l*c_l*c_l*f[ci][7]-1.00000000000000000*c_l*c_l*c_l*f[ci][8]-1.00000000000000000*c_l*c_l*c_l*f[ci][9]+1.00000000000000000*c_l*c_l*c_l*f[ci][10]+0.00000000000000000*c_l*c_l*c_l*f[ci][11]+0.00000000000000000*c_l*c_l*c_l*f[ci][12]+0.00000000000000000*c_l*c_l*c_l*f[ci][13]+0.00000000000000000*c_l*c_l*c_l*f[ci][14]+1.00000000000000000*c_l*c_l*c_l*f[ci][15]-1.00000000000000000*c_l*c_l*c_l*f[ci][16]+1.00000000000000000*c_l*c_l*c_l*f[ci][17]-1.00000000000000000*c_l*c_l*c_l*f[ci][18];

F_hat[6]=+0.00000000000000000*c_l*c_l*c_l*GuoF[0]+0.00000000000000000*c_l*c_l*c_l*GuoF[1]+0.00000000000000000*c_l*c_l*c_l*GuoF[2]-2.00000000000000000*c_l*c_l*c_l*GuoF[3]+2.00000000000000000*c_l*c_l*c_l*GuoF[4]+0.00000000000000000*c_l*c_l*c_l*GuoF[5]+0.00000000000000000*c_l*c_l*c_l*GuoF[6]+1.00000000000000000*c_l*c_l*c_l*GuoF[7]-1.00000000000000000*c_l*c_l*c_l*GuoF[8]-1.00000000000000000*c_l*c_l*c_l*GuoF[9]+1.00000000000000000*c_l*c_l*c_l*GuoF[10]+0.00000000000000000*c_l*c_l*c_l*GuoF[11]+0.00000000000000000*c_l*c_l*c_l*GuoF[12]+0.00000000000000000*c_l*c_l*c_l*GuoF[13]+0.00000000000000000*c_l*c_l*c_l*GuoF[14]+1.00000000000000000*c_l*c_l*c_l*GuoF[15]-1.00000000000000000*c_l*c_l*c_l*GuoF[16]+1.00000000000000000*c_l*c_l*c_l*GuoF[17]-1.00000000000000000*c_l*c_l*c_l*GuoF[18];

F_hat[6]*=(1-0.5*S[6]);
m_l[6]=m_l[6]-S[6]*(m_l[6]-meq[6])+dt*F_hat[6];
//=======================================

m_l[7]=+0.00000000000000000*c_l*f[ci][0]+0.00000000000000000*c_l*f[ci][1]+0.00000000000000000*c_l*f[ci][2]+0.00000000000000000*c_l*f[ci][3]+0.00000000000000000*c_l*f[ci][4]+1.00000000000000000*c_l*f[ci][5]-1.00000000000000000*c_l*f[ci][6]+0.00000000000000000*c_l*f[ci][7]+0.00000000000000000*c_l*f[ci][8]+0.00000000000000000*c_l*f[ci][9]+0.00000000000000000*c_l*f[ci][10]+1.00000000000000000*c_l*f[ci][11]-1.00000000000000000*c_l*f[ci][12]-1.00000000000000000*c_l*f[ci][13]+1.00000000000000000*c_l*f[ci][14]+1.00000000000000000*c_l*f[ci][15]-1.00000000000000000*c_l*f[ci][16]-1.00000000000000000*c_l*f[ci][17]+1.00000000000000000*c_l*f[ci][18];

F_hat[7]=+0.00000000000000000*c_l*GuoF[0]+0.00000000000000000*c_l*GuoF[1]+0.00000000000000000*c_l*GuoF[2]+0.00000000000000000*c_l*GuoF[3]+0.00000000000000000*c_l*GuoF[4]+1.00000000000000000*c_l*GuoF[5]-1.00000000000000000*c_l*GuoF[6]+0.00000000000000000*c_l*GuoF[7]+0.00000000000000000*c_l*GuoF[8]+0.00000000000000000*c_l*GuoF[9]+0.00000000000000000*c_l*GuoF[10]+1.00000000000000000*c_l*GuoF[11]-1.00000000000000000*c_l*GuoF[12]-1.00000000000000000*c_l*GuoF[13]+1.00000000000000000*c_l*GuoF[14]+1.00000000000000000*c_l*GuoF[15]-1.00000000000000000*c_l*GuoF[16]-1.00000000000000000*c_l*GuoF[17]+1.00000000000000000*c_l*GuoF[18];

F_hat[7]*=(1-0.5*S[7]);
m_l[7]=m_l[7]-S[7]*(m_l[7]-meq[7])+dt*F_hat[7];
//=======================================

m_l[8]=+0.00000000000000000*c_l*c_l*c_l*f[ci][0]+0.00000000000000000*c_l*c_l*c_l*f[ci][1]+0.00000000000000000*c_l*c_l*c_l*f[ci][2]+0.00000000000000000*c_l*c_l*c_l*f[ci][3]+0.00000000000000000*c_l*c_l*c_l*f[ci][4]-2.00000000000000000*c_l*c_l*c_l*f[ci][5]+2.00000000000000000*c_l*c_l*c_l*f[ci][6]+0.00000000000000000*c_l*c_l*c_l*f[ci][7]+0.00000000000000000*c_l*c_l*c_l*f[ci][8]+0.00000000000000000*c_l*c_l*c_l*f[ci][9]+0.00000000000000000*c_l*c_l*c_l*f[ci][10]+1.00000000000000000*c_l*c_l*c_l*f[ci][11]-1.00000000000000000*c_l*c_l*c_l*f[ci][12]-1.00000000000000000*c_l*c_l*c_l*f[ci][13]+1.00000000000000000*c_l*c_l*c_l*f[ci][14]+1.00000000000000000*c_l*c_l*c_l*f[ci][15]-1.00000000000000000*c_l*c_l*c_l*f[ci][16]-1.00000000000000000*c_l*c_l*c_l*f[ci][17]+1.00000000000000000*c_l*c_l*c_l*f[ci][18];

F_hat[8]=+0.00000000000000000*c_l*c_l*c_l*GuoF[0]+0.00000000000000000*c_l*c_l*c_l*GuoF[1]+0.00000000000000000*c_l*c_l*c_l*GuoF[2]+0.00000000000000000*c_l*c_l*c_l*GuoF[3]+0.00000000000000000*c_l*c_l*c_l*GuoF[4]-2.00000000000000000*c_l*c_l*c_l*GuoF[5]+2.00000000000000000*c_l*c_l*c_l*GuoF[6]+0.00000000000000000*c_l*c_l*c_l*GuoF[7]+0.00000000000000000*c_l*c_l*c_l*GuoF[8]+0.00000000000000000*c_l*c_l*c_l*GuoF[9]+0.00000000000000000*c_l*c_l*c_l*GuoF[10]+1.00000000000000000*c_l*c_l*c_l*GuoF[11]-1.00000000000000000*c_l*c_l*c_l*GuoF[12]-1.00000000000000000*c_l*c_l*c_l*GuoF[13]+1.00000000000000000*c_l*c_l*c_l*GuoF[14]+1.00000000000000000*c_l*c_l*c_l*GuoF[15]-1.00000000000000000*c_l*c_l*c_l*GuoF[16]-1.00000000000000000*c_l*c_l*c_l*GuoF[17]+1.00000000000000000*c_l*c_l*c_l*GuoF[18];

F_hat[8]*=(1-0.5*S[8]);
m_l[8]=m_l[8]-S[8]*(m_l[8]-meq[8])+dt*F_hat[8];
//=======================================

m_l[9]=+0.00000000000000000*c_l*c_l*f[ci][0]+2.00000000000000000*c_l*c_l*f[ci][1]+2.00000000000000000*c_l*c_l*f[ci][2]-1.00000000000000000*c_l*c_l*f[ci][3]-1.00000000000000000*c_l*c_l*f[ci][4]-1.00000000000000000*c_l*c_l*f[ci][5]-1.00000000000000000*c_l*c_l*f[ci][6]+1.00000000000000000*c_l*c_l*f[ci][7]+1.00000000000000000*c_l*c_l*f[ci][8]+1.00000000000000000*c_l*c_l*f[ci][9]+1.00000000000000000*c_l*c_l*f[ci][10]+1.00000000000000000*c_l*c_l*f[ci][11]+1.00000000000000000*c_l*c_l*f[ci][12]+1.00000000000000000*c_l*c_l*f[ci][13]+1.00000000000000000*c_l*c_l*f[ci][14]-2.00000000000000000*c_l*c_l*f[ci][15]-2.00000000000000000*c_l*c_l*f[ci][16]-2.00000000000000000*c_l*c_l*f[ci][17]-2.00000000000000000*c_l*c_l*f[ci][18];

F_hat[9]=+0.00000000000000000*c_l*c_l*GuoF[0]+2.00000000000000000*c_l*c_l*GuoF[1]+2.00000000000000000*c_l*c_l*GuoF[2]-1.00000000000000000*c_l*c_l*GuoF[3]-1.00000000000000000*c_l*c_l*GuoF[4]-1.00000000000000000*c_l*c_l*GuoF[5]-1.00000000000000000*c_l*c_l*GuoF[6]+1.00000000000000000*c_l*c_l*GuoF[7]+1.00000000000000000*c_l*c_l*GuoF[8]+1.00000000000000000*c_l*c_l*GuoF[9]+1.00000000000000000*c_l*c_l*GuoF[10]+1.00000000000000000*c_l*c_l*GuoF[11]+1.00000000000000000*c_l*c_l*GuoF[12]+1.00000000000000000*c_l*c_l*GuoF[13]+1.00000000000000000*c_l*c_l*GuoF[14]-2.00000000000000000*c_l*c_l*GuoF[15]-2.00000000000000000*c_l*c_l*GuoF[16]-2.00000000000000000*c_l*c_l*GuoF[17]-2.00000000000000000*c_l*c_l*GuoF[18];

F_hat[9]*=(1-0.5*S[9]);
m_l[9]=m_l[9]-S[9]*(m_l[9]-meq[9])+dt*F_hat[9];
//=======================================

m_l[10]=+0.00000000000000000*c_l*c_l*c_l*c_l*f[ci][0]-2.00000000000000000*c_l*c_l*c_l*c_l*f[ci][1]-2.00000000000000000*c_l*c_l*c_l*c_l*f[ci][2]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][3]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][4]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][5]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][6]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][7]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][8]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][9]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][10]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][11]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][12]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][13]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][14]-2.00000000000000000*c_l*c_l*c_l*c_l*f[ci][15]-2.00000000000000000*c_l*c_l*c_l*c_l*f[ci][16]-2.00000000000000000*c_l*c_l*c_l*c_l*f[ci][17]-2.00000000000000000*c_l*c_l*c_l*c_l*f[ci][18];

F_hat[10]=+0.00000000000000000*c_l*c_l*c_l*c_l*GuoF[0]-2.00000000000000000*c_l*c_l*c_l*c_l*GuoF[1]-2.00000000000000000*c_l*c_l*c_l*c_l*GuoF[2]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[3]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[4]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[5]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[6]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[7]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[8]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[9]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[10]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[11]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[12]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[13]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[14]-2.00000000000000000*c_l*c_l*c_l*c_l*GuoF[15]-2.00000000000000000*c_l*c_l*c_l*c_l*GuoF[16]-2.00000000000000000*c_l*c_l*c_l*c_l*GuoF[17]-2.00000000000000000*c_l*c_l*c_l*c_l*GuoF[18];

F_hat[10]*=(1-0.5*S[10]);
m_l[10]=m_l[10]-S[10]*(m_l[10]-meq[10])+dt*F_hat[10];
//=======================================

m_l[11]=+0.00000000000000000*c_l*c_l*f[ci][0]+0.00000000000000000*c_l*c_l*f[ci][1]+0.00000000000000000*c_l*c_l*f[ci][2]+1.00000000000000000*c_l*c_l*f[ci][3]+1.00000000000000000*c_l*c_l*f[ci][4]-1.00000000000000000*c_l*c_l*f[ci][5]-1.00000000000000000*c_l*c_l*f[ci][6]+1.00000000000000000*c_l*c_l*f[ci][7]+1.00000000000000000*c_l*c_l*f[ci][8]+1.00000000000000000*c_l*c_l*f[ci][9]+1.00000000000000000*c_l*c_l*f[ci][10]-1.00000000000000000*c_l*c_l*f[ci][11]-1.00000000000000000*c_l*c_l*f[ci][12]-1.00000000000000000*c_l*c_l*f[ci][13]-1.00000000000000000*c_l*c_l*f[ci][14]+0.00000000000000000*c_l*c_l*f[ci][15]+0.00000000000000000*c_l*c_l*f[ci][16]+0.00000000000000000*c_l*c_l*f[ci][17]+0.00000000000000000*c_l*c_l*f[ci][18];

F_hat[11]=+0.00000000000000000*c_l*c_l*GuoF[0]+0.00000000000000000*c_l*c_l*GuoF[1]+0.00000000000000000*c_l*c_l*GuoF[2]+1.00000000000000000*c_l*c_l*GuoF[3]+1.00000000000000000*c_l*c_l*GuoF[4]-1.00000000000000000*c_l*c_l*GuoF[5]-1.00000000000000000*c_l*c_l*GuoF[6]+1.00000000000000000*c_l*c_l*GuoF[7]+1.00000000000000000*c_l*c_l*GuoF[8]+1.00000000000000000*c_l*c_l*GuoF[9]+1.00000000000000000*c_l*c_l*GuoF[10]-1.00000000000000000*c_l*c_l*GuoF[11]-1.00000000000000000*c_l*c_l*GuoF[12]-1.00000000000000000*c_l*c_l*GuoF[13]-1.00000000000000000*c_l*c_l*GuoF[14]+0.00000000000000000*c_l*c_l*GuoF[15]+0.00000000000000000*c_l*c_l*GuoF[16]+0.00000000000000000*c_l*c_l*GuoF[17]+0.00000000000000000*c_l*c_l*GuoF[18];

F_hat[11]*=(1-0.5*S[11]);
m_l[11]=m_l[11]-S[11]*(m_l[11]-meq[11])+dt*F_hat[11];
//=======================================

m_l[12]=+0.00000000000000000*c_l*c_l*c_l*c_l*f[ci][0]+0.00000000000000000*c_l*c_l*c_l*c_l*f[ci][1]+0.00000000000000000*c_l*c_l*c_l*c_l*f[ci][2]-1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][3]-1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][4]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][5]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][6]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][7]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][8]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][9]+1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][10]-1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][11]-1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][12]-1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][13]-1.00000000000000000*c_l*c_l*c_l*c_l*f[ci][14]+0.00000000000000000*c_l*c_l*c_l*c_l*f[ci][15]+0.00000000000000000*c_l*c_l*c_l*c_l*f[ci][16]+0.00000000000000000*c_l*c_l*c_l*c_l*f[ci][17]+0.00000000000000000*c_l*c_l*c_l*c_l*f[ci][18];

F_hat[12]=+0.00000000000000000*c_l*c_l*c_l*c_l*GuoF[0]+0.00000000000000000*c_l*c_l*c_l*c_l*GuoF[1]+0.00000000000000000*c_l*c_l*c_l*c_l*GuoF[2]-1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[3]-1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[4]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[5]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[6]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[7]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[8]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[9]+1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[10]-1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[11]-1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[12]-1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[13]-1.00000000000000000*c_l*c_l*c_l*c_l*GuoF[14]+0.00000000000000000*c_l*c_l*c_l*c_l*GuoF[15]+0.00000000000000000*c_l*c_l*c_l*c_l*GuoF[16]+0.00000000000000000*c_l*c_l*c_l*c_l*GuoF[17]+0.00000000000000000*c_l*c_l*c_l*c_l*GuoF[18];

F_hat[12]*=(1-0.5*S[12]);
m_l[12]=m_l[12]-S[12]*(m_l[12]-meq[12])+dt*F_hat[12];
//=======================================

m_l[13]=+0.00000000000000000*c_l*c_l*f[ci][0]+0.00000000000000000*c_l*c_l*f[ci][1]+0.00000000000000000*c_l*c_l*f[ci][2]+0.00000000000000000*c_l*c_l*f[ci][3]+0.00000000000000000*c_l*c_l*f[ci][4]+0.00000000000000000*c_l*c_l*f[ci][5]+0.00000000000000000*c_l*c_l*f[ci][6]+1.00000000000000000*c_l*c_l*f[ci][7]+1.00000000000000000*c_l*c_l*f[ci][8]-1.00000000000000000*c_l*c_l*f[ci][9]-1.00000000000000000*c_l*c_l*f[ci][10]+0.00000000000000000*c_l*c_l*f[ci][11]+0.00000000000000000*c_l*c_l*f[ci][12]+0.00000000000000000*c_l*c_l*f[ci][13]+0.00000000000000000*c_l*c_l*f[ci][14]+0.00000000000000000*c_l*c_l*f[ci][15]+0.00000000000000000*c_l*c_l*f[ci][16]+0.00000000000000000*c_l*c_l*f[ci][17]+0.00000000000000000*c_l*c_l*f[ci][18];

F_hat[13]=+0.00000000000000000*c_l*c_l*GuoF[0]+0.00000000000000000*c_l*c_l*GuoF[1]+0.00000000000000000*c_l*c_l*GuoF[2]+0.00000000000000000*c_l*c_l*GuoF[3]+0.00000000000000000*c_l*c_l*GuoF[4]+0.00000000000000000*c_l*c_l*GuoF[5]+0.00000000000000000*c_l*c_l*GuoF[6]+1.00000000000000000*c_l*c_l*GuoF[7]+1.00000000000000000*c_l*c_l*GuoF[8]-1.00000000000000000*c_l*c_l*GuoF[9]-1.00000000000000000*c_l*c_l*GuoF[10]+0.00000000000000000*c_l*c_l*GuoF[11]+0.00000000000000000*c_l*c_l*GuoF[12]+0.00000000000000000*c_l*c_l*GuoF[13]+0.00000000000000000*c_l*c_l*GuoF[14]+0.00000000000000000*c_l*c_l*GuoF[15]+0.00000000000000000*c_l*c_l*GuoF[16]+0.00000000000000000*c_l*c_l*GuoF[17]+0.00000000000000000*c_l*c_l*GuoF[18];

F_hat[13]*=(1-0.5*S[13]);
m_l[13]=m_l[13]-S[13]*(m_l[13]-meq[13])+dt*F_hat[13];
//=======================================

m_l[14]=+0.00000000000000000*c_l*c_l*f[ci][0]+0.00000000000000000*c_l*c_l*f[ci][1]+0.00000000000000000*c_l*c_l*f[ci][2]+0.00000000000000000*c_l*c_l*f[ci][3]+0.00000000000000000*c_l*c_l*f[ci][4]+0.00000000000000000*c_l*c_l*f[ci][5]+0.00000000000000000*c_l*c_l*f[ci][6]+0.00000000000000000*c_l*c_l*f[ci][7]+0.00000000000000000*c_l*c_l*f[ci][8]+0.00000000000000000*c_l*c_l*f[ci][9]+0.00000000000000000*c_l*c_l*f[ci][10]+0.00000000000000000*c_l*c_l*f[ci][11]+0.00000000000000000*c_l*c_l*f[ci][12]+0.00000000000000000*c_l*c_l*f[ci][13]+0.00000000000000000*c_l*c_l*f[ci][14]+1.00000000000000000*c_l*c_l*f[ci][15]+1.00000000000000000*c_l*c_l*f[ci][16]-1.00000000000000000*c_l*c_l*f[ci][17]-1.00000000000000000*c_l*c_l*f[ci][18];

F_hat[14]=+0.00000000000000000*c_l*c_l*GuoF[0]+0.00000000000000000*c_l*c_l*GuoF[1]+0.00000000000000000*c_l*c_l*GuoF[2]+0.00000000000000000*c_l*c_l*GuoF[3]+0.00000000000000000*c_l*c_l*GuoF[4]+0.00000000000000000*c_l*c_l*GuoF[5]+0.00000000000000000*c_l*c_l*GuoF[6]+0.00000000000000000*c_l*c_l*GuoF[7]+0.00000000000000000*c_l*c_l*GuoF[8]+0.00000000000000000*c_l*c_l*GuoF[9]+0.00000000000000000*c_l*c_l*GuoF[10]+0.00000000000000000*c_l*c_l*GuoF[11]+0.00000000000000000*c_l*c_l*GuoF[12]+0.00000000000000000*c_l*c_l*GuoF[13]+0.00000000000000000*c_l*c_l*GuoF[14]+1.00000000000000000*c_l*c_l*GuoF[15]+1.00000000000000000*c_l*c_l*GuoF[16]-1.00000000000000000*c_l*c_l*GuoF[17]-1.00000000000000000*c_l*c_l*GuoF[18];

F_hat[14]*=(1-0.5*S[14]);
m_l[14]=m_l[14]-S[14]*(m_l[14]-meq[14])+dt*F_hat[14];
//=======================================

m_l[15]=+0.00000000000000000*c_l*c_l*f[ci][0]+0.00000000000000000*c_l*c_l*f[ci][1]+0.00000000000000000*c_l*c_l*f[ci][2]+0.00000000000000000*c_l*c_l*f[ci][3]+0.00000000000000000*c_l*c_l*f[ci][4]+0.00000000000000000*c_l*c_l*f[ci][5]+0.00000000000000000*c_l*c_l*f[ci][6]+0.00000000000000000*c_l*c_l*f[ci][7]+0.00000000000000000*c_l*c_l*f[ci][8]+0.00000000000000000*c_l*c_l*f[ci][9]+0.00000000000000000*c_l*c_l*f[ci][10]+1.00000000000000000*c_l*c_l*f[ci][11]+1.00000000000000000*c_l*c_l*f[ci][12]-1.00000000000000000*c_l*c_l*f[ci][13]-1.00000000000000000*c_l*c_l*f[ci][14]+0.00000000000000000*c_l*c_l*f[ci][15]+0.00000000000000000*c_l*c_l*f[ci][16]+0.00000000000000000*c_l*c_l*f[ci][17]+0.00000000000000000*c_l*c_l*f[ci][18];

F_hat[15]=+0.00000000000000000*c_l*c_l*GuoF[0]+0.00000000000000000*c_l*c_l*GuoF[1]+0.00000000000000000*c_l*c_l*GuoF[2]+0.00000000000000000*c_l*c_l*GuoF[3]+0.00000000000000000*c_l*c_l*GuoF[4]+0.00000000000000000*c_l*c_l*GuoF[5]+0.00000000000000000*c_l*c_l*GuoF[6]+0.00000000000000000*c_l*c_l*GuoF[7]+0.00000000000000000*c_l*c_l*GuoF[8]+0.00000000000000000*c_l*c_l*GuoF[9]+0.00000000000000000*c_l*c_l*GuoF[10]+1.00000000000000000*c_l*c_l*GuoF[11]+1.00000000000000000*c_l*c_l*GuoF[12]-1.00000000000000000*c_l*c_l*GuoF[13]-1.00000000000000000*c_l*c_l*GuoF[14]+0.00000000000000000*c_l*c_l*GuoF[15]+0.00000000000000000*c_l*c_l*GuoF[16]+0.00000000000000000*c_l*c_l*GuoF[17]+0.00000000000000000*c_l*c_l*GuoF[18];

F_hat[15]*=(1-0.5*S[15]);
m_l[15]=m_l[15]-S[15]*(m_l[15]-meq[15])+dt*F_hat[15];
//=======================================

m_l[16]=+0.00000000000000000*c_l*c_l*c_l*f[ci][0]+0.00000000000000000*c_l*c_l*c_l*f[ci][1]+0.00000000000000000*c_l*c_l*c_l*f[ci][2]+0.00000000000000000*c_l*c_l*c_l*f[ci][3]+0.00000000000000000*c_l*c_l*c_l*f[ci][4]+0.00000000000000000*c_l*c_l*c_l*f[ci][5]+0.00000000000000000*c_l*c_l*c_l*f[ci][6]+1.00000000000000000*c_l*c_l*c_l*f[ci][7]-1.00000000000000000*c_l*c_l*c_l*f[ci][8]+1.00000000000000000*c_l*c_l*c_l*f[ci][9]-1.00000000000000000*c_l*c_l*c_l*f[ci][10]-1.00000000000000000*c_l*c_l*c_l*f[ci][11]+1.00000000000000000*c_l*c_l*c_l*f[ci][12]-1.00000000000000000*c_l*c_l*c_l*f[ci][13]+1.00000000000000000*c_l*c_l*c_l*f[ci][14]+0.00000000000000000*c_l*c_l*c_l*f[ci][15]+0.00000000000000000*c_l*c_l*c_l*f[ci][16]+0.00000000000000000*c_l*c_l*c_l*f[ci][17]+0.00000000000000000*c_l*c_l*c_l*f[ci][18];

F_hat[16]=+0.00000000000000000*c_l*c_l*c_l*GuoF[0]+0.00000000000000000*c_l*c_l*c_l*GuoF[1]+0.00000000000000000*c_l*c_l*c_l*GuoF[2]+0.00000000000000000*c_l*c_l*c_l*GuoF[3]+0.00000000000000000*c_l*c_l*c_l*GuoF[4]+0.00000000000000000*c_l*c_l*c_l*GuoF[5]+0.00000000000000000*c_l*c_l*c_l*GuoF[6]+1.00000000000000000*c_l*c_l*c_l*GuoF[7]-1.00000000000000000*c_l*c_l*c_l*GuoF[8]+1.00000000000000000*c_l*c_l*c_l*GuoF[9]-1.00000000000000000*c_l*c_l*c_l*GuoF[10]-1.00000000000000000*c_l*c_l*c_l*GuoF[11]+1.00000000000000000*c_l*c_l*c_l*GuoF[12]-1.00000000000000000*c_l*c_l*c_l*GuoF[13]+1.00000000000000000*c_l*c_l*c_l*GuoF[14]+0.00000000000000000*c_l*c_l*c_l*GuoF[15]+0.00000000000000000*c_l*c_l*c_l*GuoF[16]+0.00000000000000000*c_l*c_l*c_l*GuoF[17]+0.00000000000000000*c_l*c_l*c_l*GuoF[18];

F_hat[16]*=(1-0.5*S[16]);
m_l[16]=m_l[16]-S[16]*(m_l[16]-meq[16])+dt*F_hat[16];
//=======================================

m_l[17]=+0.00000000000000000*c_l*c_l*c_l*f[ci][0]+0.00000000000000000*c_l*c_l*c_l*f[ci][1]+0.00000000000000000*c_l*c_l*c_l*f[ci][2]+0.00000000000000000*c_l*c_l*c_l*f[ci][3]+0.00000000000000000*c_l*c_l*c_l*f[ci][4]+0.00000000000000000*c_l*c_l*c_l*f[ci][5]+0.00000000000000000*c_l*c_l*c_l*f[ci][6]-1.00000000000000000*c_l*c_l*c_l*f[ci][7]+1.00000000000000000*c_l*c_l*c_l*f[ci][8]+1.00000000000000000*c_l*c_l*c_l*f[ci][9]-1.00000000000000000*c_l*c_l*c_l*f[ci][10]+0.00000000000000000*c_l*c_l*c_l*f[ci][11]+0.00000000000000000*c_l*c_l*c_l*f[ci][12]+0.00000000000000000*c_l*c_l*c_l*f[ci][13]+0.00000000000000000*c_l*c_l*c_l*f[ci][14]+1.00000000000000000*c_l*c_l*c_l*f[ci][15]-1.00000000000000000*c_l*c_l*c_l*f[ci][16]+1.00000000000000000*c_l*c_l*c_l*f[ci][17]-1.00000000000000000*c_l*c_l*c_l*f[ci][18];

F_hat[17]=+0.00000000000000000*c_l*c_l*c_l*GuoF[0]+0.00000000000000000*c_l*c_l*c_l*GuoF[1]+0.00000000000000000*c_l*c_l*c_l*GuoF[2]+0.00000000000000000*c_l*c_l*c_l*GuoF[3]+0.00000000000000000*c_l*c_l*c_l*GuoF[4]+0.00000000000000000*c_l*c_l*c_l*GuoF[5]+0.00000000000000000*c_l*c_l*c_l*GuoF[6]-1.00000000000000000*c_l*c_l*c_l*GuoF[7]+1.00000000000000000*c_l*c_l*c_l*GuoF[8]+1.00000000000000000*c_l*c_l*c_l*GuoF[9]-1.00000000000000000*c_l*c_l*c_l*GuoF[10]+0.00000000000000000*c_l*c_l*c_l*GuoF[11]+0.00000000000000000*c_l*c_l*c_l*GuoF[12]+0.00000000000000000*c_l*c_l*c_l*GuoF[13]+0.00000000000000000*c_l*c_l*c_l*GuoF[14]+1.00000000000000000*c_l*c_l*c_l*GuoF[15]-1.00000000000000000*c_l*c_l*c_l*GuoF[16]+1.00000000000000000*c_l*c_l*c_l*GuoF[17]-1.00000000000000000*c_l*c_l*c_l*GuoF[18];

F_hat[17]*=(1-0.5*S[17]);
m_l[17]=m_l[17]-S[17]*(m_l[17]-meq[17])+dt*F_hat[17];
//=======================================

m_l[18]=+0.00000000000000000*c_l*c_l*c_l*f[ci][0]+0.00000000000000000*c_l*c_l*c_l*f[ci][1]+0.00000000000000000*c_l*c_l*c_l*f[ci][2]+0.00000000000000000*c_l*c_l*c_l*f[ci][3]+0.00000000000000000*c_l*c_l*c_l*f[ci][4]+0.00000000000000000*c_l*c_l*c_l*f[ci][5]+0.00000000000000000*c_l*c_l*c_l*f[ci][6]+0.00000000000000000*c_l*c_l*c_l*f[ci][7]+0.00000000000000000*c_l*c_l*c_l*f[ci][8]+0.00000000000000000*c_l*c_l*c_l*f[ci][9]+0.00000000000000000*c_l*c_l*c_l*f[ci][10]+1.00000000000000000*c_l*c_l*c_l*f[ci][11]-1.00000000000000000*c_l*c_l*c_l*f[ci][12]-1.00000000000000000*c_l*c_l*c_l*f[ci][13]+1.00000000000000000*c_l*c_l*c_l*f[ci][14]-1.00000000000000000*c_l*c_l*c_l*f[ci][15]+1.00000000000000000*c_l*c_l*c_l*f[ci][16]+1.00000000000000000*c_l*c_l*c_l*f[ci][17]-1.00000000000000000*c_l*c_l*c_l*f[ci][18];

F_hat[18]=+0.00000000000000000*c_l*c_l*c_l*GuoF[0]+0.00000000000000000*c_l*c_l*c_l*GuoF[1]+0.00000000000000000*c_l*c_l*c_l*GuoF[2]+0.00000000000000000*c_l*c_l*c_l*GuoF[3]+0.00000000000000000*c_l*c_l*c_l*GuoF[4]+0.00000000000000000*c_l*c_l*c_l*GuoF[5]+0.00000000000000000*c_l*c_l*c_l*GuoF[6]+0.00000000000000000*c_l*c_l*c_l*GuoF[7]+0.00000000000000000*c_l*c_l*c_l*GuoF[8]+0.00000000000000000*c_l*c_l*c_l*GuoF[9]+0.00000000000000000*c_l*c_l*c_l*GuoF[10]+1.00000000000000000*c_l*c_l*c_l*GuoF[11]-1.00000000000000000*c_l*c_l*c_l*GuoF[12]-1.00000000000000000*c_l*c_l*c_l*GuoF[13]+1.00000000000000000*c_l*c_l*c_l*GuoF[14]-1.00000000000000000*c_l*c_l*c_l*GuoF[15]+1.00000000000000000*c_l*c_l*c_l*GuoF[16]+1.00000000000000000*c_l*c_l*c_l*GuoF[17]-1.00000000000000000*c_l*c_l*c_l*GuoF[18];

F_hat[18]*=(1-0.5*S[18]);
m_l[18]=m_l[18]-S[18]*(m_l[18]-meq[18])+dt*F_hat[18];
//=======================================



		
			
			// ==================   m=Mf matrix calculation  =============================
			// ==================   F_hat=(I-.5*S)MGuoF =====================================
				for (int mi=0; mi<19; mi++)
					{
					 /*
					 //========================       
					m_l[mi]=0;F_hat[mi]=0;
					for (int mj=0; mj<19; mj++)
						{
						m_l[mi]+=M[mi][mj]*f[ci][mj];
						F_hat[mi]+=M[mi][mj]*GuoF[mj];
						}
					F_hat[mi]*=(1-0.5*S[mi]);
					m_l[mi]=m_l[mi]-S[mi]*(m_l[mi]-meq[mi])+dt*F_hat[mi];
			//==============================================================================
			                */


			//================================================================================




				eu=elat[mi][0]*u[ci][0]+elat[mi][1]*u[ci][1]+elat[mi][2]*u[ci][2];

                	//=========================EVOLUTION OF G FOR SIMPLE BOUNDARY========================
               		//	g_r[mi]=fg[ci][mi]-(fg[ci][mi]-w[mi]*rho_r[ci]*(1+3*eu))/tau_s;
			//=========================EVOLUTION OF G FOR COMPLEX BOUNDARY=======================
                		g_r[mi]=fg[ci][mi]-(fg[ci][mi]-w[mi]*rho_r[ci]*(1+3*eu/c2+4.5*eu*eu/c4-1.5*uu/c2))/tau_s;
			//===================================================================================

					}

//==========================
m_inv_l[0]=+((double)0X1.5555555555555P-2)*1.0*m_l[0]+((double)-0X1P-1)*1.0/(c_l*c_l)*m_l[1]+((double)0X1.5555555555555P-3)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)0X0P+0)*1.0/c_l*m_l[3]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)0X0P+0)*1.0/(c_l)*m_l[5]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)0X0P+0)*1.0/c_l*m_l[7]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[9]+((double)0X0P+0)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[11]+((double)0X0P+0)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[13]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[14]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[15]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[18];

m_inv_l[1]=+((double)0X1.C71C71C71C71CP-5)*1.0*m_l[0]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[1]+((double)-0X1.C71C71C71C71EP-5)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)0X1.5555555555555P-3)*1.0/c_l*m_l[3]+((double)-0X1.5555555555556P-3)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)0X0P+0)*1.0/(c_l)*m_l[5]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)0X0P+0)*1.0/c_l*m_l[7]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)0X1.5555555555556P-4)*1.0/(c_l*c_l)*m_l[9]+((double)-0X1.5555555555554P-4)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[11]+((double)0X0P+0)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[13]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[14]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[15]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[18];

m_inv_l[2]=+((double)0X1.C71C71C71C71DP-5)*1.0*m_l[0]+((double)-0X1P-56)*1.0/(c_l*c_l)*m_l[1]+((double)-0X1.C71C71C71C71DP-5)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)-0X1.5555555555555P-3)*1.0/c_l*m_l[3]+((double)0X1.5555555555556P-3)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)0X0P+0)*1.0/(c_l)*m_l[5]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)0X0P+0)*1.0/c_l*m_l[7]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)0X1.5555555555556P-4)*1.0/(c_l*c_l)*m_l[9]+((double)-0X1.5555555555554P-4)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[11]+((double)0X0P+0)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[13]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[14]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[15]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[18];

m_inv_l[3]=+((double)0X1.C71C71C71C71DP-5)*1.0*m_l[0]+((double)0X1P-56)*1.0/(c_l*c_l)*m_l[1]+((double)-0X1.C71C71C71C71EP-5)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)0X0P+0)*1.0/c_l*m_l[3]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)0X1.5555555555554P-3)*1.0/(c_l)*m_l[5]+((double)-0X1.5555555555556P-3)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)0X0P+0)*1.0/c_l*m_l[7]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)-0X1.5555555555555P-5)*1.0/(c_l*c_l)*m_l[9]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)0X1.FFFFFFFFFFFFEP-4)*1.0/(c_l*c_l)*m_l[11]+((double)-0X1.0000000000001P-3)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[13]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[14]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[15]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)0X1P-56)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[18];

m_inv_l[4]=+((double)0X1.C71C71C71C71DP-5)*1.0*m_l[0]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[1]+((double)-0X1.C71C71C71C71AP-5)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)0X0P+0)*1.0/c_l*m_l[3]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)-0X1.5555555555555P-3)*1.0/(c_l)*m_l[5]+((double)0X1.5555555555556P-3)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)0X0P+0)*1.0/c_l*m_l[7]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)-0X1.5555555555555P-5)*1.0/(c_l*c_l)*m_l[9]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)0X1.FFFFFFFFFFFFEP-4)*1.0/(c_l*c_l)*m_l[11]+((double)-0X1.0000000000001P-3)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[13]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[14]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[15]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)-0X1P-56)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[18];

m_inv_l[5]=+((double)0X1.C71C71C71C718P-5)*1.0*m_l[0]+((double)-0X1.8P-56)*1.0/(c_l*c_l)*m_l[1]+((double)-0X1.C71C71C71C71DP-5)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)0X0P+0)*1.0/c_l*m_l[3]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)0X0P+0)*1.0/(c_l)*m_l[5]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)0X1.5555555555555P-3)*1.0/c_l*m_l[7]+((double)-0X1.5555555555556P-3)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)-0X1.5555555555558P-5)*1.0/(c_l*c_l)*m_l[9]+((double)0X1.5555555555552P-5)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)-0X1.FFFFFFFFFFFFDP-4)*1.0/(c_l*c_l)*m_l[11]+((double)0X1.0000000000002P-3)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[13]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[14]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[15]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[18];

m_inv_l[6]=+((double)0X1.C71C71C71C71AP-5)*1.0*m_l[0]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[1]+((double)-0X1.C71C71C71C71DP-5)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)0X0P+0)*1.0/c_l*m_l[3]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)-0X1.5555555555554P-58)*1.0/(c_l)*m_l[5]+((double)-0X1.5555555555554P-59)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)-0X1.5555555555555P-3)*1.0/c_l*m_l[7]+((double)0X1.5555555555555P-3)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)-0X1.5555555555556P-5)*1.0/(c_l*c_l)*m_l[9]+((double)0X1.5555555555554P-5)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)-0X1.FFFFFFFFFFFFEP-4)*1.0/(c_l*c_l)*m_l[11]+((double)0X1P-3)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[13]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[14]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[15]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)-0X1P-55)*1.0/(c_l*c_l*c_l)*m_l[18];

m_inv_l[7]=+((double)0X1.C71C71C71C71BP-6)*1.0*m_l[0]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l)*m_l[1]+((double)0X1.C71C71C71C71BP-7)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)0X1.5555555555555P-4)*1.0/c_l*m_l[3]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)0X1.5555555555555P-4)*1.0/(c_l)*m_l[5]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)0X0P+0)*1.0/c_l*m_l[7]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)0X1.5555555555555P-6)*1.0/(c_l*c_l)*m_l[9]+((double)0X1.5555555555555P-6)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)0X1.0000000000001P-4)*1.0/(c_l*c_l)*m_l[11]+((double)0X1.0000000000001P-4)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)0X1P-2)*1.0/(c_l*c_l)*m_l[13]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[14]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[15]+((double)0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)-0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[18];

m_inv_l[8]=+((double)0X1.C71C71C71C71BP-6)*1.0*m_l[0]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l)*m_l[1]+((double)0X1.C71C71C71C71BP-7)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)-0X1.5555555555555P-4)*1.0/c_l*m_l[3]+((double)-0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)-0X1.5555555555555P-4)*1.0/(c_l)*m_l[5]+((double)-0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)0X0P+0)*1.0/c_l*m_l[7]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)0X1.5555555555555P-6)*1.0/(c_l*c_l)*m_l[9]+((double)0X1.5555555555555P-6)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)0X1.0000000000001P-4)*1.0/(c_l*c_l)*m_l[11]+((double)0X1.0000000000001P-4)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)0X1P-2)*1.0/(c_l*c_l)*m_l[13]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[14]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[15]+((double)-0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[18];

m_inv_l[9]=+((double)0X1.C71C71C71C71BP-6)*1.0*m_l[0]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l)*m_l[1]+((double)0X1.C71C71C71C71BP-7)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)0X1.5555555555555P-4)*1.0/c_l*m_l[3]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)-0X1.5555555555555P-4)*1.0/(c_l)*m_l[5]+((double)-0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)0X0P+0)*1.0/c_l*m_l[7]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)0X1.5555555555555P-6)*1.0/(c_l*c_l)*m_l[9]+((double)0X1.5555555555555P-6)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)0X1.0000000000001P-4)*1.0/(c_l*c_l)*m_l[11]+((double)0X1.0000000000001P-4)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)-0X1P-2)*1.0/(c_l*c_l)*m_l[13]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[14]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[15]+((double)0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[18];

m_inv_l[10]=+((double)0X1.C71C71C71C71BP-6)*1.0*m_l[0]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l)*m_l[1]+((double)0X1.C71C71C71C71BP-7)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)-0X1.5555555555555P-4)*1.0/c_l*m_l[3]+((double)-0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)0X1.5555555555555P-4)*1.0/(c_l)*m_l[5]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)0X0P+0)*1.0/c_l*m_l[7]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)0X1.5555555555555P-6)*1.0/(c_l*c_l)*m_l[9]+((double)0X1.5555555555555P-6)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)0X1.0000000000001P-4)*1.0/(c_l*c_l)*m_l[11]+((double)0X1.0000000000001P-4)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)-0X1P-2)*1.0/(c_l*c_l)*m_l[13]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[14]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[15]+((double)-0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)-0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[18];

m_inv_l[11]=+((double)0X1.C71C71C71C71DP-6)*1.0*m_l[0]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l)*m_l[1]+((double)0X1.C71C71C71C71DP-7)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)0X1.5555555555555P-4)*1.0/c_l*m_l[3]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)0X0P+0)*1.0/(c_l)*m_l[5]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)0X1.5555555555555P-4)*1.0/c_l*m_l[7]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)0X1.5555555555555P-6)*1.0/(c_l*c_l)*m_l[9]+((double)0X1.5555555555555P-6)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)-0X1.0000000000001P-4)*1.0/(c_l*c_l)*m_l[11]+((double)-0X1.0000000000001P-4)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[13]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[14]+((double)0X1P-2)*1.0/(c_l*c_l)*m_l[15]+((double)-0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[18];

m_inv_l[12]=+((double)0X1.C71C71C71C71DP-6)*1.0*m_l[0]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l)*m_l[1]+((double)0X1.C71C71C71C71DP-7)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)-0X1.5555555555555P-4)*1.0/c_l*m_l[3]+((double)-0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)0X0P+0)*1.0/(c_l)*m_l[5]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)-0X1.5555555555555P-4)*1.0/c_l*m_l[7]+((double)-0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)0X1.5555555555555P-6)*1.0/(c_l*c_l)*m_l[9]+((double)0X1.5555555555555P-6)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)-0X1.0000000000001P-4)*1.0/(c_l*c_l)*m_l[11]+((double)-0X1.0000000000001P-4)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[13]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[14]+((double)0X1P-2)*1.0/(c_l*c_l)*m_l[15]+((double)0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)-0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[18];

m_inv_l[13]=+((double)0X1.C71C71C71C71DP-6)*1.0*m_l[0]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l)*m_l[1]+((double)0X1.C71C71C71C71DP-7)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)0X1.5555555555555P-4)*1.0/c_l*m_l[3]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)0X0P+0)*1.0/(c_l)*m_l[5]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)-0X1.5555555555555P-4)*1.0/c_l*m_l[7]+((double)-0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)0X1.5555555555555P-6)*1.0/(c_l*c_l)*m_l[9]+((double)0X1.5555555555555P-6)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)-0X1.0000000000001P-4)*1.0/(c_l*c_l)*m_l[11]+((double)-0X1.0000000000001P-4)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[13]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[14]+((double)-0X1P-2)*1.0/(c_l*c_l)*m_l[15]+((double)-0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)-0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[18];

m_inv_l[14]=+((double)0X1.C71C71C71C71DP-6)*1.0*m_l[0]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l)*m_l[1]+((double)0X1.C71C71C71C71DP-7)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)-0X1.5555555555555P-4)*1.0/c_l*m_l[3]+((double)-0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)0X0P+0)*1.0/(c_l)*m_l[5]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)0X1.5555555555555P-4)*1.0/c_l*m_l[7]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)0X1.5555555555555P-6)*1.0/(c_l*c_l)*m_l[9]+((double)0X1.5555555555555P-6)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)-0X1.0000000000001P-4)*1.0/(c_l*c_l)*m_l[11]+((double)-0X1.0000000000001P-4)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[13]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[14]+((double)-0X1P-2)*1.0/(c_l*c_l)*m_l[15]+((double)0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[18];

m_inv_l[15]=+((double)0X1.C71C71C71C71CP-6)*1.0*m_l[0]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l)*m_l[1]+((double)0X1.C71C71C71C71CP-7)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)0X0P+0)*1.0/c_l*m_l[3]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)0X1.5555555555555P-4)*1.0/(c_l)*m_l[5]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)0X1.5555555555555P-4)*1.0/c_l*m_l[7]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)-0X1.5555555555554P-5)*1.0/(c_l*c_l)*m_l[9]+((double)-0X1.5555555555554P-5)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[11]+((double)0X0P+0)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[13]+((double)0X1P-2)*1.0/(c_l*c_l)*m_l[14]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[15]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)-0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[18];

m_inv_l[16]=+((double)0X1.C71C71C71C71AP-6)*1.0*m_l[0]+((double)0X1.5555555555557P-5)*1.0/(c_l*c_l)*m_l[1]+((double)0X1.C71C71C71C71AP-7)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)0X0P+0)*1.0/c_l*m_l[3]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)-0X1.5555555555555P-4)*1.0/(c_l)*m_l[5]+((double)-0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)-0X1.5555555555555P-4)*1.0/c_l*m_l[7]+((double)-0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)-0X1.5555555555554P-5)*1.0/(c_l*c_l)*m_l[9]+((double)-0X1.5555555555554P-5)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[11]+((double)0X0P+0)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[13]+((double)0X1P-2)*1.0/(c_l*c_l)*m_l[14]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[15]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)-0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[18];

m_inv_l[17]=+((double)0X1.C71C71C71C71CP-6)*1.0*m_l[0]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l)*m_l[1]+((double)0X1.C71C71C71C71CP-7)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)0X0P+0)*1.0/c_l*m_l[3]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)0X1.5555555555555P-4)*1.0/(c_l)*m_l[5]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)-0X1.5555555555555P-4)*1.0/c_l*m_l[7]+((double)-0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)-0X1.5555555555556P-5)*1.0/(c_l*c_l)*m_l[9]+((double)-0X1.5555555555556P-5)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[11]+((double)0X0P+0)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[13]+((double)-0X1P-2)*1.0/(c_l*c_l)*m_l[14]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[15]+((double)0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[18];

m_inv_l[18]=+((double)0X1.C71C71C71C71CP-6)*1.0*m_l[0]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l)*m_l[1]+((double)0X1.C71C71C71C71CP-7)*1.0/(c_l*c_l*c_l*c_l)*m_l[2]+((double)-0X0P+0)*1.0/c_l*m_l[3]+((double)-0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[4]+((double)-0X1.5555555555555P-4)*1.0/(c_l)*m_l[5]+((double)-0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[6]+((double)0X1.5555555555555P-4)*1.0/c_l*m_l[7]+((double)0X1.5555555555555P-5)*1.0/(c_l*c_l*c_l)*m_l[8]+((double)-0X1.5555555555556P-5)*1.0/(c_l*c_l)*m_l[9]+((double)-0X1.5555555555556P-5)*1.0/(c_l*c_l*c_l*c_l)*m_l[10]+((double)-0X0P+0)*1.0/(c_l*c_l)*m_l[11]+((double)-0X0P+0)*1.0/(c_l*c_l*c_l*c_l)*m_l[12]+((double)-0X0P+0)*1.0/(c_l*c_l)*m_l[13]+((double)-0X1P-2)*1.0/(c_l*c_l)*m_l[14]+((double)0X0P+0)*1.0/(c_l*c_l)*m_l[15]+((double)-0X0P+0)*1.0/(c_l*c_l*c_l)*m_l[16]+((double)-0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[17]+((double)-0X1P-3)*1.0/(c_l*c_l*c_l)*m_l[18];

//====================				



		for (int mi=0; mi<19; mi++)
			{
			//sum=0;
			//for (int mj=0; mj<19; mj++)
			//	sum+=MI[mi][mj]*m_l[mj];
			
			sum=m_inv_l[mi];


			ip=i+e[mi][0];
			jp=j+e[mi][1];if (jp<0) {jp=NY;}; if (jp>NY) {jp=0;};
			kp=m+e[mi][2];if (kp<0) {kp=NZ;}; if (kp>NZ) {kp=0;};

			

			if (ip<0) 
				if (Sl[jp*(NZ+1)+kp]>0)
				{
				sendl[(Sl[jp*(NZ+1)+kp]-1)*5+FLN[mi]]=sum;
				sendl_s[(Sl[jp*(NZ+1)+kp]-1)*5+FLN[mi]]=g_r[mi];
				}
				else
				{
				F[ci][LR[mi]]=sum;
				Fg[ci][LR[mi]]=g_r[mi];
				
				}
					
						
					
					
			if (ip>=nx_l)
				if (Sr[jp*(NZ+1)+kp]>0)
				{
				sendr[(Sr[jp*(NZ+1)+kp]-1)*5+FRP[mi]]=sum;
				sendr_s[(Sr[jp*(NZ+1)+kp]-1)*5+FRP[mi]]=g_r[mi];
				}
				else
				{
				F[ci][LR[mi]]=sum;
				Fg[ci][LR[mi]]=g_r[mi];
				//rhor[ci]+=g_r[mi];
				}

			if ((ip>=0) and (ip<nx_l)) 
				if (Solid[ip][jp][kp]>0)
				{
				F[Solid[ip][jp][kp]][mi]=sum;
				Fg[Solid[ip][jp][kp]][mi]=g_r[mi];
				
				}
				else
				{
				F[ci][LR[mi]]=sum;
				Fg[ci][LR[mi]]=g_r[mi];
				//rhor[ci]+=g_r[mi];
				}
		//=======================G streaming=================================================
		
                
		
                 }

		

		
			
		}
                      
	
	if (rank==0)
		{
		
		
		MPI_Isend(sendr_s, Gcl[1]*5, MPI_DOUBLE, rank+1, rank*2+1, MPI_COMM_WORLD,&request2[0]);
      		MPI_Isend(sendl_s, Gcr[mpi_size-1]*5, MPI_DOUBLE, mpi_size-1, rank*2, MPI_COMM_WORLD,&request2[1]);
		MPI_Irecv(recvr_s, Gcr[0]*5, MPI_DOUBLE, rank+1, (rank+1)*2, MPI_COMM_WORLD,&request2[2]);		
      		MPI_Irecv(recvl_s, Gcl[0]*5, MPI_DOUBLE, mpi_size-1, (mpi_size-1)*2+1, MPI_COMM_WORLD,&request2[3] );
		
		MPI_Isend(sendr, Gcl[1]*5, MPI_DOUBLE, rank+1, rank*2+1+20000, MPI_COMM_WORLD,&request2[4]);
      		MPI_Isend(sendl, Gcr[mpi_size-1]*5, MPI_DOUBLE, mpi_size-1, rank*2+20000, MPI_COMM_WORLD,&request2[5]);
		MPI_Irecv(recvr, Gcr[0]*5, MPI_DOUBLE, rank+1, (rank+1)*2+20000, MPI_COMM_WORLD,&request2[6]);		
      		MPI_Irecv(recvl, Gcl[0]*5, MPI_DOUBLE, mpi_size-1, (mpi_size-1)*2+1+20000, MPI_COMM_WORLD,&request2[7] );
		}
		else
		if (rank==mpi_size-1)
			{
			MPI_Isend(sendl_s, Gcr[rank-1]*5, MPI_DOUBLE, rank-1, rank*2, MPI_COMM_WORLD,&request2[0]);
      			MPI_Isend(sendr_s, Gcl[0]*5,  MPI_DOUBLE, 0, rank*2+1, MPI_COMM_WORLD,&request2[1]);
			MPI_Irecv(recvl_s, Gcl[rank]*5, MPI_DOUBLE, rank-1, (rank-1)*2+1, MPI_COMM_WORLD,&request2[2] );
      			MPI_Irecv(recvr_s, Gcr[rank]*5, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD,&request2[3]);
			
			MPI_Isend(sendl, Gcr[rank-1]*5, MPI_DOUBLE, rank-1, rank*2+20000, MPI_COMM_WORLD,&request2[4]);
      			MPI_Isend(sendr, Gcl[0]*5,  MPI_DOUBLE, 0, rank*2+1+20000, MPI_COMM_WORLD,&request2[5]);
			MPI_Irecv(recvl, Gcl[rank]*5, MPI_DOUBLE, rank-1, (rank-1)*2+1+20000, MPI_COMM_WORLD,&request2[6] );
      			MPI_Irecv(recvr, Gcr[rank]*5, MPI_DOUBLE, 0, 0+20000, MPI_COMM_WORLD,&request2[7]);
			}
			else
			{

      			MPI_Isend(sendl_s, Gcr[rank-1]*5, MPI_DOUBLE, rank-1, rank*2, MPI_COMM_WORLD,&request2[0]);
      			MPI_Isend(sendr_s, Gcl[rank+1]*5, MPI_DOUBLE, rank+1, rank*2+1, MPI_COMM_WORLD,&request2[1]);
			MPI_Irecv(recvl_s, Gcl[rank]*5, MPI_DOUBLE, rank-1, (rank-1)*2+1, MPI_COMM_WORLD,&request2[2]);
      			MPI_Irecv(recvr_s, Gcr[rank]*5, MPI_DOUBLE, rank+1, (rank+1)*2, MPI_COMM_WORLD,&request2[3]);
			
			MPI_Isend(sendl, Gcr[rank-1]*5, MPI_DOUBLE, rank-1, rank*2+20000, MPI_COMM_WORLD,&request2[4]);
      			MPI_Isend(sendr, Gcl[rank+1]*5, MPI_DOUBLE, rank+1, rank*2+1+20000, MPI_COMM_WORLD,&request2[5]);
			MPI_Irecv(recvl, Gcl[rank]*5, MPI_DOUBLE, rank-1, (rank-1)*2+1+20000, MPI_COMM_WORLD,&request2[6]);
      			MPI_Irecv(recvr, Gcr[rank]*5, MPI_DOUBLE, rank+1, (rank+1)*2+20000, MPI_COMM_WORLD,&request2[7]);
			};

	
	MPI_Waitall(8,request2, status2);

	MPI_Testall(8,request2,&mpi_test2,status2);	


	

			for(i=1;i<=Gcl[rank];i++)
				for (int lm=0;lm<5;lm++)
					if (recvl[(i-1)*5+lm]>0)
				        {
					F[i][RP[lm]]=recvl[(i-1)*5+lm];
					Fg[i][RP[lm]]=recvl_s[(i-1)*5+lm];
			        	}
			for(j=Count-Gcr[rank]+1;j<=Count;j++)
				for (int lm=0;lm<5;lm++)
					if (recvr[(j-(Count-Gcr[rank]+1))*5+lm]>0)
					{
					Fg[j][LN[lm]]=recvr_s[(j-(Count-Gcr[rank]+1))*5+lm];
			        	F[j][LN[lm]]=recvr[(j-(Count-Gcr[rank]+1))*5+lm];
					}
			
		
			
	
	delete [] Gcl;
	delete [] Gcr;
	delete [] sendl_s;
	delete [] sendr_s;
	delete [] recvl_s;
	delete [] recvr_s;		
	
	delete [] sendl;
	delete [] sendr;
	delete [] recvl;
	delete [] recvr;

	
	

}





void comput_macro_variables( double* rho,double** u,double** u0,double** f,double** F,double** fg, double** Fg, double* rho_r, double* rhor , double* forcex, double* forcey, double* forcez,int* SupInv,int*** Solid,double*** Psi_local)
{
	
	int rank = MPI :: COMM_WORLD . Get_rank ();
	int mpi_size=MPI :: COMM_WORLD . Get_size ();

	for(int i=1;i<=Count;i++)	
                   
			{
			        //if (n==1)
			         //       cout<<u[i][0]<<" "<<u[i][1]<<" "<<u[i][2]<<endl;
			        
				u0[i][0]=u[i][0];
				u0[i][1]=u[i][1];
				u0[i][2]=u[i][2];
				rho[i]=0;
				rho_r[i]=0;
				u[i][0]=0;
				u[i][1]=0;
				u[i][2]=0;
	
				for(int k=0;k<19;k++)
					{
					
					f[i][k]=F[i][k];
					fg[i][k]=Fg[i][k];
					rho[i]+=f[i][k];
					rho_r[i]+=fg[i][k];
					u[i][0]+=elat[k][0]*f[i][k];
					u[i][1]+=elat[k][1]*f[i][k];
					u[i][2]+=elat[k][2]*f[i][k];
					}
				
			
				
				u[i][0]=(u[i][0]+dt*forcex[i])/rho[i];
				u[i][1]=(u[i][1]+dt*forcey[i])/rho[i];
				u[i][2]=(u[i][2]+dt*forcez[i])/rho[i];
				
				
				
			}



			

}





void Solute_ZeroFlux_BC(int sxp,double df_sxp, int sxn,double df_sxn,int syp,double df_syp,int syn,double df_syn,int szp,double df_szp,int szn,double df_szn,double** Fg,int*** Solid,double* rho_r,double** u,double** fg)
{
int Q=19;
int rank = MPI :: COMM_WORLD . Get_rank ();
int mpi_size=MPI :: COMM_WORLD . Get_size ();
double rho_t,rho_p;
double srho_syp,srho_syn,srho_szp,srho_szn,srho_sxp,srho_sxn;
double u_ls[3];

if (Sub_BC_psi==0)
{
if ((syp-1)*(syn-1)==0)
for (int i=0;i<nx_l;i++)
	for(int k=0;k<=NZ;k++)	
	  	{
		if ((syp==1) and (Solid[i][NY][k]>0))
			{
			rho_t=df_syp;
			for (int ks=0;ks<=4;ks++)
				rho_t+=Fg[Solid[i][NY][k]][SYN[ks]];

			rho_p=(rho_t)/(1.0/6.0);
			Fg[Solid[i][NY][k]][SYP[0]]=rho_p/18.0;			
			for (int ks=1;ks<=4;ks++)
				Fg[Solid[i][NY][k]][SYP[ks]]=rho_p/36.0;
			}
		        
		if ((syn==1) and (Solid[i][0][k]>0))
		        {
			rho_t=df_syn;
			for (int ks=0;ks<=4;ks++)
				rho_t+=Fg[Solid[i][0][k]][SYP[ks]];

			rho_p=(rho_t)/(1.0/6.0);
			Fg[Solid[i][0][k]][SYN[0]]=rho_p/18.0;			
			for (int ks=1;ks<=4;ks++)
				Fg[Solid[i][0][k]][SYN[ks]]=rho_p/36.0;
			}
		
		}
	        
if ((szp-1)*(szn-1)==0)
for (int i=0;i<nx_l;i++)
	for(int j=0;j<=NY;j++)
		{
		if ((szp==1) and (Solid[i][j][NZ]>0))
			{
			rho_t=df_szp;
			for (int ks=0;ks<=4;ks++)
				rho_t+=Fg[Solid[i][j][NZ]][SZN[ks]];//######

			rho_p=(rho_t)/(1.0/6.0);//######
			Fg[Solid[i][j][NZ]][SZP[0]]=rho_p/18.0;//######			
			for (int ks=1;ks<=4;ks++)
				Fg[Solid[i][j][NZ]][SZP[ks]]=rho_p/36.0;//######
			}

		if ((szn==1) and (Solid[i][j][0]>0))
			{
			rho_t=df_szn;
			for (int ks=0;ks<=4;ks++)
				rho_t+=Fg[Solid[i][j][0]][SZP[ks]];//######

			rho_p=(rho_t)/(1.0/6.0);//######
			Fg[Solid[i][j][0]][SZN[0]]=rho_p/18.0;//######			
			for (int ks=1;ks<=4;ks++)
				Fg[Solid[i][j][0]][SZN[ks]]=rho_p/36.0;//######
			}
		       
		}

		
if ((sxp==1) && (rank==mpi_size-1))
for (int j=0;j<=NY;j++)
        for (int k=0;k<=NZ;k++)
			if (Solid[nx_l-1][j][k]>0)
			{
			rho_t=df_sxp;
			for (int ks=0;ks<=4;ks++)
				rho_t+=Fg[Solid[nx_l-1][j][k]][SXN[ks]];//######

			rho_p=(rho_t)/(1.0/6.0);//######
			Fg[Solid[nx_l-1][j][k]][SXP[0]]=rho_p/18.0;//######			
			for (int ks=1;ks<=4;ks++)
				Fg[Solid[nx_l-1][j][k]][SXP[ks]]=rho_p/36.0;//######
			}
		
			

if ((sxn==1) && (rank==0))
for (int j=0;j<=NY;j++)
	for(int k=0;k<=NZ;k++)
			if (Solid[0][j][k]>0)
			{
			rho_t=df_sxn;
			for (int ks=0;ks<=4;ks++)
				rho_t+=Fg[Solid[0][j][k]][SXP[ks]];//######

			rho_p=(rho_t)/(1.0/6.0);//######
			Fg[Solid[0][j][k]][SXN[0]]=rho_p/18.0;//######			
			for (int ks=1;ks<=4;ks++)
				Fg[Solid[0][j][k]][SXN[ks]]=rho_p/36.0;//######

			}
       
}


if (Sub_BC_psi==1)
{

if ((syp-1)*(syn-1)==0)
for (int i=0;i<nx_l;i++)
	for(int k=0;k<=NZ;k++)	
	  	{
		if ((syp==1) && (Solid[i][NY][k]>0))
			{
			if (Solid[i][NY-1][k]>0)
					{
					srho_syp=df_syp*dx+rho_r[Solid[i][NY-1][k]];
					for (int ks=0;ks<19;ks++)
					Fg[Solid[i][NY][k]][ks]=feq_psi(ks,srho_syp,u[Solid[i][NY][k]])+fg[Solid[i][NY-1][k]][ks]-feq_psi(ks,rho_r[Solid[i][NY-1][k]],u[Solid[i][NY-1][k]]);
					}
				else	
					for (int ks=0;ks<19;ks++)
					{
					srho_syp=df_syp*dx+ref_psi;
					u_ls[0]=0.0;
					u_ls[1]=0.0;u_ls[2]=0.0;
					Fg[Solid[i][NY][k]][ks]=feq_psi(ks,srho_syp,u_ls);
					}

			}
		        
		if ((syn==1) && (Solid[i][0][k]>0))
		    {
			if (Solid[i][1][k]>0)
					{
					srho_syn=rho_r[Solid[i][1][k]]-df_syn*dx;
					for (int ks=0;ks<19;ks++)
					Fg[Solid[i][0][k]][ks]=feq_psi(ks,srho_syn,u[Solid[i][0][k]])+fg[Solid[i][1][k]][ks]-feq_psi(ks,rho_r[Solid[i][1][k]],u[Solid[i][1][k]]);
					}
				else
					for (int ks=0;ks<19;ks++)
					{
					srho_syn=ref_psi-df_syn*dx;
					u_ls[0]=0.0;
					u_ls[1]=0.0;u_ls[2]=0.0;
					Fg[Solid[i][0][k]][ks]=feq_psi(ks,srho_syn,u_ls);
					}

			}   
		
		}

if ((szp-1)*(szn-1)==0)
for (int i=0;i<nx_l;i++)
	for(int j=0;j<=NY;j++)
		{
		if ((szp==1) and (Solid[i][j][NZ]>0))
			{
			if (Solid[i][j][NZ-1]>0)
					{
					srho_szp=df_szp*dx+rho_r[Solid[i][j][NZ-1]];
					for (int ks=0;ks<19;ks++)
					Fg[Solid[i][j][NZ]][ks]=feq_psi(ks,srho_szp,u[Solid[i][j][NZ]])+fg[Solid[i][j][NZ-1]][ks]-feq_psi(ks,rho_r[Solid[i][j][NZ-1]],u[Solid[i][j][NZ-1]]);
					}
				else
					for (int ks=0;ks<19;ks++)
					{
					srho_szp=df_szp*dx+ref_psi;
					u_ls[0]=0.0;
					u_ls[1]=0.0;u_ls[2]=0.0;
					Fg[Solid[i][j][NZ]][ks]=feq_psi(ks,srho_szp,u_ls);
					}

			}

		if ((szn==1) and (Solid[i][j][0]>0))
			{
			if (Solid[i][j][1]>0)
					{
					srho_szn=rho_r[Solid[i][j][1]]-df_szn*dx;
					for (int ks=0;ks<19;ks++)
					Fg[Solid[i][j][0]][ks]=feq_psi(ks,srho_szn,u[Solid[i][j][0]])+fg[Solid[i][j][1]][ks]-feq_psi(ks,rho_r[Solid[i][j][1]],u[Solid[i][j][1]]);
					}
				else
					for (int ks=0;ks<19;ks++)
					{
					srho_szn=ref_psi-df_szn*dx;
					u_ls[0]=0.0;
					u_ls[1]=0.0;u_ls[2]=0.0;
					Fg[Solid[i][j][0]][ks]=feq_psi(ks,srho_szn,u_ls);
					}
			}
		       
		}

if ((sxp==1) && (rank==mpi_size-1)) 
for (int j=0;j<=NY;j++)
        for (int k=0;k<=NZ;k++) 
			if (Solid[nx_l-1][j][k]>0)
			{
			if (Solid[nx_l-2][j][k]>0)
					{
					srho_sxp=df_sxp*dx+rho_r[Solid[nx_l-2][j][k]];
					for (int ks=0;ks<19;ks++)
					Fg[Solid[nx_l-1][j][k]][ks]=feq_psi(ks,srho_sxp,u[Solid[nx_l-1][j][k]])+fg[Solid[nx_l-2][j][k]][ks]-feq_psi(ks,rho_r[Solid[nx_l-2][j][k]],u[Solid[nx_l-2][j][k]]);
					}
				else
					for (int ks=0;ks<19;ks++)
					{
					srho_sxp=df_sxp*dx+ref_psi;
					u_ls[0]=0.0;
					u_ls[1]=0.0;u_ls[2]=0.0;
					Fg[Solid[nx_l-1][j][k]][ks]=feq_psi(ks,srho_sxp,u_ls);
					}
			}
		
			

if ((sxn==1) && (rank==0))
for (int j=0;j<=NY;j++)
	for(int k=0;k<=NZ;k++) 
			if (Solid[0][j][k]>0)
			{
			if (Solid[1][j][k]>0)
					{
					srho_sxn=rho_r[Solid[1][j][k]]-df_sxn*dx;
					for (int ks=0;ks<19;ks++)
					Fg[Solid[0][j][k]][ks]=feq_psi(ks,srho_sxn,u[Solid[0][j][k]])+fg[Solid[1][j][k]][ks]-feq_psi(ks,rho_r[Solid[1][j][k]],u[Solid[1][j][k]]);
					}
				else
					for (int ks=0;ks<19;ks++)
					{
					srho_sxn=ref_psi-df_sxn*dx;
					u_ls[0]=0.0;
					u_ls[1]=0.0;u_ls[2]=0.0;
					Fg[Solid[0][j][k]][ks]=feq_psi(ks,srho_sxn,u_ls);
					}
			

			}
      



}



}



void Solute_Constant_BC(int sxp,double srho_sxp,int sxn, double srho_sxn,int syp,double srho_syp,int syn,double srho_syn,int szp,double srho_szp,int szn,double srho_szn,double** Fg,int*** Solid,double* rho_r,double** u,double** fg)
{
int Q=19;
int rank = MPI :: COMM_WORLD . Get_rank ();
int mpi_size=MPI :: COMM_WORLD . Get_size ();
double rho_t,rho_p;
double u_ls[3];

if (Sub_BC_psi==0)
{
if ((syp-1)*(syn-1)==0)
for (int i=0;i<nx_l;i++)
	for(int k=0;k<=NZ;k++)	
	  	{
		if ((syp==1) and (Solid[i][NY][k]>0))
			{
			rho_t=0;
			for (int ks=0;ks<19;ks++)
				rho_t+=Fg[Solid[i][NY][k]][ks];

			rho_t-=(Fg[Solid[i][NY][k]][SYP[0]]+Fg[Solid[i][NY][k]][SYP[1]]+Fg[Solid[i][NY][k]][SYP[2]]+Fg[Solid[i][NY][k]][SYP[3]]+Fg[Solid[i][NY][k]][SYP[4]]);
			rho_p=(srho_syp-rho_t)/(1.0/6.0);
			Fg[Solid[i][NY][k]][SYP[0]]=rho_p/18.0;			
			for (int ks=1;ks<=4;ks++)
				Fg[Solid[i][NY][k]][SYP[ks]]=rho_p/36.0;
			}
		        
		if ((syn==1) and (Solid[i][0][k]>0))
		        {
			rho_t=0;
			for (int ks=0;ks<19;ks++)
				rho_t+=Fg[Solid[i][0][k]][ks];

			rho_t-=(Fg[Solid[i][0][k]][SYN[0]]+Fg[Solid[i][0][k]][SYN[1]]+Fg[Solid[i][0][k]][SYN[2]]+Fg[Solid[i][0][k]][SYN[3]]+Fg[Solid[i][0][k]][SYN[4]]);
			rho_p=(srho_syn-rho_t)/(1.0/6.0);
			Fg[Solid[i][0][k]][SYN[0]]=rho_p/18.0;			
			for (int ks=1;ks<=4;ks++)
				Fg[Solid[i][0][k]][SYN[ks]]=rho_p/36.0;
			}
		
		}
	        
if ((szp-1)*(szn-1)==0)
for (int i=0;i<nx_l;i++)
	for(int j=0;j<=NY;j++)
		{
		if ((szp==1) and (Solid[i][j][NZ]>0))
			{
			rho_t=0;
			for (int ks=0;ks<19;ks++)
				rho_t+=Fg[Solid[i][j][NZ]][ks];//######

			rho_t-=(Fg[Solid[i][j][NZ]][SZP[0]]+Fg[Solid[i][j][NZ]][SZP[1]]+Fg[Solid[i][j][NZ]][SZP[2]]+Fg[Solid[i][j][NZ]][SZP[3]]+Fg[Solid[i][j][NZ]][SZP[4]]);//######
			rho_p=(srho_szp-rho_t)/(1.0/6.0);//######
			Fg[Solid[i][j][NZ]][SZP[0]]=rho_p/18.0;//######			
			for (int ks=1;ks<=4;ks++)
				Fg[Solid[i][j][NZ]][SZP[ks]]=rho_p/36.0;//######
			}

		if ((szn==1) and (Solid[i][j][0]>0))
			{
			rho_t=0;
			for (int ks=0;ks<19;ks++)
				rho_t+=Fg[Solid[i][j][0]][ks];//######

			rho_t-=(Fg[Solid[i][j][0]][SZN[0]]+Fg[Solid[i][j][0]][SZN[1]]+Fg[Solid[i][j][0]][SZN[2]]+Fg[Solid[i][j][0]][SZN[3]]+Fg[Solid[i][j][0]][SZN[4]]);//######
			rho_p=(srho_szn-rho_t)/(1.0/6.0);//######
			Fg[Solid[i][j][0]][SZN[0]]=rho_p/18.0;//######			
			for (int ks=1;ks<=4;ks++)
				Fg[Solid[i][j][0]][SZN[ks]]=rho_p/36.0;//######
			}
		       
		}

		
if ((sxp==1) && (rank==mpi_size-1))
for (int j=0;j<=NY;j++)
        for (int k=0;k<=NZ;k++)
			if (Solid[nx_l-1][j][k]>0)
			{
			rho_t=0;
			for (int ks=0;ks<19;ks++)
				rho_t+=Fg[Solid[nx_l-1][j][k]][ks];//######

			rho_t-=(Fg[Solid[nx_l-1][j][k]][SXP[0]]+Fg[Solid[nx_l-1][j][k]][SXP[1]]+Fg[Solid[nx_l-1][j][k]][SXP[2]]+Fg[Solid[nx_l-1][j][k]][SXP[3]]+Fg[Solid[nx_l-1][j][k]][SXP[4]]);//######
			rho_p=(srho_sxp-rho_t)/(1.0/6.0);//######
			Fg[Solid[nx_l-1][j][k]][SXP[0]]=rho_p/18.0;//######			
			for (int ks=1;ks<=4;ks++)
				Fg[Solid[nx_l-1][j][k]][SXP[ks]]=rho_p/36.0;//######
			}
		
			

if ((sxn==1) && (rank==0))
for (int j=0;j<=NY;j++)
	for(int k=0;k<=NZ;k++)
			if(Solid[0][j][k]>0)
			{
			rho_t=0;
			for (int ks=0;ks<19;ks++)
				rho_t+=Fg[Solid[0][j][k]][ks];//######

			rho_t-=(Fg[Solid[0][j][k]][SXN[0]]+Fg[Solid[0][j][k]][SXN[1]]+Fg[Solid[0][j][k]][SXN[2]]+Fg[Solid[0][j][k]][SXN[3]]+Fg[Solid[0][j][k]][SXN[4]]);//######
			rho_p=(srho_sxn-rho_t)/(1.0/6.0);//######
			Fg[Solid[0][j][k]][SXN[0]]=rho_p/18.0;//######			
			for (int ks=1;ks<=4;ks++)
				Fg[Solid[0][j][k]][SXN[ks]]=rho_p/36.0;//######

			}
       
}



if (Sub_BC_psi==1)
{
if ((syp-1)*(syn-1)==0)
for (int i=0;i<nx_l;i++)
	for(int k=0;k<=NZ;k++)	
	  	{
		if ((syp==1) && (Solid[i][NY][k]>0))
			{
			if (Solid[i][NY-1][k]>0)
					{
					for (int ks=0;ks<19;ks++)
					Fg[Solid[i][NY][k]][ks]=feq_psi(ks,srho_syp,u[Solid[i][NY][k]])+fg[Solid[i][NY-1][k]][ks]-feq_psi(ks,rho_r[Solid[i][NY-1][k]],u[Solid[i][NY-1][k]]);
					}
				else	
					for (int ks=0;ks<19;ks++)
					{
					u_ls[0]=0.0;
					u_ls[1]=0.0;u_ls[2]=0.0;
					Fg[Solid[i][NY][k]][ks]=feq_psi(ks,srho_syp,u_ls);
					}

			}
		        
		if ((syn==1) && (Solid[i][0][k]>0))
		    {
			if (Solid[i][1][k]>0)
					{
					for (int ks=0;ks<19;ks++)
					Fg[Solid[i][0][k]][ks]=feq_psi(ks,srho_syn,u[Solid[i][0][k]])+fg[Solid[i][1][k]][ks]-feq_psi(ks,rho_r[Solid[i][1][k]],u[Solid[i][1][k]]);
					}
				else
					for (int ks=0;ks<19;ks++)
					{
					u_ls[0]=0.0;
					u_ls[1]=0.0;u_ls[2]=0.0;
					Fg[Solid[i][0][k]][ks]=feq_psi(ks,srho_syn,u_ls);
					}

			}   
		
		}

if ((szp-1)*(szn-1)==0)
for (int i=0;i<nx_l;i++)
	for(int j=0;j<=NY;j++)
		{
		if ((szp==1) and (Solid[i][j][NZ]>0))
			{
			if (Solid[i][j][NZ-1]>0)
					{
					for (int ks=0;ks<19;ks++)
					Fg[Solid[i][j][NZ]][ks]=feq_psi(ks,srho_szp,u[Solid[i][j][NZ]])+fg[Solid[i][j][NZ-1]][ks]-feq_psi(ks,rho_r[Solid[i][j][NZ-1]],u[Solid[i][j][NZ-1]]);
					}
				else
					for (int ks=0;ks<19;ks++)
					{
					u_ls[0]=0.0;
					u_ls[1]=0.0;u_ls[2]=0.0;
					Fg[Solid[i][j][NZ]][ks]=feq_psi(ks,srho_szp,u_ls);
					}

			}

		if ((szn==1) and (Solid[i][j][0]>0))
			{
			if (Solid[i][j][1]>0)
					{
					for (int ks=0;ks<19;ks++)
					Fg[Solid[i][j][0]][ks]=feq_psi(ks,srho_szn,u[Solid[i][j][0]])+fg[Solid[i][j][1]][ks]-feq_psi(ks,rho_r[Solid[i][j][1]],u[Solid[i][j][1]]);
					}
				else
					for (int ks=0;ks<19;ks++)
					{
					u_ls[0]=0.0;
					u_ls[1]=0.0;u_ls[2]=0.0;
					Fg[Solid[i][j][0]][ks]=feq_psi(ks,srho_szn,u_ls);
					}
			}
		       
		}

if ((sxp==1) && (rank==mpi_size-1)) 
for (int j=0;j<=NY;j++)
        for (int k=0;k<=NZ;k++) 
			if (Solid[nx_l-1][j][k]>0)
			{
			if (Solid[nx_l-2][j][k]>0)
					{
					for (int ks=0;ks<19;ks++)
					Fg[Solid[nx_l-1][j][k]][ks]=feq_psi(ks,srho_sxp,u[Solid[nx_l-1][j][k]])+fg[Solid[nx_l-2][j][k]][ks]-feq_psi(ks,rho_r[Solid[nx_l-2][j][k]],u[Solid[nx_l-2][j][k]]);
					}
				else
					for (int ks=0;ks<19;ks++)
					{
					u_ls[0]=0.0;
					u_ls[1]=0.0;u_ls[2]=0.0;
					Fg[Solid[nx_l-1][j][k]][ks]=feq_psi(ks,srho_sxp,u_ls);
					}
			}
		
			

if ((sxn==1) && (rank==0))
for (int j=0;j<=NY;j++)
	for(int k=0;k<=NZ;k++) 
			if (Solid[0][j][k]>0)
			{
			if (Solid[1][j][k]>0)
					{
					for (int ks=0;ks<19;ks++)
					Fg[Solid[0][j][k]][ks]=feq_psi(ks,srho_sxn,u[Solid[0][j][k]])+fg[Solid[1][j][k]][ks]-feq_psi(ks,rho_r[Solid[1][j][k]],u[Solid[1][j][k]]);
					}
				else
					for (int ks=0;ks<19;ks++)
					{
					u_ls[0]=0.0;
					u_ls[1]=0.0;u_ls[2]=0.0;
					Fg[Solid[0][j][k]][ks]=feq_psi(ks,srho_sxn,u_ls);
					}
			

			}
       

}


}




void boundary_velocity(int xp,double v_xp,int xn, double v_xn,int yp,double v_yp,int yn,double v_yn,int zp,double v_zp,int zn,double v_zn,double** f,double** F,double* rho,double** u,int*** Solid)

{

int Q=19;
//double ux0,uy0,uz0;

double u_xp[3]={v_xp,0,0};
double u_xn[3]={v_xn,0,0};
double u_yp[3]={0,v_yp,0};
double u_yn[3]={0,v_yn,0};
double u_zp[3]={0,0,v_zp};
double u_zn[3]={0,0,v_zn};


int rank = MPI :: COMM_WORLD . Get_rank ();
int mpi_size=MPI :: COMM_WORLD . Get_size ();


//Equilibrium boundary condition (Use equilibrium distribution to update the distributions of particles on the boundaries)
if (Sub_BC==0)
{
if ((yp-1)*(yn-1)==0)
for (int i=0;i<nx_l;i++)
	for(int k=0;k<=NZ;k++)
		for (int ks=0;ks<Q;ks++)
		{
		if ((yp==1)  && (Solid[i][NY][k]>0))   
		        F[Solid[i][NY][k]][ks]=feq(ks,1.0,u_yp);
		if ((yn==1) && (Solid[i][0][k]>0))
		       F[Solid[i][0][k]][ks]=feq(ks,1.0,u_yn);      
		}

if ((zp-1)*(zn-1)==0)		
for (int i=0;i<nx_l;i++)
	for(int j=0;j<=NY;j++)
		for (int ks=0;ks<Q;ks++)
		{
		if ((zp==1) && (Solid[i][j][NZ]>0))    
		        F[Solid[i][j][NZ]][ks]=feq(ks,1.0,u_zp); 
		if ((zn==1) && (Solid[i][j][0]>0))
		        F[Solid[i][j][0]][ks]=feq(ks,1.0,u_zn);
		}


if ((xp==1) && (rank==mpi_size-1))
for (int j=0;j<=NY;j++)
	for (int k=0;k<=NZ;k++)
		for (int ks=0;ks<Q;ks++)
			F[Solid[nx_l-1][j][k]][ks]=feq(ks,1.0,u_xp);
			



if ((xn==1) && (rank==0))
for (int j=0;j<=NY;j++)
	for(int k=0;k<=NZ;k++)
		for (int ks=0;ks<Q;ks++)
			F[Solid[0][j][k]][ks]=feq(ks,1.0,u_xn);
			
	

}

//Equilibrium boundary the value of the pressure of the boundary particles are set by using the value of neighbouring points
if (Sub_BC==1)
{
if ((yp-1)*(yn-1)==0)
for (int i=0;i<nx_l;i++)
	for(int k=0;k<=NZ;k++)
		for (int ks=0;ks<Q;ks++)
		{
		if ((yp==1)  && (Solid[i][NY][k]>0))
		        if (Solid[i][NY-1][k]>0)
		                 F[Solid[i][NY][k]][ks]=feq(ks,rho[Solid[i][NY-1][k]],u_yp);
		         else
		                 F[Solid[i][NY][k]][ks]=feq(ks,1.0,u_yp);
		if ((yn==1) && (Solid[i][0][k]>0))
		        if (Solid[i][1][k]>0)
		                 F[Solid[i][0][k]][ks]=feq(ks,rho[Solid[i][1][k]],u_yn);
		         else
		                 F[Solid[i][0][k]][ks]=feq(ks,1.0,u_yn);      
		}

if ((zp-1)*(zn-1)==0)		
for (int i=0;i<nx_l;i++)
	for(int j=0;j<=NY;j++)
		for (int ks=0;ks<Q;ks++)
		{
		if ((zp==1) && (Solid[i][j][NZ]>0)) 
		        if (Solid[i][j][NZ-1]>0)
		                F[Solid[i][j][NZ]][ks]=feq(ks,rho[Solid[i][j][NZ-1]],u_zp);
		        else
		                F[Solid[i][j][NZ]][ks]=feq(ks,1.0,u_zp); 
		if ((zn==1) && (Solid[i][j][0]>0))
		        if (Solid[i][j][1]>0)
		                F[Solid[i][j][0]][ks]=feq(ks,rho[Solid[i][j][1]],u_zn);
		        else
		                F[Solid[i][j][0]][ks]=feq(ks,1.0,u_zn);
		}


if ((xp==1) && (rank==mpi_size-1))
for (int j=0;j<=NY;j++)
	for (int k=0;k<=NZ;k++)
		for (int ks=0;ks<Q;ks++)
		        if (Solid[nx_l-2][j][k]>0)
			        F[Solid[nx_l-1][j][k]][ks]=feq(ks,rho[Solid[nx_l-2][j][k]],u_xp);
			else
			         F[Solid[nx_l-1][j][k]][ks]=feq(ks,1.0,u_xp);
			



if ((xn==1) && (rank==0))
for (int j=0;j<=NY;j++)
	for(int k=0;k<=NZ;k++)
		for (int ks=0;ks<Q;ks++)
		        if (Solid[1][j][k]>0)
		                F[Solid[0][j][k]][ks]=feq(ks,rho[Solid[1][j][k]],u_xn);
		        else
		                F[Solid[0][j][k]][ks]=feq(ks,1.0,u_xn);
			
	

}


//Non-Equilibrium boundary condition the distribution of BC points is updated using static pressure: rho=1.0
if (Sub_BC==2)
{
if (yp==1)
for (int i=0;i<nx_l;i++)
	for(int k=0;k<=NZ;k++)
		if (Solid[i][NY][k]>0)
		for (int ks=0;ks<Q;ks++)
		{
		if (NY+e[ks][1]<NY)
			if (Solid[i][NY-1][k]>0) 
				F[Solid[i][NY][k]][ks]=feq(LR[ks],rho[Solid[i][NY-1][k]],u_yp)-F[Solid[i][NY][k]][LR[ks]]+feq(ks,rho[Solid[i][NY-1][k]],u_yp);
			else
				F[Solid[i][NY][k]][ks]=feq(LR[ks],1.0,u_yp)-F[Solid[i][NY][k]][LR[ks]]-feq(ks,1.0,u_yp);

			}


if (yn==1)
for (int i=0;i<nx_l;i++)
	for(int k=0;k<=NZ;k++)
		if (Solid[i][0][k]>0)
		for (int ks=0;ks<Q;ks++)
			{
			if (e[ks][1]>0)
				if (Solid[i][1][k]>0) 
					F[Solid[i][0][k]][ks]=feq(LR[ks],rho[Solid[i][1][k]],u_yn)-F[Solid[i][0][k]][LR[ks]]+feq(ks,rho[Solid[i][1][k]],u_yn);
				else
					F[Solid[i][0][k]][ks]=feq(LR[ks],1.0,u_yn)-F[Solid[i][0][k]][LR[ks]]+feq(ks,1.0,u_yn);

			}



if (zp==1)
for (int i=0;i<nx_l;i++)
	for(int j=0;j<=NY;j++)
	if (Solid[i][j][NZ]>0)
		for (int ks=0;ks<Q;ks++)
			{
			if (e[ks][2]<0)
				if (Solid[i][j][NZ-1]>0) 
					F[Solid[i][j][NZ]][ks]=feq(LR[ks],rho[Solid[i][j][NZ-1]],u_zp)-F[Solid[i][j][NZ]][LR[ks]]+feq(ks,rho[Solid[i][j][NZ-1]],u_zp);
				else
					F[Solid[i][j][NZ]][ks]=feq(LR[ks],1.0,u_zp)-F[Solid[i][j][NZ]][LR[ks]]+feq(ks,1.0,u_zp);
		
			}



if (zn==1)
for (int i=0;i<nx_l;i++)
	for(int j=0;j<=NY;j++)
	if (Solid[i][j][0]>0)
		for (int ks=0;ks<Q;ks++)
			{
			if (e[ks][2]>0)
				if (Solid[i][j][1]>0) 
					F[Solid[i][j][0]][ks]=feq(LR[ks],rho[Solid[i][j][1]],u_zn)-F[Solid[i][j][0]][LR[ks]]+feq(ks,rho[Solid[i][j][1]],u_zn);
				else
					F[Solid[i][j][0]][ks]=feq(LR[ks],1.0,u_zn)-F[Solid[i][j][0]][LR[ks]]+feq(ks,1.0,u_zn);
	
			}




if ((xp==1) && (rank==mpi_size-1))
for (int j=0;j<=NY;j++)
	for (int k=0;k<=NZ;k++)
	if (Solid[nx_l-1][k][j]>0)
		for (int ks=0;ks<Q;ks++)
			{
			if (e[ks][0]<0)
				if (Solid[nx_l-2][j][k]>0) 
					F[Solid[nx_l-1][j][k]][ks]=feq(LR[ks],rho[Solid[nx_l-2][j][k]],u_xp)-F[Solid[nx_l-1][j][k]][LR[ks]]+feq(ks,rho[Solid[nx_l-2][j][k]],u_xp);
				else
					F[Solid[nx_l-1][j][k]][ks]=feq(LR[ks],1.0,u_xp)-F[Solid[nx_l-1][j][k]][LR[ks]]+feq(ks,1.0,u_xp);
		
			}



if ((xn==1) && (rank==0))
for (int j=0;j<=NY;j++)
	for(int k=0;k<=NZ;k++)
	if (Solid[0][j][k]>0)
		for (int ks=0;ks<Q;ks++)
			{
			if (e[ks][0]>0)
				if (Solid[1][j][k]>0) 
					F[Solid[0][j][k]][ks]=feq(LR[ks],rho[Solid[1][j][k]],u_xn)-F[Solid[0][j][k]][LR[ks]]+feq(ks,rho[Solid[0][j][k]],u_xn);
				else
					F[Solid[0][j][k]][ks]=feq(LR[ks],1.0,u_xn)-F[Solid[0][j][k]][LR[ks]]+feq(ks,1.0,u_xn);
		
			}
	
}

//Non-Equilibrium boundary condition use neighbouring points' value to update the BC points
if (Sub_BC==3)
{
if (yp==1)
for (int i=0;i<nx_l;i++)
	for(int k=0;k<=NZ;k++)
	if (Solid[i][NY][k]>0)
		for (int ks=0;ks<Q;ks++)
			{
				if (Solid[i][NY-1][k]>0) 
					F[Solid[i][NY][k]][ks]=feq(ks,rho[Solid[i][NY-1][k]],u_yp)+f[Solid[i][NY-1][k]][ks]-feq(ks,rho[Solid[i][NY-1][k]],u[Solid[i][NY-1][k]]);
				else
					F[Solid[i][NY][k]][ks]=feq(ks,rho[Solid[i][NY-1][k]],u_yp);

			}


if (yn==1)
for (int i=0;i<nx_l;i++)
	for(int k=0;k<=NZ;k++)
	if (Solid[i][0][k]>0)
		for (int ks=0;ks<Q;ks++)
			{
				if (Solid[i][1][k]>0) 
					F[Solid[i][0][k]][ks]=feq(ks,rho[Solid[i][1][k]],u_yn)+f[Solid[i][1][k]][ks]-feq(ks,rho[Solid[i][1][k]],u[Solid[i][1][k]]);
				else
					F[Solid[i][0][k]][ks]=feq(ks,rho[Solid[i][1][k]],u_yn);

			}



if (zp==1)
for (int i=0;i<nx_l;i++)
	for(int j=0;j<=NY;j++)
	if (Solid[i][j][NZ]>0)
		for (int ks=0;ks<Q;ks++)
			{
				if (Solid[i][j][NZ-1]>0) 
					F[Solid[i][j][NZ]][ks]=feq(ks,rho[Solid[i][j][NZ-1]],u_zp)+f[Solid[i][j][NZ-1]][ks]-feq(ks,rho[Solid[i][j][NZ-1]],u[Solid[i][j][NZ-1]]);
				else
					F[Solid[i][j][NZ]][ks]=feq(ks,rho[Solid[i][j][NZ-1]],u_zp);
		
			}



if (zn==1)
for (int i=0;i<nx_l;i++)
	for(int j=0;j<=NY;j++)
	if (Solid[i][j][0]>0)
		for (int ks=0;ks<Q;ks++)
			{
				if (Solid[i][j][1]>0) 
					F[Solid[i][j][0]][ks]=feq(ks,rho[Solid[i][j][1]],u_zn)+f[Solid[i][j][1]][ks]-feq(ks,rho[Solid[i][j][1]],u[Solid[i][j][1]]);
				else
					F[Solid[i][j][0]][ks]=feq(ks,rho[Solid[i][j][1]],u_zn);
	
			}




if ((xp==1) && (rank==mpi_size-1))
for (int j=0;j<=NY;j++)
	for (int k=0;k<=NZ;k++)
	if (Solid[nx_l-1][k][j]>0)
		for (int ks=0;ks<Q;ks++)
			{
				if (Solid[nx_l-2][j][k]>0) 
					F[Solid[nx_l-1][j][k]][ks]=feq(ks,rho[Solid[nx_l-2][j][k]],u_xp)+f[Solid[nx_l-2][j][k]][ks]-feq(ks,rho[Solid[nx_l-2][j][k]],u[Solid[nx_l-2][j][k]]);
				else
					F[Solid[nx_l-1][j][k]][ks]=feq(ks,rho[Solid[nx_l-2][j][k]],u_xp);
		
			}



if ((xn==1) && (rank==0))
for (int j=0;j<=NY;j++)
	for(int k=0;k<=NZ;k++)
	if (Solid[0][j][k]>0)
		for (int ks=0;ks<Q;ks++)
			{
				if (Solid[1][j][k]>0) 
					F[Solid[0][j][k]][ks]=feq(ks,rho[Solid[1][j][k]],u_xn)+f[Solid[1][j][k]][ks]-feq(ks,rho[Solid[1][j][k]],u[Solid[1][j][k]]);
				else
					F[Solid[0][j][k]][ks]=feq(ks,rho[Solid[1][j][k]],u_xn);
		
			}
	
}


}



void boundary_pressure(int xp,double rho_xp,int xn, double rho_xn,int yp,double rho_yp,int yn,double rho_yn,int zp,double rho_zp,int zn,double rho_zn,double** f,double** F,double** u,double* rho,int*** Solid)


{

int Q=19;
double u_ls[3]={0,0,0};
int rank = MPI :: COMM_WORLD . Get_rank ();
int mpi_size=MPI :: COMM_WORLD . Get_size ();
double m_l[19];


//Equilibriun boundary condition. velocities of boundary particles are set as 0.0
if (Sub_BC==0)
{
if ((yp-1)*(yn-1)==0)
for (int i=0;i<nx_l;i++)
	for(int k=0;k<=NZ;k++)	
	        for (int ks=0;ks<Q;ks++)
		{
		if ((yp==1) && (Solid[i][NY][k]>0))
		        F[Solid[i][NY][k]][ks]=feq(ks,rho_yp,u_ls);
		if ((yn==1) && (Solid[i][0][k]>0))
		        F[Solid[i][0][k]][ks]=feq(ks,rho_yn,u_ls);
		
		}
	        
if ((zp-1)*(zn-1)==0)
for (int i=0;i<nx_l;i++)
	for(int j=0;j<=NY;j++)
		for (int ks=0;ks<Q;ks++)
		{
		if ((zp==1) && (Solid[i][j][NZ]>0))
		        F[Solid[i][j][NZ]][ks]=feq(ks,rho_zp,u_ls);
		if ((zn==1) && (Solid[i][j][0]>0))
		        F[Solid[i][j][0]][ks]=feq(ks,rho_zn,u_ls);
		}

		
if ((xp==1) && (rank==mpi_size-1))
for (int j=0;j<=NY;j++)
        for (int k=0;k<=NZ;k++)
		for (int ks=0;ks<Q;ks++)
		if (Solid[nx_l-1][j][k]>0)
			F[Solid[nx_l-1][j][k]][ks]=feq(ks,rho_xp,u_ls);
			

if ((xn==1) && (rank==0))
for (int j=0;j<=NY;j++)
	for(int k=0;k<=NZ;k++)
		for (int ks=0;ks<Q;ks++)		
		if (Solid[0][j][k]>0)
			F[Solid[0][j][k]][ks]=feq(ks,rho_xn,u_ls);
}
       


//Equilibruim boundary condition: Use the value of neighbouring points values to update the distributions of BC points
 if (Sub_BC==1)
{
if (yp==1)
for (int i=0;i<nx_l;i++)
	for(int k=0;k<=NZ;k++)
	if (Solid[i][NY][k]>0)
		for (int ks=0;ks<Q;ks++)
			{
				if (Solid[i][NY-1][k]>0)
					{
					u_ls[0]=u[Solid[i][NY-1][k]][0];
					u_ls[1]=u[Solid[i][NY-1][k]][1];
					u_ls[2]=u[Solid[i][NY-1][k]][2];
					F[Solid[i][NY][k]][ks]=feq(ks,rho_yp,u_ls);
					}
				else
					{
					u_ls[0]=0.0;
					u_ls[1]=0.0;u_ls[2]=0.0;
					F[Solid[i][NY][k]][ks]=feq(ks,rho_yp,u_ls);
					}
			}
	


if (yn==1)
for (int i=0;i<nx_l;i++)
	for(int k=0;k<=NZ;k++)
	if (Solid[i][0][k]>0)
		for (int ks=0;ks<Q;ks++)
			{
				if (Solid[i][1][k]>0)
					{
					u_ls[0]=u[Solid[i][1][k]][0];
					u_ls[1]=u[Solid[i][1][k]][1];
					u_ls[2]=u[Solid[i][1][k]][2];
					F[Solid[i][0][k]][ks]=feq(ks,rho_yn,u_ls);
					}
			else
					{
					u_ls[0]=0.0;
					u_ls[1]=0.0;u_ls[2]=0.0;
					F[Solid[i][0][k]][ks]=feq(ks,rho_yn,u_ls);
					}
			}



if (zp==1)
for (int i=0;i<nx_l;i++)
	for(int j=0;j<=NY;j++)
	if (Solid[i][j][NZ]>0)
		for (int ks=0;ks<Q;ks++)
			{			
				if (Solid[i][j][NZ-1]>0)
					{
					u_ls[0]=u[Solid[i][j][NZ-1]][0];
					u_ls[1]=u[Solid[i][j][NZ-1]][1];
					u_ls[2]=u[Solid[i][j][NZ-1]][2];
					F[Solid[i][j][NZ]][ks]=feq(ks,rho_zp,u_ls);
					}
				else
					{
					u_ls[0]=0.0;
					u_ls[1]=0.0;u_ls[2]=0.0;
					F[Solid[i][j][NZ]][ks]=feq(ks,rho_zp,u_ls);
					}

			}



if (zn==1)
for (int i=0;i<nx_l;i++)
	for(int j=0;j<=NY;j++)
	if (Solid[i][j][0]>0)
		for (int ks=0;ks<Q;ks++)
			{
				if (Solid[i][j][1]>0)
					{
					u_ls[0]=u[Solid[i][j][1]][0];
					u_ls[1]=u[Solid[i][j][1]][1];
					u_ls[2]=u[Solid[i][j][1]][2];
					F[Solid[i][j][0]][ks]=feq(ks,rho_zn,u_ls);
					}
				else
					{
					u_ls[0]=0.0;
					u_ls[1]=0.0;u_ls[2]=0.0;
					F[Solid[i][j][0]][ks]=feq(ks,rho_zn,u_ls);
					}
			
			}
	



if ((xp==1) && (rank==mpi_size-1))
for (int j=0;j<=NY;j++)
	for (int k=0;k<=NZ;k++)
	if (Solid[nx_l-1][j][k]>0)
		for (int ks=0;ks<Q;ks++)
			{
				if (Solid[nx_l-2][j][k]>0)
					{
					u_ls[0]=u[Solid[nx_l-2][j][k]][0];
					u_ls[1]=u[Solid[nx_l-2][j][k]][1];
					u_ls[2]=u[Solid[nx_l-2][j][k]][2];
					//f[Solid[nx_l-1][j][k]][ks]=feq(ks,rho_xp,u_ls)+f[Solid[nx_l-2][j][k]][ks]-feq(ks,rho[Solid[nx_l-2][j][k]],u[Solid[nx_l-2][j][k]]);
					F[Solid[nx_l-1][j][k]][ks]=feq(ks,rho_xp,u_ls);
					}
				else	
					{
					u_ls[0]=0.0;
					u_ls[1]=0.0;
					u_ls[2]=0.0;
					F[Solid[nx_l-1][j][k]][ks]=feq(ks,rho_xp,u_ls);
					}

			}
			
			



if ((xn==1) && (rank==0))
for (int j=0;j<=NY;j++)
	for(int k=0;k<=NZ;k++)
	if (Solid[0][j][k]>0)
		for (int ks=0;ks<Q;ks++)
			{
				if(Solid[1][j][k]>0)
					{
					u_ls[0]=u[Solid[1][j][k]][0];
					u_ls[1]=u[Solid[1][j][k]][1];
					u_ls[2]=u[Solid[1][j][k]][1];
					//f[Solid[0][j][k]][ks]=feq(ks,rho_xn,u_ls)+f[Solid[1][j][k]][ks]-feq(ks,rho[Solid[1][j][k]],u[Solid[1][j][k]]);
					F[Solid[0][j][k]][ks]=feq(ks,rho_xn,u_ls);
					}
				else
					{
					u_ls[0]=0.0;
					u_ls[1]=0.0;
					u_ls[2]=0.0;
					F[Solid[0][j][k]][ks]=feq(ks,rho_xn,u_ls);
					}
			}
	

}	  


//Non-equilibrium boundary condition  
if (Sub_BC==2)
{

if (yp==1)
for (int i=0;i<nx_l;i++)
	for(int k=0;k<=NZ;k++)	
	        if (Solid[i][NY][k]>0)
			if (Solid[i][NY-1][k]>0)
			{
			for (int mi=0; mi<19; mi++)
					{
					m_l[mi]=0;
					for (int mj=0; mj<19; mj++)
						m_l[mi]+=M[mi][mj]*f[Solid[i][NY-1][k]][mj];
					}

			m_l[0]=rho_yp;
			for (int mi=0; mi<19; mi++)
				{
				F[Solid[i][NY][k]][mi]=0;
				for (int mj=0; mj<19; mj++)
					F[Solid[i][NY][k]][mi]+=MI[mi][mj]*m_l[mj];
				}
			}
			else
			{
			u_ls[0]=0.0;u_ls[1]=0.0;u_ls[2]=0.0;
			for (int ks=0;ks<Q;ks++)
				F[Solid[i][NY][k]][ks]=feq(ks,rho_yp,u_ls);
			
			}

		

if (yn==1)
for (int i=0;i<nx_l;i++)
	for(int k=0;k<=NZ;k++)
	if (Solid[i][0][k]>0)	
		if (Solid[i][1][k]>0)
			{
			for (int mi=0; mi<19; mi++)
					{
					m_l[mi]=0;
					for (int mj=0; mj<19; mj++)
						m_l[mi]+=M[mi][mj]*f[Solid[i][1][k]][mj];
					}

			m_l[0]=rho_yn;
			for (int mi=0; mi<19; mi++)
				{
				F[Solid[i][0][k]][mi]=0;
				for (int mj=0; mj<19; mj++)
					F[Solid[i][0][k]][mi]+=MI[mi][mj]*m_l[mj];
				}
			}
			else
			{
			u_ls[0]=0.0;u_ls[1]=0.0;u_ls[2]=0.0;
			for (int ks=0;ks<Q;ks++)
				F[Solid[i][0][k]][ks]=feq(ks,rho_yn,u_ls);
			
			}




if (zp==1)
for (int i=0;i<nx_l;i++)
	for(int j=0;j<=NY;j++)
	if (Solid[i][j][NZ]>0)
		if (Solid[i][j][NZ-1]>0)
			{
			for (int mi=0; mi<19; mi++)
					{
					m_l[mi]=0;
					for (int mj=0; mj<19; mj++)
						m_l[mi]+=M[mi][mj]*f[Solid[i][j][NZ-1]][mj];
					}

			m_l[0]=rho_zp;
			for (int mi=0; mi<19; mi++)
				{
				F[Solid[i][j][NZ]][mi]=0;
				for (int mj=0; mj<19; mj++)
					F[Solid[i][j][NZ]][mi]+=MI[mi][mj]*m_l[mj];
				}
			}
			else
			{
			u_ls[0]=0.0;u_ls[1]=0.0;u_ls[2]=0.0;
			for (int ks=0;ks<Q;ks++)
				F[Solid[i][j][NZ]][ks]=feq(ks,rho_zp,u_ls);
			
			}




if (zn==1)
for (int i=0;i<nx_l;i++)
	for(int j=0;j<=NY;j++)
	if (Solid[i][j][0]>0)
		if (Solid[i][j][1]>0)
			{
			for (int mi=0; mi<19; mi++)
					{
					m_l[mi]=0;
					for (int mj=0; mj<19; mj++)
						m_l[mi]+=M[mi][mj]*f[Solid[i][j][1]][mj];
					}

			m_l[0]=rho_zn;
			for (int mi=0; mi<19; mi++)
				{
				F[Solid[i][j][0]][mi]=0;
				for (int mj=0; mj<19; mj++)
					F[Solid[i][j][0]][mi]+=MI[mi][mj]*m_l[mj];
				}
			}
			else
			{
			u_ls[0]=0.0;u_ls[1]=0.0;u_ls[2]=0.0;
			for (int ks=0;ks<Q;ks++)
				F[Solid[i][j][0]][ks]=feq(ks,rho_zn,u_ls);
			
			}



if ((xp==1) && (rank==mpi_size-1))
for (int j=0;j<=NY;j++)
	for (int k=0;k<=NZ;k++)
	if (Solid[nx_l-1][j][k]>0)
		if (Solid[nx_l-2][j][k]>0)
			{
			for (int mi=0; mi<19; mi++)
					{
					m_l[mi]=0;
					for (int mj=0; mj<19; mj++)
						m_l[mi]+=M[mi][mj]*f[Solid[nx_l-2][j][k]][mj];
					}

			m_l[0]=rho_xp;
			for (int mi=0; mi<19; mi++)
				{
				F[Solid[nx_l-1][j][k]][mi]=0;
				for (int mj=0; mj<19; mj++)
					F[Solid[nx_l-1][j][k]][mi]+=MI[mi][mj]*m_l[mj];
				}
			}
			else
			{
			u_ls[0]=0.0;u_ls[1]=0.0;u_ls[2]=0.0;
			for (int ks=0;ks<Q;ks++)
				F[Solid[nx_l-1][j][NZ]][ks]=feq(ks,rho_xp,u_ls);
			
			}
		
		


if ((xn==1) && (rank==0))
for (int j=0;j<=NY;j++)
	for(int k=0;k<=NZ;k++)
	if (Solid[0][j][k]>0)
		if (Solid[1][j][k]>0)
			{
			for (int mi=0; mi<19; mi++)
					{
					m_l[mi]=0;
					for (int mj=0; mj<19; mj++)
						m_l[mi]+=M[mi][mj]*f[Solid[1][j][k]][mj];
					}

			m_l[0]=rho_xn;
			for (int mi=0; mi<19; mi++)
				{
				F[Solid[0][j][k]][mi]=0;
				for (int mj=0; mj<19; mj++)
					F[Solid[0][j][k]][mi]+=MI[mi][mj]*m_l[mj];
				}
			}
			else
			{
			u_ls[0]=0.0;u_ls[1]=0.0;u_ls[2]=0.0;
			for (int ks=0;ks<Q;ks++)
				F[Solid[0][j][NZ]][ks]=feq(ks,rho_xn,u_ls);
			
			}
	

}		

//Non-equilibrium boundary condition
if (Sub_BC==3)
{
if (yp==1)
for (int i=0;i<nx_l;i++)
	for(int k=0;k<=NZ;k++)	
	if (Solid[i][NY][k]>0)
		for (int ks=0;ks<Q;ks++)
			{
				if (Solid[i][NY-1][k]>0)
					{
					u_ls[0]=u[Solid[i][NY-1][k]][0];
					u_ls[1]=u[Solid[i][NY-1][k]][1];
					u_ls[2]=u[Solid[i][NY-1][k]][2];
					F[Solid[i][NY][k]][ks]=feq(ks,rho_yp,u_ls)+f[Solid[i][NY-1][k]][ks]-feq(ks,rho[Solid[i][NY-1][k]],u[Solid[i][NY-1][k]]);
					}
				else
					{
					u_ls[0]=0.0;
					u_ls[1]=0.0;u_ls[2]=0.0;
					F[Solid[i][NY][k]][ks]=feq(ks,rho_yp,u_ls);
					}
			}


if (yn==1)
for (int i=0;i<nx_l;i++)
	for(int k=0;k<=NZ;k++)	
	if (Solid[i][0][k]>0)
		for (int ks=0;ks<Q;ks++)
			{
				if (Solid[i][1][k]>0)
					{
					u_ls[0]=u[Solid[i][1][k]][0];
					u_ls[1]=u[Solid[i][1][k]][1];
					u_ls[2]=u[Solid[i][1][k]][2];
					F[Solid[i][0][k]][ks]=feq(ks,rho_yn,u_ls)+f[Solid[i][1][k]][ks]-feq(ks,rho[Solid[i][1][k]],u[Solid[i][1][k]]);
					}
			else
					{
					u_ls[0]=0.0;
					u_ls[1]=0.0;u_ls[2]=0.0;
					F[Solid[i][0][k]][ks]=feq(ks,rho_yn,u_ls);
					}
			}

if (zp==1)
for (int i=0;i<nx_l;i++)
	for(int j=0;j<=NY;j++)
	if (Solid[i][j][NZ]>0)
		for (int ks=0;ks<Q;ks++)
			{			
			if (Solid[i][j][NZ-1]>0)
				{
				u_ls[0]=u[Solid[i][j][NZ-1]][0];
				u_ls[1]=u[Solid[i][j][NZ-1]][1];
				u_ls[2]=u[Solid[i][j][NZ-1]][2];
				F[Solid[i][j][NZ]][ks]=feq(ks,rho_zp,u_ls)+f[Solid[i][j][NZ-1]][ks]-feq(ks,rho[Solid[i][j][NZ-1]],u[Solid[i][j][NZ-1]]);
				}
				else
				{
				u_ls[0]=0.0;
				u_ls[1]=0.0;u_ls[2]=0.0;
				F[Solid[i][j][NZ]][ks]=feq(ks,rho_zp,u_ls);
				}
			}




if (zn==1)
for (int i=0;i<nx_l;i++)
	for(int j=0;j<=NY;j++)
	if (Solid[i][j][0]>0)
		for (int ks=0;ks<Q;ks++)
		{
			if (Solid[i][j][1]>0)
			{
			u_ls[0]=u[Solid[i][j][1]][0];
			u_ls[1]=u[Solid[i][j][1]][1];
			u_ls[2]=u[Solid[i][j][1]][2];
			F[Solid[i][j][0]][ks]=feq(ks,rho_zn,u_ls)+f[Solid[i][j][1]][ks]-feq(ks,rho[Solid[i][j][1]],u[Solid[i][j][1]]);
			}
			else
			{
			u_ls[0]=0.0;
			u_ls[1]=0.0;u_ls[2]=0.0;
			F[Solid[i][j][0]][ks]=feq(ks,rho_zn,u_ls);
			}
		
		}

if ((xp==1) && (rank==mpi_size-1))
for (int j=0;j<=NY;j++)
	for (int k=0;k<=NZ;k++)
	if (Solid[nx_l-1][j][k]>0)
		for (int ks=0;ks<Q;ks++)			
			{
				if (Solid[nx_l-2][j][k]>0)
					{
					u_ls[0]=u[Solid[nx_l-2][j][k]][0];
					u_ls[1]=u[Solid[nx_l-2][j][k]][1];
					u_ls[2]=u[Solid[nx_l-2][j][k]][2];
					F[Solid[nx_l-1][j][k]][ks]=feq(ks,rho_xp,u_ls)+f[Solid[nx_l-2][j][k]][ks]-feq(ks,rho[Solid[nx_l-2][j][k]],u[Solid[nx_l-2][j][k]]);
					//F[Solid[nx_l-1][j][k]][ks]=feq(ks,rho_xp,u_ls);
					}
				else	
					{
					u_ls[0]=0.0;u_ls[1]=0.0;u_ls[2]=0.0;
					F[Solid[nx_l-1][j][k]][ks]=feq(ks,rho_xp,u_ls);
					}

			}
		


if ((xn==1) && (rank==0))
for (int j=0;j<=NY;j++)
	for(int k=0;k<=NZ;k++)
	if (Solid[0][j][k]>0)
		for (int ks=0;ks<Q;ks++)
			{
				if(Solid[1][j][k]>0)
					{
					u_ls[0]=u[Solid[1][j][k]][0];
					u_ls[1]=u[Solid[1][j][k]][1];
					u_ls[2]=u[Solid[1][j][k]][1];
					F[Solid[0][j][k]][ks]=feq(ks,rho_xn,u_ls)+f[Solid[1][j][k]][ks]-feq(ks,rho[Solid[1][j][k]],u[Solid[1][j][k]]);
					//f[Solid[0][j][k]][ks]=feq(ks,rho_xn,u_ls);
					}
				else
					{
					u_ls[0]=0.0;
					u_ls[1]=0.0;
					u_ls[2]=0.0;
					F[Solid[0][j][k]][ks]=feq(ks,rho_xn,u_ls);
					}

			}

	
	
}	
}


double Error(double** u,double** u0,double *v_max,double* u_average)
{	
	int rank = MPI :: COMM_WORLD . Get_rank ();
	int mpi_size=MPI :: COMM_WORLD . Get_size ();
	double *rbuf,*um,*uave,u_compt;
	double temp1,temp2,temp3;
	temp1=0;
	temp2=0;
	temp3=0;
	double error_in;
	double u_max1;
	

	rbuf=new double[mpi_size];
	um = new double[mpi_size];
	uave = new double[mpi_size];
	u_max1=0;
	*v_max=0;




//MPI_Barrier(MPI_COMM_WORLD);



for(int i=1; i<Count; i++)
			{	

			temp1+=(u[i][0]-u0[i][0])*(u[i][0]-u0[i][0])+(u[i][1]-u0[i][1])*(u[i][1]-u0[i][1])+(u[i][2]-u0[i][2])*(u[i][2]-u0[i][2]);
			temp2 += u[i][0]*u[i][0]+u[i][1]*u[i][1]+u[i][2]*u[i][2];	
			temp3+=sqrt(u[i][0]*u[i][0]+u[i][1]*u[i][1]+u[i][2]*u[i][2]);
			if (u[i][0]*u[i][0]+u[i][1]*u[i][1]+u[i][2]*u[i][2]>u_max1)
				u_max1=u[i][0]*u[i][0]+u[i][1]*u[i][1]+u[i][2]*u[i][2];
			
			}
		
		temp1=sqrt(temp1);
		temp2=sqrt(temp2);
		error_in=temp1/(temp2+1e-30);
		u_max1=sqrt(u_max1);

		MPI_Barrier(MPI_COMM_WORLD);
		
	MPI_Gather(&error_in,1,MPI_DOUBLE,rbuf,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
		
	MPI_Gather(&u_max1,1,MPI_DOUBLE,um,1,MPI_DOUBLE,0,MPI_COMM_WORLD);

	MPI_Gather(&temp3,1,MPI_DOUBLE,uave,1,MPI_DOUBLE,0,MPI_COMM_WORLD);

	u_compt=0;
	if (rank==0)
	    for (int i=0;i<mpi_size;i++)
		{
		if (rbuf[i]>error_in)
			error_in=rbuf[i];
		if (um[i]>*v_max)
			*v_max=um[i];
		u_compt+=uave[i];

		}

	u_compt/=(NX+1)*(NY+1)*(NZ+1);
	*u_average=u_compt;
	
	delete [] rbuf;
	delete [] um;
	delete [] uave;

	//MPI_Barrier(MPI_COMM_WORLD);

	return(error_in);

}




void Geometry(int*** Solid)	
{	
	int rank = MPI :: COMM_WORLD . Get_rank ();
	int mpi_size=MPI :: COMM_WORLD . Get_size ();
	MPI_Status status;
	MPI_Request request;
	
	const int root_rank=0;

	int* send;
	int* rece;
	
	int nx_g[mpi_size];
	int disp[mpi_size];

	
	MPI_Gather(&nx_l,1,MPI_INT,nx_g,1,MPI_INT,root_rank,MPI_COMM_WORLD);
	
	if (rank==root_rank)
		{
		disp[0]=0;
	
		for (int i=1;i<mpi_size;i++)
			disp[i]=disp[i-1]+nx_g[i-1];
		}

	
	MPI_Bcast(&disp,mpi_size,MPI_INT,0,MPI_COMM_WORLD);
	
	
	
	int NX0=NX+1;
	int NY0=NY+1;
	int NZ0=NZ+1;

if (mir==0)
	{	
	if (mirX==1)
		NX0=NX0/2;
	if (mirY==1)
		NY0=NY0/2;
	if (mirZ==1)
		NZ0=NZ0/2;
	}


	ostringstream name;
	name<<outputfile<<"LBM_Geometry"<<".vtk";
	if (rank==root_rank)
	{

	ofstream out;
	out.open(name.str().c_str());
	out<<"# vtk DataFile Version 2.0"<<endl;
	out<<"J.Yang Lattice Boltzmann Simulation 3D Single Phase-Solid-Geometry"<<endl;
	out<<"ASCII"<<endl;
	out<<"DATASET STRUCTURED_POINTS"<<endl;
	out<<"DIMENSIONS         "<<NZ0<<"         "<<NY0<<"         "<<NX0<<endl;
	out<<"ORIGIN 0 0 0"<<endl;
	out<<"SPACING 1 1 1"<<endl;
	out<<"POINT_DATA     "<<NX0*NY0*NZ0<<endl;
	out<<"SCALARS sample_scalars float"<<endl;
	out<<"LOOKUP_TABLE default"<<endl;
	out.close();
	}
	
	
	send = new int[nx_l*NY0*NZ0];
	for (int i=0;i<nx_l;i++)
		for (int j=0;j<NY0;j++)
			for (int k=0;k<NZ0;k++)
			send[i*NY0*NZ0+j*NZ0+k]=Solid[i][j][k];
		
	
	if (rank==0)
	{
	ofstream out(name.str().c_str(),ios::app);
	for(int i=0;i<nx_l;i++)
        	for(int j=0; j<NY0; j++)
			for(int k=0;k<NZ0;k++)
			if (Solid[i][j][k]<=0)
				out<<1<<endl;
			else
				out<<0<<endl;
	out.close();
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	for (int processor=1;processor<mpi_size;processor++)
	{	
		
		if (rank==0)
			rece = new int[nx_g[processor]*NY0*NZ0];
		
		
		MPI_Barrier(MPI_COMM_WORLD);
		if (rank==processor)
			MPI_Isend(send,nx_l*NY0*NZ0,MPI_INT,0,processor,MPI_COMM_WORLD,&request);
			
		
		
		if (rank==0)
			MPI_Irecv(rece,nx_g[processor]*NY0*NZ0,MPI_INT,processor,processor,MPI_COMM_WORLD,&request);
		
		
		if ((rank==0) or (rank==processor))
			MPI_Wait(&request,&status);
			
		
		if (rank==0)
		{
		ofstream out(name.str().c_str(),ios::app);
		
		for(int i=0;i<nx_g[processor];i++)
			if (i+disp[processor]<NX0)
        		for(int j=0; j<NY0; j++)
				for(int k=0;k<NZ0;k++)

				{
				if (rece[i*NY0*NZ0+j*NZ0+k]<=0)
					out<<1<<endl;
				else
					out<<0<<endl;
				}
	
		out.close();
		}
		
		
		if (rank==0)
			delete [] rece;
		
	}

	delete [] send;
	
	
		
}




void output_velocity(int m,double* rho,double** u,int MirX,int MirY,int MirZ,int mir,int*** Solid)	
{

	int rank = MPI :: COMM_WORLD . Get_rank ();
	const int mpi_size=MPI :: COMM_WORLD . Get_size ();
	const int root_rank=0;
	
	int nx_g[mpi_size];
	int disp[mpi_size];
	        
	MPI_Status status;
	MPI_Request request;

	double* send;
	double* rece;
	
	
	MPI_Gather(&nx_l,1,MPI_INT,nx_g,1,MPI_INT,root_rank,MPI_COMM_WORLD);
	
	
	if (rank==root_rank)
		{
		disp[0]=0;

		for (int i=1;i<mpi_size;i++)
			disp[i]=disp[i-1]+nx_g[i-1];
		}
	

	

	int NX0=NX+1;
	int NY0=NY+1;
	int NZ0=NZ+1;

if (mir==0)
	{	
	if (MirX==1)
		NX0=NX0/2;
	if (MirY==1)
		NY0=NY0/2;
	if (MirZ==1)
		NZ0=NZ0/2;
	}


	ostringstream name;
	name<<outputfile<<"LBM_velocity_Vector_"<<m<<".vtk";
	if (rank==root_rank)
	{

	ofstream out;
	out.open(name.str().c_str());
	out<<"# vtk DataFile Version 2.0"<<endl;
	out<<"J.Yang Lattice Boltzmann Simulation 3D Single Phase-Velocity"<<endl;
	out<<"ASCII"<<endl;
	out<<"DATASET STRUCTURED_POINTS"<<endl;
	out<<"DIMENSIONS         "<<NZ0<<"         "<<NY0<<"         "<<NX0<<endl;
	out<<"ORIGIN 0 0 0"<<endl;
	out<<"SPACING 1 1 1"<<endl;

	out<<"POINT_DATA     "<<NX0*NY0*NZ0<<endl;
	out<<"VECTORS sample_vectors double"<<endl;
	out<<endl;

	out.close();
	}


	send = new double[nx_l*NY0*NZ0*3];
	for (int i=0;i<nx_l;i++)
		for (int j=0;j<NY0;j++)
			for (int k=0;k<NZ0;k++)
			if (Solid[i][j][k]>0)
			{
			send[i*NY0*NZ0*3+j*NZ0*3+k*3]=u[Solid[i][j][k]][0];
			send[i*NY0*NZ0*3+j*NZ0*3+k*3+1]=u[Solid[i][j][k]][1];
			send[i*NY0*NZ0*3+j*NZ0*3+k*3+2]=u[Solid[i][j][k]][2];
			}
			else
			        {
			        send[i*NY0*NZ0*3+j*NZ0*3+k*3]=0.0;
			        send[i*NY0*NZ0*3+j*NZ0*3+k*3+1]=0.0;
			        send[i*NY0*NZ0*3+j*NZ0*3+k*3+2]=0.0;        
			        }
			        
			        
	
			        
	
	if (rank==0)
	{
	ofstream out(name.str().c_str(),ios::app);
	for(int i=0;i<nx_l;i++)
        	for(int j=0; j<NY0; j++)
			for(int k=0;k<NZ0;k++)
			if (Solid[i][j][k]>0)
				out<<u[Solid[i][j][k]][2]<<" "<<u[Solid[i][j][k]][1]<<" "<<u[Solid[i][j][k]][0]<<endl;
			else
				out<<0.0<<" "<<0.0<<" "<<0.0<<endl;
			
	out.close();
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	for (int processor=1;processor<mpi_size;processor++)
	{	
		
		if (rank==0)
			rece = new double[nx_g[processor]*NY0*NZ0*3];
		
		
		MPI_Barrier(MPI_COMM_WORLD);
		if (rank==processor)
			MPI_Isend(send,nx_l*NY0*NZ0*3,MPI_DOUBLE,0,processor,MPI_COMM_WORLD,&request);
		
		
		
		if (rank==0)
		{
		        
			MPI_Irecv(rece,nx_g[processor]*NY0*NZ0*3,MPI_DOUBLE,processor,processor,MPI_COMM_WORLD,&request);
		}
		
		if ((rank==0) or (rank==processor))
			MPI_Wait(&request,&status);
			
		MPI_Barrier(MPI_COMM_WORLD);
		
		if (rank==0)
		{
		ofstream out(name.str().c_str(),ios::app);
		
		for(int i=0;i<nx_g[processor];i++)
			if (i+disp[processor]<NX0)
        		for(int j=0; j<NY0; j++)
				for(int k=0;k<NZ0;k++)
				out<<rece[i*NY0*NZ0*3+j*NZ0*3+k*3+2]<<" "<<rece[i*NY0*NZ0*3+j*NZ0*3+k*3+1]<<" "<<rece[i*NY0*NZ0*3+j*NZ0*3+k*3]<<endl;	

		
		out.close();
		}
		
		
		if (rank==0)
			delete [] rece;
		
	}

	delete [] send;
	

	
/*	ostringstream name2;
	name2<<"LBM_velocity_"<<m<<".out";
	ofstream out2(name2.str().c_str());
	for (int j=0;j<=NY;j++)
		{
		if (Solid[1][j][1]>0)
			out2<<u[Solid[2][j][1]][0]<<endl;
		else
			out2<<0.0<<endl;
		}
*/
	
	

	

		
}


void output_density(int m,double* rho,int MirX,int MirY,int MirZ,int mir,int*** Solid)	
{

	int rank = MPI :: COMM_WORLD . Get_rank ();
	const int mpi_size=MPI :: COMM_WORLD . Get_size ();
	const int root_rank=0;
	
	double rho_0=1.0;
	
	
	MPI_Status status;
	MPI_Request request;

	double* send;
	double* rece;

	int nx_g[mpi_size];
	int disp[mpi_size];

	
	MPI_Gather(&nx_l,1,MPI_INT,nx_g,1,MPI_INT,root_rank,MPI_COMM_WORLD);
	
	
	if (rank==root_rank)
		{
		disp[0]=0;
	
		for (int i=1;i<mpi_size;i++)
			disp[i]=disp[i-1]+nx_g[i-1];
		}
	

	
	int NX0=NX+1;
	int NY0=NY+1;
	int NZ0=NZ+1;

	if (mir==0)
	{	
	if (MirX==1)
		NX0=NX0/2;
	if (MirY==1)
		NY0=NY0/2;
	if (MirZ==1)
		NZ0=NZ0/2;
	}

	ostringstream name;
	name<<outputfile<<"LBM_Density_"<<m<<".vtk";
	if (rank==root_rank)
	{
	ofstream out;
	out.open(name.str().c_str());
	out<<"# vtk DataFile Version 2.0"<<endl;
	out<<"J.Yang Lattice Boltzmann Simulation 3D Single Phase-Solid-Density"<<endl;
	out<<"ASCII"<<endl;
	out<<"DATASET STRUCTURED_POINTS"<<endl;
	out<<"DIMENSIONS         "<<NZ0<<"         "<<NY0<<"         "<<NX0<<endl;
	out<<"ORIGIN 0 0 0"<<endl;
	out<<"SPACING 1 1 1"<<endl;
	out<<"POINT_DATA     "<<NX0*NY0*NZ0<<endl;
	out<<"SCALARS sample_scalars float"<<endl;
	out<<"LOOKUP_TABLE default"<<endl;
	out.close();

	}
        
	send = new double[nx_l*NY0*NZ0];
	for (int i=0;i<nx_l;i++)
		for (int j=0;j<NY0;j++)
			for (int k=0;k<NZ0;k++)
			if (Solid[i][j][k]>0)
			send[i*NY0*NZ0+j*NZ0+k]=rho[Solid[i][j][k]];
			else
			send[i*NY0*NZ0+j*NZ0+k]=rho_0;
		
		
		
	if (rank==0)
	{
	ofstream out(name.str().c_str(),ios::app);
	for(int i=0;i<nx_l;i++)
        	for(int j=0; j<NY0; j++)
			for(int k=0;k<NZ0;k++)
			if (Solid[i][j][k]>0)
				out<<rho[Solid[i][j][k]]<<endl;
			else
				out<<rho_0<<endl;
			
	out.close();
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	for (int processor=1;processor<mpi_size;processor++)
	{	
		
		if (rank==0)
			rece = new double[nx_g[processor]*NY0*NZ0];
		
		
		MPI_Barrier(MPI_COMM_WORLD);
		if (rank==processor)
			MPI_Isend(send,nx_l*NY0*NZ0,MPI_DOUBLE,0,processor,MPI_COMM_WORLD,&request);
		
		
		
		if (rank==0)
		{
		        
			MPI_Irecv(rece,nx_g[processor]*NY0*NZ0,MPI_DOUBLE,processor,processor,MPI_COMM_WORLD,&request);
		}
		
		if ((rank==0) or (rank==processor))
			MPI_Wait(&request,&status);
			
		MPI_Barrier(MPI_COMM_WORLD);
		
		if (rank==0)
		{
		ofstream out(name.str().c_str(),ios::app);
		
		for(int i=0;i<nx_g[processor];i++)
			if (i+disp[processor]<NX0)
        		for(int j=0; j<NY0; j++)
				for(int k=0;k<NZ0;k++)
				out<<rece[i*NY0*NZ0+j*NZ0+k]<<endl;	

		
		out.close();
		}
		
		
		if (rank==0)
			delete [] rece;
		
	}

	delete [] send;

}

void Geometry_b(int*** Solid)	
{	
	int rank = MPI :: COMM_WORLD . Get_rank ();
	int mpi_size=MPI :: COMM_WORLD . Get_size ();

	const int root_rank=0;

	
	int* nx_g = new int[mpi_size];
	int* disp = new int[mpi_size];

	
	MPI_Gather(&nx_l,1,MPI_INT,nx_g,1,MPI_INT,root_rank,MPI_COMM_WORLD);
	
	
	if (rank==root_rank)
		{
		disp[0]=0;
		for (int i=0;i<mpi_size;i++)
			nx_g[i]*=(NY+1)*(NZ+1);

		for (int i=1;i<mpi_size;i++)
			disp[i]=disp[i-1]+nx_g[i-1];
		}


		
	
	int* Solid_storage= new int[nx_l*(NY+1)*(NZ+1)];
	int* rbuf;

	for(int i=0;i<nx_l;i++)
		for(int j=0;j<=NY;j++)
			for(int k=0;k<=NZ;k++)
			if (Solid[i][j][k]<=0)
				Solid_storage[i*(NY+1)*(NZ+1)+j*(NZ+1)+k]=1;
			else
				Solid_storage[i*(NY+1)*(NZ+1)+j*(NZ+1)+k]=0;

	if (rank==root_rank)
		rbuf= new int[(NX+1)*(NY+1)*(NZ+1)];
	
	//MPI_Barrier(MPI_COMM_WORLD);
	MPI_Gatherv(Solid_storage,nx_l*(NY+1)*(NZ+1),MPI_INT,rbuf,nx_g,disp,MPI_INT,root_rank,MPI_COMM_WORLD);
	
	int NX0=NX+1;
	int NY0=NY+1;
	int NZ0=NZ+1;

if (mir==0)
	{	
	if (mirX==1)
		NX0=NX0/2;
	if (mirY==1)
		NY0=NY0/2;
	if (mirZ==1)
		NZ0=NZ0/2;
	}



	if (rank==root_rank)
	{
	ostringstream name;
	name<<outputfile<<"LBM_Geometry"<<".vtk";
	ofstream out;
	out.open(name.str().c_str());
	out<<"# vtk DataFile Version 2.0"<<endl;
	out<<"J.Yang Lattice Boltzmann Simulation 3D Single Phase-Solid-Geometry"<<endl;
	out<<"ASCII"<<endl;
	out<<"DATASET STRUCTURED_POINTS"<<endl;
	out<<"DIMENSIONS         "<<NX0<<"         "<<NY0<<"         "<<NZ0<<endl;
	out<<"ORIGIN 0 0 0"<<endl;
	out<<"SPACING 1 1 1"<<endl;
	out<<"POINT_DATA     "<<NX0*NY0*NZ0<<endl;
	out<<"SCALARS sample_scalars float"<<endl;
	out<<"LOOKUP_TABLE default"<<endl;
for(int k=0;k<NZ0;k++) 
        for(int j=0; j<NY0; j++)
		for(int i=0;i<NX0;i++)
			//for(int k=0;k<=NZ;k++)
			out<<rbuf[i*(NY+1)*(NZ+1)+j*(NZ+1)+k]<<endl;
	out.close();
	
	}
		
	delete [] Solid_storage;
	if (rank==root_rank)
		delete [] rbuf;

	delete [] nx_g;
	delete [] disp;

		
}


void output_velocity_b(int m,double* rho,double** u,int MirX,int MirY,int MirZ,int mir,int*** Solid)	
{

	int rank = MPI :: COMM_WORLD . Get_rank ();
	const int mpi_size=MPI :: COMM_WORLD . Get_size ();
	const int root_rank=0;
	
	int* nx_g = new int[mpi_size];
	int* disp = new int[mpi_size];

	
	MPI_Gather(&nx_l,1,MPI_INT,nx_g,1,MPI_INT,root_rank,MPI_COMM_WORLD);
	
	
	if (rank==root_rank)
		{
		disp[0]=0;
		for (int i=0;i<mpi_size;i++)
			nx_g[i]*=(NY+1)*(NZ+1)*3;

		for (int i=1;i<mpi_size;i++)
			disp[i]=disp[i-1]+nx_g[i-1];
		}
	

	double* rbuf_v;
	double* v_storage = new double[nx_l*(NY+1)*(NZ+1)*3];


	for (int i=0;i<nx_l;i++)
		for (int j=0;j<=NY;j++)
			for(int k=0;k<=NZ;k++)
			{
			if (Solid[i][j][k]>0)
				{
				v_storage[i*(NY+1)*(NZ+1)*3+j*(NZ+1)*3+k*3]=u[Solid[i][j][k]][0];
				v_storage[i*(NY+1)*(NZ+1)*3+j*(NZ+1)*3+k*3+1]=u[Solid[i][j][k]][1];
				v_storage[i*(NY+1)*(NZ+1)*3+j*(NZ+1)*3+k*3+2]=u[Solid[i][j][k]][2];
				}				
			else
				{
				v_storage[i*(NY+1)*(NZ+1)*3+j*(NZ+1)*3+k*3]=0;
				v_storage[i*(NY+1)*(NZ+1)*3+j*(NZ+1)*3+k*3+1]=0;
				v_storage[i*(NY+1)*(NZ+1)*3+j*(NZ+1)*3+k*3+2]=0;
				}
			}

	if (rank==root_rank)
		rbuf_v= new double[(NX+1)*(NY+1)*(NZ+1)*3];


	//MPI_Barrier(MPI_COMM_WORLD);
	MPI_Gatherv(v_storage,nx_l*(NY+1)*(NZ+1)*3,MPI_DOUBLE,rbuf_v,nx_g,disp,MPI_DOUBLE,root_rank,MPI_COMM_WORLD);


	int NX0=NX+1;
	int NY0=NY+1;
	int NZ0=NZ+1;

if (mir==0)
	{	
	if (MirX==1)
		NX0=NX0/2;
	if (MirY==1)
		NY0=NY0/2;
	if (MirZ==1)
		NZ0=NZ0/2;
	}



	if (rank==root_rank)
	{
	ostringstream name;
	name<<outputfile<<"LBM_velocity_Vector_"<<m<<".vtk";
	ofstream out;
	out.open(name.str().c_str());
	out<<"# vtk DataFile Version 2.0"<<endl;
	out<<"J.Yang Lattice Boltzmann Simulation 3D Single Phase-Velocity"<<endl;
	out<<"ASCII"<<endl;
	out<<"DATASET STRUCTURED_POINTS"<<endl;
	out<<"DIMENSIONS         "<<NX0<<"         "<<NY0<<"         "<<NZ0<<endl;
	out<<"ORIGIN 0 0 0"<<endl;
	out<<"SPACING 1 1 1"<<endl;

	out<<"POINT_DATA     "<<NX0*NY0*NZ0<<endl;
	out<<"VECTORS sample_vectors double"<<endl;
	out<<endl;
	//out<<"LOOKUP_TABLE default"<<endl;
	for(int k=0;k<NZ0;k++)
      		for(int j=0; j<NY0; j++)
			{
			for(int i=0;i<NX0;i++)
        		out<<rbuf_v[i*(NY+1)*(NZ+1)*3+j*(NZ+1)*3+k*3]<<" "<<rbuf_v[i*(NY+1)*(NZ+1)*3+j*(NZ+1)*3+k*3+1]<<" "<<rbuf_v[i*(NY+1)*(NZ+1)*3+j*(NZ+1)*3+k*3+2]<<" "<<endl;
			//out<<endl;
			}
			
	out.close();
/*
//=======================================	
	ostringstream name2;
	name2<<"LBM_velocity_"<<m<<".out";
	ofstream out2(name2.str().c_str());
	for (int j=0;j<=NY;j++)
		{
		if (Solid[1][j][1]>0)
			out2<<u[Solid[2][j][1]][0]<<endl;
		else
			out2<<0.0<<endl;
		}
//===================================
*/


	
	}

	if (rank==root_rank)
		{		
		delete [] rbuf_v;
		}
	delete [] nx_g;
	delete [] disp;
	delete [] v_storage;
	

		
}


void output_density_b(int m,double* rho,int MirX,int MirY,int MirZ,int mir,int*** Solid)	
{
        
	int rank = MPI :: COMM_WORLD . Get_rank ();
	const int mpi_size=MPI :: COMM_WORLD . Get_size ();
	const int root_rank=0;
	

	int* nx_g = new int[mpi_size];
	int* disp = new int[mpi_size];

	
	MPI_Gather(&nx_l,1,MPI_INT,nx_g,1,MPI_INT,root_rank,MPI_COMM_WORLD);
	
	
	if (rank==root_rank)
		{
		disp[0]=0;
		for (int i=0;i<mpi_size;i++)
			nx_g[i]*=(NY+1)*(NZ+1);

		for (int i=1;i<mpi_size;i++)
			disp[i]=disp[i-1]+nx_g[i-1];
		}
	

	double* rbuf_rho;
	double* rho_storage = new double[nx_l*(NY+1)*(NZ+1)];


	for (int i=0;i<nx_l;i++)
		for (int j=0;j<=NY;j++)
			for(int k=0;k<=NZ;k++)
			{
			if (Solid[i][j][k]>0)
				rho_storage[i*(NY+1)*(NZ+1)+j*(NZ+1)+k]=rho[Solid[i][j][k]];
			else
				rho_storage[i*(NY+1)*(NZ+1)+j*(NZ+1)+k]=1.0;
			}

	if (rank==root_rank)
		rbuf_rho= new double[(NX+1)*(NY+1)*(NZ+1)];

	
	//MPI_Barrier(MPI_COMM_WORLD);	
	MPI_Gatherv(rho_storage,nx_l*(NY+1)*(NZ+1),MPI_DOUBLE,rbuf_rho,nx_g,disp,MPI_DOUBLE,root_rank,MPI_COMM_WORLD);

	
	int NX0=NX+1;
	int NY0=NY+1;
	int NZ0=NZ+1;

	if (mir==0)
	{	
	if (MirX==1)
		NX0=NX0/2;
	if (MirY==1)
		NY0=NY0/2;
	if (MirZ==1)
		NZ0=NZ0/2;
	}

	if (rank==root_rank)
	{
	
	ostringstream name;
	name<<outputfile<<"LBM_Density_"<<m<<".vtk";
	ofstream out;
	out.open(name.str().c_str());
	out<<"# vtk DataFile Version 2.0"<<endl;
	out<<"J.Yang Lattice Boltzmann Simulation 3D Single Phase-Solid-Density"<<endl;
	out<<"ASCII"<<endl;
	out<<"DATASET STRUCTURED_POINTS"<<endl;
	out<<"DIMENSIONS         "<<NX0<<"         "<<NY0<<"         "<<NZ0<<endl;
	out<<"ORIGIN 0 0 0"<<endl;
	out<<"SPACING 1 1 1"<<endl;
	out<<"POINT_DATA     "<<NX0*NY0*NZ0<<endl;
	out<<"SCALARS sample_scalars float"<<endl;
	out<<"LOOKUP_TABLE default"<<endl;

        for(int k=0;k<NZ0;k++)
      		for(int j=0; j<NY0; j++)
			for(int i=0;i<NX0;i++)
				out<<rbuf_rho[i*(NY+1)*(NZ+1)+j*(NZ+1)+k]<<endl;

	out.close();
				
	}
	
	//int lss=0;
	//if (rank==0)	
	//	for (int i=1;i<=SumCount;i++)
//			lss+=rbuf_rho[i];
//		cout<<lss<<endl;
	//cout<<SumCount<<endl;
	
	if (rank==root_rank)
		{		
		delete [] rbuf_rho;
		}
	delete [] nx_g;
	delete [] disp;
	delete [] rho_storage;

		
}

void output_psi_b(int m,double* psi,int MirX,int MirY,int MirZ,int mir,int*** Solid)	
{
       
	int rank = MPI :: COMM_WORLD . Get_rank ();
	const int mpi_size=MPI :: COMM_WORLD . Get_size ();
	const int root_rank=0;
	

	int* nx_g = new int[mpi_size];
	int* disp = new int[mpi_size];

	
	MPI_Gather(&nx_l,1,MPI_INT,nx_g,1,MPI_INT,root_rank,MPI_COMM_WORLD);
	
	
	if (rank==root_rank)
		{
		disp[0]=0;
		for (int i=0;i<mpi_size;i++)
			nx_g[i]*=(NY+1)*(NZ+1);

		for (int i=1;i<mpi_size;i++)
			disp[i]=disp[i-1]+nx_g[i-1];
		}
	

	double* rbuf_psi;
	double* psi_storage = new double[nx_l*(NY+1)*(NZ+1)];


	for (int i=0;i<nx_l;i++)
		for (int j=0;j<=NY;j++)
			for(int k=0;k<=NZ;k++)
			{
			if (Solid[i][j][k]>0)
				psi_storage[i*(NY+1)*(NZ+1)+j*(NZ+1)+k]=psi[Solid[i][j][k]];
			else
				psi_storage[i*(NY+1)*(NZ+1)+j*(NZ+1)+k]=ref_psi;
			}

	if (rank==root_rank)
		rbuf_psi= new double[(NX+1)*(NY+1)*(NZ+1)];

	
	//MPI_Barrier(MPI_COMM_WORLD);	
	MPI_Gatherv(psi_storage,nx_l*(NY+1)*(NZ+1),MPI_DOUBLE,rbuf_psi,nx_g,disp,MPI_DOUBLE,root_rank,MPI_COMM_WORLD);

	
	int NX0=NX+1;
	int NY0=NY+1;
	int NZ0=NZ+1;

	if (mir==0)
	{	
	if (MirX==1)
		NX0=NX0/2;
	if (MirY==1)
		NY0=NY0/2;
	if (MirZ==1)
		NZ0=NZ0/2;
	}

	if (rank==root_rank)
	{
	
	ostringstream name;
	name<<outputfile<<"LBM_psi_"<<m<<".vtk";
	ofstream out;
	out.open(name.str().c_str());
	out<<"# vtk DataFile Version 2.0"<<endl;
	out<<"J.Yang Lattice Boltzmann Simulation 3D Single Phase-Solid-Density"<<endl;
	out<<"ASCII"<<endl;
	out<<"DATASET STRUCTURED_POINTS"<<endl;
	out<<"DIMENSIONS         "<<NX0<<"         "<<NY0<<"         "<<NZ0<<endl;
	out<<"ORIGIN 0 0 0"<<endl;
	out<<"SPACING 1 1 1"<<endl;
	out<<"POINT_DATA     "<<NX0*NY0*NZ0<<endl;
	out<<"SCALARS sample_scalars double"<<endl;
	out<<"LOOKUP_TABLE default"<<endl;

        for(int k=0;k<NZ0;k++)
      		for(int j=0; j<NY0; j++)
			for(int i=0;i<NX0;i++)
				out<<setiosflags(ios::fixed)<<setprecision(7)<<rbuf_psi[i*(NY+1)*(NZ+1)+j*(NZ+1)+k]<<endl;

	out.close();

	

	/*
	
	ostringstream name2;
	name2<<"LBM_psi_"<<m<<".out";
	ofstream out2(name2.str().c_str());
	for (int j=0;j<=NY;j++)
		{
		if (Solid[2][j][1]>0)
			out2<<j<<" "<<rbuf_psi[2*(NY+1)*(NZ+1)+j*(NZ+1)+1]<<endl;
		else
			out2<<0.0<<endl;
		}

	*/
	
				
	}
	
	
	
	
	if (rank==root_rank)
		{		
		delete [] rbuf_psi;
		}
	delete [] nx_g;
	delete [] disp;
	delete [] psi_storage;

		
}

void output_psi(int m,double* psi,int MirX,int MirY,int MirZ,int mir,int*** Solid)	
{
       
        
	int rank = MPI :: COMM_WORLD . Get_rank ();
	const int mpi_size=MPI :: COMM_WORLD . Get_size ();
	const int root_rank=0;
	
	double psi_0=ref_psi;
	
	
	MPI_Status status;
	MPI_Request request;

	double* send;
	double* rece;

	int nx_g[mpi_size];
	int disp[mpi_size];

	
	MPI_Gather(&nx_l,1,MPI_INT,nx_g,1,MPI_INT,root_rank,MPI_COMM_WORLD);
	
	
	if (rank==root_rank)
		{
		disp[0]=0;
	
		for (int i=1;i<mpi_size;i++)
			disp[i]=disp[i-1]+nx_g[i-1];
		}
	

	
	int NX0=NX+1;
	int NY0=NY+1;
	int NZ0=NZ+1;

	if (mir==0)
	{	
	if (MirX==1)
		NX0=NX0/2;
	if (MirY==1)
		NY0=NY0/2;
	if (MirZ==1)
		NZ0=NZ0/2;
	}

	ostringstream name;
	name<<outputfile<<"LBM_psi_"<<m<<".vtk";
	if (rank==root_rank)
	{
	ofstream out;
	out.open(name.str().c_str());
	out<<"# vtk DataFile Version 2.0"<<endl;
	out<<"J.Yang Lattice Boltzmann Simulation 3D Single Phase-Solid-Density"<<endl;
	out<<"ASCII"<<endl;
	out<<"DATASET STRUCTURED_POINTS"<<endl;
	out<<"DIMENSIONS         "<<NZ0<<"         "<<NY0<<"         "<<NX0<<endl;
	out<<"ORIGIN 0 0 0"<<endl;
	out<<"SPACING 1 1 1"<<endl;
	out<<"POINT_DATA     "<<NX0*NY0*NZ0<<endl;
	out<<"SCALARS sample_scalars double"<<endl;
	out<<"LOOKUP_TABLE default"<<endl;
	out.close();

	}
        
	send = new double[nx_l*NY0*NZ0];
	for (int i=0;i<nx_l;i++)
		for (int j=0;j<NY0;j++)
			for (int k=0;k<NZ0;k++)
			if (Solid[i][j][k]>0)
			send[i*NY0*NZ0+j*NZ0+k]=psi[Solid[i][j][k]];
			else
			send[i*NY0*NZ0+j*NZ0+k]=psi_0;
		
		
		
	if (rank==0)
	{
	ofstream out(name.str().c_str(),ios::app);
	for(int i=0;i<nx_l;i++)
        	for(int j=0; j<NY0; j++)
			for(int k=0;k<NZ0;k++)
			if (Solid[i][j][k]>0)
				out<<psi[Solid[i][j][k]]<<endl;
			else
				out<<psi_0<<endl;
			
	out.close();
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	for (int processor=1;processor<mpi_size;processor++)
	{	
		
		if (rank==0)
			rece = new double[nx_g[processor]*NY0*NZ0];
		
		
		MPI_Barrier(MPI_COMM_WORLD);
		if (rank==processor)
			MPI_Isend(send,nx_l*NY0*NZ0,MPI_DOUBLE,0,processor,MPI_COMM_WORLD,&request);
		
		
		
		if (rank==0)
		{
		        
			MPI_Irecv(rece,nx_g[processor]*NY0*NZ0,MPI_DOUBLE,processor,processor,MPI_COMM_WORLD,&request);
		}
		
		if ((rank==0) or (rank==processor))
			MPI_Wait(&request,&status);
			
		MPI_Barrier(MPI_COMM_WORLD);
		
		if (rank==0)
		{
		ofstream out(name.str().c_str(),ios::app);
		
		for(int i=0;i<nx_g[processor];i++)
			if (i+disp[processor]<NX0)
        		for(int j=0; j<NY0; j++)
				for(int k=0;k<NZ0;k++)
				out<<rece[i*NY0*NZ0+j*NZ0+k]<<endl;	

		
		out.close();
		}
		
		
		if (rank==0)
			delete [] rece;
		
	}

	delete [] send;

}




double Comput_Perm(double** u,double* Permia,int PerDIr, int* SupInv)
{

	int rank = MPI :: COMM_WORLD . Get_rank ();
	int mpi_size=MPI :: COMM_WORLD . Get_size ();


	int nx_g[mpi_size];
	int disp[mpi_size];
	int si,sj,sm;

	double avex,avey,avez;

	MPI_Gather(&nx_l,1,MPI_INT,nx_g,1,MPI_INT,0,MPI_COMM_WORLD);
	
	
	if (rank==0)
		{
		disp[0]=0;
	
		for (int i=1;i<mpi_size;i++)
			disp[i]=disp[i-1]+nx_g[i-1];
		
		}

	MPI_Bcast(disp,mpi_size,MPI_INT,0,MPI_COMM_WORLD);



	double *rbuf;
	rbuf=new double[mpi_size*3];
	double Perm[3];
	double error;
	double Q[3]={0.0,0.0,0.0};

	double dp;
	if (in_BC==0)
	        dp=0;
	else
	switch(PerDIr)
		{
		case 1:
			dp=abs(p_xp-p_xn)*c_s2/(NX+1)/dx;break;
		case 2:
			dp=abs(p_yp-p_yn)*c_s2/(NY+1)/dx;break;
		case 3:
			dp=abs(p_zp-p_zn)*c_s2/(NZ+1)/dx;break;
		default:
			dp=abs(p_xp-p_xn)*c_s2/(NX+1)/dx;
		}
		
	
	if ((par_per_x-1)*(par_per_y-1)*(par_per_z-1)==0)	
	for (int i=1;i<=Count;i++)
	{
		si=(int)(SupInv[i]/((NY+1)*(NZ+1)));
		sj=(int)((SupInv[i]%((NY+1)*(NZ+1)))/(NZ+1));
		sm=(int)(SupInv[i]%(NZ+1)); 
		si+=disp[rank];
		//if (rank==1)
		//cout<<rank<<"  "<<si<<" "<<sj<<" "<<sm<<endl;
		//cout<<si<<"  "<<per_xn<<"  "<<per_xp<<endl;
		if ((si>=per_xn) and (si<=per_xp) and (sj>=per_yn) and (sj<=per_yp) and (sm>=per_zn) and (sm<=per_zp))
		{
	        Q[0]+=u[i][0];
		Q[1]+=u[i][1];
		Q[2]+=u[i][2];
		}


	}
	else
	for (int i=1;i<=Count;i++)
	       {
		Q[0]+=u[i][0];
		Q[1]+=u[i][1];
		Q[2]+=u[i][2];
		} 



		
		
//	for (int i=1;i<=Count;i++)
//		{
//		Q[0]+=u[i][0];
//		Q[1]+=u[i][1];
//		Q[2]+=u[i][2];
//		}

	MPI_Barrier(MPI_COMM_WORLD);

	
	MPI_Gather(&Q,3,MPI_DOUBLE,rbuf,3,MPI_DOUBLE,0,MPI_COMM_WORLD);
	
	if (rank==0)
		{
		Q[0]=0;Q[1]=0;Q[2]=0;
		for (int i=0;i<mpi_size;i++)
			{
			Q[0]+=rbuf[i*3+0];
			Q[1]+=rbuf[i*3+1];
			Q[2]+=rbuf[i*3+2];
			}

		//Perm[0]=Q[0]*(1.0/Zoom)*(1.0/Zoom)/((NX+1)/Zoom*(NY+1)/Zoom*(NZ+1)/Zoom)*(niu)/gx;
		//Perm[1]=Q[1]*(1.0/Zoom)*(1.0/Zoom)/((NX+1)/Zoom*(NY+1)/Zoom*(NZ+1)/Zoom)*(niu)/gy;
		//Perm[2]=Q[2]*(1.0/Zoom)*(1.0/Zoom)/((NX+1)/Zoom*(NY+1)/Zoom*(NZ+1)/Zoom)*(niu)/gz;

		Perm[0]=Q[0]/((per_xp-per_xn+1)*(per_yp-per_yn+1)*(per_zp-per_zn+1))*(niu)/(gx+dp);
		Perm[1]=Q[1]/((per_xp-per_xn+1)*(per_yp-per_yn+1)*(per_zp-per_zn+1))*(niu)/(gy+dp);
		Perm[2]=Q[2]/((per_xp-per_xn+1)*(per_yp-per_yn+1)*(per_zp-per_zn+1))*(niu)/(gz+dp);
		
		switch(PerDIr)
		{
		case 1:
			error=(Perm[0]-Permia[0])/Permia[0];break;
		case 2:
			error=(Perm[1]-Permia[1])/Permia[1];break;
		case 3:
			error=(Perm[2]-Permia[2])/Permia[2];break;
		default:
			error=(Perm[0]-Permia[0])/Permia[0];
		}


		Permia[0]=Perm[0];
		Permia[1]=Perm[1];
		Permia[2]=Perm[2];

		avex=Q[0]/((per_xp-per_xn+1)*(per_yp-per_yn+1)*(per_zp-per_zn+1)*porosity);
		avey=Q[1]/((per_xp-per_xn+1)*(per_yp-per_yn+1)*(per_zp-per_zn+1)*porosity);
		avez=Q[2]/((per_xp-per_xn+1)*(per_yp-per_yn+1)*(per_zp-per_zn+1)*porosity);

		u_ave=sqrt(avex*avex+avey*avey+avez*avez);

		}
	
	delete [] rbuf;
	
	return (error);
	

}


void Statistical_psi(double* psi,int m, int*** Solid)
{
        int rank = MPI :: COMM_WORLD . Get_rank ();
	int mpi_size=MPI :: COMM_WORLD . Get_size ();
	
double* s_psi;
double* rbuf;
int * nx_g;
int* disp;
double EX,DX,EX2;

if (Sub_Dis==1)
        {
                nx_g = new int[mpi_size];
                disp = new int[mpi_size];

	
	MPI_Gather(&nx_l,1,MPI_INT,nx_g,1,MPI_INT,0,MPI_COMM_WORLD);
	
	
	if (rank==0)
		{
		disp[0]=0;
		for (int i=0;i<mpi_size;i++)
			nx_g[i]*=1;

		for (int i=1;i<mpi_size;i++)
			disp[i]=disp[i-1]+nx_g[i-1];
		
		 rbuf = new double[NX+1];
		}
		

                s_psi = new double[nx_l];
                for (int i=0;i<nx_l;i++)
                        {
                        s_psi[i]=0;
                        for (int j=0;j<=NY;j++)
                                for (int k=0;k<=NZ;k++)
                                if (Solid[i][j][k]>0)
                                        s_psi[i]+=psi[Solid[i][j][k]];
                                                        
                        }
               
           MPI_Gatherv(s_psi,nx_l,MPI_DOUBLE,rbuf,nx_g,disp,MPI_DOUBLE,0,MPI_COMM_WORLD);     
       if (rank==0)
	{
	//psi_total=0;
	//for (int i=0;i<=NX;i++)
	//	psi_total+=rbuf[i];

        ostringstream name;
	name<<outputfile<<"Statistical_data_concentration_X_"<<m<<".sta";
	ofstream out;
	out.open(name.str().c_str());
	EX=0;EX2=0;
	for (int i=per_xn;i<=per_xp;i++)
		{
	        out<<rbuf[i]<<endl;
		EX+=i*dx*rbuf[i]/psi_total;
		EX2+=(i*dx*i*dx)*rbuf[i]/psi_total;
		}
	out.close();
	        
		DX=EX2-EX*EX;
		Dispersion=(DX-disp_pre)*0.5/(freDis*dt);
		disp_pre=DX;
        	//cout<<EX<<"    nis   "<<psi_total<<endl;
                delete [] rbuf;




        }
	delete [] s_psi;
	delete [] disp;
	delete [] nx_g;
	         
                
                
        }
        
        
        
        
if (Sub_Dis==2)
        {
               if (rank==0)
                       rbuf = new double [(NY+1)*mpi_size];
   
               s_psi = new double[NY+1];
                for (int j=0;j<=NY;j++)
                {        
                s_psi[j]=0;
                for (int i=0;i<nx_l;i++)
                        for (int k=0;k<=NZ;k++)
                                if(Solid[i][j][k]>0)
                                s_psi[j]+=psi[Solid[i][j][k]];
                                     
                
                }

            MPI_Barrier(MPI_COMM_WORLD);                    
           MPI_Gather(s_psi,NY+1,MPI_DOUBLE,rbuf,NY+1,MPI_DOUBLE,0,MPI_COMM_WORLD);  
            
	if (rank==0)
	{
		 for (int j=0;j<=NY;j++)
		 {
		         s_psi[j]=0;
		         for (int i=0;i<mpi_size;i++)
		                 s_psi[j]+=rbuf[i*(NY+1)+j];
				
		 }
       
        ostringstream name;
	name<<outputfile<<"Statistical_data_concentration_Y_"<<m<<".sta";
	ofstream out;
	out.open(name.str().c_str());
	EX=0;EX2=0;
	for (int j=per_yn;j<=per_yp;j++)
		{
	        out<<s_psi[j]<<endl;
		EX+=j*dx*s_psi[j]/psi_total;
		EX2+=(j*dx*j*dx)*s_psi[j]/psi_total;
		}
	out.close();
		DX=EX2-EX*EX;
		Dispersion=(DX-disp_pre)*0.5/(freDis*dt);	
		disp_pre=DX;
        
                delete [] rbuf;
        }
	delete [] s_psi;
	

}

if (Sub_Dis==3)
        {
               if (rank==0)
                       rbuf = new double [(NZ+1)*mpi_size];
   
               s_psi = new double[NZ+1];
                for (int k=0;k<=NZ;k++)
                {        
                s_psi[k]=0;
                for (int i=0;i<nx_l;i++)
                        for (int j=0;j<=NY;j++)
                                if(Solid[i][j][k]>0)
                                s_psi[k]+=psi[Solid[i][j][k]];
                                     
                
                }

            MPI_Barrier(MPI_COMM_WORLD);                    
           MPI_Gather(s_psi,NZ+1,MPI_DOUBLE,rbuf,NZ+1,MPI_DOUBLE,0,MPI_COMM_WORLD);  
            
	if (rank==0)
	{
		 for (int k=0;k<=NZ;k++)
		 {
		         s_psi[k]=0;
		         for (int i=0;i<mpi_size;i++)
		                 s_psi[k]+=rbuf[i*(NZ+1)+k];
				
		 }
       
        ostringstream name;
	name<<outputfile<<"Statistical_data_concentration_Z_"<<m<<".sta";
	ofstream out;
	EX=0;EX2=0;
	out.open(name.str().c_str());
	for (int k=per_zn;k<=per_zp;k++)
		{
	        out<<s_psi[k]<<endl;
		EX+=k*dx*s_psi[k]/psi_total;
		EX2+=(k*dx*k*dx)*s_psi[k]/psi_total;
		}
	out.close();
	        DX=EX2-EX*EX;
		Dispersion=(DX-disp_pre)*0.5/(freDis*dt);
		disp_pre=DX;
        	
                delete [] rbuf;
        }
	delete [] s_psi;
	

}



}

void Backup_init(double* rho, double** u, double** f,double** fg, double** F, double** Fg,double* rho_r, double* rhor, double* forcex,double* forcey, double* forcez, char backup_rho[128], char backup_velocity[128], char backup_psi[128], char backup_f[128], char backup_g[128])
{	
      
        int rank = MPI :: COMM_WORLD . Get_rank ();
	int mpi_size=MPI :: COMM_WORLD . Get_size ();
	
	
	
	
	double usqr,vsqr,eu;
	double c2,c4;
	
	rho0=1.0;dt=1.0/Zoom;dx=1.0/Zoom;
 	uMax=0.0;

	if (lattice_v==1)
		{dx=dx_input;dt=dt_input;}

	lat_c=dx/dt;
	c_s=lat_c/sqrt(3);
	c_s2=lat_c*lat_c/3;

	c2=lat_c*lat_c;c4=c2*c2;

	//niu=niu;
	tau_f=niu/(c_s2*dt)+0.5;
	tau_s=niu_s/(c_s2*dt)+0.5;
	
	s_v=1/tau_f;
       
	double s_other=8*(2-s_v)/(8-s_v);
	double u_tmp[3];

	S[0]=0;
	S[1]=s_v;
	S[2]=s_v;
	S[3]=0;
	S[4]=s_other;
	S[5]=0;
	S[6]=s_other;
	S[7]=0;
	S[8]=s_other;
	S[9]=s_v;
	S[10]=s_v;
	S[11]=s_v;
	S[12]=s_v;
	S[13]=s_v;
	S[14]=s_v;
	S[15]=s_v;
	S[16]=s_other;
	S[17]=s_other;
	S[18]=s_other;

	if (lattice_v==1)
	{
	
	M_c[0]=1.0;
	M_c[1]=lat_c*lat_c;
	M_c[2]=lat_c*lat_c*lat_c*lat_c;
	M_c[3]=lat_c;
	M_c[4]=lat_c*lat_c*lat_c;
	M_c[5]=lat_c;
	M_c[6]=lat_c*lat_c*lat_c;
	M_c[7]=lat_c;	
	M_c[8]=lat_c*lat_c*lat_c;
	M_c[9]=lat_c*lat_c;
	M_c[10]=lat_c*lat_c*lat_c*lat_c;
	M_c[11]=lat_c*lat_c;
	M_c[12]=lat_c*lat_c*lat_c*lat_c;
	M_c[13]=lat_c*lat_c;
	M_c[14]=lat_c*lat_c;
	M_c[15]=lat_c*lat_c;
	M_c[16]=lat_c*lat_c*lat_c;
	M_c[17]=lat_c*lat_c*lat_c;
	M_c[18]=lat_c*lat_c*lat_c;


	for (int i=0;i<19;i++)
		for (int j=0;j<19;j++)
		M[i][j]*=M_c[i];

	Comput_MI(M,MI);

	}

	for (int i=0;i<19;i++)
		for (int j=0;j<3;j++)
		elat[i][j]=e[i][j]*lat_c;
	
	for (int i=0;i<19;i++)
		for (int j=0;j<19;j++)
		M[i][j]*=M_c[i];
	
	Comput_MI(M,MI);
	
	ostringstream name5;
	name5<<"LBM_checkpoint_fg_"<<mode_backup_ini<<"."<<rank<<".bin_input";
 	ostringstream name4;
	name4<<"LBM_checkpoint_f_"<<mode_backup_ini<<"."<<rank<<".bin_input";
	ostringstream name3;
	name3<<"LBM_checkpoint_psi_"<<mode_backup_ini<<"."<<rank<<".bin_input";
 	ostringstream name2;
	name2<<"LBM_checkpoint_velocity_"<<mode_backup_ini<<"."<<rank<<".bin_input";
	ostringstream name;
	name<<"LBM_checkpoint_rho_"<<mode_backup_ini<<"."<<rank<<".bin_input";
	
	 
	fstream fin;
	fin.open(name.str().c_str(),ios::in);
	fin.read((char *)(&rho[0]), sizeof(double)*(Count+1));
        //	for(int i=1;i<=Count;i++)
        //	        fin >> rho[i];
  
       fin.close();
       
   
	fin.open(name2.str().c_str(),ios::in);
	fin.read((char *)(&u[0][0]), sizeof(double)*(Count+1)*3);
      //  	for(int i=1;i<=Count;i++)
       // 	        fin >> u[i][0] >> u[i][1] >> u[i][2];
  
       fin.close();
       
       fin.open(name3.str().c_str(),ios::in);
       fin.read((char *)(&rho_r[0]), sizeof(double)*(Count+1));
       // 	for(int i=1;i<=Count;i++)
       // 	        cout <<"   ddddddd  "<< rho_r[3]<<endl;
  
       fin.close();
       
       fin.open(name4.str().c_str(),ios::in);
	fin.read((char *)(&f[0][0]), sizeof(double)*(Count+1)*19);
       // 	for(int i=1;i<=Count;i++)
       // 	        fin >> f[i][0] >> f[i][1] >> f[i][2] >> f[i][3] >> f[i][4] >> f[i][5] >>f[i][6] >> f[i][7] >> f[i][8] >> f[i][9] >> f[i][10] >> f[i][11] >> f[i][12] >> f[i][13] >> f[i][14] >> f[i][15] >> f[i][16] >>f[i][17] >> f[i][18] ;
  
       fin.close();
       
       fin.open(name5.str().c_str(),ios::in);
	fin.read((char *)(&fg[0][0]), sizeof(double)*(Count+1)*19);
        //	for(int i=1;i<=Count;i++)
        //	        fin >> fg[i][0] >> fg[i][1] >> fg[i][2] >> fg[i][3] >> fg[i][4] >> fg[i][5] >>fg[i][6] >> fg[i][7] >> fg[i][8] >> fg[i][9] >> fg[i][10] >> fg[i][11] >> fg[i][12] >> fg[i][13] >> fg[i][14] >> fg[i][15] >> fg[i][16] >>fg[i][17] >> fg[i][18] ;
  
       fin.close();

	
       for(int i=1;i<=Count;i++)
		{
		//rhor[i]=0;
		forcex[i]=gx;
		forcey[i]=gy+Gravity*Buoyancy_parameter*rho_r[i];
		forcez[i]=gz;
		
		for (int lm=0;lm<19;lm++)
			{
			//f[i][lm]=feq(lm,rho[i],u_tmp);
			//eu=elat[lm][0]*u[i][0]+elat[lm][1]*u[i][1]+elat[lm][2]*u[i][2];
			//fg[i][lm]=w[lm]*rho_r[i]*(1+3*eu/c2);
			F[i][lm]=f[i][lm];Fg[i][lm]=fg[i][lm];
			//fg[i][lm]=w[mi]*rho_r[ci]*(1+3*eu/c2+4.5*eu*eu/c4-1.5*uu/c2);
			}
		}

	 	
}




void Backup(int m,double* rho,double* psi, double** u, double** f, double** g)
{

        int rank = MPI :: COMM_WORLD . Get_rank ();
        int mpi_size=MPI :: COMM_WORLD . Get_size ();
        
	ostringstream name;
	name<<outputfile<<"LBM_checkpoint_velocity_"<<m<<"."<<rank<<".bin_input";
	ofstream out;
	out.open(name.str().c_str());
	//for (int i=1;i<=Count;i++)
        //		out<<u[i][0]<<" "<<u[i][1]<<" "<<u[i][2]<<" "<<endl;
	
        out.write((char *)(&u[0][0]),sizeof(double)*(Count+1)*3);	
			
	out.close();
	

	
	
	ostringstream name2;
	name2<<outputfile<<"LBM_checkpoint_rho_"<<m<<"."<<rank<<".bin_input";
	ofstream out2;
	out2.open(name2.str().c_str());
	
	//for (int i=1;i<=Count;i++)
        //		out2<<rho[i]<<endl;
	out2.write((char *)(&rho[0]), sizeof(double)*(Count+1));	
			
	out2.close();
	

	
	ostringstream name3;
	name3<<outputfile<<"LBM_checkpoint_psi_"<<m<<"."<<rank<<".bin_input";
	//ofstream out;
	out.open(name3.str().c_str());
	
	//for (int i=1;i<=Count;i++)
        //		out<<psi[i]<<endl;
	//out.write((char *)(&f[0][0]), sizeof(double)*(Count+1)*19);	
	out.write((char *)(&psi[0]), sizeof(double)*(Count+1));	
	
	out.close();
	
	
	ostringstream name4;
	name4<<outputfile<<"LBM_checkpoint_f_"<<m<<"."<<rank<<".bin_input";
	//ofstream out;
	out.open(name4.str().c_str());
	
	//for (int i=1;i<=Count;i++)
	//{
	//        for (int j=0;j<19;j++)
        //		out<<f[i][j]<<" ";
        //out<<endl;
        //}
        out.write((char *)(&f[0][0]), sizeof(double)*(Count+1)*19);      
        out.close();
        
	ostringstream name5;
	name5<<outputfile<<"LBM_checkpoint_fg_"<<m<<"."<<rank<<".bin_input";
	//ofstream out;
	out.open(name5.str().c_str());
	//cout<<disp[mpi_size-1]+c_l[mpi_size-1]<<"    @@@@@@@@@@@@  "<<endl;
	//for (int i=1;i<=Count;i++)
	//{
	   //     for (int j=0;j<19;j++)
        	//	out<<g[i][j]<<" ";
        //out<<endl;
        //}
        
        out.write((char *)(&g[0][0]), sizeof(double)*(Count+1)*19);    
	out.close();


}



void psi_reset(double** u,double** fg, double* rho_r, double* rhor, double*** Psi_local, int* SupInv)
{	
      
        double eu;
	double c2=lat_c*lat_c;
	
	for (int i=1;i<=Count;i++)	
			
		{
			
			rho_r[i]=Psi_local[(int)(SupInv[i]/((NY+1)*(NZ+1)))][(int)((SupInv[i]%((NY+1)*(NZ+1)))/(NZ+1))][SupInv[i]%(NZ+1)];
			
			rhor[i]=0;
			
			

			
			
			//INITIALIZATION OF m and f

			for (int lm=0;lm<19;lm++)
			{
			
			eu=elat[lm][0]*u[i][0]+elat[lm][1]*u[i][1]+elat[lm][2]*u[i][2];
			fg[i][lm]=w[lm]*rho_r[i]*(1+3*eu/c2);
			//fg[i][lm]=w[mi]*rho_r[ci]*(1+3*eu/c2+4.5*eu*eu/c4-1.5*uu/c2);
			}
                	


		
		

	}

	

	 	
} 




