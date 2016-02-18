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
#pragma hdrstop

#include "TCCPulso.h"
//#include <cmath>
#include <iostream>

#include "TTubo.h"
#include "TEntradaPulso.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCCPulso::TCCPulso(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
				   nmCalculoGamma GammaCalculation, bool ThereIsEGR) :
	TCondicionContorno(TipoCC, numCC, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {

	FTuboExtremo = NULL;
	FPulso = NULL;
	FComposicion = NULL;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCCPulso::~TCCPulso() {

	delete[] FTuboExtremo;

	if(FPulso != NULL)
		delete FPulso;

	if(FComposicion != NULL)
		delete[] FComposicion;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCPulso::ReadBoundaryData(const char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe, int nDPF,
								TDPF **DPF) {
	try {
		int i = 0;
		double fracciontotal = 0.;

		FTuboExtremo = new stTuboExtremo[1];
		FTuboExtremo[0].Pipe = NULL;

		FTiempo = 0;
		FPref = 1;

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

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		FPulso = new TEntradaPulso();
		FPulso->LeeEntradaPulso(fich);

// Inicializacion del transporte de especies quimicas.
		FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
		FComposicion = new double[FNumeroEspecies - FIntEGR];
		for(int i = 0; i < FNumeroEspecies - 1; i++) {
			fscanf(fich, "%lf ", &FComposicion[i]);
			FFraccionMasicaEspecie[i] = FTuboExtremo[0].Pipe->GetFraccionMasicaInicial(i);
			fracciontotal += FComposicion[i];
		}
		if(FHayEGR) {
			FFraccionMasicaEspecie[FNumeroEspecies - 1] = FTuboExtremo[0].Pipe->GetFraccionMasicaInicial(FNumeroEspecies - 1);
			if(FCalculoEspecies == nmCalculoCompleto) {
				if(FComposicion[0] > 0.2)
					FComposicion[FNumeroEspecies - 1] = 0.;
				else
					FComposicion[FNumeroEspecies - 1] = 1.;
			} else {
				if(FComposicion[0] > 0.5)
					FComposicion[FNumeroEspecies - 1] = 1.;
				else
					FComposicion[FNumeroEspecies - 1] = 0.;
			}
		}

		if(fracciontotal != 1.) {
			std::cout <<
					  "ERROR: La fraccion masica total no puede ser distinta de 1. Repasa la lectura en la condicion de contorno  " <<
					  FNumeroCC << std::endl;
			throw Exception(" ");
		}

		fgetpos(fich, &filepos);
		fclose(fich);

	}

	catch(exception &N) {
		std::cout << "ERROR: TCCPulso::LecturaPulso en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCPulso::CalculaCondicionContorno(double Time) {
	try {
		double Pressure, Entropia, FraccionMasicaAcum = 0.;

		FGamma = FTuboExtremo[0].Pipe->GetGamma(FNodoFin);

		FTiempo = FTuboExtremo[0].Pipe->getTime1();
		FPulso->BusquedaInstante(FTiempo);

		Pressure = FPulso->InterpolaPresion();
		Entropia = FPulso->InterpolaEntropia();

		*FCD = (2.0 * pow((1 + Pressure) / FPref, __Gamma::G5(FGamma)) - 1.0) * Entropia;
		*FCC = Entropia;
		FTuboExtremo[0].Entropia = Entropia;

// Transporte de Especies Quimicas
		if(*FCC > *FCD) {   // Flujo saliente del tubo
			for(int j = 0; j < FNumeroEspecies - 2; j++) {
				FFraccionMasicaEspecie[j] = FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC, j);
				FraccionMasicaAcum += FFraccionMasicaEspecie[j];
			}
			FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
			if(FHayEGR)
				FFraccionMasicaEspecie[FNumeroEspecies - 1] = FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC, FNumeroEspecies - 1);
		} else if(*FCD > *FCC) {   // Flujo entrante al tubo
			for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
				FFraccionMasicaEspecie[j] = FComposicion[j];
			}
		}
		/* La ultima opcion es que *FCC=*FCD. En este caso el flujo esta parado y la fraccion masica
		 de las especies permanece constante en dicho instante */

	} catch(exception &N) {
		std::cout << "ERROR: TCCPulso::CalculaCondicionesContorno en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)

