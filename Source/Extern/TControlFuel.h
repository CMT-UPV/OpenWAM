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
#ifndef TControlFuelH
#define TControlFuelH

#include <string.h>
#include <cstdio>
#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
//#include <cmath>
#include "Globales.h"
//---------------------------------------------------------------------------

class TControlFuel {
  private:

	FILE *FichFuel;
	double FuelAct;
	double FFuelDeseado;
	int FNumeroDatos_Regimen;
	int FNumeroDatos_Ma;
	double *FVector_Ma_mapa;
	double *FVector_Regimen_mapa;
	double **FMapa_Limitador_Humos;

//Funcion de interpolacion
	double xit_(double vizq, double vder, double axid, double xif);

  public:
	TControlFuel();

	~TControlFuel();

	double getFuel() {
		return FuelAct;
	}
	;

	double getFuelDeseado() {
		return FFuelDeseado;
	}
	;

	double CalculaFuel(double MasaPorAdmision, double Regimen, double TiempoActual);

	void IniciaFuel(double mfuel);

	void LeeDatosEntrada(char *Dir, FILE *fich);
};

#endif
