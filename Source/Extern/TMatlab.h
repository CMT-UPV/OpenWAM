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
#ifndef TMatlabH
#define TMatlabH
//---------------------------------------------------------------------------
#include "engine.h"
#include <string.h>
#include <cstdio>
#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif

enum nmVersionMatlab {
	nmMatlab53 = 0, nmMatlab65 = 1
};

class TMatlab {
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
  private:
	// VARIABLES
	mxArray *FInputMatlab;
	mxArray *FOutputMatlab;
	Engine *ep;

	bool FOpenECU;

	int FNInputs;
	int FNOutputs;
	double *FInput;
	double *FOutput;

	char Fdirtrab[256];
	char Fworkspace[20];
	char Fficheme[20];
	nmVersionMatlab VersMatlab;

	void PutInput(int i, double valor);

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
  protected:

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
  public:
	// VARIABLES
	//__property double Input[int i]={write=PutInput};

//FUNCIONES
	double GetOutput(int i);

	// FUNCIONES

	TMatlab(int Inputs, int Outputs);

	~TMatlab();

	void IniciaECU();

	void FinalizaECU();

	void CalculaECU();

	void LeeFicherosECU(FILE *fich);

};

#endif

