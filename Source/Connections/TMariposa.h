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
#ifndef TMariposaH
#define TMariposaH

#include <cstdio>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include <iostream>

#include "TController.h"

#include "TTipoValvula.h"
//---------------------------------------------------------------------------

class TMariposa: public TTipoValvula {
  private:

	int FNumLev;
	dVector FLevantamiento;
	dVector FDatosCDEntrada;
	Hermite_interp *fun_CDin;
	dVector FDatosCDSalida;
	Hermite_interp *fun_CDout;
	double FDiametroRef;
	double FLevActual;
	bool FLevControlled;
	int FControllerID;

	TController *FController;

  public:

	TMariposa(TMariposa *Origen, int valv);

	TMariposa();

	~TMariposa();

	void LeeDatosIniciales(const char *FileWAM, fpos_t &filepos, int norden, bool HayMotor, TBloqueMotor *Engine);

	void AsignaLevController(TController **Controller);

	void CalculaCD(double Time);

	void GetCDin(double Time);

	void GetCDout(double Time);

};

#endif
