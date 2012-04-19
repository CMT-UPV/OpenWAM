// ---------------------------------------------------------------------------

#ifdef __BORLANDC__
    #include <vcl.h>
#endif
#pragma hdrstop

#include "TOpenWAM.h"

// #include <tchar.h>
// ---------------------------------------------------------------------------

#pragma argsused

TOpenWAM* Aplication = NULL;

main(int argc, char *argv[]) {

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

		}
		while (!Aplication->CalculationEnd());
	}
	else {
 		do {

			Aplication->Progress();

			Aplication->DetermineTimeStepCommon();

			Aplication->NewEngineCycle();

			Aplication->CalculateFlowCommon();

			Aplication->ManageOutput();

		}
		while (!Aplication->CalculationEnd());
	}

	Aplication->GeneralOutput();

	delete Aplication;

}
// ---------------------------------------------------------------------------
