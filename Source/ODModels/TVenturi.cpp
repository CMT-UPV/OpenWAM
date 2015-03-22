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

#include "TVenturi.h"

#include "TCCDeposito.h"
#include "TTubo.h"
#include "TTipoValvula.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TVenturi::TVenturi(int i, nmTipoCalculoEspecies SpeciesModel,
		int numeroespecies, nmCalculoGamma GammaCalculation, bool ThereIsEGR) :
		TDepVolCteBase(i, nmVenturi, SpeciesModel, numeroespecies,
				GammaCalculation, ThereIsEGR) {

	asgNumeroVenturi = false;

	FResInstantVenturi.PresionEntrada = false;
	FResInstantVenturi.PresionGarganta = false;
	FResInstantVenturi.MachEntrada = false;
	FResInstantVenturi.MachGarganta = false;
	FResInstantVenturi.VelEntrada = false;
	FResInstantVenturi.VelLateral = false;
	FResInstantVenturi.GastoEntrada = false;
	FResInstantVenturi.GastoLateral = false;

	FResMediosVenturi.PresionEntrada = false;
	FResMediosVenturi.PresionGarganta = false;
	FResMediosVenturi.MachEntrada = false;
	FResMediosVenturi.MachGarganta = false;
	FResMediosVenturi.VelEntrada = false;
	FResMediosVenturi.VelLateral = false;
	FResMediosVenturi.GastoEntrada = false;
	FResMediosVenturi.GastoLateral = false;

	FResMediosVenturi.Tiempo0 = 0.;
	FResMediosVenturi.TiempoSUM = 0.;
	FResMediosVenturi.PresionEntradaSUM = 0.;
	FResMediosVenturi.PresionGargantaSUM = 0.;
	FResMediosVenturi.MachEntradaSUM = 0.;
	FResMediosVenturi.MachGargantaSUM = 0.;
	FResMediosVenturi.VelEntradaSUM = 0.;
	FResMediosVenturi.VelLateralSUM = 0.;
	FResMediosVenturi.GastoEntradaSUM = 0.;
	FResMediosVenturi.GastoLateralSUM = 0.;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TVenturi::~TVenturi() {
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TVenturi::LeeDatosVenturi(const char *FileWAM, fpos_t &filepos) {
	try {
		int numid; // dato para Wamer
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &numid); /* DATO PARA WAMER */
		fscanf(fich, "%d %d %d ", &FNodoEntrada, &FNodoSalida, &FNodoLateral);
		fscanf(fich, "%lf %lf %lf ", &FRelacionSecciones, &FRendimientoVenturi,
				&FPerdidasCalor);

		fgetpos(fich, &filepos);
		fclose(fich);

	} catch (Exception & N) {
		std::cout << "ERROR: TVenturi::LeeDatosVenturi en el deposito: "
				<< FNumeroDeposito << std::endl;
		// std::cout << "Tipo de error: " << N.Message << std::endl;
		throw Exception(N.Message);
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TVenturi::ActualizaPropiedades(double TimeCalculo) {

	double H; // Entalpia de entrada
	double Energia;
	double MasaEntrante, FraccionMasicaAcum = 0.;
	double DeltaT;
	double g, v, a, m;
	int SignoFlujo = 1;

	try {
		FMasa0 = FMasa;
		MasaEntrante = 0.;
		H = 0.;
		DeltaT = TimeCalculo - FTime;

		if (FCalculoEspecies == nmCalculoCompleto) {

			FRMezcla = CalculoCompletoRMezcla(FFraccionMasicaEspecie[0],
					FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2], 0,
					FCalculoGamma, nmMEP);
			FCpMezcla = CalculoCompletoCpMezcla(FFraccionMasicaEspecie[0],
					FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2], 0,
					FTemperature + 273., FCalculoGamma, nmMEP);
			FGamma = CalculoCompletoGamma(FRMezcla, FCpMezcla, FCalculoGamma);

		} else if (FCalculoEspecies == nmCalculoSimple) {

			FRMezcla = CalculoSimpleRMezcla(FFraccionMasicaEspecie[0],
					FFraccionMasicaEspecie[1], FCalculoGamma, nmMEP);
			FCvMezcla = CalculoSimpleCvMezcla(FTemperature + 273.,
					FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1],
					FCalculoGamma, nmMEP);
			FGamma = CalculoSimpleGamma(FRMezcla, FCvMezcla, FCalculoGamma);

		}

		FGamma1 = Gamma1(FGamma);
		FGamma2 = Gamma2(FGamma);
		FGamma3 = Gamma3(FGamma);
		FGamma5 = Gamma5(FGamma);
		FGamma6 = Gamma6(FGamma);

		bool Converge = false;
		bool FirstStep = true;
		double H0 = 0.;
		double Asonido0 = FAsonido;
		double Asonido1 = FAsonido;
		double Error = 0.;
		double Diff;

		while (!Converge) {
			H = 0.;
			for (int i = 0; i < FNumeroUniones; i++) {
				if (dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getSentidoFlujo()
						== nmEntrante) {
					SignoFlujo = 1;
				} else if (dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getSentidoFlujo()
						== nmSaliente) {
					SignoFlujo = -1;
				}
				g =
						(double) -dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getMassflow();
				v =
						(double) SignoFlujo
								* dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getVelocity();
				a = dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getSpeedSound();
				m =
						g * DeltaT
								* FCCDeposito[i]->GetTuboExtremo(0).Pipe->getNumeroConductos();
				if (FirstStep) {
					MasaEntrante += m;
					for (int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
						FMasaEspecie[j] +=
								FCCDeposito[i]->GetFraccionMasicaEspecie(j) * m;
					}
				}
				if (v > 0) {
					H += EntalpiaEntrada(a, v, m, Asonido1, FMasa,
							FCCDeposito[i]->getGamma());
				}

			}
			if (FirstStep) {
				FMasa = FMasa0 + MasaEntrante;
				for (int j = 0; j < FNumeroEspecies - 2; j++) {
					FFraccionMasicaEspecie[j] = FMasaEspecie[j] / FMasa;
					FraccionMasicaAcum += FFraccionMasicaEspecie[j];
				}
				FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1.
						- FraccionMasicaAcum;
				if (FHayEGR)
					FFraccionMasicaEspecie[FNumeroEspecies - 1] =
							FMasaEspecie[FNumeroEspecies - 1] / FMasa;
				H0 = H;
				FirstStep = false;
			}

			Energia = pow(FMasa / FMasa0 * exp((H + H0) / 2), FGamma1);
			Asonido1 = FAsonido * sqrt(Energia);
			Error = (Diff = Asonido1 - Asonido0, fabs(Diff)) / Asonido1;
			if (Error > 1e-6) {
				Asonido0 = Asonido1;
			} else {
				FAsonido = Asonido1;
				Converge = true;
			}
		}
		FTemperature = pow2(FAsonido * ARef) / (FGamma * FRMezcla) - 273.;
		FPressure = ARef * ARef * FAsonido * FAsonido / FGamma / FVolumen
				* FMasa * 1e-5;
		FPresionIsen = pow(FPressure / FPresRef, FGamma5);
		FTime = TimeCalculo;
	} catch (Exception & N) {
		std::cout << "ERROR: TVenturi::ActualizaPropiedades en el venturi: "
				<< FNumeroVenturi << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TVenturi::AsignaEntradaSalidaLateralCC() {
	try {

		if (FTipoDeposito == nmVenturi) {
			for (int i = 0; i < FNumeroUniones; i++) {
				if (FCCDeposito[i]->getNumeroCC() == FNodoEntrada) {
					FCCEntrada = FCCDeposito[i];
				} else if (FCCDeposito[i]->getNumeroCC() == FNodoSalida) {
					FCCSalida = FCCDeposito[i];
				} else if (FCCDeposito[i]->getNumeroCC() == FNodoLateral) {
					FCCLateral = FCCDeposito[i];
				}
			}
		}

	} catch (Exception & N) {
		std::cout
				<< "ERROR: TVenturi::AsignaEntradaSalidaLateralCC en el venturi "
				<< FNumeroVenturi << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TVenturi::CalculaVenturi() {

	// El venturi se considera simetrico, es decir, el fluido puede entrar tanto por el
	// ramal de entrada como por el ramal de salida.

	try {
		double Velocity, Speedsound;
		double Converge, VelGarganta0, VelGarganta1;
		double Mach0, Mach1;
		double CSup;
		double TempGarganta, TempEntrada;
		int SentidoEntrada = 0, SentidoSalida = 0;

		TempEntrada = FTemperature + 273.;

		if (dynamic_cast<TCCDeposito*>(FCCEntrada)->getSentidoFlujo()
				== nmEntrante) {
			SentidoEntrada = 1;
		} else if (dynamic_cast<TCCDeposito*>(FCCEntrada)->getSentidoFlujo()
				== nmSaliente) {
			SentidoEntrada = -1;
		}

		Velocity = SentidoEntrada
				* dynamic_cast<TCCDeposito*>(FCCEntrada)->getVelocity();
		Speedsound = dynamic_cast<TCCDeposito*>(FCCEntrada)->getSpeedSound();

		if (Velocity > 0.0) {
			if (Speedsound > 0)
				dynamic_cast<TCCDeposito*>(FCCEntrada)->putMachVenturi(
						Velocity / Speedsound);
			dynamic_cast<TCCDeposito*>(FCCSalida)->putMachVenturi(0.);
// if(Speedsound > 0) dynamic_cast<TCCDeposito *>(FCCEntrada)->getValvula()->getCRecuperacion() = Velocity / Speedsound;
// dynamic_cast<TCCDeposito *>(FCCSalida)->getValvula()->getCRecuperacion() = 0.;
		} else {
			if (dynamic_cast<TCCDeposito*>(FCCSalida)->getSentidoFlujo()
					== nmEntrante) {
				SentidoSalida = 1;
			} else if (dynamic_cast<TCCDeposito*>(FCCSalida)->getSentidoFlujo()
					== nmSaliente) {
				SentidoSalida = -1;
			}
			Velocity = SentidoSalida
					* dynamic_cast<TCCDeposito*>(FCCSalida)->getVelocity();
			Speedsound = dynamic_cast<TCCDeposito*>(FCCSalida)->getSpeedSound();
			dynamic_cast<TCCDeposito*>(FCCEntrada)->putMachVenturi(0.);
			if (Speedsound > 0)
				dynamic_cast<TCCDeposito*>(FCCSalida)->putMachVenturi(
						Velocity / Speedsound);
// dynamic_cast<TCCDeposito *>(FCCEntrada)->getValvula()->getCRecuperacion() = 0.;
// if(Speedsound > 0) dynamic_cast<TCCDeposito *>(FCCSalida)->getValvula()->getCRecuperacion() = Velocity / Speedsound;
		}

		Mach0 = Velocity / FAsonido;
		Mach1 = Mach0 * FRelacionSecciones;

		FCpMezcla = FGamma * FRMezcla / FGamma1;

		TempGarganta = TempEntrada
				- pow((FRelacionSecciones - 1.) * Velocity * ARef, 2.)
						/ (2. * FCpMezcla);

		Converge = 0.;
		VelGarganta0 = Velocity;
		VelGarganta1 = Velocity;

		if (Velocity > 0. && FRelacionSecciones > 0.) {
			while (Converge < 0.99999 || Converge > 1.00001) {
				if (Mach1 > 0.99999999) {
					Mach1 = 1.;
					printf(
							"N. de Mach en el venturi situado en el deposito %d = 1. ",
							FNumeroDeposito);
					printf("Velocity = %g (m/s) \t", VelGarganta1 * ARef);
					printf("Temperature = %g (degC)\n", TempGarganta);
				} else if (Mach1 == 1.) {
					VelGarganta1 = sqrt(
							((1. + FGamma1 / 2. * pow2(Mach0))
									/ (FGamma2 * pow2(Mach0) / 2.)
									* pow2(Velocity)));
				} else {
					VelGarganta1 = FRelacionSecciones * Velocity
							* pow(TempEntrada / TempGarganta, FGamma6);
				}

				TempGarganta = TempEntrada
						- (pow2(VelGarganta1 * ARef) - pow2(Velocity * ARef))
								/ (2. * FCpMezcla);

				Mach1 = VelGarganta1 * ARef
						/ sqrt(FGamma * FRMezcla * TempGarganta);
				Converge = VelGarganta1 / VelGarganta0;
				VelGarganta0 = VelGarganta1;
			}
			VelGarganta0 = sqrt(
					(FRendimientoVenturi * pow2(VelGarganta0)
							- FPerdidasCalor * 2.));
// dynamic_cast<TCCDeposito *>(FCCLateral)->getValvula()->getCRecuperacion() = (VelGarganta0  * ARef)/pow(FGamma*FRMezcla*TempGarganta,0.5);
			dynamic_cast<TCCDeposito*>(FCCLateral)->putMachVenturi(
					(VelGarganta0 * ARef)
							/ pow(FGamma * FRMezcla * TempGarganta, 0.5));
		} else {
// dynamic_cast<TCCDeposito *>(FCCLateral)->getValvula()->getCRecuperacion() = 0.;
			dynamic_cast<TCCDeposito*>(FCCLateral)->putMachVenturi(0.);
		}

	} catch (Exception & N) {
		std::cout << "ERROR: TVenturi::CalculaVenturi en el venturi: "
				<< FNumeroVenturi << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

//void TVenturi::PutNumeroVenturi(int NumeroVenturi) {
//	try {
//		if (!asgNumeroVenturi) {
//			FNumeroVenturi = NumeroVenturi;
//			asgNumeroVenturi = true;
//		}
//		else {
//			std::cout << "ERROR: Este Venturi ya tiene numero asignado" << std::endl;
//			throw Exception("");
//		}
//	}
//	catch(Exception & N) {
//		std::cout << "ERROR: TVenturi::PutNumeroVenturi en la Venturi " << FNumeroVenturi << std::endl;
//		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
//		throw Exception(N.Message.c_str());
//	}
//}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TVenturi::LeeResultadosInstantVenturi(const char *FileWAM,
		fpos_t &filepos) {

	int nvars, var;

	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &nvars);
		for (int i = 0; i < nvars; ++i) {
			fscanf(fich, "%d ", &var);
			switch (var) {
			case 0:
				FResInstantVenturi.PresionEntrada = true;
				break;
			case 1:
				FResInstantVenturi.PresionGarganta = true;
				break;
			case 2:
				FResInstantVenturi.MachEntrada = true;
				break;
			case 3:
				FResInstantVenturi.MachGarganta = true;
				break;
			case 4:
				FResInstantVenturi.VelEntrada = true;
				break;
			case 5:
				FResInstantVenturi.VelLateral = true;
				break;
			case 6:
				FResInstantVenturi.GastoEntrada = true;
				break;
			case 7:
				FResInstantVenturi.GastoLateral = true;
				break;
			default:
				std::cout << "Resultados instantaneos en venturi "
						<< FNumeroVenturi << " no implementados " << std::endl;
			}
		}

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch (Exception & N) {
		std::cout
				<< "ERROR: TVenturi::LeeResultadosInstantVenturi en el venturi: "
				<< FNumeroVenturi << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TVenturi::CabeceraResultadosInstantVenturi(stringstream& insoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");
		AnsiString Label;

		if (FResInstantVenturi.PresionEntrada) {
			Label = "\t" + PutLabel(5006) + "/" + IntToStr(FNumeroVenturi) + "/"
					+ PutLabel(4006) + PutLabel(908) + "/" + PutLabel(3047);
			insoutput << Label.c_str();
		}
		if (FResInstantVenturi.PresionGarganta) {
			Label = "\t" + PutLabel(5006) + "/" + IntToStr(FNumeroVenturi) + "/"
					+ PutLabel(4006) + PutLabel(908) + "/" + PutLabel(3049);
			insoutput << Label.c_str();
		}
		if (FResInstantVenturi.MachEntrada) {
			Label = "\t" + PutLabel(5006) + "/" + IntToStr(FNumeroVenturi) + "/"
					+ PutLabel(4014) + PutLabel(901) + "/" + PutLabel(3047);
			insoutput << Label.c_str();
		}
		if (FResInstantVenturi.MachGarganta) {
			Label = "\t" + PutLabel(5006) + "/" + IntToStr(FNumeroVenturi) + "/"
					+ PutLabel(4014) + PutLabel(901) + "/" + PutLabel(3049);
			insoutput << Label.c_str();
		}
		if (FResInstantVenturi.VelEntrada) {
			Label = "\t" + PutLabel(5006) + "/" + IntToStr(FNumeroVenturi) + "/"
					+ PutLabel(4007) + PutLabel(909) + "/" + PutLabel(3047);
			insoutput << Label.c_str();
		}
		if (FResInstantVenturi.VelLateral) {
			Label = "\t" + PutLabel(5006) + "/" + IntToStr(FNumeroVenturi) + "/"
					+ PutLabel(4007) + PutLabel(909) + "/" + PutLabel(3050);
			insoutput << Label.c_str();
		}
		if (FResInstantVenturi.GastoEntrada) {
			Label = "\t" + PutLabel(5006) + "/" + IntToStr(FNumeroVenturi) + "/"
					+ PutLabel(4008) + PutLabel(904) + "/" + PutLabel(3047);
			insoutput << Label.c_str();
		}
		if (FResInstantVenturi.GastoLateral) {
			Label = "\t" + PutLabel(5006) + "/" + IntToStr(FNumeroVenturi) + "/"
					+ PutLabel(4008) + PutLabel(904) + "/" + PutLabel(3050);
			insoutput << Label.c_str();
		}

		// fclose(fich);
	} catch (Exception & N) {
		std::cout
				<< "ERROR: TVenturi::CabeceraResultadosInstantVenturi en el venturi: "
				<< FNumeroVenturi << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TVenturi::ImprimeResultadosInstantVenturi(stringstream& insoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");

		if (FResInstantVenturi.PresionEntrada)
			insoutput << "\t" << FResInstantVenturi.PresionEntradaINS;
		if (FResInstantVenturi.PresionGarganta)
			insoutput << "\t" << FResInstantVenturi.PresionGargantaINS;
		if (FResInstantVenturi.MachEntrada)
			insoutput << "\t" << FResInstantVenturi.MachEntradaINS;
		if (FResInstantVenturi.MachGarganta)
			insoutput << "\t" << FResInstantVenturi.MachGargantaINS;
		if (FResInstantVenturi.VelEntrada)
			insoutput << "\t" << FResInstantVenturi.VelEntradaINS;
		if (FResInstantVenturi.VelLateral)
			insoutput << "\t" << FResInstantVenturi.VelLateralINS;
		if (FResInstantVenturi.GastoEntrada)
			insoutput << "\t" << FResInstantVenturi.GastoEntradaINS;
		if (FResInstantVenturi.GastoLateral)
			insoutput << "\t" << FResInstantVenturi.GastoLateralINS;

		// fclose(fich);
	} catch (Exception & N) {
		std::cout
				<< "ERROR: TVenturi::ImprimeResultadosInstantVenturi en el venturi: "
				<< FNumeroVenturi << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TVenturi::CalculaResultadosVenturi() {
	try {
		int SentidoEntrada, SentidoLateral, SentidoSalida;
		if (FResInstantVenturi.PresionEntrada) {
			FResInstantVenturi.PresionEntradaINS = FPressure;
		}
		if (FResInstantVenturi.PresionGarganta) {
			FResInstantVenturi.PresionGargantaINS =
					FPressure
							/ pow(
									1
											+ FGamma1 / 2
													* pow2(
															dynamic_cast<TCCDeposito*>(FCCLateral)->getMachVenturi()),
									FGamma / FGamma1);
		}
		if (FResInstantVenturi.MachEntrada) {
			if (dynamic_cast<TCCDeposito*>(FCCEntrada)->getMachVenturi() != 0) {
				FResInstantVenturi.MachEntradaINS =
						fabs(
								dynamic_cast<TCCDeposito*>(FCCEntrada)->getMachVenturi());
			} else {
				FResInstantVenturi.MachEntradaINS =
						-fabs(
								dynamic_cast<TCCDeposito*>(FCCSalida)->getMachVenturi());
			}
		}
		if (FResInstantVenturi.MachGarganta) {
			FResInstantVenturi.MachGargantaINS = fabs(
					dynamic_cast<TCCDeposito*>(FCCLateral)->getMachVenturi());
		}
		if (FResInstantVenturi.VelEntrada) {
			if (dynamic_cast<TCCDeposito*>(FCCEntrada)->getMachVenturi() != 0) {
				if (dynamic_cast<TCCDeposito*>(FCCEntrada)->getSentidoFlujo()
						== nmEntrante) {
					SentidoEntrada = 1;
				} else if (dynamic_cast<TCCDeposito*>(FCCEntrada)->getSentidoFlujo()
						== nmSaliente) {
					SentidoEntrada = -1;
				}
				FResInstantVenturi.VelEntradaINS = SentidoEntrada * ARef
						* dynamic_cast<TCCDeposito*>(FCCEntrada)->getVelocity();
			} else {
				if (dynamic_cast<TCCDeposito*>(FCCSalida)->getSentidoFlujo()
						== nmEntrante) {
					SentidoSalida = 1;
				} else if (dynamic_cast<TCCDeposito*>(FCCSalida)->getSentidoFlujo()
						== nmSaliente) {
					SentidoSalida = -1;
				}
				FResInstantVenturi.VelEntradaINS = SentidoSalida * ARef
						* dynamic_cast<TCCDeposito*>(FCCSalida)->getVelocity();
			}
		}

		if (FResInstantVenturi.VelLateral) {
			if (dynamic_cast<TCCDeposito*>(FCCLateral)->getSentidoFlujo()
					== nmEntrante) {
				SentidoLateral = 1;
			} else if (dynamic_cast<TCCDeposito*>(FCCLateral)->getSentidoFlujo()
					== nmSaliente) {
				SentidoLateral = -1;
			}
			FResInstantVenturi.VelLateralINS = SentidoLateral * ARef
					* dynamic_cast<TCCDeposito*>(FCCLateral)->getVelocity();
		}
		if (FResInstantVenturi.GastoEntrada) {
			if (dynamic_cast<TCCDeposito*>(FCCEntrada)->getMachVenturi() != 0) {
				FResInstantVenturi.GastoEntradaINS =
						-dynamic_cast<TCCDeposito*>(FCCEntrada)->getMassflow();
			} else {
				FResInstantVenturi.GastoEntradaINS =
						-dynamic_cast<TCCDeposito*>(FCCSalida)->getMassflow();
			}
		}
		if (FResInstantVenturi.GastoLateral) {
			FResInstantVenturi.GastoLateralINS =
					-dynamic_cast<TCCDeposito*>(FCCLateral)->getMassflow();
		}

	} catch (Exception & N) {
		std::cout << "ERROR: TVenturi::CalculaResultadosVenturi en el venturi: "
				<< FNumeroVenturi << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TVenturi::ReadAverageResultsVenturi(const char *FileWAM, fpos_t &filepos) {
	int nvars, var;

	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &nvars);
		for (int i = 0; i < nvars; ++i) {
			fscanf(fich, "%d ", &var);
			switch (var) {
			case 0:
				FResMediosVenturi.PresionEntrada = true;
				break;
			case 1:
				FResMediosVenturi.PresionGarganta = true;
				break;
			case 2:
				FResMediosVenturi.MachEntrada = true;
				break;
			case 3:
				FResMediosVenturi.MachGarganta = true;
				break;
			case 4:
				FResMediosVenturi.VelEntrada = true;
				break;
			case 5:
				FResMediosVenturi.VelLateral = true;
				break;
			case 6:
				FResMediosVenturi.GastoEntrada = true;
				break;
			case 7:
				FResMediosVenturi.GastoLateral = true;
				break;
			default:
				std::cout << "Resultados medios en venturi " << FNumeroVenturi
						<< " no implementados " << std::endl;
			}
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch (Exception & N) {
		std::cout
				<< "ERROR: TVenturi::ReadAverageResultsVenturi en el venturi: "
				<< FNumeroVenturi << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TVenturi::HeaderAverageResultsVenturi(stringstream& medoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");
		AnsiString Label;

		if (FResMediosVenturi.PresionEntrada) {
			Label = "\t" + PutLabel(5006) + "/" + IntToStr(FNumeroVenturi) + "/"
					+ PutLabel(4006) + PutLabel(908) + "/" + PutLabel(3047);
			medoutput << Label.c_str();
		}
		if (FResMediosVenturi.PresionGarganta) {
			Label = "\t" + PutLabel(5006) + "/" + IntToStr(FNumeroVenturi) + "/"
					+ PutLabel(4006) + PutLabel(908) + "/" + PutLabel(3049);
			medoutput << Label.c_str();
		}
		if (FResMediosVenturi.MachEntrada) {
			Label = "\t" + PutLabel(5006) + "/" + IntToStr(FNumeroVenturi) + "/"
					+ PutLabel(4014) + PutLabel(901) + "/" + PutLabel(3047);
			medoutput << Label.c_str();
		}
		if (FResMediosVenturi.MachGarganta) {
			Label = "\t" + PutLabel(5006) + "/" + IntToStr(FNumeroVenturi) + "/"
					+ PutLabel(4014) + PutLabel(901) + "/" + PutLabel(3049);
			medoutput << Label.c_str();
		}
		if (FResMediosVenturi.VelEntrada) {
			Label = "\t" + PutLabel(5006) + "/" + IntToStr(FNumeroVenturi) + "/"
					+ PutLabel(4007) + PutLabel(909) + "/" + PutLabel(3047);
			medoutput << Label.c_str();
		}
		if (FResMediosVenturi.VelLateral) {
			Label = "\t" + PutLabel(5006) + "/" + IntToStr(FNumeroVenturi) + "/"
					+ PutLabel(4007) + PutLabel(909) + "/" + PutLabel(3050);
			medoutput << Label.c_str();
		}
		if (FResMediosVenturi.GastoEntrada) {
			Label = "\t" + PutLabel(5006) + "/" + IntToStr(FNumeroVenturi) + "/"
					+ PutLabel(4008) + PutLabel(904) + "/" + PutLabel(3047);
			medoutput << Label.c_str();
		}
		if (FResMediosVenturi.GastoLateral) {
			Label = "\t" + PutLabel(5006) + "/" + IntToStr(FNumeroVenturi) + "/"
					+ PutLabel(4008) + PutLabel(904) + "/" + PutLabel(3050);
			medoutput << Label.c_str();
		}

		// fclose(fich);
	} catch (Exception & N) {
		std::cout
				<< "ERROR: TVenturi::HeaderAverageResultsVenturi en el venturi: "
				<< FNumeroVenturi << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TVenturi::AcumulaResultadosMediosVenturi(double Actual) {
	try {
		int SentidoEntrada, SentidoLateral;
		double Delta = Actual - FResMediosVenturi.Tiempo0;

		if (FResMediosVenturi.PresionEntrada) {
			FResMediosVenturi.PresionEntradaSUM += FPressure * Delta;
		}
		if (FResMediosVenturi.PresionGarganta) {
			FResMediosVenturi.PresionGargantaSUM +=
					(FPressure
							/ pow(
									1
											+ FGamma1 / 2
													* pow(
															dynamic_cast<TCCDeposito*>(FCCLateral)->getMachVenturi(),
															2.),
									FGamma / FGamma1)) * Delta;
		}
		if (FResMediosVenturi.MachEntrada) {
			FResMediosVenturi.MachEntradaSUM += fabs(
					dynamic_cast<TCCDeposito*>(FCCEntrada)->getMachVenturi())
					* Delta;
		}
		if (FResMediosVenturi.MachGarganta) {
			FResMediosVenturi.MachGargantaSUM += fabs(
					dynamic_cast<TCCDeposito*>(FCCLateral)->getMachVenturi())
					* Delta;
		}
		if (FResMediosVenturi.VelEntrada) {
			if (dynamic_cast<TCCDeposito*>(FCCEntrada)->getSentidoFlujo()
					== nmEntrante) {
				SentidoEntrada = 1;
			} else if (dynamic_cast<TCCDeposito*>(FCCEntrada)->getSentidoFlujo()
					== nmSaliente) {
				SentidoEntrada = -1;
			}
			FResMediosVenturi.VelEntradaSUM += SentidoEntrada * ARef * Delta
					* dynamic_cast<TCCDeposito*>(FCCEntrada)->getVelocity();
		}
		if (FResMediosVenturi.VelLateral) {
			if (dynamic_cast<TCCDeposito*>(FCCLateral)->getSentidoFlujo()
					== nmEntrante) {
				SentidoLateral = 1;
			} else if (dynamic_cast<TCCDeposito*>(FCCLateral)->getSentidoFlujo()
					== nmSaliente) {
				SentidoLateral = -1;
			}
			FResMediosVenturi.VelLateralSUM += SentidoLateral * ARef * Delta
					* dynamic_cast<TCCDeposito*>(FCCLateral)->getVelocity();

		}
		if (FResMediosVenturi.GastoEntrada) {
			FResMediosVenturi.GastoEntradaSUM += -Delta
					* dynamic_cast<TCCDeposito*>(FCCEntrada)->getMassflow();
		}
		if (FResMediosVenturi.GastoLateral) {
			FResMediosVenturi.GastoLateralSUM += -Delta
					* dynamic_cast<TCCDeposito*>(FCCLateral)->getMassflow();
		}

		FResMediosVenturi.TiempoSUM += Delta;
		FResMediosVenturi.Tiempo0 = Actual;
	} catch (Exception & N) {
		std::cout
				<< "ERROR: TVenturi::AcumulaResultadosMediosVenturi en el venturi: "
				<< FNumeroVenturi << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TVenturi::ResultadosMediosVenturi() {
	try {
		if (FResMediosVenturi.PresionEntrada) {
			FResMediosVenturi.PresionEntradaMED =
					FResMediosVenturi.PresionEntradaSUM
							/ FResMediosVenturi.TiempoSUM;
			FResMediosVenturi.PresionEntradaSUM = 0.;
		}
		if (FResMediosVenturi.PresionGarganta) {
			FResMediosVenturi.PresionGargantaMED =
					FResMediosVenturi.PresionGargantaSUM
							/ FResMediosVenturi.TiempoSUM;
			FResMediosVenturi.PresionGargantaSUM = 0.;
		}
		if (FResMediosVenturi.MachEntrada) {
			FResMediosVenturi.MachEntradaMED = FResMediosVenturi.MachEntradaSUM
					/ FResMediosVenturi.TiempoSUM;
			FResMediosVenturi.MachEntradaSUM = 0.;
		}
		if (FResMediosVenturi.MachGarganta) {
			FResMediosVenturi.MachGargantaMED =
					FResMediosVenturi.MachGargantaSUM
							/ FResMediosVenturi.TiempoSUM;
			FResMediosVenturi.MachGargantaSUM = 0.;
		}
		if (FResMediosVenturi.VelEntrada) {
			FResMediosVenturi.VelEntradaMED = FResMediosVenturi.VelEntradaSUM
					/ FResMediosVenturi.TiempoSUM;
			FResMediosVenturi.VelEntradaSUM = 0.;
		}
		if (FResMediosVenturi.VelLateral) {
			FResMediosVenturi.VelLateralMED = FResMediosVenturi.VelLateralSUM
					/ FResMediosVenturi.TiempoSUM;
			FResMediosVenturi.VelLateralSUM = 0.;
		}
		if (FResMediosVenturi.GastoEntrada) {
			FResMediosVenturi.GastoEntradaMED =
					FResMediosVenturi.GastoEntradaSUM
							/ FResMediosVenturi.TiempoSUM;
			FResMediosVenturi.GastoEntradaSUM = 0.;
		}
		if (FResMediosVenturi.GastoLateral) {
			FResMediosVenturi.GastoLateralMED =
					FResMediosVenturi.GastoLateralSUM
							/ FResMediosVenturi.TiempoSUM;
			FResMediosVenturi.GastoLateralSUM = 0.;
		}
		FResMediosVenturi.TiempoSUM = 0;
	} catch (Exception & N) {
		std::cout << "ERROR: TVenturi::ResultadosMediosVenturi en el venturi: "
				<< FNumeroVenturi << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TVenturi::ImprimeResultadosMediosVenturi(stringstream& medoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");

		if (FResMediosVenturi.PresionEntrada)
			medoutput << "\t" << FResMediosVenturi.PresionEntradaMED;
		if (FResMediosVenturi.PresionGarganta)
			medoutput << "\t" << FResMediosVenturi.PresionGargantaMED;
		if (FResMediosVenturi.MachEntrada)
			medoutput << "\t" << FResMediosVenturi.MachEntradaMED;
		if (FResMediosVenturi.MachGarganta)
			medoutput << "\t" << FResMediosVenturi.MachGargantaMED;
		if (FResMediosVenturi.VelEntrada)
			medoutput << "\t" << FResMediosVenturi.VelEntradaMED;
		if (FResMediosVenturi.VelLateral)
			medoutput << "\t" << FResMediosVenturi.VelLateralMED;
		if (FResMediosVenturi.GastoEntrada)
			medoutput << "\t" << FResMediosVenturi.GastoEntradaMED;
		if (FResMediosVenturi.GastoLateral)
			medoutput << "\t" << FResMediosVenturi.GastoLateralMED;

		// fclose(fich);
	} catch (Exception & N) {
		std::cout
				<< "ERROR: TVenturi::ImprimeResultadosMediosVenturi en el venturi: "
				<< FNumeroVenturi << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

void TVenturi::UpdateProperties0DModel(double TimeCalculo) {
	ActualizaPropiedades(TimeCalculo);

	CalculaVenturi();

	AcumulaResultadosMediosVenturi(TimeCalculo);

	AcumulaResultadosMedios(TimeCalculo);

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#pragma package(smart_init)
