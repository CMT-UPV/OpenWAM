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
#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
//#include <cmath>

#include "TCompTubos.h"
#include "TCondicionContorno.h"
#include "TTubo.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCompTubos::TCompTubos(int i, nmTipoCalculoEspecies SpeciesModel, int numeroespecies, nmCalculoGamma GammaCalculation,
					   bool ThereIsEGR) :
	TCompresor(i, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {
	FModeloCompresor = nmCompPipes;
	Mapa2T = new TMapaComp2Tub(FNumeroCompresor);
	FPrimerCiclo = true;
	FCoefPresiones = 0.10344;
	FGastoOut = 0.;
	FFlowIn = 0;
	FFlowOut = 0;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCompTubos::~TCompTubos() {

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompTubos::CondicionCompresor(double Theta, stTuboExtremo *TuboExtremo, double TiempoActual, int TuboCalculado) {

	double ErrorVelMax = 0., ErrorVelMin = 0., ErrorCheck = 0., RC = 0., RD = 0., CP = 0.;
	double temp = 0.;
	double CoefPresiones_anterior, Rendimiento_ant = 0.;
	double FraccionMasicaAcum = 0.;
	double vout = 0., aaout = 0.;

	try {
		// FContadorCheckSentido=0;
		CoefPresiones_anterior = FCoefPresiones;
		Rendimiento_ant = FRendimiento;

		FGamma = (FTuboRot->GetGamma(FNodoFinRotor) + FTuboEst->GetGamma(FNodoFinStator)) / 2.;
		// Se ha de hacer una media de entrada y salida
		FRMezcla = (FTuboRot->GetRMezcla(FNodoFinRotor) + FTuboEst->GetRMezcla(FNodoFinStator)) / 2.;
		FCpMezcla = FGamma * FRMezcla / (FGamma - 1);
		FGamma1 = __Gamma::G1(FGamma);
		FGamma2 = __Gamma::G2(FGamma);
		FGamma3 = __Gamma::G3(FGamma);
		FGamma4 = __Gamma::G4(FGamma);
		FGamma5 = __Gamma::G5(FGamma);

		FDeltaTiempo = TiempoActual - FTiempo0;
		FTiempo0 = TiempoActual;

		if(FPrimerCiclo) {

			double vR = FSignoRotor * (TuboExtremo[FPosicionRotor].Landa - TuboExtremo[FPosicionRotor].Beta) / FGamma3 *
						__cons::ARef;
			double aR = (TuboExtremo[FPosicionRotor].Landa + TuboExtremo[FPosicionRotor].Beta) / 2. * __cons::ARef;
			double pR = __units::BarToPa(pow(aR / TuboExtremo[FPosicionRotor].Entropia / __cons::ARef, FGamma4));
			// double dR=FGamma*pR/aR/aR;

			double aS = (TuboExtremo[FPosicionEstator].Landa + TuboExtremo[FPosicionEstator].Beta) / 2. * __cons::ARef;
			double vS = FSignoStator * (TuboExtremo[FPosicionEstator].Landa - TuboExtremo[FPosicionEstator].Beta) / FGamma3 *
						__cons::ARef;
			double pS = __units::BarToPa(pow(aS / TuboExtremo[FPosicionEstator].Entropia / __cons::ARef, FGamma4));

			FAsonidoIn = aR;
			FAsonidoOut = aS;
			FVelocidadIn = vR;
			FVelocidadOut = vS;
			FAout = aS;
			FPreTotalOut = pS;

			FPresionDep = __units::PaToBar(pS);
			FAsonidoDep = aS;
			FTempDep = aS * aS / FGamma / FRMezcla;
			FVolumen = FLongitudCaract * FAreaStator;
			FMasaDep = pS / FRMezcla / FTempDep * FVolumen;

			FRendimiento = Mapa2T->EvaluaRendSplines(0.);
			FRelacionCompresion = pS * pow(1 + FGamma3 * vS * vS / aS / aS,
										   FGamma / FGamma1) / pR / pow(1 + FGamma3 * vR * vR / aR / aR, FGamma / FGamma1);
			FCoefPresiones = (pow(FRelacionCompresion, FGamma1 / FGamma) - 1) / FRendimiento;

			if(FExtremoRotor == nmLeft) {
				FCarCIn = &(TuboExtremo[FPosicionRotor].Beta);
				FAaIn = &(TuboExtremo[FPosicionRotor].Entropia);
				FCarDIn = &(TuboExtremo[FPosicionRotor].Landa);
			} else {
				FCarCIn = &(TuboExtremo[FPosicionRotor].Landa);
				FAaIn = &(TuboExtremo[FPosicionRotor].Entropia);
				FCarDIn = &(TuboExtremo[FPosicionRotor].Beta);
			}
			if(FExtremoStator == nmRight) {
				FCarCOut = &(TuboExtremo[FPosicionEstator].Landa);
				FAaOut = &(TuboExtremo[FPosicionEstator].Entropia);
				FCarDOut = &(TuboExtremo[FPosicionEstator].Beta);
			} else {
				FCarCOut = &(TuboExtremo[FPosicionEstator].Beta);
				FAaOut = &(TuboExtremo[FPosicionEstator].Entropia);
				FCarDOut = &(TuboExtremo[FPosicionEstator].Landa);
			}
			FAreaIn = FAreaRotor;
			FAreaOut = FAreaStator;
			FPrimerCiclo = false;
			FCoefPresiones = pow2(*FAaOut / *FAaIn) * pow(FRelacionCompresion, FGamma1 / FGamma) - 1;
		}

		if(TuboCalculado == 10000) {

			SolveInletBoundary(FA1in, FU1in, FA1out, FU1out);

			double AA1fin = 0.;

			if(!FFlujoDirecto) {
				AA1fin = FAsonidoDep / __cons::ARef * pow(FPresionDep, -FGamma5) * pow(FRelacionCompresion,
						 FGamma5) / sqrt(1 + FCoefPresiones);
			} else {
				AA1fin = *FAaIn;
			}
			FFlowIn = FGamma * __units::BarToPa(pow(FA1in * __cons::ARef / pow(AA1fin * __cons::ARef, FGamma),
													1 / FGamma3)) * FU1in * __cons::ARef * FAreaIn;

			// NewPropertiesInTheVolume();

			FAsonidoIn = FA1in * __cons::ARef;
			FVelocidadIn = FU1in * __cons::ARef;

			SolveOutletBoundary(FA2, FU2);

			double AA2fin = 0.;
			if(FFlowOut < 0) {
				AA2fin = FAsonidoDep / __cons::ARef * pow(FPresionDep, -FGamma5);
			} else {
				AA2fin = *FAaOut;
			}

			NewPropertiesInTheVolume();
			FAsonidoOut = FA2 * __cons::ARef;
			FVelocidadOut = FU2 * __cons::ARef;
			// <--- NUEVO MODELO

			*FCarDIn = (FAsonidoIn - FGamma1 / 2. * FVelocidadIn) / __cons::ARef;
			if(FVelocidadIn < 0) {
				*FAaIn = AA1fin;
			}
			*FCarDOut = (FAsonidoOut + FGamma1 / 2. * FVelocidadOut) / __cons::ARef;
			if(FVelocidadOut > 0) {
				*FAaOut = AA2fin;

			}

		} else if(TuboCalculado == FPosicionRotor) {
			// NUEVO MODELO ---->
			SolveInletBoundary(FA1in, FU1in, FA1out, FU1out);

			double AA1fin = 0.;

			if(!FFlujoDirecto) {
				AA1fin = FAsonidoDep / __cons::ARef * pow(FPresionDep, -FGamma5) * pow(FRelacionCompresion,
						 FGamma5) / sqrt(1 + FCoefPresiones);
			} else {
				AA1fin = *FAaIn;
			}
			FFlowIn = FGamma * __units::BarToPa(pow(FA1in * __cons::ARef / pow(AA1fin * __cons::ARef, FGamma),
													1 / FGamma3)) * FU1in * __cons::ARef * FAreaIn;

			NewPropertiesInTheVolume();

			FAsonidoIn = FA1in * __cons::ARef;
			FVelocidadIn = FU1in * __cons::ARef;
			// <--- NUEVO MODELO

			*FCarDIn = (FAsonidoIn - FGamma1 / 2. * FVelocidadIn) / __cons::ARef;
			if(FVelocidadIn < 0) {
				*FAaIn = AA1fin;
			}
		} else {
			// NUEVO MODELO ---->
			SolveOutletBoundary(FA2, FU2);

			double AA2fin = 0.;
			if(FFlowOut < 0) {
				AA2fin = FAsonidoDep / __cons::ARef * pow(FPresionDep, -FGamma5);
			} else {
				AA2fin = *FAaOut;
			}

			NewPropertiesInTheVolume();
			FAsonidoOut = FA2 * __cons::ARef;
			FVelocidadOut = FU2 * __cons::ARef;
			// <--- NUEVO MODELO

			*FCarDOut = (FAsonidoOut + FGamma1 / 2. * FVelocidadOut) / __cons::ARef;
			if(FVelocidadOut > 0) {
				*FAaOut = AA2fin;
			}

		}

		FGasto1 = FGamma * __units::BarToPa(pow(FAsonidoIn / pow(*FAaIn * __cons::ARef, FGamma),
												1 / FGamma3)) * FVelocidadIn * FAreaIn;

		FTempIn = FAsonidoIn * FAsonidoIn / (FGamma * FRMezcla);
		FTempTotalIn = FTempIn + FVelocidadIn * FVelocidadIn / 2 / FCpMezcla;
		FPresionIn = __units::BarToPa(pow(*FAaIn * __cons::ARef / FAsonidoIn, -FGamma4));
		FPreTotalIn = FPresionIn * pow(FTempTotalIn / FTempIn, FGamma / FGamma1);
		FTempOut = FAsonidoOut * FAsonidoOut / (FGamma * FRMezcla);
		FTempTotalOut = FTempOut + FVelocidadOut * FVelocidadOut / 2 / FCpMezcla;
		FPresionOut = __units::BarToPa(pow(*FAaOut * __cons::ARef / FAsonidoOut, -FGamma4));
		FPreTotalOut = FPresionOut * pow(FTempTotalOut / FTempOut, FGamma / FGamma1);

		FGastoCorregido = FGasto1 * sqrt(FTempTotalIn / Mapa2T->getTempRef()) / (FPreTotalIn / Mapa2T->getPresionRef());

		// Busqueda de relacion de compresion en el compresor dado por el mapa
		if(FGastoCorregido < -0.07) {
			RC = Mapa2T->EvaluaRCHermite(Mapa2T->getGastoBombeo());
		} else if(FGastoCorregido >= -0.07 && FGastoCorregido < Mapa2T->getGastoBombeo()) {
			RC = Mapa2T->EvaluaRCHermite(FGastoCorregido);
		} else if(FGastoCorregido >= Mapa2T->getGastoBombeo() && FGastoCorregido <= Mapa2T->getGastoRelComp1()) {
			RC = Mapa2T->EvaluaRCHermite(FGastoCorregido);
		} else if(FGastoCorregido > Mapa2T->getGastoRelComp1()) {
			RC = Mapa2T->EvaluaRCHermite(Mapa2T->getGastoRelComp1());
		}

		if(RC < 1.)
			RC = 1.;
		FRelacionCompresion = FRelacionCompresion + (1 / FLongitudCaract) * (RC - FRelacionCompresion) * 0.5 * fabs(
								  FVelocidadIn + FVelocidadOut) * FDeltaTiempo;

		// Busqueda del rendimiento en el compresor dado por el mapa
		if(FGastoCorregido < -0.07) {
			RD = Mapa2T->EvaluaRendSplines(Mapa2T->getGastoBombeo());
		} else if(FGastoCorregido >= -0.07 && FGastoCorregido < Mapa2T->getGastoBombeo()) {
			RD = Mapa2T->EvaluaRendSplines(Mapa2T->getGastoBombeo());
		} else if(FGastoCorregido >= Mapa2T->getGastoBombeo() && FGastoCorregido <= Mapa2T->getGastoRelComp1()) {
			RD = Mapa2T->EvaluaRendSplines(FGastoCorregido);
		} else if(FGastoCorregido > Mapa2T->getGastoRelComp1()) {
			RD = Mapa2T->EvaluaRendSplines(Mapa2T->getGastoRelComp1());
		}

		if(RD < 0.4)
			RD = 0.4;

		FRendimiento = RD;

		FCoefPresiones = (pow(FRelacionCompresion, FGamma1 / FGamma) - 1) / FRendimiento;

		if(FDeltaTiempo > 0) {

			FTrabajo = (FTempTotalOut - FTempTotalIn) * FRMezcla * FGamma4 / 2. * fabs(FGasto1) * FDeltaTiempo;
			FPotencia = FTrabajo / FDeltaTiempo;

			FTrabajoPaso += FTrabajo;
			FDeltaTPaso += FDeltaTiempo;

			FGastoCompresor = FGasto1;
			FTemperatura10 = FTempTotalIn;
		}

		FRegimenCorregido = Mapa2T->getRegimenCorregido();
	} catch(exception & N) {
		std::cout << "ERROR: CalculaCondicionContorno en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: CalculaCondicionContorno en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompTubos::Biseccion(double *VelIn, double *VelOut, double *AIn, double *AOut, double CarIn, double AaIn,
						   double CarOut, int sig) {
	try {
		bool Primera = true;
		FVelInMin = 0;
		FVelInMax = (double) sig * 2 * __cons::ARef * CarIn / FGamma2;

		FErrorVelIn = 10000.;
		FErrorCarIn = 1e-6;
		FCuentaVelIn = 0.;

		while(fabs(FErrorVelIn) > FErrorCarIn && FCuentaVelIn < 200) {
			*VelIn = (FVelInMin + FVelInMax) / 2.;

			*AIn = __cons::ARef * CarIn - (double) sig * 0.5 * FGamma1 * *VelIn;
			FPresionIn = __units::BarToPa(pow(AaIn / *AIn, -FGamma4));
			FTempIn = pow2(*AIn) / FGamma / FRMezcla;
			FDensidadIn = FPresionIn / (FRMezcla * FTempIn);
			FTempTotalIn = FTempIn + pow2(*VelIn) / 2. / FCpMezcla;
			FPreTotalIn = FPresionIn * pow(FTempTotalIn / FTempIn, FGamma / FGamma1);
			FGasto1 = FSentidoFlujo * FAreaIn * FDensidadIn * *VelIn;

			FPreTotalOut = FPreTotalIn * pow(FRelacionCompresion, FSentidoFlujo);

			if(FFlujoDirecto) {
				FTempTotalOut = FTempTotalIn * (1 + FCoefPresiones);
			} else if(!FPrimerCiclo) {
				FTempTotalOut = FTempTotalInAnt; // andres
				FTempTotalIn = FTempTotalOut;
			} else if(FPrimerCiclo) {
				FTempTotalOut = FTempTotalInAnt;
				FTempTotalIn = FTempTotalOutAnt;
				FPrimerCiclo = false;
			}

			FVelOutMin = 0.;
			FVelOutMax = (double) sig * 2 * __cons::ARef * CarOut / (3 - FGamma);

			if(FVelOutMax < -sqrt(2 * FCpMezcla * FTempTotalOut)) {
				FVelOutMax = -sqrt(2 * FCpMezcla * FTempTotalOut);
			}
			if(FVelOutMax < -sqrt(2 * FRMezcla * FGamma * FTempTotalOut / FGamma2)) {
				FVelOutMax = -sqrt(2 * FRMezcla * FGamma * FTempTotalOut / FGamma2);
			}
			FCuentaVelOut = 0;
			FErrorVelOut = 1000.;

			while(fabs(FErrorVelOut) > FErrorCarOut && FCuentaVelOut < 1050.) {
				*VelOut = (FVelOutMin + FVelOutMax) / 2.;
				++FCuentaVelOut;
				FDensidadOut = fabs(FGasto1 / (FAreaOut * *VelOut));
				FTempOut = FTempTotalOut - pow2(*VelOut) / 2. / FCpMezcla;
				FPresionOut = FDensidadOut * FTempOut * FRMezcla;
				FErrorVelOut = FPreTotalOut - FPresionOut * pow(FTempTotalOut / FTempOut, FGamma / FGamma1);
				if(FErrorVelOut > 0)
					FVelOutMax = *VelOut;
				else
					FVelOutMin = *VelOut;
			}
			*AOut = sqrt(FGamma * FRMezcla * FTempOut);
			FErrorVelIn = CarOut - (*AOut - (double) sig * *VelOut * FGamma1 / 2) / __cons::ARef;

			if(FErrorVelIn > 0)
				FVelInMax = *VelIn;
			else
				FVelInMin = *VelIn;

			++FCuentaVelIn;
		}

	} catch(exception & N) {
		std::cout << "ERROR: BuscaErrorCaracteristica en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: BuscaErrorCaracteristica en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompTubos::LeeCompresor(const char *FileWAM, fpos_t &filepos) {
	int tipo = 0;
	try {

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d %d ", &FTuboRotor, &FTuboStator);
		fscanf(fich, "%lf %lf %lf %lf", &FRadioTip, &FRadioHub, &FRadioRodete, &FLongitudCaract);

		int format = 0;
		fscanf(fich, "%d ", &format);
		if(format == 1) {
			std::cout << "SAE Compressor map format is not compatible with two pipes compressor model" << endl;

			cout << "Press anykey to exit.";
			cin.ignore();

			exit(0);
		}
		Mapa2T->PutRadioHub(FRadioHub);
		Mapa2T->PutRadioRadioTip(FRadioTip);
		Mapa2T->PutRadioRodete(FRadioRodete);

		Mapa2T->LeeMapa(fich);

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: LeeCompresor en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: LeeCompresor en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompTubos::RelacionTubos(TCondicionContorno **BC, int NumeroCC) {
	try {
		double Cp = 0.;

		for(int i = 0; i < 2; i++) {
			if(FTuboRotor == BC[NumeroCC - 1]->GetTuboExtremo(i).Pipe->getNumeroTubo()) {
				FTuboRot = BC[NumeroCC - 1]->GetTuboExtremo(i).Pipe;
				FPosicionRotor = i;
			} else if(FTuboStator == BC[NumeroCC - 1]->GetTuboExtremo(i).Pipe->getNumeroTubo()) {
				FTuboEst = BC[NumeroCC - 1]->GetTuboExtremo(i).Pipe;
				FPosicionEstator = i;
			}
		}

		if(FTuboRot->getNodoIzq() == NumeroCC) {
			FExtremoRotor = nmLeft;
			FSignoRotor = -1;
			FNodoFinRotor = 0;
			// FIndExtRotor=0;
			FAreaRotor = __geom::Circle_area(FTuboRot->GetDiametro(0));
			FIndiceCCRotor = 0;

		} else if(FTuboRot->getNodoDer() == NumeroCC) {
			FExtremoRotor = nmRight;
			FNodoFinRotor = FTuboRot->getNin() - 1;
			FSignoRotor = 1;
			// FIndExtRotor=1;
			FAreaRotor = __geom::Circle_area(FTuboRot->GetDiametro(FTuboRot->getNin() - 1));
			FIndiceCCRotor = 1;
		} else {
			std::cout << "ERROR: Asigncion tubo entrada compresor " << FNumeroCompresor << std::endl;
		}
		if(FTuboEst->getNodoIzq() == NumeroCC) {
			FExtremoStator = nmLeft;
			FNodoFinStator = 0;
			FSignoStator = -1;
			// FIndExtStator=0;
			FAreaStator = __geom::Circle_area(FTuboEst->GetDiametro(0));
			FIndiceCCStator = 0;
		} else if(FTuboEst->getNodoDer() == NumeroCC) {
			FExtremoStator = nmRight;
			FNodoFinStator = FTuboEst->getNin() - 1;
			FSignoStator = 1;
			// FIndExtStator=1;
			FAreaStator = __geom::Circle_area(FTuboEst->GetDiametro(FTuboEst->getNin() - 1));
			FIndiceCCStator = 1;
		} else {
			std::cout << "ERROR: Asignacion tubo salida compresor " << FNumeroCompresor << std::endl;
		}
		if(FExtremoRotor == nmLeft) {
			Cp = (FTuboRot->GetGamma(0) * FTuboRot->GetRMezcla(0)) / (FTuboRot->GetGamma(0) - 1);
		} else
			Cp = (FTuboRot->GetGamma(FTuboRot->getNin() - 1) * FTuboRot->GetRMezcla(FTuboRot->getNin() - 1)) / (FTuboRot->GetGamma(
						FTuboRot->getNin() - 1) - 1);

		FTemperatura10 = __units::degCToK(FTuboRot->getTemperaturaInicial()) + pow2(FTuboRot->getVelocidadMedia()) / 2. / Cp;

		// Inicializacion del transporte de especies quimicas.
		FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
		for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
			FFraccionMasicaEspecie[i] = FTuboRot->GetFraccionMasicaInicial(i);
		}

	} catch(exception & N) {
		std::cout << "ERROR: TCompTubos::RelacionTubos en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: LeeCompresor en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompTubos::MetodoNewton2D(double *a1, double *a2, double *u1, double *u2, double aa1, double aa2, double cc1,
								double cc2, double s1, double s2, double k, int sig) {
	try {
		double f1 = 0., f2 = 0., J11 = 0., J12 = 0., J22 = 0., J21 = 0., DetJ = 0., da1 = 0., da2 = 0., Error = 0.;
		double a10 = 0., a20 = 0., v10 = 0., v20 = 0., Lim_u1 = 0., Lim_u2 = 0., aa2new = 0.;
		bool biseccion = false;
		int cont = 0;

		/* Pedro */
		double a1_local = 0., a2_local = 0., u1_local = 0., u2_local = 0.;

		a1_local = *a1;
		a2_local = *a2;
		u1_local = *u1;
		u2_local = *u2;

		a10 = *a1;
		a20 = *a2;

		Lim_u1 = (double) sig * 2 * __cons::ARef * cc1 / FGamma2;
		Lim_u2 = (double) sig * 2 * __cons::ARef * cc2 / FGamma2;

		// aa2new=sqrt(1+FCoefPresiones)* aa1/pow(FRelacionCompresion,sig*FGamma5);

		aa2new = aa2;
		// k_local=k;

		if(fabs(*u1 / *a1) < 0.7 && fabs(*u2 / *a2) < 0.7) {

			do {
				f1 = FGamma * __units::BarToPa(pow(*a1 / pow(aa1, FGamma), 1 / FGamma3) * *u1 * s1 - pow(*a2 / pow(aa2new, FGamma),
											   1 / FGamma3) * *u2 * s2);
				f2 = (*a2 * *a2 + FGamma3 * *u2 * *u2) / aa2 / aa2 - (*a1 * *a1 + FGamma3 * *u1 * *u1) / aa1 / aa1 * pow(
						 FRelacionCompresion, sig * FGamma1 / FGamma);

				J11 = FGamma * 2 * s1 / FGamma1 * __units::BarToPa(pow(*a1 / pow(aa1, FGamma),
						1 / FGamma3)) * (*u1 / *a1 - (double) sig);
				J12 = -FGamma * 2 * s2 / FGamma1 * __units::BarToPa(pow(*a2 / pow(aa2new, FGamma),
						1 / FGamma3)) * (*u2 / *a1 + (double) sig);
				J21 = -2 * pow(FRelacionCompresion, (sig * FGamma1 / FGamma)) / aa1 / aa1 * (*a1 - (double) sig * *u1);
				J22 = 2 / aa2 / aa2 * (*a2 - (double) sig * *u2);

				DetJ = J11 * J22 - J12 * J21;

				if(fabs(DetJ) < 1e-10) {
					da1 = 0;
					da2 = *a2 - *a1;
				} else {
					da1 = (J22 * f1 - J12 * f2) / DetJ;
					da2 = (-J21 * f1 + J11 * f2) / DetJ;
				}

				*a1 = *a1 - da1;
				*a2 = *a2 - da2;

				*u1 = (double) sig * (cc1 * __cons::ARef - *a1) / FGamma3;
				*u2 = -(double) sig * (cc2 * __cons::ARef - *a2) / FGamma3;

				if(fabs(*u1) > *a1)
					*u1 = *a1 * *u1 / fabs(*u1);
				if(fabs(*u2) > *a2)
					*u2 = *a2 * *u2 / fabs(*u2);

				Error = sqrt(pow2(da1 / (*a1 + da1)) + pow2(da2 / (*a2 + da2)));

				cont++;
			} while(Error > 1e-12 && cont < 5000);

			if(cont == 5000 || *u1 * *u2 < 0) {
				biseccion = true;
			} else {
				// FPresionOut=1e5*pow(aa2new/ *a2,-FGamma4);
				FTempIn = *a1 * *a1 / FGamma / FRMezcla;
				FTempTotalIn = FTempIn + pow2(*u1) / 2. / FCpMezcla;
				FPresionIn = __units::BarToPa(pow(aa1 / *a1, -FGamma4));
				FPreTotalIn = FPresionIn * pow(FTempTotalIn / FTempIn, FGamma / FGamma1);
				FPreTotalOut = FPreTotalIn * pow(FRelacionCompresion, FSentidoFlujo);
				FPresionOut = FPreTotalOut / pow(1 + FGamma3 * pow2(*u2 / *a2), FGamma / FGamma1);
				FTempTotalOut = 1 / FGamma / FRMezcla * (*a2 * *a2 + FGamma3 * *u2 * *u2);
			}
		} else {
			biseccion = true;
		}

		if(biseccion) {
			Biseccion(&u1_local, &u2_local, &a1_local, &a2_local, cc1, aa1, cc2, sig);
			*u1 = u1_local;
			*a1 = a1_local;
			*u2 = u2_local;
			*a2 = a2_local;
		}

	} catch(exception & N) {
		std::cout << "ERROR: MetodoNewton2D en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: LeeCompresor en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

void TCompTubos::Solver(double *a1, double *a2, double *u1, double *u2, double aa1, double aa2, double cc1, double cc2,
						double s1, double s2, double k, int sig) {
	try {

		stCompSolverA1 FunA1(cc1, cc2, s1, s2, FGamma, aa1, aa2, FRelacionCompresion, FRendimiento, FLongitudCaract,
							 FCoefPresiones, FDeltaTiempo);

		double X1 = cc1 * __cons::ARef - (FGamma - 1) / 2 * (*u1 + 5);
		double X2 = cc1 * __cons::ARef - (FGamma - 1) / 2 * (*u1 - 5);

		double X1lim = cc1 * __cons::ARef * 2 / (FGamma + 1);
		double X2lim = cc1 * __cons::ARef * 2 / (3 - FGamma);

		if(zbrac2(FunA1, X1, X2, X1lim, X2lim)) {
			*a1 = FindRoot(FunA1, X1, X2);
			*a2 = FunA1.A2;
			*u1 = FunA1.V1;
			*u2 = FunA1.V2;
			FCoefPresiones = FunA1.CPfin;
		}
	} catch(exception & N) {
		std::cout << "ERROR: MetodoNewton2D en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: LeeCompresor en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompTubos::ExtremoCerrado() {
	try {

		FVelocidadIn = 0.;
		FVelocidadOut = 0.;
		FGasto1 = 0.;
		*FCarDIn = *FCarCIn;
		*FCarDOut = *FCarCOut;

	} catch(exception & N) {
		std::cout << "ERROR: TCompTubos::ExtremoCerrado en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: TCompTubos::ExtremoCerrado en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCompTubos::SolveInletBoundary(double &A, double &U, double &Ao, double &Uo) {
	double K1 = 0., K2 = 0., K3 = 0.;

	double A_dep = pow(FPresionDep, FGamma5);
	double A_pipe = *FCarCIn / *FAaIn * pow(FRelacionCompresion, FGamma5);

	K1 = FGamma2 / FGamma1;
	K2 = -4 / FGamma1 * *FCarCIn;

	if(A_dep / A_pipe > 1 + 1e-10) {
		FFlujoDirecto = false;

		K3 = 2 * pow2(*FCarCIn) / FGamma1 - pow2(FAsonidoDep / __cons::ARef) / (1 + FCoefPresiones);

	} else if(A_dep / A_pipe < 1 - 1e-10) {
		FFlujoDirecto = true;

		K3 = 2 * pow2(*FCarCIn) / FGamma1 - pow(FPresionDep / FRelacionCompresion, FGamma1 / FGamma) * pow2(*FAaIn);
	} else {
		FFlujoDirecto = true;

		U = 0;
		A = *FCarCIn;
		return;
	}

	A = QuadraticEqP(K1, K2, K3);
	U = (*FCarCIn - A) / FGamma3;

	if(FFlujoDirecto) {

		stCompOut FCompOut(A, U, FRelacionCompresion, FCoefPresiones, FGamma);

		double Uo = U;

		double Lim1 = Uo / fabs(Uo) * 1e-10;
		double Lim2 = Uo + Uo / fabs(Uo) * 1e-3;

		if(Lim1 * Lim2 < 0)
			printf("cosa rara");

		if(zbrac(FCompOut, Lim1, Lim2)) {
			Uo = FindRoot(FCompOut, Lim1, Lim2);
		}
		Ao = pow(FCompOut.A1outA, FGamma3);
	}
}

void TCompTubos::SolveOutletBoundary(double &A, double &U) {
	double Pref = 1;

	double Ad = pow(FPresionDep / Pref, 1. / FGamma4);
	double rel_CCon_Entropia = *FCarCOut / *FAaOut;

	if(rel_CCon_Entropia / Ad > 1 + 1e-10) {   // Flujo entrante al deposito
		InFlow(Ad, A, U);

		double xaa2 = pow(*FAaOut, FGamma4);
		FFlowOut = -FGamma * FAreaOut * __units::BarToPa(pow(A, 1 / FGamma3)) * U / (__cons::ARef * xaa2);
		// Massflow entrante al deposito negativo

	} else if(rel_CCon_Entropia / Ad < 1 - 1e-10) {
		// Flujo saliente del deposito
		OutFlow(Ad, A, U);

		FFlowOut = -FAreaOut * FGamma * pow(Ad * __cons::ARef / FAsonidoDep, FGamma4) * U * __units::BarToPa(pow(A,
				   2. / FGamma1)) / __cons::ARef;

	} else { // Flujo Parado
		U = 0;
		A = *FCarCOut;
		FFlowOut = 0.;
	}

}

void TCompTubos::NewPropertiesInTheVolume() {
	double H = 0.;

	double MasaIn = FFlowIn * FDeltaTiempo;
	double MasaOut = FFlowOut * FDeltaTiempo;

	if(FFlowIn > 0) {
		H += EntalpiaEntrada(FA1out, FU1out, MasaIn, FAsonidoDep / __cons::ARef, FMasaDep, FGamma);
	}
	if(FFlowOut > 0) {
		H += EntalpiaEntrada(FA2, FU2, MasaOut, FAsonidoDep / __cons::ARef, FMasaDep, FGamma);
	}
	double MasaDep0 = FMasaDep;
	FMasaDep = FMasaDep + MasaIn + MasaOut;

	double Energia = pow(FMasaDep / MasaDep0 * exp(H), FGamma1);
	FAsonidoDep *= sqrt(Energia);
	double A2 = FAsonidoDep * FAsonidoDep;
	FPresionDep = __units::PaToBar(A2 / FGamma / FVolumen * FMasaDep);
	FTempDep = __units::KTodegC(A2 / FGamma / FRMezcla);

}

void TCompTubos::InFlow(double Ad, double &A, double &U) {
	double AThroat = *FAaOut * Ad;
	nmCaso Caso = nmFlujoEntranteSaltoSubcritico;

	A = AThroat;
	U = -(*FCarCOut - A) / FGamma3;

	double UThroat = U;

	if(UThroat > AThroat) {

	} else {

	}
}

void TCompTubos::OutFlow(double Ad, double &A, double &U) {
	// Critical conditions

	double U2cr = FAsonidoDep * sqrt(2. / FGamma2) * (sqrt(1 + FGamma1 * FGamma2) - 1) / FGamma1;
	double A2cr = sqrt(pow2(FAsonidoDep) - FGamma3 * pow2(U2cr));

	stFSSub FSA2(*FAaOut, Ad, FGamma, 1, *FCarCOut, FAsonidoDep / __cons::ARef);

	double error = FSA2(A2cr / __cons::ARef);

	if(error < 0.) {   // Salto de presiones supercritico.
	} else {
		A = FindRoot(FSA2, A2cr / __cons::ARef, FAsonidoDep / __cons::ARef);
		U = FSA2.U2;
	}

}

double TCompTubos::EntalpiaEntrada(double ASonidoE, double VelocidadE, double MasaE, double ASonidoD, double MasaD,
								   double Gamma) {

	double xx = 0., yy = 0., ret_val = 0.;

	if(fabs(MasaE) != 0.) {
		xx = (pow2(ASonidoE / ASonidoD) - 1.) / __Gamma::G1(Gamma);
		yy = pow2(VelocidadE / ASonidoD) / 2.;
		ret_val = Gamma * MasaE * (xx + yy) / MasaD;
	} else {
		ret_val = 0.;
	}
	return ret_val;

}

void TCompTubos::AsignPipes(TCondicionContorno **BC, int NumeroCC) {

	for(int i = 0; i < 2; i++) {
		if(FTuboRotor == BC[NumeroCC - 1]->GetTuboExtremo(i).Pipe->getNumeroTubo()) {
			FTuboRot = BC[NumeroCC - 1]->GetTuboExtremo(i).Pipe;
			FPosicionRotor = i;
		} else if(FTuboStator == BC[NumeroCC - 1]->GetTuboExtremo(i).Pipe->getNumeroTubo()) {
			FTuboEst = BC[NumeroCC - 1]->GetTuboExtremo(i).Pipe;
			FPosicionEstator = i;
		}
	}

	if(FTuboRot->getNodoIzq() == NumeroCC) {
		FExtremoRotor = nmLeft;
		FSignoRotor = -1;
		FNodoFinRotor = 0;
		// FIndExtRotor=0;
		FAreaRotor = __geom::Circle_area(FTuboRot->GetDiametro(0));
		FIndiceCCRotor = 0;

	} else if(FTuboRot->getNodoDer() == NumeroCC) {
		FExtremoRotor = nmRight;
		FNodoFinRotor = FTuboRot->getNin() - 1;
		FSignoRotor = 1;
		// FIndExtRotor=1;
		FAreaRotor = __geom::Circle_area(FTuboRot->GetDiametro(FTuboRot->getNin() - 1));
		FIndiceCCRotor = 1;
	} else {
		std::cout << "ERROR: Asigncion tubo entrada compresor " << FNumeroCompresor << std::endl;
	}
	if(FTuboEst->getNodoIzq() == NumeroCC) {
		FExtremoStator = nmLeft;
		FNodoFinStator = 0;
		FSignoStator = -1;
		// FIndExtStator=0;
		FAreaStator = __geom::Circle_area(FTuboEst->GetDiametro(0));
		FIndiceCCStator = 0;
	} else if(FTuboEst->getNodoDer() == NumeroCC) {
		FExtremoStator = nmRight;
		FNodoFinStator = FTuboEst->getNin() - 1;
		FSignoStator = 1;
		// FIndExtStator=1;
		FAreaStator = __geom::Circle_area(FTuboEst->GetDiametro(FTuboEst->getNin() - 1));
		FIndiceCCStator = 1;
	} else {
		std::cout << "ERROR: Asignacion tubo salida compresor " << FNumeroCompresor << std::endl;
	}

}

void TCompTubos::Initialize() {

	double Cp = 0.;

	if(FExtremoRotor == nmLeft) {
		Cp = (FTuboRot->GetGamma(0) * FTuboRot->GetRMezcla(0)) / (FTuboRot->GetGamma(0) - 1);
	} else
		Cp = (FTuboRot->GetGamma(FTuboRot->getNin() - 1) * FTuboRot->GetRMezcla(FTuboRot->getNin() - 1)) / (FTuboRot->GetGamma(
					FTuboRot->getNin() - 1) - 1);

	FTemperatura10 = __units::degCToK(FTuboRot->getTemperaturaInicial()) + pow2(FTuboRot->getVelocidadMedia()) / 2. / Cp;

	// Inicializacion del transporte de especies quimicas.
	FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
	for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
		FFraccionMasicaEspecie[i] = FTuboRot->GetFraccionMasicaInicial(i);
	}

}

#pragma package(smart_init)
