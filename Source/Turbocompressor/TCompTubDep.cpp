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
#include <cmath>
#include "TCompTubDep.h"
#include "TCCCompresor.h"
#include "TDeposito.h"
#include "TTubo.h"
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCompTubDep::TCompTubDep(int i, nmTipoCalculoEspecies SpeciesModel,
	int numeroespecies, nmCalculoGamma GammaCalculation, bool ThereIsEGR)
	: TCompresor(i, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR)
{

	FModeloCompresor = nmCompOriginal;
	FCorrector = 1;
	// Mapa = new TMapaComp(FNumeroCompresor);

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCompTubDep::~TCompTubDep() {

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompTubDep::LeeCompresor(char *FileWAM, fpos_t &filepos) {
	try {

		int format, ac;
		int InID, OutID, VolID, StaID, RotID;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

#if tchtm

		fscanf(fich, "%d ", &ac);
		if (ac == 1) {
			FIsAcoustic = true;
			fscanf(fich, "%d %d %d %d %d", &InID, &OutID, &VolID, &RotID,
				&StaID);
			FAcComp = new TAcousticCompressor(InID, VolID, OutID, RotID, StaID);
		}

#endif

		fscanf(fich, "%d ", &format);
		if (format == 0) {
			FCompressorMapFormat = nmOldWAMmap;
			Mapa = new TMapaComp(FNumeroCompresor);

		}
		else {
			FCompressorMapFormat = nmSAMmap;
			Mapa = new TSAEMap(FNumeroCompresor);
		}

		Mapa->LeeMapa(fich);

		fgetpos(fich, &filepos);
		fclose(fich);
	}
	catch (Exception & N) {
		std::cout << "ERROR: TCompTubDep::LeeCompresor en el compresor: " <<
			FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception("ERROR: LeeCompresor en el compresor: " +
			AnsiString(FNumeroCompresor) + N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TCompTubDep::CalGastoNuevo(double MasaAire) {
	double ret_val, ac, bc, cc, discr;
	try {

		FRelacionCompresion = Mapa->EvaluaRCHermite(MasaAire);
		FRendimiento = Mapa->EvaluaRendSplines(MasaAire);

#if tchtm

		double Kef = FAcComp->EFCorrector(FCorrector, FRelacionCompresion);
		FRelacionCompresion = FRelacionCompresion * FCorrector;
		FRendimiento = FRendimiento * Kef;
#endif

		FPresion20 = FRelacionCompresion * FPresion10;
		FTemperatura20 = FTemperatura10 +
			(pow(FRelacionCompresion, FGamma5 * 2.) - 1.)
			* FTemperatura10 / FRendimiento;
		FEntropia2 = sqrt(FGamma * FRMezcla * FTemperatura20) / pow
			(FPresion20 * 1e5 / 1e5, FGamma5) / ARef;

		ac = pow(FEntropia2 * FGamma3 / *FEntro, 2.0) + FGamma3;
		bc = -pow(FEntropia2 / *FEntro, 2.0) * FGamma1 * *FLanda;
		cc = pow(FEntropia2 * *FLanda / *FEntro, 2.0) -
			FTemperatura20 * FGamma * FRMezcla / ARef / ARef;
		discr = bc * bc - ac * 4 * cc;

		if (discr < 0.) {
			FVelocidad2 = -bc / 2. / ac;
		}
		else {
			FVelocidad2 = (-bc - sqrt(discr)) / 2. / ac;
		}

		FASonidoSalida = (*FLanda - FGamma3 * FVelocidad2)
			* FEntropia2 / *FEntro;
		FDensidad20 = FPresion20 * 1e5 / FRMezcla / FTemperatura20;
		FTemperatura2 = pow(FASonidoSalida * ARef, 2.0) / FGamma / FRMezcla;
		FDensidad2 = FDensidad20 * pow(FTemperatura2 / FTemperatura20,
			1. / FGamma1);
		ret_val = -FDensidad2 * FVelocidad2 * FAreaSalComp * ARef * sqrt
			(FTemperatura10 / Mapa->getTempRef()) * Mapa->getPresionRef() /
			(FPresion10 * 1e5);

		return ret_val;
	}
	catch (Exception & N) {
		std::cout << "ERROR: CalGastoNuevo en el compresor: " <<
			FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception("ERROR: CalGastoNuevo en el compresor: " +
			AnsiString(FNumeroCompresor) + N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TCompTubDep::RegulaFalsi() {
	double Masa0, Masa1, MasaX, fMasa0, fMasa1, fMasaX, Masa, MasaXant,
		GastoNuevo;
	bool valido;
	try {
		valido = false;
		FCambiaReg = false;

#if tchtm
		FCorrector = FAcComp->CRCorrector() * 0.0001 + FCorrector * 0.9999;
#endif

		Masa0 = 0;
		Masa = CalGastoNuevo(Masa0);
		fMasa0 = Masa - Masa0;

		Masa1 = Mapa->getGastoRelComp1();
		Masa = CalGastoNuevo(Masa1);
		fMasa1 = Masa - Masa1;

		if (fMasa0 * fMasa1 < 0)
			valido = true;

		if (!valido) {
			FCambiaReg = true;
			if (fMasa0 <= 0) {
				// FVariacionRegimen=1.05;
				// std::cout << "WARNING: El compresor: " << FNumeroCompresor <<
				// " intenta trabajar con flujo inverso" << std::endl;
				GastoNuevo = 0.;
				FPresion20 = Mapa->getRelCompBombeo();
				FASonidoSalida = *FLanda;
				FVelocidad2 = 0;
				FEntropia2 = *FEntro;
				return 0;
			}
			else {

				GastoNuevo =
					Mapa->getGastoRelComp1() /
					(sqrt(FTemperatura10 / Mapa->getTempRef())
					* Mapa->getPresionRef() / (FPresion10 * 1e5));
				FPresion20 = FPresion10;
				FTemperatura20 = FTemperatura10;

				double Adep = sqrt(FGamma * FRMezcla * FTemperatura20) / ARef;
				FEntropia2 = Adep / pow(FPresion20 * 1e5 / 1e5, FGamma5);

				double a = pow(FGamma3 * FEntropia2 / *FEntro, 2.) + FGamma3;
				double b = Gamma1(FGamma) * *FLanda * pow
					(FEntropia2 / *FEntro, 2);
				double c = pow(FEntropia2 / *FEntro * *FLanda, 2.) -
					pow(Adep, 2);

				FVelocidad2 = -QuadraticEqP(a, b, c);
				FASonidoSalida =
					sqrt(pow(Adep, 2.) - FGamma3 * pow(FVelocidad2, 2.));

				std::cout << "WARNING: El compresor: " << FNumeroCompresor <<
					" esta trabajando en zona de choque" << std::endl;
				// FVariacionRegimen=0.95;
				FDensidad2 = 1e5 * pow(FASonidoSalida / FEntropia2, FGamma4)
					/ pow2(FASonidoSalida * ARef);

				GastoNuevo = -FVelocidad2 * ARef * FAreaSalComp * FDensidad2 *
					(sqrt(FTemperatura10 / Mapa->getTempRef())
					/ Mapa->getPresionRef() * (FPresion10 * 1e5));

				return GastoNuevo;
				// GastoNuevo = Mapa->getGastoRelComp1();
				// FPresion20 = FPresion10;
				// double U2Diff = 1;
				// double U2Old;
				// while (fabs(U2Diff) > 0.05) {
				// U2Old = FVelocidad2;
				// FVelocidad2 = fabs(FVelocidad2) / FVelocidad2 * GastoNuevo / FDensidad2 /
				// FAreaSalComp / ARef;
				// U2Diff = (FVelocidad2 - U2Old) / FVelocidad2;
				// FASonidoSalida = (*FLanda - FGamma3 * FVelocidad2) * FEntropia2 / *FEntro;
				// FDensidad20 = FPresion20 * 1e5 / FRMezcla / FTemperatura20;
				// FTemperatura2 = pow(FASonidoSalida * ARef, 2.0) / FGamma / FRMezcla;
				// FDensidad2 = FDensidad20 * pow(FTemperatura2 / FTemperatura20, 1. / FGamma1);
				// FTemperatura20 = FTemperatura2 * pow(FDensidad2 / FDensidad20, 1. / FGamma1);
				// FEntropia2 = sqrt(FGamma * FRMezcla * FTemperatura20) / pow
				// (FPresion20 * 1e5 / 1e5, FGamma5) / ARef;
				// }
				//
				// std::cout << "WARNING: El compresor: " << FNumeroCompresor <<
				// " esta trabajando en zona de choque" << std::endl;
				// // FVariacionRegimen=0.95;
			}
			// return 0;
		}
		else {
			// Metodo regula-falsi modificado
			MasaXant = Masa0;
			int i = 0;
			do {
				MasaX = (fMasa0 * Masa1 - fMasa1 * Masa0) / (fMasa0 - fMasa1);
				Masa = CalGastoNuevo(MasaX);
				fMasaX = Masa - MasaX;
				if (fMasa0 * fMasaX < 0) {
					fMasa1 = fMasaX;
					Masa1 = MasaX;
					if (Masa0 == MasaXant) {
						// Se mantiene 2 veces o mas el extremo izquierdo intervalo
						fMasa0 = fMasa0 / 2.;
					}
					MasaXant = Masa0;
				}
				else {
					fMasa0 = fMasaX;
					Masa0 = MasaX;
					if (Masa1 == MasaXant) {
						// Se mantiene 2 veces o mas el extremo izquierdo intervalo
						fMasa1 = fMasa1 / 2.;
					}
					MasaXant = Masa1;
				}
				++i;
			}
			while (fabs(fMasaX) > 1e-5 && i <= 100 && fabs(Masa0 - Masa1)
				> 1e-5);
			if (i > 100) {
				std::cout << "ERROR: El metodo de iteraci�n del compresor no converge en 100 iteraciones"
					<< std::endl;
				throw Exception
					("ERROR: El metodo de iteraci�n del compresor no converge en 100 iteraciones"
					);
			}
			GastoNuevo = MasaX;
			return GastoNuevo;
		}

	}
	catch (Exception & N) {
		std::cout << "ERROR: RegulaFalsi en el compresor: " <<
			FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception("ERROR: RegulaFalsi en el compresor: " +
			AnsiString(FNumeroCompresor) + N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompTubDep::CalculaCompresor(double Theta) {
	double temp, work;
	try {

		temp = pow(*FLanda / *FEntro, FGamma4) * 1e5;
		double P20Surge = Mapa->getRelCompBombeo() * FPresion10 * 1e5;
#if tchtm
		P20Surge = P20Surge * FCorrector;
#endif
		if (temp >= P20Surge) {
			*FBeta = *FLanda;
			work = 0.;
			FGastoCorregido = 0.;
			FCambiaReg = false;
		}
		else {
			FGastoCorregido = RegulaFalsi();
			// if(!FCambiaReg){
			*FEntro = FEntropia2;
			*FLanda = FASonidoSalida + FGamma3 * FVelocidad2;
			*FBeta = FASonidoSalida - FGamma3 * FVelocidad2;
			work = (FTemperatura20 - FTemperatura10) * FRMezcla * FGamma4 / 2.;
			// }
		}
		// if(!FCambiaReg){
//		if (FGastoCorregido < Mapa->getGastoBombeo() && FGasto0Correg >
//			Mapa->getGastoBombeo()) {
//			std::cout << "INFO: Inicio Bombeo en " << Theta << std::endl;
//		}
//		if (FGastoCorregido > Mapa->getGastoBombeo() && FGasto0Correg <
//			Mapa->getGastoBombeo()) {
//			std::cout << "INFO: Fin Bombeo en " << Theta << std::endl;
//		}
		FGasto1 = FGastoCorregido * FPresion10 * 1e5 / Mapa->getPresionRef()
			/ sqrt(FTemperatura10 / Mapa->getTempRef());
		FGasto0 = FGasto1;
		FGasto0Correg = FGastoCorregido;
		FTrabajo = work * FGasto1 * FDeltaTiempo;
		FGastoCompresor = FGasto1;

		if (FDeltaTiempo != 0) {
			FPotencia = FTrabajo / FDeltaTiempo;
		}
		else
			FPotencia = 0.;
		FTrabajoPaso += FTrabajo;
		FDeltaTPaso += FDeltaTiempo;
		// }

	}
	catch (Exception & N) {
		std::cout << "ERROR: TCompTubDep::CalculaCompresor en el compresor: " <<
			FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception
			("ERROR: TCompTubDep::CalculaCompresor en el compresor: " +
			AnsiString(FNumeroCompresor) + N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompTubDep::CondicionCompresor(double Theta, stTuboExtremo *TuboExtremo,
	double AcumulatedTime, int TuboCalculado) {
	try {
		double FraccionMasicaAcum = 0.;

		if (FExtremoSalida == nmRight) {
			FLanda = &(TuboExtremo[0].Landa);
			FBeta = &(TuboExtremo[0].Beta);
			FEntro = &(TuboExtremo[0].Entropia);
		}
		else {
			FBeta = &(TuboExtremo[0].Landa);
			FLanda = &(TuboExtremo[0].Beta);
			FEntro = &(TuboExtremo[0].Entropia);
		}
		FDeltaTiempo = AcumulatedTime - FTiempo0;
		FTiempo0 = AcumulatedTime;

		FCambiaReg = true;
		// while(FCambiaReg){
		CalculaCompresor(Theta);
		/* if(FCambiaReg){
		 FRegimen*=FVariacionRegimen;
		 if(FRegimen<=200){
		 std::cout << "ERROR: Regimen menor de 200; compresor inadecuado o condiciones inadecuadas en depositos" << std::endl;
		 throw Exception("");
		 }
		 if(FRegimen>=300000){
		 std::cout << "ERROR: Regimen mayor de 300000; compresor inadecuado o condiciones inadecuadas en depositos" << std::endl;
		 throw Exception("");
		 }
		 Mapa->InterpolaMapa(FRegimen,FTemperatura10);
		 } */
		// }

		// Transporte de especies qu�micas.
		if (FGasto1 > 0.) {
			switch (FEntradaCompresor) {
			case nmPipe:
				// Transporte de especies qu�micas.
				for (int i = 0; i < FNumeroEspecies - 2; i++) {
					FFraccionMasicaEspecie[i] =
						FTuboRotor->GetFraccionMasicaCC(FIndiceCC, i);
					FraccionMasicaAcum += FFraccionMasicaEspecie[i];
				}
				FFraccionMasicaEspecie[FNumeroEspecies - 2] =
					1. - FraccionMasicaAcum;
				if (FHayEGR)
					FFraccionMasicaEspecie[FNumeroEspecies - 1] =
						FTuboRotor->GetFraccionMasicaCC(FIndiceCC,
					FNumeroEspecies - 1); // EGR
				break;
			case nmPlenum:
				// Transporte de especies qu�micas.
				for (int i = 0; i < FNumeroEspecies - 2; i++) {
					FFraccionMasicaEspecie[i] =
						FDeposito->GetFraccionMasicaEspecie(i);
					FraccionMasicaAcum += FFraccionMasicaEspecie[i];
				}
				FFraccionMasicaEspecie[FNumeroEspecies - 2] =
					1. - FraccionMasicaAcum;
				if (FHayEGR)
					FFraccionMasicaEspecie[FNumeroEspecies - 1] =
						FDeposito->GetFraccionMasicaEspecie
						(FNumeroEspecies - 1); // EGR
				break;

				// Cuando la entrada sea la atm�sfera, la composici�n se mantendr� siempre constante.
			}
		}

		FRegimenCorregido = Mapa->getRegimenCorregido();

	}
	catch (Exception & N) {
		std::cout << "ERROR: CondicionCompresor en el compresor: " <<
			FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception("ERROR: CondicionCompresor en el compresor: " +
			AnsiString(FNumeroCompresor) + N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompTubDep::BusquedaEntradaSalida(nmCompressorInlet EntradaCompresor,
	double AmbientTemperature, int numeroCC, TCondicionContorno **BC,
	double *AtmosphericComposition) {
	try {
		double Cp;

		FEntradaCompresor = EntradaCompresor;
		if (BC[numeroCC - 1]->GetTuboExtremo(0).TipoExtremo == nmIzquierda) {
			FAreaSalComp =
				pow(BC[numeroCC - 1]->GetTuboExtremo(0).Pipe->GetDiametro(0),
			2.) * Pi / 4.;
			FExtremoSalida = nmLeft;
			FNodoFinTuboSalida = 0;
			FIndiceCC = 0;
		}
		else if (BC[numeroCC - 1]->GetTuboExtremo(0).TipoExtremo == nmDerecha) {
			FAreaSalComp =
				pow(BC[numeroCC - 1]->GetTuboExtremo(0).Pipe->GetDiametro
				(BC[numeroCC - 1]->GetTuboExtremo(0).Pipe->getNin() - 1), 2.)
				* Pi / 4.;
			FExtremoSalida = nmRight;
			FNodoFinTuboSalida = BC[numeroCC - 1]->GetTuboExtremo(0)
				.Pipe->getNin() - 1;
			FIndiceCC = 1;
		}

		switch (FEntradaCompresor) {

		case nmAtmosphere:
			FTemperatura10 = AmbientTemperature + 273.;
			// Inicializaci�n del transporte de especies qu�micas.
			FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
			for (int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
				FFraccionMasicaEspecie[i] = AtmosphericComposition[i];
			}
			break;
		case nmPipe:
			FTuboRotor = dynamic_cast<TCCCompresor*>(BC[numeroCC - 1])
				->getTuboRotor();
			FExtremoTuboRotor = dynamic_cast<TCCCompresor*>(BC[numeroCC - 1])
				->getExtremoTuboRotor();
			break;
		case nmPlenum:
			FDeposito = dynamic_cast<TCCCompresor*>(BC[numeroCC - 1])
				->getPlenum();
			FDeposito->AsignaCompresor(this, -1); /* Entrada */
		}

	}
	catch (Exception & N) {
		std::cout << "ERROR: BusquedaEntradaSalida en el compresor: " <<
			FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception("ERROR: BusquedaEntradaSalida en el compresor: " +
			AnsiString(FNumeroCompresor) + N.Message.c_str());
	}
}

void TCompTubDep::Initialize() {

	double Cp;

	switch (FEntradaCompresor) {
	case nmAtmosphere:
		if (FCalculoEspecies == nmCalculoCompleto) {

			FRAtm = CalculoCompletoRMezcla(FFraccionMasicaEspecie[0],
				FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2],
				FCalculoGamma);
			FCpAtm = CalculoCompletoCpMezcla(FFraccionMasicaEspecie[0],
				FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2],
				FTemperatura10, FCalculoGamma);
			FGammaAtm = CalculoCompletoGamma(FRAtm, FCpAtm, FCalculoGamma);

		}
		else if (FCalculoEspecies == nmCalculoSimple) {

			FRAtm = CalculoSimpleRMezcla(FFraccionMasicaEspecie[0],
				FCalculoGamma);
			FCvAtm = CalculoSimpleCvMezcla(FTemperatura10,
				FFraccionMasicaEspecie[0], FCalculoGamma);
			FGammaAtm = CalculoSimpleGamma(FRAtm, FCvAtm, FCalculoGamma);
		}

		break;

	case nmPipe:
		if (FExtremoTuboRotor == nmIzquierda) {
			FNodoFinEntrada = 0;
			Cp = (FTuboRotor->GetGamma(0) * FTuboRotor->GetRMezcla(0)) /
				(FTuboRotor->GetGamma(0) - 1);
		}
		else {
			Cp = (FTuboRotor->GetGamma(FTuboRotor->getNin() - 1)
				* FTuboRotor->GetRMezcla(FTuboRotor->getNin() - 1)) /
				(FTuboRotor->GetGamma(FTuboRotor->getNin() - 1) - 1);
			FNodoFinEntrada = FTuboRotor->getNin() - 1;
		}
		FTemperatura10 = FTuboRotor->getTemperaturaInicial() + 273. +
			pow(FTuboRotor->getVelocidadMedia(), 2.) / 2. / Cp;

		// Inicializaci�n del transporte de especies qu�micas.
		FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
		for (int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
			FFraccionMasicaEspecie[i] = FTuboRotor->GetFraccionMasicaInicial(i);
		}

		break;
	case nmPlenum:
		FTemperatura10 = FDeposito->getTemperature() + 273.;

		// Inicializaci�n del transporte de especies qu�micas.
		FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
		for (int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
			FFraccionMasicaEspecie[i] = FDeposito->GetFraccionMasicaEspecie(i);
		}
		break;
	}

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompTubDep::DatosEntradaCompresor(double AmbientTemperature,
	double AmbientPressure, TCondicionContorno *BC) {
	try {
		double pentcomp = 0., tentcomp = 0., ventcomp = 0.;
		double RMezclaEnt, RMezclaSal, GammaEnt, GammaSal, Cp;

		switch (FEntradaCompresor) {
		case nmAtmosphere:
			// Calculo de Gamma y R en el compresor. Media de valores a la entrada y la salida.
			RMezclaSal = BC->GetTuboExtremo(0).Pipe->GetRMezcla
				(FNodoFinTuboSalida);
			GammaSal = BC->GetTuboExtremo(0).Pipe->GetGamma(FNodoFinTuboSalida);

			FGamma = (GammaSal + FGammaAtm) / 2.;
			FRMezcla = (RMezclaSal + FRAtm) / 2.;
			FGamma1 = Gamma1(FGamma);
			FGamma3 = Gamma3(FGamma);
			FGamma4 = Gamma4(FGamma);
			FGamma5 = Gamma5(FGamma);

			FPresion10 = AmbientPressure;
			FTemperatura10 = AmbientTemperature + 273.;
			break;
		case nmPlenum:
			// Calculo de Gamma y R en el compresor. Media de valores a la entrada y la salida.
			RMezclaEnt = FDeposito->getR();
			GammaEnt = FDeposito->getGamma();

			RMezclaSal = BC->GetTuboExtremo(0).Pipe->GetRMezcla
				(FNodoFinTuboSalida);
			GammaSal = BC->GetTuboExtremo(0).Pipe->GetGamma(FNodoFinTuboSalida);
			if (GammaSal < 1.3) {
				GammaSal = 1.4;
			}
			FGamma = (GammaSal + GammaEnt) / 2.;
			FRMezcla = (RMezclaSal + RMezclaEnt) / 2.;
			FGamma1 = Gamma1(FGamma);
			FGamma3 = Gamma3(FGamma);
			FGamma4 = Gamma4(FGamma);
			FGamma5 = Gamma5(FGamma);

			FPresion10 = FDeposito->getPressure();
			FTemperatura10 = FDeposito->getTemperature() + 273.;
			break;
		case nmPipe:

			// Calculo de Gamma y R en el compresor. Media de valores a la entrada y la salida.
			RMezclaEnt = FTuboRotor->GetRMezcla(FNodoFinEntrada);
			GammaEnt = FTuboRotor->GetGamma(FNodoFinEntrada);

			RMezclaSal = BC->GetTuboExtremo(0).Pipe->GetRMezcla
				(FNodoFinTuboSalida);
			GammaSal = BC->GetTuboExtremo(0).Pipe->GetGamma(FNodoFinTuboSalida);

			FGamma = (GammaSal + GammaEnt) / 2.;
			FRMezcla = (RMezclaSal + RMezclaEnt) / 2.;
			FGamma1 = Gamma1(FGamma);
			FGamma3 = Gamma3(FGamma);
			FGamma4 = Gamma4(FGamma);
			FGamma5 = Gamma5(FGamma);

			if (FExtremoTuboRotor == nmIzquierda) {
				pentcomp = FTuboRotor->GetPresion(0) * 1e5;
				ventcomp = FTuboRotor->GetVelocidad(0) * ARef;
				tentcomp = pow(FTuboRotor->GetAsonido(0) * ARef, 2.)
					/ FGamma / FRMezcla;
				Cp = (FTuboRotor->GetGamma(0) * FTuboRotor->GetRMezcla(0)) /
					(FTuboRotor->GetGamma(0) - 1);
			}
			else {
				pentcomp =
					FTuboRotor->GetPresion(FTuboRotor->getNin() - 1) * 1e5;
				ventcomp =
					FTuboRotor->GetVelocidad(FTuboRotor->getNin() - 1) * ARef;
				tentcomp =
					pow(FTuboRotor->GetAsonido(FTuboRotor->getNin() - 1) * ARef,
					2.) / FGamma / FRMezcla;
				Cp = (FTuboRotor->GetGamma(FTuboRotor->getNin() - 1)
					* FTuboRotor->GetRMezcla(FTuboRotor->getNin() - 1)) /
					(FTuboRotor->GetGamma(FTuboRotor->getNin() - 1) - 1);
			}
			FTemperatura10 = tentcomp + pow(ventcomp, 2.) / 2. / Cp;
			FPresion10 = pentcomp * pow((FTemperatura10 / tentcomp),
				(FGamma / FGamma1));
			break;
		}

	}
	catch (Exception & N) {
		std::cout << "ERROR: DatosEntradaCompresor en el compresor: " <<
			FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception("ERROR: DatosEntradaCompresor en el compresor: " +
			AnsiString(FNumeroCompresor) + N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#pragma package(smart_init)
