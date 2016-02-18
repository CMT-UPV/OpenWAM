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
#ifndef NewCompSolverH
#define NewCompSolverH
//---------------------------------------------------------------------------

//#include "roots.h"
#include "Constantes.h"

struct stCompSolverA2 {

	double CC2;
	double Gam;
	double AA1ini;
	double AA2ini;
	double RelComp;
	double A1ini;
	double Sec1;
	double Sec2;
	double V1ini;

	double RelSec;
	double V2;
	double Gaa;
	double Gae;
	double n;
	double k;
	double AA1new;
	double AA2new;
	double CP;

	stCompSolverA2(double c2, double g, double RC, double cp0, double aa1, double aa2, double a1, double s1, double s2,
				   double v1) :
		Gam(g), CC2(c2), CP(cp0), AA1ini(aa1), AA2ini(aa2), RelComp(RC), A1ini(a1), Sec1(s1), Sec2(s2), V1ini(v1) {
		Gaa = 2 / (Gam - 1);
		Gae = (Gam - 1) / Gam;
		RelSec = s1 / s2;
	}
	double operator()(const double A2) {
		V2 = (CC2 * __cons::ARef - A2) * (-Gaa);
		if(V2 < 0) {
			AA1new = AA2ini * pow(RelComp, Gae / 2.) / sqrt(1 + CP);
			AA2new = AA2ini;
		} else {
			AA2new = sqrt(1 + CP) * AA1ini / pow(RelComp, Gae / 2.);
			AA1new = AA1ini;
		}
		return pow(A1ini / pow(AA1new, Gam), Gaa) * V1ini * RelSec - pow(A2 / pow(AA2new, Gam), Gaa) * V2;
	}
};

struct stCompSolverA1 {

	double CC1;
	double CC2;
	double AA1ini;
	double AA2ini;
	double Sec1;
	double Sec2;
	double RelComp;
	double Rend;
	double Gam;
	double Dt;
	double CPAnt;
	double LCar;

	double RelSec;
	double V1;
	double V2;
	double A2;
	double CP;
	double n;
	double AA1new;
	double AA2new;
	double Gaa;
	double Gae;
	double CPfin;
	double A2Max;
	double A2Min;
	double A2MaxLim;
	double A2MinLim;

	stCompSolverA1(double c1, double c2, double s1, double s2, double g, double aa1, double aa2, double RC, double Rd,
				   double Lc, double cp0, double inct) :
		CC1(c1), CC2(c2), AA1ini(aa1), AA2ini(aa2), Sec1(s1), Sec2(s2), RelComp(RC), Gam(g), Rend(Rd), Dt(inct), CPAnt(cp0),
		LCar(Lc) {

		Gaa = 2 / (Gam - 1);
		Gae = (Gam - 1) / Gam;
		CP = (pow(RC, Gae) - 1) / Rd;
		RelSec = s1 / s2;
	}

	double operator()(const double A1) {
		V1 = (CC1 * __cons::ARef - A1) * Gaa;
		if(V1 < 0) {
			CPfin = CPAnt + (1 / LCar * 10) * (-CPAnt) * 0.5 * fabs(V1) * Dt;
			A2Min = CC2 * __cons::ARef + V1 * RelSec / Gaa;
			A2Max = CC2 * __cons::ARef;
			A2MinLim = CC2 * __cons::ARef * 2 / (Gam + 1);
			A2MaxLim = CC2 * __cons::ARef;
		} else if(V1 > 0) {
			CPfin = CPAnt + (1 / LCar * 10) * (CP - CPAnt) * 0.5 * fabs(V1) * Dt;
			A2Min = CC2 * __cons::ARef;
			A2Max = CC2 * __cons::ARef + V1 * RelSec / Gaa;
			A2MinLim = CC2 * __cons::ARef;
			A2MaxLim = CC2 * __cons::ARef * 2 / (3 - Gam);
		} else {
			A2 = CC2 * __cons::ARef;
			V2 = 0.;
			return (A2 * A2) - (A1 * A1) * (1 + CPfin);
		}
		stCompSolverA2 FunA2(CC2, Gam, RelComp, CPAnt, AA1ini, AA2ini, A1, Sec1, Sec2, V1);

		bool Verdad = zbrac2(FunA2, A2Min, A2Max, A2MinLim, A2MaxLim);
		if(Verdad) {
			A2 = FindRoot(FunA2, A2Min, A2Max);
		} else {
			if(fabs(FunA2(A2Min)) < fabs(FunA2(A2Max))) {
				double Error = FunA2(A2Min);
				A2 = A2Min;
			} else {
				double Error = FunA2(A2Max);
				A2 = A2Max;
			}
		}
		V2 = FunA2.V2;

		AA1new = FunA2.AA1new;
		AA2new = FunA2.AA2new;

		return (A2 * A2 + V2 * V2 / Gaa) - (A1 * A1 + V1 * V1 / Gaa) * (1 + CPfin);
	}
};
#endif
