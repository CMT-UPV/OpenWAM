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
#pragma hdrstop

#include "TAcousticCompressor.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

TAcousticCompressor::TAcousticCompressor() {
}

TAcousticCompressor::TAcousticCompressor(int InletPipeID, int VoluteID, int OutletPipeID, int RotorVolumeID,
		int StatorVolumeID) {

	FInletPipeID = InletPipeID;
	FVoluteID = VoluteID;
	FOutletPipeID = OutletPipeID;
	FRotorVolumeID = RotorVolumeID;
	FStatorVolumeID = StatorVolumeID;
}

TAcousticCompressor::~TAcousticCompressor() {
}

double TAcousticCompressor::P10() {

	double p = FInletPipe->GetPresion(0);
	double a = FInletPipe->GetAsonido(0) * __cons::ARef;
	double v = FInletPipe->GetVelocidad(0) * __cons::ARef;
	double g = FInletPipe->GetGamma(0);

	double p0 = p * pow(1 + (g - 1) / 2 * pow2(v / a), g / (g - 1));

	return p0;
}

double TAcousticCompressor::P1() {

	return FInletPipe->GetPresion(0);

}

double TAcousticCompressor::P20() {

	int n = FOutletPipe->getNin() - 1;

	double p = FOutletPipe->GetPresion(n);
	double a = FOutletPipe->GetAsonido(n) * __cons::ARef;
	double v = FOutletPipe->GetVelocidad(n) * __cons::ARef;
	double g = FOutletPipe->GetGamma(n);

	double p0 = p * pow(1 + (g - 1) / 2 * pow2(v / a), g / (g - 1));

	return p0;
}

double TAcousticCompressor::P2() {

	int n = FOutletPipe->getNin() - 1;

	return FOutletPipe->GetPresion(n);

}

double TAcousticCompressor::T10() {

	double p = FInletPipe->GetPresion(0);
	double a = FInletPipe->GetAsonido(0) * __cons::ARef;
	double v = FInletPipe->GetVelocidad(0) * __cons::ARef;
	double g = FInletPipe->GetGamma(0);
	double R = FInletPipe->GetRMezcla(0);

	double T0 = (pow2(a) + (g - 1) / 2 * pow2(v)) / g / R;

	return T0;

}

double TAcousticCompressor::T2() {

	int n = FOutletPipe->getNin() - 1;

	double a = FOutletPipe->GetAsonido(n) * __cons::ARef;
	double g = FOutletPipe->GetGamma(n);
	double R = FOutletPipe->GetRMezcla(n);

	return a * a / g / R;

}

double TAcousticCompressor::T20() {

	int n = FOutletPipe->getNin() - 1;

	double a = FOutletPipe->GetAsonido(n) * __cons::ARef;
	double g = FOutletPipe->GetGamma(n);
	double v = FOutletPipe->GetVelocidad(n) * __cons::ARef;
	double R = FOutletPipe->GetRMezcla(n);

	double T0 = (pow2(a) + (g - 1) / 2 * pow2(v)) / g / R;

	return T0;

}

double TAcousticCompressor::T1() {

	double a = FInletPipe->GetAsonido(0) * __cons::ARef;
	double g = FInletPipe->GetGamma(0);
	double R = FInletPipe->GetRMezcla(0);

	return a * a / g / R;

}

double TAcousticCompressor::CompRatio() {
	return P20() / P10();
}

void TAcousticCompressor::PutHeatPower(double Power) {
	FStatorVolume->PutHeatPower(Power);
}

void TAcousticCompressor::PutHeatPowerIn(double Power) {
	FRotorVolume->PutHeatPower(Power);
}

void TAcousticCompressor::AsignElementsID(int InletPipeID, int VoluteID, int OutletPipeID, int RotorVolumeID,
		int StatorVolumeID) {

	FInletPipeID = InletPipeID;
	FVoluteID = VoluteID;
	FOutletPipeID = OutletPipeID;
	FRotorVolumeID = RotorVolumeID;
	FStatorVolumeID = StatorVolumeID;
}

double TAcousticCompressor::CRCorrector() {

	double p = FVolute->GetPresion(0);
	double a = FVolute->GetAsonido(0) * __cons::ARef;
	double v = FVolute->GetVelocidad(0) * __cons::ARef;
	double g = FVolute->GetGamma(0);

	double p20out = p * pow(1 + (g - 1) / 2 * pow2(v / a), 2 * g / (g - 1));

	double Correction = p20out * P10() / FRotorVolume->getPressure() / P20();

	return Correction;

}

double TAcousticCompressor::EFCorrector(double rcorr, double rorig) {

	double g = FRotorVolume->getGamma();

	if(rorig <= 1.) {
		return 1;
	}

	double Correction = __units::degCToK(FRotorVolume->getTemperature()) / T10() * (pow(rorig * rcorr,
						(g - 1) / g) - 1) / (pow(rorig, (g - 1) / g) - 1);

	return Correction;

}

double TAcousticCompressor::MassFlow() {

	double p = FVolute->GetPresion(0);
	double a = FVolute->GetAsonido(0) * __cons::ARef;
	double v = FVolute->GetVelocidad(0) * __cons::ARef;
	double g = FVolute->GetGamma(0);
	double S = FVolute->GetArea(0);

	double Mass = __units::BarToPa(p) * g * v * S / a / a;

	return Mass;

}

double TAcousticCompressor::Efficiency() {
	double g = FVolute->GetGamma(0);
	double eff = T10() * (pow(CompRatio(), (g - 1) / g) - 1) / (T20() - T10());

	return eff;
}

double TAcousticCompressor::Din() {

	return FInletPipe->GetDiametro(0);
}
