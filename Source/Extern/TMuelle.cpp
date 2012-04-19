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


#pragma hdrstop

#include "TMuelle.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

 TMuelle::TMuelle()
{

/* Introducción de los valores de rigidez, amortiguamiento y masa del muelle */

FRigidez=1;
FAmort=1;
FMasa=1;
Fx=0;
Fdx=0;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

 TMuelle::~TMuelle()
{

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


void TMuelle::CalculaPosicion(double fuerza,double DeltaT)
{
  try
  {
    double ddx, xact, dxact;


    ddx = (fuerza- FRigidez * Fx - FAmort * Fdx)/FMasa;

    xact = Fx + Fdx * DeltaT + DeltaT * DeltaT * ddx/2.;

    dxact = Fdx + DeltaT * ddx;

    Fx = xact;

    Fdx = dxact;

  }

  catch(Exception &N)
  {
    std::cout << "ERROR: Calculo de la Posición del Muelle(DLL)" << std::endl;
    std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
    throw Exception(N.Message.c_str());
  }
}


//---------------------------------------------------------------------------

#pragma package(smart_init)
