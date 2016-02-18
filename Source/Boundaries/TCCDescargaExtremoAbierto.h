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
#ifndef TCCDescargaExtremoAbiertoH
#define TCCDescargaExtremoAbiertoH

#include "TCondicionContorno.h"

//#include <cmath>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include <cstdio>
#include <iostream>
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class TCCDescargaExtremoAbierto: public TCondicionContorno {
  private:

	int FNodoFin; // Nodo del extremo del tubo que conecta con la condicion de contorno.
	int FIndiceCC; // Posicion del vector para tomar datos del tubo para la BC (0 Nodo izquierdo; 1 Nodo derecho)
	double *FComposicion; // Composicion de la atmasfera,el remanso o el deposito matlab(este inicialmente);
	double FPressure; // Presion de descarga.
	double FPref; // Presion de referencia.
	double FTemperaturaDep; // Temperature en la atmasfera o remanso.  (degC)
	double FPerdidaExtremo; // Coeficiente de perdidas en el extremo del tubo.
	double FVelocidadSonidoDep; // Velocity del sonido en el deposito adimensional.
	nmTipoDescarga FTipoDescarga;

	double FGamma3; // Son expresiones con Gamma. Se usan estas variables para no calcularlas tantas veces por instante de tiempo en la misma funcion.
	bool FModeladoEscape;

	double *FCC; // Caracteristica conocida del tubo.
	double *FCD; // Caracteristica desconocida del tubo.

	// void PutPresion(double valor);
	// void PutTemperatura(double valor);

  public:

	void PutComposicion(int i, double valor) {
		FFraccionMasicaEspecie[i] = valor;
	}

	void PutPresion(double valor) {

		FPressure = valor;

	}

	void PutTemperatura(double valor) {
		try {
			double RMezclaDep, CvMezclaDep, CpMezclaDep, GammaDep;

			FTemperaturaDep = valor;
			if(FCalculoEspecies == nmCalculoCompleto) {

				RMezclaDep = CalculoCompletoRMezcla(FComposicion[0], FComposicion[1], FComposicion[2], 0, FCalculoGamma, nmMEP);
				CpMezclaDep = CalculoCompletoCpMezcla(FComposicion[0], FComposicion[1], FComposicion[2], 0, FTemperaturaDep,
													  FCalculoGamma, nmMEP);
				GammaDep = CalculoCompletoGamma(RMezclaDep, CpMezclaDep, FCalculoGamma);

			} else if(FCalculoEspecies == nmCalculoSimple) {

				RMezclaDep = CalculoSimpleRMezcla(FComposicion[0], 0, FCalculoGamma, nmMEP);
				CvMezclaDep = CalculoSimpleCvMezcla(FTemperaturaDep, FComposicion[0], 0, FCalculoGamma, nmMEP);
				GammaDep = CalculoSimpleGamma(RMezclaDep, CvMezclaDep, FCalculoGamma);

			}
			FVelocidadSonidoDep = sqrt(FTemperaturaDep * GammaDep * RMezclaDep) / __cons::ARef;
		} catch(exception & N) {
			std::cout << "ERROR: TCCDescargaExtremoAbierto::PutTemperatura en la condicion de contorno: " << FNumeroCC << std::endl;
			std::cout << "Tipo de error: " << N.what() << std::endl;
			throw Exception(N.what());
		}
	}

	TCCDescargaExtremoAbierto(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
							  nmCalculoGamma GammaCalculation, bool ThereIsEGR);

	~TCCDescargaExtremoAbierto();

	void CalculaCondicionContorno(double Time);

	void ReadBoundaryData(const char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe, int nDPF, TDPF **DPF);

	void AsignAmbientConditions(double Tamb, double Pamb, double *AtmosphericComposition);

	void TuboCalculandose(int TuboActual) {
	}
	;

};

#endif
