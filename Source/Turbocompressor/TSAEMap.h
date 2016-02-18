// ---------------------------------------------------------------------------

#ifndef TSAEMapH
#define TSAEMapH

#include <vector>
#include <cstdio>

#include "TCompressorMap.h"

// ---------------------------------------------------------------------------

class TSAEMap: public TCompressorMap {
  private:
	int FNumeroCompresor;       //Numero del compresor al que pertenece el mapa.

	double FTempRef;
	double FPresionRef;

	bool FIsAdiabatic;
	double FTempMeasure;

	dMatrix FSpeed;
	dVector FSpeedVec;
	dVector FSpeedAdim;

	dVector FSpeedIndex;
	double FSpeedMAX;

	int FNumLines;

	dMatrix FMass;
	dMatrix FMassAdim;
	Base_interp* FMassMAX_int;
	dVector FMassMAX;
	dVector FMassMAXAdim;
	double FMassMAXMAX;

	dMatrix FPres;
	dMatrix FPresAdim;
	Base_interp* FPresMAX_int;
	dVector FPresMAX;
	dVector FPresMAXAdim;
	double FPresMAXMAX;

	dMatrix FEff;
	dMatrix FEffAdim;
	Base_interp* FEffMAX_int;
	std::vector<double> FEffMAX;
	std::vector<double> FEffMAXAdim;
	double FEffMAXMAX;

	dMatrix FCoefCR;
	dMatrix FCoefEff;

	std::vector<Base_interp*> FPre_MassCurve;
	std::vector<Base_interp*> FEff_MassCurve;

	int FCurrentIND;
	double FRTCAdim;
	double FCurrentMassMAX;
	double FCurrentPresMAX;
	double FCurrentEffMAX;
	double FDeltaLow;

	dVector *vdMassAdim, *vdPresAdim, *vdEffAdim;

  public:
	TSAEMap(int i);

	~TSAEMap();

	void ReadSAECompressorMap(FILE *fich);

	void AdimensionalizeMap();

	double GetCurrentRC(double Mass);

	double getMaxCompRatio() {
		return FCurrentPresMAX;
	}
	;

	double GetCurrentEff(double Mass);

	void InterpolateMAP(double RTC);

	void LeeMapa(FILE *fich);

	double EvaluaRCHermite(double mass);

	double EvaluaRendSplines(double mass);

	double getTempRef() {
		return FTempRef;
	}
	;

	double getPresionRef() {
		return FPresionRef;
	}
	;

	double getTempMeasure() {
		return FTempMeasure;
	}
	;

	double getGastoRelComp1();

	double getRelCompBombeo();

	void InterpolaMapa(double rtc, double T10);

	double getGastoBombeo();

	double getRegimenCorregido();

	void PutReference(double pref, double tref);

	void CalculateAdiabaticEfficiency(TTC_HTM *HTM, double TinT);

};

#endif
