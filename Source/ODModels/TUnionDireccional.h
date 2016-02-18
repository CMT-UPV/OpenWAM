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
#ifndef TUnionDireccionalH
#define TUnionDireccionalH
#include "TDepVolCteBase.h"

class TUnionDireccional: public TDepVolCteBase {
  private:

// Variable privadas

	TCondicionContorno **FCCEntrada;
	TCondicionContorno *FCCSalida;

	int FNumUnionDireccional; // Numero de Union Direccional.
	int *FNodoEntrada;       // Nodo de entrada(en cada una de las dos entradas.
	int FNodoSalida;          // Nodo de salida.
	int *FSentidoEntrada; // Sentido del flujo en los nodos de entrada a la union direccional.
	double *FCDSalidaInicial; // Valor del coeficiente de descarga de salida hasta la velocidad en que aparece el efecto direccional.
	double *FVelocidadCorte; // Valor de la velocidad en el tubo i para la que aparece el efecto direccional sobre el tubo j.
	double *FVelocidadFin; // Valor de la velocidad en el tubo i para la que se anula el coeficiente de descarga de salida en el tubo j.
	double *FVelocity; // Velocity en los nodos de entrada a la union direccional.
	double *FCoefA;
	double *FCoefB;

  protected:

  public:

	//---------------------------------------------------------------------------
	// FUNCIONES PUBLICAS
	//---------------------------------------------------------------------------

	TUnionDireccional(int i, int NUnionDireccional, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
					  nmCalculoGamma GammaCalculation, bool ThereIsEGR);

	~TUnionDireccional();

	void ActualizaPropiedades(double TimeCalculo);

	void AsignaCCUnionDireccional();

	void LeeDatosUnionDireccional(const char *FileWAM, fpos_t &filepos);

	void CalculoUnionDireccional();

	void UpdateProperties0DModel(double TimeCalculo);

};

//---------------------------------------------------------------------------
#endif
