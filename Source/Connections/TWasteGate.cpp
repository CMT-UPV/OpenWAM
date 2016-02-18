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

#include "TWasteGate.h"
#include "TTubo.h"
#include "TDeposito.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TWasteGate::TWasteGate() :
	TTipoValvula(nmWasteGate) {
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TWasteGate::~TWasteGate() {
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TWasteGate::TWasteGate(TWasteGate *Origen, int Valvula) :
	TTipoValvula(nmWasteGate) {

	FTuboControl = Origen->FTuboControl;
	FDistancia = Origen->FDistancia;
	FPresCapMando = Origen->FPresCapMando;
	FCoefC1 = Origen->FCoefC1;
	FCoefC2 = Origen->FCoefC2;
	FMasa = Origen->FMasa;
	FAmortiguamiento = Origen->FAmortiguamiento;
	FRigidez = Origen->FRigidez;
	FPrecarga = Origen->FPrecarga;
	FAreaDiaf = Origen->FAreaDiaf;
	FAreaPlato = Origen->FAreaPlato;
	FDiametroRef = Origen->FDiametroRef;
	FSentidoWG = Origen->FSentidoWG;

	FNumeroOrden = Origen->FNumeroOrden;

	FValvula = Valvula;

	FDiamRef = FDiametroRef;

	FX = 0.;
	FdX = 0.;
	FddX = 0.;

	FGraficasWG = false;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TWasteGate::LeeDatosIniciales(const char *FileWAM, fpos_t &filepos, int norden, bool HayMotor,
								   TBloqueMotor *Engine) {
	try {
		int modo = 0;

		FNumeroOrden = norden;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d", &FTuboControl, &FDistancia, &FPresCapMando, &FCoefC1,
			   &FCoefC2, &FMasa, &FAmortiguamiento, &FRigidez, &FPrecarga, &FAreaDiaf,
			   &FAreaPlato, &FDiametroRef, &modo);

		switch(modo) {
		case 0:
			FSentidoWG = 1;
			break;
		case 1:
			FSentidoWG = -1;
			break;
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: LeeDatosIniciales WasteGate" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TWasteGate::CalculoNodoAdm(TTubo **Pipe) {
	try {

		FTuboAdm = Pipe[FTuboControl - 1];

		FNodoAdm = floor(FDistancia / Pipe[FTuboControl - 1]->getXRef());
		if(FNodoAdm >= Pipe[FTuboControl - 1]->getNin() - 1) {
			FNodoAdm = FNodoAdm - 1;
			FDist = 1;
		} else
			FDist = FDistancia / Pipe[FTuboControl - 1]->getXRef() - FNodoAdm;

	} catch(exception &N) {
		std::cout << "ERROR: TWasteGate::CalculoNodoAdm " << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TWasteGate::CalculaCD(double Padm, double PTubo, double PDeposito, double deltaT) {
	try {
		FFuerza = (Padm - FPresCapMando) * FAreaDiaf + FSentidoWG * (PDeposito - PTubo) * FAreaPlato - FPrecarga;

		FddX = (FFuerza - FRigidez * FX - FAmortiguamiento * FdX) / FMasa;
		FX = FX + FdX * deltaT + FddX * deltaT * deltaT / 2.;
		FdX = FdX + FddX * deltaT;
		if(FX > 11.0e-3) {
			FX = 11.0e-3;
			FdX = 0.;
		}
		if(FX < 0) {
			FX = 0.;
			FdX = 0.;
			FCDTubVol = 0.;
			FCDVolTub = 0.;
		} else {
			FCDTubVol = FCoefC1 * (pow((FX / FDiametroRef), FCoefC2));
			FCDVolTub = FCoefC1 * (pow((FX / FDiametroRef), FCoefC2));
		}
	} catch(exception &N) {
		std::cout << "ERROR: LeeDatosIniciales WasteGate" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

void TWasteGate::GetCDin(double Time) {

	double deltaT = Time - FTime0;
	double p1 = FTuboAdm->GetPresion(FNodoAdm);
	double p2 = FTuboAdm->GetPresion(FNodoAdm + 1);
	double Padm = Interpola(p1, p2, 1.0, FDist);
	double PDeposito = FPlenum->getPressure();
	double PTubo = FPipe->GetPresion(FPipeNode);
	FFuerza = (Padm - FPresCapMando) * FAreaDiaf + FSentidoWG * (PDeposito - PTubo) * FAreaPlato - FPrecarga;

	FddX = (FFuerza - FRigidez * FX - FAmortiguamiento * FdX) / FMasa;
	FX = FX + FdX * deltaT + FddX * deltaT * deltaT / 2.;
	FdX = FdX + FddX * deltaT;
	if(FX > 11.0e-3) {
		FX = 11.0e-3;
		FdX = 0.;
	}
	if(FX < 0) {
		FX = 0.;
		FdX = 0.;
		FCDTubVol = 0.;
		FCDVolTub = 0.;
	} else {
		FCDTubVol = FCoefC1 * (pow((FX / FDiametroRef), FCoefC2));
	}
}

void TWasteGate::GetCDout(double Time) {

	double deltaT = Time - FTime0;
	double p1 = FTuboAdm->GetPresion(FNodoAdm);
	double p2 = FTuboAdm->GetPresion(FNodoAdm + 1);
	double Padm = Interpola(p1, p2, 1.0, FDist);
	double PDeposito = FPlenum->getPressure();
	double PTubo = FPipe->GetPresion(FPipeNode);
	FFuerza = (Padm - FPresCapMando) * FAreaDiaf + FSentidoWG * (PDeposito - PTubo) * FAreaPlato - FPrecarga;

	FddX = (FFuerza - FRigidez * FX - FAmortiguamiento * FdX) / FMasa;
	FX = FX + FdX * deltaT + FddX * deltaT * deltaT / 2.;
	FdX = FdX + FddX * deltaT;
	if(FX > 11.0e-3) {
		FX = 11.0e-3;
		FdX = 0.;
	}
	if(FX < 0) {
		FX = 0.;
		FdX = 0.;
		FCDTubVol = 0.;
		FCDVolTub = 0.;
	} else {
		FCDVolTub = FCoefC1 * (pow((FX / FDiametroRef), FCoefC2));
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TWasteGate::LeeDatosGraficas(const char *FileWAM, fpos_t &filepos) {
	try {
		int ndv = 0, var = 0;
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);
		FGraficasWG = true;
		FGrafLev = false;
		fscanf(fich, " %d", &ndv);
		for(int i = 0; i < ndv; i++) {
			fscanf(fich, " %d", &var);
			switch(var) {
			case 0:
				FGrafLev = true;
				break;
			}
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: LeeDatosGraficas WasteGate" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TWasteGate::CabeceraGraficaINS(stringstream& insoutput, int lam) {
	try {
//FILE *fich=fopen(FileSALIDA,"a");
		std::string Label;

		if(FGraficasWG) {
			if(FGrafLev) {
				Label = "\t" + PutLabel(13) + std::to_string(lam) + PutLabel(902);
				insoutput << Label.c_str();
			}
		}
//fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: CabeceraGrafica WasteGate" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TWasteGate::ImprimeGraficaINS(stringstream& insoutput) {
	try {
//FILE *fich=fopen(FileSALIDA,"a");
		if(FGraficasWG) {
			if(FGrafLev)
				insoutput << "\t" << FX;
		}
//fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: ImprimeGrafica WasteGate" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)

