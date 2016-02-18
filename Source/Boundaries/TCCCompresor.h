/* --------------------------------------------------------------------------------*\
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


 \*-------------------------------------------------------------------------------- */

// ---------------------------------------------------------------------------
#ifndef TCCCompresorH
#define TCCCompresorH

#include "TCondicionContorno.h"

#include <cstdio>
#include <iostream>

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class TCompresor;

class TDeposito;

class TCCCompresor: public TCondicionContorno {
  private:

	nmCompressorInlet FEntradaCompresor;
	nmPipeEnd FExtremoTuboRotor;

	int FNumeroCompresor;
	int FNumeroDeposito;
	int FNumeroDepositoRot;
	int FNumeroDepositoEst;

	TDeposito *FDeposito;
	TDeposito *FDepositoRot;
	TDeposito *FDepositoEst;

	TCompresor *FCompresor;

	TTubo *FTuboRotor;

	int FTuboActual; // Identifica el tubo que se esta calculando para el caso de Compressor de 2 tubos.
	int *FNumeroTubo;

	double FPamb;
	double FTamb;

	double FTheta;
	// double FTiempoActual;

  public:

	int getNumeroCompresor() {
		return FNumeroCompresor;
	}
	;

	TCompresor* getCompressor() {
		return FCompresor;
	}
	;

	TDeposito* getPlenum() {
		return FDeposito;
	}
	;

	TTubo* getTuboRotor() {
		return FTuboRotor;
	}
	;

	nmPipeEnd getExtremoTuboRotor() {
		return FExtremoTuboRotor;
	}
	;

	int getNumeroDeposito() {
		return FNumeroDeposito;
	}
	;

	int getNumeroDepositoRot() {
		return FNumeroDepositoRot;
	}
	;

	int getNumeroDepositoEst() {
		return FNumeroDepositoEst;
	}
	;

	double FTiempoActual;

	double getInstanteCalculo() {
		return FTiempoActual;
	}

	void PutInstanteCalculo(double valor) {
		FTiempoActual = valor;
	}
	;

	nmCompressorInlet getEntradaCompresor() {
		return FEntradaCompresor;
	}
	;

	TCCCompresor(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
				 nmCalculoGamma GammaCalculation, bool ThereIsEGR);

	~TCCCompresor();

	void LeeNumeroCompresor(const char *FileWAM, fpos_t &filepos);

	void AsignacionDatos(TCompresor **Compressor, TDeposito **Plenum, const char *FileWAM, fpos_t &filepos,
						 int NumberOfPipes, TTubo **Pipe, TCondicionContorno **BC, int numCC, double AmbientTemperature, double AmbientPressure,
						 double *AtmosphericComposition);

	void ObtencionValoresInstantaneos(double Theta, double tiempoactual);

	void CalculaCondicionContorno(double Time);

	void TuboCalculandose(int TuboActual);

	void ReadCompressorData(const char *FileWAM, fpos_t &filepos, TCompresor **Compressor);

	void AsignData(TDeposito **Plenum, int NumberOfPipes, TTubo **Pipe, TCondicionContorno **BC, int numCC,
				   double *AtmosphericComposition, TCompresor **Compressor, double AmbientTemperature, double AmbientPressure);
};

#endif
