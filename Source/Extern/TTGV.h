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

#ifndef TTGVH
#define TTGVH

#include <string.h>
#include <cstdio>
#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
//#include <cmath>
#include "Globales.h"

class TTGV {
  private:

//Variables para la determinacion de la correlacion utilizada en la turbina.
	FILE *FichTGV;
	int FNumeroDatos_Regimen;
	int FNumeroDatos_Mf;

	double *FVector_Mf_mapa;
	double *FVector_Regimen_mapa;
	double **FMapa_PosicionTurbina;
	double **FMapa_Padmision;

	double FPresionAdmisionConsigna;
//double FPosicionTurbina;
	double FDistanciaVastago;
	double *FDistanciasVastago;

	double FError;     // Error en la presion de admision en el instante current
	double FError_ant; // Error en la presion de admision en el instante anterior
	double FErrorI;    // Integral del error para el control con el PID
	double FErrorIAnt;
	double FP;
	double FI;
	double FD;
	double FKc1;
	double FKi1;
	double FKd1;
	double FKc2;
	double FKi2;
	double FKd2;

	double FCorr0;
	double FCorr1;
	double FCorr2;
	double FCorr4;
	double FCorr6;
	double FCorr8;
	double FCorr10;
	double FCorr12;

	double FAEstator; // area de referencia del estator en cm2
	double FARotor;  //  area de referencia del rotor en cm2
	double FCDStator;
	double FCDRotor;
	double FRendimiento;

//double FCierreMaximo;

	bool FPrimeraVez;

  public:

	double getCDStator() {
		return FCDStator;
	}
	;

	double getCDRotor() {
		return FCDRotor;
	}
	;

	double getRendimiento() {
		return FRendimiento;
	}
	;

	double FPosicionTurbina;
	double getPosicionTurbina() {
		return FPosicionTurbina;
	}
	void PutPosicionTurbina(double valor) {
		FPosicionTurbina = valor;
	}
	;

	double FCierreMaximo;
	double getCierreMaximo() {
		return FCierreMaximo;
	}
	void PutCierreMaximo(double valor) {
		FCierreMaximo = valor;
	}
	;

	TTGV();

	~TTGV();

	void LeeDatosEntrada(char *Ruta, FILE *fich);

	void CalculaTurbina(nmTipoControl TipoControl, double MasaFuel, double Regimen, double PresionAdmision,
						double RelacionCinematica, double RelExp, double RegTurboCorr, double GastoCorr, double Tiempo);

};

//---------------------------------------------------------------------------
#endif

