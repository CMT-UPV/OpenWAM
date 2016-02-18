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

#include "TDepVolCte.h"

#include "TCCDeposito.h"
#include "TCCUnionEntreDepositos.h"
#include "TTubo.h"
#include "TCompresor.h"
#include "TDPF.h"
#include "TCanalDPF.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TDepVolCte::TDepVolCte(int i, nmTipoCalculoEspecies SpeciesModel, int numeroespecies, nmCalculoGamma GammaCalculation,
					   bool ThereIsEGR) :
	TDepVolCteBase(i, nmDepVolCte, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TDepVolCte::~TDepVolCte() {

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDepVolCte::ActualizaPropiedades(double TimeCalculo) {

	double H = 0.; // Entalpia de entrada
	double Energia = 0.;
	double MasaEntrante, FraccionMasicaAcum = 0.;
	double DeltaT = 0.;
	double g = 0., v = 0., a = 0., m = 0., g1 = 0.;
	int SignoFlujo = 1; // Inicializado por si el flujo esta parado.
	int SignoFlujoED = 1; // Inicializado por si el flujo esta parado.

	try {
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
		double Heat = 0.;
		double MTemp = FGamma / (pow2(FAsonido * __cons::ARef) * FMasa0);

		while(!Converge) {
			H = 0.;
			for(int i = 0; i < FNumeroUniones; i++) {
				if(FCCDeposito[i]->getTipoCC() == nmPipeToPlenumConnection) {
					if(dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getSentidoFlujo() == nmEntrante) {
						SignoFlujo = 1;
					} else if(dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getSentidoFlujo() == nmSaliente) {
						SignoFlujo = -1;
					}
					g = (double) - dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getMassflow();
					if(!FCCDeposito[i]->getUnionDPF()) {
						m = g * DeltaT * FCCDeposito[i]->GetTuboExtremo(0).Pipe->getNumeroConductos();
					} else {
#ifdef ParticulateFilter
						int NumeroCanales = 0;
						int NumeroHaz = FCCDeposito[i]->GetTuboExtremo(0).NumeroHaz;
						int TipoCanal = FCCDeposito[i]->GetTuboExtremo(0).TipoCanal;
						NumeroCanales = FCCDeposito[i]->GetTuboExtremo(0).DPF->GetCanal(NumeroHaz, TipoCanal)->getNumeroCanales();
						m = g * DeltaT * NumeroCanales;
#endif
					}
					v = (double) SignoFlujo * dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getVelocity();
					a = dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getSpeedSound();
					if(FirstStep) {
						MasaEntrante += m;
						for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
							FMasaEspecie[j] += FCCDeposito[i]->GetFraccionMasicaEspecie(j) * m;
						}
					}
					if(v > 0) {
						H += EntalpiaEntrada(a, v, m, Asonido1, FMasa, FCCDeposito[i]->getGamma());
					}
				}
			}
			for(int i = 0; i < FNumeroUnionesED; i++) {

				if(FCCUnionEntreDep[i]->getTipoCC() == nmUnionEntreDepositos) {
					if(FNumeroDeposito == dynamic_cast<TCCUnionEntreDepositos*>(FCCUnionEntreDep[i])->getNumeroDeposito1()) {
						SignoFlujoED = dynamic_cast<TCCUnionEntreDepositos*>(FCCUnionEntreDep[i])->getSentidoFlujoED1();
					} else if(FNumeroDeposito == dynamic_cast<TCCUnionEntreDepositos*>(FCCUnionEntreDep[i])->getNumeroDeposito2()) {
						SignoFlujoED = dynamic_cast<TCCUnionEntreDepositos*>(FCCUnionEntreDep[i])->getSentidoFlujoED2();
					} else {
						printf("ERROR:TDepVolCte::ActualizaPropiedades en el deposito %d, union entre depositos %d\n", FNumeroDeposito, i);
					}
					g = (double) SignoFlujoED * dynamic_cast<TCCUnionEntreDepositos*>(FCCUnionEntreDep[i])->getMassflow();
					m = g * DeltaT;
					a = (double) dynamic_cast<TCCUnionEntreDepositos*>(FCCUnionEntreDep[i])->getSpeedSound();
					v = (double) SignoFlujoED * dynamic_cast<TCCUnionEntreDepositos*>(FCCUnionEntreDep[i])->getVelocity() / __cons::ARef;
					if(FirstStep) {
						MasaEntrante += m;
						for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
							FMasaEspecie[j] += FCCUnionEntreDep[i]->GetFraccionMasicaEspecie(j) * m;
						}
					}
					if(g > 0) {
						H += EntalpiaEntrada(a, 0., m, Asonido1, FMasa, FCCUnionEntreDep[i]->getGamma());

					}

				}
			}

			if(FHayCompresor) {
				g = (double) FCompresorSentido * FCompresor->getMassflow();
				m = g * DeltaT;
				a = FCompresor->getSpeedSound();
				if(FirstStep) {
					MasaEntrante += m;
					for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
						FMasaEspecie[j] += FCompresor->GetFraccionMasicaEspecie(j) * m;
					}
				}
				if(g > 0) {
					H += EntalpiaEntrada(a, 0, m, Asonido1, FMasa, FCompresor->getGamma());
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
			Heat = FHeatPower * DeltaT * (MTemp + FGamma / (pow2(Asonido1 * __cons::ARef) * FMasa)) / 2.;
			Energia = pow(FMasa / FMasa0 * exp((H0 + H) / 2 - Heat), __Gamma::G1(FGamma));

			Asonido1 = FAsonido * sqrt(Energia);
			Error = (Diff = Asonido1 - Asonido0, fabs(Diff)) / Asonido1;
			if(Error > 1e-6) {
				Asonido0 = Asonido1;
			} else {
				Converge = true;
				FAsonido = Asonido1;
			}
		}
		double A2 = pow2(__cons::ARef * FAsonido);
		FPressure = __units::PaToBar(A2 / FGamma / FVolumen * FMasa);
		FPresionIsen = pow(FPressure / FPresRef, __Gamma::G5(FGamma));
		FTemperature = __units::KTodegC(A2 / FGamma / FRMezcla);
		FTime = TimeCalculo;
	} catch(exception & N) {
		std::cout << "ERROR: TDepVolCte::ActualizaPropiedades en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TDepVolCte::UpdateProperties0DModel(double TimeCalculo) {

	ActualizaPropiedades(TimeCalculo);

	AcumulaResultadosMedios(TimeCalculo);

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#pragma package(smart_init)
