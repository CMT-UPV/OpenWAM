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
#ifndef TTipoValvulaH
#define TTipoValvulaH

#include <cstdio>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include <iostream>

#include "Globales.h"
//#include "interp_1d.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

enum nmTipoValvula {
	nmCDFijo = 0, nmValvula4T = 1, nmLamina = 2, nmDiscoRotativo = 3, nmLumbrera2T = 4, nmValvulaContr = 5, nmWasteGate = 6, nmStator = 7, nmRotor = 8, nmCalcExtern = 9, nmMariposa = 10
};

class TTubo;
class TCilindro;
class TDeposito;
class TBloqueMotor;

class TTipoValvula {
  private:

  protected:

	bool FToCylinder;

	nmTipoValvula FTipoValvula;

	int FNumeroOrden;

	double FCDTubVol;
//double FCDVolTub;
	double FCTorb;
//double FCRecuperacion;

	int FTubo;
	int FNodo;
	int FTipo;
	int FValvula;
	int FSentido;

	double FDiamTubo;
	double FDiamRef;
	double FSectionRatio;

	bool FGraficasINS;
	bool FGraficaCDEINS;
	bool FGraficaCDSINS;

	bool FGraficasMED;
	bool FGraficaCDEMED;
	bool FGraficaCDSMED;

	double FSumCDE;
	double FSumCDS;
	double FCDEMedio;
	double FCDSMedio;
	double FSumTime;

	double FTiempoAnt;

	nmRegimenValv FControlRegimen;
	double FRegimen;
	double FRelacionVelocidades;

	TCilindro *FCylinder;
	TTubo *FPipe;
	TDeposito *FPlenum;
	TBloqueMotor *FEngine;
	int FPipeNode;

	double FTime0;

	double LeeDiametro();

//void PutDiametroTubo(double valor);

  public:

	TTipoValvula(nmTipoValvula TipVal);

	virtual ~TTipoValvula() = 0;

	nmTipoValvula getTypeOfValve() {
		return FTipoValvula;
	}
	;

	double getRegimen() {
		return FRegimen;
	}
	;
	double getRelacionVelocidades() {
		return FRelacionVelocidades;
	}
	;
	nmRegimenValv getControlRegimen() {
		return FControlRegimen;
	}
	;

	double getCDTubVol() {
		return FCDTubVol;
	}
	;
	double FCDVolTub;
	double getCDVolTub() {
		return FCDVolTub;
	}
	void AsignaCDVolTub(double Valor);

	double getCTorb() {
		return FCTorb;
	}
	;

	int getNodo() {
		return FNodo;
	}
	;
	int getTipo() {
		return FTipo;
	}
	;
	int getSentido() {
		return FSentido;
	}
	;
	int getNumOrden() {
		return FNumeroOrden;
	}
	;
	int getValvula() {
		return FValvula;
	}
	;
	int getPipe() {
		return FTubo;
	}
	;

	double getDiametro() {
		return LeeDiametro();
	}
	;
	void PutDiametroTubo(double Valor) {
		FDiamTubo = Valor;
	}

	double FCRecuperacion;
	double getCRecuperacion() {
		return FCRecuperacion;
	}
	void AsignaCRecuperacion(double Valor);

	virtual void LeeDatosIniciales(const char *FileWAM, fpos_t &filepos, int norden, bool HayMotor,
								   TBloqueMotor *Engine) = 0;

	void AsignaParametros(int Pipe, int Nodo, int Tipo, int valvula, double dTubo, int sentido);

	void LeeDatosGraficasINS(const char *FileWAM, fpos_t &filepos);

	void CabeceraGraficaINS(stringstream& insoutput, int nodo);

	void ImprimeGraficaINS(stringstream& insoutput);

	void AcumulaCDMedio(double TiempoActual);

	void LeeDatosGraficasMED(const char *FileWAM, fpos_t &filepos);

	void CabeceraGraficaMED(stringstream& medoutput, int nodo);

	void ImprimeGraficaMED(stringstream& medoutput);

	void PutPipe(TTubo *Pipe, int node);

	void PutCylider(TCilindro *Cylinder);

	void PutPlenum(TDeposito *Plenum);

	virtual void GetCDin(double Time) {
	}
	;

	virtual void GetCDout(double Time) {
	}
	;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif

