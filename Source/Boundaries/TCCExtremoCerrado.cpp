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

#include "TCCExtremoCerrado.h"
#include "TTubo.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCCExtremoCerrado::TCCExtremoCerrado(nmTypeBC TipoCC, int numCC,
		nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
		nmCalculoGamma GammaCalculation, bool ThereIsEGR) :
		TCondicionContorno(TipoCC, numCC, SpeciesModel, numeroespecies,
				GammaCalculation, ThereIsEGR) {

	FTuboExtremo = NULL;

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCCExtremoCerrado::~TCCExtremoCerrado() {

	delete[] FTuboExtremo;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCExtremoCerrado::AsignaTubos(int NumberOfPipes, TTubo **Pipe) {
	try {
		int i = 0;

		FTuboExtremo = new stTuboExtremo[1];
		FTuboExtremo[0].Pipe = NULL;

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

// Inicializacion del transporte de especies quimicas.
		FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
		for (int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
			FFraccionMasicaEspecie[i] =
					FTuboExtremo[0].Pipe->GetFraccionMasicaInicial(i);
		}

	} catch (Exception &N) {
		std::cout
				<< "ERROR: TCCExtremoCerrado::AsignaTubos en la condicion de contorno: "
				<< FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCExtremoCerrado::CalculaCondicionContorno(double Time) {
	try {
		double FraccionMasicaAcum = 0.;

		*FCD = *FCC;

//Transporte de especies quimicas.
		for (int j = 0; j < FNumeroEspecies - 2; j++) {
			FFraccionMasicaEspecie[j] =
					FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC, j);
			FraccionMasicaAcum += FFraccionMasicaEspecie[j];
		}
		FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
		if (FHayEGR)
			FFraccionMasicaEspecie[FNumeroEspecies - 1] =
					FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC,
							FNumeroEspecies - 1);
	} catch (Exception &N) {
		std::cout
				<< "ERROR: TCCExtremoCerrado::CalculaCondicionesContorno en la condicion de contorno: "
				<< FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)

