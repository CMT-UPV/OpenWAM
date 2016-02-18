/*--------------------------------------------------------------------------------*\
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


 \*--------------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
#pragma hdrstop

#include "TCCPerdidadePresion.h"

//#include <cmath>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include "TTubo.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCCPerdidadePresion::TCCPerdidadePresion(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel,
		int numeroespecies, nmCalculoGamma GammaCalculation, bool ThereIsEGR) :
	TCondicionContorno(TipoCC, numCC, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {

	FTuboExtremo = NULL;
	FCC = NULL;
	FCD = NULL;
	FNodoFin = NULL;
	FIndiceCC = NULL;
	FNumeroTubo = NULL;

	if(TipoCC == nmLinearPressureLoss)
		FTipoPP = nmPPLineal;
	else if(TipoCC == nmQuadraticPressureLoss)
		FTipoPP = nmPPCuadratica;
	else
		printf("ERROR en tipo de perdida de presion TCCPerdidadePresion en la condicion de contorno: %d\n", FNumeroCC);

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCCPerdidadePresion::~TCCPerdidadePresion() {
	delete[] FTuboExtremo;

	if(FNodoFin != NULL)
		delete[] FNodoFin;
	if(FIndiceCC != NULL)
		delete[] FIndiceCC;
	if(*FCC != NULL)
		delete[] FCC;
	if(*FCD != NULL)
		delete[] FCD;
	if(FNumeroTubo != NULL)
		delete[] FNumeroTubo;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCPerdidadePresion::ReadBoundaryData(const char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe,
		int nDPF, TDPF **DPF) {
	try {
		int i = 0;
		FK = 0;

		FTuboExtremo = new stTuboExtremo[2];
		FNodoFin = new int[2];
		FIndiceCC = new int[2];
		FCC = new double*[2];
		FCD = new double*[2];
		FNumeroTubo = new int[2];

		for(int i = 0; i < 2; i++) {
			FTuboExtremo[i].Pipe = NULL;
		}

		while(FNumeroTubosCC < 2 && i < NumberOfPipes) {
			if(Pipe[i]->getNodoIzq() == FNumeroCC) {
				FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
				FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmLeft;
				FNodoFin[FNumeroTubosCC] = 0;
				FIndiceCC[FNumeroTubosCC] = 0;
				FNumeroTubo[FNumeroTubosCC] = Pipe[i]->getNumeroTubo() - 1;
				FCC[FNumeroTubosCC] = &(FTuboExtremo[FNumeroTubosCC].Beta);
				FCD[FNumeroTubosCC] = &(FTuboExtremo[FNumeroTubosCC].Landa);
				FNumeroTubosCC++;
			}
			if(Pipe[i]->getNodoDer() == FNumeroCC) {
				FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
				FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmRight;
				FNodoFin[FNumeroTubosCC] = Pipe[i]->getNin() - 1;
				FIndiceCC[FNumeroTubosCC] = 1;
				FNumeroTubo[FNumeroTubosCC] = Pipe[i]->getNumeroTubo() - 1;
				FCC[FNumeroTubosCC] = &(FTuboExtremo[FNumeroTubosCC].Landa);
				FCD[FNumeroTubosCC] = &(FTuboExtremo[FNumeroTubosCC].Beta);
				FNumeroTubosCC++;
			}
			i++;
		}

// Inicializacion del transporte de especies quimicas.
		FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
		for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
			// Se elige como composicion inicial la del tubo 0. Es arbitrario.
			FFraccionMasicaEspecie[i] = FTuboExtremo[0].Pipe->GetFraccionMasicaInicial(i);
		}

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%lf ", &FK); /* Coeficiente de perdidas con signo positivo */

		fgetpos(fich, &filepos);
		fclose(fich);

	} catch(exception &N) {
		std::cout << "ERROR: TCCPerdidadePresion::LecturaPerdidaPresion en la condicion de contorno: " << FNumeroCC <<
				  std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCPerdidadePresion::TuboCalculandose(int TuboActual) {
	try {
		FTuboActual = TuboActual;
	} catch(exception &N) {
		std::cout << "ERROR: TCCPerdidadePresion::TuboCalculandose en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCPerdidadePresion::CalculaCondicionContorno(double Time) {
	try {

		double vel_sonido_Out = 0., vel_Out = 0., vel_sonido_In = 0., vel_In = 0., xx3 = 0., ei = 0, ed = 0;
		double flujo, FraccionMasicaAcum = 0.;
		int TuboCalculado = 0;

		if(FTuboActual == 10000) {
			TuboCalculado = FTuboActual;
			FGamma = FTuboExtremo[0].Pipe->GetGamma(FNodoFin[0]);
		} else {
			for(int i = 0; i < FNumeroTubosCC; i++) {
				if(FNumeroTubo[i] == FTuboActual) {
					TuboCalculado = i;
				}
			}
			FGamma = FTuboExtremo[TuboCalculado].Pipe->GetGamma(FNodoFin[TuboCalculado]);
		}
		FGamma1 = __Gamma::G1(FGamma);
		FGamma3 = __Gamma::G3(FGamma);
		FGamma2 = __Gamma::G2(FGamma);
		FGamma5 = __Gamma::G5(FGamma);

		flujo = (*FCC[1] / FTuboExtremo[1].Entropia) / (*FCC[0] / FTuboExtremo[0].Entropia);

		if(flujo < .999995) {  /* Flujo de 0 (Saliente) a 1 (Entrante) */

			FRelacionEntropia = FTuboExtremo[0].Entropia / FTuboExtremo[1].Entropia;

			/*Acotacion del intervalo de busqueda para A1 (Velocity del sonido a la entrada)*/
			if((*FCC[0] * 2 / FGamma2) < (*FCC[0] / (FGamma3 / sqrt(FK) + 1) + 1e-6)) {
				ei = *FCC[0] / (FGamma3 / sqrt(FK) + 1) + 1e-6;
			} else {
				ei = *FCC[0] * 2 / FGamma2;
			}
			ed = *FCC[0];

			if(FTipoPP == nmPPLineal) {  /* Perdida lineal */

				ei = QuadraticEqP(FGamma1, 2 * FK, -2 * FK * *FCC[0]) + 1e-6;
				ed = *FCC[0];

				stPerdPresAdL PPAL(*FCC[0], *FCC[1], FK, FGamma, FRelacionEntropia, __cons::ARef);
				vel_sonido_Out = FindRoot(PPAL, ei, ed);
				vel_sonido_In = PPAL.A2;
				vel_In = PPAL.U2;
				vel_Out = PPAL.U1;
				xx3 = PPAL.xx3;

			} else if(FTipoPP == nmPPCuadratica) {  /* Perdida cuadratica */

				stPerdPresAd PPA(*FCC[0], *FCC[1], FK, FGamma, FRelacionEntropia);
				vel_sonido_Out = FindRoot(PPA, ei, ed);
				vel_sonido_In = PPA.A2;
				vel_In = PPA.U2;
				vel_Out = PPA.U1;
				xx3 = PPA.xx3;
				/*nuevo	if (abs(vel_sonido_Out-vel_Out)<1E-12) {
				 printf ("");
				 }               */

				if(PPA.U1 * PPA.U2 < 0) {
					PPA.U1 = 0;
					PPA.U2 = 0;
				}

			} else
				printf("Error en el tipo de perdida de presion en la condicion de contorno: %d\n", FNumeroCC);

			if(TuboCalculado == 1) {
				*FCC[1] = vel_sonido_In - FGamma3 * vel_In;
				*FCD[1] = vel_sonido_In + FGamma3 * vel_In;
				FTuboExtremo[1].Entropia = FTuboExtremo[1].Entropia * vel_sonido_In / xx3;

			} else if(TuboCalculado == 0) {
				*FCC[0] = vel_sonido_Out + FGamma3 * vel_Out;
				*FCD[0] = vel_sonido_Out - FGamma3 * vel_Out;

			} else if(TuboCalculado == 10000) {
				*FCC[1] = vel_sonido_In - FGamma3 * vel_In;
				*FCD[1] = vel_sonido_In + FGamma3 * vel_In;
				FTuboExtremo[1].Entropia = FTuboExtremo[1].Entropia * vel_sonido_In / xx3;
				*FCC[0] = vel_sonido_Out + FGamma3 * vel_Out;
				*FCD[0] = vel_sonido_Out - FGamma3 * vel_Out;

			}

			// Chemical species transport
			for(int j = 0; j < FNumeroEspecies - 2; j++) {
				FFraccionMasicaEspecie[j] = FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC[0], j);
				FraccionMasicaAcum += FFraccionMasicaEspecie[j];
			}
			FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
			if(FHayEGR)
				FFraccionMasicaEspecie[FNumeroEspecies - 1] = FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC[0],
						FNumeroEspecies - 1);
		} else if(flujo >= 1.000005) {  /* Flujo de 1 (Saliente) a 0 (Entrante) */

			FRelacionEntropia = FTuboExtremo[1].Entropia / FTuboExtremo[0].Entropia;

			if((*FCC[1] * 2 / FGamma2) < (*FCC[1] / (FGamma3 / sqrt(FK) + 1) + 1e-6)) {
				ei = *FCC[1] / (FGamma3 / sqrt(FK) + 1) + 1e-6;
			} else {
				ei = *FCC[1] * 2 / FGamma2;
			}
			ed = *FCC[1];

			if(FTipoPP == nmPPLineal) {  /* Linear pressure loss */

				ei = QuadraticEqP(FGamma1, 2 * FK, -2 * FK * *FCC[1]) + 1e-6;
				ed = *FCC[1];

				stPerdPresAdL PPAL(*FCC[1], *FCC[0], FK, FGamma, FRelacionEntropia, __cons::ARef);
				vel_sonido_Out = FindRoot(PPAL, ei, ed);
				vel_sonido_In = PPAL.A2;
				vel_In = PPAL.U1;
				vel_Out = PPAL.U2;
				xx3 = PPAL.xx3;

			} else if(FTipoPP == nmPPCuadratica) {  /* Quadratic pressure loss */
				stPerdPresAd PPA(*FCC[1], *FCC[0], FK, FGamma, FRelacionEntropia);
				vel_sonido_Out = FindRoot(PPA, ei, ed);
				vel_sonido_In = PPA.A2;
				vel_In = PPA.U1;
				vel_Out = PPA.U2;
				xx3 = PPA.xx3;

			} else
				printf("Error en el tipo de perdida de presion en la condicion de contorno: %d\n", FNumeroCC);

			if(TuboCalculado == 0) {
				*FCC[0] = vel_sonido_In - FGamma3 * vel_In;
				*FCD[0] = vel_sonido_In + FGamma3 * vel_In;
				FTuboExtremo[0].Entropia = FTuboExtremo[0].Entropia * vel_sonido_In / xx3;

			} else if(TuboCalculado == 1) {
				*FCC[1] = vel_sonido_Out + FGamma3 * vel_Out;
				*FCD[1] = vel_sonido_Out - FGamma3 * vel_Out;

			} else if(TuboCalculado == 10000) {
				*FCC[0] = vel_sonido_In - FGamma3 * vel_In;
				*FCD[0] = vel_sonido_In + FGamma3 * vel_In;
				FTuboExtremo[0].Entropia = FTuboExtremo[0].Entropia * vel_sonido_In / xx3;
				*FCC[1] = vel_sonido_Out + FGamma3 * vel_Out;
				*FCD[1] = vel_sonido_Out - FGamma3 * vel_Out;
			}

			// Transporte de Especies Quimicas
			// Se actualiza todos los instantes de calculo.
			for(int j = 0; j < FNumeroEspecies - 2; j++) {
				FFraccionMasicaEspecie[j] = FTuboExtremo[1].Pipe->GetFraccionMasicaCC(FIndiceCC[1], j);
				FraccionMasicaAcum += FFraccionMasicaEspecie[j];
			}
			FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
			if(FHayEGR)
				FFraccionMasicaEspecie[FNumeroEspecies - 1] = FTuboExtremo[1].Pipe->GetFraccionMasicaCC(FIndiceCC[1],
						FNumeroEspecies - 1);

		} else {
			if(TuboCalculado == 1) {
				*FCD[1] = *FCC[1];
			} else if(TuboCalculado == 0) {
				*FCD[0] = *FCC[0];
			} else if(TuboCalculado == 10000) {
				*FCD[0] = *FCC[0];
				*FCD[1] = *FCC[1];
			}

			// La composicion se mantiene, al estar el flujo parado.
		}
	}

	catch(exception &N) {
		std::cout << "ERROR: TCCPerdidadePresion::CalculaCondicionContorno en la condicion de contorno: " << FNumeroCC <<
				  std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)
