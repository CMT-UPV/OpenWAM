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

#include "TCCUnionEntreDepositos.h"
//#include <cmath>
#ifdef __BORLANDC__
#include <vcl.h>
#endif

#include "TCDFijo.h"
#include "TValvula4T.h"
#include "TLamina.h"
#include "TDiscoRotativo.h"
#include "TLumbrera.h"
#include "TValvulaContr.h"
#include "TWasteGate.h"
#include "TEstatorTurbina.h"
#include "TRotorTurbina.h"
#include "TCDExterno.h"

#include "TDepVolVariable.h"
#include "TDepVolCte.h"

#include "TTubo.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCCUnionEntreDepositos::TCCUnionEntreDepositos(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel,
		int numeroespecies, nmCalculoGamma GammaCalculation, bool ThereIsEGR) :
	TCondicionContorno(TipoCC, numCC, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {
	FValvula = NULL;
	FGasto = 0.;
	FGastoImpreso = 0.;
	FTiempoActual = 0.;

	FPresionDep1SUM = 0.;
	FAsonidoDep1SUM = 0.;
	FGammaDep1SUM = 0.;
	FRDep1SUM = 0.;
	FTiempoDep1SUM = 0.;
	FPresionDep2SUM = 0.;
	FAsonidoDep2SUM = 0.;
	FGammaDep2SUM = 0.;
	FRDep2SUM = 0.;
	FTiempoDep2SUM = 0.;

	FResInstantUED.Massflow = false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCCUnionEntreDepositos::~TCCUnionEntreDepositos() {

	if(FValvula != NULL)
		delete FValvula;

	FValvula = NULL;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCUnionEntreDepositos::LeeUEDepositos(const char *FileWAM, fpos_t &filepos, bool Independent) {
	try {
		int numid = 0; // Variable necesaria para WAMer.

		FIndependiente = Independent;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ",
			   &numid); // Esto es un dato que necesita el WAMer. Los usuarios de WAM hacemos la vista gorda hasta que se arregle.
		fscanf(fich, "%d ", &FNumeroDeposito1);
		fscanf(fich, "%d ", &FNumeroDeposito2);

		fgetpos(fich, &filepos);
		fclose(fich);

	} catch(exception &N) {
		std::cout << "ERROR: TCCUnionEntreDepositos::LeeNumDepositos en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void TCCUnionEntreDepositos::AsignaDepositos(TDeposito **Plenum) {
	try {

		FDeposito1 = Plenum[FNumeroDeposito1 - 1];
		FDeposito2 = Plenum[FNumeroDeposito2 - 1];

// Inicializacion del transporte de especies quimicas.
		FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
		for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
			FFraccionMasicaEspecie[i] = FDeposito1->GetFraccionMasicaEspecie(
											i); // Se inicializa con el Deposito1 de modo arbitrario.
		}

	} catch(exception &N) {
		std::cout << "ERROR: TCCUnionEntreDepositos::AsignaDepositos en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void TCCUnionEntreDepositos::AsignaTipoValvula(TTipoValvula **Origen, int Valv, int i) {
	try {

		switch(Origen[Valv - 1]->getTypeOfValve()) {
		case nmCDFijo:
			FValvula = new TCDFijo(dynamic_cast<TCDFijo *>(Origen[Valv - 1]), i);
			break;
		case nmValvula4T:
			FValvula = new TValvula4T(dynamic_cast<TValvula4T *>(Origen[Valv - 1]), i);
			break;
		case nmLamina:
			FValvula = new TLamina(dynamic_cast<TLamina *>(Origen[Valv - 1]), i);
			break;
		case nmDiscoRotativo:
			FValvula = new TDiscoRotativo(dynamic_cast<TDiscoRotativo *>(Origen[Valv - 1]), i);
			break;
		case nmLumbrera2T:
			FValvula = new TLumbrera(dynamic_cast<TLumbrera *>(Origen[Valv - 1]), i);
			break;
		case nmValvulaContr:
			FValvula = new TValvulaContr(dynamic_cast<TValvulaContr *>(Origen[Valv - 1]), i);
			break;
		case nmWasteGate:
			FValvula = new TWasteGate(dynamic_cast<TWasteGate *>(Origen[Valv - 1]), i);
			break;
		case nmStator:
			FValvula = new TEstatorTurbina(dynamic_cast<TEstatorTurbina *>(Origen[Valv - 1]), i);
			break;
		case nmRotor:
			FValvula = new TRotorTurbina(dynamic_cast<TRotorTurbina *>(Origen[Valv - 1]), i);
			break;
		case nmCalcExtern:
			FValvula = new TCDExterno(dynamic_cast<TCDExterno *>(Origen[Valv - 1]), i);
			break;
		}

		FValvula->PutDiametroTubo(FValvula->getDiametro());
		if(FValvula->getTypeOfValve() == nmCDFijo) {
			dynamic_cast<TCDFijo *>(FValvula)->CalculaCD();
			FCDEntrada = FValvula->getCDTubVol();
			FCDSalida = FValvula->getCDVolTub();
			FCTorbellino = FValvula->getCTorb();
			if(FCDEntrada > 1.) {
				FValvula->AsignaCRecuperacion(FCDEntrada - 1.);
			} else
				FValvula->AsignaCRecuperacion(0.);
		}

	} catch(exception &N) {
		std::cout << "ERROR: TCCUnionEntreDepositos::AsignaTipoValvula en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void TCCUnionEntreDepositos::CalculaCoeficientesDescarga(double TiempoActual, double mfcomb, double RegimenMotor) {
	try {

		double PVol = 0., PTub = 0., PAdm = 0., DeltaP = 0., distancia = 0., CDExt = 0., nodoadm = 0., p1 = 0., p2 = 0.;
		int turb = 0, entr = 0;

		if(FValvula->getTypeOfValve() == nmLumbrera2T) {
			FRegimen = dynamic_cast<TDepVolVariable*>(FDeposito1)->getRegimen();
			FTime0 = FTime1;
			FTime1 = TiempoActual;
			FDeltaT = FTime1 - FTime0;
			FDeltaAngulo = 360. * FRegimen * dynamic_cast<TDepVolVariable*>(FDeposito1)->getRelacionVelocidades() / 60. * FDeltaT;
			FAnguloAnterior = FAnguloActual;
			FAnguloActual = FAnguloAnterior + FDeltaAngulo;
			if(FAnguloActual > 360.) {
				FAnguloActual -= 360.;
			}
		}

		if(FValvula->getTypeOfValve() == nmDiscoRotativo) {
			if(FValvula->getControlRegimen() == nmMotor) {
				FRegimen = RegimenMotor;
			} else {
				FRegimen = FValvula->getRegimen();
			}
			FTime0 = FTime1;
			FTime1 = TiempoActual;
			FDeltaT = FTime1 - FTime0;
			FDeltaAngulo = 360. * FRegimen * FValvula->getRelacionVelocidades() / 60. * FDeltaT;
			FAnguloAnterior = FAnguloActual;
			FAnguloActual = FAnguloAnterior + FDeltaAngulo;
			if(FAnguloActual > 360.) {
				FAnguloActual -= 360.;
			}
		}

		if(FValvula->getTypeOfValve() == nmValvula4T) {
			if(FValvula->getControlRegimen() == nmMotor) {
				FRegimen = RegimenMotor;
			} else {
				FRegimen = FValvula->getRegimen();
			}
			FTime0 = FTime1;
			FTime1 = TiempoActual;
			FDeltaT = FTime1 - FTime0;
			FDeltaAngulo = 360. * FRegimen * FValvula->getRelacionVelocidades() / 60. * FDeltaT;
			FAnguloAnterior = FAnguloActual;
			FAnguloActual = FAnguloAnterior + FDeltaAngulo;
			if(FAnguloActual > 360.) {
				FAnguloActual -= 360.;
			}
		}

		if(FValvula->getTypeOfValve() == nmValvulaContr) {
			FRegimen = RegimenMotor;
			FTime0 = FTime1;
			FTime1 = TiempoActual;
			FDeltaT = FTime1 - FTime0;
			FDeltaAngulo = 360. * FRegimen / 60. * FDeltaT;
			FAnguloAnterior = FAnguloActual;
			FAnguloActual = FAnguloAnterior + FDeltaAngulo;
			if(FAnguloActual > 720.) {
				FAnguloActual -= 720.;
			}
		}

		switch(FValvula->getTypeOfValve()) {
		case nmCDFijo:
			dynamic_cast<TCDFijo *>(FValvula)->CalculaCD();
			break;

		case nmValvula4T:
			dynamic_cast<TValvula4T *>(FValvula)->CalculaCD(FAnguloActual);
			break;

		case nmLamina:
			PTub = FDeposito1->getPressure();
			PVol = FDeposito2->getPressure();
			DeltaP = PTub - PVol;
			dynamic_cast<TLamina *>(FValvula)->CalculaCD(DeltaP, TiempoActual);
			break;

		case nmDiscoRotativo:
			dynamic_cast<TDiscoRotativo *>(FValvula)->CalculaCD(FAnguloActual);
			break;

		case nmLumbrera2T:
			dynamic_cast<TLumbrera *>(FValvula)->CalculaCD(FAnguloActual);
			break;

		case nmValvulaContr:
			dynamic_cast<TValvulaContr *>(FValvula)->CalculaCD(FAnguloActual, mfcomb);
			break;

		case nmWasteGate:
			PTub = FDeposito1->getPressure();
			PVol = FDeposito2->getPressure();
			nodoadm = dynamic_cast<TWasteGate *>(FValvula)->getNodoAdm();
			distancia = dynamic_cast<TWasteGate *>(FValvula)->getDist();
			p1 = dynamic_cast<TWasteGate *>(FValvula)->getTuboAdm()->GetPresion(nodoadm);
			p2 = dynamic_cast<TWasteGate *>(FValvula)->getTuboAdm()->GetPresion(nodoadm + 1);
			PAdm = InterpolaDeposito(p1, p2, 1.0, distancia);
			dynamic_cast<TWasteGate *>(FValvula)->CalculaCD(PAdm, PTub, PVol, TiempoActual);
			break;

		case nmStator:
			dynamic_cast<TEstatorTurbina *>(FValvula)->CalculaCD();
			break;

		case nmRotor:
			dynamic_cast<TRotorTurbina *>(FValvula)->CalculaCD();
			break;

		case nmCalcExtern:
			dynamic_cast<TCDExterno *>(FValvula)->CalculaCD();
			break;
		}

		FCDEntrada = FValvula->getCDTubVol();
		FCDEntrada = FValvula->getCDTubVol();
		FCDSalida = FValvula->getCDVolTub();
		FCTorbellino = FValvula->getCTorb();

		if(FCDEntrada > 2.0 || FCDEntrada < 0.0) {
			printf("ERROR: TCCUnionEntreDepositos::CalculaCoeficientesDescarga, en calculo coeficiente descarga entrante: %lf, en %lf grados,en la condicion de contorno: %d\n",
				   FCDEntrada,
				   FAnguloActual, FNumeroCC);
			throw Exception(
				"ERROR:TCCUnionEntreDepositos::CalculaCoeficientesDescarga en calculo coeficiente descarga entrante: " + std::to_string(
					FCDEntrada) + ", en " + std::to_string(FAnguloActual) + " grados ");

		}
		if(FCDSalida > 1.0 || FCDSalida < 0.0) {
			printf("ERROR: TCCUnionEntreDepositos::CalculaCoeficientesDescarga, en calculo coeficiente descarga saliente: %lf, en %lf grados, en la condicion de contorno: %d\n",
				   FCDSalida,
				   FAnguloActual, FNumeroCC);
			throw Exception(
				"ERROR: TCCUnionEntreDepositos::CalculaCoeficientesDescarga en calculo coeficiente descarga saliente: " +
				std::to_string(FCDSalida) + ", en " + std::to_string(FAnguloActual) + " grados ");
		}
	} catch(exception &N) {
		std::cout << "ERROR: TCCUnionEntreDepositos::CalculaCoeficientesDescarga en la condicion de contorno: " << FNumeroCC <<
				  std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

double TCCUnionEntreDepositos::InterpolaDeposito(double vizq, double vder, double axid, double xif) {
	try {
		double xx = 0., yy = 0., ret_val = 0.;

		xx = vder - vizq;
		if(axid != 0.) {
			yy = (xx / axid) * xif;
			ret_val = vizq + yy;
		} else {
			std::cout << "ERROR: valores entrada TCCUnionEntreDepositos::InterpolaDeposito: " << std::endl;
			throw Exception(" ");
		}
		return ret_val;
	} catch(exception &N) {
		std::cout << "ERROR: TCCUnionEntreDepositos::InterpolaDeposito en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCUnionEntreDepositos::CalculaCondicionContorno(double Time) {
	try {

		if(FIndependiente) {

			/*FPresionDep1SUM+=FDeposito1->getPressure()*(FTiempoActual-FDeposito1->getTiempo());
			 FGammaDep1SUM+=FDeposito1->getGamma()*(FTiempoActual-FDeposito1->getTiempo());
			 FRDep1SUM+=FDeposito1->getR()*(FTiempoActual-FDeposito1->getTiempo());
			 FAsonidoDep1SUM+=FDeposito1->getSpeedsound()*(FTiempoActual-FDeposito1->getTiempo());
			 FTiempoDep1SUM+=(FTiempoActual-FDeposito1->getTiempo());

			 FPresionDep2SUM+=FDeposito2->getPressure()*(FTiempoActual-FDeposito2->getTiempo());
			 FGammaDep2SUM+=FDeposito2->getGamma()*(FTiempoActual-FDeposito2->getTiempo());
			 FRDep2SUM+=FDeposito2->getR()*(FTiempoActual-FDeposito2->getTiempo());
			 FAsonidoDep2SUM+=FDeposito2->getSpeedsound()*(FTiempoActual-FDeposito2->getTiempo());
			 FTiempoDep2SUM+=(FTiempoActual-FDeposito2->getTiempo());   */

			double p0, p1, T0,/*T1,*/gasto_isen, a0, FraccionMasicaAcum = 0., Massflow;
			double paso1 = 0., paso2 = 0., paso3 = 0., paso4 = 0.;
			int signodep1 = 0, signodep2 = 0;

			/* if(FNumeroCC==3){
			 printf(".\n");
			 } */
			if(FDeposito1->getPressure() > FDeposito2->getPressure()) {
				FGamma = FDeposito1->getGamma();
				FRMezcla = FDeposito1->getR();
				p0 = __units::BarToPa(FDeposito1->getPressure());
				T0 = pow2(FDeposito1->getSpeedsound() * __cons::ARef) / (FGamma * FRMezcla);
				Fa0 = FDeposito1->getSpeedsound();
				//Fa1=FDeposito2->getSpeedsound();
				p1 = __units::BarToPa(FDeposito2->getPressure());
				//T1=pow(FDeposito2->getSpeedsound()*__cons::ARef,2)/(FGamma*FRMezcla);

			} else {
				FGamma = FDeposito2->getGamma();
				FRMezcla = FDeposito2->getR();
				p0 = __units::BarToPa(FDeposito2->getPressure());
				T0 = pow2(FDeposito2->getSpeedsound() * __cons::ARef) / (FGamma * FRMezcla);
				Fa0 = FDeposito2->getSpeedsound();
				//Fa1=FDeposito1->getSpeedsound();
				p1 = __units::BarToPa(FDeposito1->getPressure());
				//T1=pow(FDeposito1->getSpeedsound()*__cons::ARef,2)/(FGamma*FRMezcla);
			}

			FGamma1 = __Gamma::G1(FGamma);
			FGamma2 = __Gamma::G2(FGamma);

			paso1 = (__cons::Pi_4 * pow2(FValvula->getDiametro())) * p0 * sqrt(2 * FGamma / (FRMezcla * FGamma1 * T0));
			paso2 = pow(p1 / p0, 2 / FGamma);
			paso3 = pow(p1 / p0, FGamma2 / FGamma);
			paso4 = sqrt(paso2 - paso3);
			gasto_isen = paso1 * paso4;

			/* FGasto es el valor del massflow en modulo. Asi lo necesitan recibir los depositos, que ademas conocen el signo por
			 el que han de multiplicar este valor. */
			/* FGastoImpreso es el valor del massflow con el signo de acuerdo al convenio de WAMer. Es decir, massflow positivo en el
			 sentido del deposito con la union (deposito 1) al deposito al que llega el hilo (deposito 2). Este massflow es el que
			 se imprime como resultado y el que se pasa a los sensores. */

			if(FDeposito1->getPressure() > FDeposito2->getPressure()) {  /* Flujo del deposito 1 al deposito 2 */
				Massflow = FCDSalida * gasto_isen;
				FGasto = 0.9 * FGasto + 0.1 * Massflow;
				FVelocity = FGasto / (__geom::Circle_area(FValvula->getDiametro())) / (p0 / (FRMezcla * T0));
				FSentidoFlujoED1 = -1; /* Saliente -1*/
				FSentidoFlujoED2 = 1; /* Entrante */
				FGastoImpreso = FGasto; /* Al imprimir resultados se considera negativo si va del deposito
				 del que sale el hilo hacia el que tiene la union */

				//Transporte de especies quimicas.
				for(int j = 0; j < FNumeroEspecies - 2; j++) {
					FFraccionMasicaEspecie[j] = FDeposito1->GetFraccionMasicaEspecie(j);
					FraccionMasicaAcum += FFraccionMasicaEspecie[j];
				}
				FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
				if(FHayEGR)
					FFraccionMasicaEspecie[FNumeroEspecies - 1] = FDeposito1->GetFraccionMasicaEspecie(FNumeroEspecies - 1);

			} else { /* Flujo del deposito 2 al deposito 1 */
				Massflow = FCDEntrada * gasto_isen;
				FGasto = 0.9 * FGasto - 0.1 * Massflow;
				FVelocity = FGasto / (__geom::Circle_area(FValvula->getDiametro())) / (p0 / (FRMezcla * T0));
				FSentidoFlujoED1 = -1; /* Entrante */
				FSentidoFlujoED2 = 1; /* Saliente -1*/
				FGastoImpreso = FGasto;

				//Transporte de especies quimicas.
				for(int j = 0; j < FNumeroEspecies - 2; j++) {
					FFraccionMasicaEspecie[j] = FDeposito2->GetFraccionMasicaEspecie(j);
					FraccionMasicaAcum += FFraccionMasicaEspecie[j];
				}
				FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
				if(FHayEGR)
					FFraccionMasicaEspecie[FNumeroEspecies - 1] = FDeposito2->GetFraccionMasicaEspecie(FNumeroEspecies - 1);
			}

		} else {

			double p0, p1, T0, T1, gasto_isen, a0, FraccionMasicaAcum = 0., Massflow;
			double paso1 = 0., paso2 = 0., paso3 = 0., paso4 = 0.;
			int signodep1 = 0, signodep2 = 0;

			if(FDeposito1->getPressure() > FDeposito2->getPressure()) {
				FGamma = FDeposito1->getGamma();
				FRMezcla = FDeposito1->getR();
				p0 = __units::BarToPa(FDeposito1->getPressure());
				T0 = pow2(FDeposito1->getSpeedsound() * __cons::ARef) / (FGamma * FRMezcla);
				Fa0 = FDeposito1->getSpeedsound();
				//Fa1=FDeposito2->getSpeedsound();
				p1 = __units::BarToPa(FDeposito2->getPressure());
				//T1=pow(FDeposito2->getSpeedsound()*__cons::ARef,2)/(FGamma*FRMezcla);

			} else {
				FGamma = FDeposito2->getGamma();
				FRMezcla = FDeposito2->getR();
				p0 = __units::BarToPa(FDeposito2->getPressure());
				T0 = pow2(FDeposito2->getSpeedsound() * __cons::ARef) / (FGamma * FRMezcla);
				Fa0 = FDeposito2->getSpeedsound();
				//Fa1=FDeposito1->getSpeedsound();
				p1 = __units::BarToPa(FDeposito1->getPressure());
				//T1=pow(FDeposito1->getSpeedsound()*__cons::ARef,2)/(FGamma*FRMezcla);
			}

			FGamma1 = __Gamma::G1(FGamma);
			FGamma2 = __Gamma::G2(FGamma);

			paso1 = (__geom::Circle_area(FValvula->getDiametro())) * p0 * sqrt(2 * FGamma / (FRMezcla * FGamma1 * T0));
			paso2 = pow(p1 / p0, 2 / FGamma);
			paso3 = pow(p1 / p0, FGamma2 / FGamma);
			paso4 = sqrt(paso2 - paso3);
			gasto_isen = paso1 * paso4;

			/* FGasto es el valor del massflow en modulo. Asi lo necesitan recibir los depositos, que ademas conocen el signo por
			 el que han de multiplicar este valor. */
			/* FGastoImpreso es el valor del massflow con el signo de acuerdo al convenio de WAMer. Es decir, massflow positivo en el
			 sentido del deposito con la union (deposito 1) al deposito al que llega el hilo (deposito 2). Este massflow es el que
			 se imprime como resultado y el que se pasa a los sensores. */

			if(FDeposito1->getPressure() > FDeposito2->getPressure()) {  /* Flujo del deposito 1 al deposito 2 */
				FGasto = FCDSalida * gasto_isen;
				FVelocity = FGasto / (__geom::Circle_area(FValvula->getDiametro())) / (p0 / (FRMezcla * T0));
				FSentidoFlujoED1 = -1; /* Saliente */
				FSentidoFlujoED2 = 1; /* Entrante */
				FGastoImpreso = FGasto; /* Al imprimir resultados se considera negativo si va del deposito
				 del que sale el hilo hacia el que tiene la union */

				//Transporte de especies quimicas.
				for(int j = 0; j < FNumeroEspecies - 2; j++) {
					FFraccionMasicaEspecie[j] = FDeposito1->GetFraccionMasicaEspecie(j);
					FraccionMasicaAcum += FFraccionMasicaEspecie[j];
				}
				FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
				if(FHayEGR)
					FFraccionMasicaEspecie[FNumeroEspecies - 1] = FDeposito1->GetFraccionMasicaEspecie(FNumeroEspecies - 1);

			} else { /* Flujo del deposito 2 al deposito 1 */
				FGasto = FCDEntrada * gasto_isen;
				FVelocity = FGasto / (__geom::Circle_area(FValvula->getDiametro())) / (p0 / (FRMezcla * T0));
				FSentidoFlujoED1 = 1; /* Entrante */
				FSentidoFlujoED2 = -1; /* Saliente */
				FGastoImpreso = FGasto;

				//Transporte de especies quimicas.
				for(int j = 0; j < FNumeroEspecies - 2; j++) {
					FFraccionMasicaEspecie[j] = FDeposito2->GetFraccionMasicaEspecie(j);
					FraccionMasicaAcum += FFraccionMasicaEspecie[j];
				}
				FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
				if(FHayEGR)
					FFraccionMasicaEspecie[FNumeroEspecies - 1] = FDeposito2->GetFraccionMasicaEspecie(FNumeroEspecies - 1);
			}
		}

	} catch(exception &N) {
		std::cout << "ERROR: TCCUnionEntreDepositos::CalculaCondicionContorno en la condicion de contorno: " << FNumeroCC <<
				  std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCUnionEntreDepositos::CalculaUED() {
	try {
		double p0, p1, T0, gasto_isen, gasto_real, a0, FraccionMasicaAcum = 0.;
		double paso1 = 0., paso2 = 0., paso3 = 0., paso4 = 0.;
		int signodep1 = 0, signodep2 = 0;

		FPresionDep1 = FPresionDep1SUM / FTiempoDep1SUM;
		FPresionDep2 = FPresionDep2SUM / FTiempoDep2SUM;

		if(FPresionDep1 > FPresionDep2) {
			FGamma = FGammaDep1SUM / FTiempoDep1SUM;
			FRMezcla = FRDep1SUM / FTiempoDep1SUM;
			p0 = __units::BarToPa(FPresionDep1);
			a0 = FAsonidoDep1SUM / FTiempoDep1SUM;
			T0 = pow2(a0 * __cons::ARef) / (FGamma * FRMezcla);
			Fa1 = FAsonidoDep2SUM / FTiempoDep2SUM;
			p1 = __units::BarToPa(FPresionDep2);

			FGamma1 = __Gamma::G1(FGamma);
			FGamma2 = __Gamma::G2(FGamma);

			paso1 = (__geom::Circle_area(FValvula->getDiametro())) * p0 * sqrt(2 * FGamma / (FRMezcla * FGamma1 * T0));
			paso2 = pow(p1 / p0, 2 / FGamma);
			paso3 = pow(p1 / p0, FGamma2 / FGamma);
			paso4 = sqrt(paso2 - paso3);
			gasto_isen = paso1 * paso4;

		} else if(FPresionDep2 > FPresionDep1) {
			FGamma = FGammaDep2SUM / FTiempoDep2SUM;
			FRMezcla = FRDep2SUM / FTiempoDep2SUM;
			p0 = __units::BarToPa(FPresionDep2);
			a0 = FAsonidoDep2SUM / FTiempoDep2SUM;
			;
			T0 = pow2(a0 * __cons::ARef) / (FGamma * FRMezcla);
			Fa1 = FAsonidoDep1SUM / FTiempoDep1SUM;
			p1 = __units::BarToPa(FPresionDep1);

			FGamma1 = __Gamma::G1(FGamma);
			FGamma2 = __Gamma::G2(FGamma);

			paso1 = (__geom::Circle_area(FValvula->getDiametro())) * p0 * sqrt(2 * FGamma / (FRMezcla * FGamma1 * T0));
			paso2 = pow(p1 / p0, 2 / FGamma);
			paso3 = pow(p1 / p0, FGamma2 / FGamma);
			paso4 = sqrt(paso2 - paso3);
			gasto_isen = paso1 * paso4;
		}

		/* FGasto es el valor del massflow en modulo. Asi lo necesitan recibir los depositos, que ademas conocen el signo por
		 el que han de multiplicar este valor. */
		/* FGastoImpreso es el valor del massflow con el signo de acuerdo al convenio de WAMer. Es decir, massflow positivo en el
		 sentido del deposito con la union (deposito 1) al deposito al que llega el hilo (deposito 2). Este massflow es el que
		 se imprime como resultado y el que se pasa a los sensores. */

		if(FPresionDep1 > FPresionDep2) {  /* Flujo del deposito 1 al deposito 2 */
			FGasto = FCDEntrada * gasto_isen;
			FSentidoFlujoED1 = -1; /* Saliente */
			FSentidoFlujoED2 = 1; /* Entrante */
			FGastoImpreso = FGasto; /* Al imprimir resultados se considera negativo si va del deposito
			 del que sale el hilo hacia el que tiene la union */

			//Transporte de especies quimicas.
			for(int j = 0; j < FNumeroEspecies - 2; j++) {
				FFraccionMasicaEspecie[j] = FDeposito1->GetFraccionMasicaEspecie(j);
				FraccionMasicaAcum += FFraccionMasicaEspecie[j];
			}
			FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
			if(FHayEGR)
				FFraccionMasicaEspecie[FNumeroEspecies - 1] = FDeposito1->GetFraccionMasicaEspecie(FNumeroEspecies - 1);

		} else if(FPresionDep2 > FPresionDep1) {  /* Flujo del deposito 2 al deposito 1 */
			FGasto = FCDSalida * gasto_isen;
			FSentidoFlujoED1 = 1; /* Entrante */
			FSentidoFlujoED2 = -1; /* Saliente */
			FGastoImpreso = -FGasto;

			//Transporte de especies quimicas.
			for(int j = 0; j < FNumeroEspecies - 2; j++) {
				FFraccionMasicaEspecie[j] = FDeposito2->GetFraccionMasicaEspecie(j);
				FraccionMasicaAcum += FFraccionMasicaEspecie[j];
			}
			FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
			if(FHayEGR)
				FFraccionMasicaEspecie[FNumeroEspecies - 1] = FDeposito2->GetFraccionMasicaEspecie(FNumeroEspecies - 1);
		}

		FPresionDep1SUM = 0.;
		FAsonidoDep1SUM = 0.;
		FGammaDep1SUM = 0.;
		FRDep1SUM = 0.;
		FTiempoDep1SUM = 0.;
		FPresionDep2SUM = 0.;
		FAsonidoDep2SUM = 0.;
		FGammaDep2SUM = 0.;
		FRDep2SUM = 0.;
		FTiempoDep2SUM = 0.;

	} catch(exception &N) {
		std::cout << "ERROR: TCCUnionEntreDepositos::CalculaUED en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCUnionEntreDepositos::LeeResultadosInstantUED(const char *FileWAM, fpos_t &filepos) {
	int nvars = 0, var = 0;
	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &nvars);
		for(int i = 0; i < nvars; i++) {
			fscanf(fich, "%d ", &var);
			switch(var) {
			case 0:
				FResInstantUED.Massflow = true;
				break;
			default:
				std::cout << "Resultados instantaneos en UED(BC) " << FNumeroCC << " no implementados " << std::endl;
			}
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TCCUnionEntreDepositos::LeeResultadosInstantUED en la BC " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCUnionEntreDepositos::CabeceraResultadosInstantUED(stringstream& insoutput) {
	try {
//FILE *fich=fopen(FileSALIDA,"a");
		std::string Label;

		if(FResInstantUED.Massflow) {
			Label = "\t" + PutLabel(411) + std::to_string(FNumeroCC) + PutLabel(904);
			insoutput << Label.c_str();
		}

//fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TCCUnionEntreDepositos::CabeceraResultadosInstantUED en la BC " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCUnionEntreDepositos::ResultadosInstantUED() {
	try {
		if(FResInstantUED.Massflow)
			FResInstantUED.GastoINS = FGastoImpreso;

	} catch(exception &N) {
		std::cout << "ERROR: TCCUnionEntreDepositos::ResultadosInstantUED en la BC " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCUnionEntreDepositos::ImprimeResultadosInstantUED(stringstream& insoutput) {
	try {
//FILE *fich=fopen(FileSALIDA,"a");

		if(FResInstantUED.Massflow)
			insoutput << "\t" << FResInstantUED.GastoINS;

//fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TCCUnionEntreDepositos::ImprimeResultadosInstantUED en la BC " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void TCCUnionEntreDepositos::ReadAverageResultsUED(const char *FileWAM, fpos_t &filepos) {
	int nvars = 0, var = 0;
	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &nvars);
		for(int i = 0; i < nvars; i++) {
			fscanf(fich, "%d ", &var);
			switch(var) {
			case 0:
				FResMediosUED.Massflow = true;
				break;
			default:
				std::cout << "Resultados medios en UED(BC) " << FNumeroCC << " no implementados " << std::endl;
			}
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TCCUnionEntreDepositos::ReadAverageResultsUED en la BC " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCUnionEntreDepositos::HeaderAverageResultsUED(stringstream& medoutput) {
	try {
//FILE *fich=fopen(FileSALIDA,"a");
		std::string Label;

		if(FResMediosUED.Massflow) {
			Label = "\t" + PutLabel(411) + std::to_string(FNumeroCC) + PutLabel(904);
			medoutput << Label.c_str();
		}

//fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TCCUnionEntreDepositos::HeaderAverageResultsUED en la BC " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCUnionEntreDepositos::AcumulaResultadosMediosUED(double Actual) {
	try {

		double Delta = Actual - FResMediosUED.Tiempo0;

		if(FResMediosUED.Massflow) {
			FResMediosUED.GastoSUM += FGastoImpreso * Delta;
		}
		FResMediosUED.TiempoSUM += Delta;
		FResMediosUED.Tiempo0 = Actual;
	} catch(exception &N) {
		std::cout << "ERROR: TCCUnionEntreDepositos::AcumulaResultadosMediosUED en la BC " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCUnionEntreDepositos::ResultadosMediosUED() {
	try {
		if(FResMediosUED.Massflow) {
			FResMediosUED.GastoMED = FResMediosUED.GastoSUM / FResMediosUED.TiempoSUM;
			FResMediosUED.GastoSUM = 0.;
		}

		FResMediosUED.TiempoSUM = 0;
	} catch(exception &N) {
		std::cout << "ERROR: TCCUnionEntreDepositos::ResultadosMediosUED en la BC " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCUnionEntreDepositos::ImprimeResultadosMediosUED(stringstream& medoutput) {
	try {
//FILE *fich=fopen(FileSALIDA,"a");

		if(FResMediosUED.Massflow)
			medoutput << "\t" << FResMediosUED.GastoMED;

//fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TCCUnionEntreDepositos::ImprimeResultadosMediosUED en la BC " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)
