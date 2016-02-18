/*--------------------------------------------------------------------------------*\
|==========================|
 |\\   /\ /\   // O pen     | OpenWAM: The Open Source 1D Gas-Dynamic Code
 | \\ |  X  | //  W ave     |
 |  \\ \/_\/ //   A ction   | CMT-Motores Termicos / Universidad Politecnica Valencia
 |   \\/   \//    M odel    |
 ----------------------------------------------------------------------------------
 | License
 |
 |	This file is part of OpenWAM.
 |
 |	OpenWAM is free software: you can redistribute it and/or modify
 |	it under the terms of the GNU General Public License as published by
 |	the Free Software Foundation, either version 3 of the License, or
 |	(at your option) any later version.
 |
 |	OpenWAM is distributed in the hope that it will be useful,
 |	but WITHOUT ANY WARRANTY; without even the implied warranty of
 |	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 |	GNU General Public License for more details.
 |
 |	You should have received a copy of the GNU General Public License
 |	along with OpenWAM.  If not, see <http://www.gnu.org/licenses/>.
 |
 \*--------------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
#ifndef TCGestorWAMH
#define TCGestorWAMH
//---------------------------------------------------------------------------
#endif

//#include <Classes.hpp>
//#include <stdio.h>
#include <windows.h>
//#include <iostream>
//#include <sstream>
#include "Globales.h"

#define BUFSIZE 4096

class TCGestorWAM {
  private:
	// User declarations
	HANDLE hPipe;
	HANDLE hPipeResp;

	void Enviar(LPTSTR msg);
	void Enviar(float valor);
	void EsperarRespuesta();
  public:
	__fastcall TCGestorWAM();
	void Init();
	void NuevoMensaje(LPTSTR msg);
	void ProcesoTranscurrido(float valor);
	void CabeceraResInstantActualizada();
	void CabeceraResMediosActualizada();
	void FichResMediosActualizado();
	void FichResInstantActualizado();
	void Terminar();
};
