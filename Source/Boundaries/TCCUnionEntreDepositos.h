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
#ifndef TCCUnionEntreDepositosH
#define TCCUnionEntreDepositosH

#include "TCondicionContorno.h"

#include <cstdio>
#include <iostream>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TDeposito;
class TCCUnionEntreDepositos: public TCondicionContorno {
  private:

	bool FIndependiente;

	TTipoValvula *FValvula;
	TDeposito *FDeposito1;
	TDeposito *FDeposito2;

	int FNumeroDeposito1;
	int FNumeroDeposito2;

	double FPresionDep1;
	double FPresionDep2;
	double FPresionDep1SUM;
	double FAsonidoDep1SUM;
	double FGammaDep1SUM;
	double FRDep1SUM;
	double FTiempoDep1SUM;
	double FPresionDep2SUM;
	double FAsonidoDep2SUM;
	double FGammaDep2SUM;
	double FRDep2SUM;
	double FTiempoDep2SUM;

	double FCDEntrada;
	double FCDSalida;
	double FCTorbellino;

	double FGasto; // Se calcula en valor absluto. EL tratamiento de su signo se realiza con las variables
	// auxialares FSentidoFlujoED.
	double FVelocity;
	double FGastoImpreso;
	double Fa0;     // Velocity del sonido en el deposito del que sale el flujo.
	double Fa1;     // Velocity del sonido en el deposito al que llega el flujo.

	int FSentidoFlujoED1;
	int FSentidoFlujoED2;

	stResInstantUED FResInstantUED;
	stResMediosUED FResMediosUED;

//double FTiempoActual;

	double FRegimen;

	double FGamma1;
	double FGamma2;

// FUNCIONES

	double InterpolaDeposito(double vizq, double vder, double axid, double xif);

  public:

	int getNumeroDeposito1() {
		return FNumeroDeposito1;
	}
	;
	int getNumeroDeposito2() {
		return FNumeroDeposito2;
	}
	;
	int getSentidoFlujoED1() {
		return FSentidoFlujoED1;
	}
	;
	int getSentidoFlujoED2() {
		return FSentidoFlujoED2;
	}
	;
	double getMassflow() {
		return FGasto;
	}
	;
	double getVelocity() {
		return FVelocity;
	}
	;
	double getGastoImpreso() {
		return FGastoImpreso;
	}
	;
	double getSpeedSound() {
		return Fa0;
	}
	;
	TTipoValvula* getValvula() {
		return FValvula;
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

	TCCUnionEntreDepositos(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
						   nmCalculoGamma GammaCalculation, bool ThereIsEGR);

	~TCCUnionEntreDepositos();

	void AsignaTipoValvula(TTipoValvula **Origen, int Valv, int i);

	void CalculaCondicionContorno(double Time);

	void LeeUEDepositos(const char *FileWAM, fpos_t &filepos, bool Independent);

	void AsignaDepositos(TDeposito **Plenum);

	void CalculaCoeficientesDescarga(double Angulo, double mfcomb = 0., double RegimenMotor = 0.);

	void LeeResultadosInstantUED(const char *FileWAM, fpos_t &filepos);

	void CabeceraResultadosInstantUED(stringstream& insoutput);

	void ImprimeResultadosInstantUED(stringstream& insoutput);

	void ResultadosInstantUED();

	void ReadAverageResultsUED(const char *FileWAM, fpos_t &filepos);

	void HeaderAverageResultsUED(stringstream& medoutput);

	void ImprimeResultadosMediosUED(stringstream& medoutput);

	void ResultadosMediosUED();

	void AcumulaResultadosMediosUED(double Actual);

	void CalculaUED();

};

#endif
