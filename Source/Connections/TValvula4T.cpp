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

#include "TValvula4T.h"
#include "TCilindro.h"
#include "TTubo.h"
#include "TBloqueMotor.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TValvula4T::TValvula4T() :
	TTipoValvula(nmValvula4T) {

	fun_FLift = NULL;
	fun_CDin = NULL;
	fun_CDout = NULL;
	fun_Torb = NULL;
	FVVTLift = false;
	FVVTTiming = false;
	FVVTDuration = false;
	FVVT = false;
	FVVTLiftMultiplier = 1.;
	FVVTTimigGap = 0.;
	FVVTDurationMultiplier = 1.;
	FAngle0 = 0.;
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TValvula4T::~TValvula4T() {

	delete fun_Torb;
	delete fun_FLift;
	delete fun_CDin;
	delete fun_CDout;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TValvula4T::TValvula4T(TValvula4T *Origen, int Valvula) :
	TTipoValvula(nmValvula4T) {

	FCDEntrada = Origen->FCDEntrada;
	FCDSalida = Origen->FCDSalida;
	FDiametro = Origen->FDiametro;
	FDiametroRef = Origen->FDiametroRef;
	FAnguloApertura = Origen->FAnguloApertura;
	FAnguloApertura0 = Origen->FAnguloApertura0;
	FAnguloCierre = Origen->FAnguloCierre;
	FCoefTorbMedio = Origen->FCoefTorbMedio;
	FIncrLev = Origen->FIncrLev;
	FIncrAng = Origen->FIncrAng;
	FNumeroOrden = Origen->FNumeroOrden;
	FAngle0 = Origen->FAngle0;

	FValvula = Valvula;

	FDiamRef = FDiametroRef;

	FEngine = Origen->FEngine;

	FAngle.resize(Origen->FAngle.size());
	FLevantamiento.resize(Origen->FLevantamiento.size());
	FDatosCDEntrada.resize(Origen->FDatosCDEntrada.size());
	FDatosCDSalida.resize(Origen->FDatosCDSalida.size());
	FDatosTorbellino.resize(Origen->FDatosTorbellino.size());
	FLiftCD.resize(Origen->FLiftCD.size());

	for(dVector::size_type i = 0; i < Origen->FAngle.size(); i++) {
		FAngle[i] = Origen->FAngle[i];
		FLevantamiento[i] = Origen->FLevantamiento[i];
	}
	fun_FLift = new Hermite_interp(FAngle, FLevantamiento);

	for(dVector::size_type i = 0; i < Origen->FLiftCD.size(); i++) {
		FLiftCD[i] = Origen->FLiftCD[i];
		FDatosCDEntrada[i] = Origen->FDatosCDEntrada[i];
		FDatosCDSalida[i] = Origen->FDatosCDSalida[i];
		FDatosTorbellino[i] = Origen->FDatosTorbellino[i];
	}

	fun_CDin = new Hermite_interp(FLiftCD, FDatosCDEntrada);
	fun_CDout = new Hermite_interp(FLiftCD, FDatosCDSalida);
	fun_Torb = new Hermite_interp(FLiftCD, FDatosTorbellino);

	FRegimen = Origen->getRegimen();
	FRelacionVelocidades = Origen->getRelacionVelocidades();
	FControlRegimen = Origen->getControlRegimen();

	FVVTLift = Origen->FVVTLift;
	FVVTTiming = Origen->FVVTTiming;
	FVVTDuration = Origen->FVVTDuration;

	FVVT = Origen->FVVT;

	FVVTLiftMultiplier = Origen->FVVTLiftMultiplier;
	FVVTTimigGap = Origen->FVVTTimigGap;
	FVVTDurationMultiplier = Origen->FVVTDurationMultiplier;

	FVVTLiftCtrlID = Origen->FVVTLiftCtrlID;
	FVVTTimingCtrlID = Origen->FVVTTimingCtrlID;
	FVVTDurationCtrlID = Origen->FVVTDurationCtrlID;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TValvula4T::LeeDatosIniciales(const char *FileWAM, fpos_t &filepos, int norden, bool HayMotor,
								   TBloqueMotor *Engine) {
	try {
		int ControlRegimen, NumLev = 0, NumCD = 0;

		FEngine = Engine;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		FNumeroOrden = norden;

		fscanf(fich, "%lf %d %lf %lf %lf %lf ", &FDiametro, &NumLev, &FIncrAng, &FAnguloApertura, &FDiametroRef,
			   &FCoefTorbMedio);

		FAnguloApertura0 = FAnguloApertura;

		FAnguloCierre = FAnguloApertura + (double)(NumLev - 1) * FIncrAng;

		// FLevantamiento=new double[NumLev];
		FLevantamiento.resize(NumLev);
		FAngle.resize(NumLev);

		for(int i = 0; i < NumLev; i++) {
			FAngle[i] = (double) i * FIncrAng;
			fscanf(fich, " %lf", &FLevantamiento[i]);
		}

		fscanf(fich, "%d %lf ", &NumCD, &FIncrLev);

		FLiftCD.resize(NumCD);
		FDatosCDEntrada.resize(NumCD);
		FDatosCDSalida.resize(NumCD);
		FDatosTorbellino.resize(NumCD);

		for(int i = 0; i < NumCD; i++) {
			FLiftCD[i] = (double) i * FIncrLev;
			fscanf(fich, " %lf", &FDatosCDEntrada[i]);
		}
		for(int i = 0; i < NumCD; i++) {
			fscanf(fich, " %lf", &FDatosCDSalida[i]);
		}
		for(int i = 0; i < NumCD; i++) {
			fscanf(fich, " %lf", &FDatosTorbellino[i]);
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
			std::cout << "ERROR: TValvula4T::LeeDatosIniciales Lectura del Control del Regimen erronea " << std::endl;
			throw Exception(" ");
		}
		int controllers = 0;
		int param = 0;
		fscanf(fich, "%d ", &controllers);
		for(int i = 0; i < controllers; i++) {
			fscanf(fich, "%d ", &param);
			switch(param) {
			case 0:
				fscanf(fich, "%d ", &FVVTTimingCtrlID);
				FVVTLift = true;
				break;
			case 1:
				fscanf(fich, "%d ", &FVVTLiftCtrlID);
				FVVTTiming = true;
				break;
			case 2:
				fscanf(fich, "%d", &FVVTDurationCtrlID);
				FVVTDuration = true;
				break;
			}
			if(FVVTLift || FVVTTiming || FVVTDuration)
				FVVT = true;
		}

		fgetpos(fich, &filepos);
		fclose(fich);

	} catch(exception & N) {
		std::cout << "ERROR: LeeDatosIniciales Valvula4T" << std::endl;
		// std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TValvula4T::CalculaCD(double Angulo) {
	try {
		double X = 0., XLv = 0., XCd = 0.;

		if(Angulo <= FAnguloApertura || Angulo >= FAnguloCierre) {
			FCDTubVol = 0.;
			FCDVolTub = 0.;
		} else {
			X = Angulo - FAnguloApertura;
			XLv = X / (FIncrAng * FVVTDurationMultiplier);

			FApertura = fun_FLift->interp(XLv) * FVVTLiftMultiplier;

			if(FApertura < 0)
				FApertura = 0;

			FCDTubVol = fun_CDin->interp(FApertura) * FSectionRatio;
			FCDVolTub = fun_CDout->interp(FApertura) * FSectionRatio;
			FCTorb = fun_Torb->interp(FApertura);

		}
	} catch(exception & N) {
		std::cout << "ERROR: LeeDatosIniciales Valvula4T" << std::endl;
		// std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

void TValvula4T::GetCDin(double Time) {

	double X = 0., XLv = 0., XCd = 0., Angulo = 0.;

	if(FControlRegimen == nmPropio) {
		Angulo = 6. * FRegimen * Time; // It's correct if FRegimen is constant.
	} else {
		if(FToCylinder) {
			Angulo = FCylinder->getAnguloActual();
		} else {
			double DeltaT = Time - FTime0;
			FTime0 = Time;
			double DeltaA = 6 * FEngine->getRegimen() * DeltaT * FRelacionVelocidades;
			Angulo = DeltaA + FAngle0;
			if(Angulo > 360)
				Angulo -= 360.;
			FAngle0 = Angulo;
		}
	}

	if(Angulo <= FAnguloApertura || Angulo >= FAnguloCierre) {
		FCDTubVol = 0.;
		FCDVolTub = 0.;
	} else {
		X = Angulo - FAnguloApertura;
		XLv = X / FVVTDurationMultiplier;
		if(XLv > 720)
			XLv -= 720.;

		FApertura = fun_FLift->interp(XLv) * FVVTLiftMultiplier;

		if(FApertura < 0)
			FApertura = 0;

		FCDTubVol = fun_CDin->interp(FApertura) * FSectionRatio;
		FCTorb = fun_Torb->interp(FApertura);
	}
}

void TValvula4T::GetCDout(double Time) {
	double X = 0., XLv = 0., XCd = 0., Angulo = 0.;

	if(FControlRegimen == nmPropio) {
		Angulo = 360. * FRegimen / 60. * Time; // It's correct if FRegimen is constant.
	} else {
		if(FToCylinder) {
			Angulo = FCylinder->getAnguloActual();
		} else {
			double DeltaT = Time - FTime0;
			FTime0 = Time;
			double DeltaA = 6 * FEngine->getRegimen() * DeltaT * FRelacionVelocidades;
			Angulo = DeltaA + FAngle0;
			if(Angulo > 360)
				Angulo -= 360.;
			FAngle0 = Angulo;
		}
	}

	if(Angulo <= FAnguloApertura || Angulo >= FAnguloCierre) {
		FCDTubVol = 0.;
		FCDVolTub = 0.;
	} else {
		X = Angulo - FAnguloApertura;
		XLv = X / FVVTDurationMultiplier;
		if(XLv > 720)
			XLv -= 720.;

		FApertura = fun_FLift->interp(XLv) * FVVTLiftMultiplier;

		if(FApertura < 0)
			FApertura = 0;

		FCDVolTub = fun_CDout->interp(FApertura) * FSectionRatio;
		FCTorb = fun_Torb->interp(FApertura);
	}
}

void TValvula4T::VVTControl(double Time) {
	if(FVVT) {
		if(FVVTLift) {
			FVVTLiftMultiplier = FVVTLiftCtrl->Output(Time);
		}
		if(FVVTTiming) {
			FVVTTimigGap = FVVTTimingCtrl->Output(Time);
		}
		if(FVVTDuration) {
			FVVTDurationMultiplier = FVVTDurationCtrl->Output(Time);
		}
		FAnguloApertura = FAnguloApertura0 + FVVTTimigGap;
		FAnguloCierre = FAnguloApertura + (double)(FAngle.size() - 1) * FIncrAng * FVVTDurationMultiplier;
	}
}

void TValvula4T::AsignaLevController(TController **Controller) {
	if(FVVT) {
		if(FVVTLift) {
			FVVTLiftCtrl = Controller[FVVTLiftCtrlID - 1];
		}
		if(FVVTTiming) {
			FVVTTimingCtrl = Controller[FVVTTimingCtrlID - 1];
		}
		if(FVVTDuration) {
			FVVTDurationCtrl = Controller[FVVTDurationCtrlID - 1];
		}
	}
}

#pragma package(smart_init)
