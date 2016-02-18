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

#include "TRotorTurbina.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TRotorTurbina::TRotorTurbina() :
	TTipoValvula(nmRotor) {

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TRotorTurbina::~TRotorTurbina() {
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TRotorTurbina::TRotorTurbina(TRotorTurbina *Origen, int Valvula) :
	TTipoValvula(nmRotor) {

	FValvula = Valvula;

	FTipoRotor = Origen->FTipoRotor;
	FCDEInicial = Origen->FCDEInicial;
	FCDSInicial = Origen->FCDSInicial;
	FDiametroRef = Origen->FDiametroRef;

	FNumeroOrden = Origen->FNumeroOrden;

	FDiamRef = FDiametroRef;

	FCDTubVol = FCDEInicial;
	FCDVolTub = FCDSInicial;

	FCDVbl = FCDEInicial;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TRotorTurbina::LeeDatosIniciales(const char *FileWAM, fpos_t &filepos, int norden, bool HayMotor,
									  TBloqueMotor *Engine) {
	try {
		int tprotor = 0;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		FNumeroOrden = norden;

		fscanf(fich, "%lf %lf %lf ", &FCDEInicial, &FCDSInicial, &FDiametroRef);

		fgetpos(fich, &filepos);
		fclose(fich);

	} catch(exception &N) {
		std::cout << "ERROR: LeeDatosIniciales CDFijo" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TRotorTurbina::TipodeRotor(nmTipoRotor TipoRotor) {
	try {

		FTipoRotor = TipoRotor;

	} catch(exception &N) {
		std::cout << "ERROR: TRotorTurbina::TipodeRotor " << std::endl;
//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TRotorTurbina::CalculaCD() {
	try {
		switch(FTipoRotor) {
		case nmRotFijo:
			FCDTubVol = FCDEInicial * FSectionRatio;
			FCDVolTub = FCDSInicial * FSectionRatio;
			break;
		case nmRotMapa:
			FCDTubVol = FAreaEff / __geom::Circle_area(FDiamTubo);
			FCDVolTub = FAreaEff / __geom::Circle_area(FDiamTubo);
			break;
		case nmRotVariable:
			FCDTubVol = FCDVbl * FSectionRatio;
			FCDVolTub = FCDVbl * FSectionRatio;
			break;
		}
		if(FCDTubVol > 1) {
			FCDTubVol = 1.;
		} else if(FCDTubVol < 0) {
			FCDTubVol = 0.001;
		}
		if(FCDVolTub > 1) {
			FCDVolTub = 1.;
		} else if(FCDVolTub < 0) {
			FCDVolTub = 0.001;
		}
	} catch(exception &N) {
		std::cout << "ERROR: CalculaCD RotorTurbine" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

void TRotorTurbina::GetCDin(double Time) {

	switch(FTipoRotor) {
	case nmRotFijo:
		FCDTubVol = FCDEInicial * FSectionRatio;
		break;
	case nmRotMapa:
		FCDTubVol = FAreaEff / __geom::Circle_area(FDiamTubo);
		break;
	case nmRotVariable:
		FCDTubVol = FCDVbl * FSectionRatio;
		break;
	}
	if(FCDTubVol > 1) {
		FCDTubVol = 1.;
	} else if(FCDTubVol < 0) {
		FCDTubVol = 0.001;
	}
}

void TRotorTurbina::GetCDout(double Time) {

	switch(FTipoRotor) {
	case nmRotFijo:
		FCDVolTub = FCDSInicial * FSectionRatio;
		break;
	case nmRotMapa:
		FCDVolTub = FAreaEff / (pow2(FDiamTubo) * __cons::Pi_4);
		break;
	case nmRotVariable:
		FCDVolTub = FCDVbl * FSectionRatio;
		break;
	}
	if(FCDVolTub > 1) {
		FCDVolTub = 1.;
	} else if(FCDVolTub < 0) {
		FCDVolTub = 0.001;
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TRotorTurbina::AsignaTurbina(int NTurb) {
	FNumeroTurbina = NTurb;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#pragma package(smart_init)

