// ---------------------------------------------------------------------------

#pragma hdrstop

#include "TCCExternalConnection.h"
#include "TTubo.h"

// ---------------------------------------------------------------------------

TCCExternalConnection::TCCExternalConnection(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel,
		int numeroespecies, nmCalculoGamma GammaCalculation, bool ThereIsEGR) :
	TCondicionContorno(TipoCC, numCC, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {

	FTuboExtremo = NULL;
	// FComposicion = NULL;

	FUExt.resize(2);
	FTExt.resize(2);
	FPExt.resize(2);
	FDExt.resize(2);
	FAExt.resize(2);
}

TCCExternalConnection::~TCCExternalConnection() {
}

void TCCExternalConnection::UpdateCurrentExternalProperties(double U0, double U1, double T0, double T1, double P0,
		double P1, double t) {

	FUExt[0] = U0;
	FUExt[1] = U1;
	FTExt[0] = T0;
	FTExt[1] = T1;
	FPExt[0] = P0;
	FPExt[1] = P1;

	FCurrentTime = t;
}

void TCCExternalConnection::AsignGeometricalData(double D0, double D1, double deltaX) {

	FDExt[0] = D0;
	FDExt[1] = D1;
	FAExt[0] = __geom::Circle_area(D0);
	FAExt[1] = __geom::Circle_area(D1);

	FDeltaX = deltaX;
}

void TCCExternalConnection::ExternalCharacteristics(double Time) {

	// Calculo Entropia
	double x = 0., Px = 0., Tx = 0., Ux = 0., Ax = 0.;
	double Deltat = Time - FCurrentTime;

	double g = __Gamma::G;
	double gg = __Gamma::G_5;

	double A0 = sqrt(g * __R::Air * FTExt[0]);
	double A1 = sqrt(g * __R::Air * FTExt[1]);

	double AA0 = A0 / pow(FPExt[0], gg);
	double AA1 = A1 / pow(FPExt[1], gg);

	if(FUExt[0] > 0) {
		x = FUExt[0] * FDeltaX / (FDeltaX / Deltat + FUExt[0] - FUExt[1]);

		FA_AExt = (AA0 * (FDeltaX - x) + AA1 * x) / FDeltaX / __cons::ARef;

	} else {
		FA_AExt = AA0 / __cons::ARef;
	}
	double UWave0 = A0 + FUExt[0];
	double UWave1 = A1 + FUExt[1];

	x = UWave0 * FDeltaX / (FDeltaX / Deltat + UWave0 - UWave1);

	Ux = (FUExt[0] * (FDeltaX - x) + FUExt[1] * x) / FDeltaX;
	Ax = (A0 * (FDeltaX - x) + A1 * x) / FDeltaX;

	double deltaEnt = Ax * (AA0 - FA_AExt * __cons::ARef) / FA_AExt / __cons::ARef;

	FK_CExt = (Ax + (g - 1) / 2 * Ux + deltaEnt) / __cons::ARef;

}

void TCCExternalConnection::CalculaCondicionContorno(double Time) {

	double flujo = 0.;

	ExternalCharacteristics(Time);

	flujo = (FK_CExt / FA_AExt) / (*FCC / FTuboExtremo[0].Entropia);

	if(flujo < 0.999995) {

		double A = FTuboExtremo[0].Entropia / (FTuboExtremo[0].Entropia + FA_AExt) * (*FCC + FK_CExt);
		FT_Boundary = pow2(A * __cons::ARef) / __R::Air / __Gamma::G;
		FU_Boundary = (A - *FCC) / __Gamma::G_3;
		*FCD = A + __Gamma::G_3 * FU_Boundary;
		FU_Boundary *= __cons::ARef;
		FP_Boundary = pow(A / FTuboExtremo[0].Entropia, __Gamma::G_4);
	} else if(flujo > 1.000005) {
		double A = FA_AExt / (FTuboExtremo[0].Entropia + FA_AExt) * (*FCC + FK_CExt);
		FT_Boundary = pow2(A * __cons::ARef) / __R::Air / __Gamma::G;
		FU_Boundary = (FK_CExt - A) / __Gamma::G_3;
		*FCD = A + __Gamma::G_3 * FU_Boundary;
		*FCC = A - __Gamma::G_3 * FU_Boundary;
		FTuboExtremo[0].Entropia = FA_AExt;
		FU_Boundary *= __cons::ARef;
		FP_Boundary = pow(A / FTuboExtremo[0].Entropia, __Gamma::G_4);

	} else {
		double A = *FCC;
		FT_Boundary = pow2(A * __cons::ARef) / __R::Air / __Gamma::G;
		FU_Boundary = 0;
		*FCD = *FCC;
		FP_Boundary = pow(A / FTuboExtremo[0].Entropia, __Gamma::G_4);
	}

}

void TCCExternalConnection::ReadBoundaryData(const char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe,
		int nDPF, TDPF **DPF) {

	int i = 0;

	FTuboExtremo = new stTuboExtremo[1];
	FTuboExtremo[0].Pipe = NULL;

	while(FNumeroTubosCC < 1 && i < NumberOfPipes) {
		if(Pipe[i]->getNodoIzq() == FNumeroCC) {
			FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
			FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmLeft;
			FCC = &(FTuboExtremo[FNumeroTubosCC].Beta);
			FCD = &(FTuboExtremo[FNumeroTubosCC].Landa);
			FNodoFin = 0;
			FIndiceCC = 0;
			FNumeroTubosCC++;
		}
		if(Pipe[i]->getNodoDer() == FNumeroCC) {
			FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
			FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmRight;
			FCC = &(FTuboExtremo[FNumeroTubosCC].Landa);
			FCD = &(FTuboExtremo[FNumeroTubosCC].Beta);
			FNodoFin = FTuboExtremo[FNumeroTubosCC].Pipe->getNin() - 1;
			FIndiceCC = 1;
			FNumeroTubosCC++;
		}
		i++;
	}

	// Inicializacion del transporte de especies quimicas.
	FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
	for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
		FFraccionMasicaEspecie[i] = FTuboExtremo[0].Pipe->GetFraccionMasicaInicial(i);
	}

	FILE *fich = fopen(FileWAM, "r");
	fsetpos(fich, &filepos);

	fscanf(fich, "%d ", &FID);

	fgetpos(fich, &filepos);
	fclose(fich);

}

void TCCExternalConnection::LoadNewData(double* p, double* T, double* u) {

	*p = FP_Boundary;
	*T = FT_Boundary;
	*u = FU_Boundary;
}

#pragma package(smart_init)
