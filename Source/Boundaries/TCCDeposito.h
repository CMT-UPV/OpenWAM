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
#ifndef TCCDepositoH
#define TCCDepositoH

#include "TCondicionContorno.h"
#include <cstdio>
#include <iostream>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class TDeposito;

class TCCDeposito: public TCondicionContorno {
  private:

	TTipoValvula *FValvula;
	TDeposito *FDeposito;

	int FNumeroDeposito; // Numero de deposito ligado a la condicion de contorno.

	int FNodoFin;               // Nodo en el extremo del tubo.
	int FIndiceCC; // Posicion del vector para tomar datos del tubo para la BC (0 Nodo izquierdo; 1 Nodo derecho)
	double *FCC;                // Caracteristica conocida del tubo.
	double *FCD;                // Caracteristica desconocida del tubo.

	double FPref;            // Pressure de referencia

	double FCDEntrada;       // Coeficiente de descarga a la entrada
	double FCDSalida;        // Coeficiente de descarga a la salida
	double FCTorbellino;    // Coeficiente de torbellino

	double FSeccionEficaz;   // Seccion eficaz de la valvula
	double FGasto; // Massflow en el extremo del tubo. Si es entrante, signo -. Si es saliente, signo +
	double FVelocity;         // Velocity en el extremo del tubo
	double FSonido;            // Velocity del sonido en el extremo del tubo
	double FVelocidadGarganta;  // Velocity en la garganta
	double FMachGarganta;       // Numero de Mach en la garganta.
	double FGastoGarganta;      // Massflow en la garganta.
	double FRelacionPresionGarganta;
	double FAd;
	double FAdCr;  // Ad teniendo en cuenta la recuperacion de energia cinetica.

	double FSeccionValvula;   // Seccion en la garganta
	double FSeccionTubo;      // Seccion en el extremo del tubo

	double FCarrera;
	double Fk; // Relacion entre la seccion del tubo en el extremo y la seccion eficaz de la valvula.
	double Fcc; // Variable auxiliar utilizada para el calculo de el caso saliente supercritico.
// double FMachVenturi;      // Numero de Mach en la condicion de contorno que para el calculo de presion de parada en                    // por el modelo en el extremo del tubo. Influye en el calculo de FAdCr
	// el extremo del tubo.
	nmCaso FCaso;
	nmSentidoFlujo FSentidoFlujo;

	double FRegimen;

	double FGamma1;
	double FGamma2;
	double FGamma3;
	double FGamma4;
	double FGamma5;
	double FGamma6;

	bool FEncontrado;

// FUNCIONES

	void FEDRecuperacionEnergiaCinetica();

	void FlujoEntranteDeposito();

	void FlujoSalienteDeposito();

	void Resolucion(double ext1, double ext2, nmCaso Caso, double *u2t, double *a2t);

  public:

	int getNumeroDeposito() {
		return FNumeroDeposito;
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
	double getSpeedSound() {
		return FSonido;
	}
	;
	double getSeccionTubo() {
		return FSeccionTubo;
	}
	;
	TTipoValvula* getValvula() {
		return FValvula;
	}
	;
	nmSentidoFlujo getSentidoFlujo() {
		return FSentidoFlujo;
	}
	;
	TDeposito* getPlenum() {
		return FDeposito;
	}
	;
	void PutCDSalida(double valor) {
		FCDSalida = valor;
	}
	;
	double FMachVenturi;
	double getMachVenturi() {
		return FMachVenturi;
	}
	;
	void putMachVenturi(double valor) {
		FMachVenturi = valor;
	}

	TCCDeposito(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
				nmCalculoGamma GammaCalculation, bool ThereIsEGR);

	~TCCDeposito();

	void ReadBoundaryData(const char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe, int nDPF, TDPF **DPF);

	void AsignaDeposito(TDeposito **Plenum);

	void CalculaCondicionContorno(double Time);

	void AsignaTipoValvula(TTipoValvula **Origen, int Valv, int i);

	void CalculaCoeficientesDescarga(double TiempoAcutal, double mfcomb = 0., double RegimenMotor = 0.);

	void IniciaGamma();

	void TuboCalculandose(int TuboActual) {
	}
	;

};

#endif

