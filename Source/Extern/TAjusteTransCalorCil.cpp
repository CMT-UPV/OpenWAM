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

#include "TAjusteTransCalorCil.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TAjusteTransCalorCil::TAjusteTransCalorCil() {

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TAjusteTransCalorCil::~TAjusteTransCalorCil() {

	if(FCiclo != NULL)
		delete[] FCiclo;
	if(FCadmision != NULL)
		delete[] FCadmision;
	if(FCescape != NULL)
		delete[] FCescape;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TAjusteTransCalorCil::CalculaTCC(double TiempoActual, double Regimen) {
	try {
		int jmax = 0, j = 0;
		double deltaT = 0., t = 0.;
		jmax = FCiclo[FNumeroTramos - 1];

		if(TiempoActual < FCiclo[1] * 120 / Regimen) {
			FCaqAdm = FCadmision[0];
			FCaqEsc = FCescape[0];
		}

		j = 1;
		while(TiempoActual > FCiclo[j] * 120 / Regimen && j < jmax) {
			j++;
		}
		if(j == jmax) {
			FCaqAdm = FCadmision[jmax];
			FCaqEsc = FCescape[jmax];
		} else {
			deltaT = (FCiclo[j] - FCiclo[j - 1]) * 120 / Regimen;
			t = TiempoActual - FCiclo[j - 1] * 120 / Regimen;
			FCaqAdm = xit_(FCadmision[j - 1], FCadmision[j], deltaT, t);
			FCaqEsc = xit_(FCescape[j - 1], FCescape[j], deltaT, t);
		}
		/*
		 if(TiempoActual>FCicloCambioTC*120/Regimen && TiempoActual<FCicloCambioTC2*120/Regimen){
		 FCaqAdm=FCaqAdm2;
		 }else if(TiempoActual>FCicloCambioTC2*120/Regimen){
		 FCaqAdm=FCaqAdm3;
		 } */
	} catch(exception &N) {
		std::cout << "ERROR: TTransCalorCil::CalculaTCC (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TAjusteTransCalorCil::IniciaTCC(int NumeroTramos, int *Ciclo, double *CoefTCAdm, double *CoefTCEsc) {
	try {
		FCiclo = new int[NumeroTramos];
		FCadmision = new double[NumeroTramos];
		FCescape = new double[NumeroTramos];
		FNumeroTramos = NumeroTramos;
		for(int i = 0; i < NumeroTramos; i++) {
			FCiclo[i] = Ciclo[i];
			FCadmision[i] = CoefTCAdm[i];
			FCescape[i] = CoefTCEsc[i];
		}

		FCaqAdm = FCadmision[0];
		FCaqEsc = FCescape[0];

	} catch(exception &N) {
		std::cout << "ERROR: TTransCalorCil::CalculaCD (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TAjusteTransCalorCil::xit_(double vizq, double vder, double axid, double xif) {
	try {
		double xx = 0., yy = 0.;
		double ret_val = 0.;

		xx = vder - vizq;
		if(axid != 0.) {
			yy = xx / axid * xif;
			ret_val = vizq + yy;
		} else {
			printf("ERROR: valores entrada xit\n");
			throw Exception("");
		}
		return ret_val;
	} catch(exception &N) {
		std::cout << "ERROR: TAjusteTransCalorCil::xit_" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)
