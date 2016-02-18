// ---------------------------------------------------------------------------

#pragma hdrstop

#include "TEffCorrector.h"

// ---------------------------------------------------------------------------

TEffCorrector::TEffCorrector() {

	FNumberNodes = 8;

	FMatriz_KS.resize(FNumberNodes);
	for(int i = 0; i < FNumberNodes; i++) {
		FMatriz_KS[i].resize(FNumberNodes, 0.0);
		// for (int j = 0; j < FNumberNodes; j++) {
		// FMatriz_KS[i][j] = 0.0;
		// }
	}
	FMatrix_dT.resize(FNumberNodes);
	for(int i = 0; i < FNumberNodes; i++) {
		FMatrix_dT[i].resize(FNumberNodes, 0.0);
	}
	FMatrix_HF.resize(FNumberNodes);
	for(int i = 0; i < FNumberNodes; i++) {
		FMatrix_HF[i].resize(FNumberNodes, 0.0);
	}
	FNode_Temp_K.resize(FNumberNodes);

	TurbineNode = 0;
	CompressorNode = 1;
	OilNode = 2;
}

TEffCorrector::~TEffCorrector() {
}

void TEffCorrector::InputData(double T_AF, double T_Humidity, double T_MassFlow, double T_IT_C, double T_IP,
							  double T_PR, double C_Humidity, double C_MassFlow, double C_IT_C, double C_IP, double C_PR, double O_MassFlow,
							  double O_IT_C, double O_IP, double RTC) {

	FT.AF = T_AF; // Turbine A/F
	FT.Humidity = T_Humidity; // Turbine Humidity
	FT.MassFlow = T_MassFlow; // Turbine mass flow (kg/s)
	FT.IT_C = T_IT_C; // Turbine Inlet Temperature (degC)
	FT.IT_K = __units::degCToK(FT.IT_C); // Turbine Inlet Temperature (K)
	FT.IP = T_IP; // Turbine Inlet Pressure (bar)
	FT.PR = T_PR; // Turbine expansion ratio (-)

	// Input compressor data
	FC.Humidity = C_Humidity; // Compressor Humidity;
	FC.MassFlow = C_MassFlow; // Compressor mass flow (kg/s)
	FC.IT_C = C_IT_C; // Compressor inlet temperature (degC)
	FC.IT_K = __units::degCToK(FC.IT_C); // Compressor inlet temperature (K)
	FC.IP = C_IP; // Compressor inelt pressure (bar)
	FC.PR = C_PR; // Compressor compression ratio (-)

	// Input oil data
	FO_MassFlow = O_MassFlow; // Oil mass flow (kg/s)
	FO_IT_C = O_IT_C; // Oil inlet temperature (degC)
	FO_IT_K = __units::degCToK(FO_IT_C); // Oil inlet temperature (K)
	FO_IP = O_IP; // Oil inlet pressure (bar)

	// Input data turbocharger
	FRTC = RTC; // Turbocharger Speed

	FT.MassFlowC = FT.funCORRMass();
	FT.RTC_C = FC.funCORRRTC(RTC);
	FT.TISO_K = FC.funTiso(1.4, 1.);

	FC.MassFlowC = FT.funCORRMass();
	FC.RTC_C = FC.funCORRRTC(RTC);
	FC.TISO_K = FC.funTiso(1.32, -1.);

}

void TEffCorrector::BuidMatrix() {

	double Re_mass_tur = 0., Re_mass_com = 0., Re_shaft = 0., Re_mass_oil = 0.;
	double k_air_T = 0., k_air_C = 0., k_oil = 0.;
	double Pr_oil = 0.;
	double mu_oil_h1 = 0., mu_oil_h2 = 0., mu_oil_h3 = 0.;

	FMatriz_KS[0][0] = 1.0;
	FMatriz_KS[1][1] = 1.0;
	FMatriz_KS[2][2] = 1.0;

	FMatriz_KS[3][0] = hAk_GAS_T(Re_mass_tur, Re_shaft) * k_air_T;

	FMatriz_KS[4][2] = hAk_H1_OIL(Re_mass_oil, Re_shaft, Pr_oil, mu_oil_h1) * k_oil;
	FMatriz_KS[4][3] = K_T_H1();

	FMatriz_KS[5][2] = hAk_H2_OIL(Re_mass_oil, Re_shaft, Pr_oil, mu_oil_h2) * k_oil;
	FMatriz_KS[5][4] = K_H1_H2();

	FMatriz_KS[6][1] = hAk_H3_AIR(Re_mass_com) * k_air_C;
	FMatriz_KS[6][2] = hAk_H3_OIL(Re_mass_oil, Re_shaft, Pr_oil, mu_oil_h3) * k_oil;
	FMatriz_KS[6][5] = K_H2_H3();

	FMatriz_KS[7][1] = hAk_C_AIR(Re_mass_com) * k_air_C;
	FMatriz_KS[7][6] = K_H3_C();

	FMatriz_KS[3][4] = FMatriz_KS[4][3];
	FMatriz_KS[4][5] = FMatriz_KS[5][4];
	FMatriz_KS[5][6] = FMatriz_KS[6][5];
	FMatriz_KS[6][7] = FMatriz_KS[7][6];

	for(int i = 3; i < FNumberNodes; i++) {
		FMatriz_KS[i][i] = 0;
		for(int j = 0; j < FNumberNodes; j++) {
			if(i != j)
				FMatriz_KS[i][i] -= FMatriz_KS[i][j];
		}

	}

}

void TEffCorrector::SolveNodeTemperatures(double TET, double TSC, double TOIL) {

	LUdcmp TempSolver(FMatriz_KS);

	dVector InputTemp;
	InputTemp.resize(FNumberNodes, 0.0);

	InputTemp[0] = TET;
	InputTemp[1] = TSC;
	InputTemp[2] = TOIL;

	TempSolver.solve(InputTemp, FNode_Temp_K);
}

void TEffCorrector::SolveDeltaTemp() {

	for(int i = 0; i < FNumberNodes; i++) {
		for(int j = 0; j < FNumberNodes; j++) {
			FMatrix_dT[i][j] = FNode_Temp_K[j] - FNode_Temp_K[i];
		}
	}

}

void TEffCorrector::SolveHeatFlowMatix() {

	for(int i = 0; i < FNumberNodes; i++) {
		for(int j = 0; j < FNumberNodes; j++) {
			FMatrix_HF[i][j] = FMatriz_KS[i][j] * FMatrix_dT[i][j];
		}
	}
}

double TEffCorrector::Turb_Heat_Flow() {

	double heat = 0;

	for(int i = 0; i < FNumberNodes; i++) {
		heat += FMatrix_HF[i][TurbineNode];
	}
	return heat;
}

double TEffCorrector::Comp_Heat_Flow() {

	double heat = 0;

	for(int i = 0; i < FNumberNodes; i++) {
		heat += FMatrix_HF[i][CompressorNode];
	}
	return heat;
}

double TEffCorrector::Oil_Heat_Flow() {

	double heat = 0;

	for(int i = 0; i < FNumberNodes; i++) {
		heat += FMatrix_HF[i][OilNode];
	}
	return heat;
}

#pragma package(smart_init)
