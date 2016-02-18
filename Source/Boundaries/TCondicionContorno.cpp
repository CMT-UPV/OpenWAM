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

#include "TCondicionContorno.h"
//#include <cmath>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include "TTubo.h"

//---------------------------------------------------------------------------

TCondicionContorno::TCondicionContorno(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel,
									   int numeroespecies, nmCalculoGamma GammaCalculation, bool ThereIsEGR) {

	FCalculoEspecies = SpeciesModel;
	FNumeroEspecies = numeroespecies;
	FCalculoGamma = GammaCalculation;
	FFraccionMasicaEspecie = NULL;

//FTuboExtremo=NULL;

	FTipoCC = TipoCC;
	FNumeroCC = numCC + 1;
	FNumeroTubosCC = 0;

	FHayEGR = ThereIsEGR;
	if(FHayEGR)
		FIntEGR = 0;
	else
		FIntEGR = 1;

	FUnionDPF = false;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCondicionContorno::~TCondicionContorno() {

	if(FFraccionMasicaEspecie != NULL)
		delete[] FFraccionMasicaEspecie;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

stTuboExtremo TCondicionContorno::GetTuboExtremo(int i) {
	return FTuboExtremo[i];
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCondicionContorno::PutBeta(int i, double valor) {

	FTuboExtremo[i].Beta = valor;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCondicionContorno::PutLanda(int i, double valor) {

	FTuboExtremo[i].Landa = valor;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCondicionContorno::PutEntropia(int i, double valor) {

	FTuboExtremo[i].Entropia = valor;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//
////void TCondicionContorno::PutPosicionNodo(double valor)
//{
//FPosicionNodo=valor;
//}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TCondicionContorno::GetFraccionMasicaEspecie(int i) {
	try {
		return FFraccionMasicaEspecie[i];
	} catch(exception &N) {
		std::cout << "ERROR: TCondicionContorno::GetConcentracionEspecie en la condicion de contorno: " << FNumeroCC <<
				  std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
double TCondicionContorno::GetEntropia(int i) {

	return FTuboExtremo[i].Entropia;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TCondicionContorno::GetBeta(int i) {

	return FTuboExtremo[i].Beta;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TCondicionContorno::GetLanda(int i) {

	return FTuboExtremo[i].Landa;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCondicionContorno::CalculaCaracteristicas(double Time) {

	for(int i = 0; i < FNumeroTubosCC; i++) {
		double dt = Time - FTuboExtremo[i].Pipe->getTime0();
		if(FTuboExtremo[i].TipoExtremo == nmRight) {
			int nin = FTuboExtremo[i].Pipe->getNin() - 1;
			if(FTuboExtremo[i].Pipe->GetVelocidad(nin) > 0) {
				FTuboExtremo[i].Entropia = FTuboExtremo[i].Pipe->Interpola_Entropia(FTuboExtremo[i].TipoExtremo, dt);
			}
			FTuboExtremo[i].Landa = FTuboExtremo[i].Pipe->Interpola_Caracteristica(FTuboExtremo[i].Entropia, -1, nin, dt);
		} else {
			if(FTuboExtremo[i].Pipe->GetVelocidad(0) < 0) {
				FTuboExtremo[i].Entropia = FTuboExtremo[i].Pipe->Interpola_Entropia(FTuboExtremo[i].TipoExtremo, dt);
			}
			FTuboExtremo[i].Beta = FTuboExtremo[i].Pipe->Interpola_Caracteristica(FTuboExtremo[i].Entropia, 1, 0, dt);
		}
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)

