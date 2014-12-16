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

#include "TControlK.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TControlK::TControlK() {

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TControlK::~TControlK() {

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TControlK::LeeDatosEntrada(char *Ruta, FILE *fich, double DiametroTubo) {
	char *FileK_Re;
	char *DatosK_Re;

	try {

		FDiametro = DiametroTubo;
		for (int i = 0; i <= (int) strlen(Ruta); i++) {
			DatosK_Re[i] = Ruta[i];
		}

		fscanf(fich, "%s ", &FileK_Re);
		strcat(DatosK_Re, FileK_Re);

		FichK_Re = fopen(DatosK_Re, "r");
		if ((FichK_Re = fopen(DatosK_Re, "r")) == NULL) {
			std::cout << "ERROR: Fichero de K vs Re no cargado";
		} else {
			fscanf(FichK_Re, "%d ", &FNumeroDatos);
			FVector_Re = new double[FNumeroDatos];
			FVector_K = new double[FNumeroDatos];

			for (int i = 0; i < FNumeroDatos; i++) {
				fscanf(FichK_Re, "%lf ", &FVector_Re[i]);
			}

			for (int i = 0; i < FNumeroDatos; i++) {
				fscanf(FichK_Re, "%lf ", &FVector_K[i]);
			}

			fclose(FichK_Re);
		}

	} catch (Exception &N) {
		std::cout << "ERROR: TControlK::LeeDatosEntrada (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TControlK::CalculaK(double velocidad, double temperatura, double presion,
		double R_mezcla) {
	try {
		double viscgas, rho, deltaRe, VariacionRe, K_Actual;
		int i = 0;

		viscgas = 1.4615e-6 * pow150(temperatura + 273.)
				/ (temperatura + 273. + 110.4);
		rho = presion * 1e5 / (temperatura + 273.) / R_mezcla;
		FRe = rho * velocidad * FDiametro / viscgas;

		FK = Interp1(FRe, FVector_Re, FVector_K, FNumeroDatos);

	} catch (Exception &N) {
		std::cout << "ERROR: TControlK::CalculaK (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)

