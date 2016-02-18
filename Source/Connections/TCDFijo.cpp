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

#include "TCDFijo.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TCDFijo::TCDFijo() :
	TTipoValvula(nmCDFijo) {

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCDFijo::~TCDFijo() {

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCDFijo::TCDFijo(TCDFijo *Origen, int Valvula) :
	TTipoValvula(nmCDFijo) {

	FCDEntrada = Origen->FCDEntrada;
	FCDSalida = Origen->FCDSalida;
	FActivaDiamRef = Origen->FActivaDiamRef;
	FDiametroRef = Origen->FDiametroRef;
	FNumeroOrden = Origen->FNumeroOrden;

//Se utilizara el diametro del tubo
	if(FActivaDiamRef) {
		FDiamRef = FDiametroRef;
	} else {
		FDiamRef = -1;
	}

	FCDTubVol = FCDEntrada;
	FCDVolTub = FCDSalida;

	FValvula = Valvula;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCDFijo::LeeDatosIniciales(const char *FileWAM, fpos_t &filepos, int norden, bool HayMotor, TBloqueMotor *Engine) {
	try {
		int tmp = 0;
		FActivaDiamRef = false;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		FNumeroOrden = norden;

		fscanf(fich, "%lf %lf %d ", &FCDEntrada, &FCDSalida, &tmp);
		if(tmp == 1) {
			FActivaDiamRef = true;
			fscanf(fich, "%lf ", &FDiametroRef);
		}

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

void TCDFijo::CalculaCD() {
	try {
		if(FActivaDiamRef) {
			FCDTubVol = FCDEntrada * FSectionRatio;
			FCDVolTub = FCDSalida * FSectionRatio;
		} else {
			FCDTubVol = FCDEntrada;
			FCDVolTub = FCDSalida;
		}
	} catch(exception &N) {
		std::cout << "ERROR: TCDFijo::CalculaCD " << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}
#pragma package(smart_init)
