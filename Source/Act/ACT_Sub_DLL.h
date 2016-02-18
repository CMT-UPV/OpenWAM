// ---------------------------------------------------------------------------
#define cero 1.e-6
#define NR 17
#define ANG_CHORRO 60.   // Angulo de chorro.
//#include <vcl.h>
//#include <windows.h>

// ---------------------------------------------------------------------------

#pragma hdrstop
// #include "simstruc.h"
#pragma hdrstop
#include<stdio.h>
#include<math.h>
#include<string.h>
#include<stdlib.h>
#include<malloc.h>
//#include<conio.h>

// Fichero OCULTO

struct sINtype {
	double KSOOTC1;
};

struct sOUTtype {
	double species_EVO[8];
	double *evol_Soot;
	double *evol_Soot_CIL;
	double *evol_Radiacion;
	double *HRF_PMX;
	double *evol_LOL;
};

struct stRadArray {
	double x;
	double RFlame;
	double R;
	int np;
	double dr;
	double Ilambda;
	double Itot;
	double Xsoot[NR];
	double Tau[NR];
	double PTau[NR];
	double T[NR];
};

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

void FUNCTION_FOR_INTERPOLATION(double*interpolated, double*time_interpolated, double*CAD_to_interpolate,
								double*vector_to_interpolate, int size_interpolated, int size_to_interpolate, double speed);

void CALCULUS_OF_VIRTUAL_VELOCITY(double*inj_velocity, double*virtual_velocity, double*dmf, double*time_vector,

								  double rofuel, double dc, double n_holes, double nozzle_d, double D, int size, double PI, double speed, double*EOI_IM,
								  double inj_num, double*SOI_IM, double Piston_D, double DBowl, double CTM, double*CAD, double Kswirl);

void CALCULUS_OF_ACCUMULATED_INJ_RATE(double*acu_dmf, double*dmf, double*time_vector, int size);

void CALCULUS_OF_REACTION_STOICHIOMETRY(double*O2_mass_fuelunit, double*N2_mass_fuelunit, double*CO2_mass_fuelunit,
										double*H2O_mass_fuelunit, double HC);

void STOICHIOMETRY_CONSTANTS(double HC, double*Kst1, double*Kst2, double*Kst3, double*Kst4, double*Kst5, double*Kst6);

void CALCULUS_OF_NUMBER_ELEMENTS(int*num_i_IM, double*time_vector, int size, double speed, double*SOI_IM, double*EOI_IM,
								 int inj_num);

void CALCULUS_OF_POI(double**POI_IM, double**mfuel_i_IM, double**mfuel_ij_IM, double*acu_dmf, double*time_vector,
					 int size, double speed, int*num_i_IM, int num_j, double*SOI_IM, double*EOI_IM, int inj_num,
					 stControlElementComposition * *elementcontrol);

double VOLUME(double CAD, double VTDC, double PI, double Piston_D, double Crank_L, double Connecting_Rod_L, double E);

void CALCULATE_CYCLE(double*roair, double*CAD, double delta_t, double*V_cyl, double VTDC, int counter, double speed,
					 double*p_cyl, double*HRF, double*acu_dmf, double*Mbb, double*acu_Mbb, double AFe, double f, double mfuel, double mEGR,
					 double mairIVC, double*T_cyl, double HP, double*Yair, double*Yfuel, double*Yburned, double*U, double*CV,
					 double*H_cooler, double*H, double TEB, double inj_fuel_temp, double PRECITERACIONES, double*defor, double*Rmixture,
					 double Atmosphere_press, double*Gamma, double PI, double Runiv, double Piston_D, double S, double Crank_L,
					 double Connecting_Rod_L, double E, double Piston_Axis_D, double Piston_Crown_H, double DBowl, double VBowl,
					 double M_Connecting_Rod, double M_P_R_PA, double MW_air, double MW_fuel, double MW_burned, double C_ESteel,
					 double C_Mech_Defor, double CTM, double WC1A, double WC1B, double C2, double C_MBLBY, double Cbb, double TPIS,
					 double TCYL_HEAD, double TCYL, double*Qcylhead, double*Qcyl, double*Qpis);

void DEFORMATIONS(double*V_cyl, double*DEFOR, double p_cyl, double CAD, double delta_CAD, double speed, double PI,
				  double Piston_D, double S, double Connecting_Rod_L, double E, double Piston_Axis_D, double Piston_Crown_H,
				  double M_Connecting_Rod, double M_P_R_PA, double C_ESteel, double C_Mech_Defor);

void MASIC_RATIO(double*Yair, double*Yfuel, double*Yburned, double*Rmixture, double HRF, double acu_mf, double acu_Mbb,
				 double AFe, double f, double mfuel, double mEGR, double mairIVC, double Runiv, double MW_air, double MW_fuel,
				 double MW_burned);

void PROPERTIES(double*u, double*CV, double T_cyl, double T_cyl_pre, double Yair, double Yfuel, double Yburned);

void PROPERTIES_FUEL(double*uf, double T_cyl);

double HEAT_COOLER(double p_cyl, double pressureIVC, double T_cyl, double temperatureIVC, double average_Volume,
				   double volumeIVC, double delta_CAD, double speed, double VTDC, double*H, double PI, double Piston_D, double S,
				   double DBowl, double VBowl, double CTM, double WC1A, double WC1B, double C2, double TPIS, double TCYL_HEAD, double TCYL,
				   double CAD, double*Qcylhead, double*Qcyl, double*Qpis, int counter);

double CALCULATE_C1(double cm, double CTM, double WC1A, double WC1B, double Piston_D, double DBowl, double speed,
					double CAD, double PI);

double BLOW_BY(double p_cyl, double T_cyl, double Rmixture, double delta_CAD, double speed, double Gamma,
			   double Atmosphere_press, double Piston_D, double C_MBLBY, double Cbb);

void CALCULATE_AREAS(double*Piston_area, double*Cylinder_head_area, double PI, double Piston_D, double DBowl,
					 double VBowl);

void CALCULUS_OF_MEAN_VARIABLES(double*p_cyl, double*T_cyl, double*dp_da_cyl, double*CAD, double*pmax, double*Tmax,
								double*dp_da_max, double*p_exit, double*T_exit, int size);

void CALCULUS_OF_IMP_HP(double*complete_p_cyl, double*complete_CAD, double*p_cyl, double*V_cyl, double*complete_V_cyl,
						double*complete_deform, double*WI_HP, double*IMP_HP, int complete_size, int complete_prev_size, double delta_t,
						double speed, int size, double IVC, double EVO, double VTDC, double Cylinder_capacity, double PI, double Piston_D,
						double S, double Crank_L, double Connecting_Rod_L, double E, double Piston_Axis_D, double Piston_Crown_H,
						double M_Connecting_Rod, double M_P_R_PA, double C_ESteel, double C_Mech_Defor, double inlet_pres, double exhaust_pres);

void FUNCTION_NOX(double*YNOeq_value, double*KdYNO_value, double**YNOeq, double**KdYNO, double temperature, double mO2,
				  double mtotal);

void FUNCTION_SOOT_C(double*soot_pre, double element_FI);

double min(double a, double b);

double YSoot_to_FSN(double YSoot);

void ACT(double*engine_parameters, double*engine_model_constants, double*test_variables, double*injection_rate,
		 double*CAD_injection_rate, int size_inlet_inj, int NIN, double*SOI, double*EOI, int CAI, double*CAD_exit,
		 double*HRF_exit, double*ROHR_exit, double*p_cyl_exit, double*dp_da_cyl_exit, double*T_cyl_exit, double*H_cooler_exit,
		 double*mean_var_exit, double*heat_transfer, double*injection_rate_exit, double*accum_injection_rate_exit,
		 sINtype dataIN, sOUTtype * dataOUT);

#pragma argsused
