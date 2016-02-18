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
#ifndef TDiscoRotativoH
#define TDiscoRotativoH

#include <cstdio>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include <iostream>
//#include <cmath>

#include "TTipoValvula.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TDiscoRotativo: public TTipoValvula {
  private:

	double FDiametroRef;
	int FNumeroPuntos;

	double FAngle0;

	dVector FAngulo;
	dVector FDatosCDEntrada;
	Hermite_interp *fun_CDin;
	dVector FDatosCDSalida;
	Hermite_interp *fun_CDout;

	int FValvula;

	double FDCMultiplier;
	double FShift;
	double FDuration;

  public:

	TDiscoRotativo(TDiscoRotativo *Origen, int valv);

	TDiscoRotativo();

	~TDiscoRotativo();

	void LeeDatosIniciales(const char *FileWAM, fpos_t &filepos, int norden, bool HayMotor, TBloqueMotor *Engine);

	void CalculaCD(double AnguloActual);

	void GetCDin(double Time);

	void GetCDout(double Time);

	void PutAngle0(double val) {
		FAngle0 = val * FRelacionVelocidades - FShift;
	}
	;

};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif
