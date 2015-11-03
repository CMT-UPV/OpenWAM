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

#include "TEntradaPulso.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TEntradaPulso::TEntradaPulso() {

	FTiempo = NULL;
	FPresionRelativa = NULL;
	FNivelEntropia = NULL;
	FNumeroCiclo = 1;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TEntradaPulso::~TEntradaPulso() {
	if (FTiempo != NULL)
		delete FTiempo;
	if (FPresionRelativa != NULL)
		delete FPresionRelativa;
	if (FNivelEntropia != NULL)
		delete FNivelEntropia;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TEntradaPulso::LeeEntradaPulso(FILE *fich) {
	try {

		fscanf(fich, "%d ", &FNumeroDatos);
		FTiempo = new double[FNumeroDatos];
		FPresionRelativa = new double[FNumeroDatos];
		FNivelEntropia = new double[FNumeroDatos];
		for (int i = 0; i < FNumeroDatos; i++) {
			fscanf(fich, "%lf %lf %lf ", &FTiempo[i], &FPresionRelativa[i],
					&FNivelEntropia[i]);
		}
		if (FTiempo[0] > 0.0) {
			std::cout
					<< "WARNING: El primer instante de tiempo deberia ser 0 para evitar problemas"
					<< std::endl;
		}
	} catch (exception &N) {
		stringstream err;
		std::cout << "ERROR: TEntradaPulso::LeeEntradaPulso " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		err << "ERROR: TEntradaPulso::LeeEntradaPulso " << N.what();
		throw Exception(err.str());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TEntradaPulso::LeeEntradaPulsoXML(xml_node node_pulse,nmTypeBC FTipoCC) {
	try {

		FNumeroDatos = CountNodes(node_pulse,"Pvb_Pulse");
		FTiempo = new double[FNumeroDatos];
		FPresionRelativa = new double[FNumeroDatos];
		FNivelEntropia = new double[FNumeroDatos];

		xml_node unit_node = node_pulse.child("Units");
		std::string unittime = unit_node.attribute("Time").value();
		std::string unitpres = unit_node.attribute("Pressure").value();

		int i = 0;
		if(FTipoCC==nmPresionVble){
			std::string unittemp = unit_node.attribute("Temperature").value();
			for(xml_node node = GetNodeChild(node_pulse,"Pvb_Pulse"); node;
					node = node.next_sibling("Pvb_Pulse")){
				FTiempo[i] = GetXMLTime(node,"Time",unittime);
				FPresionRelativa[i] = GetXMLPressure(node,"Pressure",unitpres);
				FNivelEntropia[i] = GetXMLTemperature(node,"Entropy",unittemp);
				++i;
			}
		}else if(FTipoCC==nmIncidentPressurWave)
			for(xml_node node = GetNodeChild(node_pulse,"Pvb_Pulse"); node;
					node = node.next_sibling("Pvb_Pulse")){
				FTiempo[i] = GetXMLTime(node,"Time",unittime);
				FPresionRelativa[i] = GetXMLPressure(node,"Pressure",unitpres);
				FNivelEntropia[i] = GetAttributeAsDouble(node,"Entropy");
				++i;
		}

		if (FTiempo[0] > 0.0) {
			std::cout
					<< "WARNING: El primer instante de tiempo deberia ser 0 para evitar problemas"
					<< std::endl;
		}
	} catch (exception & N) {
		std::cout << "ERROR: TEntradaPulso::LeeEntradaPulso " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TEntradaPulso::BusquedaInstante(double Tiempo) {
	try {
		int i = 0;
		FTiempoActual = Tiempo - (FNumeroCiclo - 1) * FTiempo[FNumeroDatos - 1];
//std::cout << FTiempoActual << "\t" << FNumeroCiclo << "\t" << FNumeroCiclo*FTiempo[FNumeroDatos-1] << std::endl;
		if (FTiempoActual > FTiempo[FNumeroDatos - 1]) {
			++FNumeroCiclo;
			FTiempoActual = FTiempoActual - FTiempo[FNumeroDatos - 1];
			std::cout << "INFO: Comienza el ciclo " << FNumeroCiclo
					<< std::endl;
			std::cout << "      Tiempo transcurrido: " << Tiempo << std::endl;
		}
		while (Tiempo
				> FTiempo[i] + (FNumeroCiclo - 1) * FTiempo[FNumeroDatos - 1]) {
			++i;
		}
		FInstante = i;

	} catch (exception &N) {
		stringstream err;
		std::cout << "ERROR: TEntradaPulso::BusquedaInstante " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		err << "ERROR: TEntradaPulso::BusquedaInstante " << N.what();
		throw Exception(err.str());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TEntradaPulso::InterpolaPresion() {
	try {
		double deltat = (FTiempoActual - FTiempo[FInstante - 1])
				/ (FTiempo[FInstante] - FTiempo[FInstante - 1]);
		double presion = (1 - deltat) * FPresionRelativa[FInstante - 1]
				+ deltat * FPresionRelativa[FInstante];
		return presion;
	} catch (exception &N) {
		stringstream err;
		std::cout << "ERROR: TEntradaPulso::InterpolaPresion " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		err << "ERROR: TEntradaPulso::InterpolaPresion " << N.what();
		throw Exception(err.str());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TEntradaPulso::InterpolaEntropia() {
	try {
		double deltat = (FTiempoActual - FTiempo[FInstante - 1])
				/ (FTiempo[FInstante] - FTiempo[FInstante - 1]);
		double entropia = (1 - deltat) * FNivelEntropia[FInstante - 1]
				+ deltat * FNivelEntropia[FInstante];
		return entropia;
	} catch (exception &N) {
		stringstream err;
		std::cout << "ERROR: TEntradaPulso::InterpolaEntropia" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		err << "ERROR: TEntradaPulso::InterpolaEntropia " << N.what();
		throw Exception(err.str());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)
