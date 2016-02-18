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

#include "TTurbina.h"

#include "TCCDeposito.h"
#include "TTubo.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TTurbina::TTurbina(int i, nmTipoDeposito TipoDeposito, int nentradas, nmTipoCalculoEspecies SpeciesModel,
				   int numeroespecies, nmCalculoGamma GammaCalculation, bool ThereIsEGR) :
	TDepVolCteBase(i, TipoDeposito, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {
	asgNumeroTurbina = false;

	FTimeTurbina = 0.;
	FNumeroEntradas = nentradas;
	FNodoEntrada = new int[nentradas];
	FTuboEntrada = new int[nentradas];
	FExtremoEntrada = new int[nentradas];
	FSentidoEntrada = new int[nentradas];
	FPresionEntrada = new double[nentradas];
	FVelocidadEntrada = new double[nentradas];
	FGastoEntrada = new double[nentradas];
	FTempEntrada = new double[nentradas];
	FAsonidoEntrada = new double[nentradas];
	FRhoEntrada = new double[nentradas];
	FTemp0Entrada = new double[nentradas];
	FEntalpia0Entrada = new double[nentradas];
	FPresion0Entrada = new double[nentradas];
	FEntalpiaIsenSalida = new double[nentradas];
	FRelacionCinematica = new double[nentradas];
	FRendTurbina = new double[nentradas];
	FDeltaPaso = 0.;
	FTrabajoReal = 0.;
	FTrabajoRealPaso = 0.;
	FRendInstantaneo = 0.;
	FTrabajoIsenInstTotal = 0.;
	FTrabajoFluido = 0.;
	FRelacionCinGlobalAcum = 0.;
	FRelacionCinAcum = new double[nentradas];
	for(int j = 0; j < nentradas; j++) {
		FRelacionCinAcum[j] = 0.;
	}
	FPonderacionRelacionCinematica = new double[nentradas];

	FCCSalida = NULL;
	FCCSalida = NULL;
	FResMediosTurbina.RelaCinematica = false;
	FResMediosTurbina.RelaCinematicaMED = NULL;
	FResInstantTurbina.RelaCinematicaINS = NULL;
	FResInstantTurbina.RelaCinematica = false;

	FGastoCorregido = NULL;
	FRegimenCorregido = NULL;
	FRelacionExpansion = NULL;
	FGastoCorregido = new double[nentradas];
	FRegimenCorregido = new double[nentradas];
	FRelacionExpansion = new double[nentradas];

	FMapa = NULL;

	// FHTM=NULL;

	FRackIsControlled = false;
	FIsAcoustic = false;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TTurbina::~TTurbina() {
	delete[] FRelacionCinAcum;
	delete[] FTuboEntrada;
	delete[] FNodoEntrada;
	delete[] FExtremoEntrada;
	delete[] FSentidoEntrada;
	delete[] FPresionEntrada;
	delete[] FVelocidadEntrada;
	delete[] FGastoEntrada;
	delete[] FTempEntrada;
	delete[] FAsonidoEntrada;
	delete[] FRhoEntrada;
	delete[] FTemp0Entrada;
	delete[] FEntalpia0Entrada;
	delete[] FPresion0Entrada;
	delete[] FEntalpiaIsenSalida;
	delete[] FRelacionCinematica;
	delete[] FRendTurbina;
	delete[] FGastoCorregido;
	delete[] FRegimenCorregido;
	delete[] FRelacionExpansion;
	delete[] FPonderacionRelacionCinematica;

	if(FCCEntrada != NULL)
		delete[] FCCEntrada;
	if(FCCSalida != NULL)
		delete[] FCCSalida;
	if(FResMediosTurbina.RelaCinematicaMED != NULL)
		delete[] FResMediosTurbina.RelaCinematicaMED;
	if(FResMediosTurbina.GastoCorregidoMED != NULL)
		delete[] FResMediosTurbina.GastoCorregidoMED;
	if(FResMediosTurbina.GastoCorregidoSUM != NULL)
		delete[] FResMediosTurbina.GastoCorregidoSUM;
	if(FResMediosTurbina.RegimenCorregidoMED != NULL)
		delete[] FResMediosTurbina.RegimenCorregidoMED;
	if(FResMediosTurbina.RegimenCorregidoSUM != NULL)
		delete[] FResMediosTurbina.RegimenCorregidoSUM;
	if(FResMediosTurbina.RelacionExpansionMED != NULL)
		delete[] FResMediosTurbina.RelacionExpansionMED;
	if(FResMediosTurbina.RelacionExpansionSUM != NULL)
		delete[] FResMediosTurbina.RelacionExpansionSUM;
	if(FResInstantTurbina.RelaCinematicaINS != NULL)
		delete[] FResInstantTurbina.RelaCinematicaINS;
	if(FResInstantTurbina.GastoCorregidoINS != NULL)
		delete[] FResInstantTurbina.GastoCorregidoINS;
	if(FResInstantTurbina.RegimenCorregidoINS != NULL)
		delete[] FResInstantTurbina.RegimenCorregidoINS;
	if(FResInstantTurbina.RelacionExpansionINS != NULL)
		delete[] FResInstantTurbina.RelacionExpansionINS;

	if(FMapa != NULL) {
		delete FMapa;
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbina::ActualizaPropiedades(double TimeCalculo) {

	double H = 0.; // Entalpia de entrada
	double Energia = 0.;
	double MasaEntrante, FraccionMasicaAcum = 0.;
	double DeltaT = 0.;
	double g = 0., v = 0., a = 0., m = 0.;
	int SignoFlujo = 1;

	try {
		FMasa0 = FMasa;
		MasaEntrante = 0.;
		H = 0.;
		DeltaT = TimeCalculo - FTime;

		if(!(DoubEqZero(DeltaT))) {
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

			bool FirstStep = true;
			bool Converge = false;
			double Error = 0.;
			double Ason1 = FAsonido;
			double Ason0 = FAsonido;
			double MTemp0 = 1 / (FMasa0 * FRMezcla * __units::degCToK(FTemperature));
			double MTemp1 = 1 / (FMasa0 * FRMezcla * __units::degCToK(FTemperature));
			double MTemp = 0.;
			double H0 = 0;
			double Diff = 0.;
#ifdef tchtm
			FHeatPower = FHTM->Turb_Heat_Flow();
#endif
			double Heat = FHeatPower * DeltaT;

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
						H += EntalpiaEntrada(a, v, m, Ason1, FMasa, FCCDeposito[i]->getGamma());
					}

				}
				MTemp1 = FGamma / (pow2(Ason1 * __cons::ARef) * FMasa);

				if(FirstStep) {
					FMasa = FMasa0 + MasaEntrante;
					for(int j = 0; j < FNumeroEspecies - 2; j++) {
						FFraccionMasicaEspecie[j] = FMasaEspecie[j] / FMasa;
						FraccionMasicaAcum += FFraccionMasicaEspecie[j];
					}
					FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
					if(FHayEGR)
						FFraccionMasicaEspecie[FNumeroEspecies - 1] = FMasaEspecie[FNumeroEspecies - 1] / FMasa;
					H0 = H;
					FirstStep = false;
				}

				MTemp = (MTemp0 + MTemp1) / 2.;

				Energia = pow(FMasa / FMasa0 * exp((H + H0) / 2 - (FTrabajoFluido + Heat) * MTemp), __Gamma::G1(FGamma));
				Ason1 = FAsonido * sqrt(Energia);
				Error = (Diff = Ason1 - Ason0, fabs(Diff)) / Ason1;
				if(Error > 1e-6) {
					Ason0 = Ason1;
				} else {
					Converge = true;
				}

			}

			FAsonido = Ason1;
			// FTemperature = pow(FAsonido, 2) / (FGamma * FRMezcla) * pow(__cons::ARef, 2);
			FPressure = __units::PaToBar(pow2(__cons::ARef * FAsonido) / FGamma / FVolumen * FMasa);
			FPresionIsen = pow(FPressure / FPresRef, __Gamma::G5(FGamma));
			FTemperature = __units::KTodegC(pow2(FAsonido * __cons::ARef) / FGamma / FRMezcla);
		}
		FTime = TimeCalculo;
	} catch(exception & N) {
		std::cout << "ERROR: TTurbina::ActualizaPropiedades en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbina::CalculoPotenciaPaso() {
	try {

		if(FDeltaPaso != 0 & FTrabajoRealPaso != 0) {
			FPotenciaPaso = FTrabajoRealPaso / FDeltaPaso;
			FTrabajoRealPaso = 0.;
			FDeltaPaso = 0.;
		} else {
			FPotenciaPaso = 0.;
			FTrabajoRealPaso = 0.;
			FDeltaPaso = 0.;
		}

	} catch(exception & N) {
		std::cout << "ERROR: TTurbina::CalculoPotenciaPaso en el compresor: " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: TTurbina::CalculoPotenciaPaso en la turbina: " + std::to_string(FNumeroTurbina) + N.what());
	}
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbina::TransformaContorno(double *L, double *B, double *E, double *a, double *v, double *p, int modo,
								  double Gamma) {
	try {
		if(modo == 0) {
			*L = (*a + (Gamma / 2) * *v);
			*B = (*a - (Gamma / 2) * *v);
			*E = *a / pow(*p, __Gamma::G5(Gamma));
		} else {
			*a = (*L + *B) / 2.;
			*v = (*L - *B) / __Gamma::G1(Gamma);
			*p = pow(*a / *E, __Gamma::G4(Gamma));
		}
	} catch(exception & N) {
		std::cout << "ERROR: TTurbina::TransformaContorno en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbina::LeeTurbina(const char *FileWAM, fpos_t &filepos) {
	try {
		int rdturb = 0, tipoturb = 0, ctrl = 0, numctrl = 0, ac = 0;
		double AngCritico = 0., Beta = 0.;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &tipoturb);
		switch(tipoturb) {
		case 0:
			FTipoTurbina = nmFixedTurbine;
			break;
		case 1:
			FTipoTurbina = nmVariableGeometry;
			break;
		case 2:
			FTipoTurbina = nmTurbineMap;
			break;
		default:
			std::cout << "ERROR: Unknown turbine type " << std::endl;
		}
		fscanf(fich, "%lf ", &FDiametroRodete);
		if(FTipoTurbina == nmTurbineMap) {

			fscanf(fich, "%lf %lf %lf ", &FDiametroRodeteOut, &FDiametroTuerca, &FDiametroTurbinaIn);
			fscanf(fich, "%lf ", &AngCritico);
			FMapa = new TTurbineMap();
			FMapa->LoadTurbineMap(fich, FDiametroRodete, FDiametroRodeteOut, FDiametroTuerca, FDiametroTurbinaIn, AngCritico);

			fscanf(fich, "%d ", &numctrl);
			for(int i = 0; i < numctrl; ++i) {
				fscanf(fich, "%d ", &ctrl);
				switch(ctrl) {
				case 0:
					FRackIsControlled = true;
					break;

				default:
					std::cout << "ERROR: Unknown controller for the turbine " << std::endl;
				}
				fscanf(fich, "%d ", &FNumControlObject);
			}

			if(!FRackIsControlled)
				fscanf(fich, "%lf ", &FRack);
			FCalRendTurbina = nmRendMapa;

#ifdef tchtm

			fscanf(fich, "%d ", &ac);
			if(ac == 1) {
				iVector InID;
				iVector VolID;
				int OutID = 0;
				FIsAcoustic = true;
				if(FTipoDeposito == nmTurbinaSimple) {
					InID.resize(1);
					InID.resize(1);
					fscanf(fich, "%d %d %d", &InID[0], &VolID[0], &OutID);
					FAcTurb = new TAcousticTurbine(InID, VolID, OutID);
				}
			}
#endif
		} else {
			fscanf(fich, "%d ", &rdturb);
			switch(rdturb) {
			case 0:
				FCalRendTurbina = nmWatson;
				break;
			case 1:
				FCalRendTurbina = nmPolinomio;
				fscanf(fich, "%lf %lf %lf ", &FRcoptima, &FRcmaxima, &FRendmaximo);
				break;
			case 2:
				FCalRendTurbina = nmCalcExtRD;
				break;
			default:
				std::cout << "ERROR: Unknown method to calculate turbine efficiency " << std::endl;
			}
		}

		fscanf(fich, "%lf ", &FAjustRendTurb);

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TTurbina::LeeRendimientoTurbina en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

////void TTurbina::PutNumeroTurbina(int NumeroTurbina) {
// try {
// if (!asgNumeroTurbina) {
// FNumeroTurbina = NumeroTurbina;
// asgNumeroTurbina = true;
// }
// else {
// std::cout << "ERROR: Esta turbina ya tiene numero asignada" << std::endl;
// throw Exception("");
// }
// }
// catch(Exception & N) {
// std::cout << "ERROR: TTurbina::PutNumeroTurbina en la turbina " << FNumeroTurbina << std::endl;
// std::cout << "Tipo de error: " << N.what() << std::endl;
// throw Exception(N.what());
// }
// }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// void TTurbina::PutRegimen(double valor) {
// try {
//
// FRegimen = valor;
//
// }
// catch(Exception & N) {
// std::cout << "ERROR: TTurbina::PutRegimen en la turbina " << FNumeroTurbina << std::endl;
// std::cout << "Tipo de error: " << N.what() << std::endl;
// throw Exception(N.what());
// }
// }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbina::AsignaDatosSalida(int nodsaltur, int tubsaltur, int extremo, int sentido) {
	try {
		FNodoSalida = nodsaltur;
		FTuboSalida = tubsaltur;
		FExtremoSalida = extremo;
		FSentidoSalida = sentido;
	} catch(exception & N) {
		std::cout << "ERROR: TTurbina::AsignaDatosSalida en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TTurbina::GetRelacionCinematica(int i) {
	try {
		if(FRelacionCinematica != NULL && i < FNumeroEntradas) {
			return FRelacionCinematica[i];
		} else {
			std::cout << "ERROR: TTurbina::GetRelacionCinematica: " << FNumeroTurbina << std::endl;
			std::cout << "ERROR: La entrada de la turbina indicada no existe " << std::endl;
			return 0.;
		}
	} catch(exception & N) {
		std::cout << "ERROR: TTurbina::GetRelacionCinematica en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbina::AsignaDatosEntrada(int nodentur, int tubsaltur, int extremo, int sentido, int n) {
	try {
		FNodoEntrada[n] = nodentur;
		FTuboEntrada[n] = tubsaltur;
		FExtremoEntrada[n] = extremo;
		FSentidoEntrada[n] = sentido;
	} catch(exception & N) {
		std::cout << "ERROR: TTurbina::AsignaDatosEntrada en la turbina " << FNumeroTurbina << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCondicionContorno* TTurbina::GetCCEntrada(int i) {
	if(FCCEntrada != NULL && i < FNumeroEntradas) {
		return FCCEntrada[i];
	} else {
		std::cout << "ERROR: TTurbina::Peticion CCEntrada: " << FNumeroTurbina << std::endl;
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCondicionContorno* TTurbina::GetCCSalida(int i) {
	if(FCCSalida != NULL && i < 1) {
		return FCCSalida[i];
	} else {
		std::cout << "ERROR: TTurbina::Peticion CCSalida: " << FNumeroTurbina << std::endl;
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTurbina::AsignaRackController(TController **Controller) {
	if(FRackIsControlled)
		FRackController = Controller[FNumControlObject - 1];
}

void TTurbina::AllocateDatosTGV(stDatosTGV *DatosTGV) {
	FDatosTGV = DatosTGV;
}

double TTurbina::CpTurbineSimple(double Temperature, double YBurnt) {

	double R = __R::Burnt * YBurnt + (__R::Air * (1 - YBurnt - 0.0164) + 0.0164 * __R::H2O);
	double RaizdeT = sqrt(Temperature);

	double CvAir = -10.4199 * RaizdeT + 2522.88 + (-67227.1 * RaizdeT + 917124.4 - 4174853.6 / RaizdeT) / Temperature;
	double CvBurnt = 641.154 + Temperature * (0.43045 + Temperature * (-0.0001125 + Temperature * 8.979e-9));
	double CvH2O = (22.605 - 0.09067 * RaizdeT + (-826.53 * RaizdeT + 13970.1 - 82114 / RaizdeT) / Temperature) * __R::H2O -
				   __R::H2O;

	double CpMezcla = CvBurnt * YBurnt + (CvAir * (1 - YBurnt - 0.0164) + 0.0164 * CvH2O) + R;

	return CpMezcla;
}

double TTurbina::CpTurbineComplete(double YO2, double YCO2, double YH2O, double Temperature) {
	double YN2 = 1 - YO2 - YCO2 - YH2O;

	double RaizdeT = sqrt(Temperature);
	// Temperature en Kelvin. Calculado segun la correlacion de JANAF.
	double CpN2 = (12.531 - 0.05932 * RaizdeT + (-352.3 * RaizdeT + 5279.1 - 27358 / RaizdeT) / Temperature) * __R::N2;
	double CpO2 = (-0.112 + 0.0479 * RaizdeT + (195.42 * RaizdeT - 4426.1 + 32538 / RaizdeT) / Temperature) * __R::O2;
	double CpCO2 = (12.019 - 0.03566 * RaizdeT + (-142.34 * RaizdeT - 163.7 + 9470 / RaizdeT) / Temperature) * __R::CO2;
	double CpH2O = (22.605 - 0.09067 * RaizdeT + (-826.53 * RaizdeT + 13970.1 - 82114 / RaizdeT) / Temperature) * __R::H2O;

	double CpMezcla = CpO2 * YO2 + CpCO2 * YCO2 + CpH2O * YH2O + CpN2 * (YN2 - 0.01292) + 520.32 * 0.01292;

	return CpMezcla;
}

void TTurbina::CalculateAdiabaticMap(double TinC) {
	FMapa->CalculateAdiabaticEfficiency(FHTM, TinC);
}

void TTurbina::AsignAcousticElements(TTubo **Pipe) {

	if(FIsAcoustic) {
		for(int i = 0; i < FNumeroEntradas; i++) {
			FAcTurb->AsignInPipe(Pipe, i);
		}

		FAcTurb->AsignOutPipe(Pipe);
	}
}

#pragma package(smart_init)
