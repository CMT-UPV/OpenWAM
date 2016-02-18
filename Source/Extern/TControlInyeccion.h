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
#ifndef TControlInyeccionH
#define TControlInyeccionH

#include <string.h>
#include <cstdio>
#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
//#include <cmath>
#include "Globales.h"
//---------------------------------------------------------------------------

class TControlInyeccion {
  private:

	FILE *FichInyeccion;

	int FNumeroInyecciones;
	double *FMasaFuel; // Masa de combustible inyectada en cada una de las inyecciones realizadas
	double *FSOI; // Punto de inyeccion para cada una de la inyecciones realizadas
	double FPrail;      // Presion del common rail
	double FDiferencia_SOI; // Diferencia entre el angulo de inyeccion de la principal y el de la piloto.

	int FNumeroDatos_Prail_Regimen;
	int FNumeroDatos_Mf_Prail;
	int FNumeroDatos_Mf;
	int FNumeroDatos_Regimen;

	double *FVector_Mf_mapa;
	double *FVector_Mf_mapaPrail;
	double *FVector_Prail_Regimen_mapa;
	double *FVector_Regimen_mapa;

	double **FMapa_Combustible_Piloto;
	double **FMapa_SOI_Principal;
	double **FMapa_SOI_Piloto;
	double **FMapa_Prail;

  public:

	int getNumeroInyecciones() {
		return FNumeroInyecciones;
	}
	;
	double getPresionInyeccion() {
		return FPrail;
	}
	;
// FUNCIONES PRIVADAS
	double GetSOI(int i);

	double GetMasaFuel(int i);

	TControlInyeccion();

	~TControlInyeccion();

	void CalculaSistemaInyeccion(double MasaFuel, double Regimen);

	void LeeDatosEntrada(char *Ruta, FILE *fich);

};

#endif
