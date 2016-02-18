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

#include "TCDExterno.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCDExterno::TCDExterno() :
	TTipoValvula(nmCalcExtern) {

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCDExterno::~TCDExterno() {

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCDExterno::TCDExterno(TCDExterno *Origen, int Valvula) :
	TTipoValvula(nmCalcExtern) {

	FCDEInicial = Origen->FCDEInicial;
	FCDSInicial = Origen->FCDSInicial;
	FCTorbInicial = Origen->FCTorbInicial;

//Se utilizara el diametro del tubo.
	FDiamRef = -1;

	FNumeroOrden = Origen->FNumeroOrden;
	FValvula = Valvula;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCDExterno::LeeDatosIniciales(const char *FileWAM, fpos_t &filepos, int norden, bool HayMotor,
								   TBloqueMotor *Engine) {
	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%lf %lf %lf ", &FCDEInicial, &FCDSInicial, &FCTorbInicial);

		fgetpos(fich, &filepos);
		fclose(fich);

	} catch(exception &N) {
		std::cout << "ERROR: LeeDatosIniciales CDExterno" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCDExterno::CalculaCD() {
	try {
		FCDTubVol = FCDEntMatlab;
		FCDVolTub = FCDSalMatlab;
		FCTorb = FCTorMatlab;
	} catch(exception &N) {
		std::cout << "ERROR: CalculaCD CDExterno" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

#pragma package(smart_init)
