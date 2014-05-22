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
// ---------------------------------------------------------------------------
#pragma hdrstop

#include "TCCDeposito.h"
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
#include "TMariposa.h"

#include "TDepVolVariable.h"
#include "TDepVolCte.h"
#include "TTurbinaSimple.h"
#include "TTurbinaTwin.h"
#include "TVenturi.h"
#include "TUnionDireccional.h"

#include "TTubo.h"
#include "TDPF.h"
#include "TCanalDPF.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCCDeposito::TCCDeposito(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel,
	int numeroespecies, nmCalculoGamma GammaCalculation, bool ThereIsEGR) : TCondicionContorno
(TipoCC, numCC, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {

	FTuboExtremo = NULL;
	FValvula = NULL;
	FGasto = 0;
	FVelocity = 0;
	FSonido = 1;
	FVelocidadGarganta = 0;
	FMachGarganta = 1;
	FRelacionPresionGarganta = 1;
	FSentidoFlujo = nmParado;
	FMachVenturi = 0;

	FTime0 = 0.;
	FTime1 = 0.;
	FAnguloAnterior = 0.;
	FAnguloActual = 0.;

}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCCDeposito::~TCCDeposito() {

	delete[]FTuboExtremo;

	if (FValvula != NULL)
		delete FValvula;

	FValvula = NULL;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCDeposito::ReadBoundaryData(char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe,
				  int nDPF, TDPF **DPF)
{
	try {
		int i = 0,j = 0;
		int numid; // Variable necesaria para WAMer.

		FTuboExtremo = new stTuboExtremo[1];
		FTuboExtremo[0].Pipe = NULL;
		#ifdef ParticulateFilter
		FTuboExtremo[0].DPF=NULL;
		#endif
		FTuboExtremo[0].NumeroHaz=-1;
		FTuboExtremo[0].TipoCanal=-1;

		FPref = 1;

		while (FNumeroTubosCC < 1 && i < NumberOfPipes) {
			if (Pipe[i]->getNodoIzq() == FNumeroCC) {
				FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
				FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmIzquierda;
				FNodoFin = 0;
				FIndiceCC = 0;
				FCC = &(FTuboExtremo[FNumeroTubosCC].Beta);
				FCD = &(FTuboExtremo[FNumeroTubosCC].Landa);
				FNumeroTubosCC++;
				FUnionDPF=false;
				FEncontrado=true;
			}
			if (Pipe[i]->getNodoDer() == FNumeroCC) {
				FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
				FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmDerecha;
				FNodoFin = Pipe[i]->getNin() - 1;
				FIndiceCC = 1;
				FCC = &(FTuboExtremo[FNumeroTubosCC].Landa);
				FCD = &(FTuboExtremo[FNumeroTubosCC].Beta);
				FNumeroTubosCC++;
				FUnionDPF=false;
				FEncontrado=true;
			}
			i++;
		}
		#ifdef ParticulateFilter
		if (!FEncontrado){  // It is a junction between a plenum and a DPF
			int k=0;
			while(FNumeroTubosCC<1 && j<nDPF){
				 while(k<DPF[j]->getNumeroHacesCanales()){
					  if(DPF[j]->GetCanal(k,0)->getNodoIzq()==FNumeroCC){
						 FTuboExtremo[FNumeroTubosCC].NumeroHaz=k;
						 FTuboExtremo[FNumeroTubosCC].TipoCanal=0;
						 FTuboExtremo[FNumeroTubosCC].DPF=DPF[j];
						 FTuboExtremo[FNumeroTubosCC].TipoExtremo=nmIzquierda;
						 FNodoFin=0;
						 FIndiceCC=0;
						 FCC=&(FTuboExtremo[FNumeroTubosCC].Beta);
						 FCD=&(FTuboExtremo[FNumeroTubosCC].Landa);
						 FNumeroTubosCC++;
					  }
					  if(DPF[j]->GetCanal(k,0)->getNodoDer()==FNumeroCC){
						 FTuboExtremo[FNumeroTubosCC].NumeroHaz=k;
						 FTuboExtremo[FNumeroTubosCC].TipoCanal=0;
						 FTuboExtremo[FNumeroTubosCC].DPF=DPF[j];
						 FTuboExtremo[FNumeroTubosCC].TipoExtremo=nmDerecha;
						 FNodoFin=DPF[j]->GetCanal(k,0)->getNin()-1;
						 FIndiceCC=1;
						 FCC=&(FTuboExtremo[FNumeroTubosCC].Landa);
						 FCD=&(FTuboExtremo[FNumeroTubosCC].Beta);
						 FNumeroTubosCC++;
					  }
					  if(DPF[j]->GetCanal(k,1)->getNodoIzq()==FNumeroCC){
						 FTuboExtremo[FNumeroTubosCC].NumeroHaz=k;
						 FTuboExtremo[FNumeroTubosCC].TipoCanal=1;
						 FTuboExtremo[FNumeroTubosCC].DPF=DPF[j];
						 FTuboExtremo[FNumeroTubosCC].TipoExtremo=nmIzquierda;
						 FNodoFin=0;
						 FIndiceCC=0;
						 FCC=&(FTuboExtremo[FNumeroTubosCC].Beta);
						 FCD=&(FTuboExtremo[FNumeroTubosCC].Landa);
						 FNumeroTubosCC++;
					  }
					  if(DPF[j]->GetCanal(k,1)->getNodoDer()==FNumeroCC){
						 FTuboExtremo[FNumeroTubosCC].NumeroHaz=k;
						 FTuboExtremo[FNumeroTubosCC].TipoCanal=1;
						 FTuboExtremo[FNumeroTubosCC].DPF=DPF[j];
						 FTuboExtremo[FNumeroTubosCC].TipoExtremo=nmDerecha;
						 FNodoFin=DPF[j]->GetCanal(k,1)->getNin()-1;
						 FIndiceCC=1;
						 FCC=&(FTuboExtremo[FNumeroTubosCC].Landa);
						 FCD=&(FTuboExtremo[FNumeroTubosCC].Beta);
						 FNumeroTubosCC++;
					  }
					  k++;
				 }
				 j++;
				 k=0;
			}
			FEncontrado=true;
			FUnionDPF=true;
		}
		#endif
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &numid); // Esto es un dato que necesita el WAMer. Los usuarios de WAM hacemos la vista gorda hasta que se arregle.
		fscanf(fich, "%d ", &FNumeroDeposito);

		fgetpos(fich, &filepos);
		fclose(fich);

		// Inicializaciï¿½n del transporte de especies quï¿½micas
        FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
		if(!FUnionDPF){
		   for(int i=0;i<FNumeroEspecies-FIntEGR;i++){
			   FFraccionMasicaEspecie[i]=FTuboExtremo[0].Pipe->GetFraccionMasicaInicial(i);
		   }
		}else{
		   #ifdef ParticulateFilter
		   for(int i=0;i<FNumeroEspecies-FIntEGR;i++){
			   FFraccionMasicaEspecie[i]=FTuboExtremo[0].DPF->GetCanal(FTuboExtremo[0].NumeroHaz,0)->GetFraccionMasicaInicial(i);
		   }
		   #endif
		}

	}
	catch(Exception & N) {
		std::cout << "ERROR: TCCDeposito::LeeCCDeposito en la condiciï¿½n de contorno: " << FNumeroCC <<
			std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCDeposito::AsignaDeposito(TDeposito **Plenum) {
	try {

		FDeposito = Plenum[FNumeroDeposito - 1];
		FValvula->PutPlenum(FDeposito);

	}
	catch(Exception & N) {
		std::cout << "ERROR: TCCDeposito::AsignaDeposito en la condiciï¿½n de contorno: " << FNumeroCC <<
			std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCDeposito::AsignaTipoValvula(TTipoValvula **Origen, int Valv, int i) {
	try {

		switch(Origen[Valv - 1]->getTypeOfValve()) {
		case nmCDFijo:
			FValvula = new TCDFijo(dynamic_cast<TCDFijo*>(Origen[Valv - 1]), i);
			break;
		case nmValvula4T:
			FValvula = new TValvula4T(dynamic_cast<TValvula4T*>(Origen[Valv - 1]), i);
			break;
		case nmLamina:
			FValvula = new TLamina(dynamic_cast<TLamina*>(Origen[Valv - 1]), i);
			break;
		case nmDiscoRotativo:
			FValvula = new TDiscoRotativo(dynamic_cast<TDiscoRotativo*>(Origen[Valv - 1]), i);
			break;
		case nmLumbrera2T:
			FValvula = new TLumbrera(dynamic_cast<TLumbrera*>(Origen[Valv - 1]), i);
			break;
		case nmValvulaContr:
			FValvula = new TValvulaContr(dynamic_cast<TValvulaContr*>(Origen[Valv - 1]), i);
			break;
		case nmWasteGate:
			FValvula = new TWasteGate(dynamic_cast<TWasteGate*>(Origen[Valv - 1]), i);
			break;
		case nmStator:
			FValvula = new TEstatorTurbina(dynamic_cast<TEstatorTurbina*>(Origen[Valv - 1]), i);
			break;
		case nmRotor:
			FValvula = new TRotorTurbina(dynamic_cast<TRotorTurbina*>(Origen[Valv - 1]), i);
			break;
		case nmCalcExtern:
			FValvula = new TCDExterno(dynamic_cast<TCDExterno*>(Origen[Valv - 1]), i);
			break;
		case nmMariposa:
			FValvula = new TMariposa(dynamic_cast<TMariposa*>(Origen[Valv - 1]), i);
			break;
		}

		if(!FUnionDPF){
		  FValvula->PutPipe(FTuboExtremo[0].Pipe, FNodoFin);
		  FValvula->PutDiametroTubo(FTuboExtremo[0].Pipe->GetDiametro(FNodoFin));
		}else{
		  #ifdef ParticulateFilter
		  FValvula->PutDiametroTubo(FTuboExtremo[0].DPF->GetCanal(FTuboExtremo[0].NumeroHaz,FTuboExtremo[0].TipoCanal)->GetDiametro(FNodoFin));
          #endif
		}

		if (FValvula->getTypeOfValve() == nmCDFijo) {
			// dynamic_cast<TCDFijo *>(FValvula)->CalculaCD();
			FCDEntrada = FValvula->getCDTubVol();
			FCDSalida = FValvula->getCDVolTub();
			// FCTorbellino=FValvula->getCTorb();
			if (FCDEntrada > 1.) {
				FValvula->AsignaCRecuperacion(FCDEntrada - 1.);
			}
			else
				FValvula->AsignaCRecuperacion(0.);
		}

	}
	catch(Exception & N) {
		std::cout << "ERROR: TCCDeposito::AsignaTipoValvula en la condiciï¿½n de contorno: " <<
			FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCDeposito::CalculaCoeficientesDescarga(double TiempoActual, double mfcomb,
	double RegimenMotor) {
	try {
		double PVol, PTub, PAdm, DeltaP, distancia, CDExt, nodoadm, p1, p2;
		int turb, entr, NodoFin;

		switch(FValvula->getTypeOfValve()) {
		case nmCDFijo:
			dynamic_cast<TCDFijo*>(FValvula)->CalculaCD();
			break;

		case nmValvula4T:
			if (FValvula->getControlRegimen() == nmMotor) {
				FRegimen = RegimenMotor;
			}
			else {
				FRegimen = FValvula->getRegimen();
			}
			FTime0 = FTime1;
			FTime1 = TiempoActual;
			FDeltaT = FTime1 - FTime0;
			FDeltaAngulo = 360. * FRegimen * FValvula->getRelacionVelocidades() / 60. * FDeltaT;
			FAnguloAnterior = FAnguloActual;
			FAnguloActual = FAnguloAnterior + FDeltaAngulo;
			if (FAnguloActual > 360.) {
				FAnguloActual -= 360.;
			}
			dynamic_cast<TValvula4T*>(FValvula)->CalculaCD(FAnguloActual);
			break;

		case nmStator:
			dynamic_cast<TEstatorTurbina*>(FValvula)->CalculaCD();
			break;

		case nmRotor:
			dynamic_cast<TRotorTurbina*>(FValvula)->CalculaCD();
			break;

		case nmLamina:
			if (FTuboExtremo[0].TipoExtremo == nmIzquierda) {
				NodoFin = 0;
			}
			else
				NodoFin = FTuboExtremo[0].Pipe->getNin() - 1;
			DeltaP = FTuboExtremo[0].Pipe->GetPresion(NodoFin) - FDeposito->getPressure();
			dynamic_cast<TLamina*>(FValvula)->CalculaCD(DeltaP, FTuboExtremo[0].Pipe->getTime1());
			break;

		case nmDiscoRotativo:
			if (FValvula->getControlRegimen() == nmMotor) {
				FRegimen = RegimenMotor;
			}
			else {
				FRegimen = FValvula->getRegimen();
			}
			FTime0 = FTime1;
			FTime1 = TiempoActual;
			FDeltaT = FTime1 - FTime0;
			FDeltaAngulo = 360. * FRegimen * FValvula->getRelacionVelocidades() / 60. * FDeltaT;
			FAnguloAnterior = FAnguloActual;
			FAnguloActual = FAnguloAnterior + FDeltaAngulo;
			if (FAnguloActual > 360.) {
				FAnguloActual -= 360.;
			}
			dynamic_cast<TDiscoRotativo*>(FValvula)->CalculaCD(FAnguloActual);
			break;

		case nmLumbrera2T:
			FRegimen = dynamic_cast<TDepVolVariable*>(FDeposito)->getRegimen();
			FTime0 = FTime1;
			FTime1 = TiempoActual;
			FDeltaT = FTime1 - FTime0;
			FDeltaAngulo = 360. * FRegimen*dynamic_cast<TDepVolVariable*>(FDeposito)
				->getRelacionVelocidades() / 60. * FDeltaT;
			FAnguloAnterior = FAnguloActual;
			FAnguloActual = FAnguloAnterior + FDeltaAngulo;
			if (FAnguloActual > 360.) {
				FAnguloActual -= 360.;
			}
			dynamic_cast<TLumbrera*>(FValvula)->CalculaCD(FAnguloActual);
			break;

		case nmValvulaContr:
			FRegimen = RegimenMotor;
			FTime0 = FTime1;
			FTime1 = TiempoActual;
			FDeltaT = FTime1 - FTime0;
			FDeltaAngulo = 360. * FRegimen / 60. * FDeltaT;
			FAnguloAnterior = FAnguloActual;
			FAnguloActual = FAnguloAnterior + FDeltaAngulo;
			if (FAnguloActual > 720.) {
				FAnguloActual -= 720.;
			}
			dynamic_cast<TValvulaContr*>(FValvula)->CalculaCD(FAnguloActual, mfcomb);
			break;

		case nmWasteGate:
			if (FTuboExtremo[0].TipoExtremo == nmIzquierda) {
				NodoFin = 0;
			}
			else
				NodoFin = FTuboExtremo[0].Pipe->getNin() - 1;
			PTub = FTuboExtremo[0].Pipe->GetPresion(NodoFin);
			PVol = FDeposito->getPressure();
			nodoadm = dynamic_cast<TWasteGate*>(FValvula)->getNodoAdm();
			distancia = dynamic_cast<TWasteGate*>(FValvula)->getDist();
			p1 = dynamic_cast<TWasteGate*>(FValvula)->getTuboAdm()->GetPresion(nodoadm);
			p2 = dynamic_cast<TWasteGate*>(FValvula)->getTuboAdm()->GetPresion(nodoadm + 1);
			PAdm = Interpola(p1, p2, 1.0, distancia);
			dynamic_cast<TWasteGate*>(FValvula)->CalculaCD(PAdm, PTub, PVol,
				FTuboExtremo[0].Pipe->getTime1());
			break;

		case nmCalcExtern:
			dynamic_cast<TCDExterno*>(FValvula)->CalculaCD();
			break;

		case nmMariposa:
			dynamic_cast<TMariposa*>(FValvula)->CalculaCD(TiempoActual);
			break;
		}

		FCDEntrada = FValvula->getCDTubVol();
		FCDEntrada = FValvula->getCDTubVol();
		FCDSalida = FValvula->getCDVolTub();
		FCTorbellino = FValvula->getCTorb();
		if (FCDEntrada > 1.) {
			FValvula->AsignaCRecuperacion(FCDEntrada - 1.);
		}
		else
			FValvula->AsignaCRecuperacion(0.);

		if (FCDEntrada > 2.0 || FCDEntrada < 0.0) {
			printf(
				"ERROR: TCCDeposito::CalculaCoeficientesDescarga, en calculo coeficiente descarga entrante: %lf, en %lf grados,en la condiciï¿½n de contorno: %d \n", FCDEntrada, FAnguloActual, FNumeroCC);
			throw Exception(
				"ERROR: TCCDeposito::CalculaCoeficientesDescarga en calculo coeficiente descarga entrante: "
				+ AnsiString(FCDEntrada) + ", en " + AnsiString(FAnguloActual) + " grados ");

		}
		if (FCDSalida > 1.0 || FCDSalida < 0.0) {
			printf(
				"ERROR: TCCDeposito::CalculaCoeficientesDescarga, en calculo coeficiente descarga saliente: %lf, en %lf grados, en la condiciï¿½n de contorno: %d\n", FCDSalida, FAnguloActual, FNumeroCC);
			throw Exception(
				"ERROR: TCCDeposito::CalculaCoeficientesDescarga en calculo coeficiente descarga saliente: "
				+ AnsiString(FCDSalida) + ", en " + AnsiString(FAnguloActual) + " grados ");
		}
	}
	catch(Exception & N) {
		std::cout << "ERROR: TCCDeposito::CalculaCoeficientesDescarga en la condiciï¿½n de contorno: " <<
			FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCDeposito::IniciaGamma() {
	try {

	   if(!FUnionDPF){
		  FRMezcla = FTuboExtremo[0].Pipe->GetRMezcla(FNodoFin);
		  FGamma = FTuboExtremo[0].Pipe->GetGamma(FNodoFin);
		  FSeccionTubo = Pi * pow2(FTuboExtremo[0].Pipe->GetDiametro(FNodoFin)) / 4.;
	   }else{
		  #ifdef ParticulateFilter
		  FRMezcla=FTuboExtremo[0].DPF->GetCanal(FTuboExtremo[0].NumeroHaz,FTuboExtremo[0].TipoCanal)->GetRMezcla(FNodoFin);
		  FGamma=FTuboExtremo[0].DPF->GetCanal(FTuboExtremo[0].NumeroHaz,FTuboExtremo[0].TipoCanal)->GetGamma(FNodoFin);
		  FSeccionTubo=Pi*pow2(FTuboExtremo[0].DPF->GetCanal(FTuboExtremo[0].NumeroHaz,FTuboExtremo[0].TipoCanal)->GetDiametro(FNodoFin))/4.;
          #endif
	   }
		  FSeccionValvula=FSeccionTubo;

	}
	catch(Exception & N) {
		std::cout << "ERROR: TCCDeposito::CalculaCondicionContorno en la condiciï¿½n de contorno: " <<
			FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCDeposito::CalculaCondicionContorno(double Time) {
	try {
		double rel_CCon_Entropia, FraccionMasicaAcum = 0.;

		if(!FUnionDPF){
		   FGamma=FTuboExtremo[0].Pipe->GetGamma(FNodoFin);
		}else{
		   #ifdef ParticulateFilter
		   FGamma=FTuboExtremo[0].DPF->GetCanal(FTuboExtremo[0].NumeroHaz,FTuboExtremo[0].TipoCanal)->GetGamma(FNodoFin);
           #endif
		}
		FGamma1 = Gamma1(FGamma);
		FGamma2 = Gamma2(FGamma);
		FGamma3 = Gamma3(FGamma);
		FGamma4 = Gamma4(FGamma);
		FGamma5 = Gamma5(FGamma);
		FGamma6 = Gamma6(FGamma);

		FAd = pow(FDeposito->getPressure() / FPref, 1. / FGamma4);
		rel_CCon_Entropia = *FCC / FTuboExtremo[0].Entropia;
		FAdCr = FAd / sqrt(1 + pow2(FMachVenturi) * FGamma3); // Importante solo si hay venturi.
		// Si no es asï¿½, no modifica el valor de Ad.
		if (rel_CCon_Entropia / FAdCr > 1 + 1e-5) { // Flujo entrante al depï¿½sito
			FValvula->GetCDin(Time);
			FCDEntrada = FValvula->getCDTubVol();
			FSentidoFlujo = nmEntrante;
			if (FCDEntrada > 0.0001) { /* Abierto */
				if (FCDEntrada <= 1.0) {
					FSeccionEficaz = FCDEntrada * FSeccionValvula;
					FlujoEntranteDeposito();
				}
				else { /* Recuperaciï¿½n de la energï¿½a cinï¿½tica */
					FEDRecuperacionEnergiaCinetica();
				}

				// Transporte de especies quï¿½micas.
				for (int j = 0; j < FNumeroEspecies - 2; j++) {
					if(!FUnionDPF){
					   FFraccionMasicaEspecie[j]=FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC,j);
					}else{
					   #ifdef ParticulateFilter
					   FFraccionMasicaEspecie[j]=FTuboExtremo[0].DPF->GetCanal(FTuboExtremo[0].NumeroHaz,FTuboExtremo[0].TipoCanal)->GetFraccionMasicaCC(FIndiceCC,j);
                       #endif
					}
					FraccionMasicaAcum += FFraccionMasicaEspecie[j];
				}
				FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
				if(!FUnionDPF){
				   if(FHayEGR) FFraccionMasicaEspecie[FNumeroEspecies-1]=FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC,FNumeroEspecies-1);
				}else{
				   #ifdef ParticulateFilter
				   if(FHayEGR) FFraccionMasicaEspecie[FNumeroEspecies-1]=FTuboExtremo[0].DPF->GetCanal(FTuboExtremo[0].NumeroHaz,FTuboExtremo[0].TipoCanal)->GetFraccionMasicaCC(FIndiceCC,FNumeroEspecies-1);
                   #endif
				}
			}
			else { /* Cerrado */
				*FCD = *FCC;
				FGasto = 0.;
				FVelocity = 0.;
				FSonido = *FCC;
				// La composiciï¿½n se mantiene, al estar el depï¿½sito cerrado.
			}
		}
		else if (rel_CCon_Entropia / FAdCr < 1 - 1e-5) { // Flujo saliente del depï¿½sito
			FSentidoFlujo = nmSaliente;
			FValvula->GetCDout(Time);
			FCDSalida = FValvula->getCDVolTub();
			if (FCDSalida > 0.0001) { /* Abierto */
				// double TmpAa=FTuboExtremo[0].Entropia;  // nuevo para ver que pasa
				FSeccionEficaz = FCDSalida * FSeccionValvula;
				if (FDeposito->getTipoDeposito() == nmUnionDireccional) {
					FValvula->AsignaCDVolTub(FCDSalida);
				}
				FlujoSalienteDeposito();

				// Transporte de especies quï¿½micas.
				for (int j = 0; j < FNumeroEspecies - 2; j++) {
					FFraccionMasicaEspecie[j] = FDeposito->GetFraccionMasicaEspecie(j);
					FraccionMasicaAcum += FFraccionMasicaEspecie[j];
				}
				FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
				if (FHayEGR)
					FFraccionMasicaEspecie[FNumeroEspecies - 1]
						= FDeposito->GetFraccionMasicaEspecie(FNumeroEspecies - 1);
				// }                                           // nuevo para ver que pasa
			}
			else { /* Cerrado */
				*FCD = *FCC;
				FGasto = 0.;
				FVelocity = 0.;
				FSonido = *FCC;
				// La composiciï¿½n se mantiene, al estar el depï¿½sito cerrado.
			}
		}
		else { // Flujo Parado
			FSentidoFlujo = nmParado;
			*FCD = *FCC;
			FGasto = 0.;
			FVelocity = 0.;
			FSonido = *FCC;
			// La composiciï¿½n se mantiene, al estar el flujo parado.
		}
		FValvula->AcumulaCDMedio(Time);
	}
	catch(Exception & N) {
		std::cout << "ERROR: TCCDeposito::CalculaCondicionContorno en la condiciï¿½n de contorno: " <<
			FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCDeposito::FEDRecuperacionEnergiaCinetica() {
	try {
		double xaa2, ed, ei, a1, a2supuesta;

		ed = *FCC;
		ei = *FCC * 2. / FGamma2;
		a2supuesta = ei; // Se suponen condiciones sï¿½nicas.

		stRecover FA2(FTuboExtremo[0].Entropia, FAdCr, FGamma, FValvula->getCRecuperacion(), *FCC);
		double error = FA2(a2supuesta);

		if (error > 0) {
			FSonido = FindRoot(FA2, ed, ei);
			FVelocity = FA2.U2;
			FVelocidadGarganta = FA2.UThroat;
			a1 = FA2.A1;
		}
		else {
			FVelocity = 0.9999 * ei;
			FSonido = FA2.A2_2;
			FVelocidadGarganta = FA2.UThroat;
			a1 = FA2.A1;
		}

		xaa2 = pow(FTuboExtremo[0].Entropia, FGamma4);
		*FCD = FSonido - FGamma3 * FVelocity;
		*FCC = FSonido + FGamma3 * FVelocity;
		FGasto = -FGamma * FSeccionTubo * pow(FSonido, 2 * FGamma6) * FVelocity * 1e5 /
			(ARef * xaa2); // Massflow entrante al depï¿½sito negativo.
		FRelacionPresionGarganta = pow(FSonido / (FTuboExtremo[0].Entropia * FAdCr), FGamma4);
		FGastoGarganta = FGasto / (FValvula->getCRecuperacion() * FSeccionValvula);
		FMachGarganta = FVelocidadGarganta / a1; // En valor absoluto.

	}
	catch(Exception & N) {
		std::cout <<
			"ERROR: TCCDeposito::FEDRecuperacionEnergiaCinetica en la condiciï¿½n de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCDeposito::FlujoEntranteDeposito() {
	try {
		double vel_son_garganta, velocidad_garganta, Mach, xaa2, ycal, d1;

		Fk = FSeccionTubo / FSeccionEficaz;
		if (Fk < 1)
			Fk = 1.0;
		vel_son_garganta = FTuboExtremo[0].Entropia * FAdCr;
		// Velocity del sonido en la garganta. Adimensional.

		// Cï¿½lculo de la velocidad en la garganta.Caso de salto subcrï¿½tico.
		FCaso = nmFlujoEntranteSaltoSubcritico;
		if (Fk == 1) {
			FSonido = FTuboExtremo[0].Entropia * FAdCr;
			FVelocity = (*FCC - FSonido) / FGamma3;
		}
		else
			Resolucion(vel_son_garganta, *FCC, FCaso, &FVelocity, &FSonido);

		// Ecuaciï¿½n de la energï¿½a
		velocidad_garganta = sqrt(2. * FGamma6 * (pow2(FSonido) + FGamma3 * pow2(FVelocity) - pow2
				(vel_son_garganta)));
		// Se ha calculado la velocidad en la garganta en valor absoluto.

		// Cï¿½lculo de la velocidad en la garganta en el caso de salto supercrï¿½tico
		if (velocidad_garganta > vel_son_garganta) {
			FCaso = nmFlujoEntranteSaltoSupercritico;
			Resolucion(0.0, 1.0, FCaso, &ycal, &Mach);
			FVelocity = *FCC / (1 / Mach + FGamma3);
			FSonido = FVelocity / Mach;

			d1 = Fk * FVelocity * pow(FSonido, 1. / FGamma3);
			vel_son_garganta = pow(d1, FGamma1 / FGamma2);
			velocidad_garganta = vel_son_garganta;
		}
		// Fin caso de salto supercrï¿½tico

		xaa2 = pow(FTuboExtremo[0].Entropia, FGamma4);
		FGasto = -FGamma * FSeccionTubo * pow(FSonido, 2 * FGamma6) * FVelocity * 1e5 /
			(ARef * xaa2); // Massflow entrante al depï¿½sito negativo
		*FCD = FSonido - FGamma3 * FVelocity;
		*FCC = FSonido + FGamma3 * FVelocity;
		FRelacionPresionGarganta = pow(FSonido / (FTuboExtremo[0].Entropia * FAdCr), FGamma4);
		FGastoGarganta = FGasto / (FCDEntrada * FSeccionValvula);
		FMachGarganta = velocidad_garganta / vel_son_garganta;
		// En valor absoluto.Antes tenia signo.
		FVelocidadGarganta = velocidad_garganta;

	}

	catch(Exception & N) {
		std::cout << "ERROR: TCCDeposito::FlujoEntranteDeposito en la condiciï¿½n de contorno: " <<
			FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCDeposito::FlujoSalienteDeposito() {
	try {

		double a1, xx, yy, d1;
		double a2cr, val1, val2, u2cr, ycal, error, valde, miembro2;
		// Variables para resolver la onda de choque.
		double relacion_velocidades_son, Mach_tras_ondachoque, Mach, temp_antes_ondachoque,
		temp_tras_ondachoque;

		Fk = FSeccionTubo / FSeccionEficaz;
		if (Fk < 1)
			Fk = 1;

		/* Cï¿½lculo del valor de la velocidad del sonido en el extremo del tubo para
		el cual el salto es crï¿½tico. */
		u2cr = FDeposito->getSpeedsound() * sqrt(2. / FGamma2) *
			(sqrt(pow2(Fk) + FGamma1 * FGamma2) - Fk) / FGamma1;
		a2cr = sqrt(pow2(FDeposito->getSpeedsound()) - FGamma3 * pow2(u2cr));
		// Ecuaciï¿½n de la energï¿½a. Garganta-Depï¿½sito.

		/* A partir  de a2cr se determina el error en el cï¿½lculo de A2 al suponer salto
		subcrï¿½tico. Si es negativo, el salto es supercrï¿½tico. Si es positivo, el salto
		es subcrï¿½tico. */
		// FSSubcritico(a2cr,&error,&miembro2);

		stFSSub FSA2(FTuboExtremo[0].Entropia, FAdCr, FGamma, Fk, *FCC, FDeposito->getSpeedsound());

		error = FSA2(a2cr);

		if (error < 0.) { // Salto de presiones supercrï¿½tico.

			/* Determinaciï¿½n del intervalo de iteraciï¿½n. Para ello se supone que
			en el extremo del tubo se dan las condiciones crï¿½ticas. Explicado en
			los apuntes de Pedro. */
			a1 = sqrt(2. / FGamma2) * FDeposito->getSpeedsound();
			FVelocidadGarganta = a1;
			xx = pow(FAdCr / FDeposito->getSpeedsound(), FGamma4);
			yy = pow(a1, 2. / FGamma1);
			Fcc = FVelocidadGarganta * yy * xx / Fk;

			stFSSup FU2(FTuboExtremo[0].Entropia, Fcc, FGamma, Fk, *FCC,
				FDeposito->getSpeedsound());
			val1 = FU2(FVelocidadGarganta);

			// FSSupercritico(FVelocidadGarganta,&val1,&val2);
			if (val1 < 0.)
				valde = FVelocidadGarganta;
			if (val1 >= 0.) {
				double Epsilon = numeric_limits<double>::epsilon();
				valde = FDeposito->getSpeedsound() / sqrt(FGamma3) - Epsilon;
			}

			/* Una vez conocido el intervalo de iteraciï¿½n, se pasa a la resoluciï¿½n
			del caso flujo saliente salto supercrï¿½tico. */
			FCaso = nmFlujoSalienteSaltoSupercritico;
			Resolucion(0.0, valde, FCaso, &FVelocity, &FSonido);

			// Calcula del massflow. Como es saliente del depï¿½sito, siempre es positivo.
			xx = pow(sqrt(2. / FGamma2), (FGamma2 / FGamma1));
			yy = pow(FAdCr, FGamma4);
			FGasto = FCDSalida * FSeccionValvula * FGamma * xx * yy * 1e5 /
				(FDeposito->getSpeedsound() * ARef);

			/* Reducciï¿½n a flujo subsï¿½nico mediante onda de choque plana en el caso
			de que se hayan obtenido condiciones supersï¿½nicas en el extremo del
			tubo. Explicado en la tesis Corberï¿½n (pï¿½gina de la 47 a la 52
			(punto 2.5) y de la 122 a la 129 (lo importante a partir de la 127) */
			Mach = FVelocity / FSonido;
			xx = *FCC + FGamma3 * FVelocity;
			FTuboExtremo[0].Entropia = FTuboExtremo[0].Entropia * FSonido / xx;
			// Ecuaciï¿½n de la caracterï¿½stica incidente.
			if (Mach > 1.) {

				/* Las ecuaciones siguientes corresponden a la resoluciï¿½n de la onda
				de choque plana. Se pueden encontrar en el punto 2.5 de la tesis
				de Corberï¿½n. */
				xx = FGamma4 * pow2(Mach) - 1.;
				Mach_tras_ondachoque = sqrt((pow2(Mach) + 2. / FGamma1) / xx);
				temp_tras_ondachoque = FGamma3 * pow2(Mach) + 1.;
				temp_antes_ondachoque = FGamma3 * pow2(Mach_tras_ondachoque) + 1.;
				relacion_velocidades_son = sqrt(temp_tras_ondachoque / temp_antes_ondachoque);
				FSonido = FSonido * relacion_velocidades_son;
				FVelocity = FSonido * Mach_tras_ondachoque;
				d1 = xx * FGamma1 / FGamma2;
				FTuboExtremo[0].Entropia = FTuboExtremo[0].Entropia * relacion_velocidades_son / pow
					(d1, FGamma5);
			}
		}
		else { // Salto de presiones subcrï¿½tico.

			// Resolucion del caso de flujo saliente salto subcritico.
			FCaso = nmFlujoSalienteSaltoSubcritico;
			Resolucion(a2cr, FDeposito->getSpeedsound(), FCaso, &ycal, &FSonido);
			// Aplicando la Ecuaciï¿½n de la Energï¿½a entre el depï¿½sito y la garganta:
			FVelocity = sqrt((pow2(FDeposito->getSpeedsound()) - pow2(FSonido)) / FGamma3);

			// Calculo del massflow. Como es saliente del deposito, siempre es positivo.
			a1 = FDeposito->getSpeedsound() * (*FCC + FGamma3 * FVelocity) /
				(FTuboExtremo[0].Entropia * FAd);
			FVelocidadGarganta = Fk * pow2(a1) * FVelocity / pow2(FSonido);
			FGasto = FCDSalida * FSeccionValvula * FGamma * pow(FAd / FDeposito->getSpeedsound(),
				FGamma4) * FVelocidadGarganta * pow(a1, 2. / FGamma1) * 1e5 / ARef;
			xx = *FCC + FGamma3 * FVelocity;
			FTuboExtremo[0].Entropia = FTuboExtremo[0].Entropia * FSonido / xx;
			// Ecuacion de la caracteristica incidente.
		}
		*FCD = FSonido + FGamma3 * FVelocity;
		*FCC = FSonido - FGamma3 * FVelocity;
		d1 = FSonido / (FTuboExtremo[0].Entropia * FAdCr);
		FRelacionPresionGarganta = pow(d1, FGamma4);
		FMachGarganta = FVelocidadGarganta / a1; // En valor absoluto.
		FGastoGarganta = FGasto / (FCDSalida * FSeccionValvula);
	}
	catch(Exception & N) {
		std::cout << "ERROR: TCCDeposito::FlujoSalienteDeposito en la condiciï¿½n de contorno: " <<
			FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCDeposito::Resolucion(double ext1, double ext2, nmCaso Caso, double *u2t, double *a2t) {
	try {
		/* std::cout << FNumeroCC << std::endl;
		if(FNumeroCC==26){
		std::cout << "peta";
		} */

		if (Caso == nmFlujoEntranteSaltoSubcritico) {
			stFESub FEA2(FTuboExtremo[0].Entropia, FAdCr, FGamma, Fk, *FCC);
			*a2t = FindRoot(FEA2, ext1, ext2);
			*u2t = FEA2.U2;
		}
		else if (Caso == nmFlujoEntranteSaltoSupercritico) {
			stFESup FMatch(FGamma, Fk);
			*a2t = FindRoot(FMatch, ext1, ext2);
			*u2t = 0.;
		}
		else if (Caso == nmFlujoSalienteSaltoSubcritico) {
			stFSSub FSA2(FTuboExtremo[0].Entropia, FAdCr, FGamma, Fk, *FCC,
				FDeposito->getSpeedsound());
			*a2t = FindRoot(FSA2, ext1, ext2);
			*u2t = FSA2.U2;
		}
		else if (Caso == nmFlujoSalienteSaltoSupercritico) {
			stFSSup FU2(FTuboExtremo[0].Entropia, Fcc, FGamma, Fk, *FCC,
				FDeposito->getSpeedsound());
			*u2t = FindRoot(FU2, ext1, ext2);
			*a2t = FU2.A2;
		}
		else {
			printf(
				"Error en la definiciï¿½n del flujo TCCDeposito::Resolucion en la condiciï¿½n de contorno: %d\n"
				, FNumeroCC);
			throw Exception("");
		}
	}

	catch(Exception & N) {
		std::cout << "ERROR: TCCDeposito::Resolucion en la condiciï¿½n de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//
// void TCCDeposito::FESubcritico(double vel_son_supuesta,double *u2_1,double *u2_2)
// {
// try
// {
//
// double xx,yy;
//
// /* Resoluciï¿½n de la ecuaciï¿½n (20) del artï¿½culo "Soluciï¿½n a la condiciï¿½n de
// contorno de la uniï¿½n cilindro-conducto de los MCIA". Ecuaciï¿½n 4.30 en la
// tesis de Corberï¿½n */
//
// xx=vel_son_supuesta/(FTuboExtremo[0].Entropia*FAdCr);
// yy=pow(xx,4.*FGamma6);
// yy=pow(Fk,2.)*yy-1.;
// *u2_2=FTuboExtremo[0].Entropia*FAdCr*sqrt(2.*FGamma6*(pow(xx,2.)-1.)/yy);   // Valor absoluto
//
//
// /* Resoluciï¿½n de la ecuaciï¿½n de la caracterï¿½stica incidente. */
//
// *u2_1=(*FCC-vel_son_supuesta)/FGamma3;  // En valor absoluto
//
//
//
// }
// catch(Exception &N)
// {
// std::cout << "ERROR: TCCDeposito::FESubcritico en la condiciï¿½n de contorno: " << FNumeroCC << std::endl;
// std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
// throw Exception(N.Message);
// }
// }
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------.
//
// void TCCDeposito::FESupercritico(double mach_supuesto,double *miembro1,
// double *miembro2)
// {
// try
// {
//
// double xx,yy;
//
// /* Resoluciï¿½n de la ecuaciï¿½n (21) del artï¿½culo "Soluciï¿½n a la condiciï¿½n de
// contorno de la uniï¿½n cilindro-conducto de los MCIA". Ecuaciï¿½n (4.31) de
// la tesis de Corberï¿½n */
//
// yy=(FGamma2/2.)*pow(Fk*mach_supuesto,2.*FGamma1/FGamma2);
// xx=FGamma3*pow(mach_supuesto,2);
// *miembro1=xx-yy+1.;     // Miembro 1 de la ecuaciï¿½n (21)
// *miembro2=0;            // Miembro 2 de la ecuaciï¿½n (21)
//
// }
// catch(Exception &N)
// {
// std::cout << "ERROR: TCCDeposito::FESupercritico en la condiciï¿½n de contorno: " << FNumeroCC << std::endl;
// std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
// throw Exception(N.Message);
// }
// }
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//
// void TCCDeposito::FSSubcritico(double vel_son_supuesta,double *error,double *miembro2)
// {
// try
// {
// double a1,u1,u2;
//
// *miembro2=0;
//
// /* Resoluciï¿½n del algoritmo de cï¿½lculo propuesto en la pï¿½gina 113 de la tesis
// de Corberï¿½n. */
//
// u2 = sqrt((pow(FDeposito->getSpeedsound(),2)-pow(vel_son_supuesta,2))/FGamma3);
// a1 = FDeposito->getSpeedsound()*(*FCC+FGamma3*u2)/(FTuboExtremo[0].Entropia*FAdCr);
// u1 = Fk*u2*pow(a1,2)/pow(vel_son_supuesta,2);
// *error=pow(a1,2)+FGamma3*pow(u1,2)-pow(FDeposito->getSpeedsound(),2);
//
// }
// catch(Exception &N)
// {
// std::cout << "ERROR: TCCDeposito::FSSubcritico en la condiciï¿½n de contorno: " << FNumeroCC << std::endl;
// std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
// throw Exception(N.Message);
// }
// }
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//
// void TCCDeposito::FSSupercritico(double vel_supuesta,double *a2_1,double *a2_2)
// {
// try
// {
//
// // Resoluciï¿½n de la ecuaciï¿½n de la energï¿½a entre el depï¿½sito y el extremo del tubo.
// *a2_1 = sqrt(pow(FDeposito->getSpeedsound(),2)-FGamma3*pow(vel_supuesta,2));
//
// // Resoluciï¿½n de la ecuaciï¿½n 4.20 de la tesis de Corberï¿½n.
// *a2_2 = sqrt(vel_supuesta*pow((*FCC+FGamma3*vel_supuesta)/
// FTuboExtremo[0].Entropia,FGamma4)/Fcc);
//
// }
// catch(Exception &N)
// {
// std::cout << "ERROR: TCCDeposito::FSSupercritico en la condiciï¿½n de contorno: " << FNumeroCC << std::endl;
// std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
// throw Exception(N.Message);
// }
// }
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------

#pragma package(smart_init)
