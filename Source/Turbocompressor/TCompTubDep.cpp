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
#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
//#include <cmath>
#include "TCompTubDep.h"
#include "TCCCompresor.h"
#include "TDeposito.h"
#include "TTubo.h"
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCompTubDep::TCompTubDep(int i, nmTipoCalculoEspecies SpeciesModel, int numeroespecies, nmCalculoGamma GammaCalculation,
						 bool ThereIsEGR) :
	TCompresor(i, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {

	FModeloCompresor = nmCompOriginal;
	FCorrector = 1;
	// Mapa = new TMapaComp(FNumeroCompresor);

	FMass_filt_ant = 0;
	FMass_ant = 0;
	RC_ant = 1.;
	RC_filt_ant = 1.;
	FDelay = 0.001;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCompTubDep::~TCompTubDep() {

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompTubDep::LeeCompresor(const char *FileWAM, fpos_t &filepos) {
	try {

		int format = 0, ac = 0;
		int InID = 0, OutID = 0, VolID = 0, StaID = 0, RotID = 0;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

#ifdef tchtm

		fscanf(fich, "%d ", &ac);
		if(ac == 1) {
			FIsAcoustic = true;
			fscanf(fich, "%d %d %d %d %d", &InID, &OutID, &VolID, &RotID, &StaID);
			FAcComp = new TAcousticCompressor(InID, VolID, OutID, RotID, StaID);
		}

#endif

		fscanf(fich, "%lf", &FDelay);

		fscanf(fich, "%d ", &format);
		if(format == 0) {
			FCompressorMapFormat = nmOldWAMmap;
			Mapa = new TMapaComp(FNumeroCompresor);

		} else {
			FCompressorMapFormat = nmSAMmap;
			Mapa = new TSAEMap(FNumeroCompresor);
		}

		Mapa->LeeMapa(fich);

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TCompTubDep::LeeCompresor en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: LeeCompresor en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TCompTubDep::CalGastoNuevo(double MasaAire) {
	double ret_val = 0., ac = 0., bc = 0., cc = 0., discr = 0.;
	try {

		FRelacionCompresion = Mapa->EvaluaRCHermite(MasaAire);
		FRendimiento = Mapa->EvaluaRendSplines(MasaAire);
		if(FRendimiento < 0.01)
			FRendimiento = 0.01;

#ifdef tchtm

		double Kef = FAcComp->EFCorrector(FCorrector, FRelacionCompresion);
		FRelacionCompresion = FRelacionCompresion * FCorrector;
		FRendimiento = FRendimiento * Kef;

#endif

		if(FDeltaTiempo > 0) {
			RC_filt = ((2 * FDelay - FDeltaTiempo) * RC_filt_ant + FDeltaTiempo * (FRelacionCompresion + RC_ant)) /
					  (2 * FDelay + FDeltaTiempo);
			FRelacionCompresion = RC_filt;

		}
		FPresion20 = FRelacionCompresion * FPresion10;
		FTemperatura20 = FTemperatura10 + (pow(FRelacionCompresion, FGamma5 * 2.) - 1.) * FTemperatura10 / FRendimiento;
		FEntropia2 = sqrt(FGamma * FRMezcla * FTemperatura20) / pow(FPresion20 / __cons::PRef, FGamma5) / __cons::ARef;

		ac = pow2(FEntropia2 * FGamma3 / *FEntro) + FGamma3;
		bc = -pow2(FEntropia2 / *FEntro) * FGamma1 * *FLanda;
		cc = pow2(FEntropia2 * *FLanda / *FEntro) - FTemperatura20 * FGamma * FRMezcla / __cons::ARef2;
		discr = bc * bc - ac * 4 * cc;

		if(discr < 0.) {
			FVelocidad2 = -bc / 2. / ac;
		} else {
			FVelocidad2 = (-bc - sqrt(discr)) / 2. / ac;
		}

		FASonidoSalida = (*FLanda - FGamma3 * FVelocidad2) * FEntropia2 / *FEntro;
		FDensidad20 = __units::BarToPa(FPresion20) / FRMezcla / FTemperatura20;
		FTemperatura2 = pow2(FASonidoSalida * __cons::ARef) / FGamma / FRMezcla;
		FDensidad2 = FDensidad20 * pow(FTemperatura2 / FTemperatura20, 1. / FGamma1);
		ret_val = -FDensidad2 * FVelocidad2 * FAreaSalComp * __cons::ARef * sqrt(FTemperatura10 / Mapa->getTempRef()) *
				  Mapa->getPresionRef() / __units::BarToPa(FPresion10);

		return ret_val;
	} catch(exception & N) {
		std::cout << "ERROR: CalGastoNuevo en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: CalGastoNuevo en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TCompTubDep::RegulaFalsi() {
	double Masa0, Masa1, MasaX, fMasa0, fMasa1, fMasaX, Masa, MasaXant, GastoNuevo;
	bool valido;
	try {
		valido = false;
		FCambiaReg = false;

#ifdef tchtm
		FCorrector = FAcComp->CRCorrector() * 0.0001 + FCorrector * 0.9999;
#endif

		Masa0 = 0;
		Masa = CalGastoNuevo(Masa0);
		fMasa0 = Masa - Masa0;

		Masa1 = Mapa->getGastoRelComp1();
		Masa = CalGastoNuevo(Masa1);
		fMasa1 = Masa - Masa1;

		if(fMasa0 * fMasa1 < 0)
			valido = true;

		if(!valido) {
			FCambiaReg = true;
			if(fMasa0 <= 0) {
				// FVariacionRegimen=1.05;
				// std::cout << "WARNING: El compresor: " << FNumeroCompresor <<
				// " intenta trabajar con flujo inverso" << std::endl;
				GastoNuevo = 0.;
				FPresion20 = Mapa->getRelCompBombeo() * FPresion10;
				FASonidoSalida = *FLanda;
				FVelocidad2 = 0;
				FEntropia2 = *FEntro;
				FRelacionCompresion = Mapa->getRelCompBombeo();
				if(FDeltaTiempo > 0) {
					RC_filt = ((2 * FDelay - FDeltaTiempo) * RC_filt_ant + FDeltaTiempo * (FRelacionCompresion + RC_ant)) /
							  (2 * FDelay + FDeltaTiempo);
					FRelacionCompresion = RC_filt;

				}
				FRendimiento = Mapa->EvaluaRendSplines(0);
				return 0;
			} else {

				GastoNuevo = Mapa->getGastoRelComp1() / (sqrt(FTemperatura10 / Mapa->getTempRef()) * Mapa->getPresionRef() /
							 __units::BarToPa(FPresion10));
				FPresion20 = FPresion10;
				FTemperatura20 = FTemperatura10;

				double Adep = sqrt(FGamma * FRMezcla * FTemperatura20) / __cons::ARef;
				FEntropia2 = Adep / pow(FPresion20 / __cons::PRef, FGamma5);

				double a = pow2(FGamma3 * FEntropia2 / *FEntro) + FGamma3;
				double b = __Gamma::G1(FGamma) * *FLanda * pow(FEntropia2 / *FEntro, 2);
				double c = pow2(FEntropia2 / *FEntro * *FLanda) - pow(Adep, 2);

				FVelocidad2 = -QuadraticEqP(a, b, c);
				FASonidoSalida = sqrt(pow2(Adep) - FGamma3 * pow2(FVelocidad2));

				std::cout << "WARNING: El compresor: " << FNumeroCompresor << " esta trabajando en zona de choque" << std::endl;
				// FVariacionRegimen=0.95;

				FDensidad2 = FGamma * __units::BarToPa(pow(FASonidoSalida / FEntropia2, FGamma4) / pow2(FASonidoSalida * __cons::ARef));

				GastoNuevo = -FVelocidad2 * __cons::ARef * FAreaSalComp * FDensidad2 * (sqrt(FTemperatura10 / Mapa->getTempRef()) /
							 Mapa->getPresionRef() * __units::BarToPa(FPresion10));

				return GastoNuevo;
			}
			// return 0;
		} else {
			// Metodo regula-falsi modificado
			MasaXant = Masa0;
			int i = 0;
			do {
				MasaX = (fMasa0 * Masa1 - fMasa1 * Masa0) / (fMasa0 - fMasa1);
				Masa = CalGastoNuevo(MasaX);
				fMasaX = Masa - MasaX;
				if(fMasa0 * fMasaX < 0) {
					fMasa1 = fMasaX;
					Masa1 = MasaX;
					if(Masa0 == MasaXant) {
						// Se mantiene 2 veces o mas el extremo izquierdo intervalo
						fMasa0 = fMasa0 / 2.;
					}
					MasaXant = Masa0;
				} else {
					fMasa0 = fMasaX;
					Masa0 = MasaX;
					if(Masa1 == MasaXant) {
						// Se mantiene 2 veces o mas el extremo izquierdo intervalo
						fMasa1 = fMasa1 / 2.;
					}
					MasaXant = Masa1;
				}
				++i;
			} while(fabs(fMasaX) > 1e-6 && i <= 100 && fabs(Masa0 - Masa1) > 1e-6);
			if(i > 100) {
				std::cout << "ERROR: The interation method in the compressor does not converge in 100 iterations" << std::endl;
				throw Exception("ERROR: The interation method in the compressor does not converge in 100 iterations");
			}
			GastoNuevo = MasaX;
			return GastoNuevo;
		}

	} catch(exception & N) {
		std::cout << "ERROR: RegulaFalsi en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: RegulaFalsi en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompTubDep::CalculaCompresor(double Theta) {
	double temp = 0., work = 0.;
	try {

		temp = __units::BarToPa(pow(*FLanda / *FEntro, FGamma4));
		//double P20Surge = Mapa->getRelCompBombeo() * FPresion10 * 1e5;
		double P20Max = Mapa->getMaxCompRatio() * __units::BarToPa(FPresion10);

#ifdef tchtm
		P20Max = P20Max * FCorrector;
#endif
		if(temp >= P20Max) {
			*FBeta = *FLanda;
			work = 0.;
			FGastoCorregido = 0.;
			FRelacionCompresion = Mapa->getRelCompBombeo();
			if(FDeltaTiempo > 0) {
				RC_filt = ((2 * FDelay - FDeltaTiempo) * RC_filt_ant + FDeltaTiempo * (FRelacionCompresion + RC_ant)) /
						  (2 * FDelay + FDeltaTiempo);
				FRelacionCompresion = RC_filt;
			}
			FRendimiento = Mapa->EvaluaRendSplines(0);
			FCambiaReg = false;
			FASonidoSalida = *FLanda;
			FEntropia2 = *FEntro;
			//FGasto1 = NewDampedSolution(0);
			FGasto1 = 0;
		} else {
			FGastoCorregido = RegulaFalsi();

			*FEntro = FEntropia2;
			*FLanda = FASonidoSalida + FGamma3 * FVelocidad2;
			*FBeta = FASonidoSalida - FGamma3 * FVelocidad2;
			work = (FTemperatura20 - FTemperatura10) * FRMezcla * FGamma4 / 2.;

		}
		RC_ant = FRelacionCompresion;
		RC_filt_ant = RC_filt;

		FGasto1 = FGastoCorregido * __units::BarToPa(FPresion10) / Mapa->getPresionRef() / sqrt(
					  FTemperatura10 / Mapa->getTempRef());
		FGasto0 = FGasto1;
		FGasto0Correg = FGastoCorregido;
		FTrabajo = work * FGasto1 * FDeltaTiempo;
		FGastoCompresor = FGasto1;

		if(FDeltaTiempo != 0) {
			FPotencia = FTrabajo / FDeltaTiempo;
		} else
			FPotencia = 0.;
		FTrabajoPaso += FTrabajo;
		FDeltaTPaso += FDeltaTiempo;
		// }

	} catch(exception & N) {
		std::cout << "ERROR: TCompTubDep::CalculaCompresor en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: TCompTubDep::CalculaCompresor en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompTubDep::CondicionCompresor(double Theta, stTuboExtremo *TuboExtremo, double AcumulatedTime,
									 int TuboCalculado) {
	try {
		double FraccionMasicaAcum = 0.;

		if(FExtremoSalida == nmRight) {
			FLanda = &(TuboExtremo[0].Landa);
			FBeta = &(TuboExtremo[0].Beta);
			FEntro = &(TuboExtremo[0].Entropia);
		} else {
			FBeta = &(TuboExtremo[0].Landa);
			FLanda = &(TuboExtremo[0].Beta);
			FEntro = &(TuboExtremo[0].Entropia);
		}
		FDeltaTiempo = AcumulatedTime - FTiempo0;
		FTiempo0 = AcumulatedTime;

		FCambiaReg = true;

		CalculaCompresor(Theta);

		// Transporte de especies quimicas.
		if(FGasto1 > 0.) {
			switch(FEntradaCompresor) {
			case nmPipe:
				// Transporte de especies quimicas.
				for(int i = 0; i < FNumeroEspecies - 2; i++) {
					FFraccionMasicaEspecie[i] = FTuboRotor->GetFraccionMasicaCC(FIndiceCC, i);
					FraccionMasicaAcum += FFraccionMasicaEspecie[i];
				}
				FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
				if(FHayEGR)
					FFraccionMasicaEspecie[FNumeroEspecies - 1] = FTuboRotor->GetFraccionMasicaCC(FIndiceCC, FNumeroEspecies - 1); // EGR
				break;
			case nmPlenum:
				// Transporte de especies quimicas.
				for(int i = 0; i < FNumeroEspecies - 2; i++) {
					FFraccionMasicaEspecie[i] = FDeposito->GetFraccionMasicaEspecie(i);
					FraccionMasicaAcum += FFraccionMasicaEspecie[i];
				}
				FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
				if(FHayEGR)
					FFraccionMasicaEspecie[FNumeroEspecies - 1] = FDeposito->GetFraccionMasicaEspecie(FNumeroEspecies - 1); // EGR
				break;

				// Cuando la entrada sea la atmasfera, la composicion se mantendra siempre constante.
			}
		}

		FRegimenCorregido = Mapa->getRegimenCorregido();

	} catch(exception & N) {
		std::cout << "ERROR: CondicionCompresor en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: CondicionCompresor en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompTubDep::BusquedaEntradaSalida(nmCompressorInlet EntradaCompresor, double AmbientTemperature, int numeroCC,
										TCondicionContorno **BC, double *AtmosphericComposition) {
	try {
		double Cp = 0.;

		FEntradaCompresor = EntradaCompresor;
		if(BC[numeroCC - 1]->GetTuboExtremo(0).TipoExtremo == nmLeft) {
			FAreaSalComp = __geom::Circle_area(BC[numeroCC - 1]->GetTuboExtremo(0).Pipe->GetDiametro(0));
			FExtremoSalida = nmLeft;
			FNodoFinTuboSalida = 0;
			FIndiceCC = 0;
		} else if(BC[numeroCC - 1]->GetTuboExtremo(0).TipoExtremo == nmRight) {
			FAreaSalComp = __geom::Circle_area(BC[numeroCC - 1]->GetTuboExtremo(0).Pipe->GetDiametro(
												   BC[numeroCC - 1]->GetTuboExtremo(0).Pipe->getNin() - 1));
			FExtremoSalida = nmRight;
			FNodoFinTuboSalida = BC[numeroCC - 1]->GetTuboExtremo(0).Pipe->getNin() - 1;
			FIndiceCC = 1;
		}

		switch(FEntradaCompresor) {

		case nmAtmosphere:
			FTemperatura10 = __units::degCToK(AmbientTemperature);
			// Inicializacion del transporte de especies quimicas.
			FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
			for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
				FFraccionMasicaEspecie[i] = AtmosphericComposition[i];
			}
			break;
		case nmPipe:
			FTuboRotor = dynamic_cast<TCCCompresor*>(BC[numeroCC - 1])->getTuboRotor();
			FExtremoTuboRotor = dynamic_cast<TCCCompresor*>(BC[numeroCC - 1])->getExtremoTuboRotor();
			break;
		case nmPlenum:
			FDeposito = dynamic_cast<TCCCompresor*>(BC[numeroCC - 1])->getPlenum();
			FDeposito->AsignaCompresor(this, -1); /* Entrada */
		}

	} catch(exception & N) {
		std::cout << "ERROR: BusquedaEntradaSalida en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: BusquedaEntradaSalida en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

void TCompTubDep::Initialize() {

	double Cp = 0.;

	switch(FEntradaCompresor) {
	case nmAtmosphere:
		if(FCalculoEspecies == nmCalculoCompleto) {

			FRAtm = CalculoCompletoRMezcla(FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2], 0,
										   FCalculoGamma, nmMEP);
			FCpAtm = CalculoCompletoCpMezcla(FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2], 0,
											 FTemperatura10, FCalculoGamma, nmMEP);
			FGammaAtm = CalculoCompletoGamma(FRAtm, FCpAtm, FCalculoGamma);

		} else if(FCalculoEspecies == nmCalculoSimple) {

			FRAtm = CalculoSimpleRMezcla(FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FCalculoGamma, nmMEP);
			FCvAtm = CalculoSimpleCvMezcla(FTemperatura10, FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FCalculoGamma,
										   nmMEP);
			FGammaAtm = CalculoSimpleGamma(FRAtm, FCvAtm, FCalculoGamma);
		}

		break;

	case nmPipe:
		if(FExtremoTuboRotor == nmLeft) {
			FNodoFinEntrada = 0;
			Cp = (FTuboRotor->GetGamma(0) * FTuboRotor->GetRMezcla(0)) / (FTuboRotor->GetGamma(0) - 1);
		} else {
			Cp = (FTuboRotor->GetGamma(FTuboRotor->getNin() - 1) * FTuboRotor->GetRMezcla(FTuboRotor->getNin() - 1)) /
				 (FTuboRotor->GetGamma(FTuboRotor->getNin() - 1) - 1);
			FNodoFinEntrada = FTuboRotor->getNin() - 1;
		}
		FTemperatura10 = __units::degCToK(FTuboRotor->getTemperaturaInicial()) + pow(FTuboRotor->getVelocidadMedia(),
						 2.) / 2. / Cp;

		// Inicializacion del transporte de especies quimicas.
		FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
		for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
			FFraccionMasicaEspecie[i] = FTuboRotor->GetFraccionMasicaInicial(i);
		}

		break;
	case nmPlenum:
		FTemperatura10 = __units::degCToK(FDeposito->getTemperature());

		// Inicializacion del transporte de especies quimicas.
		FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
		for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
			FFraccionMasicaEspecie[i] = FDeposito->GetFraccionMasicaEspecie(i);
		}
		break;
	}

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompTubDep::DatosEntradaCompresor(double AmbientTemperature, double AmbientPressure, TCondicionContorno *BC) {
	try {
		double pentcomp = 0., tentcomp = 0., ventcomp = 0.;
		double RMezclaEnt = 0., RMezclaSal = 0., GammaEnt = 0., GammaSal = 0., Cp = 0.;

		switch(FEntradaCompresor) {
		case nmAtmosphere:
			// Calculo de Gamma y R en el compresor. Media de valores a la entrada y la salida.
			RMezclaSal = BC->GetTuboExtremo(0).Pipe->GetRMezcla(FNodoFinTuboSalida);
			GammaSal = BC->GetTuboExtremo(0).Pipe->GetGamma(FNodoFinTuboSalida);

			FGamma = (GammaSal + FGammaAtm) / 2.;
			FRMezcla = (RMezclaSal + FRAtm) / 2.;
			FGamma1 = __Gamma::G1(FGamma);
			FGamma3 = __Gamma::G3(FGamma);
			FGamma4 = __Gamma::G4(FGamma);
			FGamma5 = __Gamma::G5(FGamma);

			FPresion10 = AmbientPressure;
			FTemperatura10 = __units::degCToK(AmbientTemperature);
			break;
		case nmPlenum:
			// Calculo de Gamma y R en el compresor. Media de valores a la entrada y la salida.
			RMezclaEnt = FDeposito->getR();
			GammaEnt = FDeposito->getGamma();

			RMezclaSal = BC->GetTuboExtremo(0).Pipe->GetRMezcla(FNodoFinTuboSalida);
			GammaSal = BC->GetTuboExtremo(0).Pipe->GetGamma(FNodoFinTuboSalida);
			if(GammaSal < 1.3) {
				GammaSal = 1.4;
			}
			FGamma = (GammaSal + GammaEnt) / 2.;
			FRMezcla = (RMezclaSal + RMezclaEnt) / 2.;
			FGamma1 = __Gamma::G1(FGamma);
			FGamma3 = __Gamma::G3(FGamma);
			FGamma4 = __Gamma::G4(FGamma);
			FGamma5 = __Gamma::G5(FGamma);

			FPresion10 = FDeposito->getPressure();
			FTemperatura10 = __units::degCToK(FDeposito->getTemperature());

			break;
		case nmPipe:

			// Calculo de Gamma y R en el compresor. Media de valores a la entrada y la salida.
			RMezclaEnt = FTuboRotor->GetRMezcla(FNodoFinEntrada);
			GammaEnt = FTuboRotor->GetGamma(FNodoFinEntrada);

			RMezclaSal = BC->GetTuboExtremo(0).Pipe->GetRMezcla(FNodoFinTuboSalida);
			GammaSal = BC->GetTuboExtremo(0).Pipe->GetGamma(FNodoFinTuboSalida);

			FGamma = (GammaSal + GammaEnt) / 2.;
			FRMezcla = (RMezclaSal + RMezclaEnt) / 2.;
			FGamma1 = __Gamma::G1(FGamma);
			FGamma3 = __Gamma::G3(FGamma);
			FGamma4 = __Gamma::G4(FGamma);
			FGamma5 = __Gamma::G5(FGamma);

			if(FExtremoTuboRotor == nmLeft) {
				pentcomp = __units::BarToPa(FTuboRotor->GetPresion(0));
				ventcomp = FTuboRotor->GetVelocidad(0) * __cons::ARef;
				tentcomp = pow2(FTuboRotor->GetAsonido(0) * __cons::ARef) / FGamma / FRMezcla;
				Cp = (FTuboRotor->GetGamma(0) * FTuboRotor->GetRMezcla(0)) / (FTuboRotor->GetGamma(0) - 1);
			} else {
				pentcomp = __units::BarToPa(FTuboRotor->GetPresion(FTuboRotor->getNin() - 1));
				ventcomp = FTuboRotor->GetVelocidad(FTuboRotor->getNin() - 1) * __cons::ARef;
				tentcomp = pow2(FTuboRotor->GetAsonido(FTuboRotor->getNin() - 1) * __cons::ARef) / FGamma / FRMezcla;
				Cp = (FTuboRotor->GetGamma(FTuboRotor->getNin() - 1) * FTuboRotor->GetRMezcla(FTuboRotor->getNin() - 1)) /
					 (FTuboRotor->GetGamma(FTuboRotor->getNin() - 1) - 1);
			}
			FTemperatura10 = tentcomp + pow2(ventcomp) / 2. / Cp;
			FPresion10 = pentcomp * pow((FTemperatura10 / tentcomp), (FGamma / FGamma1));
			break;
		}

	} catch(exception & N) {
		std::cout << "ERROR: DatosEntradaCompresor en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: DatosEntradaCompresor en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

double TCompTubDep::NewDampedSolution(double Mass) {

	double A2, U2, AA2, A20, AA2Old, A2Old, delta = 1, delta0 = 1, deltaA = 1.;
	double A10 = 0., AA1 = 0.;
	double Mass_filt = 0.;
	bool conv;

	// FDelay = 0.1;

	if(FDeltaTiempo > 0) {
		Mass_filt = ((2 * FDelay - FDeltaTiempo) * FMass_filt_ant + FDeltaTiempo * (Mass + FMass_ant)) /
					(2 * FDelay + FDeltaTiempo);
		FMass_filt_ant = Mass_filt;
		FMass_ant = Mass;
	}

	// Mass_filt = 0.5 * Mass + 0.5 * FMass_ant;
	// FMass_ant = Mass;

	// Mass_filt = Mass;

	FRendimiento = Mapa->EvaluaRendSplines(Mass_filt) * FAcComp->EFCorrector(FCorrector, FRelacionCompresion);

	A10 = sqrt(FGamma * FRMezcla * FTemperatura10) / __cons::ARef;
	AA1 = pow(FPresion10, -FGamma5) * A10;

	A2 = FASonidoSalida;
	A2Old = A2;
	AA2 = FEntropia2;
	AA2Old = AA2;
	A20 = sqrt(pow2(A2) + FGamma3 * pow2(FVelocidad2));

	// double m2 = FDensidad2 * FVelocidad2 * FAreaSalComp * __cons::ARef;
	int step = 0;

	while((delta > 1e-5 || deltaA > 1e-5) && step < 100) {

		U2 = Mass_filt / (FGamma * __units::BarToPa(pow(A2 / AA2, FGamma4)) * FAreaSalComp / pow2(A2) / __cons::ARef);

		A2 = (*FLanda + FGamma3 * U2) * AA2 / *FEntro;

		A20 = sqrt(pow2(A2) + FGamma3 * pow2(U2));

		if(U2 <= 0)
			AA2 = *FEntro;
		else
			AA2 = A20 * AA1 / A10 / sqrt((pow2(A20) - pow2(A10)) / pow2(A10) * FRendimiento + 1);

		delta = fabs((AA2 - AA2Old) / AA2Old);

		deltaA = fabs((A2 - A2Old) / A2Old);

		AA2Old = AA2;
		A2Old = A2;

		if(delta > delta0) {
			// Mass_filt = (Mass_filt + Mass) / 2;
			FMass_filt_ant = Mass_filt;
		}

		delta0 = delta;

		step++;
	}

	FEntropia2 = AA2;
	FVelocidad2 = -U2;
	FASonidoSalida = A2;
	FTemperatura2 = pow(FASonidoSalida * __cons::ARef, 2.0) / FGamma / FRMezcla;
	FTemperatura20 = pow(A20 * __cons::ARef, 2.0) / FGamma / FRMezcla;
	// FPresion2 = pow(A2/AA2, FGamma4) * __cons::PRef;
	FPresion20 = pow(A20 / AA2, FGamma4) * __cons::PRef;
	FRelacionCompresion = FPresion20 / FPresion10;
	FDensidad20 = __units::BarToPa(FPresion20) / FRMezcla / FTemperatura20;
	FDensidad2 = FGamma * __units::BarToPa(pow(FASonidoSalida / FEntropia2, FGamma4)) / pow2(FASonidoSalida * __cons::ARef);

	printf("%lf %lf %lf\n", Mass_filt, Mass, -FVelocidad2 * __cons::ARef * FDensidad2 * FAreaSalComp);

	// if(conv)
	// printf("yes %lf %lf ", FTemperatura2, FPresion20);
	// else
	// printf("no %lf %lf ", FTemperatura2, FPresion20);

	return Mass_filt;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#pragma package(smart_init)
