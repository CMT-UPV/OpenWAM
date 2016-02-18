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
#ifndef TCondicionContornoH
#define TCondicionContornoH

#include "Globales.h"

#include <cstdio>
#include <iostream>
#include "BoundaryFunctions.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//int _matherr(struct _exception *a) {
//	// throw Exception(std::to_string(a->name)+"("+std::to_string(a->arg1)+","+std::to_string(a->arg2)+")");
//	if (a->arg2 != 0) {
//		MessageBeep(2);
//	}
//	std::cout << "Math error in wam" << std::endl;
//	std::cout << a->type << std::endl;
//	std::cout << a->name << std::endl;
//	std::cout << a->arg1 << std::endl;
//	std::cout << a->arg2 << std::endl;
//	std::cout << a->retval << std::endl;
//	return 0;
//}

class TCondicionContorno {

  private:

  protected:

	nmTypeBC FTipoCC;

	stTuboExtremo *FTuboExtremo;

	int FNumeroCC; // Numero de la condicion de contorno (empieza en 1)
	int FNumeroTubosCC; // Numero de tubos en la condicion de contorno.

	double FTime0;
	double FTime1;
	double FDeltaT;
	double FDeltaAngulo;
	double FAnguloAnterior;
	double FAnguloActual;

//	double FPosicionNodo;

	// Calculo de Especies
	int FNumeroEspecies;
	nmTipoCalculoEspecies FCalculoEspecies;
	nmCalculoGamma FCalculoGamma;
	double FGamma;
	double FRMezcla;
	double FCvMezcla;
	double FCpMezcla;
	double *FFraccionMasicaEspecie;

	bool FHayEGR;
	int FIntEGR;

	bool FUnionDPF;

//	void PutBeta(int i, double valor);
//	void PutLanda(int i, double valor);

//	void PutPosicionNodo(double valor);

  public:

	nmTypeBC getTipoCC() {
		return FTipoCC;
	}
	;

	int getNumeroCC() {
		return FNumeroCC;
	}
	;

	int getNumeroTubosCC() {
		return FNumeroTubosCC;
	}
	;

	bool getUnionDPF() {
		return FUnionDPF;
	}
	;

	stTuboExtremo GetTuboExtremo(int i);

	double GetLanda(int i);
	double GetBeta(int i);
	double GetEntropia(int i);
	void PutBeta(int i, double valor);
	void PutEntropia(int i, double valor);
	void PutLanda(int i, double valor);

	void PutPosicionNodo(double valor) {
		FPosicionNodo = valor;
	}
	double FPosicionNodo;
	double getPosicionNodo() {
		return FPosicionNodo;
	}

	double getGamma() {
		return FGamma;
	}
	;

	double getR() {
		return FRMezcla;
	}
	;
	double GetFraccionMasicaEspecie(int i);

	TCondicionContorno(nmTypeBC Tipo, int numCC, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
					   nmCalculoGamma GammaCalculation, bool ThereIsEGR);

	virtual ~TCondicionContorno() = 0;

	virtual void CalculaCaracteristicas(double Time);

	virtual void CalculaCondicionContorno(double Time) = 0;

	virtual void TuboCalculandose(int i) {
	}
	;

	virtual void ReadBoundaryData(const char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe, int nDPF,
								  TDPF **DPF) {
	}
	;

	virtual void AsignAmbientConditions(double Tamb, double Pamb, double *AtmosphericComposition) {
	}
	;

	virtual void AsignaTubos(int NumberOfPipes, TTubo **Pipe) {
	}
	;

};

#endif

