/* --------------------------------------------------------------------------------*\
==========================|
 \\   /\ /\   // O pen     | OpenWAM: The Open Source 1D Gas-Dynamic Code
 \\ |  X  | //  W ave     |
 \\ \/_\/ //   A ction   | CMT-Motores Termicos / Universidad Politecnica Valencia
 \\/   \//    M odel    |
 ----------------------------------------------------------------------------------
 License

 This file is part of OpenWAM.

 OpenWAM is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 OpenWAM is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with OpenWAM.  If not, see <http://www.gnu.org/licenses/>.


 \*-------------------------------------------------------------------------------- */

// ---------------------------------------------------------------------------
#pragma hdrstop

#include "TIsoSpeedLine.h"
//#include <cmath>
#include "Globales.h"

// ---------------------------------------------------------------------------

TIsoSpeedLine::TIsoSpeedLine() {
	iStator = NULL;
	iRotor = NULL;
	iEfficiency = NULL;
}

TIsoSpeedLine::~TIsoSpeedLine() {
	delete iStator;
	delete iRotor;
	delete iEfficiency;
}
;

void TIsoSpeedLine::ReadIsoSpeed(int points, FILE *Input) {
	double ER = 0., MF = 0., EF = 0.;
	FNumDatos = points;
	for(int i = 0; i < points; i++) {
		fscanf(Input, "%lf %lf %lf", &MF, &ER, &EF);
		FReducedAirMassFlow.push_back(MF);
		FExpansionRatio.push_back(ER);
		FEfficiency.push_back(EF);
	}
}

void TIsoSpeedLine::AsignaValores(double ER, double MF, double EF) {
	FReducedAirMassFlow.push_back(MF);
	FExpansionRatio.push_back(ER);
	FEfficiency.push_back(EF);
}

void TIsoSpeedLine::EffectiveSection(double Area, bool CalculaGR, double Angle, double Diam1, double Diam2,
									 double Diam3, double n_limit) {
	double tmp1 = 0., tmp2 = 0.;
	double FGamma = 1.40, FR = 287;
	double T00_T0, P00_P0, P2_P0, T2_T0, f_P2_P0, GR, nN, n, P2_P00, T2_T00, T1_T0, gG, g, kK, k, P1_P0, P00_P1, P1_P2,
		   raiZ;

	FNumDatos = FReducedAirMassFlow.size();

	for(int i = 0; i < FNumDatos; i++) {

		tmp1 = 1;
		tmp2 = 1;
		do {
			tmp1 = tmp2;
			tmp2 = 1 + ((FGamma - 1) / 2) * (FR / FGamma) * (pow2(FReducedAirMassFlow[i] / 1000000) / pow2(Area)) * pow((tmp1),
					((FGamma + 1) / (FGamma - 1)));
		} while(fabs(tmp1 - tmp2) / tmp1 > 1e-12);
		T00_T0 = tmp2;
		P00_P0 = pow(T00_T0, FGamma / (FGamma - 1));
		P2_P0 = (1 / FExpansionRatio[i]) * pow(T00_T0, (FGamma / (FGamma - 1)));
		T2_T0 = 1 - (FEfficiency[i]) * (1 - pow((1 / FExpansionRatio[i]), ((FGamma - 1) / FGamma))) * T00_T0;
		f_P2_P0 = FExpansionRatio[i] * (1 / T00_T0 - FEfficiency[i] * (1 - pow((1 / FExpansionRatio[i]),
										((FGamma - 1) / FGamma))));
		if(CalculaGR) {
			GR = 1 - (((2 * FR * tan(__units::DegToRad(Angle))) / (Diam1 * pow2(Diam2) * pow2(__cons::Pi))) * ((
						  FReducedAirMassFlow[i] / 1000000) / FSpeed) * f_P2_P0);
			// new code --> if the reaction degree is lower than 0.4 then force it to 0.4 value instead of calculating lower values or even negative values
			if(GR < 0.5) {
				GR = 0.5;
			}
		} else {
			GR = 0.5;
		}
		nN = log(P2_P0) / log(T2_T0);
		n = nN / (nN - 1);
		P2_P00 = P2_P0 / P00_P0;
		T2_T00 = T2_T0 / T00_T0;
		T1_T0 = 1 + T00_T0 * (GR - 1) * FEfficiency[i] * (1 - pow(P2_P00, (FGamma - 1) / FGamma));
		if(n > n_limit) {
			gG = ((FGamma / (FGamma - 1)) - nN * (log(T2_T00) + log(T00_T0)) / log(T1_T0)) / (1 - (log(T2_T00) + log(T00_T0)) / log(
						T1_T0));
			g = (n / (n - n_limit) + (gG / (gG - 1)) / (FGamma - n)) / (1 / (n - n_limit) + 1 / (FGamma - n));
		} else {
			gG = 0.;
			g = (FGamma / (n - 1) + n / (n_limit - n)) / (1 / (n - 1) + 1 / (n_limit - n));
		}
		kK = (g / (g - 1)) + (nN - (g / (g - 1))) * (log(T2_T00) + log(T00_T0)) / log(T1_T0);
		k = kK / (kK - 1);
		P1_P0 = pow(1 + T00_T0 * (GR - 1) * FEfficiency[i] * (1 - pow(P2_P00, ((FGamma - 1) / FGamma))), kK);
		P00_P1 = P00_P0 / P1_P0;
		P1_P2 = P1_P0 / P2_P0;
		raiZ = (2 / (FGamma - 1)) * (1 - pow(1 / P00_P1, ((FGamma - 1) / FGamma)));
		if(raiZ < 0)
			raiZ = 0;
		else
			raiZ = sqrt(raiZ);
		// raiZ = sqrt((2 / (FGamma - 1)) * (1 - pow(1 / P00_P1 , ((FGamma - 1) / FGamma))));
		StatorEffectiveSection.push_back(FReducedAirMassFlow[i] * 1e-6 * pow(P00_P1,
										 (1 / FGamma)) * (sqrt(FR / FGamma)) / raiZ);
		// raiZ = sqrt((2 / (FGamma - 1)) * (1 - pow(1 / P1_P2 , ((FGamma - 1) / FGamma))));
		raiZ = (2 / (FGamma - 1)) * (1 - pow(1 / P1_P2, ((FGamma - 1) / FGamma)));
		if(raiZ < 0)
			raiZ = 0;
		else
			raiZ = sqrt(raiZ);
//		RotorEffectiveSection.push_back
//			(FReducedAirMassFlow[i] * 1e-6 * P00_P1 * pow(P1_P2,
//				(1 / FGamma)) * (sqrt(FR / FGamma)) / raiZ * sqrt
//			((1 / T1_T0) - (T00_T0 / T1_T0) + (T2_T00 * T1_T0 / T00_T0)));
		RotorEffectiveSection.push_back(FReducedAirMassFlow[i] * 1e-6 * P00_P1 * pow(P1_P2,
										(1 / FGamma)) * (sqrt(FR / FGamma)) / raiZ);
	}
}

void TIsoSpeedLine::CalculatePower(double Tin) {

	double Cp = 1004;
	double R = 287;
	double gamma = Cp / (Cp - R);
	double gam = (1 - gamma) / gamma;
	for(int i = 0; i < FNumDatos; i++) {
		FPower.push_back(FReducedAirMassFlow[i] * Cp * FEfficiency[i] * sqrt(Tin) * FExpansionRatio[i] / 10 * (1 - pow(
							 FExpansionRatio[i], gam)));
		// printf("%4.2lf\t", FPower[i]);
	}
	FPowerMin = FPower.front();
	FPowerMax = FPower.back();
	// printf("\n");
}

void TIsoSpeedLine::PutSpeed(double sp) {
	FSpeed = sp;
}

void TIsoSpeedLine::Adimensionaliza() {
	double DeltaPreMax = FExpansionRatio.back() - FExpansionRatio.front();
	double m = 0., Rtc = 0.;

	for(int i = 0; i < FNumDatos; ++i) {
		FExpansionRatioAdim.push_back((FExpansionRatio[i] - FExpansionRatio.front()) / DeltaPreMax);

	}
	FERMin = FExpansionRatio.front();
	FERMax = FExpansionRatio.back();

	iStator = new Hermite_interp(FExpansionRatioAdim, StatorEffectiveSection);

	iRotor = new Hermite_interp(FExpansionRatioAdim, RotorEffectiveSection);

	iEfficiency = new Hermite_interp(FExpansionRatioAdim, FEfficiency);

}

void TIsoSpeedLine::GetAdiabaticEfficiency(TTC_HTM *HTM, double TinT, double TinC) {

#ifdef tchtm
	double m = 0., Rtc = 0.;

	for(int i = 0; i < FNumDatos; ++i) {
		m = FReducedAirMassFlow[i] / sqrt(TinT) * FExpansionRatio[i] / 10;
		Rtc = FSpeed * 60 * sqrt(TinT);
		FEfficiency[i] = HTM->CorrectTurbineMap(m, FExpansionRatio[i],
												FEfficiency[i], TinC, TinT, Rtc);
	}
	if(iEfficiency != NULL)
		delete iEfficiency;
	iEfficiency = new Hermite_interp(FExpansionRatioAdim, FEfficiency);
#endif
}

double TIsoSpeedLine::Stator(double ERAdim) {
	double ret_val = iStator->interp(ERAdim);

	return ret_val;
}

double TIsoSpeedLine::Rotor(double ERAdim) {
	double ret_val = iRotor->interp(ERAdim);

	return ret_val;
}

double TIsoSpeedLine::Efficiency(double ERAdim) {
	double ret_val = iEfficiency->interp(ERAdim);

	return ret_val;
}

void TIsoSpeedLine::PrintEffectiveSection(FILE * fich) {
	for(dVector::size_type i = 0; i < FExpansionRatio.size(); i++) {
		fprintf(fich, "%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n", FSpeed, FExpansionRatio[i], FReducedAirMassFlow[i], FEfficiency[i],
				FExpansionRatioAdim[i], StatorEffectiveSection[i],
				RotorEffectiveSection[i]);
	}
}

double TIsoSpeedLine::Speed() {
	return FSpeed;
}

double TIsoSpeedLine::ERMax() {
	return FERMax;
}

double TIsoSpeedLine::ERMin() {
	return FERMin;
}

#pragma package(smart_init)
