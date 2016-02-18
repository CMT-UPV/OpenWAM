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

#include "TDiscoRotativo.h"
#include "TCilindro.h"
#include "TTubo.h"
#include "TBloqueMotor.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TDiscoRotativo::TDiscoRotativo() :
	TTipoValvula(nmDiscoRotativo) {

	fun_CDin = NULL;
	fun_CDout = NULL;
	FAngle0 = 0.;

	FDCMultiplier = 1.;
	FShift = 0.;
	FDuration = 1.;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TDiscoRotativo::~TDiscoRotativo() {

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TDiscoRotativo::TDiscoRotativo(TDiscoRotativo *Origen, int Valvula) :
	TTipoValvula(nmDiscoRotativo) {

	FDiametroRef = Origen->FDiametroRef;
	FNumeroPuntos = Origen->FNumeroPuntos;

	FNumeroOrden = Origen->FNumeroOrden;

	FEngine = Origen->FEngine;

	FValvula = Valvula;

	FDiamRef = FDiametroRef;

	FAngle0 = Origen->FAngle0;

	FDCMultiplier = Origen->FDCMultiplier;
	FShift = Origen->FShift;
	FDuration = Origen->FDuration;

	FAngulo.resize(Origen->FAngulo.size());
	FDatosCDEntrada.resize(Origen->FDatosCDEntrada.size());
	FDatosCDSalida.resize(Origen->FDatosCDSalida.size());

	for(int i = 0; i < FNumeroPuntos; i++) {
		FAngulo[i] = Origen->FAngulo[i];
		FDatosCDEntrada[i] = Origen->FDatosCDEntrada[i];
		FDatosCDSalida[i] = Origen->FDatosCDSalida[i];
	}
	fun_CDin = new Hermite_interp(FAngulo, FDatosCDEntrada);
	fun_CDout = new Hermite_interp(FAngulo, FDatosCDSalida);

	FRegimen = Origen->getRegimen();
	FRelacionVelocidades = Origen->getRelacionVelocidades();
	FControlRegimen = Origen->getControlRegimen();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TDiscoRotativo::LeeDatosIniciales(const char *FileWAM, fpos_t &filepos, int norden, bool HayMotor,
									   TBloqueMotor *Engine) {
	try {
		int ControlRegimen = 0;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		FNumeroOrden = norden;

		FEngine = Engine;

		fscanf(fich, "%lf ", &FDiametroRef);
		fscanf(fich, "%lf %lf %lf ", &FDCMultiplier, &FShift, &FDuration);

		fscanf(fich, "%d ", &FNumeroPuntos);

		FAngulo.resize(FNumeroPuntos);
		FDatosCDEntrada.resize(FNumeroPuntos);
		FDatosCDSalida.resize(FNumeroPuntos);

		for(int j = 0; j < FNumeroPuntos; ++j) {
			fscanf(fich, "%lf ", &FAngulo[j]);
			FAngulo[j] *= FDuration;
		}
		for(int j = 0; j < FNumeroPuntos; ++j) {
			fscanf(fich, "%lf ", &FDatosCDEntrada[j]);
			FDatosCDEntrada[j] *= FDCMultiplier;
		}
		for(int j = 0; j < FNumeroPuntos; ++j) {
			fscanf(fich, "%lf ", &FDatosCDSalida[j]);
			FDatosCDEntrada[j] *= FDCMultiplier;
		}

		fscanf(fich, "%d ", &ControlRegimen);

		switch(ControlRegimen) {
		case 0:
			FControlRegimen = nmPropio;
			break;
		case 1:
			FControlRegimen = nmMotor;
			break;
		}
		if(FControlRegimen == nmPropio) {
			fscanf(fich, "%lf ", &FRegimen);
			FRelacionVelocidades = 1.;
		} else if(FControlRegimen == nmMotor && HayMotor) {
			fscanf(fich, "%lf ", &FRelacionVelocidades);
		} else {
			std::cout << "ERROR: TDiscoRotativo::LeeDatosIniciales Lectura del Control del Regimen erronea " << std::endl;
			throw Exception(" ");
		}

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: LeeDatosIniciales DiscoRotativo" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TDiscoRotativo::CalculaCD(double AnguloActual) {

	FCDTubVol = fun_CDin->interp(AnguloActual) * FSectionRatio;
	FCDVolTub = fun_CDout->interp(AnguloActual) * FSectionRatio;

}

void TDiscoRotativo::GetCDin(double Time) {

	double X = 0., XLv = 0., XCd = 0., Angulo = 0.;

	if(FControlRegimen == nmPropio) {
		Angulo = 6. * FRegimen * Time - FShift; // It's correct if FRegimen is constant.
	} else {
		if(FToCylinder) {
			Angulo = FCylinder->getAnguloActual() * 360. / FEngine->getAngTotalCiclo() - FShift;

		} else {
			double DeltaT = Time - FTime0;
			FTime0 = Time;
			double DeltaA = 6 * FEngine->getRegimen() * DeltaT * FRelacionVelocidades;
			Angulo = DeltaA + FAngle0;
		}
	}
	if(Angulo < 0.)
		Angulo += 360.;
	else
		Angulo = Angulo - 360 * int(floor(Angulo / 360));

	FAngle0 = Angulo;

	FCDTubVol = fun_CDin->interp(Angulo) * FSectionRatio;

	if(FCDTubVol > 1)
		FCDTubVol = 1.;

}

void TDiscoRotativo::GetCDout(double Time) {
	double X = 0., XLv = 0., XCd = 0., Angulo = 0.;

	if(FControlRegimen == nmPropio) {
		Angulo = 360. * FRegimen / 60. * Time - FShift;
		; // It's correct if FRegimen is constant.
	} else {
		if(FToCylinder) {
			Angulo = FCylinder->getAnguloActual() * 360. / FEngine->getAngTotalCiclo() - FShift;
		} else {
			double DeltaT = Time - FTime0;
			FTime0 = Time;
			double DeltaA = 6 * FEngine->getRegimen() * DeltaT * FRelacionVelocidades;
			Angulo = DeltaA + FAngle0;
		}
	}

	if(Angulo < 0.)
		Angulo += 360.;
	else
		Angulo = Angulo - 360 * int(floor(Angulo / 360));

	FAngle0 = Angulo;

	FCDVolTub = fun_CDout->interp(Angulo) * FSectionRatio;

	if(FCDVolTub > 1)
		FCDVolTub = 1.;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)
