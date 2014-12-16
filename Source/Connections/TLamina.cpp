/* --------------------------------------------------------------------------------*\
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


 \*-------------------------------------------------------------------------------- */

// ---------------------------------------------------------------------------
#pragma hdrstop

#include "TLamina.h"
#include "TTubo.h"
#include "TCilindro.h"
#include "TDeposito.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TLamina::TLamina() :
		TTipoValvula(nmLamina) {

	// FDatosCDEntrada=NULL;
	// FDatosCDSalida=NULL;
	FFuerza = NULL;
	FLev1 = NULL;
	FLev2 = NULL;
	FLev3 = NULL;
	FTime0 = 0;
	fun_CDin = NULL;
	fun_CDout = NULL;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TLamina::~TLamina() {

	// if(FDatosCDEntrada!=NULL)delete[] FDatosCDEntrada;
	// if(FDatosCDSalida!=NULL) delete[] FDatosCDSalida;
	if (FFuerza != NULL)
		delete[] FFuerza;
	if (FLev1 != NULL)
		delete[] FLev1;
	if (FLev2 != NULL)
		delete[] FLev2;
	if (FLev3 != NULL)
		delete[] FLev3;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TLamina::TLamina(TLamina *Origen, int Valvula) :
		TTipoValvula(nmLamina) {

	FValvula = Valvula;

	FFuerza = NULL;
	FLev1 = NULL;
	FLev2 = NULL;
	FLev3 = NULL;

	FTipoLamina = Origen->FTipoLamina;
	FSentidoLamina = Origen->FSentidoLamina;
	FDiametroRef = Origen->FDiametroRef;
	FNumeroOrden = Origen->FNumeroOrden;

	FDiamRef = FDiametroRef;

	FMasa = Origen->FMasa;
	FDensidad = Origen->FDensidad;
	FAmortiguamiento = Origen->FAmortiguamiento;
	FArea = Origen->FArea;
	FAnchoPetalo = Origen->FAnchoPetalo;
	FRigidez = Origen->FRigidez;
	FModuloYoung = Origen->FModuloYoung;
	FEspesor = Origen->FEspesor;
	FNumPestanyas = Origen->FNumPestanyas;
	FLongitud = Origen->FLongitud;
	FLongReal = Origen->FLongReal;

	FKCDE = Origen->FKCDE;
	FKCDS = Origen->FKCDS;

	FNumLevCDE = Origen->FNumLevCDE;
	FNumLevCDS = Origen->FNumLevCDS;
	FIncrLev = Origen->FIncrLev;

	FLiftCDin.resize(Origen->FLiftCDin.size());
	FDatosCDEntrada.resize(Origen->FDatosCDEntrada.size());
	FLiftCDout.resize(Origen->FLiftCDout.size());
	FDatosCDSalida.resize(Origen->FDatosCDSalida.size());
	for (int i = 0; i < FDatosCDEntrada.size(); i++) {
		FLiftCDin[i] = Origen->FLiftCDin[i];
		FDatosCDEntrada[i] = Origen->FDatosCDEntrada[i];
	}
	fun_CDin = new Hermite_interp(FLiftCDin, FDatosCDEntrada);

	for (int i = 0; i < FDatosCDSalida.size(); i++) {
		FLiftCDout[i] = Origen->FLiftCDout[i];
		FDatosCDSalida[i] = Origen->FDatosCDSalida[i];
	}
	fun_CDout = new Hermite_interp(FLiftCDout, FDatosCDSalida);

	if (FTipoLamina == nmLamina2D) {
		FNodosLamina = 15;
		FDeltaX = FLongitud / double(FNodosLamina - 1);
		FNodosFijos = floor((FLongReal - FLongitud) / FDeltaX);
		FCoefC = FModuloYoung * FEspesor * FEspesor / 12. / FDensidad;
		FAreaTrans = FEspesor * FAnchoPetalo * FNumPestanyas;
		FFuerza = new double[FNodosLamina + FNodosFijos + 2];
		FLev1 = new double[FNodosLamina + FNodosFijos + 3];
		FLev2 = new double[FNodosLamina + FNodosFijos + 3];
		FLev3 = new double[FNodosLamina + FNodosFijos + 3];
		for (int i = 0; i < FNodosLamina + FNodosFijos + 3; i++) {
			FLev1[i] = 0.;
			FLev2[i] = 0.;
			FLev3[i] = 0.;
		}
		FBucle = 10;
	}
	FGraficasLam = false;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TLamina::LeeDatosIniciales(const char *FileWAM, fpos_t &filepos,
		int norden, bool HayMotor, TBloqueMotor *Engine) {
	try {
		int tipo, sentido;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		FNumeroOrden = norden;

		fscanf(fich, "%d %lf %d ", &tipo, &FDiametroRef, &sentido);
		switch (tipo) {
		case 0:
			FTipoLamina = nmLamina0D;
			break;
		case 1:
			FTipoLamina = nmLamina1D;
			break;
		case 3:
			FTipoLamina = nmLamina2D;
			break;
		default:
			std::cout << "ERROR: Seleccion Tipo de Lamina" << std::endl;
			break;
		}
		switch (sentido) {
		case 0:
			FSentidoLamina = 1;
			break;
		case 1:
			FSentidoLamina = -1;
			break;
		default:
			std::cout << "ERROR: Seleccion Sentido de Lamina" << std::endl;
			break;
		}

		if (FTipoLamina == nmLamina1D) {
			fscanf(fich, "%lf %lf %lf %lf ", &FMasa, &FAmortiguamiento,
					&FRigidez, &FArea);
		} else if (FTipoLamina == nmLamina2D) {
			fscanf(fich, "%lf %lf %lf %lf %lf %d %lf %lf ", &FDensidad,
					&FAmortiguamiento, &FModuloYoung, &FAnchoPetalo, &FEspesor,
					&FNumPestanyas, &FLongitud, &FLongReal);
		}
		fscanf(fich, "%d %d ", &FNumLevCDE, &FNumLevCDS);

		FLiftCDin.resize(FNumLevCDE);
		FDatosCDEntrada.resize(FNumLevCDE);
		FLiftCDout.resize(FNumLevCDS);
		FDatosCDSalida.resize(FNumLevCDS);

		fscanf(fich, "%lf ", &FIncrLev);
		fscanf(fich, "%lf %lf ", &FKCDE, &FKCDS);
		for (int j = 0; j < FNumLevCDE; ++j) {
			FLiftCDin[j] = (double) j * FIncrLev;
			fscanf(fich, "%lf ", &FDatosCDEntrada[j]);
		}
		for (int j = 0; j < FNumLevCDS; ++j) {
			FLiftCDout[j] = (double) j * FIncrLev;
			fscanf(fich, "%lf ", &FDatosCDSalida[j]);
		}

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch (Exception & N) {
		std::cout << "ERROR: LeeDatosIniciales Lamina" << std::endl;
		// std::cout << "Tipo de error: " << N.Message.scr() << std::endl;
		throw Exception(N.Message);

	}
}

void TLamina::LeeDatosInicialesXML(xml_node node_valve, int norden,
		bool HayMotor, TBloqueMotor *Engine) {
	try {

		FNumeroOrden = norden;

		xml_node node_reed = GetNodeChild(node_valve, "Val:ReedValve");

		const char_t* Type = node_reed.attribute("Type").value();
		if (Type == "0D") {
			FTipoLamina = nmLamina0D;
		} else if (Type == "1D") {
			FTipoLamina = nmLamina1D;
		} else if (Type == "2D") {
			FTipoLamina = nmLamina2D;
		}

		FDiametroRef = GetAttributeAsDouble(node_reed, "RefDiameter");

		const char_t* Mode = node_reed.attribute("Mode").value();
		if (Mode == "Positive") {
			FSentidoLamina = 1;
		} else if (Mode == "Negative") {
			FSentidoLamina = -1;
		}

		if (FTipoLamina == nmLamina1D) {
			xml_node node_1d = GetNodeChild(node_reed, "RdV:Valve1D");
			FMasa = GetAttributeAsDouble(node_1d, "Mass");
			FAmortiguamiento = GetAttributeAsDouble(node_1d, "Damping");
			FRigidez = GetAttributeAsDouble(node_1d, "Stiffness");
			FArea = GetAttributeAsDouble(node_1d, "Area");
		} else if (FTipoLamina == nmLamina2D) {
			xml_node node_2d = GetNodeChild(node_reed, "RdV:Valve2D");
			FDensidad = GetAttributeAsDouble(node_2d, "Density");
			FAmortiguamiento = GetAttributeAsDouble(node_2d, "Damping");
			FModuloYoung = GetAttributeAsDouble(node_2d, "Stiffness");
			FAnchoPetalo = GetAttributeAsDouble(node_2d, "PetalWidth");
			FEspesor = GetAttributeAsDouble(node_2d, "Thickness");
			FNumPestanyas = GetAttributeAsDouble(node_2d, "NumPetals");
			FLongitud = GetAttributeAsDouble(node_2d, "Length");
			FLongReal = GetAttributeAsDouble(node_2d, "RealLEngth");
		}
		FNumLevCDE = CountNodes(node_reed, "RdV:DCin");
		FNumLevCDS = CountNodes(node_reed, "RdV:DCout");

		FLiftCDin.resize(FNumLevCDE);
		FDatosCDEntrada.resize(FNumLevCDE);
		FLiftCDout.resize(FNumLevCDS);
		FDatosCDSalida.resize(FNumLevCDS);

		FIncrLev = GetAttributeAsDouble(node_reed, "LiftStep");
		FKCDE = GetAttributeAsDouble(node_reed, "MultDCin");
		FKCDS = GetAttributeAsDouble(node_reed, "MultDCout");

		int j = 0;
		for (xml_node node_dcin = GetNodeChild(node_reed, "RdV:DCin");
				node_dcin; node_dcin = node_dcin.next_sibling("RdV:DCin")) {
			FLiftCDin[j] = (double) j * FIncrLev;
			FDatosCDEntrada[j] = GetAttributeAsDouble(node_dcin, "DCin");
			++j;
		}
		j = 0;
		for (xml_node node_dcin = GetNodeChild(node_reed, "RdV:DCout");
				node_dcin; node_dcin = node_dcin.next_sibling("RdV:DCout")) {
			FLiftCDout[j] = (double) j * FIncrLev;
			FDatosCDSalida[j] = GetAttributeAsDouble(node_dcin, "DCout");
			++j;
		}
	} catch (Exception & N) {
		std::cout << "ERROR: LeeDatosIniciales Lamina" << std::endl;
		// std::cout << "Tipo de error: " << N.Message.scr() << std::endl;
		throw Exception(N.Message);

	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TLamina::CalculaCD(double deltaP, double ttotal) {
	try {
		double X;
		int Indice;
		double deltaT = ttotal - FTime0;
		FTime0 = ttotal;

		// deltaP=FPipe->GetPresion(FPipeNode)-FCylinder->getPressure();

		double deltaP2 = deltaP * (double) FSentidoLamina;

		switch (FTipoLamina) {
		case nmLamina0D:
			if (deltaP2 >= 0.) {
				FCDTubVol = fun_CDin->interp(deltaP2) * FKCDE;
				FCDVolTub = fun_CDout->interp(deltaP2) * FKCDS;
			} else {
				FCDTubVol = 0.;
				FCDVolTub = 0.;
			}
			break;
		case nmLamina1D:
			FddLev = (deltaP2 * 1e5 * FArea - FRigidez * FLev
					- FAmortiguamiento * FdLev) / FMasa;
			FLev = FLev + FdLev * deltaT + FddLev / 2 * pow2(deltaT);
			FdLev = FdLev + FddLev * deltaT;

			if (FLev > 99999911.0e-3) {
				FLev = 99999911.0e-3;
				FdLev = 0.;
			}
			if (FLev > 0) {
				FCDTubVol = fun_CDin->interp(FLev) * FKCDE;
				FCDVolTub = fun_CDout->interp(FLev) * FKCDS;
			} else {
				FLev = 0.;
				FdLev = 0.;
				FCDTubVol = 0.;
				FCDVolTub = 0.;
			}
			break;
		case nmLamina2D:
			for (int i = 0; i < (FNodosLamina + FNodosFijos + 2); ++i) {
				FFuerza[i] = deltaP2 * 1e5 * FAnchoPetalo * FNumPestanyas
						/ FDensidad / FAreaTrans;
				if (i < FNodosFijos + 1) {
					FFuerza[i] = 0.;
				}
				if (i == FNodosFijos + 1
						|| i == FNodosFijos + FNodosLamina + 1) {
					FFuerza[i] *= 0.5;
				}
			}
			double deltaT2 = deltaT / (double) FBucle;
			for (int veces = 0; veces <= FBucle; ++veces) {
				FLev2[0] = FLev2[2];
				FLev2[FNodosLamina + FNodosFijos + 2] = 2
						* FLev2[FNodosLamina + FNodosFijos + 1]
						- FLev2[FNodosLamina + FNodosFijos];
				for (int i = 2; i <= FNodosLamina + FNodosFijos; ++i) {
					FDerivada4 = FLev2[i - 2] - 4 * FLev2[i - 1] + 6 * FLev2[i]
							- 4 * FLev2[i + 1] + FLev2[i + 2];
					FDerivada4 = FDerivada4 / pow4(FDeltaX);
					FLev3[i] = pow2(deltaT2)
							* (FFuerza[i] - FCoefC * FDerivada4)
							+ (2 * FAmortiguamiento * deltaT2) * FLev2[i]
							- FLev1[i];
					FLev3[i] = FLev3[i] / (1 + FAmortiguamiento * deltaT2);
					if (FLev3[i] < 0)
						FLev3[i] = 0.;
				}
				FLev3[1] = 0.;
				FLev3[FNodosLamina + FNodosFijos + 1] = 2
						* FLev3[FNodosLamina + FNodosFijos]
						- FLev3[FNodosLamina + FNodosFijos - 1];
				if (FLev3[FNodosLamina + FNodosFijos + 1] < 0.)
					FLev3[FNodosLamina + FNodosFijos + 1] = 0;
				for (int i = 1; i <= FNodosLamina + FNodosFijos + 1; ++i) {
					FLev1[i] = FLev2[i];
					FLev2[i] = FLev3[i];
				}
			}
			FLev = FLev3[FNodosLamina + FNodosFijos + 1];

			if (FLev > 99999911.0e-3) {
				FLev = 99999911.0e-3;
				FdLev = 0.;
			}
			if (FLev > 0) {
				FCDTubVol = fun_CDin->interp(FLev) * FKCDE;
				FCDVolTub = fun_CDout->interp(FLev) * FKCDS;
			} else {
				FLev = 0.;
				FCDTubVol = 0.;
				FCDVolTub = 0.;
			}

			break;
		}
		FCDTubVol *= FSectionRatio;
		FCDVolTub *= FSectionRatio;
	} catch (Exception & N) {
		std::cout << "ERROR: CalculaCD Lamina" << std::endl;
		// std::cout << "Tipo de error: " << N.Message.scr() << std::endl;
		throw Exception(N.Message);

	}
}

void TLamina::GetCDin(double Time) {

	double deltaT = Time - FTime0;
	FTime0 = Time;
	double deltaP;

	if (FToCylinder) {
		deltaP = (FPipe->GetPresion(FPipeNode) - FCylinder->getPressure())
				* (double) FSentidoLamina;
	} else {
		deltaP = (FPipe->GetPresion(FPipeNode) - FPlenum->getPressure())
				* (double) FSentidoLamina;
	}

	switch (FTipoLamina) {
	case nmLamina0D:
		if (deltaP >= 0.) {
			FCDTubVol = fun_CDin->interp(deltaP) * FKCDE;
		} else {
			FCDTubVol = 0.;
		}
		break;
	case nmLamina1D:
		FddLev = (deltaP * 1e5 * FArea - FRigidez * FLev
				- FAmortiguamiento * FdLev) / FMasa;
		FLev = FLev + FdLev * deltaT + FddLev / 2 * pow2(deltaT);
		FdLev = FdLev + FddLev * deltaT;

		if (FLev > 99999911.0e-3) {
			FLev = 99999911.0e-3;
			FdLev = 0.;
		}
		if (FLev > 0) {
			FCDTubVol = fun_CDin->interp(FLev) * FKCDE;
		} else {
			FLev = 0.;
			FdLev = 0.;
			FCDTubVol = 0.;
			;
		}
		break;
	case nmLamina2D:
		for (int i = 0; i < (FNodosLamina + FNodosFijos + 2); ++i) {
			FFuerza[i] = deltaP * 1e5 * FAnchoPetalo * FNumPestanyas / FDensidad
					/ FAreaTrans;
			if (i < FNodosFijos + 1) {
				FFuerza[i] = 0.;
			}
			if (i == FNodosFijos + 1 || i == FNodosFijos + FNodosLamina + 1) {
				FFuerza[i] *= 0.5;
			}
		}
		double deltaT2 = deltaT / (double) FBucle;
		for (int veces = 0; veces <= FBucle; ++veces) {
			FLev2[0] = FLev2[2];
			FLev2[FNodosLamina + FNodosFijos + 2] = 2
					* FLev2[FNodosLamina + FNodosFijos + 1]
					- FLev2[FNodosLamina + FNodosFijos];
			for (int i = 2; i <= FNodosLamina + FNodosFijos; ++i) {
				FDerivada4 = FLev2[i - 2] - 4 * FLev2[i - 1] + 6 * FLev2[i]
						- 4 * FLev2[i + 1] + FLev2[i + 2];
				FDerivada4 = FDerivada4 / pow4(FDeltaX);
				FLev3[i] = pow2(deltaT2) * (FFuerza[i] - FCoefC * FDerivada4)
						+ (2 * FAmortiguamiento * deltaT2) * FLev2[i]
						- FLev1[i];
				FLev3[i] = FLev3[i] / (1 + FAmortiguamiento * deltaT2);
				if (FLev3[i] < 0)
					FLev3[i] = 0.;
			}
			FLev3[1] = 0.;
			FLev3[FNodosLamina + FNodosFijos + 1] = 2
					* FLev3[FNodosLamina + FNodosFijos]
					- FLev3[FNodosLamina + FNodosFijos - 1];
			if (FLev3[FNodosLamina + FNodosFijos + 1] < 0.)
				FLev3[FNodosLamina + FNodosFijos + 1] = 0;
			for (int i = 1; i <= FNodosLamina + FNodosFijos + 1; ++i) {
				FLev1[i] = FLev2[i];
				FLev2[i] = FLev3[i];
			}
		}
		FLev = FLev3[FNodosLamina + FNodosFijos + 1];

		if (FLev > 99999911.0e-3) {
			FLev = 99999911.0e-3;
			FdLev = 0.;
		}
		if (FLev > 0) {
			FCDTubVol = fun_CDin->interp(FLev) * FKCDE;
		} else {
			FLev = 0.;
			FCDTubVol = 0.;
		}

		break;
	}
	FCDTubVol *= FSectionRatio;
}

void TLamina::GetCDout(double Time) {

	double deltaT = Time - FTime0;
	FTime0 = Time;
	double deltaP;

	if (FToCylinder) {
		deltaP = (FPipe->GetPresion(FPipeNode) - FCylinder->getPressure())
				* (double) FSentidoLamina;
	} else {
		deltaP = (FPipe->GetPresion(FPipeNode) - FPlenum->getPressure())
				* (double) FSentidoLamina;
	}

	switch (FTipoLamina) {
	case nmLamina0D:
		if (deltaP >= 0.) {
			FCDVolTub = fun_CDout->interp(deltaP) * FKCDS;
		} else {
			FCDVolTub = 0.;
		}
		break;
	case nmLamina1D:
		FddLev = (deltaP * 1e5 * FArea - FRigidez * FLev
				- FAmortiguamiento * FdLev) / FMasa;
		FLev = FLev + FdLev * deltaT + FddLev / 2 * pow2(deltaT);
		FdLev = FdLev + FddLev * deltaT;

		if (FLev > 99999911.0e-3) {
			FLev = 99999911.0e-3;
			FdLev = 0.;
		}
		if (FLev > 0) {
			FCDVolTub = fun_CDout->interp(FLev) * FKCDS;
		} else {
			FLev = 0.;
			FdLev = 0.;
			FCDVolTub = 0.;
		}
		break;
	case nmLamina2D:
		for (int i = 0; i < (FNodosLamina + FNodosFijos + 2); ++i) {
			FFuerza[i] = deltaP * 1e5 * FAnchoPetalo * FNumPestanyas / FDensidad
					/ FAreaTrans;
			if (i < FNodosFijos + 1) {
				FFuerza[i] = 0.;
			}
			if (i == FNodosFijos + 1 || i == FNodosFijos + FNodosLamina + 1) {
				FFuerza[i] *= 0.5;
			}
		}
		double deltaT2 = deltaT / (double) FBucle;
		for (int veces = 0; veces <= FBucle; ++veces) {
			FLev2[0] = FLev2[2];
			FLev2[FNodosLamina + FNodosFijos + 2] = 2
					* FLev2[FNodosLamina + FNodosFijos + 1]
					- FLev2[FNodosLamina + FNodosFijos];
			for (int i = 2; i <= FNodosLamina + FNodosFijos; ++i) {
				FDerivada4 = FLev2[i - 2] - 4 * FLev2[i - 1] + 6 * FLev2[i]
						- 4 * FLev2[i + 1] + FLev2[i + 2];
				FDerivada4 = FDerivada4 / pow4(FDeltaX);
				FLev3[i] = pow2(deltaT2) * (FFuerza[i] - FCoefC * FDerivada4)
						+ (2 * FAmortiguamiento * deltaT2) * FLev2[i]
						- FLev1[i];
				FLev3[i] = FLev3[i] / (1 + FAmortiguamiento * deltaT2);
				if (FLev3[i] < 0)
					FLev3[i] = 0.;
			}
			FLev3[1] = 0.;
			FLev3[FNodosLamina + FNodosFijos + 1] = 2
					* FLev3[FNodosLamina + FNodosFijos]
					- FLev3[FNodosLamina + FNodosFijos - 1];
			if (FLev3[FNodosLamina + FNodosFijos + 1] < 0.)
				FLev3[FNodosLamina + FNodosFijos + 1] = 0;
			for (int i = 1; i <= FNodosLamina + FNodosFijos + 1; ++i) {
				FLev1[i] = FLev2[i];
				FLev2[i] = FLev3[i];
			}
		}
		FLev = FLev3[FNodosLamina + FNodosFijos + 1];

		if (FLev > 99999911.0e-3) {
			FLev = 99999911.0e-3;
			FdLev = 0.;
		}
		if (FLev > 0) {
			FCDVolTub = fun_CDout->interp(FLev) * FKCDS;
		} else {
			FLev = 0.;
			FCDVolTub = 0.;
		}

		break;
	}
	FCDVolTub *= FSectionRatio;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TLamina::LeeDatosGraficas(char *FileWAM, fpos_t &filepos) {
	try {
		int ndv, var;
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);
		FGraficasLam = true;
		FGrafLev = false;
		fscanf(fich, " %d", &ndv);
		for (int i = 0; i < ndv; i++) {
			fscanf(fich, " %d", &var);
			switch (var) {
			case 0:
				FGrafLev = true;
				break;
			}
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch (Exception & N) {
		std::cout << "ERROR: LeeDatosGraficas Lamina" << std::endl;
		// std::cout << "Tipo de error: " << N.Message.scr() << std::endl;
		throw Exception(N.Message);

	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TLamina::CabeceraGraficaINS(stringstream& insoutput, int lam) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");
		AnsiString Label;

		if (FGraficasLam) {
			if (FGrafLev) {
				Label = "\t" + PutLabel(14) + IntToStr(lam) + PutLabel(902);
				insoutput << Label.c_str();
			}
		}
		// fclose(fich);
	} catch (Exception & N) {
		std::cout << "ERROR: CabeceraGrafica Lamina" << std::endl;
		// std::cout << "Tipo de error: " << N.Message.scr() << std::endl;
		throw Exception(N.Message);

	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TLamina::ImprimeGraficaINS(stringstream& insoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");
		if (FGraficasLam) {
			if (FGrafLev)
				insoutput << "\t" << FLev;
		}
		// fclose(fich);
	} catch (Exception & N) {
		std::cout << "ERROR: ImprimeGrafica Lamina" << std::endl;
		// std::cout << "Tipo de error: " << N.Message.scr() << std::endl;
		throw Exception(N.Message);

	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#pragma package(smart_init)
