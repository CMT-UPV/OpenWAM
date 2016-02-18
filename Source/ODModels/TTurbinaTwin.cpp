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
#pragma hdrstop

#include "TTurbinaTwin.h"

#include "TCondicionContorno.h"
#include "TCCDeposito.h"
#include "TRotorTurbina.h"
#include "TEstatorTurbina.h"
#include "TTubo.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TTurbinaTwin::TTurbinaTwin(int i, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
						   nmCalculoGamma GammaCalculation, bool ThereIsEGR) :
	TTurbina(i, nmTurbinaTwin, 2, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {

	FTime = 0.;

	FResMediosTurbina.RelaCinematicaMED = new double[FNumeroEntradas];
	FResMediosTurbina.GastoCorregidoMED = new double[FNumeroEntradas];
	FResMediosTurbina.GastoCorregidoSUM = new double[FNumeroEntradas];
	FResMediosTurbina.RegimenCorregidoMED = new double[FNumeroEntradas];
	FResMediosTurbina.RegimenCorregidoSUM = new double[FNumeroEntradas];
	FResMediosTurbina.RelacionExpansionMED = new double[FNumeroEntradas];
	FResMediosTurbina.RelacionExpansionSUM = new double[FNumeroEntradas];

	FResMediosTurbina.Trabajo = false;
	FResMediosTurbina.Rendimiento = false;
	FResMediosTurbina.RelaCinematica = false;
	FResMediosTurbina.GastoCorregido = false;
	FResMediosTurbina.RegimenCorregido = false;
	FResMediosTurbina.RelacionExpansion = false;

	FResInstantTurbina.RelaCinematicaINS = new double[FNumeroEntradas];
	FResInstantTurbina.GastoCorregidoINS = new double[FNumeroEntradas];
	FResInstantTurbina.RegimenCorregidoINS = new double[FNumeroEntradas];
	FResInstantTurbina.RelacionExpansionINS = new double[FNumeroEntradas];

	FResInstantTurbina.Potencia = false;
	FResInstantTurbina.Rendimiento = false;
	FResInstantTurbina.RelaCinematica = false;
	FResInstantTurbina.GastoCorregido = false;
	FResInstantTurbina.RegimenCorregido = false;
	FResInstantTurbina.RelacionExpansion = false;

	FResInstantTurbina.GastoCorregido = false;
	FResInstantTurbina.RegimenCorregido = false;

	for(int i = 0; i < FNumeroEntradas; i++) {
		FResMediosTurbina.RelaCinematicaMED[i] = 0.;
		FResMediosTurbina.GastoCorregidoMED[i] = 0.;
		FResMediosTurbina.GastoCorregidoSUM[i] = 0.;
		FResMediosTurbina.RegimenCorregidoMED[i] = 0.;
		FResMediosTurbina.RegimenCorregidoSUM[i] = 0.;
		FResMediosTurbina.RelacionExpansionMED[i] = 0.;
		FResMediosTurbina.RelacionExpansionSUM[i] = 0.;
		FResInstantTurbina.RelaCinematicaINS[i] = 0.;
		FResInstantTurbina.GastoCorregidoINS[i] = 0.;
		FResInstantTurbina.RegimenCorregidoINS[i] = 0.;
		FResInstantTurbina.RelacionExpansionINS[i] = 0.;
	}

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TTurbinaTwin::~TTurbinaTwin() {

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaTwin::AsignaEntradaSalidaCC() {
	try {
		int contador = 0;

		FCCEntrada = new TCondicionContorno*[FNumeroEntradas];
		FCCSalida = new TCondicionContorno*[1];
		for(int i = 0; i < FNumeroUniones; i++) {
			if(dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getValvula()->getTypeOfValve() == nmStator) {
				FCCEntrada[contador] = FCCDeposito[i];
				contador++;
			} else if(dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getValvula()->getTypeOfValve() == nmRotor) {
				FCCSalida[0] = FCCDeposito[i];
			}
		}

		dynamic_cast<TEstatorTurbina *>(dynamic_cast<TCCDeposito *>(FCCEntrada[0])->getValvula())->AsignaTurbina(FNumeroTurbina,
				0);
		dynamic_cast<TEstatorTurbina *>(dynamic_cast<TCCDeposito *>(FCCEntrada[1])->getValvula())->AsignaTurbina(FNumeroTurbina,
				1);
		dynamic_cast<TRotorTurbina *>(dynamic_cast<TCCDeposito *>(FCCSalida[0])->getValvula())->AsignaTurbina(FNumeroTurbina);

		if(FTipoTurbina == nmFixedTurbine) {
			dynamic_cast<TEstatorTurbina *>(dynamic_cast<TCCDeposito *>(FCCEntrada[0])->getValvula())->TipodeEstator(nmStFijo);
			dynamic_cast<TEstatorTurbina *>(dynamic_cast<TCCDeposito *>(FCCEntrada[1])->getValvula())->TipodeEstator(nmStFijo);
			dynamic_cast<TRotorTurbina *>(dynamic_cast<TCCDeposito *>(FCCSalida[0])->getValvula())->TipodeRotor(nmRotFijo);
		} else if(FTipoTurbina == nmTurbineMap) {
			dynamic_cast<TEstatorTurbina*>(dynamic_cast<TCCDeposito*>(FCCEntrada[0])->getValvula())->TipodeEstator(nmStMapa);
			dynamic_cast<TEstatorTurbina*>(dynamic_cast<TCCDeposito*>(FCCEntrada[1])->getValvula())->TipodeEstator(nmStMapa);
			dynamic_cast<TRotorTurbina*>(dynamic_cast<TCCDeposito*>(FCCSalida[0])->getValvula())->TipodeRotor(nmRotMapa);
		} else {
			dynamic_cast<TEstatorTurbina *>(dynamic_cast<TCCDeposito *>(FCCEntrada[0])->getValvula())->TipodeEstator(nmStVariable);
			dynamic_cast<TEstatorTurbina *>(dynamic_cast<TCCDeposito *>(FCCEntrada[1])->getValvula())->TipodeEstator(nmStVariable);
			dynamic_cast<TRotorTurbina *>(dynamic_cast<TCCDeposito *>(FCCSalida[0])->getValvula())->TipodeRotor(nmRotVariable);
		}

	} catch(exception &N) {
		std::cout << "ERROR: TTurbinaTwin::AsignaEntradaSalidaCC en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void TTurbinaTwin::CalculaCondicionTurbina(double TimeCalculo) {

	double cpte[2], cpts /* ,gammaent[2],gammasal,cvte[2],cvts,Rentrada[2],Rsalida */;
	// double B,Cte_R,A;
	double TempIsentSal[2];
	double DeltaT = 0.;
	double incrRelCin = 0.;
	double TrabIsen = 0.;
	double Landa = 0., Beta = 0., Entropia = 0.;
	int SentidoEntrada[1], SentidoSalida;
	int dd = 0, b = 0, c = 0, d = 0;
	double gam_med = 0.;
	double gam_te = 0.;
	double cp_med[2];
	double AbsoluteMass = 0;
	dVector EntalEntr;
	EntalEntr.resize(2);

	try {

		DeltaT = TimeCalculo - FTimeTurbina;
		FTimeTurbina = TimeCalculo;

		if(DeltaT > 0) {
			FTrabajoIsenInstTotal = 0.;

			// Calculo de las propiedades del gas a la salida

			Landa = FCCSalida[0]->GetTuboExtremo(0).Landa;
			Beta = FCCSalida[0]->GetTuboExtremo(0).Beta;
			Entropia = FCCSalida[0]->GetTuboExtremo(0).Entropia;

			TransformaContorno(&Landa, &Beta, &Entropia, &FAsonidoSalida, &FVelocidadSalida, &FPresionSalida, 1,
							   FCCSalida[0]->getGamma());

			if(dynamic_cast<TCCDeposito*>(FCCSalida[0])->getSentidoFlujo() == nmEntrante) {
				SentidoSalida = 1;
			} else if(dynamic_cast<TCCDeposito*>(FCCSalida[0])->getSentidoFlujo() == nmSaliente) {
				SentidoSalida = -1;
			} else
				SentidoSalida = 0; /* Flujo parado */

			FTempSalida = pow2(FAsonidoSalida * __cons::ARef) / (FCCSalida[0]->getGamma() * FCCSalida[0]->getR());
			FVelocidadSalida *= __cons::ARef * SentidoSalida;
			FRhoSalida = __units::BarToPa(FPresionSalida) / FTempSalida / FCCSalida[0]->getR();
			FGastoSalida = FRhoSalida * FVelocidadSalida * dynamic_cast<TCCDeposito*>(FCCSalida[0])->getSeccionTubo();

			// Calculo de las propiedades a la entrada
			for(int i = 0; i < 2; i++) {

				Landa = FCCEntrada[i]->GetTuboExtremo(0).Landa;
				Beta = FCCEntrada[i]->GetTuboExtremo(0).Beta;
				Entropia = FCCEntrada[i]->GetTuboExtremo(0).Entropia;

				TransformaContorno(&Landa, &Beta, &Entropia, &FAsonidoEntrada[i], &FVelocidadEntrada[i], &FPresionEntrada[i], 1,
								   FCCEntrada[i]->getGamma());

				if(dynamic_cast<TCCDeposito*>(FCCEntrada[i])->getSentidoFlujo() == nmEntrante) {
					SentidoEntrada[i] = 1;
				} else if(dynamic_cast<TCCDeposito*>(FCCEntrada[i])->getSentidoFlujo() == nmSaliente) {
					SentidoEntrada[i] = -1;
				} else
					SentidoEntrada[i] = 0; /* Flujo parado */

				FTempEntrada[i] = pow2(FAsonidoEntrada[i] * __cons::ARef) / (FCCEntrada[i]->getGamma() * FCCEntrada[i]->getR());
				FVelocidadEntrada[i] *= __cons::ARef * SentidoEntrada[i];
				FRhoEntrada[i] = __units::BarToPa(FPresionEntrada[i]) / FTempEntrada[i] / FCCEntrada[i]->getR();
				FGastoEntrada[i] = FRhoEntrada[i] * FVelocidadEntrada[i] * dynamic_cast<TCCDeposito*>(FCCEntrada[i])->getSeccionTubo();
			}

			if(FGastoEntrada[0] <= 0. && FGastoEntrada[1] > 0.) {
				FGastoEntrada[1] = FGastoEntrada[1] + FGastoEntrada[0];
				FGastoEntrada[0] = 0.;
			} else if(FGastoEntrada[1] <= 0. && FGastoEntrada[0] > 0.) {
				FGastoEntrada[0] = FGastoEntrada[1] + FGastoEntrada[0];
				FGastoEntrada[1] = 0.;
			} else if(FGastoEntrada[0] <= 0. && FGastoEntrada[1] <= 0.) {
				FGastoEntrada[1] = 0.;
				FGastoEntrada[0] = 0.;
			}
			AbsoluteMass = fabs(FGastoEntrada[0]) + fabs(FGastoEntrada[1]);

			for(int i = 0; i < 2; i++) {
				if(FCalculoEspecies == nmCalculoCompleto) {
					cpte[i] = CpTurbineComplete(FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2],
												FTempEntrada[i]);
				} else {
					cpte[i] = CpTurbineSimple(FTempEntrada[i], FFraccionMasicaEspecie[0]);

				}
			}

			FRMezcla = 287;

			for(int i = 0; i < 2; i++) {

				FTemp0Entrada[i] = FTempEntrada[i] + pow2(FVelocidadEntrada[i]) / (2. * cpte[i]);

				gam_te = cpte[i] / (cpte[i] - FRMezcla);

				FPresion0Entrada[i] = FPresionEntrada[i] * pow(FTemp0Entrada[i] / FTempEntrada[i], (gam_te / (gam_te - 1.0)));

				FGastoCorregido[i] = FGastoEntrada[i] * sqrt(FTemp0Entrada[i]) / FPresion0Entrada[i] / 0.1;
				FRegimenCorregido[i] = FRegimen / sqrt(FTemp0Entrada[i]);

				if(FCalculoEspecies == nmCalculoCompleto) {
					cp_med[i] = CpTurbineComplete(FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2],
												  (FTempEntrada[i] + FTempSalida) / 2.);
				} else {
					cp_med[i] = CpTurbineSimple((FTempEntrada[i] + FTempSalida) / 2., FFraccionMasicaEspecie[0]);
				}
				TempIsentSal[i] = FTemp0Entrada[i] * pow(FPresionSalida / FPresion0Entrada[i], (FRMezcla / cp_med[i]));

				if(FCalculoEspecies == nmCalculoCompleto) {
					cp_med[i] = CpTurbineComplete(FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2],
												  (FTempEntrada[i] + TempIsentSal[i]) / 2.);
				} else {
					cp_med[i] = CpTurbineSimple((FTempEntrada[i] + TempIsentSal[i]) / 2., FFraccionMasicaEspecie[0]);
				}

				FEntalpia0Entrada[i] = cp_med[i] * FTemp0Entrada[i];
				EntalEntr[i] = cp_med[i] * FTempEntrada[i];
				FEntalpiaIsenSalida[i] = cp_med[i] * TempIsentSal[i];

				FRelacionExpansion[i] = FPresion0Entrada[i] / FPresionSalida;
			}

			if(FTipoTurbina == nmTurbineMap) {
				if(FRackIsControlled) {
					FRack = FRackController->Output(FTime);
				}
				double MassAcum = 0.;
				double RotorEF = 0.;

				for(int i = 0; i < 2; i++) {

					FMapa->CurrentEffectiveSection(FRegimenCorregido[i] / 60., FRelacionExpansion[i], FRack,
												   __units::degCToK(FTemperature) / FTemp0Entrada[i]);

					dynamic_cast<TEstatorTurbina*>(dynamic_cast<TCCDeposito*>(FCCEntrada[i])->getValvula())->PutAreaEff(
						FMapa->StatorEF() / 2.);

					if(FGastoEntrada[i] > 0) {
						RotorEF += FMapa->RotorEF() * FGastoEntrada[i];
						MassAcum += FGastoEntrada[i];
					}
					FRendTurbina[i] = FMapa->EffTurb();
				}
				if(MassAcum > 0)
					RotorEF /= MassAcum;
				dynamic_cast<TRotorTurbina*>(dynamic_cast<TCCDeposito*>(FCCSalida[0])->getValvula())->PutAreaEff(RotorEF);

			}

			// Calculo del rendimiento de la turbina

			if((FGastoEntrada[0] > 0. || FGastoEntrada[1] > 0.) && ((FEntalpia0Entrada[0] - FEntalpiaIsenSalida[0]) >= 0.
					&& (FEntalpia0Entrada[1] - FEntalpiaIsenSalida[1]) >= 0.)) {

				for(int i = 0; i < 2; i++) {

					if((FEntalpia0Entrada[i] - FEntalpiaIsenSalida[i]) > 0.) {
						FRelacionCinematica[i] = __units::RPMToRPS(FRegimen) * __cons::Pi * FDiametroRodete / sqrt(2 *
												 (FEntalpia0Entrada[i] - FEntalpiaIsenSalida[i]));
					} else {
						FRelacionCinematica[i] = 0.;
					}

					if(FCalRendTurbina == nmWatson) {
						if(FRelacionCinematica[i] <= 0 || FRelacionCinematica[i] >= 1.19) {
							FRendTurbina[i] = 0;
						}
						FRendTurbina[i] = 0.004022 + 1.55766 * FRelacionCinematica[i] - 0.511626 * pow2(FRelacionCinematica[i]) - 0.121795 *
										  pow3(FRelacionCinematica[i]) - 0.445804 * pow4(
											  FRelacionCinematica[i]);

					} else if(FCalRendTurbina == nmPolinomio) {
						if(FRelacionCinematica[i] >= FRcmaxima || FRelacionCinematica[i] <= 0) {
							FRendTurbina[i] = 0.;
						} else {
							dd = 2. * pow3(FRcoptima) * pow2(FRcmaxima) - pow2(FRcoptima) * pow3(FRcmaxima) - pow4(FRcoptima) * FRcmaxima;
							b = FRendmaximo * (3. * pow2(FRcmaxima) * pow2(FRcoptima) - 2 * pow3(FRcmaxima) * FRcoptima) / dd;
							c = FRendmaximo * (pow3(FRcmaxima) - 3. * FRcmaxima * pow2(FRcoptima)) / dd;
							d = FRendmaximo * (2. * FRcmaxima * FRcoptima - pow2(FRcmaxima)) / dd;

							FRendTurbina[i] = b * FRelacionCinematica[i] + c * pow2(FRelacionCinematica[i]) + d * pow3(FRelacionCinematica[i]);
						}
					} else if(FCalRendTurbina == nmCalcExtRD) {
						// Todavia no esta hecha la asignacion de FNumeroTurbinaTGV
						if(FDatosTGV[FNumeroTurbinaTGV].Rendimiento[i] < 0) {
							FRendTurbina[i] = 0.;
						} else {
							FRendTurbina[i] = FDatosTGV[FNumeroTurbinaTGV].Rendimiento[i];
						}
					} else if(FCalRendTurbina == nmRendMapa) {

					} else {
						std::cout << "ERROR: Calculo del rendimiento de la turbina desconocido" << std::endl;
						throw Exception("");
					}
					FRendTurbina[i] = FRendTurbina[i] * FAjustRendTurb;
				}
			} else {
				FRelacionCinematica[0] = 0.;
				FRelacionCinematica[1] = 0.;
				FRendTurbina[0] = 0.;
				FRendTurbina[1] = 0.;
			}
			FTrabajoFluido = 0;
			incrRelCin = 0;

			for(int i = 0; i < 2; i++) {
				FTrabajoIsen = FGastoEntrada[i] * (FEntalpia0Entrada[i] - FEntalpiaIsenSalida[i]) * DeltaT;
				FTrabajoIsenInstTotal += FTrabajoIsen;
				FTrabajoFluido += FTrabajoIsen * FRendTurbina[i] - FGastoEntrada[i] * (FEntalpia0Entrada[i] - EntalEntr[i]) * DeltaT;
				FTrabajoReal += FTrabajoIsen * FRendTurbina[i];
				// Lo usa para calcular el rendimiento medio.
				FTrabajoRealPaso += FTrabajoIsen * FRendTurbina[i];
				// Para el calculo de la potencia del paso.
				// FRendInstantaneo+=FRendTurbina[i]*FGastoEntrada[i];
				// FGastoEntradaTotal+=FGastoEntrada[i];
				incrRelCin = FRendTurbina[i] * FRelacionCinematica[i] * FTrabajoIsen;

				FTrabajoTotal += FTrabajoIsen;
				FRelacionCinAcum[i] += incrRelCin;
				FRelacionCinGlobalAcum += incrRelCin;
				FPonderacionRelacionCinematica[i] += FRendTurbina[i] * FTrabajoIsen;

				FGastoCorregido[i] = FGastoEntrada[i] * sqrt(FTemp0Entrada[i]) / FPresion0Entrada[i] / 0.1;
				FRegimenCorregido[i] = FRegimen / sqrt(FTemp0Entrada[i]);
				FRelacionExpansion[i] = FPresion0Entrada[i] / FPresionSalida;
			}

			if(DeltaT != 0) {
				FPotencia = FTrabajoFluido / DeltaT;
			} else
				FPotencia = 0.;

			FDeltaPaso += DeltaT;
		} else {
			FTrabajoIsenInstTotal = 0.;
			FTrabajoFluido = 0.;
		}
	} catch(exception &N) {
		std::cout << "ERROR: TTurbinaTwin::CalculaCondicionTurbina en la turbina: " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaTwin::ReadAverageResultsTurb(const char *FileWAM, fpos_t &filepos) {
	try {
		int nvars = 0, var = 0;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &nvars);
		for(int i = 0; i < nvars; i++) {
			fscanf(fich, "%d ", &var);
			switch(var) {
			case 0:
				FResMediosTurbina.Trabajo = true;
				break;
			case 1:
				FResMediosTurbina.Rendimiento = true;
				break;
			case 2:
				FResMediosTurbina.RelaCinematica = true;
				break;
			case 3:
				FResMediosTurbina.GastoCorregido = true;
				break;
			case 4:
				FResMediosTurbina.RegimenCorregido = true;
				break;
			case 5:
				FResMediosTurbina.RelacionExpansion = true;
				break;
			default:
				std::cout << "Resultados medios en turbina " << FNumeroTurbina << " no implementados " << std::endl;
			}
		}

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TTurbinaTwin::ReadAverageResultsTurb en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaTwin::CabeceraResultadosMedTurb(stringstream& medoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");

		std::string Label;

		if(FResMediosTurbina.Trabajo) {
			Label = "\t" + PutLabel(511) + std::to_string(FNumeroTurbina) + PutLabel(907);
			medoutput << Label.c_str();
		}
		if(FResMediosTurbina.Rendimiento) {
			Label = "\t" + PutLabel(512) + std::to_string(FNumeroTurbina) + PutLabel(901);
			medoutput << Label.c_str();
		}
		if(FResMediosTurbina.RelaCinematica) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				Label = "\t" + PutLabel(513) + std::to_string(i + 1) + PutLabel(514) + std::to_string(FNumeroTurbina) + PutLabel(901);
				medoutput << Label.c_str();
			}
			// fprintf(fich,"\tRelacion_cinematica_global_turb_%d(-)",std::to_string(FNumeroTurbina));
		}
		if(FResMediosTurbina.GastoCorregido) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				Label = "\t" + PutLabel(516) + std::to_string(i + 1) + PutLabel(514) + std::to_string(FNumeroTurbina) + PutLabel(905);
				medoutput << Label.c_str();
			}
		}
		if(FResMediosTurbina.RegimenCorregido) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				Label = "\t" + PutLabel(517) + std::to_string(i + 1) + PutLabel(514) + std::to_string(FNumeroTurbina) + PutLabel(906);
				medoutput << Label.c_str();
			}
		}
		if(FResMediosTurbina.RelacionExpansion) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				Label = "\t" + PutLabel(518) + std::to_string(i + 1) + PutLabel(514) + std::to_string(FNumeroTurbina) + PutLabel(901);
				medoutput << Label.c_str();
			}
		}

		// fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TTurbinaTwin::CabeceraResultadosMedTurb en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaTwin::IniciaMedias() {
	try {

		FResMediosTurbina.TiempoSUM = 0.;
		FResMediosTurbina.TrabajoSUM = 0.;
		FResMediosTurbina.Tiempo0 = 0.;

	} catch(exception &N) {
		std::cout << "ERROR: TTurbinaTwin::IniciaMedias en el turbina: " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: TTurbinaTwin::IniciaMedias en el turbina: " + std::to_string(FNumeroTurbina) + N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaTwin::AcumulaMedias(double Tiempo) {
	double DeltaT = 0.;
	try {
		DeltaT = Tiempo - FResMediosTurbina.Tiempo0;
		FResMediosTurbina.TiempoSUM += DeltaT;
		FResMediosTurbina.TrabajoSUM = FTrabajoReal;
		FResMediosTurbina.Tiempo0 = Tiempo;
		for(int i = 0; i < FNumeroEntradas; i++) {
			if(FResMediosTurbina.GastoCorregido) {
				FResMediosTurbina.GastoCorregidoSUM[i] += FGastoCorregido[i] * DeltaT;
			}
			if(FResMediosTurbina.RegimenCorregido) {
				FResMediosTurbina.RegimenCorregidoSUM[i] += FRegimenCorregido[i] * DeltaT;
			}
			if(FResMediosTurbina.RelacionExpansion) {
				FResMediosTurbina.RelacionExpansionSUM[i] += FRelacionExpansion[i] * DeltaT;
			}
		}
	} catch(exception &N) {
		std::cout << "ERROR: TTurbinaTwin::AcumulaMedias en la turbina: " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: TTurbinaTwin::AcumulaMedias en la Turbine: " + std::to_string(FNumeroTurbina) + N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaTwin::ImprimeResultadosMedTurb(stringstream& medoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");

		if(FResMediosTurbina.Trabajo)
			medoutput << "\t" << FResMediosTurbina.TrabajoMED;
		if(FResMediosTurbina.Rendimiento)
			medoutput << "\t" << FResMediosTurbina.RendimientoMED;
		if(FResMediosTurbina.RelaCinematica) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				medoutput << "\t" << FResMediosTurbina.RelaCinematicaMED[i];
			}
			// fprintf(fich,"\t%g",FResMediosTurbina.RelaCinematicaGlobalMED);
		}
		if(FResMediosTurbina.GastoCorregido) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				medoutput << "\t" << FResMediosTurbina.GastoCorregidoMED[i];
			}
		}
		if(FResMediosTurbina.RegimenCorregido) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				medoutput << "\t" << FResMediosTurbina.RegimenCorregidoMED[i];
			}
		}
		if(FResMediosTurbina.RelacionExpansion) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				medoutput << "\t" << FResMediosTurbina.RelacionExpansionMED[i];
			}
		}

		// fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TTurbinaTwin::ImprimerResultadosMedTurb en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaTwin::CalculaResultadosMediosTurb() {
	try {

		if(FResMediosTurbina.Trabajo) {
			FResMediosTurbina.TrabajoMED = FResMediosTurbina.TrabajoSUM;
		}
		if(FResMediosTurbina.Rendimiento) {
			if(FTrabajoTotal != 0.) {
				FResMediosTurbina.RendimientoMED = FTrabajoReal / FTrabajoTotal;
			} else {
				FResMediosTurbina.RendimientoMED = 0.;
			}
		}
		if(FResMediosTurbina.RelaCinematica) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				FResMediosTurbina.RelaCinematicaMED[i] = FRelacionCinAcum[i] / FPonderacionRelacionCinematica[i];
			}
			/* if(FTrabajoReal!=0.){
			 FResMediosTurbina.RelaCinematicaGlobalMED=FRelacionCinGlobalAcum/FTrabajoReal;
			 }else{
			 FResMediosTurbina.RelaCinematicaGlobalMED=0.;
			 } */
		}
		if(FResMediosTurbina.GastoCorregido) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				FResMediosTurbina.GastoCorregidoMED[i] = FResMediosTurbina.GastoCorregidoSUM[i] / FResMediosTurbina.TiempoSUM;
			}
		}
		if(FResMediosTurbina.RegimenCorregido) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				FResMediosTurbina.RegimenCorregidoMED[i] = FResMediosTurbina.RegimenCorregidoSUM[i] / FResMediosTurbina.TiempoSUM;
			}
		}
		if(FResMediosTurbina.RelacionExpansion) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				FResMediosTurbina.RelacionExpansionMED[i] = FResMediosTurbina.RelacionExpansionSUM[i] / FResMediosTurbina.TiempoSUM;
			}
		}

		for(int i = 0; i < FNumeroEntradas; i++) {
			FRelacionCinAcum[i] = 0.;
			FResMediosTurbina.GastoCorregidoSUM[i] = 0.;
			FResMediosTurbina.RegimenCorregidoSUM[i] = 0.;
			FResMediosTurbina.RelacionExpansionSUM[i] = 0.;
			FPonderacionRelacionCinematica[i] = 0.;
		}
		FRelacionCinGlobalAcum = 0.;
		FTrabajoReal = 0.;
		FTrabajoTotal = 0.;
		FResMediosTurbina.TrabajoSUM = 0.;
		FResMediosTurbina.TiempoSUM = 0.;

	} catch(exception &N) {
		std::cout << "ERROR: TTurbinaTwin::CalculaResultadosMediosTurb en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaTwin::LeeResultadosInstantTurb(const char *FileWAM, fpos_t &filepos) {
	int nvars = 0, var = 0;

	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &nvars);
		for(int i = 0; i < nvars; i++) {
			fscanf(fich, "%d ", &var);
			switch(var) {
			case 0:
				FResInstantTurbina.Potencia = true;
				break;
			case 1:
				FResInstantTurbina.Rendimiento = true;
				break;
			case 2:
				FResInstantTurbina.RelaCinematica = true;
				break;
			case 3:
				FResInstantTurbina.GastoCorregido = true;
				break;
			case 4:
				FResInstantTurbina.RegimenCorregido = true;
				break;
			case 5:
				FResInstantTurbina.RelacionExpansion = true;
				break;
			default:
				std::cout << "Resultados instantaneos en turbina " << FNumeroTurbina << " no implementados " << std::endl;
			}
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TTurbinaTwin::LeeResultadosInstantTurb en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaTwin::CabeceraResultadosInstantTurb(stringstream& insoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");

		std::string Label;

		if(FResInstantTurbina.Potencia) {
			Label = "\t" + PutLabel(519) + std::to_string(FNumeroTurbina) + PutLabel(903);
			insoutput << Label.c_str();
		}
		if(FResInstantTurbina.Rendimiento) {
			Label = "\t" + PutLabel(512) + std::to_string(FNumeroTurbina) + PutLabel(901);
			insoutput << Label.c_str();
		}
		if(FResInstantTurbina.RelaCinematica) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				Label = "\t" + PutLabel(513) + std::to_string(i + 1) + PutLabel(514) + std::to_string(FNumeroTurbina) + PutLabel(901);
				insoutput << Label.c_str();
			}
		}
		if(FResInstantTurbina.GastoCorregido) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				Label = "\t" + PutLabel(516) + std::to_string(i + 1) + PutLabel(514) + std::to_string(FNumeroTurbina) + PutLabel(905);
				insoutput << Label.c_str();
			}
		}
		if(FResInstantTurbina.RegimenCorregido) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				Label = "\t" + PutLabel(517) + std::to_string(i + 1) + PutLabel(514) + std::to_string(FNumeroTurbina) + PutLabel(906);
				insoutput << Label.c_str();
			}
		}
		if(FResInstantTurbina.RelacionExpansion) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				Label = "\t" + PutLabel(518) + std::to_string(i + 1) + PutLabel(514) + std::to_string(FNumeroTurbina) + PutLabel(901);
				insoutput << Label.c_str();
			}
		}
		// fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TTurbinaTwin::CabeceraResultadosInstantTurb en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaTwin::ResultadosInstantTurb() {
	try {
		if(FResInstantTurbina.Potencia)
			FResInstantTurbina.PotenciaINS = FPotencia;
		if(FResInstantTurbina.Rendimiento)
			if(FTrabajoIsenInstTotal == 0)
				FResInstantTurbina.RendimientoINS = 0;
			else
				FResInstantTurbina.RendimientoINS = FTrabajoFluido / FTrabajoIsenInstTotal;
		if(FResInstantTurbina.RelaCinematica) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				FResInstantTurbina.RelaCinematicaINS[i] = FRelacionCinematica[i];
			}
		}
		if(FResInstantTurbina.GastoCorregido) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				FResInstantTurbina.GastoCorregidoINS[i] = FGastoCorregido[i];
			}
		}
		if(FResInstantTurbina.RegimenCorregido) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				FResInstantTurbina.RegimenCorregidoINS[i] = FRegimenCorregido[i];
			}
		}
		if(FResInstantTurbina.RelacionExpansion) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				FResInstantTurbina.RelacionExpansionINS[i] = FRelacionExpansion[i];
			}
		}

	} catch(exception &N) {
		std::cout << "ERROR: TTurbinaTwin::ResultadosInstantTurb en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaTwin::ImprimeResultadosInstantTurb(stringstream& insoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");

		if(FResInstantTurbina.Potencia)
			insoutput << "\t" << FResInstantTurbina.PotenciaINS;
		if(FResInstantTurbina.Rendimiento)
			insoutput << "\t" << FResInstantTurbina.RendimientoINS;
		if(FResInstantTurbina.RelaCinematica) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				insoutput << "\t" << FResInstantTurbina.RelaCinematicaINS[i];
			}
		}
		if(FResInstantTurbina.GastoCorregido) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				insoutput << "\t" << FResInstantTurbina.GastoCorregidoINS[i];
			}
		}
		if(FResInstantTurbina.RegimenCorregido) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				insoutput << "\t" << FResInstantTurbina.RegimenCorregidoINS[i];
			}
		}
		if(FResInstantTurbina.RelacionExpansion) {
			for(int i = 0; i < FNumeroEntradas; i++) {
				insoutput << "\t" << FResInstantTurbina.RelacionExpansionINS[i];
			}
		}
		// fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TTurbinaTwin::CabeceraResultadosInstantTurb en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaTwin::ImprimeResultadosMediosPantalla() {
	try {

		printf("TRABAJO TURBINA   %d     = %lf Julios \n", FNumeroTurbina, FTrabajoReal);
		if(FTrabajoTotal != 0.) {
			printf("RENDIMIENTO TURBINA   %d = %lf \n", FNumeroTurbina, FTrabajoReal / FTrabajoTotal);
		} else {
			printf("RENDIMIENTO TURBINA   %d = %lf \n", FNumeroTurbina, 0.);
		}
		if(FTrabajoReal != 0.) {
			printf("REL.CINEM.ALABE TURB(Global) %d = %lf \n", FNumeroTurbina, FRelacionCinGlobalAcum / FTrabajoReal);
		} else {
			printf("REL.CINEM.ALABE TURB(Global) %d = %lf \n", FNumeroTurbina, 0.);
		}

	} catch(exception &N) {
		std::cout << "ERROR: TTurbinaTwin::ImprimeResultadosMediosPantalla en la turbina: " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TTurbinaTwin::UpdateProperties0DModel(double TimeCalculo) {

	ActualizaPropiedades(TimeCalculo);

	CalculaCondicionTurbina(TimeCalculo);

	AcumulaMedias(TimeCalculo);

	AcumulaResultadosMedios(TimeCalculo);

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#pragma package(smart_init)
