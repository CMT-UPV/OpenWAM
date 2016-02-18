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

#include "TEstatorTurbina.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TEstatorTurbina::TEstatorTurbina() :
	TTipoValvula(nmStator) {

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TEstatorTurbina::~TEstatorTurbina() {

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TEstatorTurbina::TEstatorTurbina(TEstatorTurbina *Origen, int Valvula) :
	TTipoValvula(nmStator) {

	FTipoEstator = Origen->FTipoEstator;
	FCDEInicial = Origen->FCDEInicial;
	FCDSInicial = Origen->FCDSInicial;
	FDiametroRef = Origen->FDiametroRef;
	FNumeroOrden = Origen->FNumeroOrden;

	FValvula = Valvula;

	FDiamRef = FDiametroRef;

	FCDTubVol = FCDEInicial;
	FCDVolTub = FCDSInicial;

	FCDVbl = FCDEInicial;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TEstatorTurbina::LeeDatosIniciales(const char *FileWAM, fpos_t &filepos, int norden, bool HayMotor,
										TBloqueMotor *Engine) {
	try {
		int tpstator = 0;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		FNumeroOrden = norden;

		fscanf(fich, "%lf %lf %lf ", &FCDEInicial, &FCDSInicial, &FDiametroRef);

		fgetpos(fich, &filepos);
		fclose(fich);

	} catch(exception &N) {
		std::cout << "ERROR: LeeDatosIniciales StatorTurbine" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TEstatorTurbina::TipodeEstator(nmTipoEstator TipoEstator) {
	try {

		FTipoEstator = TipoEstator;

	} catch(exception &N) {
		std::cout << "ERROR: TEstatorTurbina::TipodeEstator " << std::endl;
//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TEstatorTurbina::CalculaCD() {
	try {
		switch(FTipoEstator) {
		case nmStFijo:
			FCDTubVol = FCDEInicial * FSectionRatio;
			FCDVolTub = FCDSInicial * FSectionRatio;
			break;
		case nmStMapa:
			FCDTubVol = FAreaEff / __geom::Circle_area(FDiamTubo);
			FCDVolTub = FAreaEff / __geom::Circle_area(FDiamTubo);
			break;
		case nmStVariable:
			FCDTubVol = FCDVbl * FSectionRatio;
			FCDVolTub = FCDVbl * FSectionRatio;
			break;
		}
		if(FCDTubVol > 1) {
			FCDTubVol = 1.;
		} else if(FCDTubVol <= 0) {
			FCDTubVol = 0.001;
		}
		if(FCDVolTub > 1) {
			FCDVolTub = 1.;
		} else if(FCDVolTub <= 0) {
			FCDVolTub = 0.001;
		}
	} catch(exception &N) {
		std::cout << "ERROR: CalculaCD StatorTurbine" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

void TEstatorTurbina::GetCDin(double Time) {

	switch(FTipoEstator) {
	case nmStFijo:
		FCDTubVol = FCDEInicial * FSectionRatio;
		break;
	case nmStMapa:
		FCDTubVol = FAreaEff / __geom::Circle_area(FDiamTubo);
		break;
	case nmStVariable:
		FCDTubVol = FCDVbl * FSectionRatio;
		break;
	}
	if(FCDTubVol > 1) {
		FCDTubVol = 1.;
	} else if(FCDTubVol <= 0) {
		FCDTubVol = 0.001;
	}
}

void TEstatorTurbina::GetCDout(double Time) {

	switch(FTipoEstator) {
	case nmStFijo:
		FCDVolTub = FCDSInicial * FSectionRatio;
		break;
	case nmStMapa:
		FCDVolTub = FAreaEff / __geom::Circle_area(FDiamTubo);
		break;
	case nmStVariable:
		FCDVolTub = FCDVbl * FSectionRatio;
		break;
	}

	if(FCDVolTub > 1) {
		FCDVolTub = 1.;
	} else if(FCDVolTub <= 0) {
		FCDVolTub = 0.001;
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TEstatorTurbina::AsignaTurbina(double NTurb, double NEntr) {
	FNumeroTurbina = NTurb;
	FNumeroEntrada = NEntr + 1;
}
#pragma package(smart_init)

