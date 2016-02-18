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
#ifndef TCCEntradaCompresorH
#define TCCEntradaCompresorH

#include "TCondicionContorno.h"
#include <cstdio>
#include <iostream>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TCompresor;

class TCCEntradaCompresor: public TCondicionContorno {
  private:

	int FNumeroCompresor;
	TCompresor *FCompresor;

	int FNodoFin; // Nodo en el extremo del tubo que esta en la condicion de contorno.
	double *FCC;          // Caracteristica conocida del tubo.
	double *FCD;          // Caracteristica desconocida del tubo.
	double FSeccionTubo;  // Diametro del tubo en la condicion de contorno.

	double FVelocity;
	double FSonido;
	double FPressure;
	double FGasto;

	double FGamma1;
	double FGamma3;
	double FGamma5;

  public:

	int getNumeroCompresor() {
		return FNumeroCompresor;
	}
	;

	TCCEntradaCompresor(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
						nmCalculoGamma GammaCalculation, bool ThereIsEGR);

	~TCCEntradaCompresor();

	void CalculaCondicionContorno(double Time);

	void ReadBoundaryData(const char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe, int nDPF, TDPF **DPF);

	void AsignaCompresor(TCompresor **Compressor);

};

#endif
