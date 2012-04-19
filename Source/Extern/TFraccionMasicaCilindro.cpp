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

#include "TFraccionMasicaCilindro.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TFraccionMasicaCilindro::TFraccionMasicaCilindro()
{

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TFraccionMasicaCilindro::~TFraccionMasicaCilindro()
{

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TFraccionMasicaCilindro::CalculaFraccionMasicaCilindro()
{
try
{
// En esta función u otras que puedas necesitar debes programar las acciones
// a realizar para imponer la composición del gas de escape a la apertura de
// las válvulas de escape. Recuerda que:

// 1.- Si realizas un calculo de especies simplificado sin transporte de la
// especie Combustible, el número de especies es 2, siendo el orden del vector de
// fracción másica el siguiente: [0] Gases quemados, [1] Aire fresco.
// 2.- Si se trata de transporte de especies simplificado, teniendo en cuenta el transporte
// de combustible, el orden del vector de fracción másica es el siguiente:
// [0] Gases quemados, [1] Combustible, [2] Aire fresco.
// 3.- Si el transporte de especies químicas es completo, sin transporte de combustible,
// el número de especies transportadas es 8, en el siguiente orden:
// [0] O2, [1] CO2, [2] H2O, [3] HC, [4] Soot, [5] NOx, [6] CO, [7] N2
// 4.- Si el transporte de especies químicas es completo con transporte de combustible,
// el orden del vector fracción másica será el siguiente:
// [0] O2, [1] CO2, [2] H2O, [3] HC, [4] Soot, [5] NOx, [6] CO, [7] Combustible, [8] N2

// Recuerda que es la suma de fracción másica total debe ser igual a la unidad.


}
catch(Exception &N)
{
std::cout << "ERROR: TFraccionMasicaCilindro::CalculaFraccionMasicaCilindro " << std::endl;
std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
throw Exception(N.Message.c_str());
}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)

