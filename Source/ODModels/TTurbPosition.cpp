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

#include "TTurbPosition.h"
#include "Globales.h"

// ---------------------------------------------------------------------------

TTurbPosition::TTurbPosition() {

}

TTurbPosition::~TTurbPosition() {
	FSpeedLine.clear();

}

void TTurbPosition::ReadTurbinPosition(FILE *Input, int rows, double pos, double ang) {
	double SP = 0., ER = 0., MF = 0., EF = 0., SP0 = 0., ER0 = 0.;

	FPosition = pos;
	FAngle = ang;

	for(int i = 0; i < rows; i++) {
		fscanf(Input, "%lf %lf %lf %lf", &SP, &ER, &MF, &EF);
		if(i == 0) {
			FLines = 1;
			FSpeedLine.resize(1);
			FSpeedLine[0].AsignaValores(ER, MF, EF);
			FSpeedLine[0].PutSpeed(SP);
		} else {
			if(SP == SP0) {
				if(ER > ER0) {
					FSpeedLine[FLines - 1].AsignaValores(ER, MF, EF);
				} else {
					// error
				}
			} else if(SP > SP0) {
				FLines++;
				FSpeedLine.resize(FLines);
				FSpeedLine[FLines - 1].AsignaValores(ER, MF, EF);
				FSpeedLine[FLines - 1].PutSpeed(SP);
			}
		}
		SP0 = SP;
		ER0 = ER;
	}
	FSpeedMin = FSpeedLine[0].Speed();
	FSpeedMax = FSpeedLine[FLines - 1].Speed();

}

void TTurbPosition::EffectiveArea(double Area, bool CalculaGR, double Diam1, double Diam2, double Diam3,
								  double n_limit) {
	for(int i = 0; i < FLines; i++) {
		FSpeedLine[i].EffectiveSection(Area, CalculaGR, FAngle, Diam1, Diam2, Diam3, n_limit);
		FSpeedLine[i].Adimensionaliza();
	}
}

void TTurbPosition::CalculatePower(double Tin) {
	for(int i = 0; i < FLines; i++) {
		FSpeedLine[i].CalculatePower(Tin);
	}
}

void TTurbPosition::PutPosition(double Pos) {
	FPosition = Pos;
}

void TTurbPosition::SearchMapLimits() {
	for(int i = 0; i < FLines; i++) {
		FSpeed.push_back(FSpeedLine[i].Speed());
		FERMax.push_back(FSpeedLine[i].ERMax());
		FERMin.push_back(FSpeedLine[i].ERMin());
	}
	h_FERMax.resize(FLines);
	h_FERMin.resize(FLines);

	Hermite(FLines, FSpeed, FERMax, &h_FERMax);
	Hermite(FLines, FSpeed, FERMin, &h_FERMin);

}

void TTurbPosition::InterpolaPosicion(double n, double er) {
	double CDStator0, CDStator1, CDRotor0, CDRotor1, ERAdim, DeltaN, ERMax, ERMin, Eff0, Eff1;

	if(n <= FSpeedMin) {
		if(er < FSpeedLine[0].ERMin()) {
			ERAdim = 0;
		} else if(er > FSpeedLine[0].ERMax()) {
			ERAdim = 1;
		} else {
			ERAdim = (er - FSpeedLine[0].ERMin()) / (FSpeedLine[0].ERMax() - FSpeedLine[0].ERMin());
		}

		FStatorSec = FSpeedLine[0].Stator(ERAdim);
		FRotorSec = FSpeedLine[0].Rotor(ERAdim);
		FEfficiency = FSpeedLine[0].Efficiency(ERAdim);
	} else if(n >= FSpeedMax) {
		if(er < FSpeedLine[FLines - 1].ERMin()) {
			ERAdim = 0;
		} else if(er > FSpeedLine[FLines - 1].ERMax()) {
			ERAdim = 1;
		} else {
			ERAdim = (er - FSpeedLine[FLines - 1].ERMin()) / (FSpeedLine[FLines - 1].ERMax() - FSpeedLine[FLines - 1].ERMin());
		}

		FStatorSec = FSpeedLine[FLines - 1].Stator(ERAdim);
		FRotorSec = FSpeedLine[FLines - 1].Rotor(ERAdim);
		FEfficiency = FSpeedLine[FLines - 1].Efficiency(ERAdim);
	} else {
		int i = 0;
		while(FSpeedLine[i].Speed() < n) {
			++i;
		}

		ERMax = EvaluaHermite(n, FLines, FSpeed, FERMax, h_FERMax);
		ERMin = EvaluaHermite(n, FLines, FSpeed, FERMin, h_FERMin);
		ERAdim = (er - ERMin) / (ERMax - ERMin);
		CDStator0 = FSpeedLine[i - 1].Stator(ERAdim);
		CDStator1 = FSpeedLine[i].Stator(ERAdim);
		CDRotor0 = FSpeedLine[i - 1].Rotor(ERAdim);
		CDRotor1 = FSpeedLine[i].Rotor(ERAdim);
		Eff0 = FSpeedLine[i - 1].Efficiency(ERAdim);
		Eff1 = FSpeedLine[i].Efficiency(ERAdim);
		DeltaN = (n - FSpeed[i - 1]) / (FSpeed[i] - FSpeed[i - 1]);
		FStatorSec = CDStator0 * (1 - DeltaN) + CDStator1 * DeltaN;
		FRotorSec = CDRotor0 * (1 - DeltaN) + CDRotor1 * DeltaN;
		FEfficiency = Eff0 * (1 - DeltaN) + Eff1 * DeltaN;
	}
}

void TTurbPosition::PrintTurbinePosition(FILE *fich) {
	for(int i = 0; i < FLines; i++) {
		FSpeedLine[i].PrintEffectiveSection(fich);
	}
}

double TTurbPosition::StatorSec() {
	return FStatorSec;
}

double TTurbPosition::RotorSec() {
	return FRotorSec;
}

double TTurbPosition::Rack() {
	return FPosition;
}

double TTurbPosition::Efficiency() {
	return FEfficiency;
}

double TTurbPosition::MaxPowerLimit(double rtc) {

	int i = 0;
	while(FSpeedLine[i].Speed() < rtc) {
		++i;
	}
	double DeltaN = (rtc - FSpeed[i - 1]) / (FSpeed[i] - FSpeed[i - 1]);
	double PowerMax0 = FSpeedLine[i - 1].PowerMax();
	double PowerMax1 = FSpeedLine[i].PowerMax();
	return PowerMax0 * (1 - DeltaN) + PowerMax1 * DeltaN;

}

double TTurbPosition::MinPowerLimit(double rtc) {

	int i = 0;
	while(FSpeedLine[i].Speed() < rtc) {
		++i;
	}
	double DeltaN = (rtc - FSpeed[i - 1]) / (FSpeed[i] - FSpeed[i - 1]);
	double PowerMin0 = FSpeedLine[i - 1].PowerMin();
	double PowerMin1 = FSpeedLine[i].PowerMin();
	return PowerMin0 * (1 - DeltaN) + PowerMin1 * DeltaN;

}

void TTurbPosition::AdiabaticEfficiency(TTC_HTM *HTM, double TinT, double TinC) {
	for(int i = 0; i < FLines; ++i) {
		FSpeedLine[i].GetAdiabaticEfficiency(HTM, TinT, TinC);
	}
}

#pragma package(smart_init)
