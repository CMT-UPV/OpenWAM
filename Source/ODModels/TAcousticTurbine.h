// ---------------------------------------------------------------------------

#ifndef TTAcousticTurbineH
#define TAcousticTurbineH
// ---------------------------------------------------------------------------

#include "TTubo.h"

/**
 * @file TAcousticTurbine.h
 * @author Francisco José Arnau Martínez <farnau@mot.upv.es>
 * @version 0.1
 *
 * @section LICENSE
 *
 * This file is part of OpenWAM.
 *
 * OpenWAM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenWAM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @section DESCRIPTION
 * The TurboBearings class represent the bearing system in a turbocharger.
 *
 * It has methods to compute the power losses in the bearing system. This file
 * has the interface of TurboBearings.
 *
 */

class TAcousticTurbine {
private:

	TTubo *FInletPipe;
	TTubo *FVolute;
	TTubo *FOutletPipe;

	int FInletPipeID;
	int FVoluteID;
	int FOutletPipeID;

public:
	TAcousticTurbine(int InletPipeID, int VoluteID, int OutletPipeID);

	TAcousticTurbine();

	~TAcousticTurbine();

	double T3();

	double P3();

	double P30();

	double ExpRatio();

	double P4();

	double T4();

	double R(){return FInletPipe->GetRMezcla(0);};

	double SIn(){return FInletPipe->GetArea(0);};

	double DIn(){return FInletPipe->GetDiametro(0);};

	double MassIn();

	void AsignInPipe(TTubo **Pipe) {
		FInletPipe = Pipe[FInletPipeID - 1];
	};

	void AsignOutPipe(TTubo **Pipe) {
		FOutletPipe = Pipe[FOutletPipeID - 1];
	};

};

#endif

