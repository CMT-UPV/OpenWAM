/*--------------------------------------------------------------------------------*\
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


 \*--------------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
#pragma hdrstop

#include "TNewMapComp.h"

//---------------------------------------------------------------------------
TNewMapComp::TNewMapComp() {
	FGastoMin = -0.07;
}

TNewMapComp::~TNewMapComp() {

}

void TNewMapComp::ReadMap(FILE *fich, int correct) {

	double speed = 0., mass = 0., pres = 0., eff = 0.;
	int filas = 0;
	int i = 0; //Curva de isoregimen
	int j = 0; //Puntos de la curva
	if(correct == 1)
		FCorrect = true;
	else
		FCorrect = false;
	fscanf(fich, "%lf %lf", &FPresionRef, &FTempRef);
	fscanf(fich, "%d ", &filas);
	FPresionRef = __units::BarToPa(FPresionRef);

	dMatrix locSpeed;
	dMatrix locMass;
	dMatrix locPre;
	dMatrix locEff;

	locSpeed.resize(i + 1);
	locMass.resize(i + 1);
	locPre.resize(i + 1);
	locEff.resize(i + 1);

	for(int k = 0; k < filas; k++) {
		fscanf(fich, "%lf %lf %lf %lf", &speed, &mass, &pres, &eff);
		if(j > 0) {
			if(speed != locSpeed[i][j - 1]) {
				i++;
				j = 0;
				locSpeed.resize(i + 1);
				locMass.resize(i + 1);
				locPre.resize(i + 1);
				locEff.resize(i + 1);
			}
			locSpeed[i].push_back(speed);
			locMass[i].push_back(mass);
			locPre[i].push_back(pres);
			locEff[i].push_back(eff);
			j++;
		}
	}

	dVector MassExtra;
	dVector PreExtra;
	dVector EffExtra;

	double r1 = sqrt((pow2(FRadHub) + pow2(FRadTip)) / 2);

	dVector::iterator it;

	for(Uint k = 0; k < locSpeed.size(); k++) {
		MassExtra.resize(3);
		PreExtra.resize(3);
		EffExtra.resize(3);
		MassExtra[0] = FGastoMin;
		MassExtra[1] = -0.0001;
		MassExtra[2] = 0;
		PreExtra[0] = locPre[k][0];
		PreExtra[1] = pow(1 + 1 / (__Gamma::Cp_x2 * (FTempRef)) * pow2(__units::RPMToRad_s(locSpeed[k][0])) * (pow2(
							  FRadWheel) - pow2(r1)), __Gamma::G_9);
		PreExtra[2] = pow(1 + 1 / (__Gamma::Cp_x2 * (FTempRef)) * pow2(__units::RPMToRad_s(locSpeed[k][0])) * (pow2(
							  FRadWheel) - pow2(r1)), __Gamma::G_9);
		EffExtra[0] = locEff[k][0];
		EffExtra[1] = locEff[k][0];
		EffExtra[2] = locEff[k][0];
		it = locMass[i].begin();
		locMass[i].insert(it, MassExtra.begin(), MassExtra.end());
		it = locPre[i].begin();
		locPre[i].insert(it, PreExtra.begin(), PreExtra.end());
		it = locEff[i].begin();
		locEff[i].insert(it, EffExtra.begin(), EffExtra.end());
	}

}

#pragma package(smart_init)
