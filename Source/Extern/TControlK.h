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
#ifndef TControlKH
#define TControlKH

#include <string.h>
#include <cstdio>
#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
//#include <cmath>
#include "Globales.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TControlK {
  private:

// Variables
	FILE *FichK_Re;

	int FNumeroDatos;
	double *FVector_Re;
	double *FVector_K;
	double FK;
	double FDiametro;
	double FRe;

  public:
	TControlK();

	~TControlK();

//   __property double K={read=FK};
//   __property double Re={read=FRe};

	void CalculaK(double velocidad, double temperatura, double presion, double R_mezcla);

	void LeeDatosEntrada(char *Dir, FILE *fich, double FDiametro);

};

#endif

