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

#include "TControlInyeccion.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TControlInyeccion::TControlInyeccion() {

	FVector_Mf_mapa = NULL;
	FVector_Mf_mapaPrail = NULL;
	FVector_Prail_Regimen_mapa = NULL;
	FVector_Regimen_mapa = NULL;

	FMapa_SOI_Principal = NULL;
	FMapa_SOI_Piloto = NULL;
	FMapa_Combustible_Piloto = NULL;
	FMapa_Prail = NULL;
	FSOI = NULL;
	FMasaFuel = NULL;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TControlInyeccion::~TControlInyeccion() {

	if(FVector_Mf_mapa != NULL)
		delete FVector_Mf_mapa;
	if(FVector_Mf_mapaPrail != NULL)
		delete FVector_Mf_mapaPrail;
	if(FVector_Prail_Regimen_mapa != NULL)
		delete FVector_Prail_Regimen_mapa;
	if(FVector_Regimen_mapa != NULL)
		delete FVector_Regimen_mapa;

	for(int i = 0; i < FNumeroDatos_Mf; i++) {
		delete FMapa_Combustible_Piloto[i];
		delete FMapa_SOI_Principal[i];
		delete FMapa_SOI_Piloto[i];
		delete FMapa_Prail[i];
	}
	delete[] FMapa_Combustible_Piloto;
	delete[] FMapa_SOI_Principal;
	delete[] FMapa_SOI_Piloto;
	delete[] FMapa_Prail;
	delete[] FSOI;
	delete[] FMasaFuel;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TControlInyeccion::LeeDatosEntrada(char *Ruta, FILE *fich) {
	char *FileInyeccion;
	char *DatosInyeccion;

	try {
		for(int i = 0; i <= (int) strlen(Ruta); i++) {
			DatosInyeccion[i] = Ruta[i];
		}

		fscanf(fich, "%s ", &FileInyeccion);
		strcat(DatosInyeccion, FileInyeccion);

		FichInyeccion = fopen(DatosInyeccion, "r");
		if((FichInyeccion = fopen(DatosInyeccion, "r")) == NULL) {
			std::cout << "ERROR: Fichero de con datos del inyeccion no cargado";
		} else {
			fscanf(FichInyeccion, "%d %d %d %d ", &FNumeroDatos_Prail_Regimen, &FNumeroDatos_Mf_Prail, &FNumeroDatos_Mf,
				   &FNumeroDatos_Regimen);
			FVector_Mf_mapa = new double[FNumeroDatos_Mf];
			FVector_Mf_mapaPrail = new double[FNumeroDatos_Mf_Prail];
			FVector_Prail_Regimen_mapa = new double[FNumeroDatos_Prail_Regimen];
			FVector_Regimen_mapa = new double[FNumeroDatos_Regimen];

			FMapa_Combustible_Piloto = new double*[FNumeroDatos_Mf];
			FMapa_SOI_Principal = new double*[FNumeroDatos_Mf];
			FMapa_SOI_Piloto = new double*[FNumeroDatos_Mf];
			FMapa_Prail = new double*[FNumeroDatos_Mf_Prail];
			for(int i = 0; i < FNumeroDatos_Mf; i++) {
				FMapa_Combustible_Piloto[i] = new double[FNumeroDatos_Regimen];
				FMapa_SOI_Principal[i] = new double[FNumeroDatos_Regimen];
				FMapa_SOI_Piloto[i] = new double[FNumeroDatos_Regimen];
				FMapa_Prail[i] = new double[FNumeroDatos_Prail_Regimen];
			}

			for(int i = 0; i < FNumeroDatos_Prail_Regimen; i++) {
				fscanf(FichInyeccion, "%lf ", &FVector_Prail_Regimen_mapa[i]);
			}
			for(int i = 0; i < FNumeroDatos_Regimen; i++) {
				fscanf(FichInyeccion, "%lf ", &FVector_Regimen_mapa[i]);
			}
			for(int i = 0; i < FNumeroDatos_Mf; i++) {
				fscanf(FichInyeccion, "%lf ", &FVector_Mf_mapa[i]);
			}
			for(int i = 0; i < FNumeroDatos_Mf_Prail; i++) {
				fscanf(FichInyeccion, "%lf ", &FVector_Mf_mapaPrail[i]);
			}
			for(int i = 0; i < FNumeroDatos_Mf_Prail; i++) {
				for(int j = 0; j < FNumeroDatos_Prail_Regimen; j++) {
					fscanf(FichInyeccion, "%lf ", &FMapa_Prail[i][j]);
				}
			}
			for(int i = 0; i < FNumeroDatos_Mf; i++) {
				for(int j = 0; j < FNumeroDatos_Regimen; j++) {
					fscanf(FichInyeccion, "%lf ", &FMapa_SOI_Principal[i][j]);
				}
			}
			for(int i = 0; i < FNumeroDatos_Mf; i++) {
				for(int j = 0; j < FNumeroDatos_Regimen; j++) {
					fscanf(FichInyeccion, "%lf ", &FMapa_SOI_Piloto[i][j]);
				}
			}
			for(int i = 0; i < FNumeroDatos_Mf; i++) {
				for(int j = 0; j < FNumeroDatos_Regimen; j++) {
					fscanf(FichInyeccion, "%lf ", &FMapa_Combustible_Piloto[i][j]);
				}
			}
			fclose(FichInyeccion);
		}
		FSOI = new double[2];
		FMasaFuel = new double[2];

	} catch(exception &N) {
		std::cout << "ERROR: TControlInyeccion::LeeDatosEntrada (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TControlInyeccion::CalculaSistemaInyeccion(double MasaFuel, double Regimen) {
	try {
		double Prail = 0.;
// En esta funcion u otras que puedas necesitar debes programar las acciones
// a realizar para realizar el control sobre el sistema de inyeccion, de acuerdo
// a los requerimientos de ACT. Actualmente ACT necesita como datos para cada ciclo:
// 1.- Numero de inyecciones.
// 2.- Presion de inyeccion.
// 3.- ACT puede calcular hasta con 8 inyecciones de combustible. Para cada una de
//     ellas se ha de definir el CrankAngle de begining y la masa inyectada. Hay que dar
//     estos datos para las 8, de modo que aquellas que no son utilizadas deben tener
//     como angulo de begining el valor 180 y como masa inyectada 0, de acuerdo a lo que
//     el manual de ACT nos indica.

		FNumeroInyecciones = 2;
		MasaFuel = MasaFuel * 1e6; // En los mapas la masa de fuel esta en mg/cc. En ACT tambien,
		// luego se pasara el dato ya en estas unidades.

		Prail = Interpolacion_bidimensional(Regimen, MasaFuel, FVector_Mf_mapaPrail, FVector_Prail_Regimen_mapa, FMapa_Prail,
											FNumeroDatos_Prail_Regimen, FNumeroDatos_Mf_Prail);

		FPrail = Prail * 10;   // Presion del common-rail en bar

		/* SOI de la inyeccion principal */
		FSOI[1] = Interpolacion_bidimensional(Regimen, MasaFuel, FVector_Mf_mapa, FVector_Regimen_mapa, FMapa_SOI_Principal,
											  FNumeroDatos_Regimen, FNumeroDatos_Mf);

		/* Masa de fuel inyectada en la piloto */
		FMasaFuel[0] = Interpolacion_bidimensional(Regimen, MasaFuel, FVector_Mf_mapa, FVector_Regimen_mapa,
					   FMapa_Combustible_Piloto, FNumeroDatos_Regimen, FNumeroDatos_Mf);

		/* Masa de fuel inyectada en la principal */
		FMasaFuel[1] = MasaFuel - FMasaFuel[0];

		FDiferencia_SOI = Interpolacion_bidimensional(Regimen, MasaFuel, FVector_Mf_mapa, FVector_Regimen_mapa,
						  FMapa_SOI_Piloto, FNumeroDatos_Regimen, FNumeroDatos_Mf);

		/* SOI de la inyeccion piloto */
		FSOI[0] = FSOI[1] - FDiferencia_SOI;

		for(int i = 2; i < 8; i++) {
			FMasaFuel[i] = 0.;
			FSOI[i] = 180.;
		}

	} catch(exception &N) {
		std::cout << "ERROR: TControlInyeccion::CalculaSistemaInyeccion " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TControlInyeccion::GetSOI(int i) {
	try {

		return FSOI[i];

	} catch(exception &N) {
		std::cout << "ERROR: TControlInyeccion::GetSOI (DLL) " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TControlInyeccion::GetMasaFuel(int i) {
	try {

		return FMasaFuel[i];

	} catch(exception &N) {
		std::cout << "ERROR: TControlInyeccion::GetMasaFuel (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)

