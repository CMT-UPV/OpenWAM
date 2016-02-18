// ---------------------------------------------------------------------------

#ifndef TCompressorMapH
#define TCompressorMapH

#include "Globales.h"
#include "TTC_HTM.h"

// #include "Math_wam.h"
// ---------------------------------------------------------------------------

class TCompressorMap {
  private:

  protected:

	double FMassMultiplier;
	double FCRMultiplier;
	double FEffMultiplier;

  public:

	TCompressorMap();

	virtual ~TCompressorMap() = 0;

	virtual void LeeMapa(FILE *fich) = 0;

	virtual void InterpolaMapa(double rtc, double AmbientTemperature) = 0;

	virtual double EvaluaRendSplines(double MasaAire) = 0;

	virtual double EvaluaRCHermite(double Massflow) = 0;

	virtual double getMaxCompRatio() = 0;

	virtual double getRelCompBombeo() = 0;

	virtual double getTempRef() = 0;

	virtual double getPresionRef() = 0;

	virtual double getTempMeasure() = 0;

	virtual double getGastoRelComp1() = 0;

	virtual double getGastoBombeo() = 0;

	virtual double getRegimenCorregido() = 0;

	virtual void CalculateAdiabaticEfficiency(TTC_HTM *HTM, double TinT) = 0;

	virtual int getNumPuntos() {
		return 0;
	}
	;

	virtual double GetRelCompInt(int i) {
		return 0;
	}
	;

	virtual double GetGastoInt(int i) {
		return 0;
	}
	;

};

#endif
