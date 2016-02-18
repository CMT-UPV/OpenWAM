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

#include "TTGV.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TTGV::TTGV() {
	FAEstator = 995.38; // en mm2
	FARotor = 1452.2;  // en mm2

	FKc1 = 0.1;
	FKi1 = 0.1;
	FKd1 = 0.1;
	FKc2 = 0.1;
	FKi2 = 0.1;
	FKd2 = 0.1;

	FError = 0.;
	FErrorI = 0.;

	FPosicionTurbina = 61.08;

	FDistanciasVastago = NULL;

	FPrimeraVez = false;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TTGV::~TTGV() {
	delete[] FDistanciasVastago;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TTGV::LeeDatosEntrada(char *Ruta, FILE *fich) {
	char *FileTGV;
	char *DatosTGV;

	try {

		for(int i = 0; i <= (int) strlen(Ruta); i++) {
			DatosTGV[i] = Ruta[i];
		}

		fscanf(fich, "%s ", &FileTGV);
		strcat(DatosTGV, FileTGV);

		FichTGV = fopen(DatosTGV, "r");
		if((FichTGV = fopen(DatosTGV, "r")) == NULL) {
			std::cout << "ERROR: Fichero de TGV no cargado";
		} else {
			fscanf(FichTGV, "%lf %lf %lf ", &FKc1, &FKi1, &FKd1);
			fscanf(FichTGV, "%lf %lf %lf ", &FKc2, &FKi2, &FKd2);
			fscanf(FichTGV, "%lf %lf %lf ", &FCorr0, &FCorr1, &FCorr2);
			fscanf(FichTGV, "%lf %lf %lf ", &FCorr4, &FCorr6, &FCorr8);
			fscanf(FichTGV, "%lf %lf ", &FCorr10, &FCorr12);
			fscanf(FichTGV, "%d %d ", &FNumeroDatos_Regimen, &FNumeroDatos_Mf);

			FVector_Mf_mapa = new double[FNumeroDatos_Mf];
			FVector_Regimen_mapa = new double[FNumeroDatos_Regimen];

			FMapa_PosicionTurbina = new double*[FNumeroDatos_Mf];
			FMapa_Padmision = new double*[FNumeroDatos_Mf];
			for(int i = 0; i < FNumeroDatos_Mf; i++) {
				FMapa_PosicionTurbina[i] = new double[FNumeroDatos_Regimen];
				FMapa_Padmision[i] = new double[FNumeroDatos_Regimen];
			}

			for(int i = 0; i < FNumeroDatos_Regimen; i++) {
				fscanf(FichTGV, "%lf ", &FVector_Regimen_mapa[i]);
			}
			for(int i = 0; i < FNumeroDatos_Mf; i++) {
				fscanf(FichTGV, "%lf ", &FVector_Mf_mapa[i]);
			}
			for(int i = 0; i < FNumeroDatos_Mf; i++) {
				for(int j = 0; j < FNumeroDatos_Regimen; j++) {
					fscanf(FichTGV, "%lf ", &FMapa_PosicionTurbina[i][j]);
				}
			}
			for(int i = 0; i < FNumeroDatos_Mf; i++) {
				for(int j = 0; j < FNumeroDatos_Regimen; j++) {
					fscanf(FichTGV, "%lf ", &FMapa_Padmision[i][j]);
				}
			}
			fclose(FichTGV);
		}

		FDistanciasVastago = new double[8];
		FDistanciasVastago[0] = 0;
		FDistanciasVastago[1] = 1;
		FDistanciasVastago[2] = 2;
		FDistanciasVastago[3] = 4;
		FDistanciasVastago[4] = 6;
		FDistanciasVastago[5] = 8;
		FDistanciasVastago[6] = 10;
		FDistanciasVastago[7] = 12;

	} catch(exception &N) {
		std::cout << "ERROR: TTGV::LeeDatosEntrada (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TTGV::CalculaTurbina(nmTipoControl TipoControl, double MasaFuel, double Regimen, double PresionAdmision,
						  double RelacionCinematica, double RelExp, double RegTurboCorr, double GastoCorr, double Tiempo) {
	try {
		int contador = 0, x1, x2;
		double Rendimiento1 = 0., Rendimiento2 = 0., AefecEstator = 0., AefecRotor = 0.;
		if(TipoControl == nmControlPadm) {

			MasaFuel = MasaFuel * 1e6; // en (mg/cc)

			FPresionAdmisionConsigna = Interpolacion_bidimensional(Regimen, MasaFuel, FVector_Mf_mapa, FVector_Regimen_mapa,
									   FMapa_Padmision, FNumeroDatos_Regimen, FNumeroDatos_Mf) / 1000.; //en (bar)

			FError_ant = FError;
			FErrorIAnt = FErrorI;
			FError = (PresionAdmision - FPresionAdmisionConsigna) / FPresionAdmisionConsigna;

			if(FError > 0) {
				FPrimeraVez = true;
			}
			if(Tiempo > 0) {
				if(FPrimeraVez) {
					FErrorI = 0.;
				} else {
					FErrorI = FErrorI + FError;
				}

				/* PID  Posicion Turbine */
				FP = -FKc1 * FError;
				FI = -FKi1 * (FErrorI);
				FD = -FKd1 * (FError - FError_ant);

				if(FPosicionTurbina + FP + FI + FD > FCierreMaximo) {
					if(FError < 0) {
						FErrorI = FErrorIAnt;
						FI = -FKi1 * (FErrorI);
					}/*else{
					 FErrorI=FErrorI+FError;
					 } */
				} else if(FPosicionTurbina + FP + FI + FD < 0.) {
					if(FError > 0) {
						FErrorI = FErrorIAnt;
						FI = -FKi1 * (FErrorI);
					}/*else{
					 FErrorI=FErrorI+FError;
					 } */
				}
			} else {
				FErrorI = FErrorI + FError;

				// PID  Posicion Turbine
				FP = -FKc2 * FError;
				FI = -FKi2 * (FErrorI);
				FD = -FKd2 * (FError - FError_ant);

				if(FPosicionTurbina + FP + FI + FD > FCierreMaximo) {
					if(FError < 0) {
						FErrorI = FErrorIAnt;
						FI = -FKi2 * (FErrorI);
					} //else{
					// FErrorI=FErrorI+FError;
					// }
				} else if(FPosicionTurbina + FP + FI + FD < 0.) {
					if(FError > 0) {
						FErrorI = FErrorIAnt;
						FI = -FKi2 * (FErrorI);
					}                    //else{
					// FErrorI=FErrorI+FError;
					// }
				}
			}

			FPosicionTurbina = FPosicionTurbina + FP + FI + FD; // Posicion de la turbina  en % (100% cerrada - 0% abierta)
			if(FPosicionTurbina > FCierreMaximo) {
				FPosicionTurbina = FCierreMaximo;
			} else if(FPosicionTurbina < 0.) {
				FPosicionTurbina = 0.;
			}

		} else if(TipoControl == nmControlMasaAire) {

			MasaFuel = MasaFuel * 1e6; // en (mg/cc)

			FPosicionTurbina = Interpolacion_bidimensional(Regimen, MasaFuel, FVector_Mf_mapa, FVector_Regimen_mapa,
							   FMapa_PosicionTurbina, FNumeroDatos_Regimen, FNumeroDatos_Mf); // en % (100% cerrada - 0% abierta)
		}

// CALCULO DEL RENDIMIENTO
		FDistanciaVastago = -(FPosicionTurbina - 100) / 8.3333; // en (mm)
		if(FDistanciaVastago > 12.) {
			FDistanciaVastago = 12.;
		}
		while(FDistanciasVastago[contador] <= FDistanciaVastago) {
			x1 = contador;
			x2 = contador + 1;
			contador++;
		}

		if(x1 == 0) {   // Distancia del vastago 0 mm
			Rendimiento1 = 1.6122 * pow3(RelacionCinematica) - 3.6641 * pow2(RelacionCinematica) + 2.0753 * RelacionCinematica;
			Rendimiento2 = 1.3275 * pow3(RelacionCinematica) - 3.42625 * pow2(RelacionCinematica) + 2.1998 * RelacionCinematica;
//  Rendimiento1=-1.5*(-12.5)*0.38*pow(RelacionCinematica,2.)-12.5*pow(RelacionCinematica,3.);
//  Rendimiento2=-1.5*(-12.7)*0.4*pow(RelacionCinematica,2.)-12.7*pow(RelacionCinematica,3.);
			FRendimiento = (Rendimiento1 - (FDistanciasVastago[x1] - FDistanciaVastago) * (Rendimiento1 - Rendimiento2) /
							(FDistanciasVastago[x1] - FDistanciasVastago[x2])) * FCorr0;

		} else if(x1 == 1) {   // Distancia del vastago 1 mm
			Rendimiento1 = 1.3275 * pow3(RelacionCinematica) - 3.42625 * pow2(RelacionCinematica) + 2.1998 * RelacionCinematica;
			Rendimiento2 = 0.6491229 * pow3(RelacionCinematica) - 2.646368 * pow2(RelacionCinematica) + 2.2409 * RelacionCinematica;

//  Rendimiento1=-1.5*(-12.7)*0.4*pow(RelacionCinematica,2.)-12.7*pow(RelacionCinematica,3.);
//  Rendimiento2=-1.5*(-9.2)*0.48*pow(RelacionCinematica,2.)-9.2*pow(RelacionCinematica,3.);
			FRendimiento = (Rendimiento1 - (FDistanciasVastago[x1] - FDistanciaVastago) * (Rendimiento1 - Rendimiento2) /
							(FDistanciasVastago[x1] - FDistanciasVastago[x2])) * FCorr1;
//  FRendimiento=(0.6491229*pow(RelacionCinematica,3.)-2.646368*pow(RelacionCinematica,2.)+2.2409*RelacionCinematica)*1.25;
		} else if(x1 == 2) {   // Distancia del vastago 2 mm
			Rendimiento1 = (0.6491229 * pow3(RelacionCinematica) - 2.646368 * pow2(RelacionCinematica) + 2.2409 *
							RelacionCinematica);
			Rendimiento2 = (-0.316970 * pow3(RelacionCinematica) - 1.5838 * pow2(RelacionCinematica) + 2.1257 * RelacionCinematica);

			//Rendimiento1=(-1.5*(-9.2)*0.48*pow(RelacionCinematica,2.)-9.2*pow(RelacionCinematica,3.))*1.25;
			//Rendimiento2=(-1.5*(-6.5)*0.58*pow(RelacionCinematica,2.)-6.5*pow(RelacionCinematica,3.))*1.1;
			FRendimiento = (Rendimiento1 - (FDistanciasVastago[x1] - FDistanciaVastago) * (Rendimiento1 - Rendimiento2) /
							(FDistanciasVastago[x1] - FDistanciasVastago[x2])) * FCorr2;
		} else if(x1 == 3) {   // Distancia del vastago 4 mm
			Rendimiento1 = (-0.316970 * pow3(RelacionCinematica) - 1.5838 * pow2(RelacionCinematica) + 2.1257 * RelacionCinematica);
			Rendimiento2 = (-0.20288 * pow3(RelacionCinematica) - 1.457777 * pow2(RelacionCinematica) + 2.0414 *
							RelacionCinematica);

			//Rendimiento1=(-1.5*(-6.5)*0.58*pow(RelacionCinematica,2.)-6.5*pow(RelacionCinematica,3.))*1.1;
			//Rendimiento2=-1.5*(-6.4)*0.59*pow(RelacionCinematica,2.)-6.4*pow(RelacionCinematica,3.);
			FRendimiento = (Rendimiento1 - (FDistanciasVastago[x1] - FDistanciaVastago) * (Rendimiento1 - Rendimiento2) /
							(FDistanciasVastago[x1] - FDistanciasVastago[x2])) * FCorr4;
		} else if(x1 == 4) {   // Distancia del vastago 6 mm
			Rendimiento1 = (-0.20288 * pow3(RelacionCinematica) - 1.457777 * pow2(RelacionCinematica) + 2.0414 *
							RelacionCinematica);
			Rendimiento2 = (-0.17608 * pow3(RelacionCinematica) - 1.37028 * pow2(RelacionCinematica) + 1.8979 * RelacionCinematica);

			//Rendimiento1=-1.5*(-6.4)*0.59*pow(RelacionCinematica,2.)-6.4*pow(RelacionCinematica,3.);
			//Rendimiento2=-1.5*(-5.35)*0.61*pow(RelacionCinematica,2.)-5.35*pow(RelacionCinematica,3.);
			FRendimiento = (Rendimiento1 - (FDistanciasVastago[x1] - FDistanciaVastago) * (Rendimiento1 - Rendimiento2) /
							(FDistanciasVastago[x1] - FDistanciasVastago[x2])) * FCorr6;
		} else if(x1 == 5) {   // Distancia del vastago 8 mm
			Rendimiento1 = (-0.17608 * pow3(RelacionCinematica) - 1.37028 * pow2(RelacionCinematica) + 1.8979 * RelacionCinematica);
			Rendimiento2 = (0.00635 * pow3(RelacionCinematica) - 1.5031 * pow2(RelacionCinematica) + 1.79457 * RelacionCinematica);

			//Rendimiento1=-1.5*(-5.35)*0.61*pow(RelacionCinematica,2.)-5.35*pow(RelacionCinematica,3.);
			//Rendimiento2=-1.5*(-5.2)*0.59*pow(RelacionCinematica,2.)-5.2*pow(RelacionCinematica,3.);
			FRendimiento = (Rendimiento1 - (FDistanciasVastago[x1] - FDistanciaVastago) * (Rendimiento1 - Rendimiento2) /
							(FDistanciasVastago[x1] - FDistanciasVastago[x2])) * FCorr8;
		} else if(x1 == 6) {   // Distancia del vastago 10 mm
			Rendimiento1 = (0.00635 * pow3(RelacionCinematica) - 1.5031 * pow2(RelacionCinematica) + 1.79457 * RelacionCinematica);
			Rendimiento2 = (0.39258 * pow3(RelacionCinematica) - 1.90361 * pow2(RelacionCinematica) + 1.7190 * RelacionCinematica);

			//Rendimiento1=-1.5*(-5.2)*0.59*pow(RelacionCinematica,2.)-5.2*pow(RelacionCinematica,3.);
			//Rendimiento2=-1.5*(-6.95)*0.5*pow(RelacionCinematica,2.)-6.95*pow(RelacionCinematica,3.);
			FRendimiento = (Rendimiento1 - (FDistanciasVastago[x1] - FDistanciaVastago) * (Rendimiento1 - Rendimiento2) /
							(FDistanciasVastago[x1] - FDistanciasVastago[x2])) * FCorr10;
		} else if(x1 == 7) {   // Distancia del vastago 12 mm
			FRendimiento = ((0.39258 * pow3(RelacionCinematica) - 1.90361 * pow2(RelacionCinematica) + 1.7190 * RelacionCinematica))
						   * FCorr12;

			//FRendimiento=-1.5*(-6.95)*0.5*pow(RelacionCinematica,2.)-6.95*pow(RelacionCinematica,3.);
		}

// CALCULO DEL COEFICIENTE DE DESCARGA DEL ESTATOR
		AefecEstator = -1.818 * pow2(FDistanciaVastago) + 54.4 * FDistanciaVastago + 73.88; // en (mm2)
		FCDStator = AefecEstator / FAEstator;

// CALCULO DEL COEFICIENTE DE DESCARGA DEL ROTOR
		AefecRotor = 59.9801 * GastoCorr - 2.79195 * RegTurboCorr + 0.695835 * RelExp * GastoCorr - 1.30223 * pow(GastoCorr,
					 2.) + 35.9154 + 10.1818 * pow(RelExp, 2.) - 29.9162 * RelExp; // en (mm2)
		FCDRotor = AefecRotor / FARotor;
		FCDRotor = FCDStator / 1.5;

		if(FCDStator < 0.) {
			FCDStator = 0.05;
		} else if(FCDStator > 0.8) {
			FCDStator = 0.8;
		}

		if(FCDRotor < 0.11) {
			FCDRotor = 0.11;
		} else if(FCDRotor > 0.8) {
			FCDRotor = 0.8;
		}

	} catch(exception &N) {
		std::cout << "ERROR: CalculaTurbina (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)

