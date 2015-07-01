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

#include "TMariposa.h"

//---------------------------------------------------------------------------

TMariposa::TMariposa() :
		TTipoValvula(nmMariposa) {
	FLevControlled = false;
	fun_CDin = NULL;
	fun_CDout = NULL;
}

TMariposa::~TMariposa() {
}

TMariposa::TMariposa(TMariposa *Origen, int valv) :
		TTipoValvula(nmMariposa) {
	FNumLev = Origen->FNumLev;

	FDiametroRef = Origen->FDiametroRef;

	FDiamRef = FDiametroRef;

	FValvula = valv;

	FLevantamiento.resize(Origen->FLevantamiento.size());
	FDatosCDEntrada.resize(Origen->FDatosCDEntrada.size());
	FDatosCDSalida.resize(Origen->FDatosCDSalida.size());

	for (int i = 0; i < FNumLev; i++) {
		FLevantamiento[i] = Origen->FLevantamiento[i];
		FDatosCDEntrada[i] = Origen->FDatosCDEntrada[i];
		FDatosCDSalida[i] = Origen->FDatosCDSalida[i];
	}
	fun_CDin = new Hermite_interp(FLevantamiento, FDatosCDEntrada);
	fun_CDout = new Hermite_interp(FLevantamiento, FDatosCDSalida);

	FLevActual = Origen->FLevActual;
	FLevControlled = Origen->FLevControlled;
	FControllerID = Origen->FControllerID;
	FLevControlled = Origen->FLevControlled;
}

void TMariposa::LeeDatosIniciales(const char *FileWAM, fpos_t &filepos,
		int norden, bool HayMotor, TBloqueMotor *Engine) {
	int ctrl = 0, prm = 0;

	FILE *fich = fopen(FileWAM, "r");
	fsetpos(fich, &filepos);

	FNumeroOrden = norden;

	fscanf(fich, "%d %lf ", &FNumLev, &FDiametroRef);

	FLevantamiento.resize(FNumLev);
	FDatosCDEntrada.resize(FNumLev);
	FDatosCDSalida.resize(FNumLev);

	for (int i = 0; i < FNumLev; i++) {
		fscanf(fich, " %lf %lf %lf", &FLevantamiento[i], &FDatosCDEntrada[i],
				&FDatosCDSalida[i]);
	}

	fscanf(fich, " %lf", &FLevActual);

	fscanf(fich, " %d", &ctrl);
	if (ctrl != 0) {
		FLevControlled = true;
		fscanf(fich, " %d %d", &prm, &FControllerID);
	}

	fgetpos(fich, &filepos);
	fclose(fich);
}

void TMariposa::LeeDatosInicialesXML(xml_node node_valve, int norden,
		bool HayMotor, TBloqueMotor *Engine) {
	int ctrl, prm;

	FNumeroOrden = norden;

	xml_node node_throttle = GetNodeChild(node_valve, "Val:RotaryValve");
	xml_node node_units = GetNodeChild(node_throttle, "Units");
	const char_t* LengthUnits = node_units.attribute("Length").value();

	FDiametroRef = GetXMLLength(node_throttle, "RefDiameter");
	FLevActual = GetXMLLength(node_throttle, "Lift");

	FNumLev = CountNodes(node_throttle, "Thr:Lift");

	FLevantamiento.resize(FNumLev);
	FDatosCDEntrada.resize(FNumLev);
	FDatosCDSalida.resize(FNumLev);

	int i = 0;
	for (xml_node node_lift = GetNodeChild(node_throttle, "Thr:Lift");
			node_lift; node_lift = node_lift.next_sibling("Thr:Lift")) {
		FLevantamiento[i] = GetXMLLength(node_lift, "Lift", LengthUnits);
		FDatosCDEntrada[i] = GetAttributeAsDouble(node_lift, "DCin");
		FDatosCDSalida[i] = GetAttributeAsDouble(node_lift, "DCin");
		i++;
	}
	ctrl = CountNodes(node_throttle, "Actuator");
	if (ctrl != 0) {
		FLevControlled = true;
		xml_node node_ctrl = GetNodeChild(node_throttle, "Actuator");
		FControllerID = GetAttributeAsInt(node_ctrl, "CtrlID");
	}

}

void TMariposa::AsignaLevController(TController **Controller) {
	if (FLevControlled)
		FController = Controller[FControllerID - 1];
}

void TMariposa::CalculaCD(double Time) {
	if (FLevControlled)
		FLevActual = FController->Output(Time);

	FCDTubVol = fun_CDin->interp(FLevActual) * FSectionRatio;
	FCDVolTub = fun_CDout->interp(FLevActual) * FSectionRatio;
}

void TMariposa::GetCDin(double Time) {

	if (FLevControlled)
		FLevActual = FController->Output(Time);

	FCDTubVol = fun_CDin->interp(FLevActual) * FSectionRatio;
}

void TMariposa::GetCDout(double Time) {

	if (FLevControlled)
		FLevActual = FController->Output(Time);

	FCDVolTub = fun_CDout->interp(FLevActual) * FSectionRatio;
}

#pragma package(smart_init)
