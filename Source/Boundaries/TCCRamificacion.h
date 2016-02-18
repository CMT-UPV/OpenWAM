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
#ifndef TCCRamificacionH
#define TCCRamificacionH

#include "TCondicionContorno.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TCCRamificacion: public TCondicionContorno {
  private:

	int *FNodoFin;               // Nodo del tubo en el extremo del tubo.
	int *FIndiceCC; // Posicion del vector para tomar datos del tubo para la BC (0 Nodo izquierdo; 1 Nodo derecho)
	double *FSeccionTubo;      // Diametro del tubo en la condicion de contorno.
	double **FCC;                // Caracteristica conocida del tubo.
	double **FCD;                // Caracteristica desconocida del tubo.
	double *FEntropia;
	double *FVelocity;          // Velocity en el extremo del tubo.
	double *FDensidad;           // Density en el extremo del tubo.
	int FTuboActual;             // Numero de tubo que se esta calculando.
	int *FNumeroTubo;

	double FTiempoActual;
	double FTiempoAnterior;

//Variables del Transporte de Especies.
	double FGamma3;
	double FGamma4;
	double FGamma1;
	double *FMasaEspecie;

  public:

	TCCRamificacion(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
					nmCalculoGamma GammaCalculation, bool ThereIsEGR);

	~TCCRamificacion();

	void CalculaCondicionContorno(double Time);

	void AsignaTubos(int NumberOfPipes, TTubo **Pipe);

	void TuboCalculandose(int TuboActual);

};

#endif

