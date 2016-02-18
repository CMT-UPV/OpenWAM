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
// OBJETO PARA EL CALCULO DE LA VALVULA DE EGR DEL MOTOR DW10 DE PSA PARA
// EL PROYECTO DEL ESTUDIO DEL EGR. CALCULA LOS COEFS DE DESCARGA Y TURBULENCIA
// A PARTIR DE LA POSICION DE LA VALVULA Y UNA CORRELACION
//
//              22 - MARZO - 2005
//---------------------------------------------------------------------------
#ifndef TEGRVH
#define TEGRVH

#include <string.h>
#include <cstdio>
#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include "Globales.h"

class TEGRV {
  private:

	FILE *FichEGRV;
//double FCDEntrante;
	double FCDSaliente;
	double FCTorbellino;

	double FMasaAireAdmitidaConsigna;

	int FNumeroDatos_TipoControl_Regimen;
	int FNumeroDatos_Mf;
	int FNumeroDatos_Regimen;

	double FError;     // Error en la masa de aire en el instante current
	double FError_ant; // Error en la masa de aire en el instante anterior
	double FErrorI;    // Integral del error para el control con el PID
	double FP;
	double FI;
	double FD;
	double FKc;
	double FKi;
	double FKd;

	double *FVector_Mf_mapa;
	double *FVector_TipoControl_Regimen_mapa;
	double *FVector_Regimen_mapa;

	double *FMapa_TipoControl;
	double **FMapa_MasaAire;

	nmTipoControl FTipoControl;

	int FCicloCerrado;

//Funcion de interpolacion
	double xit_(double vizq, double vder, double axid, double xif);

  public:

	double FCDEntrante;
	double getCDEntrante() {
		return FCDEntrante;
	}
	void PutCDEntrante(double valor) {
		FCDEntrante = valor;
	}
	;

	double getCDSaliente() {
		return FCDSaliente;
	}
	;

	double getCTorbellino() {
		return FCTorbellino;
	}
	;

	TEGRV();

	~TEGRV();

	void CalculaEGRV(double MasaFuel, double Regimen, double MasaAireAdmitida, double TiempoActual);

	void LeeDatosEntrada(char *Dir, FILE *fich);

	void IniciaEGRV(double cdEGR, int CicloCerrado);

	nmTipoControl DeterminacionTipoControl(double Regimen, double MasaFuel);

};
//---------------------------------------------------------------------------
#endif
