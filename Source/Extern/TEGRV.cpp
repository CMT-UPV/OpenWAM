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

#include "TEGRV.h"
//#include <cmath>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TEGRV::TEGRV() {

	FErrorI = 0;
	FError = 0;
	FError_ant = 0;

	FVector_Mf_mapa = NULL;
	FVector_TipoControl_Regimen_mapa = NULL;
	FVector_Regimen_mapa = NULL;

	FMapa_TipoControl = NULL;
	FMapa_MasaAire = NULL;

	FKc = 0.01;
	FKi = 0.008;
	FKd = 0.015;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TEGRV::~TEGRV() {

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TEGRV::LeeDatosEntrada(char *Ruta, FILE *fich) {
	char *FileEGR;
	char *DatosEGR;

	try {

		for(int i = 0; i <= (int) strlen(Ruta); i++) {
			DatosEGR[i] = Ruta[i];
		}

		fscanf(fich, "%s ", &FileEGR);
		strcat(DatosEGR, FileEGR);

		FichEGRV = fopen(DatosEGR, "r");
		if((FichEGRV = fopen(DatosEGR, "r")) == NULL) {
			std::cout << "ERROR: Fichero de EGR no cargado";
		} else {
			fscanf(FichEGRV, "%lf %lf %lf ", &FKc, &FKi, &FKd);
			fscanf(FichEGRV, "%d %d %d ", &FNumeroDatos_TipoControl_Regimen, &FNumeroDatos_Mf, &FNumeroDatos_Regimen);

			FVector_Mf_mapa = new double[FNumeroDatos_Mf];
			FVector_TipoControl_Regimen_mapa = new double[FNumeroDatos_TipoControl_Regimen];
			FVector_Regimen_mapa = new double[FNumeroDatos_Regimen];

			FMapa_TipoControl = new double[FNumeroDatos_TipoControl_Regimen];
			FMapa_MasaAire = new double*[FNumeroDatos_Mf];
			for(int i = 0; i < FNumeroDatos_Mf; i++) {
				FMapa_MasaAire[i] = new double[FNumeroDatos_Regimen];
			}

			for(int i = 0; i < FNumeroDatos_TipoControl_Regimen; i++) {
				fscanf(FichEGRV, "%lf ", &FVector_TipoControl_Regimen_mapa[i]);
			}
			for(int i = 0; i < FNumeroDatos_Regimen; i++) {
				fscanf(FichEGRV, "%lf ", &FVector_Regimen_mapa[i]);
			}
			for(int i = 0; i < FNumeroDatos_Mf; i++) {
				fscanf(FichEGRV, "%lf ", &FVector_Mf_mapa[i]);
			}
			for(int i = 0; i < FNumeroDatos_TipoControl_Regimen; i++) {
				fscanf(FichEGRV, "%lf ", &FMapa_TipoControl[i]);
			}
			for(int i = 0; i < FNumeroDatos_Mf; i++) {
				for(int j = 0; j < FNumeroDatos_Regimen; j++) {
					fscanf(FichEGRV, "%lf ", &FMapa_MasaAire[i][j]);
				}
			}
			fclose(FichEGRV);
		}

	} catch(exception &N) {
		std::cout << "ERROR: TEGRV::LeeDatosEntrada (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/*void TEGRV::CalculaEGRV(double TiempoActual)
 {
 try
 {
 // AQUI SE CALCULAN LOS COEFICIENTES DE DESCARGA Y TURBULENCIA

 int j=0, jmax=FNumeroDatos-1;
 double  EGRVAct,deltaT,t;
 while(TiempoActual>FTiempo[j] && j<jmax){
 j++;
 }
 if(j==jmax){
 EGRVAct=FEGRV[jmax];
 }else{
 deltaT= FTiempo[j]-FTiempo[j-1];
 t=TiempoActual-FTiempo[j-1];
 EGRVAct=xit_(FEGRV[j-1],FEGRV[j],deltaT,t);
 }
 if(EGRVAct<=50){
 FCDEntrante=0.0191*EGRVAct-0.6273;
 if (FCDEntrante > 1.0){
 FCDEntrante = 1.;
 }else if(FCDEntrante < 0.0){
 FCDEntrante = 0.;
 }
 }else{
 FCDEntrante=0.0015*EGRVAct+0.246;
 if (FCDEntrante > 1.0){
 FCDEntrante = 1.;
 }else if(FCDEntrante < 0.0){
 FCDEntrante = 0.;
 }
 }
 FCDSaliente=FCDEntrante;
 FCTorbellino=0;



 }
 catch(Exception &N)
 {
 std::cout << "ERROR: Calculo de la EGRV (DLL)" << std::endl;
 std::cout << "Tipo de error: " << N.what() << std::endl;
 throw Exception(N.what());
 }
 } */

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
double TEGRV::xit_(double vizq, double vder, double axid, double xif) {
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

nmTipoControl TEGRV::DeterminacionTipoControl(double Regimen, double MasaFuel) {
	try {
		double MasaFuel_mapadecisor = 0.;

		MasaFuel_mapadecisor = Interp1(Regimen, FVector_TipoControl_Regimen_mapa, FMapa_TipoControl,
									   FNumeroDatos_TipoControl_Regimen); // en (mg/cc)

		if(MasaFuel * 1e6 < MasaFuel_mapadecisor) {
			FTipoControl = nmControlMasaAire;
		} else {
			FTipoControl = nmControlPadm;
		}

		return FTipoControl;

	} catch(exception &N) {
		std::cout << "ERROR: TEGRV::DeterminacionTipoControl (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TEGRV::CalculaEGRV(double MasaFuel, double Regimen, double MasaAireAdmitida, double TiempoActual) {
	try {
// AQUI SE CALCULAN LOS COEFICIENTES DE DESCARGA Y TURBULENCIA

		if(FTipoControl == nmControlPadm) {
			if(TiempoActual < 13 * 120 / Regimen) {
				FCDEntrante = FCDEntrante / 2;
			} else {
				FCDEntrante = 0.;
			}
		} else if(FTipoControl == nmControlMasaAire) {
			MasaFuel = MasaFuel * 1e6; // en (mg/cc)
			FMasaAireAdmitidaConsigna = Interpolacion_bidimensional(Regimen, MasaFuel, FVector_Mf_mapa, FVector_Regimen_mapa,
										FMapa_MasaAire, FNumeroDatos_Regimen, FNumeroDatos_Mf); // (mg)

			FError_ant = FError;
			FError = (MasaAireAdmitida - FMasaAireAdmitidaConsigna) / FMasaAireAdmitidaConsigna;
			FErrorI = FErrorI + FError;

			/* PID */
			FP = FKc * FError;
			FI = FKi * (FErrorI);
			FD = FKd * (FError - FError_ant);
			FCDEntrante = FCDEntrante + FP + FI + FD;
		}
		if(FCDEntrante > 1.0) {
			FCDEntrante = 1.;
		} else if(FCDEntrante < 0.0) {
			FCDEntrante = 0.;
			FErrorI = 0;
		}
		FCDSaliente = FCDEntrante;
		FCTorbellino = 0.;

	} catch(exception &N) {
		std::cout << "ERROR: TEGRV::CalculaEGRV (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TEGRV::IniciaEGRV(double cdEGR, int CicloCerrado) {
	try {

//FEGRRef=EGRref;
		FCDEntrante = cdEGR;
		FCDSaliente = cdEGR;
		FCicloCerrado = CicloCerrado;

	} catch(exception &N) {
		std::cout << "ERROR: IniciaEGRV (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)
