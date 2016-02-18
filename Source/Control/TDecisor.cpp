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

#include "TDecisor.h"

//---------------------------------------------------------------------------

TDecisor::TDecisor(int i) :
	TController(nmCtlSwitch, i) {
	fID = i + 1;
}

TDecisor::~TDecisor() {

}

double TDecisor::Output(double Time) {
	fInput = FSensor[0]->Output();
	double OutputLow = fControllerLow->Output(Time);
	double OutputHigh = fControllerHigh->Output(Time);

	if(fTargedControlled)
		fTarget = fControllerTarget->Output(Time);

	if(fInput < fTarget) {
		fOutput = OutputLow;
	} else {
		fOutput = OutputHigh;
	}
	AcumulaResultadosMediosController(Time);

	return fOutput;
}

void TDecisor::LeeController(const char *FileWAM, fpos_t &filepos) {
	FILE *fich = fopen(FileWAM, "r");
	fsetpos(fich, &filepos);

	fscanf(fich, "%lf ", &fTarget);
	fscanf(fich, "%d ", &fControllerLowID);
	fscanf(fich, "%d ", &fControllerHighID);

	int ctrl = 0;
	fscanf(fich, "%d ", &ctrl);
	if(ctrl == 0) {
		fTargedControlled = false;
	} else {
		fTargedControlled = true;
		fControllerTargetID = ctrl;
	}

	int tmp = 0;
	FSensorID.resize(1);
	fscanf(fich, "%d ", &FSensorID[0]);

	fgetpos(fich, &filepos);
	fclose(fich);
}

void TDecisor::AsignaObjetos(TSensor **Sensor, TController **Controller) {
	FSensor.push_back(Sensor[FSensorID[0] - 1]);

	fControllerLow = Controller[fControllerLowID - 1];
	fControllerHigh = Controller[fControllerHighID - 1];
	if(fTargedControlled)
		fControllerTarget = Controller[fControllerTargetID - 1];
}

void TDecisor::LeeResultadosMedControlador(const char *FileWAM, fpos_t &filepos) {
	int nvars = 0, var = 0;

	FILE *fich = fopen(FileWAM, "r");
	fsetpos(fich, &filepos);

	fscanf(fich, "%d ", &nvars);
	for(int i = 0; i < nvars; i++) {
		fscanf(fich, "%d ", &var);
		switch(var) {
		case 0:
			FResMediosCtrl.Output = true;
			break;
		default:
			std::cout << "Resultados medios en Controlador " << fID << " no implementados " << std::endl;
		}
	}

	fgetpos(fich, &filepos);
	fclose(fich);
}

void TDecisor::LeeResultadosInsControlador(const char *FileWAM, fpos_t &filepos) {
	int nvars = 0, var = 0;

	FILE *fich = fopen(FileWAM, "r");
	fsetpos(fich, &filepos);

	fscanf(fich, "%d ", &nvars);
	for(int i = 0; i < nvars; i++) {
		fscanf(fich, "%d ", &var);
		switch(var) {
		case 0:
			FResInstantCtrl.Output = true;
			break;
		default:
			std::cout << "Resultados instantaneos en Controlador " << fID << " no implementados " << std::endl;
		}
	}

	fgetpos(fich, &filepos);
	fclose(fich);
}

void TDecisor::CabeceraResultadosMedControlador(stringstream& medoutput) {
	std::string Label;

	if(FResMediosCtrl.Output) {
		Label = "\t" + PutLabel(705) + std::to_string(fID) + PutLabel(901);
		medoutput << Label.c_str();
	}

}

void TDecisor::CabeceraResultadosInsControlador(stringstream& insoutput) {
	std::string Label;

	if(FResInstantCtrl.Output) {
		Label = "\t" + PutLabel(705) + std::to_string(fID) + PutLabel(901);
		insoutput << Label.c_str();
	}
}

void TDecisor::ImprimeResultadosMedControlador(stringstream& medoutput) {
	std::string Label;

	if(FResMediosCtrl.Output) {
		medoutput << "\t" << FResMediosCtrl.OutputMED;
	}
}

void TDecisor::ImprimeResultadosInsControlador(stringstream& insoutput) {
	std::string Label;

	if(FResInstantCtrl.Output) {
		insoutput << "\t" << FResInstantCtrl.OutputINS;
	}
}

void TDecisor::IniciaMedias() {
	FResMediosCtrl.OutputSUM = 0.;
	FResMediosCtrl.TiempoSUM = 0.;
	FResMediosCtrl.Tiempo0 = 0.;
}

void TDecisor::ResultadosMediosController() {
	if(FResMediosCtrl.Output) {
		FResMediosCtrl.OutputMED = FResMediosCtrl.OutputSUM / FResMediosCtrl.TiempoSUM;
		FResMediosCtrl.OutputSUM = 0.;
	}
}

void TDecisor::AcumulaResultadosMediosController(double Actual) {
	double Delta = Actual - FResMediosCtrl.Tiempo0;

	if(FResMediosCtrl.Output) {
		FResMediosCtrl.OutputSUM += fOutput * Delta;
	}
	FResMediosCtrl.TiempoSUM += Delta;
	FResMediosCtrl.Tiempo0 = Actual;
}

void TDecisor::ResultadosInstantController() {
	if(FResInstantCtrl.Output)
		FResInstantCtrl.OutputINS = fOutput;
}

#pragma package(smart_init)
