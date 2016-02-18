/* --------------------------------------------------------------------------------*\
==========================|
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

#include "TEjeTurbogrupo.h"

//#include <cmath>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include "TCompresor.h"
#include "TCompTubDep.h"
#include "TTurbina.h"
#include "fluids.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TEjeTurbogrupo::TEjeTurbogrupo(int i, int ncilin) {

	FNumeroEje = i + 1;
	FNumeroCompresor = NULL;
	FCompresor = NULL;
	FNumeroTurbina = NULL;
	FTurbina = NULL;
	FNumCilindros = ncilin;

	FResMediosEje.Regimen = false;
	FNumCiclo = 0;

	FRPMControlled = false;
	FTime = 0;

	FThereIsHTM = false;

	FHTM = NULL;

	FMechLosses = NULL;
	FMechPower = 0;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TEjeTurbogrupo::~TEjeTurbogrupo() {

	if(FNumeroCompresor != NULL)
		delete[] FNumeroCompresor;
	if(FCompresor != NULL)
		delete[] FCompresor;

	if(FNumeroTurbina != NULL)
		delete[] FNumeroTurbina;
	if(FTurbina != NULL)
		delete[] FTurbina;

#ifdef tchtm
	if(FHTM != NULL)
		delete FHTM;

	if(FMechLosses != NULL)
		delete FMechLosses;
#endif

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TEjeTurbogrupo::ReadTurbochargerAxis(const char *FileWAM, fpos_t &filepos, TCompresor **Compressor,
		TTurbina **Turbine) {
	try {
		int variacion = 0, htm = 0;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		// Lectura del regimen inicial y si se mantiene constante durante el calculo.
		fscanf(fich, "%lf %d", &FRegimenEje, &variacion);
		switch(variacion) {
		case 0:
			FVariacionRegimen = nmVariable;
			break;
		case 1:
			FVariacionRegimen = nmFixed;
			break;
		default:
			std::cout << "ERROR: Reading turbocharger speed variation in axis: " << FNumeroEje << std::endl;
			throw Exception("");
		}
		if(FVariacionRegimen == nmVariable) {
			fscanf(fich, "%lf ", &FMomentoInercia);
		}

		fscanf(fich, "%d ", &FNumCompresoresAcoplados);
		FCompresor = new TCompresor*[FNumCompresoresAcoplados];
		FNumeroCompresor = new int[FNumCompresoresAcoplados];
		for(int i = 0; i < FNumCompresoresAcoplados; ++i) {
			// Identifica que compresores estan acoplados a este eje.
			fscanf(fich, "%d ", &FNumeroCompresor[i]);
			FCompresor[i] = Compressor[FNumeroCompresor[i] - 1];
			FCompresor[i]->IniciaMedias();
		}

		fscanf(fich, "%d ", &FNumTurbinasAcopladas);
		FTurbina = new TTurbina*[FNumTurbinasAcopladas];
		FNumeroTurbina = new int[FNumTurbinasAcopladas];
		for(int i = 0; i < FNumTurbinasAcopladas; ++i) {
			// Identifica que turbinas estan acopladas a este eje.
			fscanf(fich, "%d ", &FNumeroTurbina[i]);
			// numidturbina es un numero que necesita WAMer y que a nosotros no nos interesa.
			FTurbina[i] = Turbine[FNumeroTurbina[i] - 1];
			FTurbina[i]->PutRegimen(FRegimenEje);
		}

		fscanf(fich, "%d ", &FControllerID);
		if(FControllerID > 0)
			FRPMControlled = true;

		// basura para wamer
		int numide = 0;
		fscanf(fich, "%d ", &numide);

#ifdef tchtm
		fscanf(fich, "%d ", &htm);
		if(htm == 1) {

			if(FNumTurbinasAcopladas != 1 || FNumCompresoresAcoplados != 1) {
				std::cout <<
						  "ERROR: Turbocharger heat trasfer model is not adapted for more than one turbine" << std::endl;
				std::cout <<
						  "       or more than one compressor in the same turbocharger"
						  << std::endl;
			}
			FThereIsHTM = true;
			// Geometrical parameters journal bearing
			fscanf(fich, "%lf %lf %lf ", &FDShaft, &FJournalBLengh, &FHD);
			// Geometrical parameters thrust bearing
			fscanf(fich, "%lf %lf", &FTthrustBRmin, &FTthrustBRmax);
			// Geometrical parameters inlet ports
			fscanf(fich, "%lf %lf", &FDoil, &FDwater);
			// Fitting coefficients
			fscanf(fich, "%lf %lf %lf %lf %lf", &FJournalB_K, &Fk_m, &Fk_tb,
				   &FCAC, &FCAT);
			// Wheel areas
			fscanf(fich, "%lf %lf", &FCWArea, &FTWArea);
			double DT = 0., LT = 0.;
			fscanf(fich, "%lf %lf", &DT, &LT);
			double DC = 0., LC = 0.;
			fscanf(fich, "%lf %lf", &DC, &LC);
			double DH = 0., LH = 0.;
			fscanf(fich, "%lf %lf", &DH, &LH);

			// Oil properties.
			fscanf(fich, "%lf %lf %lf ", &FMoil, &FToil, &FPoil);
			double K1 = 0., K2 = 0., K3 = 0.;
			// Oil Voeguel parameters.
			fscanf(fich, "%lf %lf %lf ", &K1, &K2, &K3);
			FOil = new stHTMoil();
			FOil->mu_c1 = K1;
			FOil->mu_c2 = K2;
			FOil->mu_c3 = K3;

			// Water properties in turbocharger.
			fscanf(fich, "%lf %lf ", &FTwater, &FMwater);
			FWater = new stHTMwater();

			FHTM = new TTC_HTM(FOil);

			FMechLosses = new TurboBearings(FOil, FJournalBLengh, FDShaft / 2,
											FHD, FJournalB_K, FCAC, FCAT, FCWArea, FTWArea, Fk_m,
											FTthrustBRmin, FTthrustBRmax, Fk_tb);

			FHTM->Read_HTM(fich);

			FHTM->TurbochargerData(FDShaft, FHD, FDoil, FDwater, DT, LT, DC,
								   LC, DH, LH);

		}
#endif
		fgetpos(fich, &filepos);
		fclose(fich);

	} catch(exception & N) {
		std::cout << "ERROR: TEjeTurbogrupo::ReadTurbochargerAxis in the boundary condition: " << FNumeroEje << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TEjeTurbogrupo::InterpolaValoresMapa() {
	try {

		for(int i = 0; i < FNumCompresoresAcoplados; ++i) {

			FCompresor[i]->InterpolaValoresMapa(FRegimenEje);

		}

	} catch(exception & N) {
		std::cout << "ERROR: TEjeTurbogrupo::InterpolaValoresMapa in the boundary condition: " << FNumeroEje << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TEjeTurbogrupo::CalculaEjesTurbogrupo(double Theta, nmTipoModelado SimulationType, double Time,
		double CrankAngle) {
	try {

		// Calculo del nuevo regimen del turbogrupo.

		double MechWork = 0;
		//  HAY QUE PASAR EL VALOR DE TAMB
		double DeltaTime = Time - FTime;
		FTime = Time;

		if(FRPMControlled) {
			FRegimenEje = FController->Output(FTime);
		} else {
#ifdef tchtm

			double p1 = FCompresor[0]->AcousticC()->P1() * 1e5;
			double p2 = FCompresor[0]->AcousticC()->P2() * 1e5;
			double p3 = FTurbina[0]->AcousticT()->P3() * 1e5;
			double p4 = FTurbina[0]->AcousticT()->P4() * 1e5;
			FMechPower = FMechLosses->P_oil(__units::degCToK(FToil),
											Pi * FRegimenEje / 30, p1, p2, p3, p4, FMoil);
			MechWork = FMechPower * DeltaTime;
			if(FSumTrabajoTurbinas > MechWork) {
				FMechEff = 1 - MechWork / FSumTrabajoTurbinas;
			} else {
				FMechEff = 0;
			}
			double T1 = FCompresor[0]->AcousticC()->T1();
			double T2 = FCompresor[0]->AcousticC()->T2();
			double cr = FCompresor[0]->AcousticC()->CompRatio();
			double efc = FCompresor[0]->getEfficiency();

			FHTM->CompressorWorkingPoint(0, FCompresor[0]->getMassflow(),
										 __units::KTodegC(T1), p1, cr, efc);

			double T3 = FTurbina[0]->AcousticT()->T3();
			double er = FTurbina[0]->AcousticT()->ExpRatio();
			double eft = FTurbina[0]->GetEfficiency();

			FHTM->TurbineWorkingPoint(0, 0, FTurbina[0]->AcousticT()->MassIn(),
									  __units::KTodegC(T3), p3 / 1e5, er, eft);

			FHTM->TurbochargerWorkingPoint(FRegimenEje, FMechEff, FMoil, FToil,
										   FPoil, FTwater, FMwater);

			FHTM->SolveNodeTemperaturesTransient(T3, T2, T1, __units::degCToK(FToil),
												 FMoil, FMechPower, __units::degCToK(FTwater), __units::degCToK(FTamb), Time);
			FHTM->SolveDeltaTempTr();
			FHTM->SolveHeatFlowMatix();

			FCompresor[0]->AcousticC()->PutHeatPower(FHTM->Comp_Heat_Flow());
			FCompresor[0]->AcousticC()->PutHeatPowerIn
			(FHTM->Comp_Heat_Flow_In());

#endif
			if(FVariacionRegimen == nmVariable) {
				if((Theta > 2880. && SimulationType != nmEstacionario) || (SimulationType == nmEstacionario && Theta > 50.
						&& FNumCilindros == 0) || (SimulationType == nmEstacionario && Theta > 2880 && FNumCilindros != 0)) {

					FSumTrabajoCompresores = 0.;
					FSumTrabajoTurbinas = 0.;

					for(int i = 0; i < FNumCompresoresAcoplados; i++) {
						FCompresor[i]->CalculoPotenciaPaso();
						FSumTrabajoCompresores += (FCompresor[i]->getPotenciaPaso() * DeltaTime);
					}

					for(int i = 0; i < FNumTurbinasAcopladas; i++) {
						FTurbina[i]->CalculoPotenciaPaso();
						FSumTrabajoTurbinas += (FTurbina[i]->getPotenciaPaso() * DeltaTime);
					}

					FDeltaReg = __units::Rad_sToRPM(FSumTrabajoTurbinas - FSumTrabajoCompresores - MechWork) /
								(FMomentoInercia * FRegimenEje);
					FRegimenEje += FDeltaReg;

					if(FRegimenEje < 10000)
						FRegimenEje = 10000;
				}
			}
		}

		// Actualizacion del regimen de los compresores acoplados al eje y su interpolacion.
		for(int i = 0; i < FNumCompresoresAcoplados; ++i) {
			if(FCompresor[i]->getModeloCompresor() == nmCompPlenums || FCompresor[i]->getModeloCompresor() == nmCompPipes) {
				FCompresor[i]->InterpolaValoresMapa(FRegimenEje);
			} else if(FCompresor[i]->getModeloCompresor() == nmCompOriginal) {
				if(FVariacionRegimen == nmVariable
				   || (dynamic_cast<TCompTubDep*>(FCompresor[i]))->getEntradaCompresor() != nmAtmosphere) {
					FCompresor[i]->InterpolaValoresMapa(FRegimenEje);
				}
			}
		}
		// Actualizacion del regimen de las turbinas acopladas al eje.
		for(int j = 0; j < FNumTurbinasAcopladas; ++j) {
			FTurbina[j]->PutRegimen(FRegimenEje);
		}

		// Acumulacion de valores medios.

		if(FResMediosEje.Regimen) {
			FResMediosEje.RegimenSUM += FRegimenEje * DeltaTime;
		}
		FResMediosEje.TiempoSUM += DeltaTime;

		// printf("%lf \n",FRegimenEje);

		// Salida de resultados por pantalla.
		// printf("%lf %lf\n",CrankAngle,Theta);

		if(CrankAngle - FAngle0 <= 0. && Theta >= 750.) {
			FNumCiclo++;
			printf("\n");
			printf("*****************************************************\n");
			printf("***TURBOCHARGER AVERAGE VALUES %3d **CYCLE N. %3d ***\n", FNumeroEje, FNumCiclo);
			printf("*****************************************************\n");
			printf("\n");
			for(int j = 0; j < FNumTurbinasAcopladas; ++j) {
				FTurbina[j]->ImprimeResultadosMediosPantalla();
			}
			for(int j = 0; j < FNumCompresoresAcoplados; ++j) {
				FCompresor[j]->CalculaMedias();
				printf("COMPRESSOR WORK %d       = %6.3lf Julios \n", FNumeroCompresor[j], FCompresor[j]->getTrabCiclo());
				printf("COMPRESSOR EFFICIENCY %d = %6.3lf \n", FNumeroCompresor[j], FCompresor[j]->getRendMed());
				printf("COMPRESSOR MASS FLOW %d  = %6.3lf g/s\n", FNumeroCompresor[j], FCompresor[j]->getGastoMed() * 1000);
				printf("COMPRESSOR RATIO %d      = %6.3lf \n", FNumeroCompresor[j], FCompresor[j]->getRCMed());
			}
			printf("TURBOCHARGER SPEED      = %6.3lf r.p.m.\n", FResMediosEje.RegimenSUM / FResMediosEje.TiempoSUM);
			printf("*****************************************************\n\n");
		}
		FAngle0 = CrankAngle;

	} catch(exception & N) {
		std::cout << "ERROR: TEjeTurbogrupo::CalculaEjesTurbogrupo in the boundary condition: " << FNumeroEje << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TEjeTurbogrupo::ReadAverageResultsEje(const char* FileWAM, fpos_t & filepos) {
	try {
		int nvars = 0, var = 0;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &nvars);
		for(int i = 0; i < nvars; i++) {
			fscanf(fich, "%d ", &var);
			switch(var) {
			case 0:
				FResMediosEje.Regimen = true;
				break;
			default:
				std::cout << "Resultados medios en Axis " << FNumeroEje << " no implementados " << std::endl;
			}
		}

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TEjeTurbogrupo::ReadAverageResultsEje en el eje " << FNumeroEje << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TEjeTurbogrupo::CabeceraResultadosMedEje(stringstream & medoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");
		std::string Label;

		if(FResMediosEje.Regimen) {
			Label = "\t" + PutLabel(703) + std::to_string(FNumeroEje) + PutLabel(918);
			medoutput << Label.c_str();
		}

		// fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TEjeTurbogrupo::CabeceraResultadosMedEje en el eje " << FNumeroEje << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TEjeTurbogrupo::ImprimeResultadosMedEje(stringstream & medoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");

		if(FResMediosEje.Regimen)
			medoutput << "\t" << FResMediosEje.RegimenMED;

		// fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TEjeTurbogrupo::ImprimeResultadosMedEje en el eje " << FNumeroEje << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TEjeTurbogrupo::IniciaMedias() {
	try {

		FResMediosEje.RegimenSUM = 0.;
		FResMediosEje.TiempoSUM = 0.;
		FResMediosEje.Tiempo0 = 0.;

	} catch(exception & N) {
		std::cout << "ERROR: TEjeTurbogrupo::IniciaMedias en el eje: " << FNumeroEje << std::endl;
		// std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TEjeTurbogrupo::ResultadosMediosEje() {
	try {

		if(FResMediosEje.Regimen) {
			FResMediosEje.RegimenMED = FResMediosEje.RegimenSUM / FResMediosEje.TiempoSUM;
			FResMediosEje.RegimenSUM = 0.;
		}
		FResMediosEje.TiempoSUM = 0;

	} catch(exception & N) {
		std::cout << "ERROR: TEjeTurbogrupo::ResultadosMediosEje en el eje: " << FNumeroEje << std::endl;
		// std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TEjeTurbogrupo::AcumulaResultadosMediosEje(double Actual) {
	try {
		/* Lo que se hace en esta funcion se realiza dentro del calculo del eje, para asi poder
		 llevar a cabo la salida de resultados medios por pantalla. */
		double Delta = Actual - FResMediosEje.Tiempo0;

		if(FResMediosEje.Regimen) {
			FResMediosEje.RegimenSUM += FRegimenEje * Delta;
		}
		FResMediosEje.TiempoSUM += Delta;
		FResMediosEje.Tiempo0 = Delta;

	} catch(exception & N) {
		std::cout << "ERROR: TEjeTurbogrupo::AcumulaResultadosMediosEje en el eje: " << FNumeroEje << std::endl;
		// std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TEjeTurbogrupo::ReadInstantaneousResultsEje(const char* FileWAM, fpos_t & filepos) {
	try {
		int nvars = 0, var = 0;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		FResInstantEje.Regimen = false;
		FResInstantEje.MechPower = false;
		FResInstantEje.MechEff = false;
		FResInstantEje.NodeTemp = false;
		FResInstantEje.HeatFlow = false;

		fscanf(fich, "%d ", &nvars);
		for(int i = 0; i < nvars; i++) {
			fscanf(fich, "%d ", &var);
			switch(var) {
			case 0:
				FResInstantEje.Regimen = true;
				break;
			case 1:
				FResInstantEje.MechPower = true;
				break;
			case 2:
				FResInstantEje.MechEff = true;
				break;
			case 3:
				FResInstantEje.NodeTemp = true;
				break;
			case 4:
				FResInstantEje.HeatFlow = true;
				break;
			default:
				std::cout << "Instantaneous results in axis " << FNumeroEje << " are not implemented " << std::endl;
			}
		}

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TEjeTurbogrupo::ReadInstantaneousResultsEje en el eje " << FNumeroEje << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TEjeTurbogrupo::HeaderInstantaneousResultsEje(stringstream & insoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");

		std::string Label;

		if(FResInstantEje.Regimen) {
			Label = "\t" + PutLabel(703) + std::to_string(FNumeroEje) + PutLabel(918);
			insoutput << Label.c_str();
		}
		if(FResInstantEje.MechPower) {
			Label = "\t" + PutLabel(713) + std::to_string(FNumeroEje) + PutLabel(903);
			insoutput << Label.c_str();
		}
		if(FResInstantEje.MechEff) {
			Label = "\t" + PutLabel(714) + std::to_string(FNumeroEje) + PutLabel(901);
			insoutput << Label.c_str();
		}
#ifdef tchtm
		if(FResInstantEje.NodeTemp) {
			FHTM->HeaderInsTemperatures(insoutput);
		}
		if(FResInstantEje.HeatFlow) {
			FHTM->HeaderInsHeatFlow(insoutput);
		}
#endif

		// fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TEjeTurbogrupo::HeaderInstantaneousResultsEje en el eje " << FNumeroEje << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TEjeTurbogrupo::ImprimeResultadosInstantaneosEje(stringstream & insoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");

		if(FResInstantEje.Regimen)
			insoutput << "\t" << FResInstantEje.RegimenINS;
		if(FResInstantEje.MechPower)
			insoutput << "\t" << FResInstantEje.MechPowerINS;
		if(FResInstantEje.MechEff)
			insoutput << "\t" << FResInstantEje.MechEffINS;
#ifdef tchtm
		if(FResInstantEje.NodeTemp) {
			FHTM->PrintInsTemperatures(insoutput);
		}
		if(FResInstantEje.HeatFlow) {
			FHTM->PrintInsHeatFlow(insoutput);
		}
#endif

		// fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TEjeTurbogrupo::ImprimeResultadosInstantaneosEje en el eje " << FNumeroEje << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TEjeTurbogrupo::ResultadosInstantEje() {
	try {
		if(FResInstantEje.Regimen)
			FResInstantEje.RegimenINS = FRegimenEje;
		if(FResInstantEje.MechPower)
			FResInstantEje.MechPowerINS = FMechPower;
		if(FResInstantEje.MechEff)
			FResInstantEje.MechEffINS = FMechEff;

	} catch(exception & N) {
		std::cout << "ERROR: TEjeTurbogrupo::ResultadosInstantEje en el eje " << FNumeroEje << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int TEjeTurbogrupo::GetNumeroCompresor(int i) {

	return FNumeroCompresor[i];

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TEjeTurbogrupo::AsignaRPMController(TController * *Controller) {
	if(FRPMControlled)
		FController = Controller[FControllerID - 1];
}

void TEjeTurbogrupo::InitizlizeHTM(double Tamb) {
#ifdef tchtm

	FTamb = Tamb;

	double T1 = FCompresor[0]->AcousticC()->T1();
	double T2 = FCompresor[0]->AcousticC()->T2();
	double T3 = FTurbina[0]->AcousticT()->T3();

	FHTM->AsignTCMechLosses(FMechLosses);

	FHTM->TurbochargerWorkingPoint(FRegimenEje, 0.9, FMoil, FToil, FPoil,
								   FTwater, FMwater);
	FHTM->CompressorData(FCompresor[0]->GetMap()->getPresionRef(),
						 FCompresor[0]->GetMap()->getTempRef(),
						 FCompresor[0]->GetMap()->getTempMeasure(),
						 FCompresor[0]->AcousticC()->Din());
	FHTM->TurbineData(1, 300, FTurbina[0]->getMap()->getTempMeasure(),
					  FTurbina[0]->AcousticT()->DIn());

	FTurbina[0]->AsignTCHTM(FHTM);
	FTurbina[0]->CalculateAdiabaticMap(FCompresor[0]->GetMap()->getTempMeasure()
									  );

	FCompresor[0]->AsignTCHTM(FHTM);
	FCompresor[0]->GetMap()->CalculateAdiabaticEfficiency(FHTM,
			FTurbina[0]->getMap()->getTempMeasure());

	//  TEMPERATURA AMBIENTE
	FHTM->InitializeTemp(T3, T2, T1, FToil, FTwater, Tamb);

#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#pragma package(smart_init)
