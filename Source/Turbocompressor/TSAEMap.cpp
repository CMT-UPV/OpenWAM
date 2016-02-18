// ---------------------------------------------------------------------------

#pragma hdrstop

#include "TSAEMap.h"

// ---------------------------------------------------------------------------

TSAEMap::TSAEMap(int i) :
	TCompressorMap() {

	FNumeroCompresor = i;

	FMassMAX_int = NULL;
	FPresMAX_int = NULL;
	FEffMAX_int = NULL;

	FIsAdiabatic = true;
}

TSAEMap::~TSAEMap() {
}

void TSAEMap::ReadSAECompressorMap(FILE *fich) {

	double speed = 0., mass = 0., pres = 0., eff = 0.;
	int i = 0; // Curva de isoregimen
	int j = 0; // Puntos de la curva
	int k = 0;
	int puntos = 0;
	double speedmax = 0, massmax = 0, presmax = 1, effmax = 0;
	int points = 0;

	fscanf(fich, "%d", &points);
	FSpeed.resize(i + 1);
	FMass.resize(i + 1);
	FPres.resize(i + 1);
	FEff.resize(i + 1);

	while(k < points) {
		fscanf(fich, "%lf %lf %lf %lf", &speed, &mass, &pres, &eff);
		mass *= FMassMultiplier;
		pres = (pres - 1.) * FCRMultiplier + 1.;
		eff *= FEffMultiplier;
		k += 1;
		if(!feof(fich)) {
			if(j > 0) {
				if(speed != FSpeed[i][j - 1]) {
					i++;
					j = 0;
					FSpeed.resize(i + 1);
					FMass.resize(i + 1);
					FPres.resize(i + 1);
					FEff.resize(i + 1);
					if(speed > speedmax)
						speedmax = speed;
					FMassMAX.push_back(massmax);
					FPresMAX.push_back(presmax);
					FEffMAX.push_back(effmax);
					massmax = mass;
					presmax = pres;
					effmax = eff;
				} else {
					if(mass > massmax)
						massmax = mass;
					if(pres > presmax)
						presmax = pres;
					if(eff > effmax)
						effmax = eff;
				}
			} else {
				massmax = mass;
				presmax = pres;
				effmax = eff;
			}
			FSpeed[i].push_back(speed);
			FMass[i].push_back(mass);
			FPres[i].push_back(pres);
			FEff[i].push_back(eff);
			j++;
		}
	}
	FMassMAX.push_back(massmax);
	FPresMAX.push_back(presmax);
	FEffMAX.push_back(effmax);
	FSpeedMAX = speedmax;

	FNumLines = FSpeed.size();

	FMassMAXMAX = 0;
	FPresMAXMAX = 1;
	FEffMAXMAX = 0;

	for(int i = 0; i < FNumLines; i++) {
		FSpeedVec.push_back(FSpeed[i][0]);

		if(FMassMAX[i] > FMassMAXMAX)
			FMassMAXMAX = FMassMAX[i];
		if(FPresMAX[i] > FPresMAXMAX)
			FPresMAXMAX = FPresMAX[i];
		if(FEffMAX[i] > FEffMAXMAX)
			FEffMAXMAX = FEffMAX[i];
	}

	AdimensionalizeMap();

}

void TSAEMap::AdimensionalizeMap() {

	double tmp = 0.;

	// FSpeedAdim.resize(FNumLines);
	FMassAdim.resize(FNumLines);
	FPresAdim.resize(FNumLines);
	FEffAdim.resize(FNumLines);

	FPre_MassCurve.resize(FNumLines);
	FEff_MassCurve.resize(FNumLines);

	for(int i = 0; i < FNumLines; i++) {
		FSpeedAdim.push_back(FSpeedVec[i] / FSpeedMAX);
		// printf("%lf %lf %lf\n",FSpeedVec[i] ,FSpeedAdim[i],FSpeedVec[i] / FSpeedMAX);
		FMassMAXAdim.push_back(FMassMAX[i] / FMassMAXMAX);
		FPresMAXAdim.push_back((FPresMAX[i] - 1) / (FPresMAXMAX - 1));
		FEffMAXAdim.push_back(FEffMAX[i] / FEffMAXMAX);
		for(unsigned int j = 0; j < FSpeed[i].size(); j++) {

			FMassAdim[i].push_back(FMass[i][j] / FMassMAX[i]);
			FPresAdim[i].push_back((FPres[i][j] - 1) / (FPresMAX[i] - 1));
			FEffAdim[i].push_back(FEff[i][j] / FEffMAX[i]);
		}
		FMassAdim[i].insert(FMassAdim[i].begin(), 0);
		FPresAdim[i].insert(FPresAdim[i].begin(), FPresAdim[i][0]);
		FEffAdim[i].insert(FEffAdim[i].begin(), FEffAdim[i][0]);

		FPre_MassCurve[i] = new Hermite_interp(FMassAdim[i], FPresAdim[i]);
		FEff_MassCurve[i] = new Hermite_interp(FMassAdim[i], FEffAdim[i]);
	}

	FSpeedAdim.insert(FSpeedAdim.begin(), 0.);
	FMassMAXAdim.insert(FMassMAXAdim.begin(), 0.);
	FPresMAXAdim.insert(FPresMAXAdim.begin(), 0.);
	FEffMAXAdim.insert(FEffMAXAdim.begin(), 0.);

	// printf("%d\n", FPresMAXAdim.size());

	FMassMAX_int = new Hermite_interp(FSpeedAdim, FMassMAXAdim);
	FPresMAX_int = new Hermite_interp(FSpeedAdim, FPresMAXAdim);
	FEffMAX_int = new Hermite_interp(FSpeedAdim, FEffMAXAdim);

}

void TSAEMap::InterpolateMAP(double RTC) {

	FRTCAdim = RTC / FSpeedMAX;
	FCurrentIND = FMassMAX_int->locate(FRTCAdim);

	// std::cout << FMassMAXAdim[4] << std::endl;

	if(FRTCAdim <= 1) {
		FCurrentIND = FMassMAX_int->locate(FRTCAdim);
		FCurrentMassMAX = FMassMAX_int->interp(FRTCAdim) * FMassMAXMAX;
		FCurrentPresMAX = FPresMAX_int->interp(FRTCAdim) * (FPresMAXMAX - 1.) + 1.;
		FCurrentEffMAX = FEffMAX_int->interp(FRTCAdim) * FEffMAXMAX;
		FDeltaLow = (FRTCAdim - FSpeedAdim[FCurrentIND]) / (FSpeedAdim[FCurrentIND + 1] - FSpeedAdim[FCurrentIND]);

	} else {
		FCurrentIND = FNumLines;
		FCurrentMassMAX = pow(FRTCAdim, 1.2) * FMassMAXMAX;
		FCurrentPresMAX = pow(FRTCAdim, 1.2) * (FPresMAXMAX - 1.) + 1.;
		FCurrentEffMAX = exp(1 - FRTCAdim) * FEffMAX_int->interp(FRTCAdim) * FEffMAXMAX;
	}

	// for (int i = 0; i <= FNumLines; ++i) {
	// printf("%lf %lf\n", FSpeedAdim[i], FSpeedAdim[i] * FSpeedMAX);
	// }

}

double TSAEMap::GetCurrentRC(double Mass) {

	double massadim = Mass / FCurrentMassMAX;
	double CurrentRC = 0.;

	if(FCurrentIND == 0) {
		CurrentRC = (FDeltaLow * FPre_MassCurve[FCurrentIND]->interp(massadim)) * (FCurrentPresMAX - 1) + 1;
	} else if(FCurrentIND == FNumLines) {
		CurrentRC = FPre_MassCurve[FCurrentIND - 1]->interp(massadim) * (FCurrentPresMAX - 1) + 1;
	} else {
		double pres_lo = FPre_MassCurve[FCurrentIND - 1]->interp(massadim);
		double pres_hi = FPre_MassCurve[FCurrentIND]->interp(massadim);

		CurrentRC = ((1 - FDeltaLow) * pres_lo + FDeltaLow * pres_hi) * (FCurrentPresMAX - 1) + 1;
	}
	return CurrentRC;

}

double TSAEMap::GetCurrentEff(double Mass) {

	double massadim = Mass / FCurrentMassMAX;
	double CurrentEff = 0.;

	if(FCurrentIND == 0) {
		CurrentEff = (FDeltaLow * FEff_MassCurve[FCurrentIND]->interp(massadim)) * FCurrentEffMAX;
	} else if(FCurrentIND == FNumLines) {
		CurrentEff = FEff_MassCurve[FCurrentIND - 1]->interp(massadim) * FCurrentEffMAX;
	} else {
		double pres_lo = FEff_MassCurve[FCurrentIND - 1]->interp(massadim);
		double pres_hi = FEff_MassCurve[FCurrentIND]->interp(massadim);

		CurrentEff = ((1 - FDeltaLow) * pres_lo + FDeltaLow * pres_hi) * FCurrentEffMAX;
	}
	return CurrentEff;

}

void TSAEMap::LeeMapa(FILE *fich) {

	int Adiab = 0;

	fscanf(fich, "%lf %lf ", &FPresionRef, &FTempRef);
	FTempRef = __units::degCToK(FTempRef);
	FPresionRef = __units::BarToPa(FPresionRef);

	fscanf(fich, "%lf %lf %lf ", &FMassMultiplier, &FCRMultiplier, &FEffMultiplier);
#ifdef tchtm
	fscanf(fich, "%d ", &Adiab);
	if(Adiab == 0) {
		FIsAdiabatic = false;
		fscanf(fich, "%lf ", &FTempMeasure);
	}
#endif

	ReadSAECompressorMap(fich);
}

double TSAEMap::EvaluaRCHermite(double mass) {
	return GetCurrentRC(mass);
}

double TSAEMap::EvaluaRendSplines(double mass) {
	return GetCurrentEff(mass);
}

double TSAEMap::getGastoRelComp1() {
	return FCurrentMassMAX;
}

double TSAEMap::getRelCompBombeo() {
	if(FCurrentIND == 0) {
		return (1 - FDeltaLow) + FDeltaLow * FPres[FCurrentIND][0];
	}
	if(FCurrentIND == FNumLines) {
		return (FPres[FCurrentIND - 1][0] - 1) * pow(FRTCAdim, 1.2) + 1;
	} else {
		return (1 - FDeltaLow) * FPres[FCurrentIND - 1][0] + FDeltaLow * FPres[FCurrentIND][0];
	}
}

double TSAEMap::getGastoBombeo() {
	if(FCurrentIND == FNumLines) {
		return FMass[FCurrentIND - 1][0] * pow(FRTCAdim, 1.2);
	} else {
		return (1 - FDeltaLow) * FMass[FCurrentIND - 1][0] + FDeltaLow * FMass[FCurrentIND][0];
	}
}

void TSAEMap::InterpolaMapa(double rtc, double T10) {
	double rtc_cor = rtc * sqrt(FTempRef / T10);

	InterpolateMAP(rtc_cor);
}

double TSAEMap::getRegimenCorregido() {
	return FRTCAdim * FSpeedMAX;
}

void TSAEMap::PutReference(double pref, double tref) {
	FTempRef = tref;
	FPresionRef = pref;
}

void TSAEMap::CalculateAdiabaticEfficiency(TTC_HTM *HTM, double TinT) {
#ifdef tchtm
	double m = 0., eff = 0., Rtc = 0.;

	if(!FIsAdiabatic) {
		for(int i = 0; i < FNumLines; i++) {
			for(unsigned int j = 0; j < FSpeed[i].size(); j++) {
				m = FMass[i][j] / __units::PaToBar(FPresionRef) / sqrt
					(FTempMeasure / FTempRef);
				Rtc = FSpeed[i][j] / sqrt(FTempRef / FTempMeasure);
				if(FPres[i][j] > 1)
					FEff[i][j] = HTM->CorrectCompressorMap(m, FPres[i][j],
														   FEff[i][j], FTempMeasure, TinT, Rtc);
				if(j == 0)
					FEffMAX[i] = FEff[i][j];
				else if(FEff[i][j] > FEffMAX[i])
					FEffMAX[i] = FEff[i][j];
			}
			for(unsigned int j = 0; j < FSpeed[i].size(); j++) {
				FEffAdim[i][j] = FEff[i][j] / FEffMAX[i];
			}

			if(FEff_MassCurve[i] != NULL)
				delete FEff_MassCurve[i];

			FEff_MassCurve[i] = new Hermite_interp(FMassAdim[i], FEffAdim[i]);
			if(i == 0)
				FEffMAXMAX = FEffMAX[i];
			else if(FEffMAX[i] > FEffMAXMAX)
				FEffMAXMAX = FEffMAX[i];
		}
		for(int i = 0; i < FNumLines; i++) {
			FEffMAXAdim[i] = FEffMAX[i] / FEffMAXMAX;
		}
		if(FEffMAX_int != NULL)
			delete FEffMAX_int;
		FEffMAX_int = new Hermite_interp(FSpeedAdim, FEffMAXAdim);

	}

#endif
}

#pragma package(smart_init)
