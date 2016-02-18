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
#ifndef TCompresorDepH
#define TCompresorDepH

#include "TCompresor.h"
#include "Constantes.h"

class TDeposito;

class TCompresorDep: public TCompresor {
  private:

	TDeposito *FDepositoRot;
	TDeposito *FDepositoEst;

	double FGamma4;
	double FGamma5;

	double FTempGasto; // Temperature del massflow que ha pasado por el compresor (Kelvin)

//---------------------------------------------------------------------------
//          FUNCIONES PRIVADAS
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

  public:
//---------------------------------------------------------------------------
//          VARIABLES PUBLICAS
//---------------------------------------------------------------------------

	double getTempSal() {
		return FTempGasto;
	}
	;

	int getDepRotor() {
		return FDepRotor;
	}
	;

//---------------------------------------------------------------------------
//          FUNCIONES PUBLICAS
//---------------------------------------------------------------------------

	TCompresorDep(int i, nmTipoCalculoEspecies SpeciesModel, int numeroespecies, nmCalculoGamma GammaCalculation,
				  bool ThereIsEGR);

	~TCompresorDep();

	void LeeCompresor(const char *FileWAM, fpos_t &filepos);

	void CalculaGasto(double TrabajoInsTurbina, double TiempoActual);

	void RelacionDepositoCompresor(TDeposito *DepositoRot, TDeposito *DepositoEst);

	// NO SE UTILIZA EN ESTE TIPO DE COMPRESORES
	void CondicionCompresor(double Theta, stTuboExtremo *TuboExtremo, double AcumulatedTime, int TuboCalculado) {
	}

	void Initialize();

};

//---------------------------------------------------------------------------
#endif
