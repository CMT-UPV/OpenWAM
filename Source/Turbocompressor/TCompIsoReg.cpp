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

#include "TCompIsoReg.h"

// ---------------------------------------------------------------------------

TCompIsoReg::TCompIsoReg(double Reg, dVector Mass, dVector RC, dVector Eff) {
	stPolar Convert2Polar;

	fRegimen = Reg;
	fCorrectedMass = Mass;
	fCompRatio = RC;
	fEfficiency = Eff;
	CurvaRC = new Hermite_interp(fCorrectedMass, fCompRatio);
	CurvaEf = new Hermite_interp(fCorrectedMass, fEfficiency);

	fAngulo.resize(fCorrectedMass.size());
	fModulo.resize(fCorrectedMass.size());
	int k = 0;

	for(int i = fCorrectedMass.size() - 1; i >= 0; i--) {
		Convert2Polar(fCorrectedMass[i], fCompRatio[i] - 1.);
		fAngulo[k] = Convert2Polar.Ang;
		if(k > 0) {
			if(fAngulo[k] < atan((fCompRatio[i] - fCompRatio[i - 1]) / (fCorrectedMass[i] - fCorrectedMass[i - 1]))) {
				std::cout << "error interpolacion polar" << std::endl;
			}
		}
		fModulo[k] = Convert2Polar.Mod;
		k++;
	}
	fAngMax = MaxComponent(fAngulo);
	fAngMin = MaxComponent(fAngulo);

	CurvaPol = new Hermite_interp(fAngulo, fModulo);

}

TCompIsoReg::~TCompIsoReg() {
	delete CurvaRC;
	delete CurvaEf;

}

double TCompIsoReg::CompRatio(double Mass) {
	return CurvaRC->interp(Mass);
}

double TCompIsoReg::Efficiency(double Mass) {
	return CurvaEf->interp(Mass);
}

double TCompIsoReg::CompRatioPol(double Ang) {
	return CurvaPol->interp(Ang);
}

double TCompIsoReg::AnguloMaximo() {
	return fAngMax;
}

double TCompIsoReg::AnguloMinimo() {
	return fAngMin;
}

#pragma package(smart_init)
