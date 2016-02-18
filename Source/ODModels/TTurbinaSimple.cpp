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

#include "TTurbinaSimple.h"

#include "TCondicionContorno.h"
#include "TCCDeposito.h"
#include "TRotorTurbina.h"
#include "TEstatorTurbina.h"
#include "TTubo.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TTurbinaSimple::TTurbinaSimple(int i, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
							   nmCalculoGamma GammaCalculation, bool ThereIsEGR) :
	TTurbina(i, nmTurbinaSimple, 1, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {

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

TTurbinaSimple::~TTurbinaSimple() {

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaSimple::AsignaEntradaSalidaCC() {
	try {

		FCCEntrada = new TCondicionContorno*[FNumeroEntradas];
		FCCSalida = new TCondicionContorno*[1];
		for(int i = 0; i < FNumeroUniones; i++) {
			if(dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getValvula()->getTypeOfValve() == nmStator) {
				FCCEntrada[0] = FCCDeposito[i];
			} else if(dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getValvula()->getTypeOfValve() == nmRotor) {
				FCCSalida[0] = FCCDeposito[i];
			}
		}

		dynamic_cast<TEstatorTurbina*>(dynamic_cast<TCCDeposito*>(FCCEntrada[0])->getValvula())->AsignaTurbina(FNumeroTurbina,
				0);
		dynamic_cast<TRotorTurbina*>(dynamic_cast<TCCDeposito*>(FCCSalida[0])->getValvula())->AsignaTurbina(FNumeroTurbina);

		if(FTipoTurbina == nmFixedTurbine) {
			dynamic_cast<TEstatorTurbina*>(dynamic_cast<TCCDeposito*>(FCCEntrada[0])->getValvula())->TipodeEstator(nmStFijo);
			dynamic_cast<TRotorTurbina*>(dynamic_cast<TCCDeposito*>(FCCSalida[0])->getValvula())->TipodeRotor(nmRotFijo);
		} else if(FTipoTurbina == nmTurbineMap) {
			dynamic_cast<TEstatorTurbina*>(dynamic_cast<TCCDeposito*>(FCCEntrada[0])->getValvula())->TipodeEstator(nmStMapa);
			dynamic_cast<TRotorTurbina*>(dynamic_cast<TCCDeposito*>(FCCSalida[0])->getValvula())->TipodeRotor(nmRotMapa);
		} else {
			dynamic_cast<TEstatorTurbina*>(dynamic_cast<TCCDeposito*>(FCCEntrada[0])->getValvula())->TipodeEstator(nmStVariable);
			dynamic_cast<TRotorTurbina*>(dynamic_cast<TCCDeposito*>(FCCSalida[0])->getValvula())->TipodeRotor(nmRotVariable);
		}

	} catch(exception & N) {
		std::cout << "ERROR: TTurbinaSimple::AsignaEntradaSalidaCC en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaSimple::CalculaCondicionTurbina(double TimeCalculo) {

	double cpte, cpts /* ,gammaent,gammasal,cvte,cvts,Rentrada,Rsalida */;
	// double B,Cte_R,A;
	double TempIsentSal = 0.;
	double DeltaT = 0.;
	double incrRelCin = 0.;
	double Landa = 0., Beta = 0., Entropia = 0.;
	int SentidoSalida = -1, SentidoEntrada = 1;
	double dd = 0., b = 0., c = 0., d = 0.;
	int NumberOfPipes = 0;
	double gam_med = 0.;
	double gam_te = 0.;
	double cp_med = 0.;

	try {

		DeltaT = TimeCalculo - FTimeTurbina;
		FTimeTurbina = TimeCalculo;

		if(DeltaT > 0) {

			FTrabajoIsenInstTotal = 0.;

			// Calculo de las propiedades del gas a la salida
#ifndef tchtm
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

			Landa = FCCEntrada[0]->GetTuboExtremo(0).Landa;
			Beta = FCCEntrada[0]->GetTuboExtremo(0).Beta;
			Entropia = FCCEntrada[0]->GetTuboExtremo(0).Entropia;

			TransformaContorno(&Landa, &Beta, &Entropia, &FAsonidoEntrada[0], &FVelocidadEntrada[0], &FPresionEntrada[0], 1,
							   FCCEntrada[0]->getGamma());

			if(dynamic_cast<TCCDeposito*>(FCCEntrada[0])->getSentidoFlujo() == nmEntrante) {
				SentidoEntrada = 1;
			} else if(dynamic_cast<TCCDeposito*>(FCCEntrada[0])->getSentidoFlujo() == nmSaliente) {
				SentidoEntrada = -1;
			} else
				SentidoEntrada = 0; /* Flujo parado */
			FTempEntrada[0] = pow2(FAsonidoEntrada[0] * __cons::ARef) / (FCCEntrada[0]->getGamma() * FCCEntrada[0]->getR());
			// Esta en Kelvin

			FVelocidadEntrada[0] = FVelocidadEntrada[0] * __cons::ARef * SentidoEntrada;
			FRhoEntrada[0] = __units::BarToPa(FPresionEntrada[0]) / FTempEntrada[0] / FCCEntrada[0]->getR();
			FGastoEntrada[0] = FRhoEntrada[0] * FVelocidadEntrada[0] * dynamic_cast<TCCDeposito*>(FCCEntrada[0])->getSeccionTubo();

#endif

#ifdef tchtm
			FTempEntrada[0] = FAcTurb->T3();
			FPresionEntrada[0] = FAcTurb->P3();

			FGastoEntrada[0] = FAcTurb->MassIn();

			FTempSalida = FAcTurb->T4();
			FPresionSalida = FAcTurb->P4();

#endif

			// ------------------------------------------------------------------------------
			// Calculo de la temperatura isentropica del gas a la salida de la turbina,
			// a partir del transporte de especies.
			// ------------------------------------------------------------------------------

			if(FCalculoEspecies == nmCalculoCompleto) {
				cpte = CpTurbineComplete(FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2],
										 FTempEntrada[0]);
			} else {
				cpte = CpTurbineSimple(FTempEntrada[0], FFraccionMasicaEspecie[0]);
			}
			gam_te = cpte / (cpte - FRMezcla);

#ifndef tchtm

			FTemp0Entrada[0] = FTempEntrada[0] + pow2(FVelocidadEntrada[0]) / (2. * cpte);
			FPresion0Entrada[0] = FPresionEntrada[0] * pow(FTemp0Entrada[0] / FTempEntrada[0], (gam_te / (gam_te - 1.0)));

#endif

#ifdef tchtm

			if(fabs(FGastoEntrada[0]) > 1e-10) {

				if(FTempEntrada[0] < FHTM->NodeTemperature(10)) {

					FTempEntrada[0] = FTempEntrada[0] - FHTM->Turb_Heat_Flow()
									  / cpte / FGastoEntrada[0];

				} else {

					FTempEntrada[0] = FTempEntrada[0] - FHTM->Turb_Heat_Flow()
									  / cpte / FGastoEntrada[0];
					// comprobacion para pasos iniciales.
					if(FTempEntrada[0] < FHTM->NodeTemperature(10))
						FTempEntrada[0] = FHTM->NodeTemperature(10);
				}
			}

			FVelocidadEntrada[0] = FGastoEntrada[0] * FRMezcla * FTempEntrada
								   [0] / (__units::BarToPa(FPresionEntrada[0]) * FAcTurb->SIn());

			FTemp0Entrada[0] = FTempEntrada[0] + pow2(FVelocidadEntrada[0]) /
							   (2. * cpte);
			FPresion0Entrada[0] = FPresionEntrada[0] * pow
								  (FTemp0Entrada[0] / FTempEntrada[0],
								   (gam_te / (gam_te - 1.0)));

#endif

			FGastoCorregido[0] = FGastoEntrada[0] * sqrt(FTemp0Entrada[0]) / FPresion0Entrada[0] / 0.1;
			FRegimenCorregido[0] = FRegimen / sqrt(FTemp0Entrada[0]);

			if(FCalculoEspecies == nmCalculoCompleto) {
				cp_med = CpTurbineComplete(FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2],
										   (FTempEntrada[0] + FTempSalida) / 2.);
			} else {
				cp_med = CpTurbineSimple((FTempEntrada[0] + FTempSalida) / 2., FFraccionMasicaEspecie[0]);
			}
			gam_med = cpte / (cpte - FRMezcla);

			TempIsentSal = FTemp0Entrada[0] * pow(FPresionSalida / FPresion0Entrada[0], ((gam_med - 1.0) / gam_med));

			if(FCalculoEspecies == nmCalculoCompleto) {
				cp_med = CpTurbineComplete(FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2],
										   (FTempEntrada[0] + TempIsentSal) / 2.);
			} else {
				cp_med = CpTurbineSimple((FTempEntrada[0] + TempIsentSal) / 2., FFraccionMasicaEspecie[0]);
			}

			FEntalpia0Entrada[0] = cp_med * FTemp0Entrada[0];
			double EntalEntr = cp_med * FTempEntrada[0];
			FEntalpiaIsenSalida[0] = cp_med * TempIsentSal;

			FRelacionExpansion[0] = FPresion0Entrada[0] / FPresionSalida;

			// Calculo de la posicion de la turbina

			if(FTipoTurbina == nmTurbineMap) {
				if(FRackIsControlled) {
					FRack = FRackController->Output(FTime);
				}

				FMapa->CurrentEffectiveSection(FRegimenCorregido[0] / 60., FRelacionExpansion[0], FRack,
											   __units::degCToK(FTemperature) / FTemp0Entrada[0]);

				dynamic_cast<TEstatorTurbina*>(dynamic_cast<TCCDeposito*>(FCCEntrada[0])->getValvula())->PutAreaEff(FMapa->StatorEF());
				dynamic_cast<TRotorTurbina*>(dynamic_cast<TCCDeposito*>(FCCSalida[0])->getValvula())->PutAreaEff(FMapa->RotorEF());

			}

			// Calculo del rendimiento de la turbina

			if((FGastoEntrada[0] > 0.) && (FEntalpia0Entrada[0] - FEntalpiaIsenSalida[0]) >= 0.) {

				if((FEntalpia0Entrada[0] - FEntalpiaIsenSalida[0]) > 0.) {
					FRelacionCinematica[0] = __units::RPMToRPS(FRegimen) * __cons::Pi * FDiametroRodete / sqrt(2 *
											 (FEntalpia0Entrada[0] - FEntalpiaIsenSalida[0]));
				} else {
					FRelacionCinematica[0] = 0.;
				}
				if(FCalRendTurbina == nmWatson) {
					if(FRelacionCinematica[0] <= 0 || FRelacionCinematica[0] >= 1.19) {
						FRendTurbina[0] = 0;
					} else {
						FRendTurbina[0] = 0.004022 + FRelacionCinematica[0] * (1.55766 + FRelacionCinematica[0] *
										  (-0.511626 + FRelacionCinematica[0] * (-0.121795 - 0.445804 * FRelacionCinematica[0])));
					}
				} else if(FCalRendTurbina == nmPolinomio) {
					if(FRelacionCinematica[0] >= FRcmaxima || FRelacionCinematica[0] <= 0) {
						FRendTurbina[0] = 0.;
					} else {
						dd = 2. * pow3(FRcoptima) * pow2(FRcmaxima) - pow2(FRcoptima) * pow3(FRcmaxima) - pow4(FRcoptima) * FRcmaxima;
						b = FRendmaximo * (3. * pow2(FRcmaxima) * pow2(FRcoptima) - 2 * pow3(FRcmaxima) * FRcoptima) / dd;
						c = FRendmaximo * (pow3(FRcmaxima) - 3. * FRcmaxima * pow2(FRcoptima)) / dd;
						d = FRendmaximo * (2. * FRcmaxima * FRcoptima - pow2(FRcmaxima)) / dd;

						FRendTurbina[0] = b * FRelacionCinematica[0] + c * pow2(FRelacionCinematica[0]) + d * pow3(FRelacionCinematica[0]);
					}
				} else if(FCalRendTurbina == nmCalcExtRD) {
					// Todavia no esta hecha la asignacion de FNumeroTurbinaTGV
					if(FDatosTGV[FNumeroTurbinaTGV].Rendimiento[0] < 0) {
						FRendTurbina[0] = 0.;
					} else {
						FRendTurbina[0] = FDatosTGV[FNumeroTurbinaTGV].Rendimiento[0];
					}
				} else if(FCalRendTurbina == nmRendMapa) {
					// FRendTurbina=FMapa->
					// FCCEntrada[0]->
					FRendTurbina[0] = FMapa->EffTurb();

				} else {
					std::cout << "ERROR: Calculo del rendimiento de la turbina desconocido " << std::endl;
					throw Exception("");
				}
				FRendTurbina[0] = FRendTurbina[0] * FAjustRendTurb;
			} else {
				FRelacionCinematica[0] = 0.;
				FRendTurbina[0] = 0.;
			}

			incrRelCin = 0;

			FTrabajoIsen = FGastoEntrada[0] * (FEntalpia0Entrada[0] - FEntalpiaIsenSalida[0]) * DeltaT;
			double TurbWork = FTrabajoIsen * FRendTurbina[0];
			FTrabajoFluido = TurbWork - FGastoEntrada[0] * (FEntalpia0Entrada[0] - EntalEntr) * DeltaT;
			FTrabajoReal += TurbWork; // Lo usa para calcular el rendimiento medio
			FTrabajoRealPaso += TurbWork; // Para el calculo de la potencia del paso.
			FRendInstantaneo += FRendTurbina[0] * FGastoEntrada[0];
			incrRelCin += TurbWork * FRelacionCinematica[0];

			FDeltaPaso += DeltaT;

			FPotencia = TurbWork / DeltaT;
			FTrabajoTotal += FTrabajoIsen; // Lo usa para calcular el rendimiento medio
			FRelacionCinAcum[0] += incrRelCin;
			FPonderacionRelacionCinematica[0] += TurbWork;
		}

	} catch(exception & N) {
		std::cout << "ERROR: TTurbinaSimple::CalculaCondicionTurbina en la turbina: " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaSimple::ImprimeResultadosMediosPantalla() {
	try {

		printf("TURBINE WORK   %d        = %6.3lf Julios \n", FNumeroTurbina, FTrabajoReal);
		if(FTrabajoTotal != 0.) {
			printf("TURBINE EFFICIENCY   %d  = %6.3lf \n", FNumeroTurbina, FTrabajoReal / FTrabajoTotal);
		} else {
			printf("TURBINE EFFICIENCY   %d = %6.3lf \n", FNumeroTurbina, 0.);
		}
		if(FTrabajoReal != 0.) {
			printf("BLADE SPEED RATIO TURB %d= %6.3lf \n", FNumeroTurbina, FRelacionCinAcum[0] / FTrabajoReal);
		} else {
			printf("BLADE SPEED RATIO TURB %d= %6.3lf \n", FNumeroTurbina, 0.);
		}

	} catch(exception & N) {
		std::cout << "ERROR: TTurbinaSimple::ImprimeResultadosMediosPantalla en la turbina: " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaSimple::ReadAverageResultsTurb(const char *FileWAM, fpos_t &filepos) {
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
	} catch(exception & N) {
		std::cout << "ERROR: TTurbinaSimple::ReadAverageResultsTurb en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaSimple::CabeceraResultadosMedTurb(stringstream& medoutput) {
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
	} catch(exception & N) {
		std::cout << "ERROR: TTurbinaSimple::CabeceraResultadosMedTurb en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaSimple::IniciaMedias() {
	try {

		FResMediosTurbina.TiempoSUM = 0.;
		FResMediosTurbina.TrabajoSUM = 0.;
		FResMediosTurbina.Tiempo0 = 0.;

	} catch(exception & N) {
		std::cout << "ERROR: TTurbinaSimple::IniciaMedias en el turbina: " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: TTurbinaSimple::IniciaMedias en el turbina: " + std::to_string(FNumeroTurbina) + N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaSimple::AcumulaMedias(double Tiempo) {
	double DeltaT = 0.;
	try {
		DeltaT = Tiempo - FResMediosTurbina.Tiempo0;
		FResMediosTurbina.TiempoSUM += DeltaT;
		// FResMediosTurbina.TrabajoSUM+=FTrabajoFluido;
		FResMediosTurbina.TrabajoSUM += FPotencia * DeltaT;
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
	} catch(exception & N) {
		std::cout << "ERROR: TTurbinaSimple::AcumulaMedias en la turbina: " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: TTurbinaSimple::AcumulaMedias en la Turbine: " + std::to_string(FNumeroTurbina) + N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaSimple::ImprimeResultadosMedTurb(stringstream& medoutput) {
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
	} catch(exception & N) {
		std::cout << "ERROR: TTurbinaSimple::ImprimerResultadosMedTurb en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaSimple::CalculaResultadosMediosTurb() {
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

		FTrabajoReal = 0.;
		FTrabajoTotal = 0.;
		FResMediosTurbina.TrabajoSUM = 0.;
		FResMediosTurbina.TiempoSUM = 0.;

	} catch(exception & N) {
		std::cout << "ERROR: TTurbinaSimple::CalculaResultadosMediosTurb en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaSimple::LeeResultadosInstantTurb(const char *FileWAM, fpos_t &filepos) {
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
				std::cout << "Instantaneous results in turbine " << FNumeroTurbina << " are not implemented " << std::endl;
			}
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TTurbinaSimple::LeeResultadosInstantTurb en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaSimple::CabeceraResultadosInstantTurb(stringstream& insoutput) {
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
	} catch(exception & N) {
		std::cout << "ERROR: TTurbinaSimple::CabeceraResultadosInstantTurb en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaSimple::ResultadosInstantTurb() {
	try {
		if(FResInstantTurbina.Potencia)
			FResInstantTurbina.PotenciaINS = FPotencia;
		if(FResInstantTurbina.Rendimiento)
			FResInstantTurbina.RendimientoINS = FRendTurbina[0];
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

	} catch(exception & N) {
		std::cout << "ERROR: TTurbinaSimple::ResultadosInstantTurb en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbinaSimple::ImprimeResultadosInstantTurb(stringstream& insoutput) {
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
	} catch(exception & N) {
		std::cout << "ERROR: TTurbinaSimple::CabeceraResultadosInstantTurb en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TTurbinaSimple::UpdateProperties0DModel(double TimeCalculo) {

	CalculaCondicionTurbina(TimeCalculo);

	ActualizaPropiedades(TimeCalculo);

	AcumulaMedias(TimeCalculo);

	AcumulaResultadosMedios(TimeCalculo);

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
#pragma package(smart_init)
