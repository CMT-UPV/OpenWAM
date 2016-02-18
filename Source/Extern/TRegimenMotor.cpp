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

#include "TRegimenMotor.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TRegimenMotor::TRegimenMotor() {

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TRegimenMotor::~TRegimenMotor() {
	if(FTiempo != NULL)
		delete[] FTiempo;

	if(FRegimen != NULL)
		delete[] FRegimen;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TRegimenMotor::LeeDatosEntrada(char *Ruta, FILE *fich) {
	char *FileRegimen;
	char *DatosRegimen;

	try {

		for(int i = 0; i <= (int) strlen(Ruta); i++) {
			DatosRegimen[i] = Ruta[i];
		}

		fscanf(fich, "%s ", &FileRegimen);
		strcat(DatosRegimen, FileRegimen);

		FichRegimen = fopen(DatosRegimen, "r");
		if((FichRegimen = fopen(DatosRegimen, "r")) == NULL) {
			std::cout << "ERROR: Fichero de regimen de giro no cargado";
		} else {
			FNumeroDatos = 0;
			double temp1 = 0.;
			double temp2 = 0.;
			while(!feof(FichRegimen)) {
				fscanf(FichRegimen, "%lf %lf ", &temp1, &temp2);
				FNumeroDatos++;
			}
			fclose(FichRegimen);
			FTiempo = new double[FNumeroDatos];
			FRegimen = new double[FNumeroDatos];
			FichRegimen = fopen(DatosRegimen, "r");
			int i = 0;
			while(!feof(FichRegimen)) {
				fscanf(FichRegimen, "%lf %lf ", &FTiempo[i], &FRegimen[i]);
				i++;
			}
			fclose(FichRegimen);

		}
	} catch(exception &N) {
		std::cout << "ERROR: LeeDatosEntrada de RegimenMotor (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TRegimenMotor::CalculaRegimen(double TiempoActual) {
	try {
// AQUI SE CALCULAN LOS COEFICIENTES DE DESCARGA Y TURBULENCIA

		int j = 0, jmax = FNumeroDatos - 1;
		double RegimenAct = 0., deltaT = 0., t = 0.;
		while(TiempoActual > FTiempo[j] && j < jmax) {
			j++;
		}
		if(j == jmax) {
			RegimenAct = FRegimen[jmax];
		} else {
			deltaT = FTiempo[j] - FTiempo[j - 1];
			t = TiempoActual - FTiempo[j - 1];
			RegimenAct = xit_(FRegimen[j - 1], FRegimen[j], deltaT, t);
		}

		FRegimenMotor = RegimenAct;

	} catch(exception &N) {
		std::cout << "ERROR: Calculo del Regimen del Engine(DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------

double TRegimenMotor::xit_(double vizq, double vder, double axid, double xif) {
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

#pragma package(smart_init)
