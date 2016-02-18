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
#ifndef TValvula4TH
#define TValvula4TH

#include <cstdio>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include <iostream>

#include "TTipoValvula.h"

#include "TController.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TValvula4T: public TTipoValvula {
  private:
//CONSTANTES INICIALES

	dVector FAngle;
	dVector FLevantamiento;
	Hermite_interp *fun_FLift;
	double FIncrAng;

	double FIncrLev;

	dVector FLiftCD;
	dVector FDatosCDEntrada;
	Hermite_interp *fun_CDin;
	dVector FDatosCDSalida;
	Hermite_interp *fun_CDout;
	dVector FDatosTorbellino;
	Hermite_interp *fun_Torb;

	double FCDEntrada;
	double FCDSalida;
	double FDiametro;
	double FDiametroRef;
	double FAnguloApertura;
	double FAnguloApertura0;
	double FAngle0;
	double FAnguloCierre;
	double FCoefTorbMedio;
// VARIABLES DE CALCULO
	double FApertura;

	bool FVVT;
	bool FVVTLift;
	bool FVVTTiming;
	bool FVVTDuration;

	double FVVTLiftMultiplier;
	double FVVTTimigGap;
	double FVVTDurationMultiplier;

	int FVVTLiftCtrlID;
	int FVVTTimingCtrlID;
	int FVVTDurationCtrlID;

	TController *FVVTLiftCtrl;
	TController *FVVTTimingCtrl;
	TController *FVVTDurationCtrl;

	int FValvula;

//double Interpola2(double  x,double  *y,int n);

  public:

	TValvula4T(TValvula4T *Origen, int valv);

	TValvula4T();

	~TValvula4T();

	double getAnguloApertura() {
		return FAnguloApertura;
	}
	;
	double getAnguloCierre() {
		return FAnguloCierre;
	}
	;
	double getDiametro() {
		return FDiametro;
	}
	;
	double getCTorbMed() {
		return FCoefTorbMedio;
	}
	;

	void LeeDatosIniciales(const char *FileWAM, fpos_t &filepos, int norden, bool HayMotor, TBloqueMotor *Engine);

	void CalculaCD(double Angulo);

	void VVTControl(double Time);

	void AsignaLevController(TController **Controller);

	void GetCDin(double Time);

	void GetCDout(double Time);

};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif
