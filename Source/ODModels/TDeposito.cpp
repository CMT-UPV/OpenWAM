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
#include "TDeposito.h"

#include "TCCDeposito.h"
#include "TCCUnionEntreDepositos.h"
#include "TCCCompresor.h"
#include "TCompresor.h"
#include "TTubo.h"
#include "TDPF.h"
#include "TCanalDPF.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TDeposito::TDeposito(int i, nmTipoDeposito TipoDeposito, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
					 nmCalculoGamma GammaCalculation, bool ThereIsEGR) {

	FCalculoEspecies = SpeciesModel;
	FNumeroEspecies = numeroespecies;
	FCalculoGamma = GammaCalculation;
	FFraccionMasicaEspecie = NULL;
	FMasaEspecie = NULL;

	FHayEGR = ThereIsEGR;
	if(FHayEGR)
		FIntEGR = 0;
	else
		FIntEGR = 1;

	FPresRef = 1.;
	FUnion = NULL;
	FNumConductos = NULL;
	FNumNodos = NULL;
	FSentidoFlujo = NULL;
	asgFUnion = false;
	asgFNumUniones = false;
	asgFNumConductos = false;
	asgFSentidoFlujo = false;
	FNumeroDeposito = i + 1;
	FTipoDeposito = TipoDeposito;
	FUpdated = false;
	FNUnionesED = 0;
	FNumeroUnionesED = 0;
	FNumeroUniones = 0;
	FHayCompresor = false;
	FTime = 0.;
	FEstudioEstabilidadRealizado = false;

	FCCDeposito = NULL;
	FCCUnionEntreDep = NULL;

	FResMediosDep.Pressure = false;
	FResMediosDep.PresionSUM = 0.;
	FResMediosDep.PresionMED = 1.;
	FResMediosDep.Temperature = false;
	FResMediosDep.TemperaturaSUM = 0.;
	FResMediosDep.TemperaturaMED = 0.;
	FResMediosDep.TiempoSUM = 0.;
	FResMediosDep.Tiempo0 = 0.;
	FResMediosDep.FraccionMasicaEspecies = false;
	FResMediosDep.FraccionMED = new double[FNumeroEspecies - FIntEGR];
	FResMediosDep.FraccionSUM = new double[FNumeroEspecies - FIntEGR];

	FResInstantDep.Pressure = false;
	FResInstantDep.Temperature = false;
	FResInstantDep.Volumen = false;
	FResInstantDep.Masa = false;
	FResInstantDep.PresionINS = 1.;
	FResInstantDep.TemperaturaINS = 0.;
	FResInstantDep.VolumenINS = 1.;
	FResInstantDep.MasaINS = 1.;
	FResInstantDep.FraccionMasicaEspecies = false;
	FResInstantDep.FraccionINS = new double[FNumeroEspecies - FIntEGR];
	FResInstantDep.Gamma = false;
	FResInstantDep.GammaINS = 1.4;

	for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
		FResInstantDep.FraccionINS[i] = 0.;
		FResMediosDep.FraccionSUM[i] = 0.;
		FResMediosDep.FraccionMED[i] = 0.;
	}

	FHeatPower = 0;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TDeposito::~TDeposito() {
	if(FUnion != NULL)
		delete[] FUnion;
	if(FSentidoFlujo != NULL)
		delete[] FSentidoFlujo;
	if(FNumConductos != NULL)
		delete[] FNumConductos;
	if(FNumNodos != NULL)
		delete[] FNumNodos;

	if(FFraccionMasicaEspecie != NULL)
		delete[] FFraccionMasicaEspecie;
	if(FMasaEspecie != NULL)
		delete[] FMasaEspecie;

	if(FCCDeposito != NULL)
		delete[] FCCDeposito;
	if(FCCUnionEntreDep != NULL)
		delete[] FCCUnionEntreDep;

	if(FResMediosDep.FraccionMED != NULL)
		delete[] FResMediosDep.FraccionMED;

	if(FResMediosDep.FraccionSUM != NULL)
		delete[] FResMediosDep.FraccionSUM;

	if(FResInstantDep.FraccionINS != NULL)
		delete[] FResInstantDep.FraccionINS;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::LeeDatosGeneralesDepositos(const char *FileWAM, fpos_t &filepos) {
	try {
		double fracciontotal = 0.;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
		FMasaEspecie = new double[FNumeroEspecies - FIntEGR];
		for(int i = 0; i < FNumeroEspecies - 1; i++) {
			fscanf(fich, "%lf ", &FFraccionMasicaEspecie[i]);
			fracciontotal += FFraccionMasicaEspecie[i];
		}
		if(FHayEGR) {
			if(FCalculoEspecies == nmCalculoCompleto) {
				if(FFraccionMasicaEspecie[0] > 0.2)
					FFraccionMasicaEspecie[FNumeroEspecies - 1] = 0.;
				else
					FFraccionMasicaEspecie[FNumeroEspecies - 1] = 1.;
			} else {
				if(FFraccionMasicaEspecie[0] > 0.5)
					FFraccionMasicaEspecie[FNumeroEspecies - 1] = 1.;
				else
					FFraccionMasicaEspecie[FNumeroEspecies - 1] = 0.;
			}
		}

		if(fracciontotal > 1 + 1.e-10 && fracciontotal < 1 - 1e-10) {
			std::cout << "ERROR: Total mass fraction cannot be different than 1. Check input data in plenum  " << FNumeroDeposito <<
					  std::endl;
			throw Exception(" ");
		}

		if(FTipoDeposito != nmDepVolVble) {
			fscanf(fich, "%lf %lf %lf ", &FVolumen, &FPressure, &FTemperature);
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
			FPresionIsen = pow(FPressure / FPresRef, __Gamma::G5(FGamma));
			FAsonido = sqrt(FGamma * FRMezcla * __units::degCToK(FTemperature)) / __cons::ARef;
			FMasa = FVolumen * FGamma * __units::BarToPa(FPressure) / pow2(FAsonido * __cons::ARef);
			for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
				FMasaEspecie[j] = FMasa * FFraccionMasicaEspecie[j];
			}
		} else {
			fscanf(fich, " %lf %lf ", &FPressure, &FTemperature);
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
			FPresionIsen = pow(FPressure / FPresRef, __Gamma::G5(FGamma));
			FAsonido = sqrt(FGamma * FRMezcla * __units::degCToK(FTemperature)) / __cons::ARef;
			for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
				FMasaEspecie[j] = FMasa * FFraccionMasicaEspecie[j];
			}
		}

		fgetpos(fich, &filepos);
		fclose(fich);

	} catch(exception & N) {
		std::cout << "ERROR: TDeposito::LeeDatosGeneralesDepositos en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::AsignacionCC(TCondicionContorno **BC, int numCC) {
	try {

		bool UnionAsignada;
		bool UnionEDAsignada;
		/* Union Deposito-Pipe */
		for(int i = 0; i < numCC; i++) {
			if(BC[i]->getTipoCC() == nmPipeToPlenumConnection) {
				if(FNumeroDeposito == dynamic_cast<TCCDeposito*>(BC[i])->getNumeroDeposito()) {
					FNumeroUniones++;
				}
			} else if(BC[i]->getTipoCC() == nmCompresor) {
				if((dynamic_cast<TCCCompresor*>(BC[i])->getCompressor())->getModeloCompresor() == nmCompOriginal) {
					if(FNumeroDeposito == dynamic_cast<TCCCompresor*>(BC[i])->getNumeroDeposito()) {
						FNumeroUniones++;
					}
				}
			}
		}

		/* Comprobacion que los depositos especiales tienen el numero de entradas que les corresponden. */
		if(FTipoDeposito == nmTurbinaSimple) {
			if(FNumeroUniones != 2) {
				std::cout << "ERROR: A single turbine must have 2 connections. Plenum: " << FNumeroDeposito << std::endl;
				std::cout << "Total number of connections: " << FNumeroUniones << std::endl;
				throw Exception("");
			}
		} else if(FTipoDeposito == nmTurbinaTwin) {
			if(FNumeroUniones != 3) {
				std::cout << "ERROR: A twin turbine must have 3 connections. Plenum: " << FNumeroDeposito << std::endl;
				std::cout << "Total number of connections: " << FNumeroUniones << std::endl;
				throw Exception("");
			}
		} else if(FTipoDeposito == nmVenturi) {
			if(FNumeroUniones != 3) {
				std::cout << "ERROR: A venturi must have 3 connections. Plenum: " << FNumeroDeposito << std::endl;
				std::cout << "Total number of connections: " << FNumeroUniones << std::endl;
				throw Exception("");
			}
		} else if(FTipoDeposito == nmUnionDireccional) {
			if(FNumeroUniones != 3) {
				std::cout << "ERROR: A directional junction must have 3 connections. Planum: " << FNumeroDeposito << std::endl;
				std::cout << "Total number of connections: " << FNumeroUniones << std::endl;
				throw Exception("");
			}
		}

		if(FNumeroUniones != 0)
			FCCDeposito = new TCondicionContorno*[FNumeroUniones];

		bool *CCasignadaD;
		CCasignadaD = new bool[numCC];
		for(int i = 0; i < numCC; i++)
			CCasignadaD[i] = false;

		for(int j = 0; j < FNumeroUniones; j++) {
			UnionAsignada = false;
			for(int i = 0; i < numCC; i++) {
				if(BC[i]->getTipoCC() == nmPipeToPlenumConnection) {
					if(FNumeroDeposito == dynamic_cast<TCCDeposito*>(BC[i])->getNumeroDeposito() && !CCasignadaD[i] && !UnionAsignada) {
						FCCDeposito[j] = BC[i];
						UnionAsignada = true;
						CCasignadaD[i] = true;
					}
				}
				if(BC[i]->getTipoCC() == nmCompresor) {
					if(dynamic_cast<TCCCompresor*>(BC[i])->getCompressor()->getModeloCompresor() == nmCompOriginal) {
						if(FNumeroDeposito == dynamic_cast<TCCCompresor*>(BC[i])->getNumeroDeposito() && !CCasignadaD[i] && !UnionAsignada) {
							FCCDeposito[j] = BC[i];
							UnionAsignada = true;
							CCasignadaD[i] = true;
						}
					}
				}
			}
		}

		delete[] CCasignadaD;

		/* Union Deposito-Deposito */

		for(int i = 0; i < numCC; i++) {
			if(BC[i]->getTipoCC() == nmUnionEntreDepositos) {
				if(FNumeroDeposito == dynamic_cast<TCCUnionEntreDepositos*>(BC[i])->getNumeroDeposito1()
				   || FNumeroDeposito == dynamic_cast<TCCUnionEntreDepositos*>(BC[i])->getNumeroDeposito2()) {
					FNumeroUnionesED++;
				}
			}
			if(BC[i]->getTipoCC() == nmCompresor) {
				if(dynamic_cast<TCCCompresor*>(BC[i])->getCompressor()->getModeloCompresor() == nmCompPlenums) {
					if(FNumeroDeposito == dynamic_cast<TCCCompresor*>(BC[i])->getNumeroDepositoRot()
					   || FNumeroDeposito == dynamic_cast<TCCCompresor*>(BC[i])->getNumeroDepositoEst()) {
						FNumeroUnionesED++;
					}
				}
			}
		}

		if((FTipoDeposito == nmTurbinaSimple || FTipoDeposito == nmTurbinaTwin) & FNumeroUnionesED != 0) {
			std::cout << "ERROR: Una turbina no puede estar unida directamente a un deposito. Deposito:" << std::endl;
			std::cout << "       Revisa la entrada de datos el apartado de uniones entre depositos" << std::endl;
			throw Exception("");
		} else if(FTipoDeposito == nmVenturi & FNumeroUnionesED != 0) {
			std::cout << "ERROR: Un venturi no puede estar unido directamente a un deposito. Deposito:" << std::endl;
			std::cout << "       Revisa la entrada de datos el apartado de uniones entre depositos" << std::endl;
			throw Exception("");
		} else if(FTipoDeposito == nmUnionDireccional & FNumeroUnionesED != 0) {
			std::cout << "ERROR: Una union direccional no puede estar unido directamente a un deposito" << std::endl;
			std::cout << "       Revisa la entrada de datos el apartado de uniones entre depositos" << std::endl;
			throw Exception("");
		}

		if(FNumeroUnionesED != 0)
			FCCUnionEntreDep = new TCondicionContorno*[FNumeroUnionesED];

		bool *CCasignadaUD;
		CCasignadaUD = new bool[numCC];
		for(int i = 0; i < numCC; i++)
			CCasignadaUD[i] = false;

		for(int j = 0; j < FNumeroUnionesED; j++) {
			UnionEDAsignada = false;
			for(int i = 0; i < numCC; i++) {
				if(BC[i]->getTipoCC() == nmUnionEntreDepositos) {
					if((FNumeroDeposito == dynamic_cast<TCCUnionEntreDepositos*>(BC[i])->getNumeroDeposito1()
						|| FNumeroDeposito == dynamic_cast<TCCUnionEntreDepositos*>(BC[i])->getNumeroDeposito2()) && !CCasignadaUD[i]
					   && !UnionEDAsignada) {
						FCCUnionEntreDep[j] = BC[i];
						CCasignadaUD[i] = true;
						UnionEDAsignada = true;
					}
				}
				if(BC[i]->getTipoCC() == nmCompresor) {
					if(dynamic_cast<TCCCompresor*>(BC[i])->getCompressor()->getModeloCompresor() == nmCompPlenums) {
						if((FNumeroDeposito == dynamic_cast<TCCCompresor*>(BC[i])->getNumeroDepositoRot()
							|| FNumeroDeposito == dynamic_cast<TCCCompresor*>(BC[i])->getNumeroDepositoEst()) && !CCasignadaUD[i]
						   && !UnionEDAsignada) {
							FCCUnionEntreDep[j] = BC[i];
							CCasignadaUD[i] = true;
							UnionEDAsignada = true;
						}
					}
				}
			}
		}

		delete[] CCasignadaUD;

	} catch(exception & N) {
		std::cout << "ERROR: TDeposito::AsignacionCC en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TDeposito::EntalpiaEntrada(double ASonidoE, double VelocidadE, double MasaE, double ASonidoD, double MasaD,
								  double Gamma) {
	try {

		double xx = 0., yy = 0., ret_val = 0.;

		if(fabs(MasaE) != 0.) {
			xx = (ASonidoE * ASonidoE / ASonidoD / ASonidoD - 1.) / __Gamma::G1(Gamma);
			yy = VelocidadE * VelocidadE / ASonidoD / ASonidoD / 2.;
			ret_val = Gamma * MasaE * (xx + yy) / MasaD;
		} else {
			ret_val = 0.;
		}
		return ret_val;
	} catch(exception & N) {
		std::cout << "ERROR: TDeposito:EntalpiaEntrada en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::AsignaCompresor(TCompresor *Compressor, int sentido) {
	try {
		if(FHayCompresor) {
			std::cout << "ERROR: Each plenum only can have one compressor connected" << std::endl;
			std::cout << "       Check the input data" << std::endl;
			throw Exception("");
		} else {
			if(FTipoDeposito != nmDepVolCte) {
				std::cout << "ERROR: Compressor only can be connected to constant volume plenums" << std::endl;
				std::cout << "       Check the input data" << std::endl;
				throw Exception("");
			} else {
				FHayCompresor = true;
				FCompresor = Compressor;
				FCompresorSentido = sentido;
			}
		}
	} catch(exception & N) {
		std::cout << "ERROR: TDeposito:AsignaCompresor en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::CreaUnionED(int numero, int sentido) {
	try {
		if(FTipoDeposito == nmTurbinaSimple || FTipoDeposito == nmTurbinaTwin) {
			std::cout << "ERROR: Turbines can not be connected to plenums directly" << std::endl;
			std::cout << "       Check the input data for connections between plenums" << std::endl;
			throw Exception("");
		} else if(FTipoDeposito == nmVenturi) {
			std::cout << "ERROR: Venturis can not be connected to plenums directly" << std::endl;
			std::cout << "       Check the input data for connections between plenums" << std::endl;
			throw Exception("");
		} else if(FTipoDeposito == nmUnionDireccional) {
			std::cout << "ERROR: Directional junctions can not be connected to plenums directly" << std::endl;
			std::cout << "       Check the input data for connections between plenums" << std::endl;
			throw Exception("");
		} else {
			FNUnionesED++;
			FUnionED.push_back(numero);
			FSentidoUED.push_back(sentido);
		}
	} catch(exception & N) {
		std::cout << "ERROR: TDeposito:CreaUnionED en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::PutNUniones(int value) {
	try {
		if(!asgFNumUniones) {
			FNumeroUniones = value;
			asgFNumUniones = true;
			if(FUnion == NULL) {
				FUnion = new int[FNumeroUniones];
				for(int i = 0; i < FNumeroUniones; i++) {
					FUnion[i] = -1;
				}
			} else {
				std::cout << "ERROR: The connections vector is already allocated" << std::endl;
				throw Exception("");
			}
			if(FNumConductos == NULL) {
				FNumConductos = new int[FNumeroUniones];
				for(int i = 0; i < FNumeroUniones; i++) {
					FNumConductos[i] = -1;
				}
			} else {
				std::cout << "ERROR: The pipes vector is already allocated" << std::endl;
				throw Exception("");
			}
			if(FNumNodos == NULL) {
				FNumNodos = new int[FNumeroUniones];
				for(int i = 0; i < FNumeroUniones; i++) {
					FNumNodos[i] = -1;
				}
			} else {
				std::cout << "ERROR: The nodes number vector is already allocated" << std::endl;
				throw Exception("");
			}
			if(FSentidoFlujo == NULL) {
				FSentidoFlujo = new int[FNumeroUniones];
				for(int i = 0; i < FNumeroUniones; i++) {
					FSentidoFlujo[i] = 0;
				}
			} else {
				std::cout << "ERROR: The flow sense vector is already allocated" << std::endl;
				throw Exception("");
			}
		} else {
			std::cout << "ERROR: All connections of plenum " << FNumeroDeposito << " are already asigned" << std::endl;
			throw Exception("");
		}
	} catch(exception & N) {
		std::cout << "ERROR: TDeposito:PutNUniones en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::PutUnion(int indice, int valor) {
	try {
		if(!asgFUnion) {
			if(indice < FNumeroUniones) {
				FUnion[indice] = valor;
			}
			asgFUnion = true;
			for(int i = 0; i < FNumeroUniones; i++) {
				if(FUnion[i] == -1) {
					asgFUnion = false;
					return;
				}
			}
		} else {
			std::cout << "ERROR: Se intentan definir mas nodos de los que se han declarado" << std::endl;
			throw Exception("");
		}
	} catch(exception & N) {
		std::cout << "ERROR: PutUnion en el depositito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::PutSentidoFlujo(int indice, int valor) {
	try {
		if(!asgFSentidoFlujo) {
			if(indice < FNumeroUniones) {
				FSentidoFlujo[indice] = valor;
			}
			asgFSentidoFlujo = true;
			for(int i = 0; i < FNumeroUniones; i++) {
				if(FSentidoFlujo[i] == 0) {
					asgFSentidoFlujo = false;
					return;
				}
			}
		} else {
			std::cout << "ERROR: Se intentan definir mas nodos de los que se han declarado" << std::endl;
			throw Exception("");
		}
	} catch(exception & N) {
		std::cout << "ERROR: PutSentidoFlujo en el depositito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::PutNumConductos(int indice, int valor) {
	try {
		if(!asgFNumConductos) {
			if(indice < FNumeroUniones) {
				FNumConductos[indice] = valor;
			} else {
				std::cout << "ERROR: Se intentan definir mas nodos de los que se han declarado" << std::endl;
				throw Exception("");
			}
			asgFNumConductos = true;
			for(int i = 0; i < FNumeroUniones; i++) {
				if(FNumConductos[indice] == -1) {
					asgFNumConductos = false;
					return;
				}
			}
		} else {
			std::cout << "ERROR: All pipes are already asigned" << std::endl;
			throw Exception("");
		}
	} catch(exception & N) {
		std::cout << "ERROR: PutNumConductos en el depositito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::PutNumNodos(int indice, int valor) {
	try {
		if(!asgFNumNodos) {
			if(indice < FNumeroUniones) {
				FNumNodos[indice] = valor;
			} else {
				std::cout << "ERROR: The number of connection to the plenum is lower" << std::endl;
				throw Exception("");
			}
			asgFNumNodos = true;
			for(int i = 0; i < FNumeroUniones; i++) {
				if(FNumNodos[indice] == -1) {
					asgFNumNodos = false;
					return;
				}
			}
		} else {
			std::cout << "ERROR: All connections are already asigned" << std::endl;
			throw Exception("");
		}
	} catch(exception & N) {
		std::cout << "ERROR: PutNumConductos en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::ResultadosInstantaneosDep() {
	try {
		if(FResInstantDep.Pressure)
			FResInstantDep.PresionINS = FPressure;
		if(FResInstantDep.Temperature)
			FResInstantDep.TemperaturaINS = __units::KTodegC(pow2(FAsonido * __cons::ARef) / (FGamma * FRMezcla));
		if(FResInstantDep.Volumen)
			FResInstantDep.VolumenINS = FVolumen;
		if(FResInstantDep.Masa)
			FResInstantDep.MasaINS = FMasa;
		if(FResInstantDep.FraccionMasicaEspecies) {
			for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
				FResInstantDep.FraccionINS[i] = FFraccionMasicaEspecie[i];
			}
		}
		if(FResInstantDep.Gamma)
			FResInstantDep.GammaINS = FGamma;

	} catch(exception & N) {
		std::cout << "ERROR: TDeposito::ResultadosInstantaneosDep en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::AcumulaResultadosMedios(double Actual) {
	try {
		double Delta = Actual - FResMediosDep.Tiempo0;
		if(FResMediosDep.Pressure) {
			FResMediosDep.PresionSUM += FPressure * Delta;
		}
		if(FResMediosDep.Temperature) {
			FResMediosDep.TemperaturaSUM += __units::KTodegC(pow2(FAsonido * __cons::ARef) / (FGamma * FRMezcla)) * Delta;
		}
		if(FResMediosDep.FraccionMasicaEspecies) {
			for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
				FResMediosDep.FraccionSUM[i] += FFraccionMasicaEspecie[i] * Delta;
			}
		}

		FResMediosDep.TiempoSUM += Delta;
		FResMediosDep.Tiempo0 = Actual;
	} catch(exception & N) {
		std::cout << "ERROR: TDeposito::AcumulaResultadosMedios en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::ResultadosMediosDep() {
	try {
		if(FNumResMed > 0) {
			if(FResMediosDep.Pressure) {
				FResMediosDep.PresionMED = FResMediosDep.PresionSUM / FResMediosDep.TiempoSUM;
				FResMediosDep.PresionSUM = 0.;
			}
			if(FResMediosDep.Temperature) {
				FResMediosDep.TemperaturaMED = FResMediosDep.TemperaturaSUM / FResMediosDep.TiempoSUM;
				FResMediosDep.TemperaturaSUM = 0.;
			}
			if(FResMediosDep.FraccionMasicaEspecies) {
				for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
					FResMediosDep.FraccionMED[i] = FResMediosDep.FraccionSUM[i] / FResMediosDep.TiempoSUM;
					FResMediosDep.FraccionSUM[i] = 0.;
				}
			}
			FResMediosDep.TiempoSUM = 0;
		}
	} catch(exception & N) {
		std::cout << "ERROR: TDeposito::ResultadosMediosDep en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::ReadInstantaneousResultsDep(const char *FileWAM, fpos_t &filepos) {
	int nvars = 0, var = 0;
	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &nvars);
		for(int i = 0; i < nvars; i++) {
			fscanf(fich, "%d ", &var);
			switch(var) {
			case 0:
				FResInstantDep.Pressure = true;
				break;
			case 1:
				FResInstantDep.Temperature = true;
				break;
			case 2:
				FResInstantDep.Volumen = true;
				break;
			case 3:
				FResInstantDep.Masa = true;
				break;
			case 4:
				FResInstantDep.FraccionMasicaEspecies = true;
				break;
			case 5:
				FResInstantDep.Gamma = true;
				break;
			default:
				std::cout << "Instantaneous results in plenum " << FNumeroDeposito << " does not exist " << std::endl;
			}
		}

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TDeposito::ReadInstantaneousResultsDep en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::ReadAverageResultsDep(const char *FileWAM, fpos_t &filepos) {
	int nvars = 0, var = 0;
	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &FNumResMed);
		for(int i = 0; i < FNumResMed; i++) {
			fscanf(fich, "%d ", &var);
			switch(var) {
			case 0:
				FResMediosDep.Pressure = true;
				break;
			case 1:
				FResMediosDep.Temperature = true;
				break;
			case 2:
				FResMediosDep.FraccionMasicaEspecies = true;
				break;
			default:
				std::cout << "Average result in plenum " << FNumeroDeposito << " does not exist " << std::endl;
			}
		}

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TDeposito::ReadAverageResultsDep en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::HeaderInstantaneousResultsDep(stringstream& insoutput, stEspecies *DatosEspecies) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");
		std::string Label;

		if(FResInstantDep.Pressure) {
			Label = "\t" + PutLabel(501) + std::to_string(FNumeroDeposito) + PutLabel(908);
			insoutput << Label.c_str();
		}
		if(FResInstantDep.Temperature) {
			Label = "\t" + PutLabel(502) + std::to_string(FNumeroDeposito) + PutLabel(910);
			insoutput << Label.c_str();
		}
		if(FResInstantDep.Volumen) {
			Label = "\t" + PutLabel(503) + std::to_string(FNumeroDeposito) + PutLabel(912);
			insoutput << Label.c_str();
		}
		if(FResInstantDep.Masa) {
			Label = "\t" + PutLabel(504) + std::to_string(FNumeroDeposito) + PutLabel(913);
			insoutput << Label.c_str();
		}
		if(FResInstantDep.FraccionMasicaEspecies) {
			for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
				Label = "\t" + PutLabel(505) + DatosEspecies[i].Nombre + PutLabel(506) + std::to_string(FNumeroDeposito) + PutLabel(
							901);
				insoutput << Label.c_str();
			}
		}
		if(FResInstantDep.Gamma) {
			Label = "\t" + PutLabel(507) + std::to_string(FNumeroDeposito) + PutLabel(901);
			insoutput << Label.c_str();
		}

		// fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TDeposito::CabeceraResultadosInstaneosDep en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::ImprimeResultadosInstantaneosDep(stringstream& insoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");

		if(FResInstantDep.Pressure)
			insoutput << "\t" << FResInstantDep.PresionINS;
		if(FResInstantDep.Temperature)
			insoutput << "\t" << FResInstantDep.TemperaturaINS;
		if(FResInstantDep.Volumen)
			insoutput << "\t" << FResInstantDep.VolumenINS;
		if(FResInstantDep.Masa)
			insoutput << "\t" << FResInstantDep.MasaINS;
		if(FResInstantDep.FraccionMasicaEspecies) {
			for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
				insoutput << "\t" << FResInstantDep.FraccionINS[i];
			}
		}
		if(FResInstantDep.Gamma)
			insoutput << "\t" << FResInstantDep.GammaINS;

		// fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TDeposito::ImprimeResultadosInstaneosDep en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::HeaderAverageResultsDep(stringstream& medoutput, stEspecies *DatosEspecies) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");
		std::string Label;
		if(FNumResMed > 0) {
			if(FResMediosDep.Pressure) {
				Label = "\t" + PutLabel(501) + std::to_string(FNumeroDeposito) + PutLabel(908);
				medoutput << Label.c_str();
			}
			if(FResMediosDep.Temperature) {
				Label = "\t" + PutLabel(502) + std::to_string(FNumeroDeposito) + PutLabel(910);
				medoutput << Label.c_str();
			}
			if(FResMediosDep.FraccionMasicaEspecies) {
				for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
					Label = "\t" + PutLabel(505) + DatosEspecies[i].Nombre + PutLabel(506) + std::to_string(FNumeroDeposito) + PutLabel(
								901);
					medoutput << Label.c_str();
				}
			}
		}
		// fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TDeposito::HeaderAverageResultsDep en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::ImprimeResultadosMediosDep(stringstream& medoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");
		if(FNumResMed > 0) {
			if(FResMediosDep.Pressure)
				medoutput << "\t" << FResMediosDep.PresionMED;
			if(FResMediosDep.Temperature)
				medoutput << "\t" << FResMediosDep.TemperaturaMED;
			if(FResMediosDep.FraccionMasicaEspecies) {
				for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
					medoutput << "\t" << FResMediosDep.FraccionMED[i];
				}
			}
		}
		// fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TDeposito::ImprimeResultadosMediosDep en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TDeposito::CriterioEstabilidad(double TMinimo) {
	double DeltTMin = 0., MasaFinal = 0., g = 0.;
	int SignoFlujoED = 0;

	try {

		DeltTMin = TMinimo - FTime;
		MasaFinal = 0.;
		for(int i = 0; i < FNumeroUniones; i++) {
			if(FCCDeposito[i]->getTipoCC() == nmPipeToPlenumConnection) {
				g = (double) - dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getMassflow();
				if(!(FCCDeposito[i]->getUnionDPF())) {
					MasaFinal += g * DeltTMin * FCCDeposito[i]->GetTuboExtremo(0).Pipe->getNumeroConductos();
				} else if(FCCDeposito[i]->getUnionDPF()) {
#ifdef ParticulateFilter
					MasaFinal += g * DeltTMin * FCCDeposito[i]->GetTuboExtremo(0).DPF->GetCanal(FCCDeposito[i]->GetTuboExtremo(0).NumeroHaz,
								 0)->getNumeroCanales();
#endif
				}
			}
		}
		for(int i = 1; i < FNumeroUnionesED; i++) {
			if(FCCUnionEntreDep[i]->getTipoCC() == nmUnionEntreDepositos) {
				if(FNumeroDeposito == dynamic_cast<TCCUnionEntreDepositos*>(FCCUnionEntreDep[i])->getNumeroDeposito1()) {
					SignoFlujoED = dynamic_cast<TCCUnionEntreDepositos*>(FCCUnionEntreDep[i])->getSentidoFlujoED1();
				} else if(FNumeroDeposito == dynamic_cast<TCCUnionEntreDepositos*>(FCCUnionEntreDep[i])->getNumeroDeposito2()) {
					SignoFlujoED = dynamic_cast<TCCUnionEntreDepositos*>(FCCUnionEntreDep[i])->getSentidoFlujoED2();
				}
				g = (double) SignoFlujoED * dynamic_cast<TCCDeposito*>(FCCDeposito[i])->getMassflow();
				MasaFinal += g * DeltTMin;
			}
		}

		if(FHayCompresor) {
			g = (double) FCompresorSentido * FCompresor->getMassflow();
			MasaFinal += g * DeltTMin;
		}

		MasaFinal += FMasa;
		return MasaFinal;
	} catch(exception & N) {
		std::cout << "ERROR: TDeposito::CriterioEstabilidad en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::SalidaGeneralDep(stEspecies *DatosEspecies) {
	try {

		if(FNumResMed > 0) {
			std::cout << std::endl;
			std::cout << "***************************************" << std::endl;
			std::cout << "****** AVERAGE VALUES IN PLENUMS ******" << std::endl;
			std::cout << "***************************************" << std::endl << std::endl;
			std::cout << "In plenum: " << FNumeroDeposito << std::endl;
			if(FResMediosDep.Pressure)
				std::cout << "  Pressure:     " << FResMediosDep.PresionMED << " bares" << std::endl;
			if(FResMediosDep.Temperature)
				std::cout << "  Temperature:  " << FResMediosDep.TemperaturaMED << " degC" << std::endl << std::endl;
			if(FResMediosDep.FraccionMasicaEspecies) {
				for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
					std::cout << "  Average mass fraction of " << DatosEspecies[i].Nombre << ": " << FResMediosDep.FraccionMED[i] << " (-)"
							  << std::endl;
				}
			}
			std::cout << std::endl;
		}

	} catch(exception & N) {
		std::cout << "ERROR: TDeposito::SalidaGeneralDep en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCondicionContorno* TDeposito::GetCCUnionEntreDep(int i) {
	try {
		return FCCUnionEntreDep[i];
	} catch(exception & N) {
		std::cout << "ERROR: TDeposito::GetCCUnionEntreDep en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCondicionContorno* TDeposito::GetCCDeposito(int i) {
	try {
		return FCCDeposito[i];
	} catch(exception & N) {
		std::cout << "ERROR: TDeposito::GetCCDeposito en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
////void TDeposito::PutCalculadoPaso(bool valor) {
//	try {
//
//		FCalculadoPaso = valor;
//
//	}
//	catch(Exception & N) {
//		std::cout << "ERROR: TDeposito::PutCalculadoPaso en el deposito: " << FNumeroDeposito << std::endl;
//		std::cout << "Tipo de error: " << N.what() << std::endl;
//		throw Exception(N.what());
//	}
//}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TDeposito::ActualizaTiempo(double TiempoActual) {
	try {

		FTime = TiempoActual;

	} catch(exception & N) {
		std::cout << "ERROR: TDeposito::PutCalculadoPaso en el deposito: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TDeposito::GetFraccionMasicaEspecie(int i) {
	try {
		return FFraccionMasicaEspecie[i];
	} catch(exception & N) {
		std::cout << "ERROR: TDeposito::GetConcentracionEspecie en la condicion de contorno: " << FNumeroDeposito << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#pragma package(smart_init)
