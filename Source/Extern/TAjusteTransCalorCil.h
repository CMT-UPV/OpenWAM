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
#ifndef TAjusteTransCalorCilH
#define TAjusteTransCalorCilH
//---------------------------------------------------------------------------

#include <cstring>
#include <cstdio>
#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include "Globales.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TAjusteTransCalorCil {
  private:

	double FErrorGastoAire;
	double FErrorT3;
	double FGastoAireref;
	double FT3ref;
//double FCaqAdm;
//double FCaqEsc;
//int FCicloCambioTC;
//double FCaqAdm2;
//double FCaqEsc2;
//int FCicloCambioTC2;
//double FCaqAdm3;
//double FCaqEsc3;
	int *FCiclo;
	double *FCadmision;
	double *FCescape;
	int FNumeroTramos;

  public:

	double FCaqAdm;
	double getCAdmision() {
		return FCaqAdm;
	}
	void putCAdmision(double valor) {
		FCaqAdm = valor;
	}
	;
	double FCaqEsc;
	double getCEscape() {
		return FCaqEsc;
	}
	void putCEscape(double valor) {
		FCaqEsc = valor;
	}
	;
	int FCicloCambioTC;
	int getCicloCambioTC() {
		return FCicloCambioTC;
	}
	void putCicloCambioTC(int valor) {
		FCicloCambioTC = valor;
	}
	;
	double FCaqAdm2;
	double getCAdmision2() {
		return FCaqAdm2;
	}
	void putCAdmision2(double valor) {
		FCaqAdm2 = valor;
	}
	;
	double FCaqEsc2;
	double getCEscape2() {
		return FCaqEsc2;
	}
	void putCEscape2(double valor) {
		FCaqEsc2 = valor;
	}
	;
	int FCicloCambioTC2;
	int getCicloCambioTC2() {
		return FCicloCambioTC2;
	}
	void putCicloCambioTC2(int valor) {
		FCicloCambioTC2 = valor;
	}
	;
	double FCaqAdm3;
	double getCAdmision3() {
		return FCaqAdm3;
	}
	void putCAdmision3(double valor) {
		FCaqAdm3 = valor;
	}
	;
	double FCaqEsc3;
	double getCEscape3() {
		return FCaqEsc3;
	}
	void putCEscape3(double valor) {
		FCaqEsc3 = valor;
	}
	;
	double getErrorGastoAire() {
		return FErrorGastoAire;
	}
	;

	TAjusteTransCalorCil();

	~TAjusteTransCalorCil();

	void CalculaTCC(double TiempoActual, double Regimen);

	void IniciaTCC(int NumeroTramos, int *Ciclo, double *CoefTCAdm, double *CoefTCEsc);

	double xit_(double vizq, double vder, double axid, double xif);

};
#endif
