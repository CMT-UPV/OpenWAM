// ---------------------------------------------------------------------------

#include <iostream>

#include "TOpenWAM.h"

static TOpenWAM* Library = NULL;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

extern "C" __declspec(dllexport)void __stdcall INITCMODELF(void);

extern "C" __declspec(dllexport)void __stdcall LOADCMODELF(char*FileName);
//
// extern "C" __declspec(dllexport)void __stdcall UPDATEBOUNDARY(int i, double U0, double U1,
// double T0, double T1, double P0, double P1, double t);
//
// extern "C" __declspec(dllexport)void __stdcall INITIATEBOUNDARY(int i, double D0, double D1,
// double dX);

extern "C" __declspec(dllexport)void __stdcall UPDATEBOUNDARY(int i, double U0,
	double T0, double P0, double t);

extern "C" __declspec(dllexport)void __stdcall INITIATEBOUNDARY(int i,
	double D0, double dX);

extern "C" __declspec(dllexport)void __stdcall RUNSTEP(double t);

extern "C" __declspec(dllexport)void __stdcall LOADNEWDATA(int i, double*p,
	double*T, double*u);

extern "C" __declspec(dllexport)void __stdcall CLOSEMODEL(void);

extern "C" __declspec(dllexport)void __stdcall GETPLOT(int ID, double*val);

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void __stdcall INITCMODELF(void) {
	if (Library == NULL) {
		Library = new TOpenWAM();
	}
	else {
		delete Library;
		Library = new TOpenWAM();
	}
}

void __stdcall LOADCMODELF(char *FileName) {

	// printf("LOADCMODELF\n");

	char file[256];
	int i = 1;

	char *tmp = strstr(FileName, ".wam");
	tmp[4] = '\0';
	//
	// while (FileName[i] != '}' && i < 256) {
	// file[i - 1] = FileName[i];
	// i++;
	// }
	// file[i - 1] = '\0';

	Library->ReadInputData(FileName);

	Library->ConnectFlowElements();

	Library->ConnectControlElements();

	Library->InitializeParameters();

	Library->InitializeOutput();

}

// void __stdcall UPDATEBOUNDARY(int i, double U0, double U1, double T0, double T1, double P0,
// double P1, double t) {
//
// Library->UpdateExternalBoundary(i, U0, U1, T0, T1, P0, P1, t);
// }
//
// void __stdcall INITIATEBOUNDARY(int i, double D0, double D1, double dX) {
//
// Library->InitiateExternalBoundary(i, D0, D1, dX);
// }

void __stdcall UPDATEBOUNDARY(int i, double U0, double T0, double P0, double t)
{

	Library->UpdateExternalBoundary(i, U0, T0, P0, t);
}

void __stdcall INITIATEBOUNDARY(int i, double D0, double dX) {

	Library->InitiateExternalBoundary(i, D0, dX);
}

void __stdcall RUNSTEP(double t) {

	do {

		Library->DetermineTimeStep(t);

		Library->NewEngineCycle();


		Library->CalculateFlowCommon();


		Library->ManageOutput();

	}
	while (!Library->GetIs_EndStep());
}

void __stdcall LOADNEWDATA(int i, double* p, double* T, double* u) {

	Library->LoadNewData(i, p, T, u);
}

void __stdcall CLOSEMODEL(void) {

	Library->GeneralOutput();

	delete Library;
}

void __stdcall GETPLOT(int ID, double* val) {
	*val = Library->Get_Output(ID);
}
