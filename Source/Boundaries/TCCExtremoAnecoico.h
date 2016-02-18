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
#ifndef TCCExtremoAnecoicoH
#define TCCExtremoAnecoicoH

#include "TCondicionContorno.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TCCExtremoAnecoico: public TCondicionContorno {
  private:

	int FNodoFin; // Nodo del extremo del tubo que conecta con la condicion de contorno.
	int FIndiceCC; // Posicion del vector para tomar datos del tubo para la BC (0 Nodo izquierdo; 1 Nodo derecho)
	double *FCC;          // Caracteristica conocida del tubo.
	double *FCD;          // Caracteristica desconocida del tubo.

  public:
	TCCExtremoAnecoico(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
					   nmCalculoGamma GammaCalculation, bool ThereIsEGR);

	~TCCExtremoAnecoico();

	void AsignaTubos(int NumberOfPipes, TTubo **Pipe);

	void CalculaCondicionContorno(double Time);

	void TuboCalculandose(int TuboActual) {
	}
	;
};

#endif

