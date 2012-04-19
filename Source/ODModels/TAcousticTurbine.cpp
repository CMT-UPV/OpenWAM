// ---------------------------------------------------------------------------

#pragma hdrstop

#include "TAcousticTurbine.h"

// ---------------------------------------------------------------------------

TAcousticTurbine::TAcousticTurbine(int InletPipeID, int VoluteID,
	int OutletPipeID) {
	FInletPipeID = InletPipeID;
	FVoluteID = VoluteID;
	FOutletPipeID = OutletPipeID;
}

TAcousticTurbine::TAcousticTurbine() {
}

TAcousticTurbine::~TAcousticTurbine() {
}

double TAcousticTurbine::T3() {

	double a = FInletPipe->GetAsonido(0) * ARef;
	double g = FInletPipe->GetGamma(0);
	double R = FInletPipe->GetRMezcla(0);

	return a * a / g / R;
}

double TAcousticTurbine::P3() {

	return FInletPipe->GetPresion(0);
}

double TAcousticTurbine::P30() {

	double p = FInletPipe->GetPresion(0);
	double a = FInletPipe->GetAsonido(0) * ARef;
	double v = FInletPipe->GetVelocidad(0) * ARef;
	double g = FInletPipe->GetGamma(0);

	double p3 = p * pow(1 + (g - 1) / 2 * pow2(v / a), 2 * g / (g - 1));

	return p3;
}

double TAcousticTurbine::ExpRatio() {

	return P30() / P4();

}

double TAcousticTurbine::P4() {

	int n = FOutletPipe->getNin() - 1;

	return FOutletPipe->GetPresion(n);

}

double TAcousticTurbine::T4() {

	int n = FOutletPipe->getNin() - 1;

	return pow2(FInletPipe->GetAsonido(n) * ARef) / FInletPipe->GetGamma(n)
	/ FInletPipe->GetRMezcla(0);

}

double TAcousticTurbine::MassIn() {

	return FInletPipe->GetDensidad(0) * FInletPipe->GetVelocidad(0)
	* ARef * SIn();
}



#pragma package(smart_init)
