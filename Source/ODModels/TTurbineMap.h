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
#ifndef TTurbineMapH
#define TTurbineMapH

#include "TTurbPosition.h"

class TTurbineMap {
  private:

	double FDiamIN;
	double FDiamOUT;
	int FNumPositions;

	std::vector<TTurbPosition> FTurbPosition;

	bool FFixedTurbine;

	bool FIsAdiabatic;
	double FTempMeasure;

	double FStatorES;
	double FRotorES;
	double FEffTurb;

	dVector FPowerMin;
	dVector FPowerMax;

  public:
	TTurbineMap();

	~TTurbineMap();

	void LoadTurbineMap(FILE *Input, double Diam1, double Diam2, double Diam3, double Diam4, double CriticalAngle);

	void CurrentEffectiveSection(double n, double er, double rack, double T10T00);

	double StatorEF() {
		return FStatorES;
	}
	;

	double RotorEF() {
		return FRotorES;
	}
	;

	double EffTurb() {
		return FEffTurb;
	}
	;

	double getTempMeasure() {
		return FTempMeasure;
	}
	;

	void PrintFinalMap(FILE *fich);

	void CalculateAdiabaticEfficiency(TTC_HTM *HTM, double TinC);

	double TempMeasure() {
		return FTempMeasure;
	}
	;

};
// ---------------------------------------------------------------------------
#endif
