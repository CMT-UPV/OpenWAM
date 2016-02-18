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
#ifndef TTurbPositionH
#define TTurbPositionH

#include "TIsoSpeedLine.h"
//---------------------------------------------------------------------------

class TTurbPosition {
  private:

	double FPosition;
	double FAngle;

	double FStatorSec;
	double FRotorSec;

	double FEfficiency;

	double FSpeedMin;
	double FSpeedMax;

	double FPowerMin;
	double FPowerMax;

	int FLines;

	std::vector<TIsoSpeedLine> FSpeedLine;
	std::vector<double> FERMax;
	std::vector<double> h_FERMax;
	std::vector<double> FERMin;
	std::vector<double> h_FERMin;
	std::vector<double> FSpeed;

  public:

	TTurbPosition();
	~TTurbPosition();

	void ReadTurbinPosition(FILE *Input, int rows, double pos, double ang);

	void EffectiveArea(double Area, bool CalculaGR, double Diam1, double Diam2, double Diam3, double n_limit);

	void CalculatePower(double Tin);

	void PutPosition(double Pos);

	void InterpolaPosicion(double n, double er);

	void SearchMapLimits();

	double StatorSec() /*{return FStatorSec;}*/;

	double RotorSec() /*{return FRotorSec;}*/;

	double Rack() /*{return FPosition;}*/;

	double Efficiency() /*{return FEfficiency;}*/;

	void PrintTurbinePosition(FILE *Fich);

	double MinPowerLimit(double rtc);

	double MaxPowerLimit(double rtc);

	void AdiabaticEfficiency(TTC_HTM *HTM, double TinT, double TinC);

};

#endif
