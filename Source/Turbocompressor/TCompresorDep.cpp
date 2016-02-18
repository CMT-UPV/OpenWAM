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
#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
//#include <cmath>
#include "TCompresorDep.h"

#include "TDeposito.h"
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCompresorDep::TCompresorDep(int i, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
							 nmCalculoGamma GammaCalculation, bool ThereIsEGR) :
	TCompresor(i, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {

	FModeloCompresor = nmCompPlenums;
	Mapa = new TMapaComp(FNumeroCompresor);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCompresorDep::~TCompresorDep() {
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompresorDep::LeeCompresor(const char *FileWAM, fpos_t &filepos) {
	int modelo = 0;
	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		Mapa->LeeMapa(fich);

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TCompresorDep::LeeCompresor en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: LeeCompresor en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompresorDep::RelacionDepositoCompresor(TDeposito *DepositoRot, TDeposito *DepositoEst) {
	try {

		FDepositoRot = DepositoRot;
		FDepositoEst = DepositoEst;

		FDepositoRot->AsignaCompresor(this, -1);
		FDepositoEst->AsignaCompresor(this, 1);

	} catch(exception & N) {
		std::cout << "ERROR: TCompTubos::RelacionTubos en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: LeeCompresor en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

void TCompresorDep::Initialize() {

	FTemperatura10 = __units::degCToK(FDepositoRot->getTemperature());

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// ----------------- CalculaGasto -----------------------------------------//
// Calculo del flujo que transcurre a travas de los 2 depositos del        //
// compresor                                                               //
// ---------------------------------------------------------------------------

void TCompresorDep::CalculaGasto(double TrabajoInsTurbina, double TiempoActual) {
	int NumGastos = 0;
	double Aux0 = 0.;
	double Aux1 = 0.;
	double Masa0 = 0.;
	double Masa1 = 0.;
	double MasaX = 0.;
	double RC0 = 0.;
	// double RC1 = 0.;
	double RCX = 0.;
	double ErrorMasa = 0.;
	double ErrorRC = 0.;
	double *RelComp;
	double *Gastos;
	double *Trab;
	double *Rnd;
	double *DeltaWork;
	double Min = 0.;
	int Contador = 0;
	int *PuntosDeCruce;
	int PuntoMin = 0;
	double DescorrigeGasto = 0.;
	try {

		// NO ESTAN PUESTAS LAS ESPECIES !!!!!!! SOLO ESTA HECHO PARA QUE COMPILE.
		FGamma = FDepositoRot->getGamma(); // Se ha de hacer una media de entrada y salida
		FRMezcla = FDepositoRot->getR();
		FGamma4 = __Gamma::G4(FGamma);
		FGamma5 = __Gamma::G5(FGamma);

		FPresion10 = FDepositoRot->getPressure();
		FPresion20 = FDepositoEst->getPressure();
		FTemperatura10 = pow2(FDepositoRot->getSpeedsound() * __cons::ARef) / FRMezcla / FGamma;
		FRelacionCompresion = FPresion20 / FPresion10;
		FDeltaTiempo = TiempoActual - FTiempo0;
		FTiempo0 = TiempoActual;
		DescorrigeGasto = __units::BarToPa(FPresion10) / Mapa->getPresionRef() / sqrt(FTemperatura10 / Mapa->getTempRef());

		if(FRelacionCompresion <= 1.) {
			FGastoCorregido = Mapa->getGastoRelComp1();
			FGastoCompresor = Mapa->getGastoRelComp1() * DescorrigeGasto;
			FRendimiento = Mapa->EvaluaRendSplines(Mapa->getGastoRelComp1());
			FTrabajo = FGastoCompresor * FRMezcla * FGamma4 / 2 * FTemperatura10 * (pow(FRelacionCompresion,
					   2. * FGamma5) - 1.) / FRendimiento;
			FTempGasto = FTemperatura10
						 /* +(pow(FRelacionCompresion,Gamma5*2)-1)*FTemperatura10/FRendimiento */;
		} else {
			PuntosDeCruce = new int[Mapa->getNumPuntos()];
			Aux0 = Mapa->GetRelCompInt(0) - FRelacionCompresion;
			NumGastos = 0;
			for(int i = 0; i < Mapa->getNumPuntos(); i++) {
				Aux1 = Mapa->GetRelCompInt(i + 1) - FRelacionCompresion;
				if(Aux0 * Aux1 < 0) {
					PuntosDeCruce[NumGastos] = i;
					++NumGastos;
				}
				Aux0 = Aux1;
			}
			if(NumGastos == 0) {
				FRendimiento = Mapa->EvaluaRendSplines(Mapa->getGastoBombeo());
				FTrabajo = Mapa->getGastoBombeo() * DescorrigeGasto * FRMezcla * FGamma4 / 2 * FTemperatura10 * (pow(
							   Mapa->getRelCompBombeo(), 2. * FGamma5) - 1.) / FRendimiento;
				FTempGasto = FTemperatura10 + (pow(FRelacionCompresion, FGamma5 * 2) - 1) * FTemperatura10 / FRendimiento;
				FGastoCompresor = 0.;
			} else {
				RelComp = new double[NumGastos];
				Gastos = new double[NumGastos];
				Trab = new double[NumGastos];
				Rnd = new double[NumGastos];
				for(int i = 0; i < NumGastos; i++) {
					ErrorMasa = 1.;
					ErrorRC = 1.;
					Contador = 0;
					Masa0 = Mapa->GetGastoInt(PuntosDeCruce[0]);
					RC0 = Mapa->GetRelCompInt(PuntosDeCruce[0]);
					Masa1 = Mapa->GetGastoInt(PuntosDeCruce[0] + 1);
					// RC1=Mapa->GetRelCompInt(PuntosDeCruce[0]+1);
					while(ErrorMasa > 1e-5 && ErrorRC > 1e-5 && Contador < 100) {
						MasaX = (Masa0 + Masa1) / 2.;
						RCX = Mapa->EvaluaRCHermite(MasaX);
						if((RC0 - FRelacionCompresion) * (RCX - FRelacionCompresion) < 0) {
							Masa1 = MasaX;
							// RC1=RCX;
							ErrorMasa = fabs(Masa0 - MasaX);
						} else {
							ErrorMasa = fabs(Masa0 - MasaX);
							Masa0 = MasaX;
							RC0 = RCX;
						}
						ErrorRC = fabs(RCX - FRelacionCompresion);
						++Contador;
					}
					RelComp[i] = RCX;
					Gastos[i] = MasaX;
					Rnd[i] = Mapa->EvaluaRendSplines(Gastos[i]);
					Trab[i] = Gastos[i] * DescorrigeGasto * FRMezcla * FGamma4 / 2. * FTemperatura10 * (pow(RelComp[i],
							  FGamma5 * 2.) - 1.) / Rnd[i];
				}
				if(NumGastos > 1) {
					DeltaWork = new double[NumGastos];
					for(int i = 0; i < NumGastos; i++) {
						DeltaWork[i] = fabs(Trab[i] - TrabajoInsTurbina);
					}
					PuntoMin = 0;
					Min = DeltaWork[0];
					for(int j = 1; j < NumGastos; j++) {
						if(DeltaWork[j] < Min) {
							Min = DeltaWork[j];
							PuntoMin = j;
						}
					}
					FGastoCorregido = Gastos[PuntoMin];
					FGastoCompresor = Gastos[PuntoMin] * DescorrigeGasto;
					FTrabajo = Trab[PuntoMin];
					FTempGasto = FTemperatura10 + (pow(RelComp[PuntoMin], FGamma5 * 2) - 1) * FTemperatura10 / Rnd[PuntoMin];
					FRendimiento = Rnd[PuntoMin];
				} else {
					FGastoCorregido = Gastos[0];
					FGastoCompresor = Gastos[0] * DescorrigeGasto;
					FTrabajo = Trab[0];
					FTempGasto = FTemperatura10 + (pow(RelComp[0], FGamma5 * 2) - 1) * FTemperatura10 / Rnd[0];
					FRendimiento = Rnd[0];
				}
			}
		}
		FASonidoSalida = sqrt(FGamma * FRMezcla * FTempGasto);
		FTrabajo *= FDeltaTiempo;
		FPotencia = FTrabajo / FDeltaTiempo;
		FTrabajoPaso += FTrabajo;
		FDeltaTPaso += FDeltaTiempo;
		FRegimenCorregido = Mapa->getRegimenCorregido();

	} catch(exception & N) {
		std::cout << "ERROR: TCompresorDep::CalculaGasto en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: CalculaGasto en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#pragma package(smart_init)
