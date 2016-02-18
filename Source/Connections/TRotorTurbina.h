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
#ifndef TRotorTurbinaH
#define TRotorTurbinaH

#include <cstdio>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include <iostream>
//#include <cmath>

#include "TTipoValvula.h"

enum nmTipoRotor {
	nmRotFijo = 0, nmRotVariable = 1, nmRotMapa = 2
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TRotorTurbina: public TTipoValvula {
  private:

	nmTipoRotor FTipoRotor;
	double FCDEInicial;
	double FCDSInicial;

	double FCDVbl;

	double FAreaEff;

	double FDiametroRef;

	int FNumeroTurbina;

	int FValvula;

  public:

	TRotorTurbina(TRotorTurbina *Origen, int valv);

	TRotorTurbina();

	~TRotorTurbina();

	int getNumeroTurbina() {
		return FNumeroTurbina;
	}
	;
	nmTipoRotor getTipoRotor() {
		return FTipoRotor;
	}
	;
	int getValv() {
		return FValvula;
	}
	;
	double getCDescargaTubVol() {
		return FCDTubVol;
	}
	;
	void PutCDVbl(double valor) {
		FCDVbl = valor;
	}
	;
	void PutAreaEff(double valor) {
		FAreaEff = valor;
	}
	;

	void LeeDatosIniciales(const char *FileWAM, fpos_t &filepos, int norden, bool HayMotor, TBloqueMotor *Engine);

	void CalculaCD();

	void AsignaTurbina(int NTurb);

	void TipodeRotor(nmTipoRotor TipoRotor);

	void GetCDin(double Time);

	void GetCDout(double Time);
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif
