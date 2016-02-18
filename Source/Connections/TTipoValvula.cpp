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

#include "TTipoValvula.h"
#include "TTubo.h"
#include "TCilindro.h"
#include "TDeposito.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TTipoValvula::TTipoValvula(nmTipoValvula TipVal) {

	FTipoValvula = TipVal;
	FCRecuperacion = 0;
	FGraficasINS = false;
	FGraficasMED = false;
	FGraficaCDEINS = false;
	FGraficaCDSINS = false;
	FGraficaCDEMED = false;
	FGraficaCDSMED = false;
	FSumCDE = 0.;
	FSumCDS = 0.;
	FSumTime = 0.;
	FTiempoAnt = 0.;
	FToCylinder = false;
	FEngine = NULL;
	FTime0 = 0.;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TTipoValvula::~TTipoValvula() {

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TTipoValvula::AsignaParametros(int Pipe, int Nodo, int Tipo, int Valvula, double dTubo, int sentido) {
	try {
		FTubo = Pipe;
		FNodo = Nodo;
		FTipo = Tipo;
		FValvula = Valvula;
		FDiamTubo = dTubo;
		FSentido = sentido;
	} catch(exception &N) {
		std::cout << "ERROR: AsignaParametros TypeOfValve" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TTipoValvula::LeeDiametro() {
	try {
		if(FDiamRef > 0) {
			return FDiamRef;
		} else {
			return FDiamTubo;
		}
	} catch(exception &N) {
		std::cout << "ERROR: LeeDiametro TypeOfValve" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//void TTipoValvula::PutDiametroTubo(double Valor)
//{
//FDiamTubo=Valor;
//}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TTipoValvula::AsignaCRecuperacion(double Valor) {
	FCRecuperacion = Valor;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TTipoValvula::AsignaCDVolTub(double Valor) {
	FCDVolTub = Valor;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TTipoValvula::LeeDatosGraficasINS(const char *FileWAM, fpos_t &filepos) {
	try {
		int ndv = 0, var = 0;
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);
		FGraficasINS = true;

		fscanf(fich, " %d", &ndv);
		for(int i = 0; i < ndv; i++) {
			fscanf(fich, " %d", &var);
			switch(var) {
			case 0:
				FGraficaCDEINS = true;
				break;
			case 1:
				FGraficaCDSINS = true;
				break;
			}
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: LeeDatosGraficas TypeOfValve" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TTipoValvula::CabeceraGraficaINS(stringstream& insoutput, int nodo) {
	try {
//FILE *fich=fopen(FileSALIDA,"a");
		std::string Label;

		if(FGraficasINS) {
			if(FGraficaCDEINS) {
				Label = "\t" + PutLabel(11) + std::to_string(nodo) + PutLabel(901);
				insoutput << Label.c_str();
			}
			if(FGraficaCDSINS) {
				Label = "\t" + PutLabel(12) + std::to_string(nodo) + PutLabel(901);
				insoutput << Label.c_str();
			}
		}
//fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: CabeceraGrafica TypeOfValve" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TTipoValvula::ImprimeGraficaINS(stringstream& insoutput) {
	try {
//FILE *fich=fopen(FileSALIDA,"a");
		if(FGraficasINS) {
			if(FGraficaCDEINS)
				insoutput << "\t" << FCDTubVol;
			if(FGraficaCDSINS)
				insoutput << "\t" << FCDVolTub;
		}
//fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: ImprimeGrafica TypeOfValve" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TTipoValvula::AcumulaCDMedio(double TiempoActual) {
	try {
		double DeltaT = 0.;
		DeltaT = TiempoActual - FTiempoAnt;
		FTiempoAnt = TiempoActual;

		if(FGraficasMED) {
			if(FGraficaCDEMED)
				FSumCDE += FCDTubVol * DeltaT;
			if(FGraficaCDSMED)
				FSumCDS += FCDVolTub * DeltaT;
			FSumTime += DeltaT;
		}
	} catch(exception &N) {
		std::cout << "ERROR: AcumulaCDMedio TypeOfValve" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TTipoValvula::LeeDatosGraficasMED(const char *FileWAM, fpos_t &filepos) {
	try {
		int ndv = 0, var = 0;
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		FGraficasMED = true;

		fscanf(fich, " %d", &ndv);
		for(int i = 0; i < ndv; i++) {
			fscanf(fich, " %d", &var);
			switch(var) {
			case 0:
				FGraficaCDEMED = true;
				break;
			case 1:
				FGraficaCDSMED = true;
				break;
			}
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: LeeDatosGraficas TypeOfValve" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TTipoValvula::CabeceraGraficaMED(stringstream& medoutput, int nodo) {
	try {
//FILE *fich=fopen(FileSALIDA,"a");
		std::string Label;

		if(FGraficasMED) {
			if(FGraficaCDEMED) {
				Label = "\t" + PutLabel(11) + std::to_string(nodo) + PutLabel(901);
				medoutput << Label.c_str();
			}
			if(FGraficaCDSMED) {
				Label = "\t" + PutLabel(12) + std::to_string(nodo) + PutLabel(901);
				medoutput << Label.c_str();
			}
		}
//fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: CabeceraGrafica TypeOfValve" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TTipoValvula::ImprimeGraficaMED(stringstream& medoutput) {
	try {
//FILE *fich=fopen(FileSALIDA,"a");
		if(FGraficasMED) {
			if(FGraficaCDEMED) {
				FCDEMedio = FSumCDE / FSumTime;
				medoutput << "\t" << FCDEMedio;
				FSumCDE = 0.;
			}
			if(FGraficaCDSMED) {
				FCDSMedio = FSumCDS / FSumTime;
				medoutput << "\t" << FCDSMedio;
				FSumCDS = 0.;
			}
			FSumTime = 0;
		}
//fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: ImprimeGrafica TypeOfValve" << std::endl;
		//std::cout << "Tipo de error: " << N.what().scr() << std::endl;
		throw Exception(N.what());

	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TTipoValvula::PutPipe(TTubo *Pipe, int node) {
	FPipe = Pipe;
	FPipeNode = node;
	FDiamTubo = FPipe->GetDiametro(FPipeNode);
	if(FDiamRef > 0) {
		FSectionRatio = pow2(FDiamRef / FDiamTubo);
	} else {
		FSectionRatio = 1;
	}
	FCDTubVol *= FSectionRatio;
	FCDVolTub *= FSectionRatio;
	//FDiamTubo=FPipe->D
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TTipoValvula::PutCylider(TCilindro *Cylinder) {
	FCylinder = Cylinder;
	FToCylinder = true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TTipoValvula::PutPlenum(TDeposito *Plenum) {
	FPlenum = Plenum;
	FToCylinder = false;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)
