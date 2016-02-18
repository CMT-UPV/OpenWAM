/*--------------------------------------------------------------------------------*\
 *==========================|
 *\\   /\ /\   // O pen     | OpenWAM: The Open Source 1D Gas-Dynamic Code
 * \\ |  X  | //  W ave     |
 *  \\ \/_\/ //   A ction   | CMT-Motores Termicos / Universidad Politecnica Valencia
 *   \\/   \//    M odel    |
 *----------------------------------------------------------------------------------
 *License
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
 * along with OpenWAM.  If not, see <http://www.gnu.org/licenses/>.
 *
 * TGain.cpp
 *
 * Created on: 14/07/2014
 *     Author: farnau
 *
 \*--------------------------------------------------------------------------------*/

#include "TGain.h"

TGain::TGain(int i) :
	TController(nmGainCtrl, i) {
	fID = i + 1;
	FGain = 1.;
}

TGain::~TGain() {

}

double TGain::Output(double Time) {

	fOutput = FGainInput->Output(Time) * FGain;
	return fOutput;
}

void TGain::LeeController(const char *FileWAM, fpos_t &filepos) {

	int Type = 0;

	FILE *fich = fopen(FileWAM, "r");
	fsetpos(fich, &filepos);

	fscanf(fich, "%lf ", &FGain);
	fscanf(fich, "%d %d", &Type, &FObjectID);
	if(Type == 0) {
		FInObject = nmInController;
	} else {
		FInObject = nmInSensor;
	}

	fgetpos(fich, &filepos);
	fclose(fich);

}

void TGain::AsignaObjetos(TSensor **Sensor, TController **Controller) {

	if(FInObject == nmInController) {
		FGainInput = new stGainInputController(Controller[FObjectID - 1]);
	} else {
		FGainInput = new stGainInputSensor(Sensor[FObjectID - 1]);
	}
}

void TGain::LeeResultadosMedControlador(const char *FileWAM, fpos_t &filepos) {

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

void TGain::LeeResultadosInsControlador(const char *FileWAM, fpos_t &filepos) {

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

void TGain::CabeceraResultadosMedControlador(stringstream& medoutput) {

	std::string Label;

	if(FResMediosCtrl.Output) {
		Label = "\t" + PutLabel(705) + std::to_string(fID) + PutLabel(901);
		medoutput << Label.c_str();
	}

}

void TGain::CabeceraResultadosInsControlador(stringstream& insoutput) {

	std::string Label;

	if(FResInstantCtrl.Output) {
		Label = "\t" + PutLabel(705) + std::to_string(fID) + PutLabel(901);
		insoutput << Label.c_str();
	}

}

void TGain::ImprimeResultadosMedControlador(stringstream& medoutput) {

	std::string Label;

	if(FResMediosCtrl.Output) {
		medoutput << "\t" << FResMediosCtrl.OutputMED;
	}

}

void TGain::ImprimeResultadosInsControlador(stringstream& insoutput) {

	std::string Label;

	if(FResInstantCtrl.Output) {
		insoutput << "\t" << FResInstantCtrl.OutputINS;
	}

}

void TGain::IniciaMedias() {

	FResMediosCtrl.OutputSUM = 0.;

}

void TGain::ResultadosMediosController() {

	if(FResMediosCtrl.Output) {
		FResMediosCtrl.OutputMED = FResMediosCtrl.OutputSUM / FResMediosCtrl.TiempoSUM;
		FResMediosCtrl.OutputSUM = 0.;
	}

	FResMediosCtrl.TiempoSUM = 0;

}

void TGain::AcumulaResultadosMediosController(double Actual) {

	/* Lo que se hace en esta funcion se realiza dentro del calculo del eje, para asi poder
	 llevar a cabo la salida de resultados medios por pantalla. */
	double Delta = Actual - FResMediosCtrl.Tiempo0;

	if(FResMediosCtrl.Output) {
		FResMediosCtrl.OutputSUM += fOutput * Delta;
	}
	FResMediosCtrl.TiempoSUM += Delta;
	FResMediosCtrl.Tiempo0 = Actual;

}

void TGain::ResultadosInstantController() {

	if(FResInstantCtrl.Output)
		FResInstantCtrl.OutputINS = fOutput;

}
