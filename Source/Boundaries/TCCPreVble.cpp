﻿/* --------------------------------------------------------------------------------*\
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

#include "TCCPreVble.h"
// #include <cmath>
#include <iostream>

#include "TTubo.h"
#include "TEntradaPulso.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCCPreVble::TCCPreVble(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel,
	int numeroespecies, nmCalculoGamma GammaCalculation, bool ThereIsEGR) : TCondicionContorno
(TipoCC, numCC, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {

	FTuboExtremo = NULL;
	FPulso = NULL;
	FComposicion = NULL;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCCPreVble::~TCCPreVble() {

	delete[]FTuboExtremo;

	if (FPulso != NULL)
		delete FPulso;

	if (FComposicion != NULL)
		delete[]FComposicion;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCPreVble::ReadBoundaryData(char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe,
	int nDPF, TDPF **DPF) {
	try {
		int i = 0;
		double fracciontotal = 0.;

		FTuboExtremo = new stTuboExtremo[1];
		FTuboExtremo[0].Pipe = NULL;

		FTiempo = 0;
		FPref = 1;

		while (FNumeroTubosCC < 1 && i < NumberOfPipes) {
			if (Pipe[i]->getNodoIzq() == FNumeroCC) {
				FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
				FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmLeft;
				FCC = &(FTuboExtremo[FNumeroTubosCC].Beta);
				FCD = &(FTuboExtremo[FNumeroTubosCC].Landa);
				FNodoFin = 0;
				FIndiceCC = 0;
				FNumeroTubosCC++;
			}
			if (Pipe[i]->getNodoDer() == FNumeroCC) {
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

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		FPulso = new TEntradaPulso();
		FPulso->LeeEntradaPulso(fich);

		// Inicializaci�n del transporte de especies qu�micas.
		FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
		FComposicion = new double[FNumeroEspecies - FIntEGR];
		for (int i = 0; i < FNumeroEspecies - 1; i++) {
			fscanf(fich, "%lf ", &FComposicion[i]);
			FFraccionMasicaEspecie[i] = FTuboExtremo[0].Pipe->GetFraccionMasicaInicial(i);
			fracciontotal += FComposicion[i];
		}
		if (FHayEGR) {
			FFraccionMasicaEspecie[FNumeroEspecies - 1] = FTuboExtremo[0]
				.Pipe->GetFraccionMasicaInicial(FNumeroEspecies - 1);
			if (FCalculoEspecies == nmCalculoCompleto) {
				if (FComposicion[0] > 0.2)
					FComposicion[FNumeroEspecies - 1] = 0.;
				else
					FComposicion[FNumeroEspecies - 1] = 1.;
			}
			else {
				if (FComposicion[0] > 0.5)
					FComposicion[FNumeroEspecies - 1] = 1.;
				else
					FComposicion[FNumeroEspecies - 1] = 0.;
			}
		}

		if (fracciontotal != 1.) {
			std::cout <<
				"ERROR: La fracci�n m�sica total no puede ser distinta de 1. Repasa la lectura en la condicion de contorno  " << FNumeroCC << std::endl;
			throw Exception(" ");
		}

		fgetpos(fich, &filepos);
		fclose(fich);

	}

	catch(Exception & N) {
		std::cout << "ERROR: TCCPreVble::LecturaPulso en la condici�n de contorno: " <<
			FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCPreVble::CalculaCondicionContorno(double Time) {
	try {
		double Pressure, Temp, Ason, FraccionMasicaAcum = 0.;

		FGamma = FTuboExtremo[0].Pipe->GetGamma(FNodoFin);
		double Gamma3 = (FGamma - 1) / 2;
		double R = FTuboExtremo[0].Pipe->GetRMezcla(FNodoFin);

		FTiempo = FTuboExtremo[0].Pipe->getTime1();
		FPulso->BusquedaInstante(FTiempo);

		Pressure = FPulso->InterpolaPresion();
		Temp = FPulso->InterpolaEntropia();

		double rel_CCon_entropia = *FCC / FTuboExtremo[0].Entropia;
		double yyy = pow(Pressure / FPref, Gamma5(FGamma));

		if (rel_CCon_entropia / yyy < 1.0) {

			Ason = sqrt(FGamma * R * (Temp + 273)) / ARef;
			double AA = Ason / pow(Pressure, Gamma5(FGamma));
			double U = (*FCC - Ason * FTuboExtremo[0].Entropia / AA) / Gamma3;
			*FCC = Ason + Gamma3 * U;
			*FCD = Ason - Gamma3 * U;
			FTuboExtremo[0].Entropia = Ason / pow(Pressure, Gamma5(FGamma));
		}
		else {
			Ason = FTuboExtremo[0].Entropia * pow(Pressure, Gamma5(FGamma));
			*FCD = 2 * Ason - *FCC;
		}

		// *FCD=(2.0*pow((1+Pressure)/FPref,Gamma5(FGamma))-1.0)*Entropia;
		// *FCC=Entropia;
		// FTuboExtremo[0].Entropia=Entropia;

		// Transporte de Especies Qu�micas
		if (*FCC > *FCD) { // Flujo saliente del tubo
			for (int j = 0; j < FNumeroEspecies - 2; j++) {
				FFraccionMasicaEspecie[j] = FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC, j);
				FraccionMasicaAcum += FFraccionMasicaEspecie[j];
			}
			FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
			if (FHayEGR)
				FFraccionMasicaEspecie[FNumeroEspecies - 1] = FTuboExtremo[0]
					.Pipe->GetFraccionMasicaCC(FIndiceCC, FNumeroEspecies - 1);
		}
		else if (*FCD > *FCC) { // Flujo entrante al tubo
			for (int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
				FFraccionMasicaEspecie[j] = FComposicion[j];
			}
		}
		/* La �ltima opci�n es que *FCC=*FCD. En este caso el flujo esta parado y la fracci�n masica
		de las especies permanece constante en dicho instante */

	}
	catch(Exception & N) {
		std::cout <<
			"ERROR: TCCPreVble::CalculaCondicionesContorno en la condici�n de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#pragma package(smart_init)
