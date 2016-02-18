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
#ifndef TRegimenMotorH
#define TRegimenMotorH

#include <cstring>
#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include "Globales.h"

class TRegimenMotor {
  private:

	FILE *FichRegimen;
	double FRegimenMotor;
	double *FTiempo;
	double *FRegimen;
	int FNumeroDatos;

//Funcion de interpolacion
	double xit_(double vizq, double vder, double axid, double xif);

  public:

	double getRegimen() {
		return FRegimenMotor;
	}
	;

	TRegimenMotor();

	~TRegimenMotor();

	void CalculaRegimen(double TiempoActual);

	void LeeDatosEntrada(char *Dir, FILE *fich);

};
//---------------------------------------------------------------------------
#endif
