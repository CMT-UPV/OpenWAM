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

#include "TTurbineMap.h"
//#include <cmath>

TTurbineMap::TTurbineMap() {
	FIsAdiabatic = true;
	FFixedTurbine = false;

}

TTurbineMap::~TTurbineMap() {

	FTurbPosition.clear();
}

void TTurbineMap::LoadTurbineMap(FILE *Input, double Diam1, double Diam2, double Diam3, double Diam4,
								 double CriticalAngle) {

	int rows = 0, Adiab = 0;
	double pos = 0., ang = 0.;
	double Area = __geom::Circle_area(Diam4);
	double n_limit = 1.165;
	bool CalculaGR = false;

#ifdef tchtm
	fscanf(Input, "%d ", &Adiab);
	if(Adiab == 0) {
		FIsAdiabatic = false;
		fscanf(Input, "%lf ", &FTempMeasure);
	}
#endif

	fscanf(Input, "%d ", &FNumPositions);
	FTurbPosition.resize(FNumPositions);
	for(int i = 0; i < FNumPositions; i++) {
		fscanf(Input, "%d %lf %lf", &rows, &pos, &ang);
		FTurbPosition[i].ReadTurbinPosition(Input, rows, pos, ang);
		if(ang > CriticalAngle)
			CalculaGR = false;
		else
			CalculaGR = true;
		FTurbPosition[i].EffectiveArea(Area, CalculaGR, Diam1, Diam2, Diam3, n_limit);
		// FTurbPosition[i].CalculatePower(923);
		FTurbPosition[i].SearchMapLimits();

		// FPowerMin.push_back(FTurbPosition[i].MinPowerLimit(80));
		// FPowerMax.push_back(FTurbPosition[i].MaxPowerLimit(80));
		// printf("%lf %lf %lf\n", FTurbPosition[i].Rack(), FPowerMin[i], FPowerMax[i]);

	}
	if(FNumPositions == 1)
		FFixedTurbine = true;
}

void TTurbineMap::CurrentEffectiveSection(double n, double er, double rack, double T10T00) {
	if(FFixedTurbine) {
		FTurbPosition[0].InterpolaPosicion(n, er);
		FStatorES = FTurbPosition[0].StatorSec();
		FRotorES = FTurbPosition[0].RotorSec() * sqrt(T10T00);
		FEffTurb = FTurbPosition[0].Efficiency();
	} else {
		int i = 0;
		while(rack > FTurbPosition[i].Rack() && i < FNumPositions) {
			++i;
		}
		if(i == 0) {
			FTurbPosition[i].InterpolaPosicion(n, er);
			FStatorES = FTurbPosition[i].StatorSec();
			FRotorES = FTurbPosition[i].RotorSec();
			FEffTurb = FTurbPosition[i].Efficiency();
		} else {
			FTurbPosition[i].InterpolaPosicion(n, er);
			FTurbPosition[i - 1].InterpolaPosicion(n, er);
			double DeltaRack = (rack - FTurbPosition[i - 1].Rack()) / (FTurbPosition[i].Rack() - FTurbPosition[i - 1].Rack());
			FStatorES = FTurbPosition[i - 1].StatorSec() * (1 - DeltaRack) + FTurbPosition[i].StatorSec() * DeltaRack;
			FRotorES = (FTurbPosition[i - 1].RotorSec() * (1 - DeltaRack) + FTurbPosition[i].RotorSec() * DeltaRack) * sqrt(T10T00);
			FEffTurb = FTurbPosition[i - 1].Efficiency() * (1 - DeltaRack) + FTurbPosition[i].Efficiency() * DeltaRack;
		}
	}
}
// ---------------------------------------------------------------------------

void TTurbineMap::PrintFinalMap(FILE *fich) {
	for(int i = 0; i < FNumPositions; ++i) {
		fprintf(fich, "%lf\n", FTurbPosition[i].Rack());
		FTurbPosition[i].PrintTurbinePosition(fich);
	}
}

void TTurbineMap::CalculateAdiabaticEfficiency(TTC_HTM *HTM, double TinC) {
	if(!FIsAdiabatic) {
		for(int i = 0; i < FNumPositions; ++i) {
			FTurbPosition[i].AdiabaticEfficiency(HTM, FTempMeasure, TinC);

		}
	}
}

#pragma package(smart_init)
