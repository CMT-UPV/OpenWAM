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
#ifndef TTableH
#define TTableH

#include "TController.h"
//---------------------------------------------------------------------------

class TTable: public TController {
  private:

	int fID;
	int fDimensiones;
	std::vector<double> fX_map;
	std::vector<double> fY_map;
	std::vector<std::vector<double> > fZ_map;

	double fOutput;

  public:

	TTable(int i);
	~TTable();

	double Output(double Time);

	void LeeController(const char *FileWAM, fpos_t &filepos);

	void AsignaObjetos(TSensor **Sensor, TController **Controller);

	void LeeResultadosMedControlador(const char *FileWAM, fpos_t &filepos);

	void LeeResultadosInsControlador(const char *FileWAM, fpos_t &filepos);

	void CabeceraResultadosMedControlador(stringstream *medoutput);

	void CabeceraResultadosInsControlador(stringstream *insoutput);

	void ImprimeResultadosMedControlador(stringstream *medoutput);

	void ImprimeResultadosInsControlador(stringstream *insoutput);

	void IniciaMedias();

	void ResultadosMediosController();

	void AcumulaResultadosMediosController(double Actual);

	void ResultadosInstantController();

};

#endif
