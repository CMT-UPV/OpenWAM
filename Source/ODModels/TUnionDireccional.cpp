/* --------------------------------------------------------------------------------*\
|==========================|
 |\\   /\ /\   // O pen     | OpenWAM: The Open Source 1D Gas-Dynamic Code
 | \\ |  X  | //  W ave     |
 |  \\ \/_\/ //   A ction   | CMT-Motores Termicos / Universidad Politecnica Valencia
 |   \\/   \//    M odel    |
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
#pragma hdrstop

#include "TUnionDireccional.h"

#include "TCCDeposito.h"
#include "TCondicionContorno.h"
#include "TTubo.h"
#include "TTipoValvula.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TUnionDireccional::TUnionDireccional(int i, int NUnionDireccional, nmTipoCalculoEspecies SpeciesModel,
									 int numeroespecies, nmCalculoGamma GammaCalculation, bool ThereIsEGR) :
	TDepVolCteBase(i, nmUnionDireccional, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {

	FNumUnionDireccional = NUnionDireccional;
	FCCEntrada = NULL;

	FNodoEntrada = NULL;
	FSentidoEntrada = NULL;
	FCDSalidaInicial = NULL;
	FVelocidadCorte = NULL;
	FVelocidadFin = NULL;
	FVelocity = NULL;
	FCoefA = NULL;
	FCoefB = NULL;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TUnionDireccional::~TUnionDireccional() {

	if(FNodoEntrada != NULL)
		delete[] FNodoEntrada;
	if(FSentidoEntrada != NULL)
		delete[] FSentidoEntrada;
	if(FCDSalidaInicial != NULL)
		delete[] FCDSalidaInicial;
	if(FVelocidadCorte != NULL)
		delete[] FVelocidadCorte;
	if(FVelocidadFin != NULL)
		delete[] FVelocidadFin;
	if(FVelocity != NULL)
		delete[] FVelocity;
	if(FCoefA != NULL)
		delete[] FCoefA;
	if(FCoefB != NULL)
		delete[] FCoefB;

	if(FCCEntrada != NULL)
		delete[] FCCEntrada;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TUnionDireccional::LeeDatosUnionDireccional(const char *FileWAM, fpos_t &filepos) {
	try {
		int numid = 0; // Dato para Wamer

		FNodoEntrada = new int[2];
		FSentidoEntrada = new int[2];
		FCDSalidaInicial = new double[2];
		FVelocidadCorte = new double[2];
		FVelocidadFin = new double[2];
		FVelocity = new double[2];
		FCoefA = new double[2];
		FCoefB = new double[2];

		FCCEntrada = new TCondicionContorno*[2];

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &numid); /* DATO PARA WAMER */
		fscanf(fich, "%d %d %d ", &FNodoEntrada[0], &FNodoEntrada[1], &FNodoSalida);
		/* Lectura de informacion para el calculo del coeficiente de descarga de salida
		 para las uniones de entrada al deposito de union direccional */
		fscanf(fich, "%lf %lf %lf ", &FCDSalidaInicial[0], &FVelocidadCorte[0], &FVelocidadFin[0]);
		fscanf(fich, "%lf %lf %lf ", &FCDSalidaInicial[1], &FVelocidadCorte[1], &FVelocidadFin[1]);

		fgetpos(fich, &filepos);
		fclose(fich);

		FCoefA[0] = -FCDSalidaInicial[0] * FVelocidadFin[0] / (FVelocidadCorte[0] - FVelocidadFin[0]);
		FCoefB[0] = FCDSalidaInicial[0] / (FVelocidadCorte[0] - FVelocidadFin[0]);
		FCoefA[1] = -FCDSalidaInicial[1] * FVelocidadFin[1] / (FVelocidadCorte[1] - FVelocidadFin[1]);
		FCoefB[1] = FCDSalidaInicial[1] / (FVelocidadCorte[1] - FVelocidadFin[1]);

	} catch(exception & N) {
		std::cout << "ERROR: TUnionDireccional::LeeDatosUnionDireccional en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TUnionDireccional::AsignaCCUnionDireccional() {
	try {

		if(FTipoDeposito == nmUnionDireccional) {
			for(int i = 0; i < FNumeroUniones; i++) {
				if(FCCDeposito[i]->getNumeroCC() == FNodoEntrada[0]) {
					FCCEntrada[0] = FCCDeposito[i];
				} else if(FCCDeposito[i]->getNumeroCC() == FNodoEntrada[1]) {
					FCCEntrada[1] = FCCDeposito[i];
				} else if(FCCDeposito[i]->getNumeroCC() == FNodoSalida) {
					FCCSalida = FCCDeposito[i];
				}
			}
		}

	} catch(exception & N) {
		std::cout << "ERROR: TUnionDireccional::AsignaCCUnionDireccional en la union direccional " << FNumUnionDireccional <<
				  std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TUnionDireccional::ActualizaPropiedades(double TimeCalculo) {
	try {
		double H = 0.; // Entalpia de entrada
		double Energia = 0.;
		double MasaEntrante, FraccionMasicaAcum = 0.;
		double DeltaT = 0.;
		double g = 0., v = 0., a = 0., m = 0.;
		int SignoFlujo = 1;

		FMasa0 = FMasa;
		MasaEntrante = 0.;
		H = 0.;
		DeltaT = TimeCalculo - FTime;

		if(FCalculoEspecies == nmCalculoCompleto) {

			FRMezcla = CalculoCompletoRMezcla(FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2], 0,
											  FCalculoGamma, nmMEP);
			FCpMezcla = CalculoCompletoCpMezcla(FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2], 0,
												__units::degCToK(FTemperature), FCalculoGamma, nmMEP);
			FGamma = CalculoCompletoGamma(FRMezcla, FCpMezcla, FCalculoGamma);

		} else if(FCalculoEspecies == nmCalculoSimple) {

			FRMezcla = CalculoSimpleRMezcla(FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FCalculoGamma, nmMEP);
			FCvMezcla = CalculoSimpleCvMezcla(__units::degCToK(FTemperature), FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1],
											  FCalculoGamma, nmMEP);
			FGamma = CalculoSimpleGamma(FRMezcla, FCvMezcla, FCalculoGamma);

		}

		bool Converge = false;
		bool FirstStep = true;
		double H0 = 0.;
		double Asonido0 = FAsonido;
		double Asonido1 = FAsonido;
		double Error = 0.;
		double Diff = 0.;

		while(!Converge) {
			H = 0.;
			for(int i = 0; i < FNumeroUniones; i++) {
				if(dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getSentidoFlujo() == nmEntrante) {
					SignoFlujo = 1;
				} else if(dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getSentidoFlujo() == nmSaliente) {
					SignoFlujo = -1;
				}
				g = (double) - dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getMassflow();
				v = (double) SignoFlujo * dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getVelocity();
				a = dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getSpeedSound();
				m = g * DeltaT * FCCDeposito[i]->GetTuboExtremo(0).Pipe->getNumeroConductos();
				if(FirstStep) {
					MasaEntrante += m;
					for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
						FMasaEspecie[j] += FCCDeposito[i]->GetFraccionMasicaEspecie(j) * m;
					}
				}
				if(v > 0) {
					H += EntalpiaEntrada(a, v, m, FAsonido, FMasa, FCCDeposito[i]->getGamma());
				}

			}
			if(FirstStep) {
				FMasa = FMasa0 + MasaEntrante;
				for(int j = 0; j < FNumeroEspecies - 2; j++) {
					FFraccionMasicaEspecie[j] = FMasaEspecie[j] / FMasa;
					FraccionMasicaAcum += FFraccionMasicaEspecie[j];
				}
				FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
				if(FHayEGR)
					FFraccionMasicaEspecie[FNumeroEspecies - 1] = FMasaEspecie[FNumeroEspecies - 1] / FMasa;
				FirstStep = false;
				H0 = H;
			}

			Energia = pow(FMasa / FMasa0 * exp((H + H0) / 2), __Gamma::G1(FGamma));
			Asonido1 = FAsonido * sqrt(Energia);
			Error = (Diff = Asonido1 - Asonido0, fabs(Diff)) / Asonido1;
			if(Error > 1e-6) {
				Asonido0 = Asonido1;
			} else {
				FAsonido = Asonido1;
				Converge = true;
			}
		}
		FTemperature = __units::KTodegC(pow2(FAsonido * __cons::ARef) / (FGamma * FRMezcla));
		FPressure = __units::PaToBar(pow2(__cons::ARef * FAsonido) / FGamma / FVolumen * FMasa);
		FPresionIsen = pow(FPressure / FPresRef, __Gamma::G5(FGamma));
		FTime = TimeCalculo;
	} catch(exception & N) {
		std::cout << "ERROR: TUnionDireccional::ActualizaPropiedades en la union direccional: " << FNumUnionDireccional <<
				  std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TUnionDireccional::CalculoUnionDireccional() {
	try {

		double coefA = 0., coefB = 0.;
		/* Parametro independiente y pendiente de la recta para el calculo del Coeficiende de Descarga */

		for(int i = 0; i < 2; i++) {
			if(dynamic_cast<TCCDeposito*>(FCCEntrada[i])->getSentidoFlujo() == nmEntrante) {
				FSentidoEntrada[i] = 1;
			} else if(dynamic_cast<TCCDeposito*>(FCCEntrada[i])->getSentidoFlujo() == nmSaliente) {
				FSentidoEntrada[i] = -1;
			} else
				FSentidoEntrada[i] = 0; // Flujo parado
			FVelocity[i] = FSentidoEntrada[i] * dynamic_cast<TCCDeposito*>(FCCEntrada[i])->getVelocity() * __cons::ARef;
		}

		/* Calculo del coeficiente de descarga de salida en el Pipe de Entrada 0 */
		if(FVelocity[1] <= FVelocidadCorte[0]) {
			dynamic_cast<TCCDeposito*>(FCCEntrada[0])->PutCDSalida(FCDSalidaInicial[0]);
		} else if(FVelocity[1] >= FVelocidadFin[0]) {
			dynamic_cast<TCCDeposito*>(FCCEntrada[0])->PutCDSalida(0);
		} else {
			dynamic_cast<TCCDeposito*>(FCCEntrada[0])->PutCDSalida(FCoefA[0] + FCoefB[0] * FVelocity[1]);

		}

		/* Calculo del coeficiente de descarga de salida en el Pipe de Entrada 1 */
		if(FVelocity[0] <= FVelocidadCorte[1]) {
			dynamic_cast<TCCDeposito*>(FCCEntrada[1])->PutCDSalida(FCDSalidaInicial[1]);
		} else if(FVelocity[0] >= FVelocidadFin[0]) {
			dynamic_cast<TCCDeposito*>(FCCEntrada[1])->PutCDSalida(0);
		} else {
			dynamic_cast<TCCDeposito*>(FCCEntrada[1])->PutCDSalida(FCoefA[1] + FCoefB[1] * FVelocity[0]);

		}

	} catch(exception & N) {
		std::cout << "ERROR: TUnionDireccional::CalculoUnionDireccional en la union direccional: " << FNumUnionDireccional <<
				  std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TUnionDireccional::UpdateProperties0DModel(double TimeCalculo) {
	ActualizaPropiedades(TimeCalculo);

	CalculoUnionDireccional();

	AcumulaResultadosMedios(TimeCalculo);

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#pragma package(smart_init)
