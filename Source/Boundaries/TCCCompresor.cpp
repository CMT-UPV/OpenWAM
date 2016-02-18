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

//#include <cmath>
#ifdef __BORLANDC__
#include <vcl.h>
#endif

#include "TCCCompresor.h"
#include "TCompTubDep.h"
#include "TCompTubos.h"
#include "TCompresorDep.h"
#include "TCCEntradaCompresor.h"

#include "TTubo.h"
#include "TDeposito.h"
#include "TCompresor.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TCCCompresor::TCCCompresor(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
						   nmCalculoGamma GammaCalculation, bool ThereIsEGR) :
	TCondicionContorno(TipoCC, numCC, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {

	FTuboExtremo = NULL;
	FTiempoActual = 0;
	FNumeroTubo = NULL;

}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCCCompresor::~TCCCompresor() {

	if(FNumeroTubo != NULL)
		delete[] FNumeroTubo;
	if(FTuboExtremo != NULL)
		delete[] FTuboExtremo;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCCompresor::LeeNumeroCompresor(const char *FileWAM, fpos_t &filepos) {
	try {

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &FNumeroCompresor);

		fgetpos(fich, &filepos);
		fclose(fich);

	} catch(exception & N) {
		std::cout << "ERROR: TCCCompresor::LeeCompresor en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCCompresor::AsignacionDatos(TCompresor **Compressor, TDeposito **Plenum, const char *FileWAM, fpos_t &filepos,
								   int NumberOfPipes, TTubo **Pipe, TCondicionContorno **BC, int numCC, double AmbientTemperature, double AmbientPressure,
								   double *AtmosphericComposition) {
	try {
		int i = 0;
		bool haytubo = false;
		int tipoentrada = 0;

		FTamb = AmbientTemperature;
		FPamb = AmbientPressure;
		FCompresor = Compressor[FNumeroCompresor - 1];
		FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];

		if(FCompresor->getModeloCompresor() == nmCompOriginal) {
			// Posee un tubo y un deposito. Hay que asignarselos a la BC.

			FTuboExtremo = new stTuboExtremo[1];
			FTuboExtremo[0].Pipe = NULL;

			while(FNumeroTubosCC < 1 && i < NumberOfPipes) {
				if(Pipe[i]->getNodoIzq() == FNumeroCC) {
					FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
					FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmLeft;
					FNumeroTubosCC++;
				}
				if(Pipe[i]->getNodoDer() == FNumeroCC) {
					FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
					FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmRight;
					FNumeroTubosCC++;
				}
				i++;
			}
			FILE *fich = fopen(FileWAM, "r");
			fsetpos(fich, &filepos);

			fscanf(fich, "%d ", &tipoentrada);

			fgetpos(fich, &filepos);
			fclose(fich);

			switch(tipoentrada) {
			case 0:
				FEntradaCompresor = nmAtmosphere;
				break;
			case 1:
				FEntradaCompresor = nmPipe;
				break;
			case 2:
				FEntradaCompresor = nmPlenum;
				break;
			}

			if(FEntradaCompresor == nmPlenum) {

				FILE *fich = fopen(FileWAM, "r");
				fsetpos(fich, &filepos);

				fscanf(fich, "%d ", &FNumeroDeposito);

				fgetpos(fich, &filepos);
				fclose(fich);

				FDeposito = Plenum[FNumeroDeposito - 1];
				dynamic_cast<TCompTubDep*>(FCompresor)->BusquedaEntradaSalida(FEntradaCompresor, FTamb, FNumeroCC, BC,
						AtmosphericComposition);

			} else if(FEntradaCompresor == nmPipe) {
				for(int i = 0; i < numCC; i++) {
					if(BC[i]->getTipoCC() == nmEntradaCompre) {
						if(dynamic_cast<TCCEntradaCompresor*>(BC[i])->getNumeroCompresor() == FNumeroCompresor) {
							haytubo = true;
							FTuboRotor = BC[i]->GetTuboExtremo(0).Pipe;
							FExtremoTuboRotor = BC[i]->GetTuboExtremo(0).TipoExtremo;
						}
					}
				}
				if(haytubo) {
					dynamic_cast<TCompTubDep*>(FCompresor)->BusquedaEntradaSalida(FEntradaCompresor, AmbientTemperature, FNumeroCC, BC,
							AtmosphericComposition);
				} else {
					printf("ERROR: El compresor %d no tiene una BC tipo EntradaCompresor a su entrada.\n ", FNumeroCompresor);
				}
			} else if(FEntradaCompresor == nmAtmosphere) {
				dynamic_cast<TCompTubDep*>(FCompresor)->BusquedaEntradaSalida(FEntradaCompresor, AmbientTemperature, FNumeroCC, BC,
						AtmosphericComposition);
			}

		} else if(FCompresor->getModeloCompresor() == nmCompPipes) {
			// Posee dos tubos. Hay que asignarselos a la BC.

			FTuboExtremo = new stTuboExtremo[2];
			FNumeroTubo = new int[2];
			for(int j = 0; j < 2; j++) {
				FTuboExtremo[j].Pipe = NULL;
			}

			while(FNumeroTubosCC < 2 && i < NumberOfPipes) {
				if(Pipe[i]->getNodoIzq() == FNumeroCC) {
					FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
					FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmLeft;
					FNumeroTubo[FNumeroTubosCC] = i;
					FNumeroTubosCC++;
				}
				if(Pipe[i]->getNodoDer() == FNumeroCC) {
					FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
					FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmRight;
					FNumeroTubo[FNumeroTubosCC] = i;
					FNumeroTubosCC++;
				}
				i++;
			}
			dynamic_cast<TCompTubos*>(FCompresor)->RelacionTubos(BC, FNumeroCC);

		} else if(FCompresor->getModeloCompresor() == nmCompPlenums) {
			// Posee dos depositos. Hay que asignarselos a la BC.

			FILE *fich = fopen(FileWAM, "r");
			fsetpos(fich, &filepos);

			fscanf(fich, "%d ", &FNumeroDepositoRot);
			fscanf(fich, "%d ", &FNumeroDepositoEst);

			fgetpos(fich, &filepos);
			fclose(fich);

			FDepositoRot = Plenum[FNumeroDepositoRot - 1];
			FDepositoEst = Plenum[FNumeroDepositoEst - 1];

			dynamic_cast<TCompresorDep*>(FCompresor)->RelacionDepositoCompresor(FDepositoRot, FDepositoEst);

		}

	} catch(exception & N) {
		std::cout << "ERROR: TCCCompresor::AsignaCompresor en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCCompresor::ObtencionValoresInstantaneos(double Theta, double tiempoactual) {
	try {

		FTheta = Theta;
		FTiempoActual = tiempoactual;

	} catch(exception & N) {
		std::cout << "ERROR: TCCCompresor::ObtencionValoresInstantaneos en la condicion de contorno: " << FNumeroCC <<
				  std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCCompresor::CalculaCondicionContorno(double Time) {
	try {
		double TrabajoInsTurbina = 0.; /* para que compile.inacabado. */
		int TuboCalculado = 0;

		if(FCompresor->getModeloCompresor() == nmCompOriginal) {
			TuboCalculado = 0; // Es la posicion del tubo a calcular en el vector estructura FTuboExtremo.
			// Para este modelo de compresor no hace falta,pero si en el de 2 tubos. Como comparten
			// funcion virtual, la llamada de la misma ha de ser igual. Por eso lo anado.
			dynamic_cast<TCompTubDep*>(FCompresor)->DatosEntradaCompresor(FTamb, FPamb, this);
			FCompresor->CondicionCompresor(FTheta, FTuboExtremo, FTiempoActual, TuboCalculado);
			FCompresor->AcumulaMedias(FTiempoActual);
		}
		if(FCompresor->getModeloCompresor() == nmCompPlenums) {
			FCompresor->CalculaGasto(TrabajoInsTurbina, FTiempoActual); /* Falta Acabar!!!!! */
			FCompresor->AcumulaMedias(FTiempoActual);
		}
		if(FCompresor->getModeloCompresor() == nmCompPipes) {
			// Identificacion del tubo que se esta calculando en el instante current. Solo han de variar sus propiedades.
			if(FTuboActual == 10000) {
				TuboCalculado = FTuboActual;
			} else {
				for(int i = 0; i < FNumeroTubosCC; i++) {
					if(FNumeroTubo[i] == FTuboActual) {
						TuboCalculado = i; // Es la posicion del tubo calculado en el vector estructura FTuboExtremo.
					}
				}
			}
			FCompresor->CondicionCompresor(FTheta, FTuboExtremo, FTiempoActual, TuboCalculado);
			FCompresor->AcumulaMedias(FTiempoActual);
		}

		// Transporte de especies quimicas.  (Paso de informacion del objeto Compressor al objeto BC Compressor
		for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
			FFraccionMasicaEspecie[i] = FCompresor->GetFraccionMasicaEspecie(i);
		}

	} catch(exception & N) {
		std::cout << "ERROR: TCCCompresor::CalculaCondicionesContorno en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCCompresor::TuboCalculandose(int TuboActual) {
	try {
		FTuboActual = TuboActual;
	} catch(exception & N) {
		std::cout << "ERROR: TCCUnionEntreTubos::TuboCalculandose en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TCCCompresor::ReadCompressorData(const char *FileWAM, fpos_t &filepos, TCompresor **Compressor) {

	int tipoentrada = 0;

	if(Compressor[FNumeroCompresor - 1]->getModeloCompresor() == nmCompOriginal) {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &tipoentrada);

		switch(tipoentrada) {
		case 0:
			FEntradaCompresor = nmAtmosphere;
			break;
		case 1:
			FEntradaCompresor = nmPipe;
			break;
		case 2:
			FEntradaCompresor = nmPlenum;
			fscanf(fich, "%d ", &FNumeroDeposito);
			break;
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} else if(Compressor[FNumeroCompresor - 1]->getModeloCompresor() == nmCompPlenums) {
		// Posee dos depositos. Hay que asignarselos a la BC.

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &FNumeroDepositoRot);
		fscanf(fich, "%d ", &FNumeroDepositoEst);

		fgetpos(fich, &filepos);
		fclose(fich);
	}
}

void TCCCompresor::AsignData(TDeposito **Plenum, int NumberOfPipes, TTubo **Pipe, TCondicionContorno **BC, int numCC,
							 double *AtmosphericComposition, TCompresor **Compressor, double AmbientTemperature, double AmbientPressure) {
	try {
		int i = 0;
		bool haytubo = false;

		FTamb = AmbientTemperature;
		FPamb = AmbientPressure;
		FCompresor = Compressor[FNumeroCompresor - 1];
		FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];

		if(FCompresor->getModeloCompresor() == nmCompOriginal) {
			// Posee un tubo y un deposito. Hay que asignarselos a la BC.

			FTuboExtremo = new stTuboExtremo[1];
			FTuboExtremo[0].Pipe = NULL;

			while(FNumeroTubosCC < 1 && i < NumberOfPipes) {
				if(Pipe[i]->getNodoIzq() == FNumeroCC) {
					FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
					FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmLeft;
					FNumeroTubosCC++;
				}
				if(Pipe[i]->getNodoDer() == FNumeroCC) {
					FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
					FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmRight;
					FNumeroTubosCC++;
				}
				i++;
			}

			if(FEntradaCompresor == nmPlenum) {

				FDeposito = Plenum[FNumeroDeposito - 1];
				dynamic_cast<TCompTubDep*>(FCompresor)->BusquedaEntradaSalida(FEntradaCompresor, FTamb, FNumeroCC, BC,
						AtmosphericComposition);

			} else if(FEntradaCompresor == nmPipe) {
				for(int i = 0; i < numCC; i++) {
					if(BC[i]->getTipoCC() == nmEntradaCompre) {
						if(dynamic_cast<TCCEntradaCompresor*>(BC[i])->getNumeroCompresor() == FNumeroCompresor) {
							haytubo = true;
							FTuboRotor = BC[i]->GetTuboExtremo(0).Pipe;
							FExtremoTuboRotor = BC[i]->GetTuboExtremo(0).TipoExtremo;
						}
					}
				}
				if(haytubo) {
					dynamic_cast<TCompTubDep*>(FCompresor)->BusquedaEntradaSalida(FEntradaCompresor, FTamb, FNumeroCC, BC,
							AtmosphericComposition);
				} else {
					printf("ERROR: El compresor %d no tiene una BC tipo EntradaCompresor a su entrada.\n ", FNumeroCompresor);
				}
			} else if(FEntradaCompresor == nmAtmosphere) {
				dynamic_cast<TCompTubDep*>(FCompresor)->BusquedaEntradaSalida(FEntradaCompresor, FTamb, FNumeroCC, BC,
						AtmosphericComposition);
			}

		} else if(FCompresor->getModeloCompresor() == nmCompPipes) {
			// Posee dos tubos. Hay que asignarselos a la BC.

			FTuboExtremo = new stTuboExtremo[2];
			FNumeroTubo = new int[2];
			for(int j = 0; j < 2; j++) {
				FTuboExtremo[j].Pipe = NULL;
			}

			while(FNumeroTubosCC < 2 && i < NumberOfPipes) {
				if(Pipe[i]->getNodoIzq() == FNumeroCC) {
					FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
					FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmLeft;
					FNumeroTubo[FNumeroTubosCC] = i;
					FNumeroTubosCC++;
				}
				if(Pipe[i]->getNodoDer() == FNumeroCC) {
					FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
					FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmRight;
					FNumeroTubo[FNumeroTubosCC] = i;
					FNumeroTubosCC++;
				}
				i++;
			}
			// dynamic_cast<TCompTubos*>(FCompresor)->RelacionTubos(BC, FNumeroCC);
			dynamic_cast<TCompTubos*>(FCompresor)->AsignPipes(BC, FNumeroCC);

		} else if(FCompresor->getModeloCompresor() == nmCompPlenums) {

			FDepositoRot = Plenum[FNumeroDepositoRot - 1];
			FDepositoEst = Plenum[FNumeroDepositoEst - 1];

			dynamic_cast<TCompresorDep*>(FCompresor)->RelacionDepositoCompresor(FDepositoRot, FDepositoEst);

		}

	} catch(exception & N) {
		std::cout << "ERROR: TCCCompresor::AsignaCompresor en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
#pragma package(smart_init)
