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
#ifndef TTurbinaSimpleH
#define TTurbinaSimpleH

#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include "Constantes.h"
#include "TTurbina.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TTurbinaSimple: public TTurbina {
  private:
	//---------------------------------------------------------------------------
	// VARIABLES PRIVADAS
	//---------------------------------------------------------------------------

	//---------------------------------------------------------------------------
	// FUNCIONES PRIVADAS
	//---------------------------------------------------------------------------

  public:
	//---------------------------------------------------------------------------
	// VARIABLES PUBLICAS
	//---------------------------------------------------------------------------

	double GetEfficiency() {
		return FRendTurbina[0];
	}
	;

	//---------------------------------------------------------------------------
	// FUNCIONES PUBLICAS
	//---------------------------------------------------------------------------

	TTurbinaSimple(int i, nmTipoCalculoEspecies SpeciesModel, int numeroespecies, nmCalculoGamma GammaCalculation,
				   bool ThereIsEGR);

	~TTurbinaSimple();

	void CalculaCondicionTurbina(double TimeCalculo);

	void AsignaEntradaSalidaCC();

	void ImprimeResultadosMediosPantalla();

	void ReadAverageResultsTurb(const char *FileWAM, fpos_t &filepos);

	void CabeceraResultadosMedTurb(stringstream& medoutput);

	void ImprimeResultadosMedTurb(stringstream& medoutput);

	void IniciaMedias();

	void AcumulaMedias(double Tiempo);

	void LeeResultadosInstantTurb(const char *FileWAM, fpos_t &filepos);

	void CabeceraResultadosInstantTurb(stringstream& insoutput);

	void ImprimeResultadosInstantTurb(stringstream& insoutput);

	void ResultadosInstantTurb();

	void CalculaResultadosMediosTurb();

	void UpdateProperties0DModel(double TimeCalculo);

};

//---------------------------------------------------------------------------
#endif
