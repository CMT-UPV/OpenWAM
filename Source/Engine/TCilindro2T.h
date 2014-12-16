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
#ifndef TCilindro2TH
#define TCilindro2TH

#include "TCilindro.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TTubo;

class TCilindro2T: public TCilindro {
private:

	double EntalpiaEntrada(double ASonEnt, double VelEnt, double MasEnt,
			double ASonCil, double MasCil);

	void VariableInicialesCicloACT();

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

protected:

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

public:

	TCilindro2T(TBloqueMotor *Engine, int nc, bool ThereIsEGR);

	~TCilindro2T();

	void ActualizaPropiedades(double TiempoActual);

};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif

