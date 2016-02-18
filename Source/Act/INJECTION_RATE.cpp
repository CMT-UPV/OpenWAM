#pragma hdrstop
// #include "simstruc.h"
#pragma hdrstop
#include<cstdio>
#include<cmath>
#include<string.h>
#include<stdlib.h>
#include<malloc.h>
// #include<conio.h>

using namespace std;

/** ************     FUNCTIONS DECLARATION       ************** */

extern "C" void CALCULUS_OF_INJECTION_RATE(int INITIAL, double*SOP, double*MFI, double*SOI, double*EOI, double speed,
		int CAI, double PCR, double*A, double*B, double*C, double*D, double*injection_rate, double*CAD_injection_rate);

/** ************     MAIN FUNCTION       ************* */

#pragma argsused
// int main(int argc,char **argv)
// {
//
// int counter,test_number,z,p,size_inlet_inj;
// int aux = 0;
// int CAI = 0;                                 /* vectors point number according to crank angle degree  */
// FILE *fich;
//
// char title[3000];
//
//
//
// int *INITIAL;
// double *A;
// double *B;
// double *C;
// double *D;
// double *PCR;
// double *speed;
// double **SOP;
// double **SOI;
// double **EOI;
// double **MFI;
// double PMI_hp = 0.;
//
// double **test_variables;
//
// double **injection_rate;
// double **CAD_injection_rate;
//
//
//
// CAI=1801;
// size_inlet_inj=CAI;
// test_number=1;
// z=0;
//
////instrucction to dimensionate main vectors
//
//
// PCR=(double *)malloc(test_number*sizeof(double ));
// speed=(double *)malloc(test_number*sizeof(double ));
// INITIAL=(int *)malloc(test_number*sizeof(int ));
//
// A=(double *)malloc(2*sizeof(double ));
// B=(double *)malloc(2*sizeof(double ));
// C=(double *)malloc(2*sizeof(double ));
// D=(double *)malloc(2*sizeof(double ));
//
// SOP=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// SOP[z]=(double *)malloc(8*sizeof(double ));
// }
//
// SOI=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// SOI[z]=(double *)malloc(8*sizeof(double ));
// }
// EOI=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// EOI[z]=(double *)malloc(8*sizeof(double ));
// }
// MFI=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// MFI[z]=(double *)malloc(8*sizeof(double ));
// }
//
// test_variables=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// test_variables[z]=(double *)malloc(18*sizeof(double ));
// }
//
//
//
// //the injecition rate law is created with a function
//
// if((fich=fopen("injection_system_parameters.dat","r"))==NULL){
// printf("The file of average variables could not be open.\n");
// }
//
// fscanf(fich, "%s",title);
// fscanf(fich, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",&A[0],&A[1],&B[0],&B[1],&C[0],&C[1],&D[0],&D[1]);
// fclose(fich);
//
//
//
// INITIAL[0]=0;
//
// if((fich=fopen("injection_parameters.dat","r"))==NULL){
// printf("The file of average variables could not be open.\n");
// }
//
// fscanf(fich, "%s",title);
// for (p=0;p<8;p++){
// fscanf(fich, "%d,%lf,%lf,%lf",aux,&SOP[0][p],&MFI[0][p],&PMI_hp);
//
// if(MFI[0][p]>0){
// INITIAL[0]=INITIAL[0]+1;
// }
//
// }
// fclose(fich);
//
//
// if((fich=fopen("test_variables.dat","r"))==NULL){
// printf("The file of average variables could not be open.\n");
// return 0;
// }
// fscanf(fich, "%s",title);
//
// for(p=0;p<18;p++){
// fscanf(fich, "%lf,",&test_variables[0][p]);
// }
//
// fclose(fich);
//
// speed[0]=test_variables[0][0];
// PCR[0]=test_variables[0][5];
//
//
// injection_rate=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// injection_rate[z]=(double *)malloc(CAI*sizeof(double ));
// }
//
// CAD_injection_rate=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// CAD_injection_rate[z]=(double *)malloc(CAI*sizeof(double ));
// }
//
// //initializes injection rate angle
// for (z=0;z<test_number;z++){
// CAD_injection_rate[z][0]=-180;
// for(int counter=1;counter<CAI;counter++){
// CAD_injection_rate[z][counter]=CAD_injection_rate[z][counter-1]+((360./(CAI-1)));
// }
// }
//
// for (z=0;z<test_number;z++){
//
// CALCULUS_OF_INJECTION_RATE(INITIAL[z],SOP[z],MFI[z],SOI[z],EOI[z],speed[z],CAI,
// PCR[z],A,B,C,D,injection_rate[z],CAD_injection_rate[z]);
//
// }
//
//
//
// if((fich=fopen("injection_rate.dat","w"))==NULL){
// printf("The element file results could not be opened");
// exit(1);
// }
// strcpy(title,"CAD,IR");
//
// fprintf(fich, "%s", title);
// for(p=0;p<CAI;p++){
// fprintf(fich, "\n%lf,",CAD_injection_rate[0][p]);
// for(z=0;z<test_number;z++){
// fprintf(fich, "%lf",injection_rate[z][p]);
// }
// }
// fclose(fich);
//
// if((fich=fopen("injection_rate_parameters.dat","w"))==NULL){
// printf("The element file results could not be opened");
// exit(1);
// }
// strcpy(title,"INITIAL,SOI1,EOI1,SOI2,EOI2,SOI3,EOI3,SOI4,EOI4,SOI5,EOI5,SOI6,EOI6,SOI7,EOI7,SOI8,EOI8");
//
// fprintf(fich, "%s", title);
//
// fprintf(fich, "\n%d",INITIAL[0]);
// for(p=0;p<8;p++){
// fprintf(fich, ",%lf,%lf",SOI[0][p],EOI[0][p]);
// }
//
// fclose(fich);
//
////memory liberation
// free(PCR);
// free(INITIAL);
// free(A);
// free(B);
// free(C);
// free(D);
//
//
// for(z=0;z<test_number;z++)free(SOP[z]);
// free(SOP);
// for(z=0;z<test_number;z++)free(SOI[z]);
// free(SOI);
// for(z=0;z<test_number;z++)free(EOI[z]);
// free(EOI);
// for(z=0;z<test_number;z++)free(MFI[z]);
// free(MFI);
//
// for(z=0;z<test_number;z++)free(test_variables[z]);
// free(test_variables);
//
// for (z=0;z<test_number;z++)free(injection_rate[z]);
// free(injection_rate);
// for (z=0;z<test_number;z++)free(CAD_injection_rate[z]);
// free(CAD_injection_rate);
//
//
// }

inline void CALCULUS_OF_INJECTION_RATE(int INITIAL, double *SOP, double *MFI, double *SOI, double *EOI, double speed,
									   int CAI, double PCR, double *A, double *B, double *C, double *D, double *injection_rate, double *CAD_injection_rate)

{

	int k = 0;
	// initialize injection rate
	injection_rate[0] = 0.;
	for(k = 1; k < CAI; k++) {
		injection_rate[k] = 0.;
	}

	double a = (A[1] * PCR + A[0]) * 1e3;
	double b = (B[1] * PCR + B[0]) * 1e3;
	double c = (C[1] * PCR + C[0]) * 1e3;

	// definition of injection rate for each injection

	double mftopemini = 1. / 2. * ((b / a) + (b / -c)) * b;
	double time = 0.;

	for(int j = 0; j < INITIAL; j++) {

		SOI[j] = SOP[j] + (D[1] * PCR + D[0]) * (6 * speed);

		// calculates total fuel mass injected

		if(MFI[j] >= mftopemini) {
			double DLM = (MFI[j] - mftopemini) / b;
			for(int k = 0; k < CAI; k++) {
				// time between SOI and actual point
				time = (CAD_injection_rate[k] - SOI[j]) / (6 * speed);
				// calculates injection rate
				if(time > 0. && time <= b / a)
					injection_rate[k] = time * a;
				else if(time > b / a && time <= b / a + DLM)
					injection_rate[k] = b;
				else if(time > b / a + DLM && time <= b / a + DLM + b / -c)
					injection_rate[k] = b + ((time - b / a - DLM) * c);
			}
			// calculates end of injection
			EOI[j] = SOI[j] + (b / a + DLM + b / -c) * 6 * speed;
		} else {
			double levmaxi = sqrt((2. * MFI[j]) / (1. / a + 1. / -c));
			double DTL = levmaxi / a;
			double DTB = levmaxi / -c;
			for(int k = 0; k < CAI; k++) {
				// time between SOI and actual point
				time = (CAD_injection_rate[k] - SOI[j]) / (6 * speed);
				if(time > 0. && time <= DTL)
					injection_rate[k] = time * a;
				else if(time > DTL && time <= DTL + DTB)
					injection_rate[k] = levmaxi + (time - DTL) * c;
			}
			// calculates end of injection
			EOI[j] = SOI[j] + (DTL + DTB) * 6 * speed;
		}
	}

	// converts mg/s to g/s
	for(int k = 0; k < CAI; k++) {
		injection_rate[k] /= 1000.;
	}

}
