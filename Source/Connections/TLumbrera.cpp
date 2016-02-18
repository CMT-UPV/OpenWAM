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

#include "TLumbrera.h"
#include "TCilindro.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TLumbrera::TLumbrera(double Biela, double Carrera) :
	TTipoValvula(nmLumbrera2T) {

	FBiela = Biela;
	FCarrera = Carrera;
	fun_CDin = NULL;
	fun_CDout = NULL;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TLumbrera::~TLumbrera() {

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TLumbrera::TLumbrera(TLumbrera *Origen, int Valvula) :
	TTipoValvula(nmLumbrera2T) {

	FValvula = Valvula;
	FAltura = Origen->FAltura;
	FAnchura = Origen->FAnchura;
	FRadioSup = Origen->FRadioSup;
	FRadioInf = Origen->FRadioInf;
	FPosicionPMI = Origen->FPosicionPMI;
	FDiametroRef = Origen->FDiametroRef;

	FDiamRef = FDiametroRef;

	FNumeroOrden = Origen->FNumeroOrden;

	FNumCD = Origen->FNumCD;

	FAnguloApertura = Origen->FAnguloApertura; // Hector 2T
	FAnguloCierre = Origen->FAnguloCierre; // Hector 2T
	FBiela = Origen->FBiela; // Hector 2T
	FCarrera = Origen->FCarrera; // Hector 2T

	FApertura.resize(Origen->FApertura.size());
	FDatosCDEntrada.resize(Origen->FDatosCDEntrada.size());
	FDatosCDSalida.resize(Origen->FDatosCDSalida.size());

	for(int i = 0; i < FNumCD; i++) {
		FApertura[i] = Origen->FApertura[i];
		FDatosCDEntrada[i] = Origen->FDatosCDEntrada[i];
		FDatosCDSalida[i] = Origen->FDatosCDSalida[i];
	}
	fun_CDin = new Hermite_interp(FApertura, FDatosCDEntrada);
	fun_CDout = new Hermite_interp(FApertura, FDatosCDSalida);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TLumbrera::LeeDatosIniciales(const char *FileWAM, fpos_t &filepos, int norden, bool HayMotor,
								  TBloqueMotor *Engine) {
	try {

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		FNumeroOrden = norden;

		fscanf(fich, "%lf %lf %lf %lf %lf %lf ", &FAltura, &FAnchura, &FRadioSup, &FRadioInf, &FPosicionPMI, &FDiametroRef);
		fscanf(fich, "%d ", &FNumCD);

		FApertura.resize(FNumCD);
		FDatosCDEntrada.resize(FNumCD);
		FDatosCDSalida.resize(FNumCD);

		for(int j = 0; j < FNumCD; ++j) {
			fscanf(fich, "%lf ", &FApertura[j]);
		}
		for(int j = 0; j < FNumCD; ++j) {
			fscanf(fich, "%lf ", &FDatosCDEntrada[j]);
		}
		for(int j = 0; j < FNumCD; ++j) {
			fscanf(fich, "%lf ", &FDatosCDSalida[j]);
		}

		CalculateOpeningANDClose();

		fgetpos(fich, &filepos);
		fclose(fich);

	} catch(exception &N) {
		std::cout << "ERROR: LeeDatosIniciales Lumbrera" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TLumbrera::CalculaCD(double Angulo) {

	FApertActual = FAltura + FPosicionPMI - CalculaDistPMI(Angulo);

	if(FApertActual <= 0) {
		FApertActual = 0.;
		FCDTubVol = 0.;
		FCDVolTub = 0.;
	} else {
		FCDTubVol = fun_CDin->interp(FApertActual);
		FCDVolTub = fun_CDout->interp(FApertActual);
	}
}

void TLumbrera::GetCDin(double Time) {

	double Angulo = FCylinder->getAnguloActual();

	FApertActual = FAltura + FPosicionPMI - CalculaDistPMI(Angulo);

	if(FApertActual <= 0) {
		FApertActual = 0.;
		FCDTubVol = 0.;
	} else {
		FCDTubVol = fun_CDin->interp(FApertActual);
	}
}

void TLumbrera::GetCDout(double Time) {

	double Angulo = FCylinder->getAnguloActual();

	FApertActual = FAltura + FPosicionPMI - CalculaDistPMI(Angulo);

	if(FApertActual <= 0) {
		FApertActual = 0.;
		FCDVolTub = 0.;
	} else {
		FCDVolTub = fun_CDout->interp(FApertActual);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

inline double TLumbrera::CalculaDistPMI(double x) {

	double c = __units::DegToRad(x);

	return FCarrera - (FBiela + FCarrera * (1. - cos(c)) / 2. - sqrt(pow2(FBiela) - pow2(FCarrera * sin(c) / 2.)));
}

inline double TLumbrera::CalculaApertura(double x) {
	return FAltura + FPosicionPMI - CalculaDistPMI(x);
}

void TLumbrera::CalculateOpeningANDClose() {
	bool ang_found = false;
	double ang0 = 0.;
	double ang1 = 180.;
	double apt0 = FAltura + FPosicionPMI - CalculaDistPMI(ang0);
	double apt1 = FAltura + FPosicionPMI - CalculaDistPMI(ang1);
	double ang = 0., apt = 0.;
	while(ang1 - ang0 > 0.01) {
		ang = (ang1 + ang0) / 2.;
		apt = FAltura + FPosicionPMI - CalculaDistPMI(ang);
		if(apt > 0) {
			ang1 = ang;
			apt1 = apt;
		} else {
			ang0 = ang;
			apt0 = apt;
		}
	}
	FAnguloApertura = ang;
	FAnguloCierre = 360. - ang;

}

#pragma package(smart_init)
