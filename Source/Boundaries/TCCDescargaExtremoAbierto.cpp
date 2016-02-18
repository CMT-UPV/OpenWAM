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

#include "TCCDescargaExtremoAbierto.h"
#include "TTubo.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCCDescargaExtremoAbierto::TCCDescargaExtremoAbierto(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel,
		int numeroespecies, nmCalculoGamma GammaCalculation, bool ThereIsEGR) :
	TCondicionContorno(TipoCC, numCC, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {
	if(TipoCC == nmOpenEndAtmosphere) {
		FTipoDescarga = nmDescargaAtmosfera;
	} else if(TipoCC == nmOpenEndReservoir) {
		FTipoDescarga = nmDescargaRemanso;
	} else if(TipoCC == nmOpenEndCalcExtern) {
		FTipoDescarga = nmDescargaRemansoMatlab;
	} else
		printf("ERROR:TCCDescargaExtremoAbierto:Asignacion Tipo BC,en la condicion de contorno: %d\n", FNumeroCC);

	FComposicion = NULL;
	FTuboExtremo = NULL;
	FVelocidadSonidoDep = 0;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCCDescargaExtremoAbierto::~TCCDescargaExtremoAbierto() {
	delete[] FTuboExtremo;
	if(FComposicion != NULL)
		delete[] FComposicion;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCDescargaExtremoAbierto::AsignAmbientConditions(double Tamb, double Pamb, double *AtmosphericComposition) {

	double RMezclaDep = 0., CvMezclaDep = 0., CpMezclaDep = 0., GammaDep = 0.;
	int modeladoescape = 0;

	FPressure = Pamb;
	FTemperaturaDep = Tamb;
	// Inicializacion del transporte de especies quimicas.
	FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
	FComposicion = new double[FNumeroEspecies - FIntEGR];
	for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
		FComposicion[i] = AtmosphericComposition[i];
		FFraccionMasicaEspecie[i] = FTuboExtremo[0].Pipe->GetFraccionMasicaInicial(i);
	}
	if(FCalculoEspecies == nmCalculoCompleto) {

		RMezclaDep = CalculoCompletoRMezcla(FComposicion[0], FComposicion[1], FComposicion[2], 0, FCalculoGamma, nmMEP);
		CpMezclaDep = CalculoCompletoCpMezcla(FComposicion[0], FComposicion[1], FComposicion[2], 0,
											  __units::degCToK(FTemperaturaDep), FCalculoGamma, nmMEP);
		GammaDep = CalculoCompletoGamma(RMezclaDep, CpMezclaDep, FCalculoGamma);

	} else if(FCalculoEspecies == nmCalculoSimple) {

		RMezclaDep = CalculoSimpleRMezcla(FComposicion[0], 0, FCalculoGamma, nmMEP);
		CvMezclaDep = CalculoSimpleCvMezcla(__units::degCToK(FTemperaturaDep), FComposicion[0], 0, FCalculoGamma, nmMEP);
		GammaDep = CalculoSimpleGamma(RMezclaDep, CvMezclaDep, FCalculoGamma);

	}
	FVelocidadSonidoDep = sqrt(__units::degCToK(FTemperaturaDep) * GammaDep * RMezclaDep) / __cons::ARef;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCDescargaExtremoAbierto::ReadBoundaryData(const char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe,
		int nDPF, TDPF **DPF) {
	try {
		int i = 0;
		double fracciontotal = 0.;
		double RMezclaDep = 0., CvMezclaDep = 0., CpMezclaDep = 0., GammaDep = 0.;
		int modeladoescape = 0;

		FTuboExtremo = new stTuboExtremo[1];
		FTuboExtremo[0].Pipe = NULL;

		FPref = 1;

		while(FNumeroTubosCC < 1 && i < NumberOfPipes) {
			if(Pipe[i]->getNodoIzq() == FNumeroCC) {
				FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
				FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmLeft;
				FCC = &(FTuboExtremo[FNumeroTubosCC].Beta);
				FCD = &(FTuboExtremo[FNumeroTubosCC].Landa);
				FNodoFin = 0;
				FIndiceCC = 0;
				FNumeroTubosCC++;
			}
			if(Pipe[i]->getNodoDer() == FNumeroCC) {
				FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
				FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmRight;
				FCC = &(FTuboExtremo[FNumeroTubosCC].Landa);
				FCD = &(FTuboExtremo[FNumeroTubosCC].Beta);
				FNodoFin = FTuboExtremo[FNumeroTubosCC].Pipe->getNin() - 1;
				FIndiceCC = 1;
				FNumeroTubosCC++;
			}
			i++;
		}

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		if(FTipoDescarga == nmDescargaAtmosfera) {   // DESCARGA A LA ATMOSFERA
			fscanf(fich, "%lf ", &FPerdidaExtremo);

		} else if(FTipoDescarga == nmDescargaRemanso) {   // DESCARGA A UN DEPOSITO DE REMANSO
			fscanf(fich, "%lf %lf %lf ", &FPressure, &FTemperaturaDep, &FPerdidaExtremo);

			// Se determina si este remanso modela el escape del motor.
			fscanf(fich, "%d ", &modeladoescape);
			modeladoescape == 0 ? FModeladoEscape = false : FModeladoEscape = true;

			// Inicializacion del transporte de especies quimicas.
			FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
			FComposicion = new double[FNumeroEspecies - FIntEGR];
			for(int i = 0; i < FNumeroEspecies - 1; i++) {
				fscanf(fich, "%lf ", &FComposicion[i]);
				FFraccionMasicaEspecie[i] = FTuboExtremo[0].Pipe->GetFraccionMasicaInicial(i);
				fracciontotal += FComposicion[i];
			}
			if(FHayEGR) {
				FFraccionMasicaEspecie[FNumeroEspecies - 1] = FTuboExtremo[0].Pipe->GetFraccionMasicaInicial(FNumeroEspecies - 1);
				if(FCalculoEspecies == nmCalculoCompleto) {
					if(FComposicion[0] > 0.20)
						FComposicion[FNumeroEspecies - 1] = 0.;
					else
						FComposicion[FNumeroEspecies - 1] = 1.;
				} else {
					if(FComposicion[0] > 0.50)
						FComposicion[FNumeroEspecies - 1] = 1.;
					else
						FComposicion[FNumeroEspecies - 1] = 0.;
				}
			}
			if(fracciontotal < 1. - 1e-10 || fracciontotal > 1. + 1e-10) {
				std::cout << "ERROR: Total mass fraction must be equal to 1. Check the input data for boundary condition  " << FNumeroCC
						  << std::endl;
				throw Exception(" ");
			}
			if(FCalculoEspecies == nmCalculoCompleto) {

				RMezclaDep = CalculoCompletoRMezcla(FComposicion[0], FComposicion[1], FComposicion[2], 0, FCalculoGamma, nmMEP);
				CpMezclaDep = CalculoCompletoCpMezcla(FComposicion[0], FComposicion[1], FComposicion[2], 0,
													  __units::degCToK(FTemperaturaDep), FCalculoGamma, nmMEP);
				GammaDep = CalculoCompletoGamma(RMezclaDep, CpMezclaDep, FCalculoGamma);

			} else if(FCalculoEspecies == nmCalculoSimple) {

				RMezclaDep = CalculoSimpleRMezcla(FComposicion[0], 0, FCalculoGamma, nmMEP);
				CvMezclaDep = CalculoSimpleCvMezcla(__units::degCToK(FTemperaturaDep), FComposicion[0], 0, FCalculoGamma, nmMEP);
				GammaDep = CalculoSimpleGamma(RMezclaDep, CvMezclaDep, FCalculoGamma);

			}
			FVelocidadSonidoDep = sqrt(__units::degCToK(FTemperaturaDep) * GammaDep * RMezclaDep) / __cons::ARef;

		} else if(FTipoDescarga == nmDescargaRemansoMatlab) {
			fscanf(fich, "%lf %lf %lf ", &FPressure, &FTemperaturaDep, &FPerdidaExtremo);

			// Se determina si este remanso modela el escape del motor.
			fscanf(fich, "%d ", &modeladoescape);
			modeladoescape == 0 ? FModeladoEscape = false : FModeladoEscape = true;

			// Inicializacion del transporte de especies quimicas.
			FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
			FComposicion = new double[FNumeroEspecies - FIntEGR];
			for(int i = 0; i < FNumeroEspecies - 1; i++) {
				fscanf(fich, "%lf ", &FComposicion[i]);
				FFraccionMasicaEspecie[i] = FTuboExtremo[0].Pipe->GetFraccionMasicaInicial(i);
				fracciontotal += FComposicion[i];
			}
			if(FHayEGR) {
				FFraccionMasicaEspecie[FNumeroEspecies - 1] = FTuboExtremo[0].Pipe->GetFraccionMasicaInicial(FNumeroEspecies - 1);
				if(FCalculoEspecies == nmCalculoCompleto) {
					if(FComposicion[0] > 0.2)
						FComposicion[FNumeroEspecies - 1] = 0.;
					else
						FComposicion[FNumeroEspecies - 1] = 1.;
				} else {
					if(FComposicion[0] > 0.5)
						FComposicion[FNumeroEspecies - 1] = 1.;
					else
						FComposicion[FNumeroEspecies - 1] = 0.;
				}
			}
			if(fracciontotal != 1.) {
				std::cout <<
						  "ERROR: La fraccion masica total no puede ser distinta de 1. Repasa la lectura en la condicion de contorno  " <<
						  FNumeroCC << std::endl;
				throw Exception(" ");
			}
			if(FCalculoEspecies == nmCalculoCompleto) {

				RMezclaDep = CalculoCompletoRMezcla(FComposicion[0], FComposicion[1], FComposicion[2], 0, FCalculoGamma, nmMEP);
				CpMezclaDep = CalculoCompletoCpMezcla(FComposicion[0], FComposicion[1], FComposicion[2], 0,
													  __units::degCToK(FTemperaturaDep), FCalculoGamma, nmMEP);
				GammaDep = CalculoCompletoGamma(RMezclaDep, CpMezclaDep, FCalculoGamma);

			} else if(FCalculoEspecies == nmCalculoSimple) {

				RMezclaDep = CalculoSimpleRMezcla(FComposicion[0], 0, FCalculoGamma, nmMEP);
				CvMezclaDep = CalculoSimpleCvMezcla(__units::degCToK(FTemperaturaDep), FComposicion[0], 0, FCalculoGamma, nmMEP);
				GammaDep = CalculoSimpleGamma(RMezclaDep, CvMezclaDep, FCalculoGamma);

			}
			FVelocidadSonidoDep = sqrt(__units::degCToK(FTemperaturaDep) * GammaDep * RMezclaDep) / __cons::ARef;

		} else
			printf("ERROR:TCCDescargaExtremoAbierto::LeeDescargaExtremoAbierto.Asignacion Tipo BC\n");

		fgetpos(fich, &filepos);
		fclose(fich);

	} catch(exception & N) {
		std::cout << "ERROR: TCCDescargaExtremoAbierto::LeeDescargaExtremoAbierto en la condicion de contorno: " << FNumeroCC <<
				  std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

//void TCCDescargaExtremoAbierto::PutPresion(double valor) {
//	try {
//		FPressure = valor;
//	}
//	catch(Exception & N) {
//		std::cout << "ERROR: TCCDescargaExtremoAbierto::PutPresion en la condicion de contorno: " <<
//			FNumeroCC << std::endl;
//		std::cout << "Tipo de error: " << N.what() << std::endl;
//		throw Exception(N.what());
//	}
//}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

//void TCCDescargaExtremoAbierto::PutTemperatura(double valor) {
//	try {
//		double RMezclaDep = 0., CvMezclaDep = 0., CpMezclaDep = 0., GammaDep = 0.;
//
//		FTemperaturaDep = valor;
//		if (FCalculoEspecies == nmCalculoCompleto) {
//
//			RMezclaDep = CalculoCompletoRMezcla(FComposicion[0], FComposicion[1], FComposicion[2],
//				FCalculoGamma);
//			CpMezclaDep = CalculoCompletoCpMezcla(FComposicion[0], FComposicion[1],
//				FComposicion[2], FTemperaturaDep, FCalculoGamma, 0);
//			GammaDep = CalculoCompletoGamma(RMezclaDep, CpMezclaDep, FCalculoGamma, 0);
//
//		}
//		else if (FCalculoEspecies == nmCalculoSimple) {
//
//			RMezclaDep = CalculoSimpleRMezcla(FComposicion[0], FCalculoGamma, 0);
//			CvMezclaDep = CalculoSimpleCvMezcla(FTemperaturaDep, FComposicion[0], FCalculoGamma, 0);
//			GammaDep = CalculoSimpleGamma(RMezclaDep, CvMezclaDep, FCalculoGamma);
//
//		}
//		FVelocidadSonidoDep = sqrt(FTemperaturaDep * GammaDep * RMezclaDep) / __cons::ARef;
//	}
//	catch(Exception & N) {
//		std::cout << "ERROR: TCCDescargaExtremoAbierto::PutTemperatura en la condicion de contorno: " <<
//			FNumeroCC << std::endl;
//		std::cout << "Tipo de error: " << N.what() << std::endl;
//		throw Exception(N.what());
//	}
//}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCDescargaExtremoAbierto::CalculaCondicionContorno(double Time) {
	try {
		double rel_CCon_entropia, yyy, pplo, aap, xyx, b, a2, c, u_isen, a_isen, a_real, u_real;
		double FraccionMasicaAcum = 0.;

		FGamma = FTuboExtremo[0].Pipe->GetGamma(FNodoFin);
		FGamma3 = __Gamma::G3(FGamma);

		rel_CCon_entropia = *FCC / FTuboExtremo[0].Entropia;
		yyy = pow(FPressure / FPref, __Gamma::G5(FGamma));
		if(rel_CCon_entropia / yyy >= 1.000005) {   // Flujo Saliente del conducto.
			/* ________ */
			/* caso > 1 */
			/* ________ */
			*FCD = FTuboExtremo[0].Entropia * 2. * yyy - *FCC;
			pplo = __Gamma::G7(FGamma) * *FCC; /* Condicion flujo supersonico */
			if(*FCD < pplo) {
				*FCD = pplo;
			}
			// Transporte de especies quimicas.
			for(int j = 0; j < FNumeroEspecies - 2; j++) {
				FFraccionMasicaEspecie[j] = FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC, j);
				FraccionMasicaAcum += FFraccionMasicaEspecie[j];
			}
			FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
			if(FHayEGR)
				FFraccionMasicaEspecie[FNumeroEspecies - 1] = FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC, FNumeroEspecies - 1);

		} else if(rel_CCon_entropia / yyy < .999995) {   // Flujo Entrante al conducto.
			/* ________ */
			/* caso < 1 */
			/* ________ */
			aap = FVelocidadSonidoDep / yyy;
			xyx = aap / FTuboExtremo[0].Entropia;
			b = __Gamma::G1(FGamma) * *FCC * pow2(xyx) * FPerdidaExtremo;
			a2 = pow2(FGamma3 * xyx * FPerdidaExtremo) + FGamma3;
			c = pow2(xyx * *FCC) - pow2(FVelocidadSonidoDep);
			u_isen = (-b + sqrt(pow2(b) - a2 * 4. * c)) / (2. * a2);
			// Resolucion ecuacion de segundo grado
			a_isen = sqrt(pow2(FVelocidadSonidoDep) - FGamma3 * pow2(u_isen));
			u_real = u_isen * FPerdidaExtremo; // Con esta relacion obtenemos la velocidad real.
			a_real = sqrt(pow2(FVelocidadSonidoDep) - FGamma3 * pow2(u_real));
			aap = a_real / a_isen * aap;
			if(fabs(u_real) > a_real) {  /* Condicion flujo supersonico */
				a_real = sqrt(2 / __Gamma::G2(FGamma)) * FVelocidadSonidoDep;
				u_real = a_real;
				aap = FTuboExtremo[0].Entropia * a_real / (*FCC + FGamma3 * u_real);
			}
			*FCC = a_real - FGamma3 * u_real;
			*FCD = a_real + FGamma3 * u_real;
			FTuboExtremo[0].Entropia = aap;

			if(!FModeladoEscape) {
				for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
					FFraccionMasicaEspecie[j] = FComposicion[j];
				}
			}

		} else { // Flujo Parado
			/* ________ */
			/* caso = 1 */
			/* ________ */
			*FCD = *FCC;
			// La composicion se mantiene, al estar el flujo parado.

		}

	} catch(exception & N) {
		std::cout << "ERROR: TCCDescargaExtremoAbierto::CalculaCondicionesContorno en la condicion de contorno: " << FNumeroCC
				  << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#pragma package(smart_init)
