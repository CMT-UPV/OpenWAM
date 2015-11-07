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

/** \mainpage OpenWAM source code documentation
 *
 * OpenWAM is a free, open source 1-dimensional gas-dynamic code produced by
 * CMT-Motores Termicos of the Universitat Politecnica de Valencia.
 * This code is specially developed to solve the thermo- and fluid-dynamics of
 * compressible flow in different elements that can be considered as 0 and
 * 1-dimensional.
 *
 * It can be used to predict the flow motion through the elements of an internal
 * combustion engine or in other facilities involving compressible flow.
 *
 * OpenWAM is offered under the GNU General Public License. OpenWAM will always
 * be free of charge and open source.
 *
 * For more information, see <http://www.cmt.upv.es/OpenWam.aspx>
 *
 * Also, see <http://openwam.webs.upv.es/docs/> for more documentation.
 */

// ---------------------------------------------------------------------------
#ifdef __BORLANDC__
#include <vcl.h>
#endif

#pragma hdrstop

#include "TOpenWAM.h"
#include "labels.hpp"

// #include <tchar.h>
// ---------------------------------------------------------------------------

#pragma argsused

TOpenWAM* Aplication = NULL;

int main(int argc, char *argv[]) {

	init_labels();

	Aplication = new TOpenWAM();

	Aplication->ReadInputData(argv[1]);

	Aplication->ConnectFlowElements();

	Aplication->ConnectControlElements();

	Aplication->InitializeParameters();

	Aplication->InitializeOutput();

	Aplication->ProgressBegin();

	if (Aplication->IsIndependent()) {

		do {

			Aplication->Progress();

			Aplication->DetermineTimeStepIndependent();

			Aplication->NewEngineCycle();

			Aplication->CalculateFlowIndependent();

			Aplication->ManageOutput();

		} while (!Aplication->CalculationEnd());
	} else {
		do {

			Aplication->Progress();

			Aplication->DetermineTimeStepCommon();

			Aplication->NewEngineCycle();

			Aplication->CalculateFlowCommon();

			Aplication->ManageOutput();

		} while (!Aplication->CalculationEnd());
	}

	Aplication->GeneralOutput();

	Aplication->ProgressEnd();

	delete Aplication;

	return 0;

}
// ---------------------------------------------------------------------------
