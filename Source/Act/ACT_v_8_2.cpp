#pragma hdrstop
//#include "simstruc.h"
#pragma hdrstop
#include<cstdio>
#include<cmath>
#include<string.h>
#include<stdlib.h>
#include<malloc.h>

using namespace std;
// #include<conio.h>

struct stControlElementComposition {
	int inj_number;
	int num_i;
	double mtotal;
	double mfuel;
	double mfuel_real;
	double mO2;
	double mO2_real;
	double mO;
	double mCH;
	double mCO2;
	double mH2O;
	double TSD;
	double Tadib;
	double dNOx;
	double mNOx;
	double mSOOT_B;
	double mSOOT_A;
	double mSOOT_C;
	double X;
	double FI;
};

/** ************     FUNCTIONS DECLARATION       ************** */

void FUNCTION_FOR_INTERPOLATION(double * interpolated, double * time_interpolated,
	double * CAD_to_interpolate, double * vector_to_interpolate, int size_interpolated,
	int size_to_interpolate, double speed);

void CALCULUS_OF_VIRTUAL_VELOCITY(double * inj_velocity, double * virtual_velocity, double * dmf,
	double * time_vector, double rofuel, double dc, double n_holes, double nozzle_d, double D,
	int size, double PI, double speed, double * EOI_IM, double inj_num, double * SOI_IM,
	double Piston_D, double DBowl, double CTM, double * CAD, double Kswirl);

void CALCULUS_OF_ACCUMULATED_INJ_RATE(double * acu_dmf, double * dmf, double * time_vector,
	int size);

void CALCULUS_OF_REACTION_STOICHIOMETRY(double * O2_mass_fuelunit, double * N2_mass_fuelunit,
	double * CO2_mass_fuelunit, double * H2O_mass_fuelunit, double HC);

void STOICHIOMETRY_CONSTANTS(double HC, double * Kst1, double * Kst2, double * Kst3, double * Kst4,
	double * Kst5, double * Kst6);

void CALCULUS_OF_NUMBER_ELEMENTS(int * num_i_IM, double * time_vector, int size, double speed,
	double * SOI_IM, double * EOI_IM, int inj_num);

void CALCULUS_OF_POI(double * *POI_IM, double * *mfuel_i_IM, double * *mfuel_ij_IM,
	double * acu_dmf, double * time_vector, int size, double speed, int * num_i_IM,
	int num_j, double * SOI_IM, double * EOI_IM, int inj_num,
	stControlElementComposition * *elementcontrol);

double VOLUME(double CAD, double VTDC, double PI, double Piston_D, double Crank_L,
	double Connecting_Rod_L, double E);

void CALCULATE_CYCLE(double * roair, double * CAD, double delta_t, double * V_cyl, double VTDC,
	int counter, double speed, double * p_cyl, double * HRF, double * acu_dmf, double * Mbb,
	double * acu_Mbb, double AFe, double f, double mfuel, double mEGR, double mairIVC,
	double * T_cyl, double HP, double * Yair, double * Yfuel, double * Yburned, double * U,
	double * CV, double * H_cooler, double * H, double TEB, double inj_fuel_temp,
	double PRECITERACIONES, double * defor, double * Rmixture, double Atmosphere_press,
	double * Gamma, double PI, double Runiv, double Piston_D, double S, double Crank_L,
	double Connecting_Rod_L, double E, double Piston_Axis_D, double Piston_Crown_H,
	double DBowl, double VBowl, double M_Connecting_Rod, double M_P_R_PA, double MW_air,
	double MW_fuel, double MW_burned, double C_ESteel, double C_Mech_Defor, double CTM,
	double WC1A, double WC1B, double C2, double C_MBLBY, double Cbb, double TPIS,
	double TCYL_HEAD, double TCYL, double * Qcylhead, double * Qcyl, double * Qpis);

void DEFORMATIONS(double * V_cyl, double * DEFOR, double p_cyl, double CAD, double delta_CAD,
	double speed, double PI, double Piston_D, double S, double Connecting_Rod_L, double E,
	double Piston_Axis_D, double Piston_Crown_H, double M_Connecting_Rod, double M_P_R_PA,
	double C_ESteel, double C_Mech_Defor);

void MASIC_RATIO(double * Yair, double * Yfuel, double * Yburned, double * Rmixture, double HRF,
	double acu_mf, double acu_Mbb, double AFe, double f, double mfuel, double mEGR, double mairIVC,
	double Runiv, double MW_air, double MW_fuel, double MW_burned);

void PROPERTIES(double * u, double * CV, double T_cyl, double T_cyl_pre, double Yair, double Yfuel,
	double Yburned);

void PROPERTIES_FUEL(double * uf, double T_cyl);

double HEAT_COOLER(double p_cyl, double pressureIVC, double T_cyl, double temperatureIVC,
	double average_Volume, double volumeIVC, double delta_CAD, double speed, double VTDC,
	double * H, double PI, double Piston_D, double S, double DBowl, double VBowl, double CTM,
	double WC1A, double WC1B, double C2, double TPIS, double TCYL_HEAD, double TCYL,
	double CAD, double * Qcylhead, double * Qcyl, double * Qpis, int counter);

double CALCULATE_C1(double cm, double CTM, double WC1A, double WC1B, double Piston_D, double DBowl,
	double speed, double CAD, double PI);

double BLOW_BY(double p_cyl, double T_cyl, double Rmixture, double delta_CAD, double speed,
	double Gamma, double Atmosphere_press, double Piston_D, double C_MBLBY, double Cbb);

void CALCULATE_AREAS(double * Piston_area, double * Cylinder_head_area, double PI, double Piston_D,
	double DBowl, double VBowl);

void CALCULUS_OF_MEAN_VARIABLES(double * p_cyl, double * T_cyl, double * dp_da_cyl, double * CAD,
	double * pmax, double * Tmax, double * dp_da_max, double * p_exit, double * T_exit, int size);

void CALCULUS_OF_IMP_HP(double * complete_p_cyl, double * complete_CAD, double * p_cyl,
	double * V_cyl, double * complete_V_cyl, double * complete_deform, double * WI_HP,
	double * IMP_HP, int complete_size, int complete_prev_size, double delta_t, double speed,
	int size, double IVC, double EVO, double VTDC, double Cylinder_capacity, double PI,
	double Piston_D, double S, double Crank_L, double Connecting_Rod_L, double E,
	double Piston_Axis_D, double Piston_Crown_H, double M_Connecting_Rod, double M_P_R_PA,
	double C_ESteel, double C_Mech_Defor, double inlet_pres, double exhaust_pres);

void FUNCTION_NOX(double * YNOeq_value, double * KdYNO_value, double * *YNOeq, double * *KdYNO,
	double temperature, double mO2, double mtotal);

void FUNCTION_SOOT_C(double * soot_pre, double element_FI);

extern "C" void ACT(double * engine_parameters, double * engine_model_constants,
	double * test_variables, double * injection_rate, double * CAD_injection_rate,
	int size_inlet_inj, int INITIAL, double * SOI, double * EOI, int CAI,
	double * CAD_exit, double * HRF_exit, double * ROHR_exit, double * p_cyl_exit,
	double * dp_da_cyl_exit, double * T_cyl_exit, double * H_cooler_exit, double * mean_var_exit,
	double * heat_transfer, double * injection_rate_exit, double * accum_injection_rate_exit,
	double * species_EVO_exit);

double min(double a, double b);

/** ************     MAIN FUNCTION       ************* */

#pragma argsused
// int main(int argc,char **argv)
// {
//
// int counter,test_number,z,p,size_inlet_inj;
// double NOx_before;
// double aux;
// double CAI;                                 /* vectors point number according to crank angle degree  */
// FILE *fich;
//
//
// char title[3000];
//
// double *engine_parameters;               /* Inlet vector of motor`s constant parameters  */
// double *engine_model_constants;
// double **test_variables;                   /* Inlet vector of test variables*/
// double **injection_rate;
// double **CAD_injection_rate;
// int *INITIAL;
// double **SOI;
// double **EOI;
//
//
//
// double **CAD_exit;                         /* Crank angle degrees (�) [-180 - +180] used to shown the results */
// double **HRF_exit;                         /* Heat release fraction(-) (FQL) [-180 +180]*/
// double **ROHR_exit;
// double **p_cyl_exit;                       /* In-cylinder pressure (Pa) [-180 - +180]*/
// double **dp_da_cyl_exit;
// double **T_cyl_exit;                       /* Average in-cylinder temperature (K) [-180 - +180]*/
// double **H_cooler_exit;                    /* Heat hung over to the cooler (J/�) [-180 - +180] */
// double **mean_var_exit;                    /* Output vector of mean variables*/
// double **injection_rate_exit;              /* Output vector of mean variables*/
// double **accum_injection_rate_exit;
// double **heat_transfer;
// double **species_EVO_exit;                   /* Species mass fraction at EVO: the order is
// N2, O2, CO2, H2O, NOx, CO, Soot and HC */
//
//
//
// double rateBG;
//
//
//
// CAI=1801;
// test_number=1;
//
////instrucction to dimensionate main vectors
//
//
// engine_parameters=(double *)malloc(25*sizeof(double ));
//
// engine_model_constants=(double *)malloc(22*sizeof(double ));
//
// test_variables=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// test_variables[z]=(double *)malloc(18*sizeof(double ));
// }
//
// CAD_exit=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// CAD_exit[z]=(double *)malloc(CAI*sizeof(double ));
// }
// HRF_exit=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// HRF_exit[z]=(double *)malloc(CAI*sizeof(double ));
// }
// ROHR_exit=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// ROHR_exit[z]=(double *)malloc(CAI*sizeof(double ));
// }
// p_cyl_exit=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// p_cyl_exit[z]=(double *)malloc(CAI*sizeof(double ));
// }
// dp_da_cyl_exit=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// dp_da_cyl_exit[z]=(double *)malloc(CAI*sizeof(double ));
// }
// T_cyl_exit=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// T_cyl_exit[z]=(double *)malloc(CAI*sizeof(double ));
// }
//
// injection_rate_exit=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// injection_rate_exit[z]=(double *)malloc(CAI*sizeof(double ));
// }
// accum_injection_rate_exit=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// accum_injection_rate_exit[z]=(double *)malloc(CAI*sizeof(double ));
// }
// H_cooler_exit=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// H_cooler_exit[z]=(double *)malloc(CAI*sizeof(double ));
// }
// mean_var_exit=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// mean_var_exit[z]=(double *)malloc(10*sizeof(double ));
// }
//
// heat_transfer=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// heat_transfer[z]=(double *)malloc(4*sizeof(double ));
// }
//
//
//
// INITIAL=(int *)malloc(test_number*sizeof(int ));
//
// SOI=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// SOI[z]=(double *)malloc(8*sizeof(double ));
// }
// EOI=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// EOI[z]=(double *)malloc(8*sizeof(double ));
// }
//
// species_EVO_exit=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// species_EVO_exit[z]=(double *)malloc(8*sizeof(double )); // There are 8 species
// }
//
//
//
// // engine parameters: geometry parameters and combustion model constants
//
// if((fich=fopen("engine_parameters.dat","r"))==NULL){
// printf("The file of average variables could not be open.\n");
// return 0;
// }
// fscanf(fich, "%s",title);
// for(counter=0;counter<25;counter++){
// fscanf(fich, "%lf,",&engine_parameters[counter]);
// }
// fclose(fich);
//
// // engine model constants
//
// if((fich=fopen("engine_model_constants.dat","r"))==NULL){
// printf("The file of average variables could not be open.\n");
// return 0;
// }
// fscanf(fich, "%s",title);
// for(counter=0;counter<22;counter++){
// fscanf(fich, "%lf,",&engine_model_constants[counter]);
// }
// fclose(fich);
//
//
//
// // test variables
//
// if((fich=fopen("test_variables.dat","r"))==NULL){
// printf("The file of average variables could not be open.\n");
// return 0;
// }
// fscanf(fich, "%s",title);
// for(z=0;z<test_number;z++){
// for(p=0;p<18;p++){
// fscanf(fich, "%lf,",&test_variables[z][p]);
// }
// }
// fclose(fich);
//
//
//
// if((fich=fopen("injection_rate.dat","r"))==NULL){
// printf("The file of average variables could not be open.\n");
//
// }
// size_inlet_inj=0;
// fscanf(fich,"%s",title);
// while (!feof(fich)){
// for(z=0;z<test_number;z++){
// fscanf(fich,"%lf,%lf",&aux,&aux);
// }
// size_inlet_inj++;
// }
// fclose(fich);
// size_inlet_inj--;
//
// injection_rate=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// injection_rate[z]=(double *)malloc(size_inlet_inj*sizeof(double ));
// }
//
// CAD_injection_rate=(double **)malloc(test_number*sizeof(double ));
// for (z=0;z<test_number;z++){
// CAD_injection_rate[z]=(double *)malloc(size_inlet_inj*sizeof(double ));
// }
//
//
// if((fich=fopen("injection_rate.dat","r"))==NULL){
// printf("The file of average variables could not be open.\n");
//
// }
//
// fscanf(fich, "%s",title);
// for(p=0;p<size_inlet_inj;p++){
// fscanf(fich, "%lf,",&CAD_injection_rate[0][p]);
// for(z=0;z<test_number;z++){
// fscanf(fich, "%lf,",&injection_rate[z][p]);
// }
// }
// fclose(fich);
//
// //injection rate parameters: injection number, start of injections and end of injections
//
// if((fich=fopen("injection_rate_parameters.dat","r"))==NULL){
// printf("The file of average variables could not be open.\n");
// }
//
// fscanf(fich, "%s",title);
// for(z=0;z<test_number;z++){
// fscanf(fich, "%d,",&INITIAL[z]);
// for(p=0;p<8;p++){
// fscanf(fich, "%lf,%lf,",&SOI[z][p],&EOI[z][p]);
// }
// }
// fclose(fich);
//
//
//
////main function called with a test number bucle
//
// for(z=0;z<test_number;z++){
//
//
// //it will make a iteration to calculate NOx emissions with EGR
//
// NOx_before=1e-4;
//
// while((fabs(mean_var_exit[z][7]-NOx_before)*100/NOx_before>1)){
//
// NOx_before=mean_var_exit[z][7];
//
//
// ACT(engine_parameters,engine_model_constants,test_variables[z],injection_rate[z],CAD_injection_rate[z],size_inlet_inj,INITIAL[z],SOI[z],EOI[z],CAI,CAD_exit[z],HRF_exit[z],ROHR_exit[z],p_cyl_exit[z],dp_da_cyl_exit[z],T_cyl_exit[z],H_cooler_exit[z],mean_var_exit[z],heat_transfer[z],injection_rate_exit[z],accum_injection_rate_exit[z],species_EVO_exit[z]);
//
// rateBG=1.-test_variables[z][1]/test_variables[z][2];
// test_variables[z][14]=mean_var_exit[z][7]*rateBG;
//
// if(NOx_before==0){
// NOx_before=1e-4;
// }
// if(mean_var_exit[z][7]==0){
// mean_var_exit[z][7]=1e-4;
// }
//
// }//while
//
// }//z
//
//
// //------ OUTPUT FILES ------------------------------------------------------------------------------------------
//
// if((fich=fopen("exit1.dat","w"))==NULL){
// printf("The element file results could not be opened");
// exit(1);
// }
// strcpy(title,"HRL");
// fprintf(fich, "%s", title);
// for(p=0;p<CAI;p++){
// fprintf(fich, "\n%lf,",CAD_exit[0][p]);
// for(z=0;z<test_number;z++){
// fprintf(fich, "%lf,",(HRF_exit[z][p]*test_variables[z][4]*42.92));
// }
// }
// fclose(fich);
//
// if((fich=fopen("exit2.dat","w"))==NULL){
// printf("The element file results could not be opened");
// exit(1);
// }
// strcpy(title,"ROHR");
// fprintf(fich, "%s", title);
// for(p=0;p<CAI;p++){
// fprintf(fich, "\n%lf,",CAD_exit[0][p]);
// for(z=0;z<test_number;z++){
// fprintf(fich, "%lf,",(ROHR_exit[z][p]));
// }
// }
// fclose(fich);
//
//
// if((fich=fopen("exit3.dat","w"))==NULL){
// printf("The element file results could not be opened");
// exit(1);
// }
// strcpy(title,"Pcylinder");
//
// fprintf(fich, "%s", title);
// for(p=0;p<CAI;p++){
// fprintf(fich, "\n%lf,",CAD_exit[0][p]);
// for(z=0;z<test_number;z++){
// fprintf(fich, "%lf,",(p_cyl_exit[z][p]*1e-5));
// }
// }
// fclose(fich);
//
//
// if((fich=fopen("exit4.dat","w"))==NULL){
// printf("The element file results could not be opened");
// exit(1);
// }
// strcpy(title,"dP_da_cyl");
//
// fprintf(fich, "%s", title);
// for(p=0;p<CAI;p++){
// fprintf(fich, "\n%lf,",CAD_exit[0][p]);
// for(z=0;z<test_number;z++){
// fprintf(fich, "%lf,",(dp_da_cyl_exit[z][p]*1e-5));
// }
// }
// fclose(fich);
//
//
// if((fich=fopen("exit5.dat","w"))==NULL){
// printf("The element file results could not be opened");
// exit(1);
// }
// strcpy(title,"Tcylinder");
//
// fprintf(fich, "%s", title);
// for(p=0;p<CAI;p++){
// fprintf(fich, "\n%lf,",CAD_exit[0][p]);
// for(z=0;z<test_number;z++){
// fprintf(fich, "%lf,",T_cyl_exit[z][p]);
// }
// }
// fclose(fich);
//
//
// if((fich=fopen("exit6.dat","w"))==NULL){
// printf("The element file results could not be opened");
// exit(1);
// }
// strcpy(title,"Hcooler");
// fprintf(fich, "%s", title);
// for(p=0;p<CAI;p++){
// fprintf(fich, "\n%lf,",CAD_exit[0][p]);
// for(z=0;z<test_number;z++){
// fprintf(fich, "%lf,",H_cooler_exit[z][p]);
// }
// }
// fclose(fich);
//
// if((fich=fopen("exit7.dat","w"))==NULL){
// printf("The element file results could not be opened");
// exit(1);
// }
// strcpy(title,"injection_rate");
//
// fprintf(fich, "%s", title);
// for(p=0;p<CAI;p++){
// fprintf(fich, "\n%lf,",CAD_exit[0][p]);
// for(z=0;z<test_number;z++){
// fprintf(fich, "%lf,",injection_rate_exit[z][p]);
// }
// }
// fclose(fich);
//
//
//
// if((fich=fopen("exit8.dat","w"))==NULL){
// printf("The element file results could not be opened");
// exit(1);
// }
// strcpy(title,"accum_injection_rate");
//
// fprintf(fich, "%s", title);
// for(p=0;p<CAI;p++){
// fprintf(fich, "\n%lf,",CAD_exit[0][p]);
// for(z=0;z<test_number;z++){
// fprintf(fich, "%lf,",accum_injection_rate_exit[z][p]*test_variables[z][4]*42.92);
// }
// }
// fclose(fich);
//
//
// if((fich=fopen("mean_variables.dat","w"))==NULL){
// printf("The element file results could not be opened");
// exit(1);
// }
// strcpy(title,"test_number,PMI_hp[bar],ni(%),Pmax[bar],Tmax[K],PEVO[bar],TEVO[K],dP/da[bar/�],NOx[ppm],YO2[-],SOOT_A[FSN]");
// fprintf(fich, "%s", title);
// for(z=0;z<test_number;z++){
// fprintf(fich, "\n%d,",(z+1));
// for(p=0;p<10;p++){
// fprintf(fich, "%lf,",mean_var_exit[z][p]);
// }
// }
// fclose(fich);
//
// /*
// if((fich=fopen("Heat_transfer.dat","w"))==NULL){
// printf("The element file results could not be opened");
// exit(1);
// }
// strcpy(title,"test_number,Qtotal,Qcyl_head,Qcyl,Qpiston");
// fprintf(fich, "%s", title);
// for(z=0;z<test_number;z++){
// fprintf(fich, "\n%d,",z);
// for(p=0;p<4;p++){
// fprintf(fich, "%lf,",heat_transfer[z][p]);
// }
// }
// fclose(fich);
// */
//
// if((fich=fopen("PMI_hp.dat","w"))==NULL){
// printf("The element file results could not be opened");
// exit(1);
// }
// strcpy(title,"PMI_hp[bar]");
// fprintf(fich, "%s", title);
// fprintf(fich, "\n%lf,",mean_var_exit[0][0]);
// fclose(fich);
//
//
////memory liberation
//
// free(engine_parameters);
// free(engine_model_constants);
// for (z=0;z<test_number;z++)free(test_variables[z]);
// free(test_variables);
// for (z=0;z<test_number;z++)free(CAD_exit[z]);
// free(CAD_exit);
// for (z=0;z<test_number;z++)free(HRF_exit[z]);
// free(HRF_exit);
// for (z=0;z<test_number;z++)free(ROHR_exit[z]);
// free(ROHR_exit);
// for (z=0;z<test_number;z++)free(p_cyl_exit[z]);
// free(p_cyl_exit);
// for (z=0;z<test_number;z++)free(dp_da_cyl_exit[z]);
// free(dp_da_cyl_exit);
// for (z=0;z<test_number;z++)free(T_cyl_exit[z]);
// free(T_cyl_exit);
// for (z=0;z<test_number;z++)free(injection_rate_exit[z]);
// free(injection_rate_exit);
// for (z=0;z<test_number;z++)free(accum_injection_rate_exit[z]);
// free(accum_injection_rate_exit);
// for (z=0;z<test_number;z++)free(H_cooler_exit[z]);
// free(H_cooler_exit);
// for (z=0;z<test_number;z++)free(mean_var_exit[z]);
// free(mean_var_exit);
// for (z=0;z<test_number;z++)free(heat_transfer[z]);
// free(heat_transfer);
//
//
// free(INITIAL);
// for(z=0;z<test_number;z++)free(SOI[z]);
// free(SOI);
// for(z=0;z<test_number;z++)free(EOI[z]);
// free(EOI);
//
//
// for (z=0;z<test_number;z++)free(injection_rate[z]);
// free(injection_rate);
// for (z=0;z<test_number;z++)free(CAD_injection_rate[z]);
// free(CAD_injection_rate);
//
//
//
// }

/** ************     FUNCTION FOR INTERPOLATION      ************** */

inline void FUNCTION_FOR_INTERPOLATION(double *interpolated, double *time_interpolated,
	double *CAD_to_interpolate, double *vector_to_interpolate, int size_interpolated,
	int size_to_interpolate, double speed) {

	int auxiliar, counter;
	auxiliar = 0;
	for (counter = 0; counter < size_interpolated; counter++) {
		while ((CAD_to_interpolate[auxiliar] * 60. / (360. * speed) < time_interpolated[counter])
			&& (counter < size_interpolated) && (auxiliar < size_to_interpolate - 2)) {
			auxiliar = auxiliar + 1;
		}
		interpolated[counter] = vector_to_interpolate[auxiliar - 1] +
		(vector_to_interpolate[auxiliar] - vector_to_interpolate[auxiliar - 1]) *
		(time_interpolated[counter] - CAD_to_interpolate[auxiliar - 1] * 60. / (360. * speed)) /
		(CAD_to_interpolate[auxiliar] * 60. / (360. * speed) - CAD_to_interpolate[auxiliar - 1]
			* 60. / (360. * speed));
	}
}

/** ************     FUNCTION FOR INJECTION VELOCITY AND VIRTUAL VELOCITY     ************** */

inline void CALCULUS_OF_VIRTUAL_VELOCITY(double *inj_velocity, double *virtual_velocity, double *dmf,
	double *time_vector, double rofuel, double dc, double n_holes, double nozzle_d, double D,
	int size, double PI, double speed, double *EOI_IM, double inj_num, double *SOI_IM,
	double Piston_D, double DBowl, double CTM, double *CAD, double Kswirl) {

	int counter, auxiliar;
	double *virt, *w;
	int aux, i;

	aux = inj_num - 1;
	virt = (double*)malloc(size*sizeof(double));
	w = (double*)malloc(size*sizeof(double));

	for (counter = 0; counter < size; counter++) {
		virtual_velocity[counter] = -9999.;
		w[counter] = (pow((Piston_D / DBowl), 2) * 0.75 * (DBowl / 2) * 2 * PI * speed / 60 * pow
			((1 / cosh(CAD[counter] / 100)), 3) + 1) * CTM * Kswirl;
	}

	for (counter = 0; counter < size; counter++) {

		inj_velocity[counter] = dmf[counter] * 4. / (rofuel * n_holes * dc * PI * pow(nozzle_d, 2));

		for (i = 0; i < inj_num; i++) {
			if ((time_vector[counter] > (SOI_IM[i] / (speed * 6))) &&
				((time_vector[counter] - (SOI_IM[i] / (speed * 6))) < 0.001)) {
				virt[counter] = inj_velocity[counter] *
				(0.5274 * exp(-(time_vector[counter] - (SOI_IM[i] / (speed * 6))) / 0.00023) + 1.);
				if (virt[counter] > inj_velocity[counter]) {
					inj_velocity[counter] = virt[counter];
				}
			}
		}

		for (auxiliar = counter; auxiliar < size; auxiliar++) {
			virt[auxiliar] = inj_velocity[counter] * exp
			(-(time_vector[auxiliar] - time_vector[counter]) /
				(D * exp((time_vector[auxiliar] - time_vector[counter]) / 0.006)));
			if (virt[auxiliar] > virtual_velocity[auxiliar]) {
				virtual_velocity[auxiliar] = virt[auxiliar];
			}
		}

		/*
		if((time_vector[counter]>EOI_IM[aux]/(6*speed))&&(virtual_velocity[counter]>virtual_velocity[counter-1])){
		virtual_velocity[counter]=virtual_velocity[counter-1];
		}
		 */

	} // counter

	free(virt);
	free(w);
}

/** ************     FUNCTION FOR ADIMENSIONAL ACCUMULATED INJECTION RATE     ************** */

inline void CALCULUS_OF_ACCUMULATED_INJ_RATE(double *acu_dmf, double *dmf, double *time_vector,
	int size) {

	int counter;

	for (counter = 0; counter < size; counter++) {
		if (counter == 0) {
			acu_dmf[counter] = 0.;
		}
		else {
			acu_dmf[counter] = acu_dmf[counter - 1] +
			((time_vector[counter] - time_vector[counter - 1]) *
				(dmf[counter] - (dmf[counter] - dmf[counter - 1]) / 2.));
		}
	}

	if (acu_dmf[size - 1] > 0) {
		for (counter = 0; counter < size; counter++) {
			acu_dmf[counter] = acu_dmf[counter] / acu_dmf[size - 1];

		}
	}
}

/** ************     STOICHIOMETRY_CONSTANTS     ***************** */

inline void STOICHIOMETRY_CONSTANTS(double HC, double *Kst1, double *Kst2, double *Kst3, double *Kst4,
	double *Kst5, double *Kst6) {

	*Kst1 = (1 + HC / 4) * 32 / (12 + HC);
	*Kst2 = pow(*Kst1, -1);
	*Kst3 = 11 / (8 * (1 + HC / 4));
	*Kst4 = 9 * HC / (32 * (1 + HC / 4));
	*Kst5 = 44 / (12 + HC);
	*Kst6 = 9 * HC / (12 + HC);

}

/** ************     FUNCTION FOR NUMBER OF ELEMENTS i     ************** */

inline void CALCULUS_OF_NUMBER_ELEMENTS(int *num_i_IM, double *time_vector, int size, double speed,
	double *SOI_IM, double *EOI_IM, int inj_num) {

	int time_counter, inj_counter;

	for (inj_counter = 0; inj_counter < inj_num; inj_counter++) {
		num_i_IM[inj_counter] = 0;
		for (time_counter = 0; time_counter < size; time_counter++) {
			if ((time_vector[time_counter] >= SOI_IM[inj_counter] * 60. / (360. * speed)) &&
				(time_vector[time_counter] <= EOI_IM[inj_counter] * 60. / (360. * speed))) {
				num_i_IM[inj_counter] = num_i_IM[inj_counter] + 1;
			}
		}
	}
}

/** ************     FUNCTION FOR POI OF EACH i AND FUEL MASS OF EACH ELEMENT i AND SUB-ELEMENT j     ************** */

inline void CALCULUS_OF_POI(double **POI_IM, double **mfuel_i_IM, double **mfuel_ij_IM, double *acu_dmf,
	double *time_vector, int size, double speed, int *num_i_IM, int num_j, double *SOI_IM,
	double* EOI_IM, int inj_num, stControlElementComposition **elementcontrol) {

	int counter, inj_counter, aux;
	int *auxiliar_numi;

	auxiliar_numi = (int*)malloc(inj_num*sizeof(int*));

	for (inj_counter = 0; inj_counter < inj_num; inj_counter++) {
		auxiliar_numi[inj_counter] = -1;
	}

	for (inj_counter = 0; inj_counter < inj_num; inj_counter++) {
		for (counter = 0; counter < size; counter++) {
			if (time_vector[counter] >= SOI_IM[inj_counter] * 60. / (360. * speed)
				&& time_vector[counter] <= EOI_IM[inj_counter] * 60. / (360. * speed)) {

				auxiliar_numi[inj_counter] = auxiliar_numi[inj_counter] + 1;
				aux = auxiliar_numi[inj_counter];
				POI_IM[inj_counter][aux] = time_vector[counter];

				// element selection for the model control
				if ((acu_dmf[counter - 1] < 0.1) && (acu_dmf[counter] >= 0.1)) {
					elementcontrol[0][0].inj_number = inj_counter;
					elementcontrol[0][0].num_i = aux;
					elementcontrol[0][0].inj_number = inj_counter;
					elementcontrol[1][0].num_i = aux;
					elementcontrol[1][0].inj_number = inj_counter;
					elementcontrol[1][0].num_i = aux;
					elementcontrol[2][0].num_i = aux;
					elementcontrol[2][0].inj_number = inj_counter;
					elementcontrol[2][0].num_i = aux;
				}
				if ((acu_dmf[counter - 1] < 0.3) && (acu_dmf[counter] >= 0.3)) {
					elementcontrol[3][0].inj_number = inj_counter;
					elementcontrol[3][0].num_i = aux;
					elementcontrol[3][0].inj_number = inj_counter;
					elementcontrol[4][0].num_i = aux;
					elementcontrol[4][0].inj_number = inj_counter;
					elementcontrol[4][0].num_i = aux;
					elementcontrol[5][0].num_i = aux;
					elementcontrol[5][0].inj_number = inj_counter;
					elementcontrol[5][0].num_i = aux;
				}
				if ((acu_dmf[counter - 1] < 0.5) && (acu_dmf[counter] >= 0.5)) {
					elementcontrol[6][0].inj_number = inj_counter;
					elementcontrol[6][0].num_i = aux;
					elementcontrol[6][0].inj_number = inj_counter;
					elementcontrol[7][0].num_i = aux;
					elementcontrol[7][0].inj_number = inj_counter;
					elementcontrol[7][0].num_i = aux;
					elementcontrol[8][0].num_i = aux;
					elementcontrol[8][0].inj_number = inj_counter;
					elementcontrol[8][0].num_i = aux;
				}
				if ((acu_dmf[counter - 1] < 0.7) && (acu_dmf[counter] >= 0.7)) {
					elementcontrol[9][0].inj_number = inj_counter;
					elementcontrol[9][0].num_i = aux;
					elementcontrol[9][0].inj_number = inj_counter;
					elementcontrol[10][0].num_i = aux;
					elementcontrol[10][0].inj_number = inj_counter;
					elementcontrol[10][0].num_i = aux;
					elementcontrol[11][0].num_i = aux;
					elementcontrol[11][0].inj_number = inj_counter;
					elementcontrol[11][0].num_i = aux;
				}
				if ((acu_dmf[counter - 1] < 0.9) && (acu_dmf[counter] >= 0.9)) {
					elementcontrol[12][0].inj_number = inj_counter;
					elementcontrol[12][0].num_i = aux;
					elementcontrol[12][0].inj_number = inj_counter;
					elementcontrol[13][0].num_i = aux;
					elementcontrol[13][0].inj_number = inj_counter;
					elementcontrol[13][0].num_i = aux;
					elementcontrol[14][0].num_i = aux;
					elementcontrol[14][0].inj_number = inj_counter;
					elementcontrol[14][0].num_i = aux;
				}

				if (aux == 0) {
					mfuel_i_IM[inj_counter][aux] = ((acu_dmf[counter] + acu_dmf[counter + 1]) / 2.)
					- acu_dmf[counter - 1];
				}
				else if (aux == num_i_IM[inj_counter] - 1) {
					mfuel_i_IM[inj_counter][aux] = acu_dmf[counter + 1] -
					((acu_dmf[counter - 1] + acu_dmf[counter]) / 2.);
				}
				else {
					mfuel_i_IM[inj_counter][aux] = ((acu_dmf[counter] + acu_dmf[counter + 1]) / 2.)
					- ((acu_dmf[counter - 1] + acu_dmf[counter]) / 2.);
				}

			}
		}
	}

	for (inj_counter = 0; inj_counter < inj_num; inj_counter++) {
		for (counter = 0; counter < num_i_IM[inj_counter]; counter++) {
			mfuel_ij_IM[inj_counter][counter] = mfuel_i_IM[inj_counter][counter] / num_j;
		}
	}

	free(auxiliar_numi);

}

/** ************     FUNCTION MAIN FOR THE CALCULUS OF THE TENPERATURE AND THE PRESSURE IN THE CYLINDER     ************** */

inline void CALCULATE_CYCLE(double *roair, double *CAD, double delta_t, double *V_cyl, double VTDC,
	int counter, double speed, double *p_cyl, double *HRF, double *acu_mf, double *Mbb,
	double *acu_Mbb, double AFe, double f, double mfuel, double mEGR, double mairIVC,
	double *T_cyl, double HP, double *Yair, double *Yfuel, double *Yburned, double *U, double *CV,
	double *H_cooler, double *H, double T_Evaporation_fuel, double inj_fuel_temp,
	double PRECISION_ITERATION, double *defor, double *Rmixture, double Atmosphere_press,
	double *Gamma, double PI, double Runiv, double Piston_D, double S, double Crank_L,
	double Connecting_Rod_L, double E, double Piston_Axis_D, double Piston_Crown_H,
	double DBowl, double VBowl, double M_Connecting_Rod, double M_P_R_PA, double MW_air,
	double MW_fuel, double MW_burned, double C_ESteel, double C_Mech_Defor, double CTM,
	double WC1A, double WC1B, double C2, double C_MBLBY, double Cbb, double TPIS,
	double TCYL_HEAD, double TCYL, double *Qcylhead, double *Qcyl, double *Qpis) {

	double QT; /* Resultant heat due to fuel combustion (-) */
	double QC; /* Instantaneous heat */
	double average_Volume; /* Average volume between the actual and the previous step */
	double average_Temperature; /* Average temperature between the actual and the previous step */
	double average_Pressure; /* Average temperature between the actual and the previous step */
	double DU; /* Increment of internal energy */
	double MCYL; /* Cylinder mass */
	double AERR = 0.; /* Error in the energetic balance */
	int niter = 0; /* Number of iterations */
	double Wi; /* Instantaneous work */
	double uf; /* internal energy of fuel */
	double ecg; /* enthalpy of evaporated fuel */
	double ent_ref; /* Reference enthalpy */
	double HFTiny; /* Enthalpy due to the temperature of the fuel */
	double a; /* auxiliar variable */
	double delta_CAD; /* Calculus interval CAD */
	double UANT; /* UANT acumulates a variable that is calculated  as the internal energy in the
	previous step with the actual masic ratios because of only is considered the
	increasement of energy due to the rise of the temperature not the rise of the
	mass */

	if (counter == 0) {
		acu_mf[counter] = acu_mf[counter] * mfuel;
	}
	if (counter == 1) {
		acu_mf[counter] = acu_mf[counter] * mfuel;
		acu_mf[counter - 1] = acu_mf[counter - 1] * mfuel;
	}
	if (counter >= 2) {
		acu_mf[counter] = acu_mf[counter] * mfuel;
		acu_mf[counter - 1] = acu_mf[counter - 1] * mfuel;
	}

	delta_CAD = delta_t * 360. * speed / 60.;
	V_cyl[counter] = VOLUME(CAD[counter], VTDC, PI, Piston_D, Crank_L, Connecting_Rod_L, E);

	if (counter < 2) {

		DEFORMATIONS(&(V_cyl[counter]), &(defor[counter]), p_cyl[counter], CAD[counter], delta_CAD,
			speed, PI, Piston_D, S, Connecting_Rod_L, E, Piston_Axis_D, Piston_Crown_H,
			M_Connecting_Rod, M_P_R_PA, C_ESteel, C_Mech_Defor);

		MASIC_RATIO(&Yair[counter], &Yfuel[counter], &Yburned[counter], &Rmixture[counter],
			HRF[counter], acu_mf[counter], acu_Mbb[counter], AFe, f, mfuel, mEGR, mairIVC, Runiv,
			MW_air, MW_fuel, MW_burned);

		PROPERTIES(&U[counter], &CV[counter], T_cyl[counter], T_cyl[counter], Yair[counter],
			Yfuel[counter], Yburned[counter]);

		MCYL = mairIVC + acu_mf[counter] - acu_Mbb[counter];
		roair[counter] = MCYL / V_cyl[counter];
		p_cyl[counter] = MCYL * Rmixture[counter] * T_cyl[counter] / V_cyl[counter];

	}
	else {

		*Gamma = (CV[counter - 1] + Rmixture[counter - 1]) / CV[counter - 1];

		Mbb[counter] = BLOW_BY(p_cyl[counter - 1], T_cyl[counter - 1], Rmixture[counter - 1],
			delta_CAD, speed, *Gamma, Atmosphere_press, Piston_D, C_MBLBY, Cbb);

		acu_Mbb[counter] = acu_Mbb[counter - 1] + Mbb[counter];

		DEFORMATIONS(&(V_cyl[counter]), &(defor[counter]), p_cyl[counter - 1], CAD[counter],
			delta_CAD, speed, PI, Piston_D, S, Connecting_Rod_L, E, Piston_Axis_D, Piston_Crown_H,
			M_Connecting_Rod, M_P_R_PA, C_ESteel, C_Mech_Defor);

		MASIC_RATIO(&Yair[counter], &Yfuel[counter], &Yburned[counter], &Rmixture[counter],
			HRF[counter - 1], acu_mf[counter], acu_Mbb[counter], AFe, f, mfuel, mEGR, mairIVC,
			Runiv, MW_air, MW_fuel, MW_burned);

		PROPERTIES(&UANT, &CV[counter], T_cyl[counter - 1], T_cyl[counter - 1], Yair[counter],
			Yfuel[counter], Yburned[counter]);

		MCYL = mairIVC + acu_mf[counter] - acu_Mbb[counter];

		roair[counter] = MCYL / V_cyl[counter];

		average_Volume = 0.5 * (V_cyl[counter] + V_cyl[counter - 1]);

		H_cooler[counter] = HEAT_COOLER(p_cyl[counter - 1], p_cyl[0], T_cyl[counter - 1], T_cyl[0],
			average_Volume, V_cyl[0], delta_CAD, speed, VTDC, &H[counter], PI, Piston_D, S, DBowl,
			VBowl, CTM, WC1A, WC1B, C2, TPIS, TCYL_HEAD, TCYL, CAD[counter], Qcylhead, Qcyl, Qpis,
			counter);

		QT = mfuel * HP;
		QC = (HRF[counter - 1] - HRF[counter - 2]) * QT;
		DU = -(p_cyl[counter - 1] * (V_cyl[counter] - V_cyl[counter - 1])) + QC - H_cooler[counter];

		T_cyl[counter] = T_cyl[counter - 1] + DU / MCYL / CV[counter];

		ecg = -21776.6 * Runiv / MW_fuel + Runiv / MW_fuel *
		(-4.5826 * T_Evaporation_fuel + 0.12428 / 2. * pow(T_Evaporation_fuel,
				2.) - 0.00007233 / 3. * pow(T_Evaporation_fuel, 3.) + 0.000000016269 / 4. * pow
			(T_Evaporation_fuel, 4.) - 26067.28 / T_Evaporation_fuel);

		ent_ref = (Runiv / MW_fuel) * ((-4.5826 * 298.) + (0.12428 / 2. * pow(298., 2.)) -
			(0.00007233 / 3. * pow(298., 3.)) + (0.000000016269 / 4. * pow(298., 4.)) -
			(26067.28 * pow(298., -1.)));
		ecg = ecg - ent_ref;

		HFTiny = -1852564 + 2195 * (inj_fuel_temp);
		niter = 0;
		AERR = 0;

		do {

			T_cyl[counter] = T_cyl[counter] - AERR / CV[counter] / MCYL;
			p_cyl[counter] = MCYL * Rmixture[counter] * T_cyl[counter] / V_cyl[counter];
			PROPERTIES(&U[counter], &CV[counter], T_cyl[counter], T_cyl[counter - 1],
				Yair[counter], Yfuel[counter], Yburned[counter]);

			average_Temperature = 0.5 * (T_cyl[counter] + T_cyl[counter - 1]);
			average_Pressure = 0.5 * (p_cyl[counter] + p_cyl[counter - 1]);
			H_cooler[counter] = HEAT_COOLER(average_Pressure, p_cyl[0], average_Temperature,
				T_cyl[0], average_Volume, V_cyl[0], delta_CAD, speed, VTDC, &H[counter], PI,
				Piston_D, S, DBowl, VBowl, CTM, WC1A, WC1B, C2, TPIS, TCYL_HEAD, TCYL,
				CAD[counter], Qcylhead, Qcyl, Qpis, counter);

			Wi = -average_Pressure * (V_cyl[counter] - V_cyl[counter - 1]);

			PROPERTIES_FUEL(&uf, T_cyl[counter]);

			AERR = (MCYL * (U[counter] - UANT)) - Wi - QC + H_cooler[counter] -
			((HFTiny - uf) * (acu_mf[counter] - acu_mf[counter - 1])) +
			(Rmixture[counter] * T_cyl[counter] * Mbb[counter]);
			niter = niter + 1;

			a = fabs(AERR / U[counter]);
			if (niter > 40) {
				a = 0.000000000001;
			}
		}
		while (a > PRECISION_ITERATION);
	}

	if (counter == 0) {
		acu_mf[counter] = acu_mf[counter] / mfuel;
	}
	if (counter == 1) {
		acu_mf[counter] = acu_mf[counter] / mfuel;
		acu_mf[counter - 1] = acu_mf[counter - 1] / mfuel;
	}
	if (counter >= 2) {
		acu_mf[counter] = acu_mf[counter] / mfuel;
		acu_mf[counter - 1] = acu_mf[counter - 1] / mfuel;
	}
}

/** ************     CALCULUS OF MEAN VARIABLES     ************** */
inline void CALCULUS_OF_MEAN_VARIABLES(double *p_cyl, double *T_cyl, double *dp_da_cyl, double *CAD,
	double *pmax, double *Tmax, double *dp_da_max, double *p_exit, double *T_exit,
	int size) {

	int counter, iter, n;
	double *dp_da_aux;

	dp_da_aux = (double*)malloc(size*sizeof(double));
	n = 4;

	for (counter = 0; counter < size; counter++) {

		if (p_cyl[counter] > *pmax) {
			*pmax = p_cyl[counter];
		}
		if (T_cyl[counter] > *Tmax) {
			*Tmax = T_cyl[counter];
		}
		if (counter > 0) {
			dp_da_aux[counter] = (p_cyl[counter] - p_cyl[counter - 1]) /
			(CAD[counter] - CAD[counter - 1]);
		}

		if (counter == size - 1) {
			*p_exit = p_cyl[counter];
			*T_exit = T_cyl[counter];
		}

	}
	dp_da_cyl[0] = dp_da_aux[0];
	dp_da_cyl[size - 1] = dp_da_aux[size - 1];

	for (iter = 0; iter < n; iter++) {
		for (counter = 1; counter < size - 1; counter++) {
			dp_da_cyl[counter] = (dp_da_aux[counter - 1] + dp_da_aux[counter] + dp_da_aux
				[counter + 1]) / 3;
		}
	}

	for (counter = 0; counter < size - 1; counter++) {
		if (dp_da_cyl[counter] > *dp_da_max) {
			*dp_da_max = dp_da_cyl[counter];
		}
	}

	free(dp_da_aux);

}

/** ************     VOLUME CALCULUS     ************** */

inline double VOLUME(double CAD, double VTDC, double PI, double Piston_D, double Crank_L,
	double Connecting_Rod_L, double E) {

	double V_cyl, AREA, AUX, A;

	A = CAD * PI / 180.;
	AREA = PI * Piston_D * Piston_D / 4.;
	AUX = (Crank_L * sqrt(pow(1. + 1. / (Crank_L / Connecting_Rod_L), 2) - pow(E / Crank_L, 2.))
		- Crank_L * (cos(A) + sqrt(pow(1. / (Crank_L / Connecting_Rod_L),
					2.) - pow(sin(A) - E / Crank_L, 2.))));
	V_cyl = (VTDC + AREA * AUX);

	return V_cyl;
}

/** ************     VOLUME INCREMENTS DUE TO THE PRESURE IN THE CYLINDER AND THE INERTIA OF IT     ************** */

inline void DEFORMATIONS(double *V_cyl, double *DEFOR, double p_cyl, double CAD, double delta_CAD,
	double speed, double PI, double Piston_D, double S, double Connecting_Rod_L, double E,
	double Piston_Axis_D, double Piston_Crown_H, double M_Connecting_Rod, double M_P_R_PA,
	double C_ESteel, double C_Mech_Defor) {

	double AVp; /* Increseament of volume due to the pressure */
	double AVi; /* Increasemneto of volume due to the inertia */
	double Acel; /* Linear acceleration of the piston */
	double Lactual; /* Distance between the piston and the piston head at actual step */
	double Lanterior; /* Distance between the piston and the piston head at previous step */
	double Lposterior; /* Distance between the piston and the piston head at following step */
	double auxalfa; /* Auxiliar variable */
	double Msist; /* Msist is the mass of the piston+piston axis+rings+ 0.33*connecting rod */

	AVp = (PI * pow(Piston_D, 2.) / 4.) * C_Mech_Defor * (p_cyl / C_ESteel) * pow
	(Piston_D / Piston_Axis_D, 2.) * (Piston_Crown_H + Connecting_Rod_L + S / 2.);

	auxalfa = (CAD - delta_CAD) * PI / 180.;
	Lanterior = (sqrt(pow((S / 2.) + Connecting_Rod_L, 2.) - pow(E, 2.))) -
	((S / 2.) * cos(auxalfa) + sqrt(pow(Connecting_Rod_L, 2.) - pow((S / 2.) * sin(auxalfa) - E,
				2.)));

	auxalfa = (CAD + delta_CAD) * PI / 180.;
	Lposterior = (sqrt(pow((S / 2.) + Connecting_Rod_L, 2.) - pow(E, 2.))) -
	((S / 2.) * cos(auxalfa) + sqrt(pow(Connecting_Rod_L,
				2.) - pow((S / 2.) * sin(auxalfa) - E, 2.)));

	auxalfa = (CAD) * PI / 180.;
	Lactual = (sqrt(pow((S / 2.) + Connecting_Rod_L, 2.) - pow(E, 2.))) -
	((S / 2.) * cos(auxalfa) + sqrt(pow(Connecting_Rod_L, 2.) - pow((S / 2.) * sin(auxalfa) - E,
				2.)));

	Acel = (Lanterior + Lposterior - Lactual * 2.) / pow((delta_CAD / (6. * speed)), 2.);

	Msist = 0.33 * M_Connecting_Rod + M_P_R_PA;

	AVi = Msist * Acel * C_Mech_Defor * 1. / C_ESteel * pow(Piston_D / Piston_Axis_D, 2.) *
	(Piston_Crown_H + Connecting_Rod_L + S / 2.);

	*DEFOR = (AVp - AVi) * 1000000;
	*V_cyl = *V_cyl + (AVp - AVi);

}

/** ************     FUNCTION FOR THE CALCULUS OF MASIC RATIOS     ************** */

inline void MASIC_RATIO(double *Yair, double *Yfuel, double *Yburned, double *Rmixture, double HRF,
	double acu_mf, double acu_Mbb, double AFe, double f, double mfuel, double mEGR,
	double mairIVC, double Runiv, double MW_air, double MW_fuel, double MW_burned) {

	double Yq2, Yq0;

	Yq2 = (1. + (AFe)) / (1. + (f) - ((acu_Mbb / 2.) / mfuel) * (1. / (1. + (mEGR) / (mairIVC))));
	Yq0 = Yq2 * (mEGR) / (mairIVC);

	*Yburned = (((mfuel + mfuel * AFe) * HRF) + ((mEGR) * Yq2) - ((acu_Mbb / 2.) * Yq0)) /
	(acu_mf + mairIVC - acu_Mbb / 2.);

	if (acu_mf >= mfuel * HRF) {
		*Yfuel = (acu_mf - mfuel * HRF) / (acu_mf + mairIVC - acu_Mbb);
	}
	else {
		*Yfuel = 0;
	}

	*Yair = 1. - *Yfuel - *Yburned;
	*Rmixture = *Yair * Runiv / MW_air + *Yfuel * Runiv / MW_fuel + *Yburned * Runiv / MW_burned;

}

/** ************     FUNCTION FOR THE CALCULUS OF CV AND INTERNAL ENERGY     ************** */

inline void PROPERTIES(double *u, double *CV, double T_cyl, double T_cyl_pre, double Yair, double Yfuel,
	double Yburned) {

	double cva, cvf, cvq, average_Temperature, ua, uf, uq;

	average_Temperature = 0.5 * (T_cyl + T_cyl_pre);

	cva = (-10.4199 * pow(average_Temperature, 0.5)) + 2522.88 -
	(67227.1 * pow(average_Temperature, -0.5)) + (917124.4 * pow(average_Temperature, -1.)) -
	(4174853.6 * pow(average_Temperature, -1.5));
	cvf = -256.4 + (6.95372 * average_Temperature) - (0.00404715 * pow(average_Temperature, 2.)) +
	(0.000000910259 * pow(average_Temperature, 3.)) + (1458487. * pow(average_Temperature, -2.));
	cvq = 641.154 + (0.43045 * average_Temperature) - (0.0001125 * pow(average_Temperature, 2.)) +
	(0.000000008979 * pow(average_Temperature, 3.));

	*CV = cva * Yair + cvf * Yfuel + cvq * Yburned;

	ua = -4193697.9 - (6.9466 * pow(T_cyl, 1.5)) + (2522.88 * T_cyl) -
	(134454.16 * pow(T_cyl, 0.5)) + (917124.39 * log(T_cyl)) + (8349707.14 * pow(T_cyl, -0.5));
	uf = -1445686.1 - (256.4 * T_cyl) + (3.47686 * pow(T_cyl, 2.)) - (0.00134905 * pow(T_cyl, 3.))
	+ (0.000000227565 * pow(T_cyl, 4.)) - (1458487. * pow(T_cyl, -1.));
	uq = -3251495. + (1028.75 * T_cyl) - (0.15377 * pow(T_cyl, 2.)) +
	(0.000067895 * pow(T_cyl, 3.));

	*u = ua * Yair + uf * Yfuel + uq * Yburned;
}

/** ************     FUNCTION FOR THE CALCULUS OF INTERNAL ENRGY OF FUEL     ************** */

inline void PROPERTIES_FUEL(double *uf, double T_cyl) {

	*uf = -1445686.1 - (256.4 * T_cyl) + (3.47686 * pow(T_cyl, 2.)) - (0.00134905 * pow(T_cyl, 3.))
	+ (0.000000227565 * pow(T_cyl, 4.)) - (1458487. * pow(T_cyl, -1.));
}

/** ************     FUNCTION FOR THE CALCULUS OF HEAT HUNG OVER TO THE COOLER AND H     ************** */

inline double HEAT_COOLER(double p_cyl, double pressureIVC, double T_cyl, double temperatureIVC,
	double average_Volume, double volumeIVC, double delta_CAD, double speed, double VTDC,
	double *H, double PI, double Piston_D, double S, double DBowl, double VBowl, double CTM,
	double WC1A, double WC1B, double C2, double TPIS, double TCYL_HEAD, double TCYL,
	double CAD, double *Qcylhead, double *Qcyl, double *Qpis, int counter)

{

	double C1;
	double cm; /* mean piston speed */
	double PA; /* pressure in the cylinder without combustion */
	double comb; /* Term of combustion */
	double Cylinder_capacity; /* Cylinder capacity(m3) */
	double Piston_area; /* Piston area */
	double Cylinder_head_area; /* piston head area */
	double H_cooler; /* Heat hung over to the cooler */

	cm = 2. * S * speed / 60.;
	C1 = CALCULATE_C1(cm, CTM, WC1A, WC1B, Piston_D, DBowl, speed, CAD, PI);

	PA = pressureIVC * pow(volumeIVC / average_Volume, 1.36);

	Cylinder_capacity = PI * Piston_D * Piston_D * S / 4.;

	comb = ((Cylinder_capacity * temperatureIVC) / (pressureIVC * volumeIVC)) * (p_cyl - PA);

	*H = 0.012 * pow(Piston_D, -0.2) * pow(T_cyl, -0.53) * pow(p_cyl, 0.8) * pow((C1 + C2 * comb),
		0.8);

	CALCULATE_AREAS(&Piston_area, &Cylinder_head_area, PI, Piston_D, DBowl, VBowl);

	H_cooler = *H * (Piston_area * (T_cyl - TPIS) + Cylinder_head_area * (T_cyl - TCYL_HEAD) + 4. *
		((average_Volume - VTDC) / Piston_D * (T_cyl - TCYL))) * (delta_CAD) / 6. / speed;

	Qcylhead[counter] = *H * Cylinder_head_area * (T_cyl - TCYL_HEAD) * (delta_CAD) / 6. / speed;
	Qcyl[counter] = *H * 4. * ((average_Volume - VTDC) / Piston_D * (T_cyl - TCYL)) * (delta_CAD)
	/ 6. / speed;
	Qpis[counter] = *H * Piston_area * (T_cyl - TPIS) * (delta_CAD) / 6. / speed;

	return H_cooler;
}

/** ************     FUNCTION FOR THE CALCULUS OF C1     ************** */

inline double CALCULATE_C1(double cm, double CTM, double WC1A, double WC1B, double Piston_D, double DBowl,
	double speed, double CAD, double PI) {

	double C1;
	double cu;
	double KCTM;
	double ratio_CTM;
	double x_alfa;

	KCTM = exp(-0.200679 * pow(CTM, 0.431202));

	ratio_CTM = pow(DBowl / Piston_D, 2) * (1 / KCTM);
	x_alfa = ratio_CTM + 1 / (pow(cosh(CAD / 100), 40) + ratio_CTM / (1 - ratio_CTM));

	cu = x_alfa * (2 * PI * speed / 60) * CTM * pow(Piston_D / DBowl, 2) * KCTM * (DBowl / 2);

	C1 = WC1A * cm + WC1B * cu;

	return C1;
}

/** ************     FUNCTION FOR THE CALCULUS OF PISTON AND PISTON HEAD AREAS     ************** */

inline void CALCULATE_AREAS(double *Piston_area, double *Cylinder_head_area, double PI, double Piston_D,
	double DBowl, double VBowl) {

	*Piston_area = PI * pow(Piston_D, 2.) / 4. + VBowl * 4. / DBowl;
	*Cylinder_head_area = PI * pow(Piston_D, 2.) / 4.;
}

/** ************     FUNCTION FOR THE CALCULUS OF THE MASS OF BLOW-BY     ************** */

inline double BLOW_BY(double p_cyl, double T_cyl, double Rmixture, double delta_CAD, double speed,
	double Gamma, double Atmosphere_press, double Piston_D, double C_MBLBY, double Cbb) {

	double C_Z;
	double Pressure_up;
	double Pressure_down;
	double Pressure_critic;
	double BBy;

	if (p_cyl > Atmosphere_press) {
		Pressure_up = p_cyl;
		Pressure_down = Atmosphere_press;
	}
	else {
		Pressure_up = Atmosphere_press;
		Pressure_down = p_cyl;
	}

	Pressure_critic = Pressure_up * pow(2. / (Gamma + 1.), (Gamma / (Gamma - 1.)));

	if (Pressure_down < Pressure_critic) {
		Pressure_down = Pressure_critic;
	}

	C_Z = ((2. * Gamma) / (Gamma - 1.)) * (pow(Pressure_down / Pressure_up,
			2. / Gamma) - pow(Pressure_down / Pressure_up, ((Gamma + 1.) / Gamma)));
	BBy = Cbb * C_MBLBY * Piston_D * Pressure_up * pow(C_Z / (Rmixture * T_cyl), 0.5);

	if (Atmosphere_press > p_cyl) {
		BBy = -BBy;
	}

	BBy = BBy * delta_CAD / 6. / speed;

	return BBy;
}

/** ************     CALCULUS OF IMP_HP     ************** */

inline void CALCULUS_OF_IMP_HP(double *complete_p_cyl, double *complete_CAD, double *p_cyl, double *V_cyl,
	double *complete_V_cyl, double *complete_deform, double *WI_HP, double *IMP_HP,
	int complete_size, int complete_prev_size, double delta_t, double speed, int size,
	double IVC, double EVO, double VTDC, double Cylinder_capacity, double PI, double Piston_D,
	double S, double Crank_L, double Connecting_Rod_L, double E, double Piston_Axis_D,
	double Piston_Crown_H, double M_Connecting_Rod, double M_P_R_PA, double C_ESteel,
	double C_Mech_Defor, double inlet_pres, double exhaust_pres) {

	double delta_CAD;
	int counter;

	*IMP_HP = 0;
	*WI_HP = 0;
	delta_CAD = delta_t * 360. * speed / 60.;
	complete_CAD[0] = IVC - complete_prev_size * delta_t * 360. * speed / 60.;

	for (counter = 1; counter < complete_size; counter++) {
		complete_CAD[counter] = complete_CAD[counter - 1] + delta_t * 360. * speed / 60.;
	}

	for (counter = 0; counter < complete_size; counter++) {
		if (counter < complete_prev_size) {
			complete_p_cyl[counter] = ((inlet_pres - p_cyl[0]) / (-180 - IVC)) *
			(complete_CAD[counter] - (-180)) + inlet_pres;
		}
		if ((counter >= complete_prev_size) && (counter < complete_prev_size + size)) {
			complete_p_cyl[counter] = p_cyl[counter - complete_prev_size];
		}
		if (counter >= complete_prev_size + size) {
			complete_p_cyl[counter] = ((exhaust_pres - p_cyl[size - 1]) / (180 - EVO)) *
			(complete_CAD[counter] - (180)) + exhaust_pres;
		}
	}

	for (counter = 0; counter < complete_size; counter++) {
		if (counter < complete_prev_size) {
			complete_V_cyl[counter] = VOLUME(complete_CAD[counter], VTDC, PI, Piston_D, Crank_L,
				Connecting_Rod_L, E);
			DEFORMATIONS(&complete_V_cyl[counter], &complete_deform[counter],
				complete_p_cyl[counter], complete_CAD[counter], delta_CAD, speed, PI,
				Piston_D, S, Connecting_Rod_L, E, Piston_Axis_D, Piston_Crown_H, M_Connecting_Rod,
				M_P_R_PA, C_ESteel, C_Mech_Defor);
		}
		if ((counter >= complete_prev_size) && (counter < complete_prev_size + size)) {
			complete_V_cyl[counter] = V_cyl[counter - complete_prev_size];
		}
		if (counter >= complete_prev_size + size) {
			complete_V_cyl[counter] = VOLUME(complete_CAD[counter], VTDC, PI, Piston_D, Crank_L,
				Connecting_Rod_L, E);
			DEFORMATIONS(&complete_V_cyl[counter], &complete_deform[counter],
				complete_p_cyl[counter], complete_CAD[counter], delta_CAD, speed, PI,
				Piston_D, S, Connecting_Rod_L, E, Piston_Axis_D, Piston_Crown_H, M_Connecting_Rod,
				M_P_R_PA, C_ESteel, C_Mech_Defor);
		}

		if (counter > 0) {
			*WI_HP = *WI_HP + (complete_p_cyl[counter] + complete_p_cyl[counter - 1]) * 0.5 *
			(complete_V_cyl[counter] - complete_V_cyl[counter - 1]);
		}
	}
	*IMP_HP = *WI_HP / Cylinder_capacity;

}

/** ************NOX equilibrium calculation*********************** */

inline void FUNCTION_NOX(double *YNOeq_value, double *KdYNO_value, double **YNOeq, double **KdYNO,
	double temperature, double mO2, double mtotal) {
	int i, j;
	double di, dj;

	YNOeq[0][0] = 0.000000;
	KdYNO[0][0] = 0.000000;
	YNOeq[0][1] = 0.000034;
	KdYNO[0][1] = 0.000000;
	YNOeq[0][2] = 0.000054;
	KdYNO[0][2] = 0.000000;
	YNOeq[0][3] = 0.000074;
	KdYNO[0][3] = 0.000000;
	YNOeq[0][4] = 0.000000;
	KdYNO[0][4] = 0.000000;
	YNOeq[0][5] = 0.000000;
	KdYNO[0][5] = 0.000000;
	YNOeq[0][6] = 0.000000;
	KdYNO[0][6] = 0.000000;
	YNOeq[0][7] = 0.000000;
	KdYNO[0][7] = 0.000000;
	YNOeq[0][8] = 0.000000;
	KdYNO[0][8] = 0.000000;
	YNOeq[0][9] = 0.000000;
	KdYNO[0][9] = 0.000000;
	YNOeq[0][10] = 0.000000;
	KdYNO[0][10] = 0.000000;
	YNOeq[0][11] = 0.000000;
	KdYNO[0][11] = 0.000000;
	YNOeq[1][0] = 0.000000;
	KdYNO[1][0] = 0.000000;
	YNOeq[1][1] = 0.000068;
	KdYNO[1][1] = 0.000000;
	YNOeq[1][2] = 0.000090;
	KdYNO[1][2] = 0.000000;
	YNOeq[1][3] = 0.000113;
	KdYNO[1][3] = 0.000000;
	YNOeq[1][4] = 0.000135;
	KdYNO[1][4] = 0.000000;
	YNOeq[1][5] = 0.000160;
	KdYNO[1][5] = 0.000000;
	YNOeq[1][6] = 0.000186;
	KdYNO[1][6] = 0.000000;
	YNOeq[1][7] = 0.000210;
	KdYNO[1][7] = 0.000000;
	YNOeq[1][8] = 0.000000;
	KdYNO[1][8] = 0.000000;
	YNOeq[1][9] = 0.000000;
	KdYNO[1][9] = 0.000000;
	YNOeq[1][10] = 0.000000;
	KdYNO[1][10] = 0.000000;
	YNOeq[1][11] = 0.000000;
	KdYNO[1][11] = 0.000000;
	YNOeq[2][0] = 0.000000;
	KdYNO[2][0] = 0.000000;
	YNOeq[2][1] = 0.000080;
	KdYNO[2][1] = 0.000000;
	YNOeq[2][2] = 0.000131;
	KdYNO[2][2] = 0.000000;
	YNOeq[2][3] = 0.000168;
	KdYNO[2][3] = 0.000000;
	YNOeq[2][4] = 0.000197;
	KdYNO[2][4] = 0.000000;
	YNOeq[2][5] = 0.000224;
	KdYNO[2][5] = 0.000000;
	YNOeq[2][6] = 0.000251;
	KdYNO[2][6] = 0.000000;
	YNOeq[2][7] = 0.000277;
	KdYNO[2][7] = 0.000000;
	YNOeq[2][8] = 0.000304;
	KdYNO[2][8] = 0.000000;
	YNOeq[2][9] = 0.000000;
	KdYNO[2][9] = 0.000000;
	YNOeq[2][10] = 0.000000;
	KdYNO[2][10] = 0.000000;
	YNOeq[2][11] = 0.000000;
	KdYNO[2][11] = 0.000000;
	YNOeq[3][0] = 0.000000;
	KdYNO[3][0] = 0.000000;
	YNOeq[3][1] = 0.000056;
	KdYNO[3][1] = 0.000000;
	YNOeq[3][2] = 0.000111;
	KdYNO[3][2] = 0.000000;
	YNOeq[3][3] = 0.000165;
	KdYNO[3][3] = 0.000000;
	YNOeq[3][4] = 0.000220;
	KdYNO[3][4] = 0.000000;
	YNOeq[3][5] = 0.000274;
	KdYNO[3][5] = 0.000000;
	YNOeq[3][6] = 0.000329;
	KdYNO[3][6] = 0.000000;
	YNOeq[3][7] = 0.000366;
	KdYNO[3][7] = 0.000000;
	YNOeq[3][8] = 0.000396;
	KdYNO[3][8] = 0.000000;
	YNOeq[3][9] = 0.000000;
	KdYNO[3][9] = 0.000000;
	YNOeq[3][10] = 0.000000;
	KdYNO[3][10] = 0.000000;
	YNOeq[3][11] = 0.000000;
	KdYNO[3][11] = 0.000000;
	YNOeq[4][0] = 0.000001;
	KdYNO[4][0] = 0.000000;
	YNOeq[4][1] = 0.000150;
	KdYNO[4][1] = 0.000000;
	YNOeq[4][2] = 0.000226;
	KdYNO[4][2] = 0.000000;
	YNOeq[4][3] = 0.000280;
	KdYNO[4][3] = 0.000000;
	YNOeq[4][4] = 0.000324;
	KdYNO[4][4] = 0.000000;
	YNOeq[4][5] = 0.000368;
	KdYNO[4][5] = 0.000000;
	YNOeq[4][6] = 0.000411;
	KdYNO[4][6] = 0.000000;
	YNOeq[4][7] = 0.000455;
	KdYNO[4][7] = 0.000000;
	YNOeq[4][8] = 0.000500;
	KdYNO[4][8] = 0.000000;
	YNOeq[4][9] = 0.000551;
	KdYNO[4][9] = 0.000000;
	YNOeq[4][10] = 0.000000;
	KdYNO[4][10] = 0.000000;
	YNOeq[4][11] = 0.000000;
	KdYNO[4][11] = 0.000000;
	YNOeq[5][0] = 0.000001;
	KdYNO[5][0] = 0.000000;
	YNOeq[5][1] = 0.000089;
	KdYNO[5][1] = 0.000000;
	YNOeq[5][2] = 0.000173;
	KdYNO[5][2] = 0.000000;
	YNOeq[5][3] = 0.000258;
	KdYNO[5][3] = 0.000000;
	YNOeq[5][4] = 0.000342;
	KdYNO[5][4] = 0.000000;
	YNOeq[5][5] = 0.000426;
	KdYNO[5][5] = 0.000000;
	YNOeq[5][6] = 0.000510;
	KdYNO[5][6] = 0.000000;
	YNOeq[5][7] = 0.000590;
	KdYNO[5][7] = 0.000000;
	YNOeq[5][8] = 0.000644;
	KdYNO[5][8] = 0.000000;
	YNOeq[5][9] = 0.000696;
	KdYNO[5][9] = 0.000000;
	YNOeq[5][10] = 0.000000;
	KdYNO[5][10] = 0.000000;
	YNOeq[5][11] = 0.000000;
	KdYNO[5][11] = 0.000000;
	YNOeq[6][0] = 0.000002;
	KdYNO[6][0] = 0.000000;
	YNOeq[6][1] = 0.000235;
	KdYNO[6][1] = 0.000000;
	YNOeq[6][2] = 0.000351;
	KdYNO[6][2] = 0.000000;
	YNOeq[6][3] = 0.000438;
	KdYNO[6][3] = 0.000000;
	YNOeq[6][4] = 0.000511;
	KdYNO[6][4] = 0.000000;
	YNOeq[6][5] = 0.000585;
	KdYNO[6][5] = 0.000000;
	YNOeq[6][6] = 0.000658;
	KdYNO[6][6] = 0.000000;
	YNOeq[6][7] = 0.000727;
	KdYNO[6][7] = 0.000000;
	YNOeq[6][8] = 0.000790;
	KdYNO[6][8] = 0.000000;
	YNOeq[6][9] = 0.000858;
	KdYNO[6][9] = 0.000000;
	YNOeq[6][10] = 0.000931;
	KdYNO[6][10] = 0.000000;
	YNOeq[6][11] = 0.000000;
	KdYNO[6][11] = 0.000000;
	YNOeq[7][0] = 0.000005;
	KdYNO[7][0] = 0.000000;
	YNOeq[7][1] = 0.000136;
	KdYNO[7][1] = 0.000000;
	YNOeq[7][2] = 0.000254;
	KdYNO[7][2] = 0.000000;
	YNOeq[7][3] = 0.000372;
	KdYNO[7][3] = 0.000000;
	YNOeq[7][4] = 0.000489;
	KdYNO[7][4] = 0.000000;
	YNOeq[7][5] = 0.000607;
	KdYNO[7][5] = 0.000000;
	YNOeq[7][6] = 0.000725;
	KdYNO[7][6] = 0.000000;
	YNOeq[7][7] = 0.000843;
	KdYNO[7][7] = 0.000001;
	YNOeq[7][8] = 0.000961;
	KdYNO[7][8] = 0.000001;
	YNOeq[7][9] = 0.001066;
	KdYNO[7][9] = 0.000001;
	YNOeq[7][10] = 0.001140;
	KdYNO[7][10] = 0.000001;
	YNOeq[7][11] = 0.000000;
	KdYNO[7][11] = 0.000000;
	YNOeq[8][0] = 0.000010;
	KdYNO[8][0] = 0.000000;
	YNOeq[8][1] = 0.000352;
	KdYNO[8][1] = 0.000000;
	YNOeq[8][2] = 0.000525;
	KdYNO[8][2] = 0.000000;
	YNOeq[8][3] = 0.000653;
	KdYNO[8][3] = 0.000001;
	YNOeq[8][4] = 0.000762;
	KdYNO[8][4] = 0.000001;
	YNOeq[8][5] = 0.000871;
	KdYNO[8][5] = 0.000001;
	YNOeq[8][6] = 0.000981;
	KdYNO[8][6] = 0.000001;
	YNOeq[8][7] = 0.001081;
	KdYNO[8][7] = 0.000002;
	YNOeq[8][8] = 0.001177;
	KdYNO[8][8] = 0.000002;
	YNOeq[8][9] = 0.001270;
	KdYNO[8][9] = 0.000002;
	YNOeq[8][10] = 0.001362;
	KdYNO[8][10] = 0.000002;
	YNOeq[8][11] = 0.001474;
	KdYNO[8][11] = 0.000003;
	YNOeq[9][0] = 0.000017;
	KdYNO[9][0] = 0.000000;
	YNOeq[9][1] = 0.000210;
	KdYNO[9][1] = 0.000001;
	YNOeq[9][2] = 0.000376;
	KdYNO[9][2] = 0.000001;
	YNOeq[9][3] = 0.000543;
	KdYNO[9][3] = 0.000002;
	YNOeq[9][4] = 0.000709;
	KdYNO[9][4] = 0.000003;
	YNOeq[9][5] = 0.000875;
	KdYNO[9][5] = 0.000003;
	YNOeq[9][6] = 0.001041;
	KdYNO[9][6] = 0.000004;
	YNOeq[9][7] = 0.001208;
	KdYNO[9][7] = 0.000005;
	YNOeq[9][8] = 0.001374;
	KdYNO[9][8] = 0.000005;
	YNOeq[9][9] = 0.001523;
	KdYNO[9][9] = 0.000006;
	YNOeq[9][10] = 0.001629;
	KdYNO[9][10] = 0.000007;
	YNOeq[9][11] = 0.001748;
	KdYNO[9][11] = 0.000009;
	YNOeq[10][0] = 0.000029;
	KdYNO[10][0] = 0.000000;
	YNOeq[10][1] = 0.000508;
	KdYNO[10][1] = 0.000003;
	YNOeq[10][2] = 0.000755;
	KdYNO[10][2] = 0.000005;
	YNOeq[10][3] = 0.000936;
	KdYNO[10][3] = 0.000006;
	YNOeq[10][4] = 0.001085;
	KdYNO[10][4] = 0.000008;
	YNOeq[10][5] = 0.001234;
	KdYNO[10][5] = 0.000010;
	YNOeq[10][6] = 0.001382;
	KdYNO[10][6] = 0.000012;
	YNOeq[10][7] = 0.001530;
	KdYNO[10][7] = 0.000014;
	YNOeq[10][8] = 0.001676;
	KdYNO[10][8] = 0.000016;
	YNOeq[10][9] = 0.001806;
	KdYNO[10][9] = 0.000018;
	YNOeq[10][10] = 0.001926;
	KdYNO[10][10] = 0.000020;
	YNOeq[10][11] = 0.002061;
	KdYNO[10][11] = 0.000025;
	YNOeq[11][0] = 0.000045;
	KdYNO[11][0] = 0.000001;
	YNOeq[11][1] = 0.000307;
	KdYNO[11][1] = 0.000005;
	YNOeq[11][2] = 0.000535;
	KdYNO[11][2] = 0.000010;
	YNOeq[11][3] = 0.000763;
	KdYNO[11][3] = 0.000015;
	YNOeq[11][4] = 0.000992;
	KdYNO[11][4] = 0.000020;
	YNOeq[11][5] = 0.001220;
	KdYNO[11][5] = 0.000026;
	YNOeq[11][6] = 0.001448;
	KdYNO[11][6] = 0.000031;
	YNOeq[11][7] = 0.001676;
	KdYNO[11][7] = 0.000036;
	YNOeq[11][8] = 0.001905;
	KdYNO[11][8] = 0.000041;
	YNOeq[11][9] = 0.002108;
	KdYNO[11][9] = 0.000046;
	YNOeq[11][10] = 0.002269;
	KdYNO[11][10] = 0.000056;
	YNOeq[11][11] = 0.002411;
	KdYNO[11][11] = 0.000067;
	YNOeq[12][0] = 0.000067;
	KdYNO[12][0] = 0.000002;
	YNOeq[12][1] = 0.000709;
	KdYNO[12][1] = 0.000024;
	YNOeq[12][2] = 0.001051;
	KdYNO[12][2] = 0.000036;
	YNOeq[12][3] = 0.001300;
	KdYNO[12][3] = 0.000047;
	YNOeq[12][4] = 0.001498;
	KdYNO[12][4] = 0.000059;
	YNOeq[12][5] = 0.001696;
	KdYNO[12][5] = 0.000070;
	YNOeq[12][6] = 0.001894;
	KdYNO[12][6] = 0.000082;
	YNOeq[12][7] = 0.002101;
	KdYNO[12][7] = 0.000098;
	YNOeq[12][8] = 0.002312;
	KdYNO[12][8] = 0.000116;
	YNOeq[12][9] = 0.002488;
	KdYNO[12][9] = 0.000131;
	YNOeq[12][10] = 0.002642;
	KdYNO[12][10] = 0.000143;
	YNOeq[12][11] = 0.002805;
	KdYNO[12][11] = 0.000166;
	YNOeq[13][0] = 0.000093;
	KdYNO[13][0] = 0.000007;
	YNOeq[13][1] = 0.000441;
	KdYNO[13][1] = 0.000035;
	YNOeq[13][2] = 0.000745;
	KdYNO[13][2] = 0.000068;
	YNOeq[13][3] = 0.001049;
	KdYNO[13][3] = 0.000100;
	YNOeq[13][4] = 0.001354;
	KdYNO[13][4] = 0.000133;
	YNOeq[13][5] = 0.001658;
	KdYNO[13][5] = 0.000165;
	YNOeq[13][6] = 0.001962;
	KdYNO[13][6] = 0.000198;
	YNOeq[13][7] = 0.002267;
	KdYNO[13][7] = 0.000230;
	YNOeq[13][8] = 0.002571;
	KdYNO[13][8] = 0.000263;
	YNOeq[13][9] = 0.002840;
	KdYNO[13][9] = 0.000297;
	YNOeq[13][10] = 0.003048;
	KdYNO[13][10] = 0.000349;
	YNOeq[13][11] = 0.003239;
	KdYNO[13][11] = 0.000415;
	YNOeq[14][0] = 0.000132;
	KdYNO[14][0] = 0.000021;
	YNOeq[14][1] = 0.000958;
	KdYNO[14][1] = 0.000156;
	YNOeq[14][2] = 0.001419;
	KdYNO[14][2] = 0.000238;
	YNOeq[14][3] = 0.001753;
	KdYNO[14][3] = 0.000305;
	YNOeq[14][4] = 0.002011;
	KdYNO[14][4] = 0.000368;
	YNOeq[14][5] = 0.002269;
	KdYNO[14][5] = 0.000430;
	YNOeq[14][6] = 0.002527;
	KdYNO[14][6] = 0.000492;
	YNOeq[14][7] = 0.002807;
	KdYNO[14][7] = 0.000597;
	YNOeq[14][8] = 0.003099;
	KdYNO[14][8] = 0.000724;
	YNOeq[14][9] = 0.003337;
	KdYNO[14][9] = 0.000815;
	YNOeq[14][10] = 0.003527;
	KdYNO[14][10] = 0.000865;
	YNOeq[14][11] = 0.003721;
	KdYNO[14][11] = 0.000958;
	YNOeq[15][0] = 0.000175;
	KdYNO[15][0] = 0.000057;
	YNOeq[15][1] = 0.000626;
	KdYNO[15][1] = 0.000212;
	YNOeq[15][2] = 0.001158;
	KdYNO[15][2] = 0.000372;
	YNOeq[15][3] = 0.001555;
	KdYNO[15][3] = 0.000550;
	YNOeq[15][4] = 0.001934;
	KdYNO[15][4] = 0.000729;
	YNOeq[15][5] = 0.002314;
	KdYNO[15][5] = 0.000908;
	YNOeq[15][6] = 0.002693;
	KdYNO[15][6] = 0.001088;
	YNOeq[15][7] = 0.003072;
	KdYNO[15][7] = 0.001267;
	YNOeq[15][8] = 0.003452;
	KdYNO[15][8] = 0.001447;
	YNOeq[15][9] = 0.003733;
	KdYNO[15][9] = 0.001636;
	YNOeq[15][10] = 0.003997;
	KdYNO[15][10] = 0.001895;
	YNOeq[15][11] = 0.004246;
	KdYNO[15][11] = 0.002222;
	YNOeq[16][0] = 0.000207;
	KdYNO[16][0] = 0.000109;
	YNOeq[16][1] = 0.001144;
	KdYNO[16][1] = 0.000754;
	YNOeq[16][2] = 0.001864;
	KdYNO[16][2] = 0.001323;
	YNOeq[16][3] = 0.002299;
	KdYNO[16][3] = 0.001679;
	YNOeq[16][4] = 0.002630;
	KdYNO[16][4] = 0.001974;
	YNOeq[16][5] = 0.002961;
	KdYNO[16][5] = 0.002269;
	YNOeq[16][6] = 0.003291;
	KdYNO[16][6] = 0.002565;
	YNOeq[16][7] = 0.003658;
	KdYNO[16][7] = 0.003131;
	YNOeq[16][8] = 0.004045;
	KdYNO[16][8] = 0.003843;
	YNOeq[16][9] = 0.004328;
	KdYNO[16][9] = 0.004167;
	YNOeq[16][10] = 0.004590;
	KdYNO[16][10] = 0.004491;
	YNOeq[16][11] = 0.004820;
	KdYNO[16][11] = 0.004800;
	YNOeq[17][0] = 0.000305;
	KdYNO[17][0] = 0.000389;
	YNOeq[17][1] = 0.000871;
	KdYNO[17][1] = 0.001117;
	YNOeq[17][2] = 0.001512;
	KdYNO[17][2] = 0.001698;
	YNOeq[17][3] = 0.002012;
	KdYNO[17][3] = 0.002534;
	YNOeq[17][4] = 0.002495;
	KdYNO[17][4] = 0.003402;
	YNOeq[17][5] = 0.002978;
	KdYNO[17][5] = 0.004271;
	YNOeq[17][6] = 0.003460;
	KdYNO[17][6] = 0.005140;
	YNOeq[17][7] = 0.003943;
	KdYNO[17][7] = 0.006009;
	YNOeq[17][8] = 0.004425;
	KdYNO[17][8] = 0.006877;
	YNOeq[17][9] = 0.004795;
	KdYNO[17][9] = 0.007841;
	YNOeq[17][10] = 0.005123;
	KdYNO[17][10] = 0.008969;
	YNOeq[17][11] = 0.005441;
	KdYNO[17][11] = 0.010388;
	YNOeq[18][0] = 0.000340;
	KdYNO[18][0] = 0.000618;
	YNOeq[18][1] = 0.001487;
	KdYNO[18][1] = 0.003637;
	YNOeq[18][2] = 0.002386;
	KdYNO[18][2] = 0.006320;
	YNOeq[18][3] = 0.002942;
	KdYNO[18][3] = 0.007955;
	YNOeq[18][4] = 0.003358;
	KdYNO[18][4] = 0.009197;
	YNOeq[18][5] = 0.003774;
	KdYNO[18][5] = 0.010439;
	YNOeq[18][6] = 0.004190;
	KdYNO[18][6] = 0.011681;
	YNOeq[18][7] = 0.004658;
	KdYNO[18][7] = 0.014287;
	YNOeq[18][8] = 0.005154;
	KdYNO[18][8] = 0.017620;
	YNOeq[18][9] = 0.005490;
	KdYNO[18][9] = 0.018637;
	YNOeq[18][10] = 0.005836;
	KdYNO[18][10] = 0.020254;
	YNOeq[18][11] = 0.006110;
	KdYNO[18][11] = 0.021109;
	YNOeq[19][0] = 0.000498;
	KdYNO[19][0] = 0.002188;
	YNOeq[19][1] = 0.001192;
	KdYNO[19][1] = 0.005203;
	YNOeq[19][2] = 0.001950;
	KdYNO[19][2] = 0.007094;
	YNOeq[19][3] = 0.002567;
	KdYNO[19][3] = 0.010552;
	YNOeq[19][4] = 0.003165;
	KdYNO[19][4] = 0.014212;
	YNOeq[19][5] = 0.003763;
	KdYNO[19][5] = 0.017873;
	YNOeq[19][6] = 0.004361;
	KdYNO[19][6] = 0.021533;
	YNOeq[19][7] = 0.004959;
	KdYNO[19][7] = 0.025193;
	YNOeq[19][8] = 0.005557;
	KdYNO[19][8] = 0.028854;
	YNOeq[19][9] = 0.006029;
	KdYNO[19][9] = 0.033036;
	YNOeq[19][10] = 0.006430;
	KdYNO[19][10] = 0.037389;
	YNOeq[19][11] = 0.006826;
	KdYNO[19][11] = 0.042859;
	YNOeq[20][0] = 0.000532;
	KdYNO[20][0] = 0.003029;
	YNOeq[20][1] = 0.001892;
	KdYNO[20][1] = 0.015222;
	YNOeq[20][2] = 0.002987;
	KdYNO[20][2] = 0.026207;
	YNOeq[20][3] = 0.003679;
	KdYNO[20][3] = 0.032808;
	YNOeq[20][4] = 0.004194;
	KdYNO[20][4] = 0.037504;
	YNOeq[20][5] = 0.004708;
	KdYNO[20][5] = 0.042201;
	YNOeq[20][6] = 0.005223;
	KdYNO[20][6] = 0.046897;
	YNOeq[20][7] = 0.005806;
	KdYNO[20][7] = 0.057298;
	YNOeq[20][8] = 0.006425;
	KdYNO[20][8] = 0.070743;
	YNOeq[20][9] = 0.006822;
	KdYNO[20][9] = 0.073583;
	YNOeq[20][10] = 0.007264;
	KdYNO[20][10] = 0.080335;
	YNOeq[20][11] = 0.007589;
	KdYNO[20][11] = 0.082249;
	YNOeq[21][0] = 0.000769;
	KdYNO[21][0] = 0.010316;
	YNOeq[21][1] = 0.001603;
	KdYNO[21][1] = 0.021430;
	YNOeq[21][2] = 0.002483;
	KdYNO[21][2] = 0.027015;
	YNOeq[21][3] = 0.003225;
	KdYNO[21][3] = 0.039702;
	YNOeq[21][4] = 0.003949;
	KdYNO[21][4] = 0.053304;
	YNOeq[21][5] = 0.004673;
	KdYNO[21][5] = 0.066906;
	YNOeq[21][6] = 0.005397;
	KdYNO[21][6] = 0.080508;
	YNOeq[21][7] = 0.006121;
	KdYNO[21][7] = 0.094110;
	YNOeq[21][8] = 0.006845;
	KdYNO[21][8] = 0.107712;
	YNOeq[21][9] = 0.007431;
	KdYNO[21][9] = 0.123569;
	YNOeq[21][10] = 0.007914;
	KdYNO[21][10] = 0.138603;
	YNOeq[21][11] = 0.008398;
	KdYNO[21][11] = 0.157431;
	YNOeq[22][0] = 0.000733;
	KdYNO[22][0] = 0.008818;
	YNOeq[22][1] = 0.002364;
	KdYNO[22][1] = 0.055774;
	YNOeq[22][2] = 0.003664;
	KdYNO[22][2] = 0.095179;
	YNOeq[22][3] = 0.004507;
	KdYNO[22][3] = 0.118901;
	YNOeq[22][4] = 0.005133;
	KdYNO[22][4] = 0.135043;
	YNOeq[22][5] = 0.005759;
	KdYNO[22][5] = 0.151184;
	YNOeq[22][6] = 0.006385;
	KdYNO[22][6] = 0.167326;
	YNOeq[22][7] = 0.007095;
	KdYNO[22][7] = 0.203906;
	YNOeq[22][8] = 0.007850;
	KdYNO[22][8] = 0.251396;
	YNOeq[22][9] = 0.008317;
	KdYNO[22][9] = 0.258692;
	YNOeq[22][10] = 0.008866;
	KdYNO[22][10] = 0.283291;
	YNOeq[22][11] = 0.009251;
	KdYNO[22][11] = 0.286651;
	YNOeq[23][0] = 0.001135;
	KdYNO[23][0] = 0.041490;
	YNOeq[23][1] = 0.002114;
	KdYNO[23][1] = 0.078217;
	YNOeq[23][2] = 0.003119;
	KdYNO[23][2] = 0.093379;
	YNOeq[23][3] = 0.003993;
	KdYNO[23][3] = 0.135042;
	YNOeq[23][4] = 0.004850;
	KdYNO[23][4] = 0.180119;
	YNOeq[23][5] = 0.005708;
	KdYNO[23][5] = 0.225196;
	YNOeq[23][6] = 0.006566;
	KdYNO[23][6] = 0.270273;
	YNOeq[23][7] = 0.007423;
	KdYNO[23][7] = 0.315350;
	YNOeq[23][8] = 0.008281;
	KdYNO[23][8] = 0.360426;
	YNOeq[23][9] = 0.008991;
	KdYNO[23][9] = 0.413773;
	YNOeq[23][10] = 0.009566;
	KdYNO[23][10] = 0.460907;
	YNOeq[23][11] = 0.010147;
	KdYNO[23][11] = 0.519839;
	YNOeq[24][0] = 0.001060;
	KdYNO[24][0] = 0.032900;
	YNOeq[24][1] = 0.002782;
	KdYNO[24][1] = 0.147795;
	YNOeq[24][2] = 0.003941;
	KdYNO[24][2] = 0.167231;
	YNOeq[24][3] = 0.005066;
	KdYNO[24][3] = 0.258906;
	YNOeq[24][4] = 0.005704;
	KdYNO[24][4] = 0.264210;
	YNOeq[24][5] = 0.006342;
	KdYNO[24][5] = 0.269515;
	YNOeq[24][6] = 0.007100;
	KdYNO[24][6] = 0.328396;
	YNOeq[24][7] = 0.007866;
	KdYNO[24][7] = 0.390753;
	YNOeq[24][8] = 0.008632;
	KdYNO[24][8] = 0.453110;
	YNOeq[24][9] = 0.009542;
	KdYNO[24][9] = 0.635565;
	YNOeq[24][10] = 0.010467;
	KdYNO[24][10] = 0.829667;
	YNOeq[24][11] = 0.011082;
	KdYNO[24][11] = 0.901448;
	YNOeq[25][0] = 0.001260;
	KdYNO[25][0] = 0.060880;
	YNOeq[25][1] = 0.003050;
	KdYNO[25][1] = 0.229015;
	YNOeq[25][2] = 0.004197;
	KdYNO[25][2] = 0.310817;
	YNOeq[25][3] = 0.005218;
	KdYNO[25][3] = 0.375920;
	YNOeq[25][4] = 0.006240;
	KdYNO[25][4] = 0.441022;
	YNOeq[25][5] = 0.007151;
	KdYNO[25][5] = 0.533430;
	YNOeq[25][6] = 0.008052;
	KdYNO[25][6] = 0.717659;
	YNOeq[25][7] = 0.008954;
	KdYNO[25][7] = 0.901887;
	YNOeq[25][8] = 0.009856;
	KdYNO[25][8] = 1.086116;
	YNOeq[25][9] = 0.010694;
	KdYNO[25][9] = 1.250362;
	YNOeq[25][10] = 0.011370;
	KdYNO[25][10] = 1.385262;
	YNOeq[25][11] = 0.012055;
	KdYNO[25][11] = 1.553527;
	YNOeq[26][0] = 0.001407;
	KdYNO[26][0] = 0.092724;
	YNOeq[26][1] = 0.002875;
	KdYNO[26][1] = 0.212964;
	YNOeq[26][2] = 0.004343;
	KdYNO[26][2] = 0.333203;
	YNOeq[26][3] = 0.005701;
	KdYNO[26][3] = 0.555891;
	YNOeq[26][4] = 0.006614;
	KdYNO[26][4] = 0.700366;
	YNOeq[26][5] = 0.007426;
	KdYNO[26][5] = 0.738973;
	YNOeq[26][6] = 0.008332;
	KdYNO[26][6] = 0.913561;
	YNOeq[26][7] = 0.009244;
	KdYNO[26][7] = 1.096972;
	YNOeq[26][8] = 0.010156;
	KdYNO[26][8] = 1.280383;
	YNOeq[26][9] = 0.011238;
	KdYNO[26][9] = 1.810014;
	YNOeq[26][10] = 0.012336;
	KdYNO[26][10] = 2.373222;
	YNOeq[26][11] = 0.013061;
	KdYNO[26][11] = 2.573348;
	YNOeq[27][0] = 0.001732;
	KdYNO[27][0] = 0.191921;
	YNOeq[27][1] = 0.003699;
	KdYNO[27][1] = 0.638817;
	YNOeq[27][2] = 0.004995;
	KdYNO[27][2] = 0.858891;
	YNOeq[27][3] = 0.006161;
	KdYNO[27][3] = 1.035088;
	YNOeq[27][4] = 0.007328;
	KdYNO[27][4] = 1.211285;
	YNOeq[27][5] = 0.008372;
	KdYNO[27][5] = 1.458364;
	YNOeq[27][6] = 0.009428;
	KdYNO[27][6] = 1.965043;
	YNOeq[27][7] = 0.010484;
	KdYNO[27][7] = 2.471722;
	YNOeq[27][8] = 0.011540;
	KdYNO[27][8] = 2.978402;
	YNOeq[27][9] = 0.012519;
	KdYNO[27][9] = 3.428585;
	YNOeq[27][10] = 0.013015;
	KdYNO[27][10] = 3.273183;
	YNOeq[27][11] = 0.014101;
	KdYNO[27][11] = 4.229005;
	YNOeq[28][0] = 0.001892;
	KdYNO[28][0] = 0.271459;
	YNOeq[28][1] = 0.003479;
	KdYNO[28][1] = 0.548492;
	YNOeq[28][2] = 0.005065;
	KdYNO[28][2] = 0.825524;
	YNOeq[28][3] = 0.006468;
	KdYNO[28][3] = 1.347835;
	YNOeq[28][4] = 0.007661;
	KdYNO[28][4] = 1.792081;
	YNOeq[28][5] = 0.008605;
	KdYNO[28][5] = 1.891306;
	YNOeq[28][6] = 0.009663;
	KdYNO[28][6] = 2.357629;
	YNOeq[28][7] = 0.010729;
	KdYNO[28][7] = 2.847770;
	YNOeq[28][8] = 0.011795;
	KdYNO[28][8] = 3.337912;
	YNOeq[28][9] = 0.012961;
	KdYNO[28][9] = 4.311444;
	YNOeq[28][10] = 0.014040;
	KdYNO[28][10] = 5.344972;
	YNOeq[28][11] = 0.015074;
	KdYNO[28][11] = 6.473152;
	YNOeq[29][0] = 0.002311;
	KdYNO[29][0] = 0.547036;
	YNOeq[29][1] = 0.004436;
	KdYNO[29][1] = 1.623547;
	YNOeq[29][2] = 0.005878;
	KdYNO[29][2] = 2.167642;
	YNOeq[29][3] = 0.007187;
	KdYNO[29][3] = 2.608745;
	YNOeq[29][4] = 0.008496;
	KdYNO[29][4] = 3.049848;
	YNOeq[29][5] = 0.009585;
	KdYNO[29][5] = 3.545103;
	YNOeq[29][6] = 0.010636;
	KdYNO[29][6] = 4.567760;
	YNOeq[29][7] = 0.011811;
	KdYNO[29][7] = 5.626905;
	YNOeq[29][8] = 0.012986;
	KdYNO[29][8] = 6.686050;
	YNOeq[29][9] = 0.014160;
	KdYNO[29][9] = 7.745196;
	YNOeq[29][10] = 0.014952;
	KdYNO[29][10] = 7.970170;
	YNOeq[29][11] = 0.015965;
	KdYNO[29][11] = 9.430666;
	YNOeq[30][0] = 0.002479;
	KdYNO[30][0] = 0.729234;
	YNOeq[30][1] = 0.004172;
	KdYNO[30][1] = 1.328519;
	YNOeq[30][2] = 0.005864;
	KdYNO[30][2] = 1.927803;
	YNOeq[30][3] = 0.007296;
	KdYNO[30][3] = 2.736098;
	YNOeq[30][4] = 0.008593;
	KdYNO[30][4] = 3.615589;
	YNOeq[30][5] = 0.009854;
	KdYNO[30][5] = 4.471673;
	YNOeq[30][6] = 0.011080;
	KdYNO[30][6] = 5.647474;
	YNOeq[30][7] = 0.012303;
	KdYNO[30][7] = 6.844018;
	YNOeq[30][8] = 0.013526;
	KdYNO[30][8] = 8.040562;
	YNOeq[30][9] = 0.014740;
	KdYNO[30][9] = 9.826371;
	YNOeq[30][10] = 0.016016;
	KdYNO[30][10] = 12.394166;
	YNOeq[30][11] = 0.015965;
	KdYNO[30][11] = 9.430666;
	YNOeq[31][0] = 0.002998;
	KdYNO[31][0] = 1.423809;
	YNOeq[31][1] = 0.004828;
	KdYNO[31][1] = 2.519829;
	YNOeq[31][2] = 0.006421;
	KdYNO[31][2] = 3.574074;
	YNOeq[31][3] = 0.007964;
	KdYNO[31][3] = 4.619481;
	YNOeq[31][4] = 0.009551;
	KdYNO[31][4] = 6.218774;
	YNOeq[31][5] = 0.011081;
	KdYNO[31][5] = 8.172902;
	YNOeq[31][6] = 0.012270;
	KdYNO[31][6] = 10.093628;
	YNOeq[31][7] = 0.013460;
	KdYNO[31][7] = 12.014354;
	YNOeq[31][8] = 0.014649;
	KdYNO[31][8] = 13.935080;
	YNOeq[31][9] = 0.015837;
	KdYNO[31][9] = 15.873938;
	YNOeq[31][10] = 0.016990;
	KdYNO[31][10] = 18.181945;
	YNOeq[31][11] = 0.015965;
	KdYNO[31][11] = 9.430666;
	YNOeq[32][0] = 0.002987;
	KdYNO[32][0] = 1.478557;
	YNOeq[32][1] = 0.004827;
	KdYNO[32][1] = 2.797413;
	YNOeq[32][2] = 0.006595;
	KdYNO[32][2] = 4.249854;
	YNOeq[32][3] = 0.008122;
	KdYNO[32][3] = 6.151483;
	YNOeq[32][4] = 0.009650;
	KdYNO[32][4] = 8.053111;
	YNOeq[32][5] = 0.011178;
	KdYNO[32][5] = 9.954740;
	YNOeq[32][6] = 0.012568;
	KdYNO[32][6] = 12.597257;
	YNOeq[32][7] = 0.013949;
	KdYNO[32][7] = 15.287844;
	YNOeq[32][8] = 0.015331;
	KdYNO[32][8] = 17.978430;
	YNOeq[32][9] = 0.016690;
	KdYNO[32][9] = 21.877635;
	YNOeq[32][10] = 0.016990;
	KdYNO[32][10] = 18.181945;
	YNOeq[32][11] = 0.015965;
	KdYNO[32][11] = 9.430666;
	YNOeq[33][0] = 0.003794;
	KdYNO[33][0] = 3.423591;
	YNOeq[33][1] = 0.005713;
	KdYNO[33][1] = 5.641213;
	YNOeq[33][2] = 0.007425;
	KdYNO[33][2] = 7.845004;
	YNOeq[33][3] = 0.009093;
	KdYNO[33][3] = 10.045869;
	YNOeq[33][4] = 0.010826;
	KdYNO[33][4] = 13.462808;
	YNOeq[33][5] = 0.012517;
	KdYNO[33][5] = 17.651895;
	YNOeq[33][6] = 0.013877;
	KdYNO[33][6] = 21.407047;
	YNOeq[33][7] = 0.015237;
	KdYNO[33][7] = 25.162198;
	YNOeq[33][8] = 0.016553;
	KdYNO[33][8] = 28.970749;
	YNOeq[33][9] = 0.017734;
	KdYNO[33][9] = 32.850350;
	YNOeq[33][10] = 0.016990;
	KdYNO[33][10] = 18.181945;
	YNOeq[33][11] = 0.015965;
	KdYNO[33][11] = 9.430666;
	YNOeq[34][0] = 0.003697;
	KdYNO[34][0] = 3.302310;
	YNOeq[34][1] = 0.005641;
	KdYNO[34][1] = 5.913553;
	YNOeq[34][2] = 0.007524;
	KdYNO[34][2] = 8.840037;
	YNOeq[34][3] = 0.009206;
	KdYNO[34][3] = 12.826540;
	YNOeq[34][4] = 0.010888;
	KdYNO[34][4] = 16.813044;
	YNOeq[34][5] = 0.012570;
	KdYNO[34][5] = 20.799547;
	YNOeq[34][6] = 0.014116;
	KdYNO[34][6] = 26.307950;
	YNOeq[34][7] = 0.015652;
	KdYNO[34][7] = 31.915095;
	YNOeq[34][8] = 0.017189;
	KdYNO[34][8] = 37.522241;
	YNOeq[34][9] = 0.017734;
	KdYNO[34][9] = 32.850350;
	YNOeq[34][10] = 0.016990;
	KdYNO[34][10] = 18.181945;
	YNOeq[34][11] = 0.015965;
	KdYNO[34][11] = 9.430666;
	YNOeq[35][0] = 0.004045;
	KdYNO[35][0] = 4.577814;
	YNOeq[35][1] = 0.006250;
	KdYNO[35][1] = 9.798461;
	YNOeq[35][2] = 0.008273;
	KdYNO[35][2] = 14.855960;
	YNOeq[35][3] = 0.010156;
	KdYNO[35][3] = 19.786978;
	YNOeq[35][4] = 0.012162;
	KdYNO[35][4] = 26.201035;
	YNOeq[35][5] = 0.013852;
	KdYNO[35][5] = 33.581057;
	YNOeq[35][6] = 0.015309;
	KdYNO[35][6] = 39.701992;
	YNOeq[35][7] = 0.016871;
	KdYNO[35][7] = 48.099433;
	YNOeq[35][8] = 0.018213;
	KdYNO[35][8] = 54.505888;
	YNOeq[35][9] = 0.017734;
	KdYNO[35][9] = 32.850350;
	YNOeq[35][10] = 0.016990;
	KdYNO[35][10] = 18.181945;
	YNOeq[35][11] = 0.015965;
	KdYNO[35][11] = 9.430666;
	YNOeq[36][0] = 0.004503;
	KdYNO[36][0] = 7.015211;
	YNOeq[36][1] = 0.006540;
	KdYNO[36][1] = 11.921544;
	YNOeq[36][2] = 0.008530;
	KdYNO[36][2] = 17.499392;
	YNOeq[36][3] = 0.010358;
	KdYNO[36][3] = 25.335258;
	YNOeq[36][4] = 0.012187;
	KdYNO[36][4] = 33.171123;
	YNOeq[36][5] = 0.014016;
	KdYNO[36][5] = 41.006989;
	YNOeq[36][6] = 0.015687;
	KdYNO[36][6] = 50.802898;
	YNOeq[36][7] = 0.017470;
	KdYNO[36][7] = 59.199654;
	YNOeq[36][8] = 0.018213;
	KdYNO[36][8] = 54.505888;
	YNOeq[36][9] = 0.017734;
	KdYNO[36][9] = 32.850350;
	YNOeq[36][10] = 0.016990;
	KdYNO[36][10] = 18.181945;
	YNOeq[36][11] = 0.015965;
	KdYNO[36][11] = 9.430666;
	YNOeq[37][0] = 0.004910;
	KdYNO[37][0] = 9.582819;
	YNOeq[37][1] = 0.007222;
	KdYNO[37][1] = 19.290391;
	YNOeq[37][2] = 0.009365;
	KdYNO[37][2] = 28.791551;
	YNOeq[37][3] = 0.011378;
	KdYNO[37][3] = 38.132688;
	YNOeq[37][4] = 0.013425;
	KdYNO[37][4] = 47.851436;
	YNOeq[37][5] = 0.015123;
	KdYNO[37][5] = 58.733176;
	YNOeq[37][6] = 0.016886;
	KdYNO[37][6] = 73.864675;
	YNOeq[37][7] = 0.018403;
	KdYNO[37][7] = 84.540857;
	YNOeq[37][8] = 0.018213;
	KdYNO[37][8] = 54.505888;
	YNOeq[37][9] = 0.017734;
	KdYNO[37][9] = 32.850350;
	YNOeq[37][10] = 0.016990;
	KdYNO[37][10] = 18.181945;
	YNOeq[37][11] = 0.015965;
	KdYNO[37][11] = 9.430666;
	YNOeq[38][0] = 0.005081;
	KdYNO[38][0] = 10.810942;
	YNOeq[38][1] = 0.007358;
	KdYNO[38][1] = 21.064593;
	YNOeq[38][2] = 0.009609;
	KdYNO[38][2] = 33.021948;
	YNOeq[38][3] = 0.011576;
	KdYNO[38][3] = 47.544379;
	YNOeq[38][4] = 0.013542;
	KdYNO[38][4] = 62.066810;
	YNOeq[38][5] = 0.015508;
	KdYNO[38][5] = 76.589241;
	YNOeq[38][6] = 0.017375;
	KdYNO[38][6] = 92.178864;
	YNOeq[38][7] = 0.018403;
	KdYNO[38][7] = 84.540857;
	YNOeq[38][8] = 0.018213;
	KdYNO[38][8] = 54.505888;
	YNOeq[38][9] = 0.017734;
	KdYNO[38][9] = 32.850350;
	YNOeq[38][10] = 0.016990;
	KdYNO[38][10] = 18.181945;
	YNOeq[38][11] = 0.015965;
	KdYNO[38][11] = 9.430666;
	YNOeq[39][0] = 0.005869;
	KdYNO[39][0] = 18.928014;
	YNOeq[39][1] = 0.008278;
	KdYNO[39][1] = 36.111013;
	YNOeq[39][2] = 0.010530;
	KdYNO[39][2] = 53.043002;
	YNOeq[39][3] = 0.012659;
	KdYNO[39][3] = 69.780392;
	YNOeq[39][4] = 0.014779;
	KdYNO[39][4] = 85.511999;
	YNOeq[39][5] = 0.016443;
	KdYNO[39][5] = 98.841862;
	YNOeq[39][6] = 0.018292;
	KdYNO[39][6] = 122.897344;
	YNOeq[39][7] = 0.018403;
	KdYNO[39][7] = 84.540857;
	YNOeq[39][8] = 0.018213;
	KdYNO[39][8] = 54.505888;
	YNOeq[39][9] = 0.017734;
	KdYNO[39][9] = 32.850350;
	YNOeq[39][10] = 0.016990;
	KdYNO[39][10] = 18.181945;
	YNOeq[39][11] = 0.015965;
	KdYNO[39][11] = 9.430666;
	YNOeq[40][0] = 0.006016;
	KdYNO[40][0] = 20.651069;
	YNOeq[40][1] = 0.008391;
	KdYNO[40][1] = 38.537591;
	YNOeq[40][2] = 0.010860;
	KdYNO[40][2] = 58.400257;
	YNOeq[40][3] = 0.013135;
	KdYNO[40][3] = 82.521261;
	YNOeq[40][4] = 0.015215;
	KdYNO[40][4] = 111.901382;
	YNOeq[40][5] = 0.017126;
	KdYNO[40][5] = 131.273590;
	YNOeq[40][6] = 0.018292;
	KdYNO[40][6] = 122.897344;
	YNOeq[40][7] = 0.018403;
	KdYNO[40][7] = 84.540857;
	YNOeq[40][8] = 0.018213;
	KdYNO[40][8] = 54.505888;
	YNOeq[40][9] = 0.017734;
	KdYNO[40][9] = 32.850350;
	YNOeq[40][10] = 0.016990;
	KdYNO[40][10] = 18.181945;
	YNOeq[40][11] = 0.015965;
	KdYNO[40][11] = 9.430666;
	YNOeq[41][0] = 0.006678;
	KdYNO[41][0] = 30.590106;
	YNOeq[41][1] = 0.009003;
	KdYNO[41][1] = 56.851468;
	YNOeq[41][2] = 0.011337;
	KdYNO[41][2] = 85.428380;
	YNOeq[41][3] = 0.013641;
	KdYNO[41][3] = 112.821628;
	YNOeq[41][4] = 0.015914;
	KdYNO[41][4] = 139.021404;
	YNOeq[41][5] = 0.017822;
	KdYNO[41][5] = 162.375287;
	YNOeq[41][6] = 0.018292;
	KdYNO[41][6] = 122.897344;
	YNOeq[41][7] = 0.018403;
	KdYNO[41][7] = 84.540857;
	YNOeq[41][8] = 0.018213;
	KdYNO[41][8] = 54.505888;
	YNOeq[41][9] = 0.017734;
	KdYNO[41][9] = 32.850350;
	YNOeq[41][10] = 0.016990;
	KdYNO[41][10] = 18.181945;
	YNOeq[41][11] = 0.015965;
	KdYNO[41][11] = 9.430666;
	YNOeq[42][0] = 0.007042;
	KdYNO[42][0] = 37.697174;
	YNOeq[42][1] = 0.009506;
	KdYNO[42][1] = 67.480598;
	YNOeq[42][2] = 0.012090;
	KdYNO[42][2] = 100.891368;
	YNOeq[42][3] = 0.014310;
	KdYNO[42][3] = 126.744041;
	YNOeq[42][4] = 0.016455;
	KdYNO[42][4] = 166.893523;
	YNOeq[42][5] = 0.017822;
	KdYNO[42][5] = 162.375287;
	YNOeq[42][6] = 0.018292;
	KdYNO[42][6] = 122.897344;
	YNOeq[42][7] = 0.018403;
	KdYNO[42][7] = 84.540857;
	YNOeq[42][8] = 0.018213;
	KdYNO[42][8] = 54.505888;
	YNOeq[42][9] = 0.017734;
	KdYNO[42][9] = 32.850350;
	YNOeq[42][10] = 0.016990;
	KdYNO[42][10] = 18.181945;
	YNOeq[42][11] = 0.015965;
	KdYNO[42][11] = 9.430666;
	YNOeq[43][0] = 0.007797;
	KdYNO[43][0] = 54.697991;
	YNOeq[43][1] = 0.010132;
	KdYNO[43][1] = 94.655766;
	YNOeq[43][2] = 0.012471;
	KdYNO[43][2] = 137.909658;
	YNOeq[43][3] = 0.014842;
	KdYNO[43][3] = 175.933649;
	YNOeq[43][4] = 0.017201;
	KdYNO[43][4] = 214.977433;
	YNOeq[43][5] = 0.017822;
	KdYNO[43][5] = 162.375287;
	YNOeq[43][6] = 0.018292;
	KdYNO[43][6] = 122.897344;
	YNOeq[43][7] = 0.018403;
	KdYNO[43][7] = 84.540857;
	YNOeq[43][8] = 0.018213;
	KdYNO[43][8] = 54.505888;
	YNOeq[43][9] = 0.017734;
	KdYNO[43][9] = 32.850350;
	YNOeq[43][10] = 0.016990;
	KdYNO[43][10] = 18.181945;
	YNOeq[43][11] = 0.015965;
	KdYNO[43][11] = 9.430666;
	YNOeq[44][0] = 0.008082;
	KdYNO[44][0] = 62.810606;
	YNOeq[44][1] = 0.010710;
	KdYNO[44][1] = 114.217438;
	YNOeq[44][2] = 0.013385;
	KdYNO[44][2] = 168.602766;
	YNOeq[44][3] = 0.015685;
	KdYNO[44][3] = 210.612953;
	YNOeq[44][4] = 0.017201;
	KdYNO[44][4] = 214.977433;
	YNOeq[44][5] = 0.017822;
	KdYNO[44][5] = 162.375287;
	YNOeq[44][6] = 0.018292;
	KdYNO[44][6] = 122.897344;
	YNOeq[44][7] = 0.018403;
	KdYNO[44][7] = 84.540857;
	YNOeq[44][8] = 0.018213;
	KdYNO[44][8] = 54.505888;
	YNOeq[44][9] = 0.017734;
	KdYNO[44][9] = 32.850350;
	YNOeq[44][10] = 0.016990;
	KdYNO[44][10] = 18.181945;
	YNOeq[44][11] = 0.015965;
	KdYNO[44][11] = 9.430666;
	YNOeq[45][0] = 0.008989;
	KdYNO[45][0] = 94.851480;
	YNOeq[45][1] = 0.011436;
	KdYNO[45][1] = 142.583015;
	YNOeq[45][2] = 0.013888;
	KdYNO[45][2] = 197.204589;
	YNOeq[45][3] = 0.016267;
	KdYNO[45][3] = 262.823904;
	YNOeq[45][4] = 0.017201;
	KdYNO[45][4] = 214.977433;
	YNOeq[45][5] = 0.017822;
	KdYNO[45][5] = 162.375287;
	YNOeq[45][6] = 0.018292;
	KdYNO[45][6] = 122.897344;
	YNOeq[45][7] = 0.018403;
	KdYNO[45][7] = 84.540857;
	YNOeq[45][8] = 0.018213;
	KdYNO[45][8] = 54.505888;
	YNOeq[45][9] = 0.017734;
	KdYNO[45][9] = 32.850350;
	YNOeq[45][10] = 0.016990;
	KdYNO[45][10] = 18.181945;
	YNOeq[45][11] = 0.015965;
	KdYNO[45][11] = 9.430666;
	YNOeq[46][0] = 0.009261;
	KdYNO[46][0] = 107.006637;
	YNOeq[46][1] = 0.011927;
	KdYNO[46][1] = 185.033724;
	YNOeq[46][2] = 0.014583;
	KdYNO[46][2] = 260.495804;
	YNOeq[46][3] = 0.016267;
	KdYNO[46][3] = 262.823904;
	YNOeq[46][4] = 0.017201;
	KdYNO[46][4] = 214.977433;
	YNOeq[46][5] = 0.017822;
	KdYNO[46][5] = 162.375287;
	YNOeq[46][6] = 0.018292;
	KdYNO[46][6] = 122.897344;
	YNOeq[46][7] = 0.018403;
	KdYNO[46][7] = 84.540857;
	YNOeq[46][8] = 0.018213;
	KdYNO[46][8] = 54.505888;
	YNOeq[46][9] = 0.017734;
	KdYNO[46][9] = 32.850350;
	YNOeq[46][10] = 0.016990;
	KdYNO[46][10] = 18.181945;
	YNOeq[46][11] = 0.015965;
	KdYNO[46][11] = 9.430666;
	YNOeq[47][0] = 0.010112;
	KdYNO[47][0] = 150.669136;
	YNOeq[47][1] = 0.012741;
	KdYNO[47][1] = 233.387507;
	YNOeq[47][2] = 0.015179;
	KdYNO[47][2] = 305.763808;
	YNOeq[47][3] = 0.016267;
	KdYNO[47][3] = 262.823904;
	YNOeq[47][4] = 0.017201;
	KdYNO[47][4] = 214.977433;
	YNOeq[47][5] = 0.017822;
	KdYNO[47][5] = 162.375287;
	YNOeq[47][6] = 0.018292;
	KdYNO[47][6] = 122.897344;
	YNOeq[47][7] = 0.018403;
	KdYNO[47][7] = 84.540857;
	YNOeq[47][8] = 0.018213;
	KdYNO[47][8] = 54.505888;
	YNOeq[47][9] = 0.017734;
	KdYNO[47][9] = 32.850350;
	YNOeq[47][10] = 0.016990;
	KdYNO[47][10] = 18.181945;
	YNOeq[47][11] = 0.015965;
	KdYNO[47][11] = 9.430666;
	YNOeq[48][0] = 0.010506;
	KdYNO[48][0] = 177.128602;
	YNOeq[48][1] = 0.013232;
	KdYNO[48][1] = 270.988925;
	YNOeq[48][2] = 0.015179;
	KdYNO[48][2] = 305.763808;
	YNOeq[48][3] = 0.016267;
	KdYNO[48][3] = 262.823904;
	YNOeq[48][4] = 0.017201;
	KdYNO[48][4] = 214.977433;
	YNOeq[48][5] = 0.017822;
	KdYNO[48][5] = 162.375287;
	YNOeq[48][6] = 0.018292;
	KdYNO[48][6] = 122.897344;
	YNOeq[48][7] = 0.018403;
	KdYNO[48][7] = 84.540857;
	YNOeq[48][8] = 0.018213;
	KdYNO[48][8] = 54.505888;
	YNOeq[48][9] = 0.017734;
	KdYNO[48][9] = 32.850350;
	YNOeq[48][10] = 0.016990;
	KdYNO[48][10] = 18.181945;
	YNOeq[48][11] = 0.015965;
	KdYNO[48][11] = 9.430666;
	YNOeq[49][0] = 0.011414;
	KdYNO[49][0] = 247.150173;
	YNOeq[49][1] = 0.013859;
	KdYNO[49][1] = 270.988925;
	YNOeq[49][2] = 0.015179;
	KdYNO[49][2] = 305.763808;
	YNOeq[49][3] = 0.016267;
	KdYNO[49][3] = 262.823904;
	YNOeq[49][4] = 0.017201;
	KdYNO[49][4] = 214.977433;
	YNOeq[49][5] = 0.017822;
	KdYNO[49][5] = 162.375287;
	YNOeq[49][6] = 0.018292;
	KdYNO[49][6] = 122.897344;
	YNOeq[49][7] = 0.018403;
	KdYNO[49][7] = 84.540857;
	YNOeq[49][8] = 0.018213;
	KdYNO[49][8] = 54.505888;
	YNOeq[49][9] = 0.017734;
	KdYNO[49][9] = 32.850350;
	YNOeq[49][10] = 0.016990;
	KdYNO[49][10] = 18.181945;
	YNOeq[49][11] = 0.015965;
	KdYNO[49][11] = 9.430666;
	YNOeq[50][0] = 0.011781;
	KdYNO[50][0] = 247.150173;
	YNOeq[50][1] = 0.013859;
	KdYNO[50][1] = 270.988925;
	YNOeq[50][2] = 0.015179;
	KdYNO[50][2] = 305.763808;
	YNOeq[50][3] = 0.016267;
	KdYNO[50][3] = 262.823904;
	YNOeq[50][4] = 0.017201;
	KdYNO[50][4] = 214.977433;
	YNOeq[50][5] = 0.017822;
	KdYNO[50][5] = 162.375287;
	YNOeq[50][6] = 0.018292;
	KdYNO[50][6] = 122.897344;
	YNOeq[50][7] = 0.018403;
	KdYNO[50][7] = 84.540857;
	YNOeq[50][8] = 0.018213;
	KdYNO[50][8] = 54.505888;
	YNOeq[50][9] = 0.017734;
	KdYNO[50][9] = 32.850350;
	YNOeq[50][10] = 0.016990;
	KdYNO[50][10] = 18.181945;
	YNOeq[50][11] = 0.015965;
	KdYNO[50][11] = 9.430666;
	YNOeq[51][0] = 0.012459;
	KdYNO[51][0] = 247.150173;
	YNOeq[51][1] = 0.013859;
	KdYNO[51][1] = 270.988925;
	YNOeq[51][2] = 0.015179;
	KdYNO[51][2] = 305.763808;
	YNOeq[51][3] = 0.016267;
	KdYNO[51][3] = 262.823904;
	YNOeq[51][4] = 0.017201;
	KdYNO[51][4] = 214.977433;
	YNOeq[51][5] = 0.017822;
	KdYNO[51][5] = 162.375287;
	YNOeq[51][6] = 0.018292;
	KdYNO[51][6] = 122.897344;
	YNOeq[51][7] = 0.018403;
	KdYNO[51][7] = 84.540857;
	YNOeq[51][8] = 0.018213;
	KdYNO[51][8] = 54.505888;
	YNOeq[51][9] = 0.017734;
	KdYNO[51][9] = 32.850350;
	YNOeq[51][10] = 0.016990;
	KdYNO[51][10] = 18.181945;
	YNOeq[51][11] = 0.015965;
	KdYNO[51][11] = 9.430666;

	if (temperature < 1250) {
		i = 1;
	}
	else if (temperature > 3750) {
		i = 50;
	}
	else {
		i = (temperature - 1200) / 50;
	}
	if (mO2 / mtotal <= 0.11) {
		j = (mO2 / mtotal + 0.01) / 0.01;
		di = (temperature - 1200) / 50 - i;
		dj = (mO2 / mtotal + 0.01) / 0.01 - j;
		i--;
		j--;
	}
	else {
		j = (0.10 + 0.01) / 0.01;
		di = (temperature - 1200) / 50 - i;
		dj = (0.10 + 0.01) / 0.01 - j;
		i--;
		j--;
	}

	if (i < 0) {
		i = 0;
		di = 0.00001;
	}
	if (j < 0) {
		j = j;
		dj = 0.00001;
	}

	if (di == 0)
		di = 0.00001;
	if (dj == 0)
		dj = 0.00001;

	*YNOeq_value = (1 / dj * (1 / di * YNOeq[i][j] + 1 / (1 - di) * YNOeq[i + 1][j]) /
		(1 / di + 1 / (1 - di)) + 1 / (1 - dj) * (1 / di * YNOeq[i][j + 1] + 1 / (1 - di)
			* YNOeq[i + 1][j + 1]) / (1 / di + 1 / (1 - di))) / (1 / dj + 1 / (1 - dj));

	*KdYNO_value = (1 / dj * (1 / di * KdYNO[i][j] + 1 / (1 - di) * KdYNO[i + 1][j]) /
		(1 / di + 1 / (1 - di)) + 1 / (1 - dj) * (1 / di * KdYNO[i][j + 1] + 1 / (1 - di)
			* KdYNO[i + 1][j + 1]) / (1 / di + 1 / (1 - di))) / (1 / dj + 1 / (1 - dj));

}

inline void FUNCTION_SOOT_C(double *soot_pre, double element_FI) {

	int i;
	int di, dk;
	double *soot_pre_vector;
	double *FI_vector;

	soot_pre_vector = (double*)malloc(11 * sizeof(double));
	FI_vector = (double*)malloc(11 * sizeof(double));

	soot_pre_vector[0] = 0;
	soot_pre_vector[1] = 0.01;
	soot_pre_vector[2] = 0.02;
	soot_pre_vector[3] = 0.05;
	soot_pre_vector[4] = 0.1;
	soot_pre_vector[5] = 0.2;
	soot_pre_vector[6] = 0.6;
	soot_pre_vector[7] = 0.7;
	soot_pre_vector[8] = 0.8;
	soot_pre_vector[9] = 0.9;
	soot_pre_vector[10] = 1;

	FI_vector[0] = 0;
	FI_vector[1] = 1;
	FI_vector[2] = 1.5;
	FI_vector[3] = 2;
	FI_vector[4] = 2.2;
	FI_vector[5] = 2.5;
	FI_vector[6] = 3;
	FI_vector[7] = 3.5;
	FI_vector[8] = 4;
	FI_vector[9] = 5.5;
	FI_vector[10] = 10;

	if (element_FI >= 10) {
		*soot_pre = 1;
	}
	else {
		dk = 0;
		for (i = 0; i < 10; i++) {
			if ((dk == 0) && (element_FI >= FI_vector[i])) {
				dk = 1;
				di = i;
			}
		}
		*soot_pre = (element_FI - FI_vector[di]) * (soot_pre_vector[di + 1] - soot_pre_vector[di])
			/ (FI_vector[di + 1] - FI_vector[di]) + soot_pre_vector[di];

	}

	free(soot_pre_vector);
	free(FI_vector);

}

// ****************************************************************************************************
/* ______________________________________________________________________________________________________________________

COMBUSTION MODEL MAIN FUNCTION
______________________________________________________________________________________________________________________ */

// ****************************************************************************************************

/** ************     ACT FUNCTION     ************** */

inline void ACT(double *engine_parameters, double *engine_model_constants, double *test_variables,
	double *injection_rate, double *CAD_injection_rate, int size_inlet_inj, int INITIAL,
	double *SOI, double *EOI, int CAI, double *CAD_exit, double *HRF_exit,
	double *ROHR_exit, double *p_cyl_exit, double *dp_da_cyl_exit, double *T_cyl_exit,
	double *H_cooler_exit, double *mean_var_exit, double *heat_transfer,
	double *injection_rate_exit, double *accum_injection_rate_exit,
	double *species_EVO_exit) {

	// element mixture and combustion state
	enum stState {
		stLiquid = 0, stEvaporated = 1, stOvermixed = 2, stBurned_poor_premix = 3,
		stBurned_rich_premix = 4, stBurned_by_diffusion = 5, stBurned_by_second_diffusion = 6,
		stInto_diffusion_flame = 7
	};

	// General constants
	double PI = 3.1415926;
	double Runiv = 8314.;
	FILE *fich;
	double YO2aire = 0.23019;
	double mmH2O = 18.; // Molecular weight for H2O
	double mmCO2 = 44.; // Molecular weight for CO2

	char title[3000];

	// engine parameters assignation

	double Piston_D = engine_parameters[0]; /* Piston diamteter(m) */
	double S = engine_parameters[1]; /* Piston stroke(m) */
	double Crank_L = engine_parameters[2]; /* Crank length(m) */
	double Connecting_Rod_L = engine_parameters[3]; /* Connecting rod length(m) */
	double E = engine_parameters[4]; /* Piston eccentricity(mm) */
	double Piston_Axis_D = engine_parameters[5]; /* Piston axis diameter(m) */
	double Piston_Crown_H = engine_parameters[6]; /* Piston crown height(m) */
	double DBowl = engine_parameters[7]; /* Maximum diameter of bowl(m) */
	double VBowl = engine_parameters[8]; /* Volume of Bowl(m3) */
	double M_Connecting_Rod = engine_parameters[9]; /* Mass Connecting rod(Kg) */
	double M_P_R_PA = engine_parameters[10]; /* Mass of piston+rings+piston axis(Kg) */
	double C_ESteel = engine_parameters[11]; /* Elasticity module of the steel(N/m2) */
	double C_Mech_Defor = engine_parameters[12]; /* Mechanical deformations coefficient */
	double C_MBLBY = engine_parameters[13]; /* Coefficient of leak's section of Blow-by */
	double GCRatio = engine_parameters[14]; /* Geometric compresion ratio */
	double n_holes = engine_parameters[15]; /* Number of holes of the nozzle */
	double nozzle_d = engine_parameters[16]; /* nozzle diameter (m) */
	double dc = engine_parameters[17]; /* discharge coefficient of the nozzle */
	double CTM = engine_parameters[18]; /* parameter to calculate C1 */
	double WC1A = engine_parameters[19]; /* Constant A to calculate C1 */
	double WC1B = engine_parameters[20]; /* Constant B to calculate C1 */
	double C2 = engine_parameters[21]; /* Coefficient to the Woschin's calculus */
	double IVC = engine_parameters[22]; /* Inlet valve closing (�) */
	double EVO = engine_parameters[23]; /* Exhaust valve opening (�) */

	double Kmixture1 = engine_parameters[24]; /* mixture combustion model constants */

	// combustion model constant assignation

	double Kmixture2 = engine_model_constants[0];

	double Kpmx1 = engine_model_constants[1]; /* premix combustion model constants */
	double Kpmx2 = engine_model_constants[2];
	double Kpmx3 = engine_model_constants[3];
	double Kpmx4 = engine_model_constants[4];
	double Kpmx5 = engine_model_constants[5];

	double KID1 = engine_model_constants[6]; /* Ignition delay time model constants */
	double KID2 = engine_model_constants[7];
	double KID3 = engine_model_constants[8];
	double KID4 = engine_model_constants[9];
	double KID5 = engine_model_constants[10];

	double KNOx1 = engine_model_constants[11]; /* Nox emission prediction model constants */
	double KNOx2 = engine_model_constants[12];

	double EC = engine_model_constants[13];

	double KSOOTA1 = engine_model_constants[14]; /* SOOT emission prediction model constants */
	double KSOOTA2 = engine_model_constants[15];
	double KSOOTA3 = engine_model_constants[16];

	double KSOOTA4 = engine_model_constants[17];
	double KSOOTA5 = engine_model_constants[18];
	double KSOOTA6 = engine_model_constants[19];
	double KSOOTA7 = engine_model_constants[20];

	double KSOOTC1 = 0.06861198; /* SOOT emission prediction model constants */
	double KSOOTC2 = 0.5;
	double KSOOTC3 = 6296;
	double KSOOTC4 = 856.64;
	double KSOOTC5 = 1;
	double KSOOTC6 = 1.8;
	double KSOOTC7 = 7050;

	double KLO = 1;

	double SOOT_EVO_A;
	double SOOT_EVO_C;

	double Kswirl = 0; /* swirl correction model constant */

	// test variables assignement

	double speed = test_variables[0]; /* Engine speed (rpm) */
	double mairadm = test_variables[1]; /* In-cylinder air mass at inlet valve closing (g) */
	double mairIVC = test_variables[2]; /* In-cylinder air mass at inlet valve closing (g) */
	double temperatureIVC = test_variables[3];
	/* In_cylinder temperature at inlet valve closing(K) */
	double mfuel = test_variables[4]; /* Total injected fuel mass (mg) */
	double PCR = test_variables[5];
	double inlet_pres = test_variables[6]; /* Inlet pressure (Pa) */
	double exhaust_pres = test_variables[7]; /* Exhaust pressure (Pa) */
	double Cbb = test_variables[8]; /* Adjustment coefficient of Blow-by */
	double Atmosphere_press = test_variables[9]; /* Atmosphere pressure (Pa) */
	double inj_fuel_temp = test_variables[10]; /* Injection fuel temperature (K) */
	double TCYL_HEAD = test_variables[11]; /* Piston head temperature(K) */
	double TCYL = test_variables[12]; /* Cylinder temperature(K) */
	double TPIS = test_variables[13]; /* Piston temperature(K) */

	double rateBG = 1. - mairadm / mairIVC;

	// fuel properties

	double rofuel = 830.; /* Fuel density (kg/m3) */
	double HC = 1.7843; /* Fuel H/C ratio */
	double AFe = 14.5; /* Stoichiometric air / fuel ratio */
	double mmfuel = 12. + HC; // Molecular weight of the fuel (for just 1 C atome)
	double Vc_factor = 0.8; /* Engine K factor */
	double D = 0.0018; /* Dissipation constant */
	double MW_air = 28.97; /* Molecular weight of air(g/mol) */
	double MW_fuel = 152.2; /* Molecular weight of fuel(g/mol) */
	double MW_burned = 29.13; /* Molecular weight of burned products(g/mol) */
	double HP = 42920000.; /* Net heat of combustion(J/Kg) */
	double T_Evaporation_fuel = 489.35; /* Temperature of evaporation of fuel(K) */

	// D=0.0018*2.5*speed/2000.;                                      /* Dissipation constant */

	// individual variables declaration

	double PRECISION_ITERATION = 0.000005; /* Value to iterate */
	double delta_t; /* Calculus interval time (s) */
	int size; /* Size of the temporal vector */
	double mf_burned; /* Fuel mass burned (-) */
	double Cylinder_capacity; /* Cylinder capacity(m3) */
	double VTDC; /* Volume at top dead center(m3) */
	double Gamma; /* Politropic exponent */
	double f; /* Real air fuel ratio */
	double mEGR; /* Burned gases mass(Kg) */
	double p_exit; /* In cylinder pressure at exhaust valve openning (bar) */
	double T_exit; /* In cylinder temperature at exhaust valve openning (K) */
	double WI_HP = 0; /* Indicated work of pressure high cicle(J) */
	double IMP_HP = 0; /* Indicated Mean Pressure of pressure high cicle(bar) */
	double pmax = 0; /* In-cylinder Maximum pressure(bar) */
	double Tmax = 0; /* In cylinder Maximum temperature(K) */
	double dp_da_max = 0; /* Maximum dp/d(alfa) (bar/�) */

	// TEMPORAL VARIABLES DECLARATION

	double *acu_dmf; /* Accumulated injection rate (-) */
	double *time_vector; /* Time (s) [IVC - EVO] */
	double *time_vector_exit; /* Time (s) [-180 - +180] */
	double *CAD; /* Crank angle degrees (�) [IVC - EVO] */
	double *complete_CAD; /* Crank angle degrees (�) [-180 - +180] used to calculate
	the IMP_HP */
	double *roair; /* In-cylinder air density (kg/m3) */
	double *virtual_velocity; /* Virtual velocity (m/s) */
	double *inj_velocity; /* Injection velocity (m/s) */
	double *p_cyl; /* In-cylinder pressure (bar) [IVC - EVO] */
	double *dp_da_cyl;
	double *complete_p_cyl; /* In-cylinder pressure (bar) [-180 - +180] */
	double *T_cyl; /* Averaged in-cylinder temperature (K) */
	double *HRF; /* Heat release fraction (-) (FQL) */
	double *ROHR;
	double *Yair; /* Air fraction */
	double *Yfuel; /* Fuel fraction */
	double *Yburned; /* Stoichiometricly burned gas fraction */
	double *Mbb; /* Blow-by mass (Kg) */
	double *acu_Mbb; /* Accumulated blow-by mass (Kg) */
	double *U; /* Internal energy (J) */
	double *CV; /* At constant volume heat (J/Kg) */
	double *H_cooler; /* Heat hung over to the cooler (J/�) */
	double *Qcylhead;
	double *Qcyl;
	double *Qpis;
	double *H; /* Convection coefficient */
	double *defor; /* Increase of volume due to cylinder pressure and inertia
	(m3) [IVC - EVO] */
	double *Rmixture; /* Constant of perfect gases */
	double *V_cyl; /* In-cylinder volume (m3) [IVC - EVO] */
	double *complete_V_cyl; /* In-cylinder volume (m3) [-180 - +180] used to shown the
	results */
	double *complete_deform;
	/* Increase of volume due to cylinder pressure and inertia (m3) [-180 - +180] used to calculate the IMP_HP */
	double *dmf;
	double *SOI_IM; /* start of injections of multiple pulse */
	double *EOI_IM; /* end of injections of multiple pulse */
	double *aux_vector;

	// ELEMENT AND SUB-ELEMENT VARIABLES DECLARATION

	int num_i; /* Number of elements i */
	int num_j; /* Number of sub-elements j */
	double *mixture_correction; /* Burnt limit for the sub-element j */
	int inj_num;
	int inj_counter;
	double rate_area;

	int i_aux, j_aux;

	// MULTIPLE INJECTION VARIABLES

	double **POI_IM;
	double **POC_IM;
	double **mfuel_i_IM;
	double **mfuel_ij_IM;
	int *num_i_IM;
	double *SOC_IM;

	// NOx emission variables
	double YNOeq_value;
	double KdYNO_value;

	double **YNOeq;
	double **KdYNO;

	// SOOT emission variables;

	double soot_pre;
	double RES_FSN;

	// ELEMENT TRACKING MATRIX: STATE, COMPOSITION AND CHARACTERISTICS

	struct stPropertiesElement {
		int state; /* element mixture and combustion state */
		double mtotal; /* element total mass */
		double dmtotal; /* element mass include at mixture process */
		double mO2; /* element O2 mass */
		double mN2; /* element N2 mass */
		double mCO2; /* element CO2 mass */
		double mH2O; /* element H2O mass */
		double mf_jet; /* element fuel mass injected according to injection rate law */
		double mf_reac; /* element fuel mass updated with mixture */
		double mf_evap; /* element fuel mass updated with mixture and combustion process */
		double C; /* element fuel concentration */
		double FI; /* element air-fuel ratio */
		double RID; /* element ignition delay time intensity */
		double Rpmx; /* element premix combustion intensity */
		double Rpmx_value;
		double mNOx; /* element NOx mass */
		double dNOx; /* element NOx mass increment */
		double HC; /* element HC mass */
		double CO; /* element CO mass */
		double mSOOT_A; /* element SOOT mass */
		double dSOOT_A;
		double mSOOT_B; /* element SOOT mass */
		double dSOOT_B;
		double mSOOT_C; /* element SOOT mass */
		double dSOOT_C;
		double TSD;
		double Tadib;
		double Pcyl_POC;
		double TNOx;
		double X;
		double soot_precursor;

	}**element;

	double *XLO;

	struct stRealElementComposition {
		double mtotal; /* element total mass */
		double mO2; /* element O2 mass */
		double mO; /* element O mass wich is provided by O2, H2O and CO2 */
		double mCH;
		double mN2; /* element N2 mass */
		double mCO2; /* element CO2 mass */
		double mH2O; /* element H2O mass */
		double mf_reac; /* element fuel mass updated with mixture and combustion process */
		double C; /* element fuel concentration */
		double FI; /* element air-fuel ratio */
	}**realelement;

	stControlElementComposition **elementcontrol;

	// inlet valve closing composition variables declaration

	double mtotal_IVC; /* Total mass at inlet valve closing */
	double mO2_IVC; /* O2 mass at inlet valve closing coming from air fresh mass */
	double mN2_IVC; /* N2 mass at inlet valve closing coming from air fresh mass */
	double mCO2_IVC; /* CO2 mass at inlet valve closing coming from exhaust gasses recirculated */
	double mH2O_IVC; /* H2O mass at inlet valve closing coming from exhaust gasses recirculated */
	double NOx_IVC;
	/* NOx mass at inlet valve closing coming from exhaust gasses recirculated emissions */
	double mSOOT_IVC_B;
	/* SOOT mass at inlet valve closing coming from exhaust gasses recirculated emissions */
	double mSOOT_IVC_A;
	double mSOOT_IVC_C;
	double CO_IVC;
	double HC_IVC;
	double YO2IVC; /* Mass oxygen concentration at inlet valve closing */
	double YN2IVC; /* Mass nitrogen concentration at inlet valve closing */
	double YCO2IVC;
	double YH2OIVC;

	// bowl variables definition: composition, species and characteristics

	double *mtotal_bowl;
	double *mO2_bowl;
	double *mN2_bowl;
	double *mCO2_bowl;
	double *mH2O_bowl;
	double *mHC_bowl;
	double *Tsq_cyl;
	double *Tadib_cyl;
	double *mNOx_bowl;
	double *HC_bowl;
	double *mCO_bowl;
	double *mSOOT_bowl_A;
	double *mSOOT_bowl_B;
	double *mSOOT_bowl_C;

	double mSOOT_bowl_A_i_burned;
	double mSOOT_bowl_B_i_burned;
	double mSOOT_bowl_C_i_burned;

	// Dead volume variables definition: composition, species and characteristics

	double *mtotal_Vc;
	double *mO2_Vc;
	double *mN2_Vc;
	double *mCO2_Vc;
	double *mH2O_Vc;
	double *mNOx_Vc;
	double *mSOOT_Vc_A;
	double *mSOOT_Vc_B;
	double *mSOOT_Vc_C;
	double *mHC_Vc;
	double *dmtotal_Gfactor; /* increment mass exchange between bowl and dead volume */

	// stoichometric constant link with HC fuel ratio
	double Kst1;
	double Kst2;
	double Kst3;
	double Kst4;
	double Kst5;
	double Kst6;

	/** *******AUXILIAR VARIABLES DECLARATION********* */

	int complete_size; /* vector size from -180 to +180 with a  delta_CAD increment */
	int complete_prev_size; /* vector size from -180 to IVC with a  delta_CAD increment */
	int complete_post_size; /* vector size from EVO to +180 with a  delta_CAD increment */
	double auxiliar;
	double *vector_to_interpolate;

	double counter_CAD_1;
	double counter_CAD_2;
	int counter;
	int m, i, j, aux;
	double Y1, T1, RID1;

	double maximum;
	double minimum;
	double aux_mfuel;

	int element_value;

	if (mfuel == 0) {
		mfuel = 1e-6;
	}

	// crank angle degree and time vector creation: time vector is created with a constant increment 20 microseconds
	// vector size calculation with the name 'size'. this will be the dimension of all the instantaneous cylinder vectors

	delta_t = 2. * 1e-5;
	size = 0;
	auxiliar = IVC * 60. / (360. * speed);
	do {
		size = size + 1;
		auxiliar = auxiliar + delta_t;
	}
	while (auxiliar <= EVO * 60. / (360. * speed));

	CAD = (double*)malloc(size*sizeof(double));
	time_vector = (double*)malloc(size*sizeof(double));

	for (counter = 0; counter < size; counter++) {
		if (counter == 0) {
			time_vector[counter] = IVC * 60. / (360. * speed);
			CAD[counter] = IVC;
		}
		else {
			time_vector[counter] = time_vector[counter - 1] + delta_t;
			CAD[counter] = CAD[counter - 1] + delta_t * 360. * speed / 60.;
		}
	}

	// instantaneous cylinder vectors take format with size dimension

	SOI_IM = (double*)malloc(8 * sizeof(double));
	EOI_IM = (double*)malloc(8 * sizeof(double));
	SOC_IM = (double*)malloc(8 * sizeof(double));

	mtotal_bowl = (double*)malloc(size*sizeof(double));
	mO2_bowl = (double*)malloc(size*sizeof(double));
	mN2_bowl = (double*)malloc(size*sizeof(double));
	mCO2_bowl = (double*)malloc(size*sizeof(double));
	mH2O_bowl = (double*)malloc(size*sizeof(double));
	mHC_bowl = (double*)malloc(size*sizeof(double));
	Tsq_cyl = (double*)malloc(size*sizeof(double));
	Tadib_cyl = (double*)malloc(size*sizeof(double));
	mNOx_bowl = (double*)malloc(size*sizeof(double));
	mCO_bowl = (double*)malloc(size*sizeof(double));
	HC_bowl = (double*)malloc(size*sizeof(double));
	mSOOT_bowl_A = (double*)malloc(size*sizeof(double));
	mSOOT_bowl_B = (double*)malloc(size*sizeof(double));
	mSOOT_bowl_C = (double*)malloc(size*sizeof(double));

	mtotal_Vc = (double*)malloc(size*sizeof(double));
	mO2_Vc = (double*)malloc(size*sizeof(double));
	mN2_Vc = (double*)malloc(size*sizeof(double));
	mCO2_Vc = (double*)malloc(size*sizeof(double));
	mH2O_Vc = (double*)malloc(size*sizeof(double));
	mNOx_Vc = (double*)malloc(size*sizeof(double));
	mSOOT_Vc_A = (double*)malloc(size*sizeof(double));
	mSOOT_Vc_B = (double*)malloc(size*sizeof(double));
	mSOOT_Vc_C = (double*)malloc(size*sizeof(double));
	mHC_Vc = (double*)malloc(size*sizeof(double));
	dmtotal_Gfactor = (double*)malloc(size*sizeof(double));

	dmf = (double*)malloc(size*sizeof(double));
	acu_dmf = (double*)malloc(size*sizeof(double));
	inj_velocity = (double*)malloc(size*sizeof(double));
	virtual_velocity = (double*)malloc(size*sizeof(double));

	HRF = (double*)malloc(size*sizeof(double));
	ROHR = (double*)malloc(size*sizeof(double));
	T_cyl = (double*)malloc(size*sizeof(double));
	acu_Mbb = (double*)malloc(size*sizeof(double));
	V_cyl = (double*)malloc(size*sizeof(double));
	Mbb = (double*)malloc(size*sizeof(double));
	Yair = (double*)malloc(size*sizeof(double));
	Yburned = (double*)malloc(size*sizeof(double));
	Yfuel = (double*)malloc(size*sizeof(double));
	U = (double*)malloc(size*sizeof(double));
	CV = (double*)malloc(size*sizeof(double));
	H_cooler = (double*)malloc(size*sizeof(double));
	Qcylhead = (double*)malloc(size*sizeof(double));
	Qcyl = (double*)malloc(size*sizeof(double));
	Qpis = (double*)malloc(size*sizeof(double));
	H = (double*)malloc(size*sizeof(double));
	defor = (double*)malloc(size*sizeof(double));
	Rmixture = (double*)malloc(size*sizeof(double));
	roair = (double*)malloc(size*sizeof(double));
	p_cyl = (double*)malloc(size*sizeof(double));
	dp_da_cyl = (double*)malloc(size*sizeof(double));
	XLO = (double*)malloc(size*sizeof(double));

	aux_vector = (double*)malloc(CAI*sizeof(double));

	// equilibrium NOx emession behaviour map

	YNOeq = (double * *)malloc(52 * sizeof(double));
	for (counter = 0; counter < 52; counter++) {
		YNOeq[counter] = (double*)malloc(12 * sizeof(double));
	}

	KdYNO = (double * *)malloc(52 * sizeof(double));
	for (counter = 0; counter < 52; counter++) {
		KdYNO[counter] = (double*)malloc(12 * sizeof(double));
	}

	// INJECTION RATE CONSTRUCTION

	for (counter = 0; counter < INITIAL; counter++) {
		SOI_IM[counter] = SOI[counter];
		EOI_IM[counter] = EOI[counter];
		SOC_IM[counter] = EVO;
	}
	vector_to_interpolate = (double*)malloc(size_inlet_inj*sizeof(double));
	for (counter = 0; counter < size_inlet_inj; counter++) {
		vector_to_interpolate[counter] = injection_rate[counter];
	}

	FUNCTION_FOR_INTERPOLATION(dmf, time_vector, CAD_injection_rate, vector_to_interpolate, size,
		size_inlet_inj, speed);
	free(vector_to_interpolate);

	for (counter = 0; counter < size; counter++) {
		if (time_vector[counter] <= SOI_IM[0] * 60. / (360. * speed)) {
			dmf[counter] = 0.;
		}
		if (time_vector[counter] >= EOI_IM[INITIAL - 1] * 60. / (360. * speed)) {
			dmf[counter] = 0.;
		}
		for (inj_counter = 1; inj_counter < INITIAL; inj_counter++) {
			if ((time_vector[counter] >= EOI_IM[inj_counter - 1] * 60. / (360. * speed)) &&
				(time_vector[counter] <= SOI_IM[inj_counter] * 60. / (360. * speed))) {
				dmf[counter] = 0.;
			}
		}

		if (dmf[counter] < 0) {
			dmf[counter] = 0.;
		}

	}

	// injection rate area calculation and injection rate law scale: rate shape is kept but his dimension is changed to
	// agree with fuel mass inlet variable

	rate_area = 0;
	for (counter = 0; counter < size - 1; counter++) {
		rate_area = rate_area + (time_vector[counter + 1] - time_vector[counter]) * min
			(dmf[counter], dmf[counter + 1]) + \
(time_vector[counter + 1] - time_vector[counter]) * pow(pow((dmf[counter + 1] - dmf[counter]) / 2,
				2), 0.5);
	}

	if (rate_area > 0) {
		for (counter = 0; counter < size; counter++) {
			dmf[counter] = dmf[counter] * mfuel * 1e-6 / rate_area;
		}
	}

	inj_num = INITIAL;

	/** *******ADIMENSIONAL ACCUMULATED INJECTION RATE********* */

	CALCULUS_OF_ACCUMULATED_INJ_RATE(acu_dmf, dmf, time_vector, size);

	/** *******INJECTION VELOCITY AND VIRTUAL VELOCITY********* */

	CALCULUS_OF_VIRTUAL_VELOCITY(inj_velocity, virtual_velocity, dmf, time_vector, rofuel, dc,
		n_holes, nozzle_d, D, size, PI, speed, EOI_IM, inj_num, SOI_IM, Piston_D, DBowl, CTM, CAD,
		Kswirl);

	/** ******* STOICHIOMETRY CONSTANTS********* */

	STOICHIOMETRY_CONSTANTS(HC, &Kst1, &Kst2, &Kst3, &Kst4, &Kst5, &Kst6);

	/** *******NUMBER OF ELEMENTS i******** */

	num_i_IM = (int*)malloc(inj_num*sizeof(int));

	num_i = 0; /* Initialization of number of elements to 0 */
	num_j = 5; /* Number of sub-elements fixed to 5 */

	CALCULUS_OF_NUMBER_ELEMENTS(num_i_IM, time_vector, size, speed, SOI_IM, EOI_IM, inj_num);

	/** *******POI OF EACH i AND FUEL MASS OF EACH ELEMENT i AND SUB-ELEMENT j********* */
	num_i = num_i_IM[0];

	POI_IM = (double * *)malloc(inj_num*sizeof(double));
	for (counter = 0; counter < inj_num; counter++) {
		num_i = num_i_IM[counter];
		POI_IM[counter] = (double*)malloc(num_i*sizeof(double));
	}

	POC_IM = (double * *)malloc(inj_num*sizeof(double));
	for (counter = 0; counter < inj_num; counter++) {
		element_value = num_i_IM[counter] * num_j;
		POC_IM[counter] = (double*)malloc(element_value*sizeof(double));
	}

	mfuel_i_IM = (double * *)malloc(inj_num*sizeof(double));
	for (counter = 0; counter < inj_num; counter++) {
		num_i = num_i_IM[counter];
		mfuel_i_IM[counter] = (double*)malloc(num_i*sizeof(double));
	}

	mfuel_ij_IM = (double * *)malloc(inj_num*sizeof(double));
	for (counter = 0; counter < inj_num; counter++) {
		num_i = num_i_IM[counter];
		mfuel_ij_IM[counter] = (double*)malloc(num_i*sizeof(double));
	}

	elementcontrol = (struct stControlElementComposition * *)malloc
		(15 * sizeof(struct stControlElementComposition));
	for (counter = 0; counter < 15; counter++) {
		elementcontrol[counter] = (struct stControlElementComposition*)malloc
			(size*sizeof(struct stControlElementComposition));
	}

	CALCULUS_OF_POI(POI_IM, mfuel_i_IM, mfuel_ij_IM, acu_dmf, time_vector, size, speed, num_i_IM,
		num_j, SOI_IM, EOI_IM, inj_num, elementcontrol);

	mixture_correction = (double*)malloc(num_j*sizeof(double));

	element = (struct stPropertiesElement * *)malloc(inj_num*sizeof(struct stPropertiesElement));
	for (counter = 0; counter < inj_num; counter++) {
		element_value = num_i_IM[counter] * num_j;
		element[counter] = (struct stPropertiesElement*)malloc
			(element_value*sizeof(struct stPropertiesElement));
	}

	realelement = (struct stRealElementComposition * *)malloc
		(inj_num*sizeof(struct stRealElementComposition));
	for (counter = 0; counter < inj_num; counter++) {
		element_value = num_i_IM[counter] * num_j;
		realelement[counter] = (struct stRealElementComposition*)malloc
			(element_value*sizeof(struct stRealElementComposition));
	}

	mixture_correction[0] = 0.319;
	mixture_correction[1] = 0.668;
	mixture_correction[2] = 1.;
	mixture_correction[3] = 1.397;
	mixture_correction[4] = 2.07;

	mf_burned = 0.;

	// inlet valve closing composition calculation

	mtotal_IVC = mairIVC;
	mEGR = mairIVC * rateBG;

	YO2IVC = YO2aire * (1 - rateBG * (mfuel * 1.e-3 + mfuel * 1e-3 * AFe) /
		(mairIVC * (1. - rateBG) + mfuel * 0.001));
	YCO2IVC = mmCO2 / mmfuel * mfuel * 0.001 * rateBG / (mfuel * 0.001 + mairIVC * (1 - rateBG));
	YH2OIVC = HC / 2. * mmH2O / mmfuel * mfuel * 0.001 * rateBG /
		(mfuel * 0.001 + mairIVC * (1 - rateBG));
	YN2IVC = 1 - YO2IVC - YCO2IVC - YH2OIVC;

	mO2_IVC = mairIVC * YO2IVC;
	mN2_IVC = mairIVC * YN2IVC;
	mCO2_IVC = mairIVC * YCO2IVC;
	mH2O_IVC = mairIVC * YH2OIVC;

	NOx_IVC = test_variables[14] * 1e-6 * 1.587 * mairIVC;
	mSOOT_IVC_A = test_variables[15] * 1e-6 * mairIVC;
	mSOOT_IVC_B = test_variables[15] * 1e-6 * mairIVC;
	mSOOT_IVC_C = test_variables[15] * 1e-6 * mairIVC;
	CO_IVC = 0;
	HC_IVC = 0;

	Cylinder_capacity = PI * Piston_D * Piston_D / 4. * S;
	VTDC = Cylinder_capacity / (GCRatio - 1.);
	f = mairIVC * 1000. / mfuel;

	// vectors initiate

	for (counter = 0; counter < size; counter++) {

		mtotal_bowl[counter] = mtotal_IVC;
		mO2_bowl[counter] = mO2_IVC;
		mN2_bowl[counter] = mN2_IVC;
		mCO2_bowl[counter] = mCO2_IVC;
		mH2O_bowl[counter] = mH2O_IVC;
		mHC_bowl[counter] = 0;
		Tsq_cyl[counter] = 0;
		Tadib_cyl[counter] = 0;
		mNOx_bowl[counter] = NOx_IVC;
		mCO_bowl[counter] = 0;
		HC_bowl[counter] = 0;
		mSOOT_bowl_A[counter] = mSOOT_IVC_A;
		mSOOT_bowl_B[counter] = mSOOT_IVC_B;
		mSOOT_bowl_C[counter] = mSOOT_IVC_C;
		HRF[counter] = 0;
		ROHR[counter] = 0;
		T_cyl[counter] = 0;
		acu_Mbb[counter] = 0;
		V_cyl[counter] = 0;
		Mbb[counter] = 0;
		Yair[counter] = 0;
		Yburned[counter] = 0;
		Yfuel[counter] = 0;
		U[counter] = 0;
		CV[counter] = 0;
		H_cooler[counter] = 0;
		H[counter] = 0;
		defor[counter] = 0;
		Rmixture[counter] = 0;
		roair[counter] = 0;
		p_cyl[counter] = 0;
		mtotal_Vc[counter] = 0;
		mO2_Vc[counter] = 0;
		mN2_Vc[counter] = 0;
		mCO2_Vc[counter] = 0;
		mH2O_Vc[counter] = 0;
		mNOx_Vc[counter] = 0;
		mSOOT_Vc_A[counter] = 0;
		mSOOT_Vc_B[counter] = 0;
		mHC_Vc[counter] = 0;
		dmtotal_Gfactor[counter] = 0;
		Qcyl[counter] = 0;
		Qcylhead[counter] = 0;
		Qpis[counter] = 0;
	}

	p_cyl[0] = 0;
	p_cyl[1] = 0;
	T_cyl[0] = temperatureIVC;
	T_cyl[1] = temperatureIVC;
	Mbb[0] = 0;
	Mbb[1] = 0;

	// element matrix initiate: m counter indicates injection number, i counter indicates element number, j counter indicates sub-element number

	for (m = 0; m < inj_num; m++) {
		for (i = 0; i < num_i_IM[m]; i++) {
			for (j = 0; j < num_j; j++) {

				aux = i * num_j + j;
				element[m][aux].state = stEvaporated;
				element[m][aux].mtotal = mfuel_ij_IM[m][i] * (mfuel * 1e-3);
				element[m][aux].mf_jet = mfuel_ij_IM[m][i] * (mfuel * 1e-3);
				element[m][aux].mf_reac = mfuel_ij_IM[m][i] * (mfuel * 1e-3);
				element[m][aux].mf_evap = mfuel_ij_IM[m][i] * (mfuel * 1e-3);

				element[m][aux].mO2 = 0;
				element[m][aux].mN2 = 0;
				element[m][aux].mCO2 = 0.;
				element[m][aux].mH2O = 0.;

				element[m][aux].dmtotal = 0;
				POC_IM[m][aux] = 8888;
				element[m][aux].RID = 0;
				element[m][aux].Rpmx = 0;
				element[m][aux].Rpmx_value = 0;
				element[m][aux].mNOx = 0;
				element[m][aux].dNOx = 0;
				element[m][aux].CO = 0;
				element[m][aux].HC = 0;
				element[m][aux].mSOOT_A = 0;
				element[m][aux].dSOOT_A = 0;
				element[m][aux].mSOOT_B = 0;
				element[m][aux].dSOOT_B = 0;
				element[m][aux].mSOOT_C = 0;
				element[m][aux].dSOOT_C = 0;
				element[m][aux].TSD = 0;
				element[m][aux].Tadib = 0;
				element[m][aux].Pcyl_POC = 1;
				element[m][aux].TNOx = 0;
				element[m][aux].X = 0;
				element[m][aux].soot_precursor = 0;
				element[m][aux].C = 1;
				element[m][aux].FI = 0;

				realelement[m][aux].mtotal = 0;
				realelement[m][aux].mO2 = 0;
				realelement[m][aux].mO = 0;
				realelement[m][aux].mCH = 0;
				realelement[m][aux].mN2 = 0;
				realelement[m][aux].mCO2 = 0;
				realelement[m][aux].mH2O = 0;
				realelement[m][aux].mf_reac = 0;

			}
		}
	}
	for (i = 0; i < 15; i++) {
		for (j = 0; j < size; j++) {
			elementcontrol[i][j].mfuel = 0;
			elementcontrol[i][j].mfuel_real = 0;
			elementcontrol[i][j].mtotal = 0;
			elementcontrol[i][j].mO2 = 0;
			elementcontrol[i][j].mO2_real = 0;
			elementcontrol[i][j].mO = 0;
			elementcontrol[i][j].mCO2 = 0;
			elementcontrol[i][j].mH2O = 0;
			elementcontrol[i][j].mCH = 0;
			elementcontrol[i][j].TSD = 0;
			elementcontrol[i][j].Tadib = 0;
			elementcontrol[i][j].dNOx = 0;
			elementcontrol[i][j].mNOx = 0;
			elementcontrol[i][j].mSOOT_A = 0;
			elementcontrol[i][j].mSOOT_B = 0;
			elementcontrol[i][j].mSOOT_C = 0;
			elementcontrol[i][j].X = 0;
			elementcontrol[i][j].FI = 0;

		}
	}

	/** *********TIME BUCLE BEGINING*********** */

	for (counter = 0; counter < size; counter++) {

		mf_burned = 0;

		// function call to calculate instantaneous variables with HRF
		CALCULATE_CYCLE(roair, CAD, delta_t, V_cyl, VTDC, counter, speed, p_cyl, HRF, acu_dmf, Mbb,
			acu_Mbb, AFe, f, mfuel * 1e-6, mEGR * 1e-3, mairIVC * 1e-3, T_cyl, HP, Yair, Yfuel,
			Yburned, U, CV, H_cooler, H, T_Evaporation_fuel, inj_fuel_temp, PRECISION_ITERATION,
			defor, Rmixture, Atmosphere_press, &Gamma, PI, Runiv, Piston_D, S, Crank_L,
			Connecting_Rod_L, E, Piston_Axis_D, Piston_Crown_H, DBowl, VBowl, M_Connecting_Rod,
			M_P_R_PA, MW_air, MW_fuel, MW_burned, C_ESteel, C_Mech_Defor, CTM, WC1A, WC1B, C2,
			C_MBLBY, Cbb, TPIS, TCYL_HEAD, TCYL, Qcylhead, Qcyl, Qpis);

		// mass exchange between  bowl and dead volumen with dmtotal_Gfactor quantity.

		if (counter > 0) {

			dmtotal_Gfactor[counter] = (Vc_factor - 1) *
				(exp(-6.9 * pow(CAD[counter] / 180, 2)) - exp(-6.9 * pow(CAD[counter - 1] / 180,
						2))) * (mtotal_bowl[counter - 1]);

			mtotal_bowl[counter] = mtotal_bowl[counter - 1] + dmtotal_Gfactor[counter];
			mtotal_Vc[counter] = mtotal_Vc[counter - 1] - dmtotal_Gfactor[counter];

			if (dmtotal_Gfactor[counter] < 0) {

				mO2_bowl[counter] = mO2_bowl[counter - 1] + dmtotal_Gfactor[counter] *
					(mO2_bowl[counter - 1] / mtotal_bowl[counter - 1]);
				mN2_bowl[counter] = mN2_bowl[counter - 1] + dmtotal_Gfactor[counter] *
					(mN2_bowl[counter - 1] / mtotal_bowl[counter - 1]);
				mCO2_bowl[counter] = mCO2_bowl[counter - 1] + dmtotal_Gfactor[counter] *
					(mCO2_bowl[counter - 1] / mtotal_bowl[counter - 1]);
				mH2O_bowl[counter] = mH2O_bowl[counter - 1] + dmtotal_Gfactor[counter] *
					(mH2O_bowl[counter - 1] / mtotal_bowl[counter - 1]);
				mNOx_bowl[counter] = mNOx_bowl[counter - 1] + dmtotal_Gfactor[counter] *
					(mNOx_bowl[counter - 1] / mtotal_bowl[counter - 1]);
				mSOOT_bowl_A[counter] = mSOOT_bowl_A[counter - 1] + dmtotal_Gfactor[counter] *
					(mSOOT_bowl_A[counter - 1] / mtotal_bowl[counter - 1]);
				mSOOT_bowl_B[counter] = mSOOT_bowl_B[counter - 1] + dmtotal_Gfactor[counter] *
					(mSOOT_bowl_B[counter - 1] / mtotal_bowl[counter - 1]);
				mSOOT_bowl_C[counter] = mSOOT_bowl_C[counter - 1] + dmtotal_Gfactor[counter] *
					(mSOOT_bowl_C[counter - 1] / mtotal_bowl[counter - 1]);

				mHC_bowl[counter] = mHC_bowl[counter - 1] + dmtotal_Gfactor[counter] *
					(mHC_bowl[counter - 1] / mtotal_bowl[counter - 1]);

				mO2_Vc[counter] = mO2_Vc[counter - 1] - dmtotal_Gfactor[counter] *
					(mO2_bowl[counter - 1] / mtotal_bowl[counter - 1]);
				mN2_Vc[counter] = mN2_Vc[counter - 1] - dmtotal_Gfactor[counter] *
					(mN2_bowl[counter - 1] / mtotal_bowl[counter - 1]);
				mCO2_Vc[counter] = mCO2_Vc[counter - 1] - dmtotal_Gfactor[counter] *
					(mCO2_bowl[counter - 1] / mtotal_bowl[counter - 1]);
				mH2O_Vc[counter] = mH2O_Vc[counter - 1] - dmtotal_Gfactor[counter] *
					(mH2O_bowl[counter - 1] / mtotal_bowl[counter - 1]);
				mNOx_Vc[counter] = mNOx_Vc[counter - 1] - dmtotal_Gfactor[counter] *
					(mNOx_bowl[counter - 1] / mtotal_bowl[counter - 1]);
				mSOOT_Vc_A[counter] = mSOOT_Vc_A[counter - 1] - dmtotal_Gfactor[counter] *
					(mSOOT_bowl_A[counter - 1] / mtotal_bowl[counter - 1]);
				mSOOT_Vc_B[counter] = mSOOT_Vc_B[counter - 1] - dmtotal_Gfactor[counter] *
					(mSOOT_bowl_B[counter - 1] / mtotal_bowl[counter - 1]);
				mSOOT_Vc_C[counter] = mSOOT_Vc_C[counter - 1] - dmtotal_Gfactor[counter] *
					(mSOOT_bowl_C[counter - 1] / mtotal_bowl[counter - 1]);

				mHC_Vc[counter] = mHC_Vc[counter - 1] - dmtotal_Gfactor[counter] *
					(mHC_bowl[counter - 1] / mtotal_bowl[counter - 1]);

			}
			else {

				mO2_bowl[counter] = mO2_bowl[counter - 1] + dmtotal_Gfactor[counter] *
					(mO2_Vc[counter - 1] / mtotal_Vc[counter - 1]);
				mN2_bowl[counter] = mN2_bowl[counter - 1] + dmtotal_Gfactor[counter] *
					(mN2_Vc[counter - 1] / mtotal_Vc[counter - 1]);
				mCO2_bowl[counter] = mCO2_bowl[counter - 1] + dmtotal_Gfactor[counter] *
					(mCO2_Vc[counter - 1] / mtotal_Vc[counter - 1]);
				mH2O_bowl[counter] = mH2O_bowl[counter - 1] + dmtotal_Gfactor[counter] *
					(mH2O_Vc[counter - 1] / mtotal_Vc[counter - 1]);
				mNOx_bowl[counter] = mNOx_bowl[counter - 1] + dmtotal_Gfactor[counter] *
					(mNOx_Vc[counter - 1] / mtotal_Vc[counter - 1]);
				mSOOT_bowl_A[counter] = mSOOT_bowl_A[counter - 1] + dmtotal_Gfactor[counter] *
					(mSOOT_Vc_A[counter - 1] / mtotal_Vc[counter - 1]);
				mSOOT_bowl_B[counter] = mSOOT_bowl_B[counter - 1] + dmtotal_Gfactor[counter] *
					(mSOOT_Vc_B[counter - 1] / mtotal_Vc[counter - 1]);
				mSOOT_bowl_C[counter] = mSOOT_bowl_C[counter - 1] + dmtotal_Gfactor[counter] *
					(mSOOT_Vc_C[counter - 1] / mtotal_Vc[counter - 1]);

				mHC_bowl[counter] = mHC_bowl[counter - 1] + dmtotal_Gfactor[counter] *
					(mHC_Vc[counter - 1] / mtotal_Vc[counter - 1]);

				mO2_Vc[counter] = mO2_Vc[counter - 1] - dmtotal_Gfactor[counter] *
					(mO2_Vc[counter - 1] / mtotal_Vc[counter - 1]);
				mN2_Vc[counter] = mN2_Vc[counter - 1] - dmtotal_Gfactor[counter] *
					(mN2_Vc[counter - 1] / mtotal_Vc[counter - 1]);
				mCO2_Vc[counter] = mCO2_Vc[counter - 1] - dmtotal_Gfactor[counter] *
					(mCO2_Vc[counter - 1] / mtotal_Vc[counter - 1]);
				mH2O_Vc[counter] = mH2O_Vc[counter - 1] - dmtotal_Gfactor[counter] *
					(mH2O_Vc[counter - 1] / mtotal_Vc[counter - 1]);
				mNOx_Vc[counter] = mNOx_Vc[counter - 1] - dmtotal_Gfactor[counter] *
					(mNOx_Vc[counter - 1] / mtotal_Vc[counter - 1]);
				mSOOT_Vc_A[counter] = mSOOT_Vc_A[counter - 1] - dmtotal_Gfactor[counter] *
					(mSOOT_Vc_A[counter - 1] / mtotal_Vc[counter - 1]);
				mSOOT_Vc_B[counter] = mSOOT_Vc_B[counter - 1] - dmtotal_Gfactor[counter] *
					(mSOOT_Vc_B[counter - 1] / mtotal_Vc[counter - 1]);
				mSOOT_Vc_C[counter] = mSOOT_Vc_C[counter - 1] - dmtotal_Gfactor[counter] *
					(mSOOT_Vc_C[counter - 1] / mtotal_Vc[counter - 1]);

				mHC_Vc[counter] = mHC_Vc[counter - 1] - dmtotal_Gfactor[counter] *
					(mHC_Vc[counter - 1] / mtotal_Vc[counter - 1]);
			}
		} // counter>0

		// unburned in-cylinder temperrature calculation and adiabatic in-cylinder temperature calculation

		if (time_vector[counter] < (SOC_IM[0] / (6 * speed))) {
			Tsq_cyl[counter] = T_cyl[counter];
		}
		else {
			if(counter == 0){
				printf("Stop\n");
			}else{
			Tsq_cyl[counter] = Tsq_cyl[counter - 1] * pow((p_cyl[counter] / p_cyl[counter - 1]),
				((Gamma - 1) / Gamma));
			}
		}
		if(YO2IVC<0) YO2IVC = 0;
		Tadib_cyl[counter] = Tsq_cyl[counter] + 7424.7 * pow(YO2IVC, 0.829961) * pow
			((p_cyl[counter] * (1e-5)), -0.0236771);

		// lift off calculation
		XLO[counter] = 0;

		// mixture model begin if the injection have begun

		if (time_vector[counter] >= SOI_IM[0] * 60. / (360. * speed)) {

			// m determines the injection number; i determines the element number and j determines the sub-element number (the trajectorie)

			for (m = 0; m < inj_num; m++) {
				for (i = 0; i < num_i_IM[m]; i++) {
					for (j = 0; j < num_j; j++) {

						// aux converts the element and the sub-element position into only one position

						aux = i * num_j + j;

						if ((POI_IM[m][i] <= time_vector[counter]) && (element[m][aux].mf_jet > 0)
							&& (mO2_bowl[counter] > 0) && (element[m][aux].state != stOvermixed)) {

							// mixture process and air mass include into element

							// element[m][aux].dmtotal=Kmixture1*(pow(element[m][aux].mf_jet,2)/element[m][aux].mtotal)*virtual_velocity[counter]*pow(roair[counter],0.5)*pow(nozzle_d*1000,-1)*pow((mO2_bowl[counter]/mtotal_bowl[counter]),Kmixture2)*(1/mixture_correction[j])*delta_t;
							if (mO2_bowl[counter] / mtotal_bowl[counter] < 0.049552)
								element[m][aux].dmtotal = Kmixture1 *
									(pow(element[m][aux].mf_jet, 2) / element[m][aux].mtotal)
									* virtual_velocity[counter] * pow(roair[counter], 0.5) * pow
									(nozzle_d * 1000, -1) * pow(0.23, Kmixture2) * 10. *
									(1 / mixture_correction[j]) * delta_t; // Factor limitado a 10
							else
								element[m][aux].dmtotal = Kmixture1 *
									(pow(element[m][aux].mf_jet, 2) / element[m][aux].mtotal)
									* virtual_velocity[counter] * pow(roair[counter], 0.5) * pow
									(nozzle_d * 1000, -1) * pow
									((mO2_bowl[counter] / mtotal_bowl[counter]), Kmixture2) *
									(1 / mixture_correction[j]) * delta_t; // Como siempre

							element[m][aux].mtotal = element[m][aux].mtotal + element[m][aux]
								.dmtotal;
							element[m][aux].mO2 = element[m][aux].mO2 + element[m][aux].dmtotal *
								(mO2_bowl[counter] / mtotal_bowl[counter]);
							element[m][aux].mCO2 = element[m][aux].mCO2 + element[m][aux].dmtotal *
								(mCO2_bowl[counter] / mtotal_bowl[counter]);
							element[m][aux].mH2O = element[m][aux].mH2O + element[m][aux].dmtotal *
								(mH2O_bowl[counter] / mtotal_bowl[counter]);
							element[m][aux].mN2 = element[m][aux].mN2 + element[m][aux].dmtotal *
								(mN2_bowl[counter] / mtotal_bowl[counter]);
							element[m][aux].mf_reac = element[m][aux].mf_reac + element[m][aux]
								.dmtotal * (mHC_bowl[counter] / mtotal_bowl[counter]);
							element[m][aux].mf_evap = element[m][aux].mf_evap + element[m][aux]
								.dmtotal * (mHC_bowl[counter] / mtotal_bowl[counter]);

							// element parameters to determinate combustion process

							element[m][aux].FI = element[m][aux].mf_reac * Kst1 / element[m][aux]
								.mO2;
							element[m][aux].C = element[m][aux].mf_reac / element[m][aux].mtotal;

							// element penetration length

							element[m][aux].X = 0.0001;

							// NOx mass includes into element

							element[m][aux].mNOx = element[m][aux].mNOx + element[m][aux].dmtotal *
								(mNOx_bowl[counter] / mtotal_bowl[counter]);

							// SOOT mass of the IVC is included into element and it is oxidated totally is it is inside the flame

							element[m][aux].dSOOT_A = element[m][aux].dmtotal *
								(mSOOT_bowl_A[counter] / mtotal_bowl[counter]);
							element[m][aux].dSOOT_B = element[m][aux].dmtotal *
								(mSOOT_bowl_B[counter] / mtotal_bowl[counter]);
							element[m][aux].dSOOT_C = element[m][aux].dmtotal *
								(mSOOT_bowl_C[counter] / mtotal_bowl[counter]);

							// soot model A
							if ((time_vector[counter] < (SOC_IM[m] / (6 * speed))) &&
								(element[m][aux].FI > 1)) {
								element[m][aux].mSOOT_A = element[m][aux].mSOOT_A + element[m][aux]
									.dSOOT_A;
								mSOOT_bowl_A[counter] = mSOOT_bowl_A[counter] - element[m][aux]
									.dSOOT_A;
							}
							if ((time_vector[counter] > (SOC_IM[m] / (6 * speed))) &&
								(element[m][aux].FI > 1)) {
								mSOOT_bowl_A[counter] = mSOOT_bowl_A[counter] - element[m][aux]
									.dSOOT_A;
							}

							// soot model B

							if ((time_vector[counter] < (SOC_IM[m] / (6 * speed))) &&
								(element[m][aux].FI > 1)) {
								element[m][aux].mSOOT_B = element[m][aux].mSOOT_B + element[m][aux]
									.dSOOT_B;
								mSOOT_bowl_B[counter] = mSOOT_bowl_B[counter] - element[m][aux]
									.dSOOT_B;
							}
							if ((time_vector[counter] > (SOC_IM[m] / (6 * speed))) &&
								(element[m][aux].FI > 1)) {
								mSOOT_bowl_B[counter] = mSOOT_bowl_B[counter] - element[m][aux]
									.dSOOT_B;
							}

							// soot model C

							if ((time_vector[counter] < (SOC_IM[m] / (6 * speed))) &&
								(element[m][aux].FI > 1)) {
								element[m][aux].mSOOT_C = element[m][aux].mSOOT_C + element[m][aux]
									.dSOOT_C;
								mSOOT_bowl_C[counter] = mSOOT_bowl_C[counter] - element[m][aux]
									.dSOOT_C;
							}
							if ((time_vector[counter] > (SOC_IM[m] / (6 * speed))) &&
								(element[m][aux].FI > 1)) {
								mSOOT_bowl_C[counter] = mSOOT_bowl_C[counter] - element[m][aux]
									.dSOOT_C;
							}

							// ignition delay determination

							if ((element[m][aux].state == stEvaporated) && (SOC_IM[m] == EVO)) {

								Y1 = -KID3 * pow(element[m][aux].mf_reac / element[m][aux].mtotal,
									KID4) * pow(element[m][aux].mO2 / element[m][aux].mtotal, KID5);

								if (element[m][aux].mf_reac / element[m][aux].mtotal == 1) {
									T1 = 1000 / 353;
								}
								else {
									T1 = 1000 /
										(T_cyl[counter] *
										(1 - element[m][aux].mf_reac / element[m][aux].mtotal));
								}

								if (T1 >= 2) {
									RID1 = Y1 + 0.155 * T1 - 0.2175;
								}
								else if (((T1 - 0.35) > 0.8) && (T1 < 2)) {
									RID1 = (Y1 + pow(exp((T1 - 0.35) / 5), 7) - 3.1);
									element[m][aux].RID = element[m][aux].RID + KID1 * pow
										(p_cyl[counter] * 1e-5, KID2) * pow(10, -RID1) * delta_t;
								}
								else {
									RID1 = (Y1 + pow(log(T1 - 0.35), 3));
									element[m][aux].RID = element[m][aux].RID + KID1 * pow
										(p_cyl[counter] * 1e-5, KID2) * pow(10, -RID1) * delta_t;
								}
							}

							if ((element[m][aux].RID >= 1) && (SOC_IM[m] == EVO)) {
								SOC_IM[m] = time_vector[counter] * 6 * speed;
							}

							/** ****************COMBUSTION****************************** */

							// PREMIX COMBUSTION---------------------------------------------------------------

							if ((element[m][aux].state == stEvaporated) &&
								(time_vector[counter] > (SOC_IM[m] / (6 * speed)))) {

								element[m][aux].Rpmx = element[m][aux].Rpmx + Kpmx1 * pow
									((mO2_bowl[counter] / mtotal_bowl[counter]), Kpmx3) * pow
									(fabs(log(element[m][aux].FI)), Kpmx4) * exp
									(-Kpmx2 / T_cyl[counter]) * pow(roair[counter], Kpmx5)
									* delta_t;
								element[m][aux].Rpmx_value = Kpmx1 * pow
									((mO2_bowl[counter] / mtotal_bowl[counter]), Kpmx3) * pow
									(fabs(log(element[m][aux].FI)), Kpmx4) * exp
									(-Kpmx2 / T_cyl[counter]) * pow(roair[counter], Kpmx5)
									* delta_t;
							}

							if ((element[m][aux].state == stEvaporated) &&
								(POI_IM[m][i] < SOC_IM[m] / (6 * speed)) &&
								(time_vector[counter] > (SOC_IM[m] / (6 * speed)))) {
								if (element[m][aux].Rpmx >= 1) {

									// RICH PREMIX

									if (element[m][aux].FI > 1) {
										element[m][aux].state = stBurned_rich_premix;
										POC_IM[m][aux] = time_vector[counter];

										FUNCTION_SOOT_C(&soot_pre, element[m][aux].FI);
										element[m][aux].soot_precursor = soot_pre;

									}
									else { // POOR PREMIX
										element[m][aux].state = stBurned_poor_premix;
										POC_IM[m][aux] = time_vector[counter];
										element[m][aux].Pcyl_POC = p_cyl[counter];
									} // poor pmx
								}
							} // pmx combustion

							// FIRST DIFFUSION
							if ((element[m][aux].state == stEvaporated) &&
								(POI_IM[m][i] >= SOC_IM[m] / (6 * speed)) &&
								(element[m][aux].X > XLO[counter])) {
								element[m][aux].state = stInto_diffusion_flame;

								FUNCTION_SOOT_C(&soot_pre, element[m][aux].FI);
								element[m][aux].soot_precursor = soot_pre;
							}

							if ((element[m][aux].state == stInto_diffusion_flame) &&
								(POI_IM[m][i] >= SOC_IM[m] / (6 * speed)) &&
								(time_vector[counter] > (SOC_IM[m] / (6 * speed)))) {

								if (element[m][aux].FI <= 1) {
									POC_IM[m][aux] = time_vector[counter];
									element[m][aux].Pcyl_POC = p_cyl[counter];
									element[m][aux].state = stBurned_by_diffusion;
								}

							}

							// SECOND DIFFUSION

							if ((element[m][aux].state == stBurned_rich_premix)) {

								if (element[m][aux].FI <= 1) {
									element[m][aux].state = stBurned_by_second_diffusion;
									POC_IM[m][aux] = time_vector[counter];
									element[m][aux].Pcyl_POC = p_cyl[counter];
								}

							}

							// real composition mass determination

							if (element[m][aux].state == stEvaporated) {
								realelement[m][aux].mO2 = element[m][aux].mO2;
								realelement[m][aux].mf_reac = element[m][aux].mf_reac;
							}

							if ((element[m][aux].state == stBurned_rich_premix) ||
								(element[m][aux].state == stInto_diffusion_flame)) {
								realelement[m][aux].mO2 = 0;
								realelement[m][aux].mf_reac = element[m][aux].mf_reac *
									(1 - 1 / element[m][aux].FI);
							}

							if ((element[m][aux].state == stBurned_poor_premix) ||
								(element[m][aux].state == stBurned_by_diffusion) ||
								(element[m][aux].state == stBurned_by_second_diffusion)) {
								realelement[m][aux].mO2 = element[m][aux].mO2 - element[m][aux]
									.mf_reac * Kst1;
								realelement[m][aux].mf_reac = 0;
							}

							// element temperature calculation

							element[m][aux].TSD =
								(1 / (element[m][aux].mtotal - realelement[m][aux]
									.mf_reac + 2 * realelement[m][aux].mf_reac)) *
								((element[m][aux].mtotal - realelement[m][aux].mf_reac) *
								(T_cyl[counter] + 37630.5 *
									(element[m][aux].mf_reac - realelement[m][aux].mf_reac)
									/ element[m][aux].mtotal) + inj_fuel_temp *
								(2 * realelement[m][aux].mf_reac));
							element[m][aux].TNOx = element[m][aux].TSD;

							if (element[m][aux].FI < 1) {
								element[m][aux].TSD = element[m][aux].TSD * pow
									((p_cyl[counter] / element[m][aux].Pcyl_POC),
									((Gamma - 1) / Gamma));
								element[m][aux].TNOx = element[m][aux].TNOx * pow
									((p_cyl[counter] / element[m][aux].Pcyl_POC),
									((Gamma * KNOx1 - 1) / Gamma * KNOx1));
							}

							if (element[m][aux].TSD < 2600) {
								element[m][aux].Tadib = element[m][aux].TSD - 1.5538 * 1e-7 * pow
									(element[m][aux].TSD, 2.6774);
							}
							else {
								element[m][aux].Tadib = element[m][aux].TSD - 7.136 * 1e-10 * pow
									(element[m][aux].TSD, 3.3596);
							}

							// EMISSIONS FORMATION AND OXIDATIONS

							// NOX emissions

							if ((element[m][aux].state == stBurned_by_diffusion) ||
								(element[m][aux].state == stBurned_by_second_diffusion) ||
								(element[m][aux].state == stBurned_poor_premix)) {

								if ((time_vector[counter] == POC_IM[m][aux]) &&
									(element[m][aux].state != stBurned_poor_premix)) {
									mNOx_bowl[counter] = mNOx_bowl[counter] - element[m][aux]
										.mNOx * EC;
									element[m][aux].mNOx = element[m][aux].mNOx - element[m][aux]
										.mNOx * EC;
								}

								FUNCTION_NOX(&YNOeq_value, &KdYNO_value, YNOeq, KdYNO,
									element[m][aux].TNOx, realelement[m][aux].mO2,
									element[m][aux].mtotal);

								if (YNOeq_value == 0) {
									YNOeq_value = 1e-6;
								}

								if
									(YNOeq_value - element[m][aux].mNOx / element[m][aux].mtotal > 0) {
									element[m][aux].dNOx = (KNOx2)
										* element[m][aux].mtotal * KdYNO_value *
										(YNOeq_value - element[m][aux].mNOx / element[m][aux]
										.mtotal) / YNOeq_value * delta_t;
								}
								else {
									element[m][aux].dNOx = 0;
								}

								element[m][aux].mNOx = element[m][aux].mNOx + element[m][aux].dNOx;
								mNOx_bowl[counter] = mNOx_bowl[counter] + element[m][aux].dNOx;

							}

							// SOOT formation and soot oxidation
							// SOOT model A: HIROYASU
							// Hiroyasu formation  process

							element[m][aux].dSOOT_A = KSOOTA1 * realelement[m][aux].mf_reac * pow
								(p_cyl[counter], KSOOTA2) * exp(-KSOOTA3 / element[m][aux].Tadib)
								* delta_t;
							element[m][aux].mSOOT_A = element[m][aux].mSOOT_A + element[m][aux]
								.dSOOT_A;

							// Hiroyasu oxidation process

							element[m][aux].dSOOT_A = KSOOTA4 * element[m][aux].mSOOT_A * pow
								((realelement[m][aux].mO2 / element[m][aux].mtotal), KSOOTA5) * pow
								(p_cyl[counter], KSOOTA6) * exp(-KSOOTA7 / element[m][aux].Tadib)
								* delta_t;
							if (element[m][aux].dSOOT_A <= element[m][aux].mSOOT_A) {
								element[m][aux].mSOOT_A = element[m][aux].mSOOT_A - element[m][aux]
									.dSOOT_A;
							}
							else {
								element[m][aux].mSOOT_A = 0;
							}

							// SOOT model C

							// Hiroyasu formation  process

							if ((element[m][aux].state == stBurned_rich_premix) ||
								(element[m][aux].state == stInto_diffusion_flame)) {

								element[m][aux].dSOOT_C = KSOOTC1 *
									(realelement[m][aux].mf_reac * element[m][aux].soot_precursor)
									* pow(p_cyl[counter], KSOOTC2) * exp
									(-KSOOTC3 / element[m][aux].Tadib) * delta_t;
								element[m][aux].mSOOT_C = element[m][aux].mSOOT_C + element[m][aux]
									.dSOOT_C;
							}

							// Hiroyasu oxidation process

							element[m][aux].dSOOT_C = KSOOTC4 * element[m][aux].mSOOT_C * pow
								((realelement[m][aux].mO2 / element[m][aux].mtotal), KSOOTC5) * pow
								(p_cyl[counter], KSOOTC6) * exp(-KSOOTC7 / element[m][aux].Tadib)
								* delta_t;
							if (element[m][aux].dSOOT_C <= element[m][aux].mSOOT_C) {
								element[m][aux].mSOOT_C = element[m][aux].mSOOT_C - element[m][aux]
									.dSOOT_C;
							}
							else {
								element[m][aux].mSOOT_C = 0;
							}

							// HC formation by overmixed element

							if ((SOC_IM[m] * 60. / (360. * speed) < time_vector[counter]) &&
								(POI_IM[m][i] < SOC_IM[m] * 60. / (360. * speed)) &&
								(element[m][aux].state == stEvaporated)) {
								if (element[m][aux].FI < 0.5) {

									mHC_bowl[counter] = mHC_bowl[counter] + element[m][aux].mf_reac;
									element[m][aux].state = stOvermixed;
									realelement[m][aux].mf_reac = 0;
									element[m][aux].mf_reac = 0;
									element[m][aux].mf_jet = 0;

								}
							}
						} // if  time>POI
					} // for j

					// element control assignation
					for (i_aux = 0; i_aux < 5; i_aux++) {
						j_aux = i_aux * 3;

						if ((elementcontrol[j_aux][0].inj_number == m) &&
							(elementcontrol[j_aux][0].num_i == i)) {
							aux = i * num_j + 0;
							j_aux = i_aux * 3;
							elementcontrol[j_aux][counter].mtotal = element[m][aux].mtotal;
							elementcontrol[j_aux][counter].mfuel = element[m][aux].mf_reac;
							elementcontrol[j_aux][counter].mfuel_real = realelement[m][aux].mf_reac;
							elementcontrol[j_aux][counter].mO2 = element[m][aux].mO2;
							elementcontrol[j_aux][counter].mO2_real = realelement[m][aux].mO2;
							elementcontrol[j_aux][counter].TSD = element[m][aux].TSD;
							elementcontrol[j_aux][counter].Tadib = element[m][aux].Tadib;
							elementcontrol[j_aux][counter].X = element[m][aux].X;
							elementcontrol[j_aux][counter].FI = element[m][aux].FI;

							aux = i * num_j + 2;
							j_aux = i_aux * 3 + 1;
							elementcontrol[j_aux][counter].mtotal = element[m][aux].mtotal;
							elementcontrol[j_aux][counter].mfuel = element[m][aux].mf_reac;
							elementcontrol[j_aux][counter].mfuel_real = realelement[m][aux].mf_reac;
							elementcontrol[j_aux][counter].mO2 = element[m][aux].mO2;
							elementcontrol[j_aux][counter].mO2_real = realelement[m][aux].mO2;
							elementcontrol[j_aux][counter].TSD = element[m][aux].TSD;
							elementcontrol[j_aux][counter].Tadib = element[m][aux].Tadib;
							elementcontrol[j_aux][counter].X = element[m][aux].X;
							elementcontrol[j_aux][counter].FI = element[m][aux].FI;

							aux = i * num_j + 4;
							j_aux = i_aux * 3 + 2;
							elementcontrol[j_aux][counter].mtotal = element[m][aux].mtotal;
							elementcontrol[j_aux][counter].mfuel = element[m][aux].mf_reac;
							elementcontrol[j_aux][counter].mfuel_real = realelement[m][aux].mf_reac;
							elementcontrol[j_aux][counter].mO2 = element[m][aux].mO2;
							elementcontrol[j_aux][counter].mO2_real = realelement[m][aux].mO2;
							elementcontrol[j_aux][counter].TSD = element[m][aux].TSD;
							elementcontrol[j_aux][counter].Tadib = element[m][aux].Tadib;
							elementcontrol[j_aux][counter].X = element[m][aux].X;
							elementcontrol[j_aux][counter].FI = element[m][aux].FI;

						}
					}

				} // for i
			} // for m

			for (m = 0; m < inj_num; m++) {
				for (i = 0; i < num_i_IM[m]; i++) {
					for (j = 0; j < num_j; j++) {
						if (POI_IM[m][i] <= time_vector[counter]) {
							aux = i * num_j + j;

							if ((element[m][aux].state == stBurned_rich_premix) ||
								(element[m][aux].state == stBurned_poor_premix) ||
								(element[m][aux].state == stBurned_by_diffusion) ||
								(element[m][aux].state == stBurned_by_second_diffusion) ||
								(element[m][aux].state == stInto_diffusion_flame)) {
								mf_burned = mf_burned +
									(element[m][aux].mf_reac - realelement[m][aux].mf_reac);
							}

						}
					}
				}
			}

		} // if time>SOI

		HRF[counter] = mf_burned / (mfuel * 1e-3);

		mO2_bowl[counter] = mO2_bowl[counter] - (HRF[counter] - HRF[counter - 1])
			* mfuel * 1e-3 * Kst1;
		mCO2_bowl[counter] = mCO2_bowl[counter] + (HRF[counter] - HRF[counter - 1])
			* mfuel * 1e-3 * Kst5;
		mH2O_bowl[counter] = mH2O_bowl[counter] + (HRF[counter] - HRF[counter - 1])
			* mfuel * 1e-3 * Kst6;

	} // for counter

	mSOOT_bowl_A_i_burned = mSOOT_bowl_A[size - 1];
	mSOOT_bowl_B_i_burned = mSOOT_bowl_B[size - 1];
	mSOOT_bowl_C_i_burned = mSOOT_bowl_C[size - 1];

	for (m = 0; m < inj_num; m++) {
		for (i = 0; i < num_i_IM[m]; i++) {
			for (j = 0; j < num_j; j++) {
				aux = i * num_j + j;
				mSOOT_bowl_A[size - 1] = mSOOT_bowl_A[size - 1] + element[m][aux].mSOOT_A;
				mSOOT_bowl_B[size - 1] = mSOOT_bowl_B[size - 1] + element[m][aux].mSOOT_B;
				mSOOT_bowl_C[size - 1] = mSOOT_bowl_C[size - 1] + element[m][aux].mSOOT_C;

				if (element[m][aux].FI <= 1) {
					mSOOT_bowl_A_i_burned = mSOOT_bowl_A_i_burned + element[m][aux].mSOOT_A;
					mSOOT_bowl_B_i_burned = mSOOT_bowl_B_i_burned + element[m][aux].mSOOT_B;
					mSOOT_bowl_C_i_burned = mSOOT_bowl_C_i_burned + element[m][aux].mSOOT_C;
				}
			}
		}
	}

	vector_to_interpolate = (double*)malloc(size*sizeof(double));
	vector_to_interpolate[0] = 0;
	for (counter = 1; counter < size; counter++) {
		vector_to_interpolate[counter] = (HRF[counter] - HRF[counter - 1]) * (mfuel * 1e-6) * HP /
			(time_vector[counter] - time_vector[counter - 1]);
	}

	ROHR[0] = 0;
	ROHR[size - 1] = vector_to_interpolate[size - 1];
	for (counter = 1; counter < size - 1; counter++) {
		ROHR[counter] = (vector_to_interpolate[counter - 1] + vector_to_interpolate[counter]
			+ vector_to_interpolate[counter + 1]) / 3;
	}
	free(vector_to_interpolate);

	complete_size = 0;
	complete_prev_size = 0;
	complete_post_size = 0;
	counter_CAD_1 = IVC;
	counter_CAD_2 = EVO;

	do {
		complete_prev_size = complete_prev_size + 1;
		counter_CAD_1 = counter_CAD_1 - delta_t * 360 * speed / 60.;
	}
	while (counter_CAD_1 >= -180);
	complete_prev_size--;
	do {
		complete_post_size = complete_post_size + 1;
		counter_CAD_2 = counter_CAD_2 + delta_t * 360 * speed / 60.;
	}
	while (counter_CAD_2 <= 180);
	complete_post_size--;

	complete_size = size + complete_prev_size + complete_post_size;

	complete_p_cyl = (double*)malloc(complete_size*sizeof(double));
	complete_V_cyl = (double*)malloc(complete_size*sizeof(double));
	complete_deform = (double*)malloc(complete_size*sizeof(double));
	complete_CAD = (double*)malloc(complete_size*sizeof(double));

	CALCULUS_OF_IMP_HP(complete_p_cyl, complete_CAD, p_cyl, V_cyl, complete_V_cyl, complete_deform,
		&WI_HP, &IMP_HP, complete_size, complete_prev_size, delta_t, speed, size, IVC, EVO, VTDC,
		Cylinder_capacity, PI, Piston_D, S, Crank_L, Connecting_Rod_L, E, Piston_Axis_D,
		Piston_Crown_H, M_Connecting_Rod, M_P_R_PA, C_ESteel, C_Mech_Defor, inlet_pres,
		exhaust_pres);

	CALCULUS_OF_MEAN_VARIABLES(p_cyl, T_cyl, dp_da_cyl, CAD, &pmax, &Tmax, &dp_da_max, &p_exit,
		&T_exit, size);

	// indicated average power and indicated efficiency
	mean_var_exit[0] = IMP_HP * 1e-5;
	mean_var_exit[1] = IMP_HP * Cylinder_capacity / (mfuel * 1e-6 * HP);

	// mean variables in.cylinder laws results
	mean_var_exit[2] = pmax * 1e-5;
	mean_var_exit[3] = Tmax;
	mean_var_exit[4] = p_exit * 1e-5;
	mean_var_exit[5] = T_exit;
	mean_var_exit[6] = dp_da_max * 1e-5;

	// NOx concentration exit
	mean_var_exit[7] = mNOx_bowl[size - 3] * 1e6 / (1.587 * (mairIVC + mfuel * 1e-3));

	mean_var_exit[8] = mO2_bowl[size - 3] / (mairIVC + mfuel * 1e-3);

	SOOT_EVO_A = mSOOT_bowl_A[size - 1] * 1e6 / (mairIVC + mfuel * 1e-3);

	// Species concentration at EVO
	species_EVO_exit[0] = mN2_bowl[size - 3] / (mairIVC + mfuel * 1.e-3);
	species_EVO_exit[1] = mO2_bowl[size - 3] / (mairIVC + mfuel * 1.e-3);
	species_EVO_exit[2] = mCO2_bowl[size - 3] / (mairIVC + mfuel * 1.e-3);
	species_EVO_exit[3] = mH2O_bowl[size - 3] / (mairIVC + mfuel * 1.e-3);
	species_EVO_exit[4] = mNOx_bowl[size - 3] / (mairIVC + mfuel * 1.e-3) / 1.587;
	species_EVO_exit[5] = mCO_bowl[size - 3] / (mairIVC + mfuel * 1.e-3);
	species_EVO_exit[6] = mSOOT_bowl_A[size - 1] / (mairIVC + mfuel * 1.e-3);
	species_EVO_exit[7] = mHC_bowl[size - 3] / (mairIVC + mfuel * 1.e-3);

	mean_var_exit[9] = 2;
	RES_FSN = 2.;
	for (i = 0; i < 6; i++) {
		mean_var_exit[9] = mean_var_exit[9] / RES_FSN;
		if ((SOOT_EVO_A > 0) && (SOOT_EVO_A < 3000)) {
			RES_FSN = (1 / SOOT_EVO_A) * (1 / 1.2) * (1 / 0.405) * 4.95 * mean_var_exit[9] * exp
				(0.38 * mean_var_exit[9]);
		}
		else if (SOOT_EVO_A == 0) {
			mean_var_exit[9] = 0;
			RES_FSN = 1;
		}
		else {
			mean_var_exit[9] = 10;
			RES_FSN = 1;
		}
	}

	mean_var_exit[9] = mean_var_exit[9] * 4;
	/*
	SOOT_EVO_C=mSOOT_bowl_C[size-1]*1e6/(mairIVC+mfuel*1e-3);


	mean_var_exit[10]=2;
	RES_FSN=2.;
	for(i=0;i<6;i++){
	mean_var_exit[10]=mean_var_exit[10]/RES_FSN;
	if((SOOT_EVO_C>0)&&(SOOT_EVO_C<3000)){
	RES_FSN=(1/SOOT_EVO_C)*(1/1.2)*(1/0.405)*4.95*mean_var_exit[10]*exp(0.38*mean_var_exit[10]);
	}else if(SOOT_EVO_C==0){
	mean_var_exit[10]=0;
	RES_FSN=1;
	}else{
	mean_var_exit[10]=10;
	RES_FSN=1;
	}
	}

	mean_var_exit[10]=mean_var_exit[10];
	 */

	heat_transfer[0] = 0;
	heat_transfer[1] = 0;
	heat_transfer[2] = 0;
	heat_transfer[3] = 0;

	for (counter = 0; counter < size - 1; counter++) {
		heat_transfer[0] = heat_transfer[0] + min(H_cooler[counter], H_cooler[counter + 1]) + pow
			(pow((H_cooler[counter + 1] - H_cooler[counter]) / 2, 2), 0.5);
		heat_transfer[1] = heat_transfer[1] + min(Qcylhead[counter], Qcylhead[counter + 1]) + pow
			(pow((Qcylhead[counter + 1] - Qcylhead[counter]) / 2, 2), 0.5);
		heat_transfer[2] = heat_transfer[2] + min(Qcyl[counter], Qcyl[counter + 1]) + pow
			(pow((Qcyl[counter + 1] - Qcyl[counter]) / 2, 2), 0.5);
		heat_transfer[3] = heat_transfer[3] + min(Qpis[counter], Qpis[counter + 1]) + pow
			(pow((Qpis[counter + 1] - Qpis[counter]) / 2, 2), 0.5);
	}

	// instantaneous law cycle control
	/*
	if((fich=fopen("c:\\ACT_model\\output\\seguimiento_paq_50_3.dat","w"))==NULL){
	printf("The element file results could not be opened");
	exit(1);
	}
	strcpy(title,"CAD[�],time[s],mtotal,mfuel,mfuel_real,mO2,mO2_real,TSD,Tadib");
	fprintf(fich, "%s", title);

	for(counter=0;counter<size;counter++){
	fprintf(fich, "\n%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",CAD[counter],time_vector[counter],elementcontrol[7][counter].mtotal,elementcontrol[7][counter].mfuel,elementcontrol[7][counter].mfuel_real,elementcontrol[7][counter].mO2,elementcontrol[7][counter].mO2_real,elementcontrol[7][counter].TSD,elementcontrol[7][counter].Tadib);
	}

	fclose(fich);


	if((fich=fopen("c:\\ACT_model\\output\\Instantaneous_law.dat","w"))==NULL){
	printf("The element file results could not be opened");
	exit(1);
	}
	strcpy(title,"CAD[�],time[s],Pcyl[bar],dpdacyl[bar/�],HR[J],ROHR[J/s],Tcyl[K],Vcyl[m3],ro_air[kg/m3],uo*[m/s],mtotal_cyl[g],injection[kg/s],accu_mfuel[kg],mO2bowl[g],mO2Vd[g],YO2[-],Tsq[K],Tadib_cyl[K],mNOx[g],XLO[m]");
	fprintf(fich, "%s", title);

	for(counter=0;counter<size;counter++){

	fprintf(fich, "\n%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",CAD[counter],time_vector[counter],(p_cyl[counter]*1e-5),dp_da_cyl[counter],(HRF[counter]*mfuel*42.92),ROHR[counter],\
	T_cyl[counter],V_cyl[counter],roair[counter],virtual_velocity[counter],(mtotal_bowl[counter]+mtotal_Vc[counter]),dmf[counter],(acu_dmf[counter]*mfuel*42.92),mO2_bowl[counter],mO2_Vc[counter],((mO2_bowl[counter]+mO2_Vc[counter])/(mtotal_bowl[counter]+mtotal_Vc[counter])),Tsq_cyl[counter],Tadib_cyl[counter],\
	(mNOx_bowl[counter]+mNOx_Vc[counter]),XLO[counter]);
	}
	fclose(fich);


	if((fich=fopen("c:\\ACT_model\\output\\mtotal.dat","w"))==NULL){
	printf("The element file results could not be opened");
	exit(1);
	}
	strcpy(title,"CAD[�],time[s],paq_10_1,paq_10_3,paq_10_5,paq_30_1,paq_30_3,paq_30_5,paq_50_1,paq_50_3,paq_50_5,paq_70_1,paq_70_3,paq_70_5,paq_90_1,paq_90_3,paq_90_5");
	fprintf(fich, "%s", title);

	for(counter=0;counter<size;counter++){
	fprintf(fich, "\n%lf,%lf,",CAD[counter],time_vector[counter]);
	for(i_aux=0;i_aux<15;i_aux++){
	fprintf(fich, "%lf,",elementcontrol[i_aux][counter].mtotal);
	}
	}
	fclose(fich);

	if((fich=fopen("c:\\ACT_model\\output\\mO2.dat","w"))==NULL){
	printf("The element file results could not be opened");
	exit(1);
	}
	strcpy(title,"CAD[�],time[s],paq_10_1,paq_10_3,paq_10_5,paq_30_1,paq_30_3,paq_30_5,paq_50_1,paq_50_3,paq_50_5,paq_70_1,paq_70_3,paq_70_5,paq_90_1,paq_90_3,paq_90_5");
	fprintf(fich, "%s", title);

	for(counter=0;counter<size;counter++){
	fprintf(fich, "\n%lf,%lf,",CAD[counter],time_vector[counter]);
	for(i_aux=0;i_aux<15;i_aux++){
	fprintf(fich, "%lf,",elementcontrol[i_aux][counter].mO2);
	}
	}
	fclose(fich);

	if((fich=fopen("c:\\ACT_model\\output\\mO2_real.dat","w"))==NULL){
	printf("The element file results could not be opened");
	exit(1);
	}
	strcpy(title,"CAD[�],time[s],paq_10_1,paq_10_3,paq_10_5,paq_30_1,paq_30_3,paq_30_5,paq_50_1,paq_50_3,paq_50_5,paq_70_1,paq_70_3,paq_70_5,paq_90_1,paq_90_3,paq_90_5");
	fprintf(fich, "%s", title);

	for(counter=0;counter<size;counter++){
	fprintf(fich, "\n%lf,%lf,",CAD[counter],time_vector[counter]);
	for(i_aux=0;i_aux<15;i_aux++){
	fprintf(fich, "%lf,",elementcontrol[i_aux][counter].mO2_real);
	}
	}
	fclose(fich);



	if((fich=fopen("c:\\ACT_model\\output\\mfuel.dat","w"))==NULL){
	printf("The element file results could not be opened");
	exit(1);
	}
	strcpy(title,"CAD[�],time[s],paq_10_1,paq_10_3,paq_10_5,paq_30_1,paq_30_3,paq_30_5,paq_50_1,paq_50_3,paq_50_5,paq_70_1,paq_70_3,paq_70_5,paq_90_1,paq_90_3,paq_90_5");
	fprintf(fich, "%s", title);

	for(counter=0;counter<size;counter++){
	fprintf(fich, "\n%lf,%lf,",CAD[counter],time_vector[counter]);
	for(i_aux=0;i_aux<15;i_aux++){
	fprintf(fich, "%lf,",elementcontrol[i_aux][counter].mfuel);
	}
	}
	fclose(fich);

	if((fich=fopen("c:\\ACT_model\\output\\mfuel_real.dat","w"))==NULL){
	printf("The element file results could not be opened");
	exit(1);
	}
	strcpy(title,"CAD[�],time[s],paq_10_1,paq_10_3,paq_10_5,paq_30_1,paq_30_3,paq_30_5,paq_50_1,paq_50_3,paq_50_5,paq_70_1,paq_70_3,paq_70_5,paq_90_1,paq_90_3,paq_90_5");
	fprintf(fich, "%s", title);

	for(counter=0;counter<size;counter++){
	fprintf(fich, "\n%lf,%lf,",CAD[counter],time_vector[counter]);
	for(i_aux=0;i_aux<15;i_aux++){
	fprintf(fich, "%lf,",elementcontrol[i_aux][counter].mfuel_real);
	}
	}
	fclose(fich);

	if((fich=fopen("c:\\ACT_model\\output\\FI.dat","w"))==NULL){
	printf("The element file results could not be opened");
	exit(1);
	}
	strcpy(title,"CAD[�],time[s],paq_10_1,paq_10_3,paq_10_5,paq_30_1,paq_30_3,paq_30_5,paq_50_1,paq_50_3,paq_50_5,paq_70_1,paq_70_3,paq_70_5,paq_90_1,paq_90_3,paq_90_5");
	fprintf(fich, "%s", title);

	for(counter=0;counter<size;counter++){
	fprintf(fich, "\n%lf,%lf,",CAD[counter],time_vector[counter]);
	for(i_aux=0;i_aux<15;i_aux++){

	fprintf(fich, "%lf,",(elementcontrol[i_aux][counter].FI));

	}
	}
	fclose(fich);


	if((fich=fopen("c:\\ACT_model\\output\\temperature_SD.dat","w"))==NULL){
	printf("The element file results could not be opened");
	exit(1);
	}
	strcpy(title,"CAD[�],time[s],paq_10_1,paq_10_3,paq_10_5,paq_30_1,paq_30_3,paq_30_5,paq_50_1,paq_50_3,paq_50_5,paq_70_1,paq_70_3,paq_70_5,paq_90_1,paq_90_3,paq_90_5");
	fprintf(fich, "%s", title);

	for(counter=0;counter<size;counter++){
	fprintf(fich, "\n%lf,%lf,",CAD[counter],time_vector[counter]);
	for(i_aux=0;i_aux<15;i_aux++){
	fprintf(fich, "%lf,",(elementcontrol[i_aux][counter].TSD));
	}
	}
	fclose(fich);
	if((fich=fopen("c:\\ACT_model\\output\\temperature_adib.dat","w"))==NULL){
	printf("The element file results could not be opened");
	exit(1);
	}
	strcpy(title,"CAD[�],time[s],paq_10_1,paq_10_3,paq_10_5,paq_30_1,paq_30_3,paq_30_5,paq_50_1,paq_50_3,paq_50_5,paq_70_1,paq_70_3,paq_70_5,paq_90_1,paq_90_3,paq_90_5");
	fprintf(fich, "%s", title);

	for(counter=0;counter<size;counter++){
	fprintf(fich, "\n%lf,%lf,",CAD[counter],time_vector[counter]);
	for(i_aux=0;i_aux<15;i_aux++){
	fprintf(fich, "%lf,",(elementcontrol[i_aux][counter].Tadib));
	}
	}
	fclose(fich);


	if((fich=fopen("c:\\ACT_model\\output\\penetration.dat","w"))==NULL){
	printf("The element file results could not be opened");
	exit(1);
	}
	strcpy(title,"CAD[�],time[s],paq_10_1,paq_10_3,paq_10_5,paq_30_1,paq_30_3,paq_30_5,paq_50_1,paq_50_3,paq_50_5,paq_70_1,paq_70_3,paq_70_5,paq_90_1,paq_90_3,paq_90_5");
	fprintf(fich, "%s", title);

	for(counter=0;counter<size;counter++){
	fprintf(fich, "\n%lf,%lf,",CAD[counter],time_vector[counter]);
	for(i_aux=0;i_aux<15;i_aux++){
	fprintf(fich, "%lf,",(elementcontrol[i_aux][counter].X));
	}
	}
	fclose(fich);
	 */

	// exit vectors construction with constant crank angle degree increment

	time_vector_exit = (double*)malloc(CAI*sizeof(double));
	time_vector_exit[0] = -180 * 60 / (360 * speed);
	CAD_exit[0] = -180;

	for (counter = 1; counter < CAI; counter++) {
		time_vector_exit[counter] = time_vector_exit[counter - 1] + 60 / ((CAI - 1) * speed);
		// CAD_exit[counter]=CAD_exit[counter-1]+360*pow((CAI-1),-1);
		CAD_exit[counter] = CAD_exit[counter - 1] + 360. / (CAI - 1); // <<----PACO
	}
	// pcyl
	vector_to_interpolate = (double*)malloc(size*sizeof(double));
	for (counter = 0; counter < size; counter++) {
		vector_to_interpolate[counter] = p_cyl[counter];
	}
	FUNCTION_FOR_INTERPOLATION(aux_vector, time_vector_exit, CAD, vector_to_interpolate, CAI, size,
		speed);
	free(vector_to_interpolate);
	for (counter = 0; counter < CAI; counter++) {
		if ((CAD_exit[counter] < IVC) || (CAD_exit[counter] > EVO)) {
			aux_vector[counter] = 0;
		}
		p_cyl_exit[counter] = aux_vector[counter];
	}

	// dp_da_exit
	vector_to_interpolate = (double*)malloc(size*sizeof(double));
	for (counter = 0; counter < size; counter++) {
		vector_to_interpolate[counter] = dp_da_cyl[counter];
	}
	FUNCTION_FOR_INTERPOLATION(aux_vector, time_vector_exit, CAD, vector_to_interpolate, CAI, size,
		speed);
	free(vector_to_interpolate);
	for (counter = 0; counter < CAI; counter++) {
		if ((CAD_exit[counter] < IVC) || (CAD_exit[counter] > EVO)) {
			aux_vector[counter] = 0;
		}
		dp_da_cyl_exit[counter] = aux_vector[counter];
	}

	// T_cyl
	vector_to_interpolate = (double*)malloc(size*sizeof(double));
	for (counter = 0; counter < size; counter++) {
		vector_to_interpolate[counter] = T_cyl[counter];
	}
	FUNCTION_FOR_INTERPOLATION(aux_vector, time_vector_exit, CAD, vector_to_interpolate, CAI, size,
		speed);
	free(vector_to_interpolate);
	for (counter = 0; counter < CAI; counter++) {
		if ((CAD_exit[counter] < IVC) || (CAD_exit[counter] > EVO)) {
			aux_vector[counter] = 0;
		}
		T_cyl_exit[counter] = aux_vector[counter];
	}

	// HRF
	vector_to_interpolate = (double*)malloc(size*sizeof(double));
	for (counter = 0; counter < size; counter++) {
		vector_to_interpolate[counter] = HRF[counter];
	}
	FUNCTION_FOR_INTERPOLATION(aux_vector, time_vector_exit, CAD, vector_to_interpolate, CAI, size,
		speed);
	free(vector_to_interpolate);
	for (counter = 0; counter < CAI; counter++) {
		if ((CAD_exit[counter] < IVC) || (CAD_exit[counter] > EVO)) {
			aux_vector[counter] = 0;
		}
		HRF_exit[counter] = aux_vector[counter];
	}

	/** *************** HRF=0 if mfuel==0********* */
	if (aux_mfuel == 0) {
		for (counter = 0; counter < CAI; counter++) {
			HRF_exit[counter] = 0;
		}
	}

	// ROHR
	vector_to_interpolate = (double*)malloc(size*sizeof(double));
	for (counter = 0; counter < size; counter++) {
		vector_to_interpolate[counter] = ROHR[counter];
	}
	FUNCTION_FOR_INTERPOLATION(aux_vector, time_vector_exit, CAD, vector_to_interpolate, CAI, size,
		speed);
	free(vector_to_interpolate);
	for (counter = 0; counter < CAI; counter++) {
		if ((CAD_exit[counter] < IVC) || (CAD_exit[counter] > EVO)) {
			aux_vector[counter] = 0;
		}
		ROHR_exit[counter] = aux_vector[counter];
	}

	// H_Cooler
	vector_to_interpolate = (double*)malloc(size*sizeof(double));
	for (counter = 0; counter < size; counter++) {
		vector_to_interpolate[counter] = H_cooler[counter];
	}
	FUNCTION_FOR_INTERPOLATION(aux_vector, time_vector_exit, CAD, vector_to_interpolate, CAI, size,
		speed);
	free(vector_to_interpolate);
	for (counter = 0; counter < CAI; counter++) {
		if ((CAD_exit[counter] < IVC) || (CAD_exit[counter] > EVO)) {
			aux_vector[counter] = 0;
		}
		H_cooler_exit[counter] = aux_vector[counter] / (delta_t * speed * 360 / 60);
	}

	// injection_rate
	vector_to_interpolate = (double*)malloc(size*sizeof(double));
	for (counter = 0; counter < size; counter++) {
		vector_to_interpolate[counter] = dmf[counter];
	}
	FUNCTION_FOR_INTERPOLATION(aux_vector, time_vector_exit, CAD, vector_to_interpolate, CAI, size,
		speed);
	free(vector_to_interpolate);
	for (counter = 0; counter < CAI; counter++) {
		if ((CAD_exit[counter] < IVC) || (CAD_exit[counter] > EVO)) {
			aux_vector[counter] = 0;
		}
		injection_rate_exit[counter] = aux_vector[counter];
	}

	// Accum_injection_rate
	vector_to_interpolate = (double*)malloc(size*sizeof(double));
	for (counter = 0; counter < size; counter++) {
		vector_to_interpolate[counter] = acu_dmf[counter];
	}
	FUNCTION_FOR_INTERPOLATION(aux_vector, time_vector_exit, CAD, vector_to_interpolate, CAI, size,
		speed);
	free(vector_to_interpolate);
	for (counter = 0; counter < CAI; counter++) {
		if ((CAD_exit[counter] < IVC) || (CAD_exit[counter] > EVO)) {
			aux_vector[counter] = 0;
		}
		accum_injection_rate_exit[counter] = aux_vector[counter];
	}

	/** *********MEMORY LIBERATION********* */

	free(CAD);
	free(time_vector);
	free(EOI_IM);
	free(SOI_IM);
	free(SOC_IM);

	free(mtotal_bowl);
	free(mO2_bowl);
	free(mN2_bowl);
	free(mCO2_bowl);
	free(mH2O_bowl);
	free(mHC_bowl);
	free(Tsq_cyl);
	free(Tadib_cyl);
	free(mNOx_bowl);
	free(mCO_bowl);
	free(HC_bowl);
	free(mSOOT_bowl_A);
	free(mSOOT_bowl_B);
	free(mSOOT_bowl_C);

	free(mtotal_Vc);
	free(mO2_Vc);
	free(mN2_Vc);
	free(mCO2_Vc);
	free(mH2O_Vc);
	free(mNOx_Vc);
	free(mSOOT_Vc_A);
	free(mSOOT_Vc_B);
	free(mSOOT_Vc_C);
	free(mHC_Vc);
	free(dmtotal_Gfactor);

	free(dmf);
	free(acu_dmf);

	free(virtual_velocity);
	free(inj_velocity);

	free(HRF);
	free(ROHR);

	free(T_cyl);
	free(acu_Mbb);
	free(V_cyl);
	free(Mbb);
	free(Yair);
	free(Yburned);
	free(Yfuel);
	free(U);
	free(CV);
	free(H_cooler);
	free(Qcylhead);
	free(Qcyl);
	free(Qpis);
	free(H);
	free(defor);
	free(Rmixture);
	free(roair);
	free(p_cyl);
	free(dp_da_cyl);
	free(aux_vector);
	free(XLO);

	for (counter = 0; counter < 52; counter++)
		free(YNOeq[counter]);
	free(YNOeq);
	for (counter = 0; counter < 52; counter++)
		free(KdYNO[counter]);
	free(KdYNO);

	free(num_i_IM);

	for (counter = 0; counter < inj_num; counter++)
		free(POI_IM[counter]);
	free(POI_IM);
	for (counter = 0; counter < inj_num; counter++)
		free(POC_IM[counter]);
	free(POC_IM);
	for (counter = 0; counter < inj_num; counter++)
		free(mfuel_ij_IM[counter]);
	free(mfuel_ij_IM);
	for (counter = 0; counter < inj_num; counter++)
		free(mfuel_i_IM[counter]);
	free(mfuel_i_IM);

	for (counter = 0; counter < 15; counter++)
		free(elementcontrol[counter]);
	free(elementcontrol);

	free(mixture_correction);

	for (counter = 0; counter < inj_num; counter++)
		free(element[counter]);
	free(element);
	for (counter = 0; counter < inj_num; counter++)
		free(realelement[counter]);
	free(realelement);

	free(complete_p_cyl);
	free(complete_V_cyl);
	free(complete_deform);
	free(complete_CAD);

	free(time_vector_exit);

}

inline double min(double a, double b) {
	if (a <= b) {
		return(a);
	}
	else {
		return(b);
	}
}
