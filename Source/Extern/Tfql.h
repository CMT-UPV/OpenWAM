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
//---------------------------------------------------------------------------
// OBJETO PARA LA INTERPOLACION DE LEYES DE LIBERACION DE CALOR
//
// Se deben establecer los parametros que se desean utilizar en el calculo de
// angulos de combustion y de la FQL.
// En la funcion fql se debe establecer el desfase existente entre cada cilindro
// en funcion de su numero y orden de encendido.
//
// Para validar la base de datos de FQL se dispone del programa CalculaLey.
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#ifndef TfqlH
#define TfqlH

#include <cstring>
#include <cstdio>
#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include "Globales.h"

class Tfql {

  private:

	FILE *Fichfql;
	int Fnwiebe, Fnley, Fncilin, Fnparametros;
	double **Flm, **Flc, **Flb, **Fli, **Fla, **Fpar_dist, *Fmax;
	double ***Flab;
	double Fang0, Ffinc;
	bool *Fcombustion;

  public:

	Tfql(int ncilin);

	~Tfql();

	double getInicio() {
		return Fang0;
	}
	;
	double getFinal() {
		return Ffinc;
	}
	;

	int getNumParametros() {
		return Fnparametros;
	}
	;

	void lee_leylib(char *Ruta, FILE *fich);

	void lee_leylib2(FILE *BaseDatos);

	void calcula_angulos_combustion(double *parametros, int i);

	double fun_wiebe(double x, double m, double c, double ia, double a0);

	double fql(double x, int j, int i);

	double calcula_fql(double *parametros, double x, int i);

};
//---------------------------------------------------------------------------
#endif

