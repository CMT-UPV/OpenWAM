/* --------------------------------------------------------------------------------*\
|==========================|
 |\\   /\ /\   // O pen     | OpenWAM: The Open Source 1D Gas-Dynamic Code
 | \\ |  X  | //  W ave     |
 |  \\ \/_\/ //   A ction   | CMT-Motores Termicos / Universidad Politecnica Valencia
 |   \\/   \//    M odel    |
 ----------------------------------------------------------------------------------
 | License
 |
 |	This file is part of OpenWAM.
 |
 |	OpenWAM is free software: you can redistribute it and/or modify
 |	it under the terms of the GNU General Public License as published by
 |	the Free Software Foundation, either version 3 of the License, or
 |	(at your option) any later version.
 |
 |	OpenWAM is distributed in the hope that it will be useful,
 |	but WITHOUT ANY WARRANTY; without even the implied warranty of
 |	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 |	GNU General Public License for more details.
 |
 |	You should have received a copy of the GNU General Public License
 |	along with OpenWAM.  If not, see <http://www.gnu.org/licenses/>.
 |
 \*-------------------------------------------------------------------------------- */

// ---------------------------------------------------------------------------
#ifndef TTC_HTMH
#define TTC_HTMH
// ---------------------------------------------------------------------------

/**
 * @file TTC_HTMH.h
 * @author Francisco Jose Arnau Martanez <farnau@mot.upv.es>
 * @version 0.1
 *
 * @section LICENSE
 *
 * This file is part of OpenWAM.
 *
 * OpenWAM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenWAM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @section DESCRIPTION
 * The TTC_HTMH class represents the heat transfer in the turbocharger.
 *
 * It has methods to compute the heat transfer in the turbocharger. This file
 * has the interface of TTC_HTMH.
 *
 */

#include "Globales.h"
#include "THTM_Fluids.h"
#include "turbo_bearings.hpp"
//#include <cmath>

enum nmSide {
	nmCompressor = 0, nmTurbine = 1
};

enum nmRePrmu {
	nmReMassTur = 0, nmReMassCom = 1, nmReMassOil = 2, nmReShaft = 3, nmPrTur = 4, nmPrCom = 5, nmPrOil = 6, nmmuG_T = 7, nmmuA_T = 8, nmmuO_T = 10, nmmuW_T = 11, nmmuG_H1 = 12, nmmuA_H1 = 13, nmmuO_H1 = 15, nmmuW_H1 = 16, nmmuG_H2 = 17, nmmuA_H2 = 18, nmmuO_H2 = 19, nmmuW_H2 = 20, nmmuG_H3 = 21, nmmuA_H3 = 22, nmmuO_H3 = 23, nmmuW_H3 = 24, nmmuG_C = 25, nmmuA_C = 26, nmmuO_C = 27, nmmuW_C = 28, nmReWater = 29, nmPrWater = 30, nmReMassComIn = 31, nmPrComIn = 32, nmReOilH1 = 33, nmReOilH2 = 34, nmReOilH3 = 35, nmPrOilH1 = 36, nmPrOilH2 = 37, nmPrOilH3 = 38

};

struct stTurbomachinery {
	double AF; // Air fuel ratio
	double Humidity; // Humidity
	double MassFlow; // Mass flow (kg/s)
	double IT_C; // Inlet temperature (degC)
	double IT_K; // Inlet temperature (K)
	double IP; // Inlet pressure (bar)
	double OP; // Outlet pressure (bar)
	double IP0; // Total inlet pressure (bar)
	double OP0; // Total outlet pressure (bar)
	double PR; // Pressure ratio (-)
	double RTC_C; // Corrected speed (rpm)
	double TISO_K; // Isoentropic outlet temperature (K)
	double TREF_K; // Reference map temperature (K)
	double PREF; // Reference map pressure (K)
	double TMAP_K; // Original map measurement temperature (K)
	double MassFlowC; // Corrected mass flow (kg/s)
	double EFF; // Efficiency
	double T0_K; // Stagnation temperature (K)
	double SecIn; // Inlet section (m^2)
	double SecOut; // Outlet section (m^2)
	double DIn;
	double DOut;
	stHTMair *Fluid;
	double OT_K; // Outlet temperature (K)
	double OT0_K; // Total outlet temperature (K)
	double IT0_K; // Total inlet temperature (K)
	double EFFMAX;
	double Power;

	double funCORRMass() {
		return MassFlow * sqrt(IT_K / TREF_K) / (IP / PREF);
	}
	;

	double funUnCORRMass() {
		return MassFlowC * (IP / PREF) / sqrt(IT_K / TREF_K);
	}
	;

	double funCORRRTC(double RTC) {
		return RTC / sqrt(IT_K / TREF_K);
	}
	;

	double funUnCORRRTC() {
		return RTC_C * sqrt(IT_K / TREF_K);
	}
	;

	double funTiso(double sig) {
		return funT0_in() * pow(PR, sig * (Fluid->g - 1) / Fluid->g);
	}
	;

	double funP_toTotal(double P, double T, double T0) {
		return P / pow(T / T0, Fluid->g / (Fluid->g - 1));
	}
	;

	double funP_toStatic(double P0, double T, double T0) {
		return P0 * pow(T / T0, Fluid->g / (Fluid->g - 1));
	}
	;

	double funT0_in() {
		return IT_K + pow2(MassFlow * 287 * IT_K / __units::BarToPa(IP) / SecIn) / 2 / Fluid->Cp;
	}
	;

	double funtT0_out() {
		return OT_K + pow2(MassFlow * 287 * OT_K / __units::BarToPa(OP) / SecOut) / 2 / Fluid->Cp;
	}
	;

	double funP0() {
		return IP * pow(T0_K / IT_K, (Fluid->g - 1) / Fluid->g);
	}
	;

	double funIsoPower(double sig) {
		return sig * MassFlow * Fluid->Cp * (TISO_K - IT0_K);
	}
	;

	double funT_toStatic(double T0, double P0, double M) {
		OT_K = T0;
		OP = P0;
		double error = 1;
		int iter = 0;
		do {
			OT_K = T0 - pow2(M * 287 * OT_K / __units::BarToPa(OP) / SecOut) / 2 / Fluid->Cp;
			error = funP_toStatic(P0, OT_K, T0) - OP;
			OP = OP + error;
			iter++;
		} while(iter < 100 && fabs(error) < 0.001);
		return OT_K;
	}
	;
};

struct stNodesHTM {
	int Gas;
	int Air;
	int Oil;
	int Water;
	int T;
	int H1;
	int H2;
	int H3;
	int C;
	// int OilNm;
	// int OilOut;
	int AirIn;
	int Amb;
	int O_H1;
	int O_H3;
	int O_H2;
};

struct stCapacity {
	double T;
	double H1;
	double H2;
	double H3;
	double C;
};

struct stConductivities {
	double T_H1;
	double H1_H2;
	double H2_H3;
	double H3_C;
};

struct stOilTemps {
	double In;
	double _1;
	double _2;
	double _3;
	double Out;
};

// struct stConvectivities {
// dVector GAS_T;
// dVector GAS_H1;
// dVector WAT_H2;
// dVector OIL_H1;
// dVector OIL_H2;
// dVector OIL_H3;
// dVector AIR_H3;
// dVector AIR_C_C;
// dVector AIR_C_H;
// };

struct stConvCorrelation {
	dVector Coef;
	iVector Ind;

	double Value(dVector Re_Pr_mu) {
		double val = Coef[0];
		for(unsigned int i = 1; i < Coef.size(); i++) {
			val *= pow(Re_Pr_mu[Ind[i - 1]], Coef[i]);
		}
		return val;
	}
};

struct stConvectivities {
	stConvCorrelation GAS_T;
	stConvCorrelation GAS_H1;
	stConvCorrelation WAT_H2;
	stConvCorrelation OIL_H1;
	stConvCorrelation OIL_H2;
	stConvCorrelation OIL_H3;
	stConvCorrelation AIR_H3;
	stConvCorrelation AIR_C_C;
	stConvCorrelation AIR_C_H;
	stConvCorrelation AMB_T_N;
	stConvCorrelation AMB_T_F;
	stConvCorrelation AMB_C_N;
	stConvCorrelation AMB_C_F;
};

struct stTurbinePower {
	double Power;
	double m;
	double efft;
	double Cp;
	double Tin;
	double ga;

	stTurbinePower(double pow, double ma, double efftur, double Ti, double Cpgas, double gam) {
		Power = pow;
		m = ma;
		efft = efftur;
		Tin = Ti;
		Cp = Cpgas;
		ga = gam;

	}

	double operator()(const double er) {
		return Power - efft * m * Cp * Tin * (1 - pow(er, -ga));
	}

};

struct stCompressorPower {
	double Power;
	double m;
	double effc;
	double Cp;
	double Tin;
	double ga;

	stCompressorPower(double pow, double ma, double effcom, double Ti, double Cpgas, double gam) {
		Power = pow;
		m = ma;
		effc = effcom;
		Tin = Ti;
		Cp = Cpgas;
		ga = gam;
	}

	double operator()(const double cr) {
		return Power - m * Cp * Tin * (pow(cr, ga) - 1) / effc;
	}

};

class TTC_HTM {
  private:

	stTurbomachinery FC;
	stTurbomachinery FT;

	stNodesHTM FNode;

	std::vector<std::string> FNodeLabel;

	double FTime0;

	// Input oil data
	double FO_MassFlow; // Oil mass flow (kg/s)
	double FO_IT_C; // Oil inlet temperature (degC)
	double FO_IT_K; // Oil inlet temperature (K)
	double FO_OT_K; // Oil outlet temperature (K)
	double FO_NT_K; // Oil mech. losses temperature (K)
	double FO_IP; // Oil inlet pressure (bar)

	// Input water data
	double FW_MassFlow;
	double FW_IT_C;
	double FW_IT_K;
	double FW_IP;

	double FMechEff;

	stHTMoil *FOil;
	stOilTemps FOilTemps;

	stHTMwater *FWater;

	double FDShaft;
	double FHD;
	double FDOil;
	double FDWater;

	double FConverge;

	// Input data turbocharger
	double FRTC; // Turbocharger Speed (rpm)

	double FNumberNodes;
	dVector FRePrmu;

	dMatrix FMatrix_Kinv;
	dMatrix FMatrix_KS;
	dMatrix FMatrix_C;
	dMatrix FMatrix_KC;

	dVector FNode_Temp_K;
	dVector FNode_Temp_K_Tr;
	dVector FKnown_Temp_K;

	dMatrix FMatrix_dT;
	dMatrix FMatrix_HF;

	bMatrix FMatrix_Conn;

	stCapacity FCap;
	stConductivities FCond;
	stConvectivities FConv;

	TurboBearings *FMechLosses;

	inline double K_T_H1() {
		// return 12.00;
		return FCond.T_H1;
	}
	;

	inline double K_H1_H2() {
		// return 12.11;
		return FCond.H1_H2;
	}
	;

	inline double K_H2_H3() {
		// return 20.05;
		return FCond.H2_H3;
	}
	;

	inline double K_H3_C() {
		// return 3.74;
		return FCond.H3_C;
	}
	;

	double Oil_Heat_Flow();

  public:
	TTC_HTM(stHTMoil *Oil);

	~TTC_HTM();

	void InputData(double T_AF, double T_Humidity, double T_MassFlow, double T_IT_C, double T_IP, double T_PR,
				   double C_Humidity, double C_MassFlow, double C_IT_C, double C_IP, double C_PR, double O_MassFlow, double O_IT_C,
				   double O_IP, double RTC);

	void TurbochargerData(double DShaft, double HD, double Doil, double DWater);

	void TurbochargerWorkingPoint(double RTC, double MechEff, double O_MassFlow, double O_IT, double O_IP, double W_IT,
								  double W_MassFlow);

	void CompressorData(double PREF, double TREF, double TMAP_K, double Din);

	void CompressorWorkingPoint(double C_Humidity, double C_MassFlow, double C_IT_C, double C_IP, double C_PR, double EFF);

	void TurbineData(double PREF, double TREF, double TMAP_K, double Din);

	void TurbineWorkingPoint(double T_AF, double T_Humidity, double T_MassFlow, double T_IT_C, double T_IP, double T_PR,
							 double EFF);

	void BuildMatrix();

	void BuildCMatrix(double dt);

	void BuildKCMatrix(double dt);

	void SolveNodeTemperatures(double TET, double TSC, double TEC, double TOIL, double MassOil, double MechLosses,
							   double TW, double TAMB);

	void SolveNodeTemperaturesTransient(double TET, double TSC, double TEC, double TOIL, double MassOil, double MechLosses,
										double TW, double TAMB, double time);

	void SolveHeatFlowMatix();

	void SolveDeltaTemp();

	double AdiabaticEff(nmSide Case);

	double CorrectCompressorMap(double m, double cr, double eff, double TinC, double TinT, double Rtc);

	double CorrectTurbineMap(double m, double er, double eff, double TinC, double TinT, double Rtc);

	void AsignTCMechLosses(TurboBearings *MechLosses) {
		FMechLosses = MechLosses;
	}
	;

	void Read_HTM(FILE *fich);

	void InitializeTemp(double TIT, double COT, double CIT, double OIT, double WIT, double TAMB);

	double Turb_Heat_Flow();

	double Comp_Heat_Flow();

	double Comp_Heat_Flow_In();

	double NodeTemperature(int i) {
		return FNode_Temp_K_Tr[i];
	}
	;

	void PrintInsTemperatures(stringstream & insoutput);

	void HeaderInsTemperatures(stringstream & insoutput);

	void PrintInsHeatFlow(stringstream & insoutput);

	void HeaderInsHeatFlow(stringstream & insoutput);

	void SolveDeltaTempTr();

};
// ---------------------------------------------------------------------------
#endif
