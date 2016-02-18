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
#pragma hdrstop

#include "TMatlab.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TMatlab::TMatlab(int Inputs, int Outputs) {
	FInput = NULL;
	FOutput = NULL;

	FNInputs = Inputs;
	FNOutputs = Outputs;

//VECTORES mxArray
	FInputMatlab = NULL;
	FOutputMatlab = NULL;

//DIMENSIONADO VECTORES DE COMUNICACION

	FInput = new double[FNInputs];
	FOutput = new double[FNOutputs];

//VersMatlab=nmMatlab53;
	VersMatlab = nmMatlab65;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TMatlab::~TMatlab() {
	if(FInput != NULL)
		delete[] FInput;

	if(FOutput != NULL)
		delete[] FOutput;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TMatlab::IniciaECU() {
	try {

		int i = 0;

		char daux[140];
		/* Inicializacion del acceso a MATLAB*/

		for(i = 0; i < 140; i++) {
			daux[i] = '\0';
		}

		/* Open a new instance of MATLAB */
		int retstatus = 0;
		ep = engOpenSingleUse("c:\matlab53\bin\matlab.exe", NULL, &retstatus);

		/* The pointer may not be for a valid instance.  Check. */
		int result = engEvalString(ep, "disp('foo')");

		if(result > 0) {
			/* It failed. */
			printf("\nERROR: no se puede abrir una nueva sesion de MATLAB !");
			engClose(ep);
			exit(0);
		}

		/* crean los mxarray para comunicar con MATLAB*/
		FInputMatlab = mxCreateDoubleMatrix(1, FNInputs, mxREAL);
		FOutputMatlab = mxCreateDoubleMatrix(1, FNOutputs, mxREAL);

		engEvalString(ep, "FInputMatlab=zeros(1,FNInputs);");
		engEvalString(ep, "FOutputMatlab=zeros(1,FNOutputs);");

		strcat(daux, "addpath '");
		i = 9;
		while(Fdirtrab[i - 9] != '\n') {
			daux[i] = Fdirtrab[i - 9];
			++i;
		}
		daux[i] = '\'';

		engEvalString(ep, daux);

		engEvalString(ep, Fworkspace);

		/* Fin de la inicializacion del acceso a MATLAB*/

	} catch(exception &N) {
		std::cout << "ERROR: IniciaECU" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TMatlab::FinalizaECU()

{
	try {
		engClose(ep);
	} catch(exception &N) {
		std::cout << "ERROR: FinalizaECU" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TMatlab::CalculaECU() {
	try {

		memcpy(mxGetPr(FInputMatlab), FInput, 2 * sizeof(double));
		engPutVariable(ep, "FInputMatlab", FInputMatlab);
		engEvalString(ep, Fficheme);

		FOutputMatlab = engGetVariable(ep, "FOutputMatlab");
		memcpy(FOutput, mxGetPr(FOutputMatlab), 3 * sizeof(double));
		mxDestroyArray(FOutputMatlab);

	} catch(exception &N) {
		std::cout << "ERROR: CalculaECU" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TMatlab::PutInput(int i, double valor) {
	try {
		if(i < FNInputs) {
			FInput[i] = valor;
		} else {
			std::cout << "WARNING: El valor de la entrada a matlab se sale de rango" << std::endl;
			std::cout << "         Revisa el acceso a matlab" << std::endl;
		}
	} catch(exception &N) {
		std::cout << "ERROR: PutInput" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TMatlab::GetOutput(int i) {
	try {
		if(i < FNOutputs) {
			return FOutput[i];
		} else {
			std::cout << "WARNING: El valor de la salida de matlab se sale de rango" << std::endl;
			std::cout << "         Revisa el acceso a matlab" << std::endl;
			return 0.;
		}
	} catch(exception &N) {
		std::cout << "ERROR: GetOutput" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void TMatlab::LeeFicherosECU(FILE *fich) {
	try {
		fgets(Fdirtrab, 256, fich); //Lee la ruta, maximo 256 Characters
		fscanf(fich, "%s", &Fworkspace);
		fscanf(fich, "%s", &Fficheme);
	} catch(exception &N) {
		std::cout << "ERROR: LeeFicherosECU" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)

