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
#ifndef TCompTubDepH
#define TCompTubDepH
#include "TCompresor.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TTubo;
class TDeposito;
class TCondicionContorno;

class TCompTubDep: public TCompresor {
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

  private:

	TDeposito *FDeposito;
	TTubo *FTuboRotor;
	int FTubRotor;
	int FTubStator;
	int FIndiceCC;
	double FNodoFinTuboSalida;
	double FNodoFinEntrada;
	nmCompressorInlet FEntradaCompresor;
	nmPipeEnd FExtremoTuboRotor;
	nmCompressorMapFormat FCompressorMapFormat;

	int FParticionEntrada;    // Se pueden borrar.Pedro
	int FParticionSalida;

	double FAreaSalComp;
	nmPipeEnd FExtremoSalida;

	double *FLanda;
	double *FBeta;
	double *FEntro;

	double FTemperatura20;
	double FEntropia2;
	double FTemperatura2;
	double FVelocidad2;
	double FDensidad20;
	double FDensidad2;

	double FGasto0;
	double FGasto0Correg;
	double FGasto1;

	bool FCambiaReg;
	double FVariacionRegimen;
	double FCorrector;

// Transporte de especies
	double FGamma1;
	double FGamma3;
	double FGamma4;
	double FGamma5;
	double FRAtm;       // Valor de R para la composicion atmosferica.
	double FCvAtm;      // Valor de Cv para la composicion atmosferica.
	double FCpAtm;      // Valor de Cp para la composicion atmosferica.
	double FGammaAtm;   // Valor de Gamma para la composicion atmosferica.

// Damping
	double FMass_filt_ant;
	double FMass_ant;
	double FDelay;

	double RC_filt;
	double RC_filt_ant;
	double RC_ant;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

	double CalGastoNuevo(double MasaAire);

	double RegulaFalsi();

	void CalculaCompresor(double Theta);

	int GetTubStator() {
		return FTubStator;
	}

	int GetParticionSalida() {
		return FParticionSalida;
	}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

  public:

	int getTubStator() {
		return GetTubStator();
	}
	;

	int getParticionSalida() {
		return GetParticionSalida();
	}
	;

	nmCompressorInlet getEntradaCompresor() {
		return FEntradaCompresor;
	}
	;

	TCompTubDep(int i, nmTipoCalculoEspecies SpeciesModel, int numeroespecies, nmCalculoGamma GammaCalculation,
				bool ThereIsEGR);

	~TCompTubDep();

	void LeeCompresor(const char *FileWAM, fpos_t &filepos);

	void CondicionCompresor(double Theta, stTuboExtremo *TuboExtremo, double AcumulatedTime, int TuboCalculado);

	void DatosEntradaCompresor(double AmbientTemperature, double AmbientPressure, TCondicionContorno *BC);

	void BusquedaEntradaSalida(nmCompressorInlet EntradaCompresor, double AmbientTemperature, int numeroCC,
							   TCondicionContorno **BC, double *AtmosphericComposition);

// NO SE UTILIZA EN ESTE TIPO DE COMPRESOR
	void CalculaGasto(double TrabajoInsTurbina, double TiempoActual) {
	}

	void Initialize();

	double NewDampedSolution(double Mass);

};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif
