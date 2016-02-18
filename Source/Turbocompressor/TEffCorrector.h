// ---------------------------------------------------------------------------

#ifndef TEffCorrectorH
#define TEffCorrectorH
// ---------------------------------------------------------------------------

#include "Globales.h"

struct stTurbomachinery {
	double AF; // Air fuel ratio
	double Humidity; // Humidity
	double MassFlow; // Mass flow (kg/s)
	double IT_C; // Inlet temperature (degC)
	double IT_K; // Inlet temperature (K)
	double IP; // Inlet pressure (bar)
	double PR; // Pressure ratio (-)
	double RTC_C; // Corrected speed (rpm)
	double TISO_K; // Isoentropic outlet temperature (K)
	double TREF_K; // Reference map temperature (K)
	double PREF; // Reference map pressure (K)
	double TMAP_K; // Original map measurement temperature (K)
	double MassFlowC; // Corrected mass flow (kg/s)
	double EFF; // Efficiency
	double T0_K; // Stagnation temperature (K)
	double Cp; // Heat capacity
	double SecIn; // Inlet section (m^3)

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

	double funTiso(double g, double sig) {
		return IT_K * pow(PR, sig * (g - 1) / g);
	}
	;

	double funT0() {
		return IT_K + pow2(MassFlow * 287 * IT_K / IP / SecIn) / 2 / Cp;
	}
	;

	double funP0(double g) {
		return IP * pow(T0_K / IT_K, (g - 1) / g);
	}
	;
};

class TEffCorrector {
  private:

	stTurbomachinery FC;
	stTurbomachinery FT;

	int TurbineNode;
	int CompressorNode;
	int OilNode;

	// Input oil data
	double FO_MassFlow; // Oil mass flow (kg/s)
	double FO_IT_C; // Oil inlet temperature (degC)
	double FO_IT_K; // Oil inlet temperature (K)
	double FO_IP; // Oil inlet pressure (bar)

	// Input data turbocharger
	double FRTC; // Turbocharger Speed (rpm)

	double FNumberNodes;

	dMatrix FMatriz_KS;
	dVector FNode_Temp_K;

	dMatrix FMatrix_dT;
	dMatrix FMatrix_HF;

	inline double K_T_H1() {
		return 12.00;
	}
	;

	inline double K_H1_H2() {
		return 12.11;
	}
	;

	inline double K_H2_H3() {
		return 20.05;
	}
	;

	inline double K_H3_C() {
		return 3.74;
	}
	;

	inline double hAk_GAS_T(double Re_mass_tur, double Re_shaft) {
		return 0.306 * pow(Re_mass_tur, 0.965) * pow(Re_shaft, -0.417);
	}
	;

	inline double hAk_H1_OIL(double Re_mass_oil, double Re_shaft, double Pr_oil, double mu_oil_h1) {
		return 2.7 * pow(Re_mass_oil, 1.46) * pow(Re_shaft, -1.12) * cbrt(Pr_oil) * pow(mu_oil_h1, -0.69);
	}
	;

	inline double hAk_H2_OIL(double Re_mass_oil, double Re_shaft, double Pr_oil, double mu_oil_h2) {
		return 6.2e-3 * pow(Re_mass_oil, 3.83) * pow(Re_shaft, -2.02) * cbrt(Pr_oil) * pow(mu_oil_h2, -1.02);
	}
	;

	inline double hAk_H3_OIL(double Re_mass_oil, double Re_shaft, double Pr_oil, double mu_oil_h3) {
		return 6.6e-3 * pow(Re_mass_oil, -0.128) * pow(Re_shaft, 1.25) * cbrt(Pr_oil) * pow(mu_oil_h3, -0.876);
	}
	;

	inline double hAk_H3_AIR(double Re_mass_com) {
		return 14.3 * pow(Re_mass_com, 0.225);
	}
	;

	inline double hAk_C_AIR(double Re_mass_com) {
		return 2.6e-2 * pow(Re_mass_com, 0.942);
	}
	;

	double Turb_Heat_Flow();

	double Comp_Heat_Flow();

	double Oil_Heat_Flow();

  public:
	TEffCorrector();

	~TEffCorrector();

	void InputData(double T_AF, double T_Humidity, double T_MassFlow, double T_IT_C, double T_IP, double T_PR,
				   double C_Humidity, double C_MassFlow, double C_IT_C, double C_IP, double C_PR, double O_MassFlow, double O_IT_C,
				   double O_IP, double RTC);

	void BuidMatrix();

	void SolveNodeTemperatures(double TET, double TEC, double TOIL);

	void SolveHeatFlowMatix();

	void SolveDeltaTemp();

};
#endif
