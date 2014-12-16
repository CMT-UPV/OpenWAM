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

#include "TRemansoMatlab.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TRemansoMatlab::TRemansoMatlab() {

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TRemansoMatlab::~TRemansoMatlab() {

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TRemansoMatlab::CalculaRemanso(double TiempoActual) {
	try {

		if (TiempoActual < 1.92) {
			FPressure = 1.;
			FTemperature = 30.;
		} else if (TiempoActual > 3.52) {
			FPressure = 2.5;
			FTemperature = 55.;
		} else {
			FPressure = 1. - 0.045 * (TiempoActual - 1.92)
					+ 1.74 * pow2(TiempoActual - 1.92)
					- 0.705 * pow3(TiempoActual - 1.92);
			FTemperature = 45.;
		}
	} catch (Exception &N) {
		std::cout << "ERROR: TRemansoMatlab::CalculaRemanso (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)
