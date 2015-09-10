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


 \*--------------------------------------------------------------------------------*/

/**
 * @file LaxWendroff.cpp
 * @author Francisco Jose Arnau <farnau@mot.upv.es>
 * @author Luis Miguel Garcia-Cuevas Gonzalez <farnau@mot.upv.es>
 *
 * @section LICENSE
 *
 * This file is part of OpenWAM.
 *
 * OpenWAM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenWAM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenWAM.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @section DESCRIPTION
 * This file defines a Lax Wendroff finite-differences integrator for
 * one-dimensional pipes.
 */

#include "LaxWendroff.hpp"

TLaxWendroff::TLaxWendroff() {}


TLaxWendroff::TLaxWendroff(TBasicPipe * pipe)
{
	Connect(pipe);
}


void TLaxWendroff::ComputeFlux(const RowArray & U, RowArray & W,
	const RowVector & Gamma, const RowVector & Gamma1)
{
	int m = U.rows();
	int n = U.cols();
	RowVector U1U0 = U.row(1) / (U.row(0) + 1E-10);

	W.row(0) = U.row(1);
	W.row(1) = U.row(2) * Gamma1 - (Gamma - 3.) * U.row(1) * U1U0 / 2.;
	W.row(2) = Gamma * U.row(2) * U1U0 - Gamma1 * U.row(1) * U1U0.square() / 2.;
	W.bottomRows(m - 3) = U.bottomRows(m - 3).rowwise() * U1U0;

	for (int i = 0; i < n; i++)
	{
		if (DoubEqZero(U(1, i)))
		{
			W(0, i) = 0.;
			W(2, i) = 0.;
			W.block(3, i, m - 3, 1).setZero();
		}
	}
}


void TLaxWendroff::ComputeSource2(const RowArray & U, RowArray & V,
	const RowVector & A, const RowVector & h,
	const RowVector & rho, const RowVector & Re,
	const RowVector & TWall, const RowVector & Gamma1)
{
	V.setZero();
	if (!DoubEqZero(FPipe->FCoefAjusFric))
	{
		double v = 0.;
		double d = 0.;
		double f = 0.;
		for (int i = 0; i < U.cols(); i++)
		{
			v = U(1, i) / U(0, i);
			if (!DoubEqZero(v))
			{
				d = sqrt(A(i) * 4. / Pi);
				f = FPipe->Colebrook(FPipe->FFriction, d, Re(i));
				V(1, i) = U(0, i) * v * v * v / fabs(v) * 2 / d
					* FPipe->FCoefAjusFric;
			}
		}
	}
	if (!DoubEqZero(FPipe->FCoefAjusTC))
	{
	}
}



void TLaxWendroff::ComputeSource1(const RowArray & U, RowArray & V,
	const RowVector & A, const RowVector & Gamma1)
{
	V.setZero();
	V.row(1) = - (U.row(2) - U.row(1).square() / (U.row(0) + 1E-10) / 2.)
		* Gamma1 / A;
	for (int i = 0; i < U.cols(); i++)
	{
		if (DoubEqZero(U(1, i)))
		{
			V(1, i) = U(2, i) * Gamma1(i) / A(i);
		}
	}
}


void TLaxWendroff::Connect(TBasicPipe * pipe) {
	TPipeMethod::Connect(pipe);
	int m = pipe->FU0.rows();
	int n = pipe->FU0.cols();
	FW.setZero(m, n);
	n -= 1;
	FDerLinArea_12 = (pipe->FDerLinArea.rightCols(n)
		- pipe->FDerLinArea.leftCols(n)) / pipe->FXref;
	FArea_12 = (pipe->FArea.rightCols(n) + pipe->FArea.leftCols(n)) / 2.;
	Fhi12.setZero(n);
	Frho12.setZero(n);
	FRe12.setZero(n);
	FTWPipe12.setZero(n);
	FGamma12.setZero(n);
	FR12.setZero(n);
	FGamma1_12.setZero(n);
	Fx1.setZero(m, n);
	Fx2.setZero(m, n);
	Fx3.setZero(m, n);
	Fx4.setZero(m, n);
	FU_12.setZero(m, n);
	FW_12.setZero(m, n);
	FV1_12.setZero(m, n);
	FV2_12.setZero(m, n);
	n -= 1;
	Fx1_12.setZero(m, n);
	Fx2_12.setZero(m, n);
	Fx3_12.setZero(m, n);
	Fx4_12.setZero(m, n);
}


void TLaxWendroff::Solve() {
	int m = FPipe->FU0.rows();
	int n = FPipe->FU0.cols() - 1;
	double dtdx = FPipe->FDeltaTime / FPipe->FXref;
	double dt2 = FPipe->FDeltaTime / 2.;

	ComputeFlux(FPipe->FU0, FW, FPipe->FGamma, FPipe->FGamma1);
	ComputeSource1(FPipe->FU0, FV1, FPipe->FArea, FPipe->FGamma1);
	
	Fx1 = FPipe->FU0.leftCols(n) + FPipe->FU0.rightCols(n);
	Fx2 = -dtdx * (FW.rightCols(n) - FW.leftCols(n));
	Fx3 = (FV1.rightCols(n) + FV1.leftCols(n)).rowwise()
		* FPipe->FDerLinArea * (-dt2);
	Fx4 = -dt2 * (FV2.rightCols(n) + FV2.leftCols(n));
	FU_12 = (Fx1 + Fx2 + Fx3 + Fx4) / 2.;

	Fhi12 = (FPipe->Fhi.leftCols(n) + FPipe->Fhi.rightCols(n)) / 2.;
	Frho12 = (FPipe->Frho.leftCols(n) + FPipe->Frho.rightCols(n)) / 2.;
	FRe12 = (FPipe->FRe.leftCols(n) + FPipe->FRe.rightCols(n)) / 2.;
	FTWPipe12 = (FPipe->FTWPipe.row(0).leftCols(n)
		+ FPipe->FTWPipe.row(0).rightCols(n)) / 2.;
	FGamma12 = (FPipe->FGamma.leftCols(n) + FPipe->FGamma.rightCols(n)) / 2.;
	FR12 = (FPipe->FR.leftCols(n) + FPipe->FR.rightCols(n)) / 2.;
	FGamma1_12 = (FPipe->FGamma1.leftCols(n) + FPipe->FGamma1.rightCols(n))
		/ 2.;

	ComputeFlux(FU_12, FW_12, FGamma12, FGamma1_12);
	ComputeSource1(FU_12, FV1_12, FArea_12, FGamma1_12);
	n -= 1;

	Fx1_12 = FPipe->FU0.block(0, 1, m, n);
	Fx2_12 = -dtdx * (FW_12.rightCols(n) - FW_12.leftCols(n));
	Fx3_12 = (FV1_12.rightCols(n) + FV1_12.leftCols(n)).rowwise()
		* FDerLinArea_12 * (-dt2);
	Fx4_12 = -dt2 * (FV2_12.rightCols(n) + FV2_12.leftCols(n));
	FPipe->FU1.block(0, 1, m, n) = Fx1_12 + Fx2_12 + Fx3_12 + Fx4_12;
}


void TLaxWendroff::SetPTU(double p, double T, double u)
{
	RowVector rhoA = p / (FPipe->FR * T) * FPipe->FArea;
	FPipe->FU0.row(0) = rhoA;
	FPipe->FU0.row(1) = rhoA * u;
	FPipe->FU0.row(2) = rhoA * FPipe->Fcv * T + rhoA * u * u / 2.;
}


void TLaxWendroff::UpdateFlowVariables()
{
	/* TODO: BCs */
	FPipe->Frho = FPipe->FU0.row(0) / FPipe->FArea;
	FPipe->Fspeed = FPipe->FU0.row(1) / FPipe->FU0.row(0);
	FPipe->Fpressure = (FPipe->FU0.row(2) - FPipe->FU0.row(1)
		* FPipe->Fspeed / 2.) * (FPipe->FGamma - 1.);
	FPipe->Ftemperature = FPipe->Fpressure / FPipe->Frho / FPipe->FR;
	FPipe->Fa = (FPipe->FGamma * FPipe->FR * FPipe->Ftemperature).sqrt();
	UpdateGasProperties();
}


void TLaxWendroff::UpdateGasProperties()
{
	/* TODO */
}
