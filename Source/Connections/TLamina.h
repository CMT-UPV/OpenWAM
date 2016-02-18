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
#ifndef TLaminaH
#define TLaminaH

#include <cstdio>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
//#include <iostream>
//#include <cmath>

#include "TTipoValvula.h"
#include "Globales.h"
//#include "interp_1d.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

enum nmTipoLamina {
	nmLamina0D = 0, nmLamina1D = 1, nmLamina2D = 2
};

class TLamina: public TTipoValvula {
  private:
//CONSTANTES INICIALES
	nmTipoLamina FTipoLamina;
	int FSentidoLamina;
	double FDiametroRef;

	double FMasa;
	double FDensidad;
	double FAmortiguamiento;
	double FArea;
	double FAnchoPetalo;
	double FRigidez;
	double FModuloYoung;
	double FEspesor;
	int FNumPestanyas;
	double FLongitud;
	double FLongReal;

	double FKCDE;
	double FKCDS;

	int FNumLevCDE;
	int FNumLevCDS;
	double FIncrLev;
	dVector FLiftCDin;
	dVector FDatosCDEntrada;
	Hermite_interp *fun_CDin;
	dVector FLiftCDout;
	dVector FDatosCDSalida;
	Hermite_interp *fun_CDout;

	int FNodosLamina;
	int FNodosFijos;
	int FBucle;

	double FCoefC;
	double FAreaTrans;
	double FDeltaX;

//VARIABLES DE CALCULO
	int FValvula;

	double FLev;
	double FdLev;
	double FddLev;

	double *FFuerza;
	double *FLev1;
	double *FLev2;
	double *FLev3;
	double FDerivada4;

	bool FGraficasLam;
	bool FGrafLev;

  public:

	TLamina(TLamina *Origen, int valv);

	TLamina();

	~TLamina();

	double getLevantamiento() {
		return FLev;
	}
	;

	void LeeDatosIniciales(const char *FileWAM, fpos_t &filepos, int norden, bool HayMotor, TBloqueMotor *Engine);

	void CalculaCD(double deltaP, double deltaT);

	void LeeDatosGraficas(const char *FileWAM, fpos_t &filepos);

	void CabeceraGraficaINS(stringstream& insoutput, int lam);

	void ImprimeGraficaINS(stringstream& insoutput);

	void GetCDin(double Time);

	void GetCDout(double Time);

};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif
