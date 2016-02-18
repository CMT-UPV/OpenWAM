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
#ifndef TWasteGateH
#define TWasteGateH

#include <cstdio>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include <iostream>
//#include <cmath>
#include "Globales.h"

#include "TTipoValvula.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TTubo;

class TWasteGate: public TTipoValvula {
  private:
// CONSTANTES INICIALES
	TTubo *FTuboAdm;

	int FTuboControl;
	double FDistancia;
	double FPresCapMando;
	double FCoefC1;
	double FCoefC2;
	double FMasa;
	double FAmortiguamiento;
	double FRigidez;
	double FPrecarga;
	double FAreaDiaf;
	double FAreaPlato;
	double FDiametroRef;
	double FSentidoWG;
	double FDist;
	double FNodoAdm;

//VARIABLES DE CALCULO
	double FFuerza;
	double FX;
	double FdX;
	double FddX;

	int FValvula;

	bool FGraficasWG;
	bool FGrafLev;

  public:

	TWasteGate(TWasteGate *Origen, int valv);

	TWasteGate();

	~TWasteGate();

	double getDistancia() {
		return FDistancia;
	}
	;
	int getTuboControl() {
		return FTuboControl;
	}
	;
	double getNodoAdm() {
		return FNodoAdm;
	}
	;
	TTubo* getTuboAdm() {
		return FTuboAdm;
	}
	;
	double getDist() {
		return FDist;
	}
	;

	void LeeDatosIniciales(const char *FileWAM, fpos_t &filepos, int norden, bool HayMotor, TBloqueMotor *Engine);

	void CalculoNodoAdm(TTubo **Pipe);

	void CalculaCD(double Padm, double PTubo, double PDeposito, double deltaT);

	void LeeDatosGraficas(const char *FileWAM, fpos_t &filepos);

	void CabeceraGraficaINS(stringstream& insoutput, int lam);

	void ImprimeGraficaINS(stringstream& insoutput);

	void GetCDin(double Time);

	void GetCDout(double Time);
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif
