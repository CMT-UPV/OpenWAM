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
#ifndef TDepVolVariableH
#define TDepVolVariableH

#include "Constantes.h"
#include "TDeposito.h"

class TDepVolVariable: public TDeposito {
  private:
	//---------------------------------------------------------------------------
	// VARIABLES PRIVADAS
	//---------------------------------------------------------------------------

	int FNumeroCompresorVol;
	double FLBiela;
	double FCarrera;
	double FDiametro;
	double FDescentramiento;
	double FRelCompre;
	double FDesfase;
	double FVolumenMuerto;
	double FAngulo;

	double FRegimen;
	nmRegimenValv FControlRegimen;
	double FRelacionVelocidades;

	//---------------------------------------------------------------------------
	// FUNCIONES PRIVADAS
	//---------------------------------------------------------------------------

	double CalculaVolumen(double CrankAngle, double carrera, double lbiela, double diametro, double vol_muerto);

  public:
	//---------------------------------------------------------------------------
	// VARIABLES PUBLICAS
	//---------------------------------------------------------------------------

	double getRegimen() {
		return FRegimen;
	}
	;
	double getRelacionVelocidades() {
		return FRelacionVelocidades;
	}
	;

	//---------------------------------------------------------------------------
	// FUNCIONES PUBLICAS
	//---------------------------------------------------------------------------

	TDepVolVariable(int i, int ncv, nmTipoCalculoEspecies SpeciesModel, int numeroespecies, nmCalculoGamma GammaCalculation,
					bool ThereIsEGR);

	~TDepVolVariable();

	void ActualizaPropiedades(double TimeCalculo);

	void IniciaVolumen(double Theta);

	void LeeDatosDepVolVariable(const char *FileWAM, fpos_t &filepos, bool HayMotor);

	void UpdateProperties0DModel(double TimeCalculo);

	void UpdateSpeed(double NewSpeed);

};
//---------------------------------------------------------------------------
#endif
