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

#include "TValvulaContr.h"
#include "TBloqueMotor.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TValvulaContr::TValvulaContr() :
	TTipoValvula(nmValvulaContr) {

	FAngle0 = 0;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TValvulaContr::~TValvulaContr() {

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TValvulaContr::TValvulaContr(TValvulaContr *Origen, int Valvula) :
	TTipoValvula(nmValvulaContr) {

	FTipoContr = Origen->FTipoContr;
	FLimiteInf1 = Origen->FLimiteInf1;
	FLimiteInf2 = Origen->FLimiteInf2;
	FLimiteSup1 = Origen->FLimiteSup1;
	FLimiteSup2 = Origen->FLimiteSup2;
	FCDInicial = Origen->FCDInicial;
	FCDFinal = Origen->FCDFinal;
	FAngle0 = Origen->FAngle0;

	FValvula = Valvula;

	FDiamRef = -1;

	FNumeroOrden = Origen->FNumeroOrden;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TValvulaContr::LeeDatosIniciales(const char *FileWAM, fpos_t &filepos, int norden, bool HayMotor,
									  TBloqueMotor *Engine) {
	try {
		int Ctrl = 0;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		FNumeroOrden = norden;

		FEngine = Engine;

		fscanf(fich, "%d %lf %lf %lf %lf %lf %lf ", &Ctrl, &FLimiteInf1, &FLimiteInf2, &FLimiteSup1, &FLimiteSup2, &FCDInicial,
			   &FCDFinal);

		switch(Ctrl) {
		case 0:
			FTipoContr = nmContrAngulo;
			break;
		case 1:
			FTipoContr = nmContrFuel;
			break;
		}

		fgetpos(fich, &filepos);
		fclose(fich);

	} catch(exception &N) {
		std::cout << "ERROR: LeeDatosIniciales ValvulaContr" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TValvulaContr::CalculaCD(double AnguloActual, double Mf) {
	try {
		double xx = 0., acoef = 0., bcoef = 0.;

		if(FTipoContr == nmContrAngulo)
			xx = AnguloActual;
		else if(FTipoContr == nmContrFuel)
			xx = Mf;
		else {
			printf("ERROR: This type of controlled valve is not implemented");
			throw Exception("ERROR: This type of controlled valve is not implemented");
		}
		if(FLimiteSup1 == FLimiteSup2) {
			if(xx >= FLimiteSup1) {
				FCDTubVol = FCDFinal;
				FCDVolTub = FCDFinal;
			}
		} else if(FLimiteSup1 < FLimiteSup2) {
			if(xx >= FLimiteSup1 && xx < FLimiteSup2) {
				acoef = (FCDFinal - FCDInicial) / (FLimiteSup2 - FLimiteSup1);
				bcoef = FCDInicial - acoef * FLimiteSup1;
				FCDTubVol = acoef * xx + bcoef;
				FCDVolTub = acoef * xx + bcoef;
			}
			if(xx >= FLimiteSup2) {
				FCDTubVol = FCDFinal;
				FCDVolTub = FCDFinal;
			}
		} else {
			printf("ERROR: in hysteresis valve (EGR,Swirl...");
			throw Exception("ERROR: in hysteresis valve (EGR,Swirl...)");
		}
		if(FLimiteInf1 == FLimiteInf2) {
			if(xx <= FLimiteInf1) {
				FCDTubVol = FCDInicial;
				FCDVolTub = FCDInicial;
			}
		} else if(FLimiteInf1 > FLimiteInf2) {
			if(xx <= FLimiteInf1 && xx > FLimiteInf2) {
				acoef = (FCDInicial - FCDFinal) / (FLimiteInf2 - FLimiteInf1);
				bcoef = FCDInicial - acoef * FLimiteInf2;
				FCDTubVol = acoef * xx + bcoef;
				FCDVolTub = acoef * xx + bcoef;
			}
			if(xx <= FLimiteInf2) {
				FCDTubVol = FCDInicial;
				FCDVolTub = FCDFinal;
			}
		} else {
			printf("ERROR: in hysteresis valve (EGR,Swirl...");
			throw Exception("ERROR: in hysteresis valve (EGR,Swirl...)");
		}
	} catch(exception &N) {
		std::cout << "ERROR: CalculaCD ValvulaContr" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

void TValvulaContr::GetCDin(double Time) {

	double xx = 0., acoef = 0., bcoef = 0.;

	if(FTipoContr == nmContrAngulo) {
		double DeltaT = Time - FTime0;
		FTime0 = Time;
		double DeltaA = 6 * FEngine->getRegimen() * DeltaT;
		xx = DeltaA + FAngle0;
		FAngle0 = xx;
	} else if(FTipoContr == nmContrFuel) {
		xx = FEngine->getMasaFuel();
	} else {
		printf("ERROR: This type of controlled valve is not implemented");
		throw Exception("ERROR: This type of controlled valve is not implemented");
	}
	if(FLimiteSup1 == FLimiteSup2) {
		if(xx >= FLimiteSup1) {
			FCDTubVol = FCDFinal;
		}
	} else if(FLimiteSup1 < FLimiteSup2) {
		if(xx >= FLimiteSup1 && xx < FLimiteSup2) {
			acoef = (FCDFinal - FCDInicial) / (FLimiteSup2 - FLimiteSup1);
			bcoef = FCDInicial - acoef * FLimiteSup1;
			FCDTubVol = acoef * xx + bcoef;
		}
		if(xx >= FLimiteSup2) {
			FCDTubVol = FCDFinal;
		}
	} else {
		printf("ERROR: in hysteresis valve (EGR,Swirl...");
		throw Exception("ERROR: in hysteresis valve (EGR,Swirl...)");
	}
	if(FLimiteInf1 == FLimiteInf2) {
		if(xx <= FLimiteInf1) {
			FCDTubVol = FCDInicial;
		}
	} else if(FLimiteInf1 > FLimiteInf2) {
		if(xx <= FLimiteInf1 && xx > FLimiteInf2) {
			acoef = (FCDInicial - FCDFinal) / (FLimiteInf2 - FLimiteInf1);
			bcoef = FCDInicial - acoef * FLimiteInf2;
			FCDTubVol = acoef * xx + bcoef;
		}
		if(xx <= FLimiteInf2) {
			FCDTubVol = FCDInicial;
		}
	} else {
		printf("ERROR: in hysteresis valve (EGR,Swirl...");
		throw Exception("ERROR: in hysteresis valve (EGR,Swirl...)");
	}
}

void TValvulaContr::GetCDout(double Time) {
	double xx = 0., acoef = 0., bcoef = 0.;

	if(FTipoContr == nmContrAngulo) {
		double DeltaT = Time - FTime0;
		FTime0 = Time;
		double DeltaA = 6 * FEngine->getRegimen() * DeltaT;
		xx = DeltaA + FAngle0;
		FAngle0 = xx;
	} else if(FTipoContr == nmContrFuel) {
		xx = FEngine->getMasaFuel();
	} else {
		printf("ERROR: This type of controlled valve is not implemented");
		throw Exception("ERROR: This type of controlled valve is not implemented");
	}
	if(FLimiteSup1 == FLimiteSup2) {
		if(xx >= FLimiteSup1) {
			FCDVolTub = FCDFinal;
		}
	} else if(FLimiteSup1 < FLimiteSup2) {
		if(xx >= FLimiteSup1 && xx < FLimiteSup2) {
			acoef = (FCDFinal - FCDInicial) / (FLimiteSup2 - FLimiteSup1);
			bcoef = FCDInicial - acoef * FLimiteSup1;
			FCDVolTub = acoef * xx + bcoef;
		}
		if(xx >= FLimiteSup2) {
			FCDVolTub = FCDFinal;
		}
	} else {
		printf("ERROR: in hysteresis valve (EGR,Swirl...");
		throw Exception("ERROR: in hysteresis valve (EGR,Swirl...)");
	}
	if(FLimiteInf1 == FLimiteInf2) {
		if(xx <= FLimiteInf1) {
			FCDVolTub = FCDInicial;
		}
	} else if(FLimiteInf1 > FLimiteInf2) {
		if(xx <= FLimiteInf1 && xx > FLimiteInf2) {
			acoef = (FCDInicial - FCDFinal) / (FLimiteInf2 - FLimiteInf1);
			bcoef = FCDInicial - acoef * FLimiteInf2;
			FCDVolTub = acoef * xx + bcoef;
		}
		if(xx <= FLimiteInf2) {
			FCDVolTub = FCDFinal;
		}
	} else {
		printf("ERROR: in hysteresis valve (EGR,Swirl...");
		throw Exception("ERROR: in hysteresis valve (EGR,Swirl...)");
	}
}

#pragma package(smart_init)
