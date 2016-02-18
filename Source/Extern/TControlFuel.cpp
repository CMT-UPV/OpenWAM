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

#include "TControlFuel.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TControlFuel::TControlFuel() {
	FVector_Ma_mapa = NULL;
	FVector_Regimen_mapa = NULL;
	FMapa_Limitador_Humos = NULL;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TControlFuel::~TControlFuel() {

	if(FVector_Ma_mapa != NULL)
		delete FVector_Ma_mapa;
	if(FVector_Regimen_mapa != NULL)
		delete FVector_Regimen_mapa;

	if(FMapa_Limitador_Humos != NULL) {
		for(int i = 0; i < FNumeroDatos_Ma; i++) {
			delete FMapa_Limitador_Humos[i];
		}
		delete[] FMapa_Limitador_Humos;
	}

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TControlFuel::LeeDatosEntrada(char *Ruta, FILE *fich) {
	char *FileFuel;
	char *DatosFuel;

	try {

		for(int i = 0; i <= (int) strlen(Ruta); i++) {
			DatosFuel[i] = Ruta[i];
		}

		fscanf(fich, "%s ", &FileFuel);
		strcat(DatosFuel, FileFuel);

		FichFuel = fopen(DatosFuel, "r");
		if((FichFuel = fopen(DatosFuel, "r")) == NULL) {
			std::cout << "ERROR: Fichero de fuel no cargado";
		} else {
			fscanf(FichFuel, "%d %d ", &FNumeroDatos_Regimen, &FNumeroDatos_Ma);
			FVector_Ma_mapa = new double[FNumeroDatos_Ma];
			FVector_Regimen_mapa = new double[FNumeroDatos_Regimen];
			FMapa_Limitador_Humos = new double*[FNumeroDatos_Ma];
			for(int i = 0; i < FNumeroDatos_Ma; i++) {
				FMapa_Limitador_Humos[i] = new double[FNumeroDatos_Regimen];
			}

			for(int i = 0; i < FNumeroDatos_Regimen; i++) {
				fscanf(FichFuel, "%lf ", &FVector_Regimen_mapa[i]);
			}

			for(int i = 0; i < FNumeroDatos_Ma; i++) {
				fscanf(FichFuel, "%lf ", &FVector_Ma_mapa[i]);
			}

			for(int i = 0; i < FNumeroDatos_Ma; i++) {
				for(int j = 0; j < FNumeroDatos_Regimen; j++) {
					fscanf(FichFuel, "%lf ", &FMapa_Limitador_Humos[i][j]);
				}
			}

			fclose(FichFuel);
		}

	} catch(exception &N) {
		std::cout << "ERROR: LeeDatosEntrada Fuel (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*
 double TControlFuel::CalculaFuel(double TiempoActual)
 {
 try{
 int j=0, jmax=FNumeroDatos-1;
 double  FuelAct, deltaT, t;
 while(TiempoActual>FTiempo[j] && j<jmax){
 j++;
 }
 if(j==jmax){
 FuelAct=FFuel[jmax];
 }else{
 deltaT= FTiempo[j]-FTiempo[j-1];
 t=TiempoActual-FTiempo[j-1];
 FuelAct=xit_(FFuel[j-1],FFuel[j],deltaT,t);
 }
 return FuelAct;
 }
 catch(Exception &N)
 {
 std::cout << "ERROR: CalculaFuel" << std::endl;
 std::cout << "Tipo de error: " << N.what() << std::endl;
 throw Exception(N.what());
 }

 }

 */

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
double TControlFuel::xit_(double vizq, double vder, double axid, double xif) {
	try {
		double xx = 0., yy = 0.;
		double ret_val = 0.;

		xx = vder - vizq;
		if(axid != 0.) {
			yy = xx / axid * xif;
			ret_val = vizq + yy;
		} else {
			printf("ERROR: valores entrada xit\n");
			throw Exception("");
		}
		return ret_val;
	} catch(exception &N) {
		std::cout << "ERROR: xit_" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TControlFuel::CalculaFuel(double MasaPorAdmision, double Regimen, double TiempoActual) {
	try {
		/* Estrategia de inyeccion de combustible en transitorio de carga. */
		double MasaAire = 0., FuelMin = 0., FuelMax = 0., FuelLimitador = 0.;

		FuelMin = 4.32E-06; // en (kg/cc)
		FuelMax = 5.86E-05; // en (kg/cc)

// MasaPorAdmision ha de expresarse en mg

		if(TiempoActual < 10. * 120. / Regimen) {
			FuelAct = FuelMin;
			FFuelDeseado = FuelMin;
		}

		if(TiempoActual > 10. * 120. / Regimen) {
			FuelAct = FuelMax;
			FFuelDeseado = FuelMax;
			/* Calculo de combustible dado por el limitador de humos (kg/cc para WAM)*/
			FuelLimitador = Interpolacion_bidimensional(Regimen, MasaPorAdmision, FVector_Ma_mapa, FVector_Regimen_mapa,
							FMapa_Limitador_Humos, FNumeroDatos_Regimen, FNumeroDatos_Ma) / 1e6;

			if(FuelLimitador < FuelAct) {
				FuelAct = FuelLimitador;
			}
		}

		return FuelAct;

	} catch(exception &N) {
		std::cout << "ERROR: TControlFuel::CalculaFuel" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//
//double TControlFuel::CalculaFuel()
//{
//try
//{
//
///* Provisional para dW10b */
//return FuelAct;
//
//}
//catch(Exception &N)
//{
//std::cout << "ERROR: TControlFuel::CalculaFuel" << std::endl;
//std::cout << "Tipo de error: " << N.what() << std::endl;
//throw Exception(N.what());
//}
//}
//
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TControlFuel::IniciaFuel(double mfuel) {
	try {

		FuelAct = mfuel;

	} catch(exception &N) {
		std::cout << "ERROR: TControlFuel::IniciaFuel" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)

