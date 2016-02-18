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
#ifndef TEstatorTurbinaH
#define TEstatorTurbinaH

#include <cstdio>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include <iostream>
//#include <cmath>

#include "TTipoValvula.h"

enum nmTipoEstator {
	nmStFijo = 0, nmStVariable = 1, nmStMapa = 2
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TEstatorTurbina: public TTipoValvula {
  private:

	nmTipoEstator FTipoEstator;
	double FCDEInicial;
	double FCDSInicial;
	double FDiametroRef;

	double FCDVbl;

	double FAreaEff;

	int FNumeroTurbina;
	int FNumeroEntrada;

	int FValvula;

  public:

	TEstatorTurbina(TEstatorTurbina *Origen, int valv);

	TEstatorTurbina();

	~TEstatorTurbina();

	int getNumeroTurbina() {
		return FNumeroTurbina;
	}
	;
	int getNumeroEntrada() {
		return FNumeroEntrada;
	}
	;
	nmTipoEstator getTipoEstator() {
		return FTipoEstator;
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

	void AsignaTurbina(double NTurb, double NEntr);

	void TipodeEstator(nmTipoEstator TipoEstator);

	void GetCDin(double Time);

	void GetCDout(double Time);
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif

