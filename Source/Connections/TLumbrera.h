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
#ifndef TLumbreraH
#define TLumbreraH

#include <cstdio>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include <iostream>
#include "Constantes.h"
//#include <cmath>

#include "TTipoValvula.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TLumbrera: public TTipoValvula {
  private:

//DATOS INICIALES
	double FAltura;
	double FAnchura;
	double FRadioSup;
	double FRadioInf;
	double FPosicionPMI;
	double FDiametroRef;
	double FCarrera;
	double FBiela;

	double FAnguloApertura;  // Hector 2T
	double FAnguloCierre;  // Hector 2T

	int FNumCD;
	dVector FApertura;
	dVector FDatosCDEntrada;
	Hermite_interp *fun_CDin;
	dVector FDatosCDSalida;
	Hermite_interp *fun_CDout;

//VARIABLES DE CALCULO
	int FValvula;
	double FApertActual;

	void CalculateOpeningANDClose();

  public:

	double getAnguloApertura() {
		return FAnguloApertura;
	}
	; // Hector 2T

	double getAnguloCierre() {
		return FAnguloCierre;
	}
	; // Hector 2T

	TLumbrera(TLumbrera *Origen, int valv);

	TLumbrera(double Biela, double Carrera);

	~TLumbrera();

	void LeeDatosIniciales(const char *FileWAM, fpos_t &filepos, int norden, bool HayMotor, TBloqueMotor *Engine);

	void CalculaCD(double Angulo);

	double CalculaDistPMI(double x);

	double CalculaApertura(double x);

	void GetCDin(double Time);

	void GetCDout(double Time);
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif

