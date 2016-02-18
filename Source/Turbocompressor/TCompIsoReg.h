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
#ifndef TCompIsoRegH
#define TCompIsoRegH

// #include "nr3.h"
// #include "interp_1d.h"
#include "Globales.h"

class TCompIsoReg {
  private:

	double fRegimen;

	double fAngMax;
	double fAngMin;

	dVector fCorrectedMass;
	dVector fCompRatio;
	dVector fEfficiency;

	dVector fAngulo;
	dVector fModulo;

	Base_interp *CurvaRC;
	Base_interp *CurvaEf;
	Base_interp *CurvaPol;

  public:
	TCompIsoReg(double Reg, dVector Mass, dVector RC, dVector Eff);
	~TCompIsoReg();

	double CompRatio(double Mass);

	double Efficiency(double Mass);

	double CompRatioPol(double Ang);

	double AnguloMaximo();

	double AnguloMinimo();

};
// ---------------------------------------------------------------------------

typedef std::vector<TCompIsoReg*> vecTCompIsoReg;
#endif
