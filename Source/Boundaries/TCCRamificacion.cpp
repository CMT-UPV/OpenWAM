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

#include "TCCRamificacion.h"
//#include <cmath>
#include <iostream>
#include "TTubo.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCCRamificacion::TCCRamificacion(nmTypeBC TipoCC, int numCC,
	nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
	nmCalculoGamma GammaCalculation, bool ThereIsEGR) : TCondicionContorno(TipoCC,
	numCC, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {

	FTuboExtremo = NULL;

	FNodoFin = NULL;
	FIndiceCC = NULL;
	FEntropia = NULL;
	FSeccionTubo = NULL;
	FVelocity = NULL;
	FDensidad = NULL;
	FNumeroTubo = NULL;

	FCC = NULL;
	FCD = NULL;

	FMasaEspecie = NULL;

}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCCRamificacion::~TCCRamificacion() {

	if (FTuboExtremo != NULL)
		delete[]FTuboExtremo;
	if (FNodoFin != NULL)
		delete[]FNodoFin;
	if (FIndiceCC != NULL)
		delete[]FIndiceCC;
	if (FEntropia != NULL)
		delete[]FEntropia;
	if (FSeccionTubo != NULL)
		delete[]FSeccionTubo;
	if (FVelocity != NULL)
		delete[]FVelocity;
	if (FDensidad != NULL)
		delete[]FDensidad;
	if (FNumeroTubo != NULL)
		delete[]FNumeroTubo;

	if (FCC != NULL)
		delete[]FCC;
	if (FCD != NULL)
		delete[]FCD;

	if (FMasaEspecie != NULL)
		delete[]FMasaEspecie;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCRamificacion::AsignaTubos(int NumberOfPipes, TTubo **Pipe) {
	try {
		int i = 0;
		int ContadorTubosRamificacion;

		ContadorTubosRamificacion = 0;

		for (int i = 0; i < NumberOfPipes; i++) {
			if (Pipe[i]->getNodoIzq() == FNumeroCC || Pipe[i]->getNodoDer() == FNumeroCC) {
				ContadorTubosRamificacion++;
			}
		}

		FTuboExtremo = new stTuboExtremo[ContadorTubosRamificacion];
		FNodoFin = new int[ContadorTubosRamificacion];
		FIndiceCC = new int[ContadorTubosRamificacion];
		FCC = new double*[ContadorTubosRamificacion];
		FCD = new double*[ContadorTubosRamificacion];
		FEntropia = new double[ContadorTubosRamificacion];
		FSeccionTubo = new double[ContadorTubosRamificacion];
		FVelocity = new double[ContadorTubosRamificacion];
		FDensidad = new double[ContadorTubosRamificacion];
		FNumeroTubo = new int[ContadorTubosRamificacion];

		for (int i = 0; i < ContadorTubosRamificacion; i++) {
			FTuboExtremo[i].Pipe = NULL;
			FVelocity[i] = 0;
		}

		while (FNumeroTubosCC < ContadorTubosRamificacion && i < NumberOfPipes) {
			if (Pipe[i]->getNodoIzq() == FNumeroCC) {
				FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
				FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmLeft;
				FNodoFin[FNumeroTubosCC] = 0;
				FIndiceCC[FNumeroTubosCC] = 0;
				FNumeroTubo[FNumeroTubosCC] = Pipe[i]->getNumeroTubo() - 1;
				FCC[FNumeroTubosCC] = &(FTuboExtremo[FNumeroTubosCC].Beta);
				FCD[FNumeroTubosCC] = &(FTuboExtremo[FNumeroTubosCC].Landa);
				FSeccionTubo[FNumeroTubosCC] = Pi * pow
					(Pipe[i]->GetDiametro(FNodoFin[FNumeroTubosCC]), 2) / 4;
				FNumeroTubosCC++;
			}
			if (Pipe[i]->getNodoDer() == FNumeroCC) {
				FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
				FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmRight;
				FNodoFin[FNumeroTubosCC] = Pipe[i]->getNin() - 1;
				FIndiceCC[FNumeroTubosCC] = 1;
				FNumeroTubo[FNumeroTubosCC] = Pipe[i]->getNumeroTubo() - 1;
				FCC[FNumeroTubosCC] = &(FTuboExtremo[FNumeroTubosCC].Landa);
				FCD[FNumeroTubosCC] = &(FTuboExtremo[FNumeroTubosCC].Beta);
				FSeccionTubo[FNumeroTubosCC] = Pi * pow
					(Pipe[i]->GetDiametro(FNodoFin[FNumeroTubosCC]), 2) / 4;
				FNumeroTubosCC++;
			}
			i++;
		}

		// Inicializaci�n del transporte de especies qu�micas.
		FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
		FMasaEspecie = new double[FNumeroEspecies - FIntEGR];
		for (int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
			FFraccionMasicaEspecie[i] = FTuboExtremo[0].Pipe->GetFraccionMasicaInicial(i);
			// Se inicializa con el Pipe 0 de modo arbitrario.
		}

	}
	catch(Exception & N) {
		std::cout << "ERROR: TCCRamificacion::AsignaTubos en la condici�n de contorno: " << FNumeroCC <<
			std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCRamificacion::TuboCalculandose(int TuboActual) {
	try {
		FTuboActual = TuboActual;
		if (FTuboActual == 10000) {
			FTiempoActual = FTuboExtremo[0].Pipe->getTime1();
		}
		else {
			for (int i = 0; i < FNumeroTubosCC; i++) {
				if (FNumeroTubo[i] == FTuboActual) {
					FTiempoActual = FTuboExtremo[i].Pipe->getTime1();
				}
			}
		}
	}
	catch(Exception & N) {
		std::cout << "ERROR: TCCRamificacion::TuboCalculandose en la condici�n de contorno: " <<
			FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCRamificacion::CalculaCondicionContorno(double Time) {
	try {
		double sonido_supuesta_ad, sonido_ant_ad, entropia_entrante, corr_entropia;
		double suma1, suma2, sm1, sm2, sm3;
		int TuboCalculado;
		double DeltaT, MasaTotal = 0., g, m, FraccionMasicaAcum = 0.;
		// Necesarias para el c�lculo de especies en la BC.

		sonido_supuesta_ad = 0.;

		FTiempoActual = Time;
		DeltaT = FTiempoActual - FTiempoAnterior;
		FTiempoAnterior = FTiempoActual;

		if (FTuboActual == 10000) {
			TuboCalculado = FTuboActual;
			FGamma = FTuboExtremo[0].Pipe->GetGamma(FNodoFin[0]);
		}
		else {
			for (int i = 0; i < FNumeroTubosCC; i++) {
				if (FNumeroTubo[i] == FTuboActual) {
					TuboCalculado = i;
				}
			}
			FGamma = FTuboExtremo[TuboCalculado].Pipe->GetGamma(FNodoFin[TuboCalculado]);
		}
		// FGamma=FTuboExtremo[TuboCalculado].Pipe->GetGamma(FNodoFin[TuboCalculado]);
		FGamma1 = Gamma1(FGamma);
		FGamma3 = Gamma3(FGamma);
		FGamma4 = Gamma4(FGamma);

		for (int i = 0; i < FNumeroTubosCC; i++) {
			FEntropia[i] = FTuboExtremo[i].Entropia;
		}

		do {
			/* Determinaci�n de la velocidad del sonido en la ramificaci�n. */
			suma1 = 0.;
			suma2 = 0.;
			for (int i = 0; i < FNumeroTubosCC; i++) {
				suma1 = suma1 + (*FCC[i]) * FSeccionTubo[i] / pow(FEntropia[i], 2);
				suma2 = suma2 + FTuboExtremo[i].Entropia * FSeccionTubo[i] / pow(FEntropia[i], 2);
			}
			sonido_ant_ad = sonido_supuesta_ad;
			sonido_supuesta_ad = suma1 / suma2; // Velocity del sonido adimensionalizada (si las variables fuesen dimensionales).
			// Es una especie de promedio respecto de la entrop�a de cada tubo.

			/* Determinaci�n de la velocidad de cada tubo de la ramificaci�n. Esta
			velocidad ser� positiva si el flujo sale del tubo (tubo saliente) y
			negativa si el flujo entra en el tubo (tubo entrante). En realidad se trata
			de la velocidad solo para extremo derecho. Para el extremo izquierdo,est� multiplicada
			por un signo negativo. */
			for (int i = 0; i < FNumeroTubosCC; i++) {
				FVelocity[i] = (*FCC[i] - sonido_supuesta_ad * FTuboExtremo[i].Entropia) / FGamma3;
			}

			/* C�lculo de la entropia de los tubos entrantes (el flujo entra en ellos).
			Esta entropia es igual para todos ellos y se calcula como un balance del
			flujo que llega de los tubos salientes (de velocidad positiva). */
			sm1 = 0.;
			sm2 = 0.;
			sm3 = 0.;
			for (int i = 0; i < FNumeroTubosCC; i++) {
				sm3 = sm3 + FTuboExtremo[i].Entropia;
				if (FVelocity[i] > 2e-6) {
					sm1 = sm1 + FVelocity[i] * FSeccionTubo[i] * FEntropia[i];
					sm2 = sm2 + FVelocity[i] * FSeccionTubo[i];
				}
			}

			if (sm2 < 2e-6) {
				entropia_entrante = sm3 / FNumeroTubosCC;
			}
			else { /* Desde el punto de vista te�rico esta es la forma correcta. La
				f�rmula anterior es para evitar errores de indeterminaci�n si
				sm2 es muy peque�a,por lo que se acepta como aproximaci�n. */
				entropia_entrante = sm1 / sm2;
			}
			for (int i = 0; i < FNumeroTubosCC; i++) {
				FEntropia[i] = FTuboExtremo[i].Entropia;
				if (FVelocity[i] < 0) { // Flujo entrante al tubo
					FEntropia[i] = entropia_entrante;
				}
			}
		}
		while ((sonido_supuesta_ad - sonido_ant_ad) / (sonido_ant_ad + 0.01) > 1e-4);

		/* C�lculo de las caracter�sticas y la entrop�a en los extremos del tubo que se
		est� calculando, una vez resuelta la condici�n de contorno */
		if (TuboCalculado != 10000) {
			corr_entropia = FTuboExtremo[TuboCalculado].Entropia / FEntropia[TuboCalculado];
			*FCC[TuboCalculado] = (*FCC[TuboCalculado] + FGamma3 * FVelocity[TuboCalculado] *
				(corr_entropia - 1)) / corr_entropia;
			*FCD[TuboCalculado] = *FCC[TuboCalculado] - FGamma1 * FVelocity[TuboCalculado];
			FTuboExtremo[TuboCalculado].Entropia = FEntropia[TuboCalculado];

			double ason = (*FCC[TuboCalculado] + *FCD[TuboCalculado]) / 2;
			double Machx = fabs(FVelocity[TuboCalculado]) / ason;
			if (Machx > 1) {
				printf("Sonic condition in boundary: %d\n", FNumeroCC);
				// double Machy = Machx / fabs(Machx) * sqrt
				// ((pow(Machx, 2) + 2. / FGamma1) / (FGamma4 * pow(Machx, 2) - 1.));
				// double asonido = (*FCC[TuboCalculado] + *FCD[TuboCalculado]) / 2;
				// double Sonidoy = asonido * sqrt
				// ((FGamma3 * pow(Machx, 2) + 1.) / (FGamma3 * pow(Machy, 2) + 1.));
				//
				// double Velocidady = Sonidoy * Machy;
				ReduceSubsonicFlow(ason, FVelocity[TuboCalculado], FGamma);
				*FCC[TuboCalculado] = ason + FGamma3 * FVelocity[TuboCalculado];
				*FCD[TuboCalculado] = ason - FGamma3 * FVelocity[TuboCalculado];
			}
		}
		else {
			for (int i = 0; i < FNumeroTubosCC; i++) {
				corr_entropia = FTuboExtremo[i].Entropia / FEntropia[i];
				*FCC[i] = (*FCC[i] + FGamma3 * FVelocity[i] * (corr_entropia - 1)) / corr_entropia;
				*FCD[i] = *FCC[i] - FGamma1 * FVelocity[i];
				FTuboExtremo[i].Entropia = FEntropia[i];
				double Machx = fabs(*FCC[i] - *FCD[i]) / (*FCC[i] + *FCD[i]) * 2 / FGamma1;
				if (Machx > 1) {
					printf("Sonic condition in boundary: %d\n", FNumeroCC);
					double Machy = Machx / fabs(Machx) * sqrt
						((pow(Machx, 2) + 2. / FGamma1) / (FGamma4 * pow(Machx, 2) - 1.));
					double asonido = (*FCC[i] + *FCD[i]) / 2;
					double Sonidoy = asonido * sqrt
						((FGamma3 * pow(Machx, 2) + 1.) / (FGamma3 * pow(Machy, 2) + 1.));

					double Velocidady = Sonidoy * Machy;
					*FCC[i] = Sonidoy + FGamma3 * Velocidady;
					*FCD[i] = Sonidoy - FGamma3 * Velocidady;
				}
			}
		}

		// Transporte de especies qu�micas.
		for (int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
			FMasaEspecie[j] = 0.;
		}
		for (int i = 0; i < FNumeroTubosCC; i++) {
			if (FVelocity[i] > 0.) { // Flujo Saliente del tubo
				FDensidad[i] = pow(((*FCC[i] + *FCD[i]) / 2) / FTuboExtremo[i].Entropia, FGamma4);
				g = FDensidad[i] * FSeccionTubo[i] * FVelocity[i];
				m = g * DeltaT;
				MasaTotal += m;
				for (int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
					FMasaEspecie[j] += FTuboExtremo[i].Pipe->GetFraccionMasicaCC(FIndiceCC[i], j)
						* m;
				}
			}
		}

		if (MasaTotal != 0) {
			for (int j = 0; j < FNumeroEspecies - 2; j++) {
				FFraccionMasicaEspecie[j] = FMasaEspecie[j] / MasaTotal;
				FraccionMasicaAcum += FFraccionMasicaEspecie[j];
			}
			FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
			if (FHayEGR)
				FFraccionMasicaEspecie[FNumeroEspecies - 1] = FMasaEspecie[FNumeroEspecies - 1]
					/ MasaTotal;
		}

	}
	catch(Exception & N) {
		std::cout << "ERROR: TCCRamificacion::CalculaCondicionContorno en la condici�n de contorno: " <<
			FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#pragma package(smart_init)
