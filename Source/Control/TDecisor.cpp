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

	if (fTargedControlled)
		fTarget = fControllerTarget->Output(Time);

	if (fInput < fTarget) {
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

	int ctrl;
	fscanf(fich, "%d ", &ctrl);
	if (ctrl == 0) {
		fTargedControlled = false;
	} else {
		fTargedControlled = true;
		fControllerTargetID = ctrl;
	}

	int tmp;
	FSensorID.resize(1);
	fscanf(fich, "%d ", &FSensorID[0]);

	fgetpos(fich, &filepos);
	fclose(fich);
}

void TDecisor::LeeControllerXML(xml_node node_ctrl) {

	xml_node node_sw = GetNodeChild(node_ctrl,"Ctr:Switch");

	fTarget = GetAttributeAsDouble(node_sw,"Target");
	fControllerLowID = GetAttributeAsInt(node_sw,"CtrlLow_ID");
	fControllerHighID = GetAttributeAsInt(node_sw,"CtrlHigh_ID");

	fTargedControlled = node_sw.attribute("Target_ID");
	if(fTargedControlled){
		fControllerTargetID = GetAttributeAsInt(node_sw,"Target_ID");
	}

	int tmp;
	FSensorID.resize(1);
	FSensorID[0] = GetAttributeAsInt(node_sw,"Sensor_ID");

	if(node_ctrl.child("Ctr:AvgOutput"))
		LeeResultadosMedControladorXML(node_ctrl);
	if(node_ctrl.child("Ctr:InsOutput"))
		LeeResultadosInsControladorXML(node_ctrl);
}

void TDecisor::AsignaObjetos(TSensor **Sensor, TController **Controller) {
	FSensor.push_back(Sensor[FSensorID[0] - 1]);

	fControllerLow = Controller[fControllerLowID - 1];
	fControllerHigh = Controller[fControllerHighID - 1];
	if (fTargedControlled)
		fControllerTarget = Controller[fControllerTargetID - 1];
}

void TDecisor::LeeResultadosMedControlador(const char *FileWAM,
		fpos_t &filepos) {
	int nvars, var;

	FILE *fich = fopen(FileWAM, "r");
	fsetpos(fich, &filepos);

	fscanf(fich, "%d ", &nvars);
	for (int i = 0; i < nvars; i++) {
		fscanf(fich, "%d ", &var);
		switch (var) {
		case 0:
			FResMediosCtrl.Output = true;
			break;
		default:
			std::cout << "Resultados medios en Controlador " << fID
					<< " no implementados " << std::endl;
		}
	}

	fgetpos(fich, &filepos);
	fclose(fich);
}

void TDecisor::LeeResultadosMedControladorXML(xml_node node_ctrl) {

	xml_node node_avg=GetNodeChild(node_ctrl,"Ctrl:AvgOutput");
	for(xml_attribute parameter=node_avg.attribute("Parameter"); parameter;
			parameter.next_attribute()){
		if(parameter.value() == "Output"){
			FResMediosCtrl.Output = true;
		}else{
			std::cout << "Resultados medios en Controlador " << fID
					<< " no implementados " << std::endl;
		}
	}
}

void TDecisor::LeeResultadosInsControlador(const char *FileWAM,
		fpos_t &filepos) {
	int nvars, var;

	FILE *fich = fopen(FileWAM, "r");
	fsetpos(fich, &filepos);

	fscanf(fich, "%d ", &nvars);
	for (int i = 0; i < nvars; i++) {
		fscanf(fich, "%d ", &var);
		switch (var) {
		case 0:
			FResInstantCtrl.Output = true;
			break;
		default:
			std::cout << "Resultados instantaneos en Controlador " << fID
					<< " no implementados " << std::endl;
		}
	}

	fgetpos(fich, &filepos);
	fclose(fich);
}

void TDecisor::LeeResultadosInsControladorXML(xml_node node_ctrl) {

	xml_node node_ins=GetNodeChild(node_ctrl,"Ctrl:InsOutput");
	for(xml_attribute parameter=node_ins.attribute("Parameter"); parameter;
			parameter.next_attribute()){
		if(parameter.value() == "Output"){
			FResInstantCtrl.Output = true;
		}else{
			std::cout << "Resultados instantaneos en Controlador " << fID
					<< " no implementados " << std::endl;
		}
	}
}



void TDecisor::CabeceraResultadosMedControlador(stringstream& medoutput) {
	AnsiString Label;

	if (FResMediosCtrl.Output) {
		Label = "\t" + PutLabel(5013) + "/" + IntToStr(fID) + "/"
				+ PutLabel(3051) + PutLabel(901);
		medoutput << Label.c_str();
	}

}

void TDecisor::CabeceraResultadosInsControlador(stringstream& insoutput) {
	AnsiString Label;

	if (FResInstantCtrl.Output) {
		Label = "\t" + PutLabel(5013) + "/" + IntToStr(fID) + "/"
				+ PutLabel(3051) + PutLabel(901);
		insoutput << Label.c_str();
	}
}

void TDecisor::ImprimeResultadosMedControlador(stringstream& medoutput) {
	AnsiString Label;

	if (FResMediosCtrl.Output) {
		medoutput << "\t" << FResMediosCtrl.OutputMED;
	}
}

void TDecisor::ImprimeResultadosInsControlador(stringstream& insoutput) {
	AnsiString Label;

	if (FResInstantCtrl.Output) {
		insoutput << "\t" << FResInstantCtrl.OutputINS;
	}
}

void TDecisor::IniciaMedias() {
	FResMediosCtrl.OutputSUM = 0.;
	FResMediosCtrl.TiempoSUM = 0.;
	FResMediosCtrl.Tiempo0 = 0.;
}

void TDecisor::ResultadosMediosController() {
	if (FResMediosCtrl.Output) {
		FResMediosCtrl.OutputMED = FResMediosCtrl.OutputSUM
				/ FResMediosCtrl.TiempoSUM;
		FResMediosCtrl.OutputSUM = 0.;
	}
}

void TDecisor::AcumulaResultadosMediosController(double Actual) {
	double Delta = Actual - FResMediosCtrl.Tiempo0;

	if (FResMediosCtrl.Output) {
		FResMediosCtrl.OutputSUM += fOutput * Delta;
	}
	FResMediosCtrl.TiempoSUM += Delta;
	FResMediosCtrl.Tiempo0 = Actual;
}

void TDecisor::ResultadosInstantController() {
	if (FResInstantCtrl.Output)
		FResInstantCtrl.OutputINS = fOutput;
}

#pragma package(smart_init)
