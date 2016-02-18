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
#ifndef TCCPerdidadePresionH
#define TCCPerdidadePresionH

#include "TCondicionContorno.h"

//#include <cmath>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include <cstdio>
#include <iostream>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TCCPerdidadePresion: public TCondicionContorno {
  private:

	nmTipoPP FTipoPP;

	int *FNodoFin;               // Nodo del tubo en la condicion de contorno.
	int *FIndiceCC; // Posicion del vector para tomar datos del tubo para la BC (0 Nodo izquierdo; 1 Nodo derecho)
	double **FCC;                // Caracteristica conocida del tubo.
	double **FCD;                // Caracteristica desconocida del tubo.
	int *FNumeroTubo;
	int FTuboActual;

	double FGamma3; // Son expresiones con Gamma. Se usan estas variables para no calcularlas tantas veces por instante de tiempo en la misma funcion.
	double FGamma2;
	double FGamma5;
	double FGamma1;

	double FK;           // Coeficiente de resistencia caracteristico. Negativo.
	// Se encuentra dividida por 2 respecto de la definicion teorica.
	double FRelacionEntropia; // Relacion entre la entropia del tubo saliente y la del tubo entrante.

  public:

	TCCPerdidadePresion(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
						nmCalculoGamma GammaCalculation, bool ThereIsEGR);

	~TCCPerdidadePresion();

	void ReadBoundaryData(const char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe, int nDPF, TDPF **DPF);

	void CalculaCondicionContorno(double Time);

	void TuboCalculandose(int TuboActual);

	double getK() {
		return FK;
	}
	;

	void PutK(double valor) {
		FK = valor;
	}

};

//---------------------------------------------------------------------------
#endif
