/* --------------------------------------------------------------------------------*\
|==========================|
 |\\   /\ /\   // O pen     | OpenWAM: The Open Source 1D Gas-Dynamic Code
 | \\ |  X  | //  W ave     |
 |  \\ \/_\/ //   A ction   | CMT-Motores Termicos / Universidad Politecnica Valencia
 |   \\/   \//    M odel    |
 |----------------------------------------------------------------------------------
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

#include "TCCUnionEntreTubos.h"
//#include <cmath>
#include <iostream>
#include "TTubo.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCCUnionEntreTubos::TCCUnionEntreTubos(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel,
									   int numeroespecies, nmCalculoGamma GammaCalculation, bool ThereIsEGR) :
	TCondicionContorno(TipoCC, numCC, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {

	FTuboExtremo = NULL;
	FNodoFin = NULL;
	FIndiceCC = NULL;
	FNumeroTubo = NULL;
	FTubo = NULL;
	FCC = NULL;
	FCD = NULL;

}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCCUnionEntreTubos::~TCCUnionEntreTubos() {

	if(FTuboExtremo != NULL)
		delete[] FTuboExtremo;
	if(FNodoFin != NULL)
		delete[] FNodoFin;
	if(FIndiceCC != NULL)
		delete[] FIndiceCC;
	if(FNumeroTubo != NULL)
		delete[] FNumeroTubo;
	if(FTubo != NULL)
		delete[] FTubo;
	if(FCC != NULL)
		delete[] FCC;
	if(FCD != NULL)
		delete[] FCD;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCUnionEntreTubos::ReadBoundaryData(const char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe,
		int nDPF, TDPF **DPF) {
	try {
		int i = 0;

		FEspesor = 0.;
		FConductividad = 0.;

		FTuboExtremo = new stTuboExtremo[2];
		FNodoFin = new int[2];
		FIndiceCC = new int[2];
		FCC = new double*[2];
		FCD = new double*[2];
		FNumeroTubo = new int[2];
		FTubo = new int[2];

		for(int i = 0; i < 2; i++) {
			FTuboExtremo[i].Pipe = NULL;
		}

		while(FNumeroTubosCC < 2 && i < NumberOfPipes) {
			if(Pipe[i]->getNodoIzq() == FNumeroCC || Pipe[i]->getNodoDer() == FNumeroCC) {
				FTubo[FNumeroTubosCC] = i;
				if(Pipe[FTubo[FNumeroTubosCC]]->getNodoIzq() == FNumeroCC) {
					FNodoFin[FNumeroTubosCC] = 0;
				}
				if(Pipe[FTubo[FNumeroTubosCC]]->getNodoDer() == FNumeroCC) {
					FNodoFin[FNumeroTubosCC] = Pipe[FTubo[FNumeroTubosCC]]->getNin() - 1;
				}
				FNumeroTubosCC++;
			}
			i++;
		}

		/* Ahora al tubo de mayor diametro se le asignara la posicion 1 de los vectores
		 y al de menor diametro la posicion 0 */
		if(Pipe[FTubo[0]]->GetDiametro(FNodoFin[0]) >= Pipe[FTubo[1]]->GetDiametro(FNodoFin[1])) {
			if(Pipe[FTubo[0]]->getNodoIzq() == FNumeroCC) {
				FTuboExtremo[1].Pipe = Pipe[FTubo[0]];
				FTuboExtremo[1].TipoExtremo = nmLeft;
				FNodoFin[1] = 0;
				FIndiceCC[1] = 0;
				FNumeroTubo[1] = FTubo[0];
				FCC[1] = &(FTuboExtremo[1].Beta);
				FCD[1] = &(FTuboExtremo[1].Landa);
			}
			if(Pipe[FTubo[0]]->getNodoDer() == FNumeroCC) {
				FTuboExtremo[1].Pipe = Pipe[FTubo[0]];
				FTuboExtremo[1].TipoExtremo = nmRight;
				FNodoFin[1] = Pipe[FTubo[0]]->getNin() - 1;
				FIndiceCC[1] = 1;
				FNumeroTubo[1] = FTubo[0];
				FCC[1] = &(FTuboExtremo[1].Landa);
				FCD[1] = &(FTuboExtremo[1].Beta);
			}
			if(Pipe[FTubo[1]]->getNodoIzq() == FNumeroCC) {
				FTuboExtremo[0].Pipe = Pipe[FTubo[1]];
				FTuboExtremo[0].TipoExtremo = nmLeft;
				FNodoFin[0] = 0;
				FIndiceCC[0] = 0;
				FNumeroTubo[0] = FTubo[1];
				FCC[0] = &(FTuboExtremo[0].Beta);
				FCD[0] = &(FTuboExtremo[0].Landa);
			}
			if(Pipe[FTubo[1]]->getNodoDer() == FNumeroCC) {
				FTuboExtremo[0].Pipe = Pipe[FTubo[1]];
				FTuboExtremo[0].TipoExtremo = nmRight;
				FNodoFin[0] = Pipe[FTubo[1]]->getNin() - 1;
				FIndiceCC[0] = 1;
				FNumeroTubo[0] = FTubo[1];
				FCC[0] = &(FTuboExtremo[0].Landa);
				FCD[0] = &(FTuboExtremo[0].Beta);
			}

		} else {
			if(Pipe[FTubo[1]]->getNodoIzq() == FNumeroCC) {
				FTuboExtremo[1].Pipe = Pipe[FTubo[1]];
				FTuboExtremo[1].TipoExtremo = nmLeft;
				FNodoFin[1] = 0;
				FIndiceCC[1] = 0;
				FNumeroTubo[1] = FTubo[1];
				FCC[1] = &(FTuboExtremo[1].Beta);
				FCD[1] = &(FTuboExtremo[1].Landa);
			}
			if(Pipe[FTubo[1]]->getNodoDer() == FNumeroCC) {
				FTuboExtremo[1].Pipe = Pipe[FTubo[1]];
				FTuboExtremo[1].TipoExtremo = nmRight;
				FNodoFin[1] = Pipe[FTubo[1]]->getNin() - 1;
				FIndiceCC[1] = 1;
				FNumeroTubo[1] = FTubo[1];
				FCC[1] = &(FTuboExtremo[1].Landa);
				FCD[1] = &(FTuboExtremo[1].Beta);
			}
			if(Pipe[FTubo[0]]->getNodoIzq() == FNumeroCC) {
				FTuboExtremo[0].Pipe = Pipe[FTubo[0]];
				FTuboExtremo[0].TipoExtremo = nmLeft;
				FNodoFin[0] = 0;
				FIndiceCC[0] = 0;
				FNumeroTubo[0] = FTubo[0];
				FCC[0] = &(FTuboExtremo[0].Beta);
				FCD[0] = &(FTuboExtremo[0].Landa);
			}
			if(Pipe[FTubo[0]]->getNodoDer() == FNumeroCC) {
				FTuboExtremo[0].Pipe = Pipe[FTubo[0]];
				FTuboExtremo[0].TipoExtremo = nmRight;
				FNodoFin[0] = Pipe[FTubo[0]]->getNin() - 1;
				FIndiceCC[0] = 1;
				FNumeroTubo[0] = FTubo[0];
				FCC[0] = &(FTuboExtremo[0].Landa);
				FCD[0] = &(FTuboExtremo[0].Beta);
			}
		}

		// Inicializacion del transporte de especies quimicas.
		FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
		for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
			// Se elige como composicion inicial la del tubo 0. Es arbitrario.
			FFraccionMasicaEspecie[i] = FTuboExtremo[0].Pipe->GetFraccionMasicaInicial(i);
		}

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%lf %lf", &FEspesor, &FConductividad);
		/* Coeficiente de perdidas con signo positivo */

		fgetpos(fich, &filepos);
		fclose(fich);

	} catch(exception & N) {
		std::cout << "ERROR: TCCUnionEntreTubos::LeeUnionEntreTubos en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCUnionEntreTubos::TuboCalculandose(int TuboActual) {
	try {
		FTuboActual = TuboActual;
	} catch(exception & N) {
		std::cout << "ERROR: TCCUnionEntreTubos::TuboCalculandose en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCUnionEntreTubos::CalculaCondicionContorno(double Time) {
	try {
		double rel_entropia = 0., rel_area = 0., vel_sonido_In = 0., vel_sonido_Out = 0., vel_In = 0., vel_Out = 0.,
			   correc_sonido_In = 0.;
		double flujo, FraccionMasicaAcum = 0., exd, exi;
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

		FGamma3 = __Gamma::G3(FGamma);
		FGamma2 = __Gamma::G2(FGamma);
		FGamma1 = __Gamma::G1(FGamma);

		/* Criterio para determinar el sentido el flujo */
		flujo = (*FCC[1] / FTuboExtremo[1].Entropia) / (*FCC[0] / FTuboExtremo[0].Entropia);
		if(flujo < 0.999995) {  /* Sentido del flujo: de 0(saliente (out)) a 1(entrante (in)) */
			rel_entropia = FTuboExtremo[0].Entropia / FTuboExtremo[1].Entropia;
			rel_area = pow2(FTuboExtremo[1].Pipe->GetDiametro(FNodoFin[1]) / FTuboExtremo[0].Pipe->GetDiametro(FNodoFin[0]));

			int cont = 0;
			/* Intervalo de acotacion de A1 */
			exd = *FCC[0];
			exi = *FCC[0] * 2. / FGamma2;

			/* LLAMADA A LA ESTRUCTURA-ensanchamiento */
			stExpansion EnsA1(*FCC[0], *FCC[1], rel_entropia, rel_area, FGamma);
			vel_sonido_Out = FindRoot(EnsA1, exi, exd);
			vel_In = EnsA1.U2;
			vel_Out = EnsA1.U1;
			vel_sonido_In = EnsA1.A2;
			correc_sonido_In = EnsA1.xx3;
			/* nuevo	if (abs(vel_sonido_Out-vel_Out)<1E-12) {
			 printf ("");
			 } */
			if(TuboCalculado == 1) {
				*FCD[1] = vel_sonido_In + FGamma3 * vel_In;
				*FCC[1] = vel_sonido_In - FGamma3 * vel_In;
				FTuboExtremo[1].Entropia = vel_sonido_In * FTuboExtremo[1].Entropia / correc_sonido_In;

			} else if(TuboCalculado == 0) {
				*FCD[0] = vel_sonido_Out - FGamma3 * vel_Out;

			} else if(TuboCalculado == 10000) {
				*FCD[1] = vel_sonido_In + FGamma3 * vel_In;
				*FCC[1] = vel_sonido_In - FGamma3 * vel_In;
				FTuboExtremo[1].Entropia = vel_sonido_In * FTuboExtremo[1].Entropia / correc_sonido_In;
				*FCD[0] = vel_sonido_Out - FGamma3 * vel_Out;
			}

			// Transporte de Especies Quimicas
			// Se actualiza todos los instantes de calculo.
			for(int j = 0; j < FNumeroEspecies - 2; j++) {
				FFraccionMasicaEspecie[j] = FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC[0], j);
				FraccionMasicaAcum += FFraccionMasicaEspecie[j];
			}
			FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
			if(FHayEGR)
				FFraccionMasicaEspecie[FNumeroEspecies - 1] = FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC[0],
						FNumeroEspecies - 1);

		} else if(flujo > 1.000005) {  /* Sentido del flujo: de 1(saliente) a 0(entrante) */

			rel_entropia = FTuboExtremo[0].Entropia / FTuboExtremo[1].Entropia;
			rel_area = pow2(FTuboExtremo[0].Pipe->GetDiametro(FNodoFin[0]) / FTuboExtremo[1].Pipe->GetDiametro(FNodoFin[1]));

			/* LLAMADA A LA ESTRUCTURA-estrechamiento */

			/* Intervalo de acotacion de U1 */
			exi = 0;
			exd = *FCC[1] * 2. / FGamma2;

			stContraction EstU1(*FCC[1], *FCC[0], rel_entropia, rel_area, FGamma);
			vel_Out = FindRoot(EstU1, exi, exd);
			vel_sonido_In = EstU1.A2;
			vel_sonido_Out = EstU1.A1;
			vel_In = EstU1.U2;

			if(TuboCalculado == 0) {
				*FCD[0] = vel_sonido_In + FGamma3 * vel_In;
				*FCC[0] = vel_sonido_In - FGamma3 * vel_In;
				FTuboExtremo[0].Entropia = FTuboExtremo[1].Entropia;

			} else if(TuboCalculado == 1) {
				*FCD[1] = vel_sonido_Out - FGamma3 * vel_Out;

			} else if(TuboCalculado == 10000) {
				*FCD[0] = vel_sonido_In + FGamma3 * vel_In;
				*FCC[0] = vel_sonido_In - FGamma3 * vel_In;
				FTuboExtremo[0].Entropia = FTuboExtremo[1].Entropia;
				*FCD[1] = vel_sonido_Out - FGamma3 * vel_Out;
			}

			// Transporte de Especies Quimicas
			// Se actualiza todos los instantes de calculo (al igual que la temperatura en la BC).
			for(int j = 0; j < FNumeroEspecies - 2; j++) {
				FFraccionMasicaEspecie[j] = FTuboExtremo[1].Pipe->GetFraccionMasicaCC(FIndiceCC[1], j);
				FraccionMasicaAcum += FFraccionMasicaEspecie[j];
			}
			FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
			if(FHayEGR)
				FFraccionMasicaEspecie[FNumeroEspecies - 1] = FTuboExtremo[1].Pipe->GetFraccionMasicaCC(FIndiceCC[1],
						FNumeroEspecies - 1);

		} else { /* Flujo Parado */

			if(TuboCalculado == 0) {
				*FCD[0] = *FCC[0];
			} else if(TuboCalculado == 1) {
				*FCD[1] = *FCC[1];
			} else if(TuboCalculado == 10000) {
				*FCD[0] = *FCC[0];
				*FCD[1] = *FCC[1];
			}
			// La composicion se mantiene, al estar el flujo parado.

		}
	} catch(exception & N) {
		std::cout << "ERROR: TCCUnionEntreTubos::CalculaCondicionContorno en la condicion de contorno: " << FNumeroCC <<
				  std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/* void TCCUnionEntreTubos::Estrechamiento(double CCS,double CCE,double rel_entropia,
 double rel_area,double smag,double *xa1,double *xa2,double *xu2,double *xu1)
 {
 try
 {
 double xx, xx1, xx2, xx3, exd, exi,ex1,ex2,xxx, xu1p, ytty, xxxx;



 }
 catch(Exception &N)
 {
 std::cout << "ERROR: TCCUnionEntreTubos::Estrechamiento en la condicion de contorno: " << FNumeroCC <<  std::endl;
 std::cout << "Tipo de error: " << N.what() << std::endl;
 throw Exception(N.what());
 }
 }

 //---------------------------------------------------------------------------
 //---------------------------------------------------------------------------

 void TCCUnionEntreTubos::Ensanchamiento(double CCS,double CCE,double rel_entropia,
 double rel_area,double )
 {
 try
 {


 }
 catch(Exception &N)
 {
 std::cout << "ERROR: TCCUnionEntreTubos::Ensanchamiento en la condicion de contorno: " << FNumeroCC <<  std::endl;
 std::cout << "Tipo de error: " << N.what() << std::endl;
 throw Exception(N.what());
 }
 }

 //void TCCUnionEntreTubos::CalculaCaracteristicas(double Time)
 //{
 //int signo = 0;
 //
 //	for(int i=0;i<FNumeroTubosCC;i++){
 //		signo=1.;
 //		if(FTuboExtremo[i].TipoExtremo==nmRight) signo=-1;
 //		signo=FTuboExtremo[i].Pipe->getNumeroTubo();
 //		//if(FTuboExtremo[i].Pipe->GetVelocidad(0)*signo<=0){
 //			//FTuboExtremo[i].Entropia=FTuboExtremo[i].Pipe
 //		//}
 //	}
 //}


 //---------------------------------------------------------------------------
 //---------------------------------------------------------------------------


 #pragma package(smart_init)
 */
