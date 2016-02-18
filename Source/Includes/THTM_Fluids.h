/*--------------------------------------------------------------------------------*\
|==========================|
 |\\   /\ /\   // O pen     | OpenWAM: The Open Source 1D Gas-Dynamic Code
 | \\ |  X  | //  W ave     |
 |  \\ \/_\/ //   A ction   | CMT-Motores Termicos / Universidad Politecnica Valencia
 |   \\/   \//    M odel    |
 ----------------------------------------------------------------------------------
 | License
 |
 |	This file is part of OpenWAM.
 |
 |	OpenWAM is free software: you can redistribute it and/or modify
 |	it under the terms of the GNU General Public License as published by
 |	the Free Software Foundation, either version 3 of the License, or
 |	(at your option) any later version.
 |
 |	OpenWAM is distributed in the hope that it will be useful,
 |	but WITHOUT ANY WARRANTY; without even the implied warranty of
 |	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 |	GNU General Public License for more details.
 |
 |	You should have received a copy of the GNU General Public License
 |	along with OpenWAM.  If not, see <http://www.gnu.org/licenses/>.
 |
 \*--------------------------------------------------------------------------------*/

// ---------------------------------------------------------------------------
#ifndef THTM_FluidsH
#define THTM_FluidsH
// ---------------------------------------------------------------------------

#include "Globales.h"

struct stHTM_Fluid {
	double mu;
	double rho;
	double Cp;
	double k;
	double Pr;
	double Re;
	double AF;
	double Hum;
	double R;
	double g;

	stHTM_Fluid() {
	}
	;

	double Property(double T, dVector A) {
		return A[0] + A[1] * T + A[2] * pow2(T) + A[3] * pow3(T) + A[4] * log(T) + A[5] / T + A[6] / pow2(T);
	}
	;

	double fun_mu(double T);

	double fun_k(double T);

	double fun_g(double T);

	double fun_mu(double T, dVector _X);

	double fun_k(double T, dVector _X);

	double fun_g(double T, dVector _X);

	double fun_rho(double T);

	double fun_rho(double p, double T);

	double fun_Pr(double T);

	double fun_Pr(double T, dVector _X);

	void virtual CalcProperties(double p, double T) = 0;
};

struct stHTMair: stHTM_Fluid {

	stHTMair() :
		stHTM_Fluid() {
		AF = 0.;
		Hum = 0.;
	}

	double fun_mu(double T) {
		dVector A;
		A.resize(7);
		A[0] = 0.0000997217;
		A[1] = 0.000000214683;
		A[2] = -0.000000000226615;
		A[3] = 1.1354e-13;
		A[4] = -0.0000218018;
		A[5] = -0.001168773;
		A[6] = 0;

		return Property(T, A);
	}

	double fun_k(double T) {
		dVector A;
		A.resize(7);
		A[0] = -0.028882591;
		A[1] = 0.0000746349;
		A[2] = -0.000000020381;
		A[3] = 6.66102E-13;
		A[4] = 0.005818895;
		A[5] = 0.407241757;
		A[6] = 0;

		return Property(T, A);
	}

	double fun_Cp(double T) {

		double Cp, x;

		if(AF == 0.0) {
			x = Hum / (1 + Hum);
			Cp = (1 - x) * fun_Cp_AirS(T) + x * fun_Cp_W(T);
		} else {
			x = 1 / AF;
			Cp = (1 - x) * fun_Cp_AirS(T) + x * fun_Cp_Gas(T);
		}
		return Cp;
	}

	double fun_Cp_AirS(double T) {
		return -10.4199 * sqrt(T) + 2809.87 - 67227.1 / sqrt(T) + 917124.4 / T - 4174853.6 / pow150(T);
	}

	double fun_Cp_W(double T) {
		return -41.9055 * sqrt(T) + 10447.493 - 382002.49 / sqrt(T) + 6456647.7 / T - 37951136.5 / pow150(T);
	}

	double fun_Cp_Gas(double T) {
		return 926.554 + 0.43045 * T - 0.0001125 * pow2(T) + 0.000000008979 * pow3(T);
	}

	double fun_g(double T) {
		return fun_Cp(T) / (fun_Cp(T) - fun_R());
	}

	double fun_R() {
		double x, R_Air;

		if(AF == 0.0) {
			x = Hum / (1 + Hum);
			R_Air = (1 - x) * 287 + x * 462.1762;
		} else {
			x = 1 / AF;
			R_Air = (1 - x) * 287 + x * 285.4;
		}
		return R_Air;
	}

	double fun_rho(double p, double T) {
		return (100000 * p) / (fun_R() * T);
	}

	double fun_Pr(double T) {
		return fun_mu(T) * fun_Cp(T) / fun_k(T);
	}

	void CalcProperties(double p, double T) {
		mu = fun_mu(T);
		R = fun_R();
		Cp = fun_Cp(T);
		g = fun_g(T);
		rho = fun_rho(p, T);
		k = fun_k(T);
		Pr = mu * Cp / k;
	}
};

struct stHTMoil: stHTM_Fluid {

	double mu_c1;
	double mu_c2;
	double mu_c3;

	stHTMoil() :
		stHTM_Fluid() {
		mu_c1 = 0.0115374825;
		mu_c2 = 62.1420218;
		mu_c3 = 275.888115;
	}

	double fun_rho(double T) {
		dVector A;
		A.resize(7);
		A[0] = -16572.74497;
		A[1] = -38.71871842;
		A[2] = 0.054641551;
		A[3] = -3.45327E-05;
		A[4] = 4398.316892;
		A[5] = 0;
		A[6] = 0;

		return Property(T, A);
	}

	double fun_Cp(double T) {
		dVector A;
		A.resize(7);
		A[0] = -34040.12013;
		A[1] = -71.24978467;
		A[2] = 0.107361063;
		A[3] = -6.63966E-05;
		A[4] = 8670.542844;
		A[5] = 0;
		A[6] = 0;

		return Property(T, A);
	}

	double fun_mu(double T) {
		return mu_c1 * exp(mu_c2 / (T - mu_c3));
	}

	double fun_k(double T) {
		dVector A;
		A.resize(7);
		A[0] = -13.78897843;
		A[1] = -0.028315003;
		A[2] = 3.80794E-05;
		A[3] = -2.25321E-08;
		A[4] = 3.437962885;
		A[5] = 0;
		A[6] = 0;

		return Property(T, A);
	}

	double fun_Pr(double T) {
		return fun_mu(T) * fun_Cp(T) / fun_k(T);
	}

	void CalcProperties(double p, double T) {
		mu = fun_mu(T);
		Cp = fun_Cp(T);
		rho = fun_rho(T);
		k = fun_k(T);
		Pr = mu * Cp / k;
	}

};

struct stHTMwater: stHTM_Fluid {

	stHTMwater() :
		stHTM_Fluid() {
	}

	double fun_mu(double T) {

		return 3.117996E-11 * pow2(pow2(T)) - 4.275045E-08 * pow3(T) + 2.202288E-05 * pow2(T) - 5.057924E-03 * T + 4.378690E-01;
	}

	double fun_Cp(double T) {
//		return -41.9055 * sqrt(T) + 10447.493 - 382002.49 / sqrt(T)
//		+ 6456647.7 / T - 37951136.5 / pow150(T);
		double T_K = __units::degCToK(T);
		return 1987087.20115782 + T_K * (4493.2068586048 + T_K * (-6.72837895716355 + T_K * 0.00446936120713534)) -
			   498966.339389734 * log(T_K);
	}

	double fun_rho(double p, double T) {
		return -1.374183E-07 * pow2(pow2(T)) + 1.937996E-04 * pow3(T) - 1.050544E-01 * pow2(
				   T) + 2.527211E+01 * T - 1.249635E+03;
	}

	double fun_k(double T) {
		return 0.58;
	}

	double fun_Pr(double T) {
		return fun_mu(T) * fun_Cp(T) / fun_k(T);
	}

	void CalcProperties(double p, double T) {
		mu = fun_mu(T);
		Cp = fun_Cp(T);
		rho = fun_rho(p, T);
		k = fun_k(T);
		Pr = mu * Cp / k;
	}

};

struct stHTMFreshair: stHTM_Fluid {

	stHTMFreshair() :
		stHTM_Fluid() {
	}

	double fun_mu(double T) {
		dVector A;
		A.resize(7);
		A[0] = 0.0000997217;
		A[1] = 0.000000214683;
		A[2] = -0.000000000226615;
		A[3] = 1.1354e-13;
		A[4] = -0.0000218018;
		A[5] = -0.001168773;
		A[6] = 0;

		return Property(T, A);
	}

	double fun_k(double T) {
		dVector A;
		A.resize(7);
		A[0] = -0.028882591;
		A[1] = 0.0000746349;
		A[2] = -0.000000020381;
		A[3] = 6.66102E-13;
		A[4] = 0.005818895;
		A[5] = 0.407241757;
		A[6] = 0;

		return Property(T, A);
	}

	double fun_Cp(double T) {
		return -10.4199 * sqrt(T) + 2809.87 - 67227.1 / sqrt(T) + 917124.4 / T - 4174853.6 / pow150(T);
	}

	double fun_g(double T) {
		return fun_Cp(T) / (fun_Cp(T) - fun_R());
	}

	double fun_R() {
		double x, R_Air;

		R_Air = 287.;

		return R_Air;
	}

	double fun_rho(double p, double T) {
		return (100000 * p) / (fun_R() * T);
	}

	double fun_Pr(double T) {
		return fun_mu(T) * fun_Cp(T) / fun_k(T);
	}

	void CalcProperties(double p, double T) {
		mu = fun_mu(T);
		R = fun_R();
		Cp = fun_Cp(T);
		g = fun_g(T);
		rho = fun_rho(p, T);
		k = fun_k(T);
		Pr = mu * Cp / k;
	}
};

struct stHTMBurntGas: stHTM_Fluid {

	stHTMBurntGas() :
		stHTM_Fluid() {
	}

	double fun_mu(double T) {
		dVector A;
		A.resize(7);
		A[0] = 0.0000997217;
		A[1] = 0.000000214683;
		A[2] = -0.000000000226615;
		A[3] = 1.1354e-13;
		A[4] = -0.0000218018;
		A[5] = -0.001168773;
		A[6] = 0;

		return Property(T, A);
	}

	double fun_k(double T) {
		dVector A;
		A.resize(7);
		A[0] = -0.028882591;
		A[1] = 0.0000746349;
		A[2] = -0.000000020381;
		A[3] = 6.66102E-13;
		A[4] = 0.005818895;
		A[5] = 0.407241757;
		A[6] = 0;

		return Property(T, A);
	}

	double fun_Cp(double T) {
		return 926.554 + 0.43045 * T - 0.0001125 * pow2(T) + 0.000000008979 * pow3(T);
	}

	double fun_g(double T) {
		return fun_Cp(T) / (fun_Cp(T) - fun_R());
	}

	double fun_R() {
		double x, R_Air;

		R_Air = 285.4;

		return R_Air;
	}

	double fun_rho(double p, double T) {
		return (100000 * p) / (fun_R() * T);
	}

	double fun_Pr(double T) {
		return fun_mu(T) * fun_Cp(T) / fun_k(T);
	}

	void CalcProperties(double p, double T) {
		mu = fun_mu(T);
		R = fun_R();
		Cp = fun_Cp(T);
		g = fun_g(T);
		rho = fun_rho(p, T);
		k = fun_k(T);
		Pr = mu * Cp / k;
	}
};

struct stBurntAirWater: stHTM_Fluid {

	dVector X;
	stHTMwater X0_wat;
	stHTMBurntGas X1_gas;
	stHTMFreshair X2_air;

	stBurntAirWater() :
		stHTM_Fluid() {
		X.resize(2);
		X[0] = 0.;
		X[1] = 0.;
	}

	stBurntAirWater(dVector _X) :
		stHTM_Fluid() {
		X.resize(2);
		X[0] = _X[0];
		X[1] = _X[1];
	}

	void Put_comp(dVector _X) {

		X[0] = _X[0];
		X[1] = _X[1];
	}

	double fun_mu(double T) {
		return X[0] * X0_wat.fun_mu(T) + X[1] * X1_gas.fun_mu(T) + (1 - X[0] - X[1]) * X2_air.fun_mu(T);
	}

	double fun_mu(double T, dVector _X) {
		return _X[0] * X0_wat.fun_mu(T) + _X[1] * X1_gas.fun_mu(T) + (1 - X[0] - X[1]) * X2_air.fun_mu(T);
	}

	double fun_Cp(double T) {
		return X[0] * X0_wat.fun_Cp(T) + X[1] * X1_gas.fun_Cp(T) + (1 - X[0] - X[1]) * X2_air.fun_Cp(T);
	}

	double fun_Cp(double T, dVector _X) {
		return _X[0] * X0_wat.fun_Cp(T) + _X[1] * X1_gas.fun_Cp(T) + (1 - X[0] - X[1]) * X2_air.fun_Cp(T);
	}

	double fun_R() {
		return X[1] * X1_gas.fun_R() + (1 - X[1]) * X2_air.fun_R();
	}

	double fun_R(dVector _X) {
		return _X[1] * X1_gas.fun_R() + (1 - _X[1]) * X2_air.fun_R();
	}

	double fun_Pr(double T) {
		return fun_mu(T) * fun_Cp(T) / fun_k(T);
	}

	double fun_Pr(double T, dVector _X) {
		return fun_mu(T, _X) * fun_Cp(T, _X) / fun_k(T, _X);
	}

	double fun_g(double T) {
		return fun_Cp(T) / (fun_Cp(T) - fun_R());
	}

	double fun_g(double T, dVector _X) {
		return fun_Cp(T, _X) / (fun_Cp(T, _X) - fun_R(_X));
	}

	void CalcProperties(double p, double T) {

	}

};

#endif

