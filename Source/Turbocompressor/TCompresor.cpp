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
#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
//#include <cmath>

#include "TCompresor.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCompresor::TCompresor(int i, nmTipoCalculoEspecies SpeciesModel, int numeroespecies, nmCalculoGamma GammaCalculation,
					   bool ThereIsEGR) {
	FNumeroCompresor = i + 1;
	FMedias.GraficaMedias = false;
	FMedias.GraficaTrabajo = false;
	FMedias.GraficaRendimiento = false;
	FMedias.GraficaRelacionCompresion = false;
	FMedias.GraficaGasto = false;
	FMedias.GraficaGastoCorregido = false;
	FMedias.GraficaRegimenCorregido = false;
	FInstant.GraficaInstantaneas = false;
	FInstant.GraficaPotencia = false;
	FInstant.GraficaRendimiento = false;
	FInstant.GraficaRelacionCompresion = false;
	FInstant.GraficaGasto = false;
	FInstant.GraficaGastoCorregido = false;
	FInstant.GraficaRegimenCorregido = false;
	FInstant.GraficaGamma = false;
	FTiempo0 = 0.;
	Mapa = NULL;
	Mapa2T = NULL;
	FBombeo = true;
	FTrabajoPaso = 0.;
	FDeltaTPaso = 0.;

	FCalculoEspecies = SpeciesModel;
	FNumeroEspecies = numeroespecies;
	FCalculoGamma = GammaCalculation;
	FFraccionMasicaEspecie = NULL;

	FHayEGR = ThereIsEGR;
	if(FHayEGR)
		FIntEGR = 0;
	else
		FIntEGR = 1;

	FIsAcoustic = false;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCompresor::~TCompresor() {
	if(Mapa != NULL)
		delete Mapa;
	if(Mapa2T != NULL)
		delete Mapa2T;
	if(FFraccionMasicaEspecie != NULL)
		delete[] FFraccionMasicaEspecie;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCompresor::InterpolaValoresMapa(double rtc) {
	try {
		FRegimen = rtc;

		if(FModeloCompresor == nmCompPipes) {
			Mapa2T->InterpolaMapa(FRegimen, FTemperatura10);
		} else {
			Mapa->InterpolaMapa(FRegimen, FTemperatura10);
		}

	} catch(exception &N) {
		std::cout << "ERROR: InterpolaValoresMapa en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: InterpolaValoresMapa en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCompresor::AcumulaMedias(double Tiempo) {
	double DeltaT = 0.;
	try {
		DeltaT = Tiempo - FMedias.Tiempo0;
		FMedias.TiempoSUM += DeltaT;
		FMedias.RelacionCompresionSUM += FRelacionCompresion * DeltaT;
		FMedias.GastoSUM += FGastoCompresor * DeltaT;
		FMedias.RendimientoSUM += FRendimiento * DeltaT;
		FMedias.GastoCorregidoSUM += FGastoCorregido * DeltaT;
		FMedias.RegimenCorregidoSUM += FRegimenCorregido * DeltaT;
		FMedias.TrabajoSUM += FPotencia * DeltaT;
		FMedias.Tiempo0 = Tiempo;
	} catch(exception &N) {
		std::cout << "ERROR: AcumulaMedias en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: AcumulaMedias en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCompresor::IniciaMedias() {

	try {
		FMedias.TiempoSUM = 0.;
		FMedias.RelacionCompresionSUM = 0.;
		FMedias.GastoSUM = 0.;
		FMedias.RendimientoSUM = 0.;
		FMedias.GastoCorregidoSUM = 0.;
		FMedias.RegimenCorregidoSUM = 0.;
		FMedias.TrabajoSUM = 0.;
		FMedias.Tiempo0 = 0.;

	} catch(exception &N) {
		std::cout << "ERROR: IniciaMedias en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: IniciaMedias en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCompresor::CalculoPotenciaPaso() {
	try {

		FPotenciaPaso = FTrabajoPaso / FDeltaTPaso;
		FTrabajoPaso = 0.;
		FDeltaTPaso = 0.;

	} catch(exception &N) {
		std::cout << "ERROR: TCompresor::CalculoPotenciaPaso en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: TCompresor::CalculoPotenciaPaso en el compresor: " + std::to_string(
							FNumeroCompresor) + N.what());
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCompresor::CalculaMedias() {

	try {
		FMedias.RelacionCompresion = FMedias.RelacionCompresionSUM / FMedias.TiempoSUM;
		FMedias.Massflow = FMedias.GastoSUM / FMedias.TiempoSUM;
		FMedias.Rendimiento = FMedias.RendimientoSUM / FMedias.TiempoSUM;
		FMedias.Trabajo = FMedias.TrabajoSUM;
		FMedias.GastoCorregido = FMedias.GastoCorregidoSUM / FMedias.TiempoSUM;
		FMedias.RegimenCorregido = FMedias.RegimenCorregidoSUM / FMedias.TiempoSUM;

		FMedias.TiempoSUM = 0.;
		FMedias.RelacionCompresionSUM = 0.;
		FMedias.GastoSUM = 0.;
		FMedias.RendimientoSUM = 0.;
		FMedias.TrabajoSUM = 0.;
		FMedias.GastoCorregidoSUM = 0.;
		FMedias.RegimenCorregidoSUM = 0.;

	} catch(exception &N) {
		std::cout << "ERROR: CalculaMedias en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: CalculaMedias en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCompresor::CalculaInstantaneos() {
	try {
		FInstant.RelacionCompresion = FRelacionCompresion;
		FInstant.Massflow = FGastoCompresor;
		FInstant.Rendimiento = FRendimiento;
		FInstant.Potencia = FPotencia;
		FInstant.GastoCorregido = FGastoCorregido;
		FInstant.RegimenCorregido = FRegimenCorregido;
		FInstant.Gamma = FGamma;
	} catch(exception &N) {
		std::cout << "ERROR: CalculaInstantaneos en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: CalculaInstantaneos en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCompresor::LeeDatosGraficasMedias(const char *FileWAM, fpos_t &filepos) {
	int NMagnitudes = 0, Magnitud = 0;
	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		FMedias.GraficaMedias = true;
		fscanf(fich, "%d", &NMagnitudes);
		for(int i = 0; i < NMagnitudes; i++) {
			fscanf(fich, "%d", &Magnitud);
			switch(Magnitud) {
			case 0:
				FMedias.GraficaTrabajo = true;
				break;
			case 1:
				FMedias.GraficaRendimiento = true;
				break;
			case 2:
				FMedias.GraficaRelacionCompresion = true;
				break;
			case 3:
				FMedias.GraficaGasto = true;
				break;
			case 4:
				FMedias.GraficaGastoCorregido = true;
				break;
			case 5:
				FMedias.GraficaRegimenCorregido = true;
				break;
			}
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: LeeDatosGraficasMedias en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: LeeDatosGraficasMedias en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCompresor::CabeceraGraficasMedias(stringstream& medoutput) {
	try {
//FILE *fich=fopen(FileSALIDA,"a");
		std::string Label;
		if(FMedias.GraficaMedias) {
			if(FMedias.GraficaTrabajo) {
				Label = "\t" + PutLabel(28) + std::to_string(FNumeroCompresor) + PutLabel(907);
				medoutput << Label.c_str();
			}
			if(FMedias.GraficaRendimiento) {
				Label = "\t" + PutLabel(22) + std::to_string(FNumeroCompresor) + PutLabel(901);
				medoutput << Label.c_str();
			}
			if(FMedias.GraficaRelacionCompresion) {
				Label = "\t" + PutLabel(23) + std::to_string(FNumeroCompresor) + PutLabel(901);
				medoutput << Label.c_str();
			}
			if(FMedias.GraficaGasto) {
				Label = "\t" + PutLabel(24) + std::to_string(FNumeroCompresor) + PutLabel(904);
				medoutput << Label.c_str();
			}
			if(FMedias.GraficaGastoCorregido) {
				Label = "\t" + PutLabel(25) + std::to_string(FNumeroCompresor) + PutLabel(905);
				medoutput << Label.c_str();
			}
			if(FMedias.GraficaRegimenCorregido) {
				Label = "\t" + PutLabel(26) + std::to_string(FNumeroCompresor) + PutLabel(906);
				medoutput << Label.c_str();
			}
		}
//fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: CabeceraGraficasMedias en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: CabeceraGraficasMedias en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCompresor::ImprimeGraficasMedias(stringstream& medoutput) {
	try {
//FILE *fich=fopen(FileSALIDA,"a");
		if(FMedias.GraficaMedias) {
			if(FMedias.GraficaTrabajo)
				medoutput << "\t" << FMedias.Trabajo;
			if(FMedias.GraficaRendimiento)
				medoutput << "\t" << FMedias.Rendimiento;
			if(FMedias.GraficaRelacionCompresion)
				medoutput << "\t" << FMedias.RelacionCompresion;
			if(FMedias.GraficaGasto)
				medoutput << "\t" << FMedias.Massflow;
			if(FMedias.GraficaGastoCorregido)
				medoutput << "\t" << FMedias.GastoCorregido;
			if(FMedias.GraficaRegimenCorregido)
				medoutput << "\t" << FMedias.RegimenCorregido;
		}
//fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: ImprimeGraficasMedias en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: ImprimeGraficasMedias en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCompresor::LeeDatosGraficasInstantaneas(const char *FileWAM, fpos_t &filepos) {
	int NMagnitudes = 0, Magnitud = 0;
	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		FInstant.GraficaInstantaneas = true;
		fscanf(fich, "%d", &NMagnitudes);
		for(int i = 0; i < NMagnitudes; i++) {
			fscanf(fich, "%d", &Magnitud);
			switch(Magnitud) {
			case 0:
				FInstant.GraficaRelacionCompresion = true;
				break;
			case 1:
				FInstant.GraficaRendimiento = true;
				break;
			case 2:
				FInstant.GraficaPotencia = true;
				break;
			case 3:
				FInstant.GraficaGasto = true;
				break;
			case 4:
				FInstant.GraficaGastoCorregido = true;
				break;
			case 5:
				FInstant.GraficaRegimenCorregido = true;
				break;
			case 6:
				FInstant.GraficaGamma = true;
				break;
			}
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: LeeDatosGraficasMedias en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: LeeDatosGraficasMedias en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCompresor::CabeceraGraficasInstantaneas(stringstream& insoutput) {
	try {
//FILE *fich=fopen(FileSALIDA,"a");
		std::string Label;

		if(FInstant.GraficaInstantaneas) {
			if(FInstant.GraficaPotencia) {
				Label = "\t" + PutLabel(21) + std::to_string(FNumeroCompresor) + PutLabel(903);
				insoutput << Label.c_str();
			}
			if(FInstant.GraficaRendimiento) {
				Label = "\t" + PutLabel(22) + std::to_string(FNumeroCompresor) + PutLabel(901);
				insoutput << Label.c_str();
			}
			if(FInstant.GraficaRelacionCompresion) {
				Label = "\t" + PutLabel(23) + std::to_string(FNumeroCompresor) + PutLabel(901);
				insoutput << Label.c_str();
			}
			if(FInstant.GraficaGasto) {
				Label = "\t" + PutLabel(24) + std::to_string(FNumeroCompresor) + PutLabel(904);
				insoutput << Label.c_str();
			}
			if(FInstant.GraficaGastoCorregido) {
				Label = "\t" + PutLabel(25) + std::to_string(FNumeroCompresor) + PutLabel(905);
				insoutput << Label.c_str();
			}
			if(FInstant.GraficaRegimenCorregido) {
				Label = "\t" + PutLabel(26) + std::to_string(FNumeroCompresor) + PutLabel(906);
				insoutput << Label.c_str();
			}
			if(FInstant.GraficaGamma) {
				Label = "\t" + PutLabel(27) + std::to_string(FNumeroCompresor) + PutLabel(901);
				insoutput << Label.c_str();
			}
		}
//fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: CabeceraGraficasInstantaneas en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: CabeceraGraficasInstantaneas en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCompresor::ImprimeGraficasInstantaneas(stringstream& insoutput) {
	try {
//FILE *fich=fopen(FileSALIDA,"a");
		if(FInstant.GraficaInstantaneas) {
			if(FInstant.GraficaPotencia)
				insoutput << "\t" << FInstant.Potencia;
			if(FInstant.GraficaRendimiento)
				insoutput << "\t" << FInstant.Rendimiento;
			if(FInstant.GraficaRelacionCompresion)
				insoutput << "\t" << FInstant.RelacionCompresion;
			if(FInstant.GraficaGasto)
				insoutput << "\t" << FInstant.Massflow;
			if(FInstant.GraficaGastoCorregido)
				insoutput << "\t" << FInstant.GastoCorregido;
			if(FInstant.GraficaRegimenCorregido)
				insoutput << "\t" << FInstant.RegimenCorregido;
			if(FInstant.GraficaGamma)
				insoutput << "\t" << FInstant.Gamma;
		}
//fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: ImprimeGraficasInstantaneas en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: ImprimeGraficasInstantaneas en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TCompresor::GetASonidoComp() {
	try {
		if(FModeloCompresor == nmCompPipes) {
			printf("ERROR:El compresor de tubos no puede estar unido a un deposito\n");
			return 0;
		} else
			return FASonidoSalida;
	} catch(exception &N) {
		std::cout << "ERROR: TCompresor::GetASonidoComp en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: ImprimeGraficasInstantaneas en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TCompresor::GetFraccionMasicaEspecie(int i) {
	try {
		return FFraccionMasicaEspecie[i];
	} catch(exception &N) {
		std::cout << "ERROR: TCompresor::GetFraccionMasicaEspecie en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCompresor::AsignAcousticElements(TTubo **Pipe, TDeposito **Volume) {
	if(FIsAcoustic) {
		FAcComp->AsignInPipe(Pipe);
		FAcComp->AsignOutPipe(Pipe);
		FAcComp->AsignVolute(Pipe);
		FAcComp->AsignRotorVol(Volume);
		FAcComp->AsignStatorVol(Volume);
	}
}

#pragma package(smart_init)

