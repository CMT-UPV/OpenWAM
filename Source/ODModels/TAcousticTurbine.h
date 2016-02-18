// ---------------------------------------------------------------------------

#ifndef TAcousticTurbineH
#define TAcousticTurbineH
// ---------------------------------------------------------------------------

#include "TTubo.h"

/**
 * @file TAcousticTurbine.h
 * @author Francisco Jose Arnau Martinez <farnau@mot.upv.es>
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

	std::vector<TTubo*> FInletPipe;
	std::vector<TTubo*> FVolute;
	TTubo *FOutletPipe;

	iVector FInletPipeID;
	iVector FVoluteID;
	int FOutletPipeID;

  public:
	TAcousticTurbine(iVector InletPipeID, iVector VoluteID, int OutletPipeID);

	TAcousticTurbine();

	~TAcousticTurbine();

	double T3(int i);

	double T3();

	double T30(int i);

	double P3(int i);

	double P3();

	double P30(int i);

	double ExpRatio(int i);

	double ExpRatio();

	double DiabEfficiency(int i);

	double P4();

	double T4();

	double R(int i) {
		return FInletPipe[i]->GetRMezcla(0);
	}
	;

	double SIn(int i) {
		return FInletPipe[i]->GetArea(0);
	}
	;

	double DIn(int i) {
		return FInletPipe[i]->GetDiametro(0);
	}
	;

	double DIn();

	double DOut() {
		return FOutletPipe->GetDiametro(FOutletPipe->getNin() - 1);
	}
	;

	double SOut() {
		return FOutletPipe->GetArea(FOutletPipe->getNin() - 1);
	}
	;

	double MassIn(int i);

	double MassIn();

	double MassOut();

	void AsignInPipe(TTubo **Pipe, int i) {
		FInletPipe[i] = Pipe[FInletPipeID[i] - 1];
	}
	;

	void AsignOutPipe(TTubo **Pipe) {
		FOutletPipe = Pipe[FOutletPipeID - 1];
	}
	;

};

#endif

