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

#include "TCCExtremoInyeccion.h"
//#include <cmath>
#include <iostream>
#include "TTubo.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCCExtremoInyeccion::TCCExtremoInyeccion(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel,
		int numeroespecies, nmCalculoGamma GammaCalculation, bool ThereIsEGR) :
	TCondicionContorno(TipoCC, numCC, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {

	FTuboExtremo = NULL;
	FComposicion = NULL;
	FVelocity = 0;
	FSonido = 1;
	FPressure = 0;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCCExtremoInyeccion::~TCCExtremoInyeccion() {

	delete[] FTuboExtremo;
	if(FComposicion != NULL)
		delete[] FComposicion;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCExtremoInyeccion::ReadBoundaryData(const char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe,
		int nDPF, TDPF **DPF) {
	try {
		int i = 0;
		double fracciontotal = 0.;

		FTuboExtremo = new stTuboExtremo[1];
		FTuboExtremo[0].Pipe = NULL;

		while(FNumeroTubosCC < 1 && i < NumberOfPipes) {
			if(Pipe[i]->getNodoIzq() == FNumeroCC) {
				FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
				FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmLeft;
				FNodoFin = 0;
				FIndiceCC = 0;
				FCC = &(FTuboExtremo[FNumeroTubosCC].Beta);
				FCD = &(FTuboExtremo[FNumeroTubosCC].Landa);
				FSeccion = __geom::Circle_area(Pipe[i]->GetDiametro(FNodoFin));
				FNumeroTubosCC++;
			}
			if(Pipe[i]->getNodoDer() == FNumeroCC) {
				FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
				FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmRight;
				FNodoFin = Pipe[i]->getNin() - 1;
				FIndiceCC = 1;
				FCC = &(FTuboExtremo[FNumeroTubosCC].Landa);
				FCD = &(FTuboExtremo[FNumeroTubosCC].Beta);
				FSeccion = __geom::Circle_area(Pipe[i]->GetDiametro(FNodoFin));
				FNumeroTubosCC++;
			}
			i++;
		}

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%lf %lf %lf ", &FGastoIny, &FTemperaturaIny, &FInicioIny);
		fscanf(fich, "%lf ", &FDuracionIny);

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
		std::cout << "ERROR: TCCExtremoInyeccion::LeeExtremoInyeccion en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCExtremoInyeccion::ObtencionValoresInstantaneos(double Theta) {
	try {
		double ang0 = 0.;

		FTheta = Theta;

		ang0 = FTheta - FInicioIny;
		FAngap = ang0 - floor(ang0 / 360.) * 360.;

	} catch(exception &N) {
		std::cout << "ERROR: TCCExtremoInyeccion::ObtencionValoresInstantaneos en la condicion de contorno: " << FNumeroCC <<
				  std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCExtremoInyeccion::CalculaCondicionContorno(double Time) {
	try {
		double FraccionMasicaAcum = 0.;

		FGamma = FTuboExtremo[0].Pipe->GetGamma(FNodoFin);
		FRMezcla = FTuboExtremo[0].Pipe->GetRMezcla(FNodoFin);

		FGamma3 = __Gamma::G3(FGamma);
		FGamma5 = __Gamma::G5(FGamma);

		if(FAngap < FDuracionIny && FTheta > 720.) {
			FSonido = sqrt(FGamma * FRMezcla * __units::degCToK(FTemperaturaIny)) / __cons::ARef;
			FVelocity = (FSonido - *FCC) / FGamma3;
			FPressure = __units::PaToBar(FGastoIny * FRMezcla * __units::degCToK(FTemperaturaIny) /
										 (FVelocity * __cons::ARef * FSeccion)); // Cociente entre presion y la presion de referencia(1e5);
			if(FPressure < 0.0) {
				printf("ERROR: TCCExtremoInyeccion::CalculaCondicionContorno Velocity negativa en inyector %lf,Theta= %lf\n en la condicion de contorno: %d",
					   FVelocity * __cons::ARef, FTheta,
					   FNumeroCC);
				printf("       TCCExtremoInyeccion::CalculaCondicionContorno Presion negativa en inyector %lf\n", FPressure);
				throw Exception("");
			}
			*FCD = FSonido + FGamma3 * FVelocity;
			FTuboExtremo[0].Entropia = FSonido / pow(FPressure, FGamma5);

			//Transporte de especies quimicas.
			for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
				FFraccionMasicaEspecie[j] = FComposicion[j];
			}

		} else {
			// No hay inyeccion,se comporta como un extremo cerrado.
			*FCD = *FCC;

			//Transporte de especies quimicas.
			for(int j = 0; j < FNumeroEspecies - 2; j++) {
				FFraccionMasicaEspecie[j] = FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC, j);
				FraccionMasicaAcum += FFraccionMasicaEspecie[j];
			}
			FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
			if(FHayEGR)
				FFraccionMasicaEspecie[FNumeroEspecies - 1] = FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC, FNumeroEspecies - 1);
		}

	} catch(exception &N) {
		std::cout << "ERROR: TCCExtremoInyeccion::CalculaCondicionContorno en la condicion de contorno: " << FNumeroCC <<
				  std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//void TCCExtremoInyeccion::PutIniIny(double valor)
//{
//try
//{
//
//FInicioIny=valor;
//
//}
//catch(Exception &N)
//{
//std::cout << "ERROR: TCCExtremoInyeccion::PutIniIny en la condicion de contorno: " << FNumeroCC <<  std::endl;
//std::cout << "Tipo de error: " << N.what() << std::endl;
//throw Exception(N.what());
//}
//}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//void TCCExtremoInyeccion::PutDuracionIny(double valor)
//{
//try
//{
//
//FDuracionIny=valor;
//
//}
//catch(Exception &N)
//{
//std::cout << "ERROR: TCCExtremoInyeccion::PutDuracionIny en la condicion de contorno: " << FNumeroCC <<  std::endl;
//std::cout << "Tipo de error: " << N.what() << std::endl;
//throw Exception(N.what());
//}
//}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)
