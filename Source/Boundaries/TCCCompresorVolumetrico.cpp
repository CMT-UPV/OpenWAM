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

#include "TCCCompresorVolumetrico.h"
//#include <cmath>
#include <iostream>
#include "TTubo.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCCCompresorVolumetrico::TCCCompresorVolumetrico(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel,
		int numeroespecies, nmCalculoGamma GammaCalculation, bool ThereIsEGR) :
	TCondicionContorno(TipoCC, numCC, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {

	FTuboExtremo = NULL;
	asgNumeroCV = false;

	FResMediosCV.Potencia = false;
	FResMediosCV.Massflow = false;
	FResMediosCV.Pressure = false;

	FResInstantCV.Potencia = false;
	FResInstantCV.Massflow = false;
	FResInstantCV.Pressure = false;

	FComposicion = NULL;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCCCompresorVolumetrico::~TCCCompresorVolumetrico() {

	delete[] FTuboExtremo;
	if(FComposicion != NULL)
		delete[] FComposicion;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCompresorVolumetrico::LeeCCCompresorVol(const char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe,
		bool HayMotor) {
	try {
		int i = 0, ControlRegimen;
		double fracciontotal = 0.;

		FTuboExtremo = new stTuboExtremo[1];
		FTuboExtremo[0].Pipe = NULL;

		while(FNumeroTubosCC < 1 && i < NumberOfPipes) {
			if(Pipe[i]->getNodoIzq() == FNumeroCC) {
				FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
				FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmLeft;
				FNodoFin = 0;
				FIndiceCC = 0;
				FCC = &(FTuboExtremo[FNumeroTubosCC].Beta);
				FCD = &(FTuboExtremo[FNumeroTubosCC].Landa);
				FSeccionTubo = __geom::Circle_area(Pipe[i]->GetDiametro(FNodoFin));
				FNumeroTubosCC++;
			}
			if(Pipe[i]->getNodoDer() == FNumeroCC) {
				FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
				FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmRight;
				FNodoFin = Pipe[i]->getNin() - 1;
				FIndiceCC = 1;
				FCC = &(FTuboExtremo[FNumeroTubosCC].Landa);
				FCD = &(FTuboExtremo[FNumeroTubosCC].Beta);
				FSeccionTubo = __geom::Circle_area(Pipe[i]->GetDiametro(FNodoFin));
				FNumeroTubosCC++;
			}
			i++;
		}

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &ControlRegimen);

		switch(ControlRegimen) {
		case 0:
			FControlRegimen = nmPropio;
			break;
		case 1:
			FControlRegimen = nmMotor;
			break;
		}

		if(FControlRegimen == nmPropio) {
			fscanf(fich, "%lf ", &FRegimen);
			FRelacionVelocidadesCV = 1.;
		} else if(FControlRegimen == nmMotor && HayMotor) {
			fscanf(fich, "%lf ", &FRelacionVelocidadesCV);
		} else {
			std::cout <<
					  "ERROR: TCCCompresorVolumetrico::LeeCCDeposito Lectura del Control del Regimen erronea en la condicion de contorno: " <<
					  FNumeroCC << std::endl;
			throw Exception(" ");
		}

		fscanf(fich, "%lf %lf ", &FPresionCV, &FTemperaturaCV);
		fscanf(fich, "%lf %lf %lf ", &FC1Caudal, &FC2Caudal, &FC3Caudal);
		fscanf(fich, "%lf %lf %lf ", &FC1Temperatura, &FC2Temperatura, &FC3Temperatura);
		fscanf(fich, "%lf %lf %lf ", &FC1Potencia, &FC2Potencia, &FC3Potencia);
		fscanf(fich, "%lf %lf %lf ", &FC4Potencia, &FC5Potencia, &FC6Potencia);

// Inicializacion del transporte de especies quimicas.
		FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
		FComposicion = new double[FNumeroEspecies - FIntEGR];
		for(int i = 0; i < FNumeroEspecies - 1; i++) {
			fscanf(fich, "%lf ", &FComposicion[i]);
			FFraccionMasicaEspecie[i] = FTuboExtremo[0].Pipe->GetFraccionMasicaInicial(i);
			fracciontotal += FComposicion[i];
		}
		if(FHayEGR) {
			FFraccionMasicaEspecie[FNumeroEspecies - 1] = FTuboExtremo[0].Pipe->GetFraccionMasicaInicial(FNumeroEspecies - 1);
			if(FCalculoEspecies == nmCalculoCompleto) {
				if(FComposicion[0] > 0.2)
					FComposicion[FNumeroEspecies - 1] = 0.;
				else
					FComposicion[FNumeroEspecies - 1] = 1.;
			} else {
				if(FComposicion[0] > 0.5)
					FComposicion[FNumeroEspecies - 1] = 1.;
				else
					FComposicion[FNumeroEspecies - 1] = 0.;
			}
		}
		if(fracciontotal != 1.) {
			std::cout <<
					  "ERROR: La fraccion masica total no puede ser distinta de 1. Repasa la lectura en la condicion de contorno  " <<
					  FNumeroCC << std::endl;
			throw Exception(" ");
		}

		fgetpos(fich, &filepos);
		fclose(fich);

	}

	catch(exception &N) {
		std::cout << "ERROR: TCCCompresorVolumetrico::LeeCCCompresorVol en la condicion de contorno: " << FNumeroCC <<
				  std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCompresorVolumetrico::ObtencionValoresInstantaneos(double RegimenMotor) {
	try {

		FPressure = FTuboExtremo[FNumeroTubosCC - 1].Pipe->GetPresion(FNodoFin);
		if(FControlRegimen == nmMotor) {
			FRegimen = RegimenMotor;
		}

	} catch(exception &N) {
		std::cout << "ERROR: TCCCompresorVolumetrico::ObtencionValoresInstantaneos en la condicion de contorno: " << FNumeroCC
				  << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCompresorVolumetrico::CalculaCondicionContorno(double Time) {
	try {
		double ei, ed, massflow, entropia, FraccionMasicaAcum = 0.;
		int cd = 0;

		FGamma = FTuboExtremo[0].Pipe->GetGamma(FNodoFin);
		FRMezcla = FTuboExtremo[0].Pipe->GetRMezcla(FNodoFin);
		FGamma3 = __Gamma::G3(FGamma);
		FGamma4 = __Gamma::G4(FGamma);

		/* Calculo del massflow volumetrico (l/s) */
		massflow = FC1Caudal + FC2Caudal * FPressure + FC3Caudal * FRelacionVelocidadesCV * FRegimen;

		/* Calculo del massflow masico (kg/s) */
		FDensidad = __units::BarToPa(FPresionCV) / (FRMezcla * __units::degCToK(FTemperaturaCV));
		FGasto = massflow * FDensidad / 1000.;

		/* Temperature del gas entrante en grados centigrados */
		FTemperature = FC1Temperatura * pow2(FPressure) + FC2Temperatura * FPressure + FC3Temperatura;

		/* Velocity del sonido en el tubo */
		FSonido = sqrt(FGamma * __units::degCToK(FTemperature) * FRMezcla) / __cons::ARef;

		/* Potencia */
		FPotencia = FC1Potencia * pow3(FPressure) + FC2Potencia * pow2(FPressure) + FC3Potencia * FPressure + FC4Potencia +
					FC5Potencia * exp(FC6Potencia * FRelacionVelocidadesCV * FRegimen);

		/*!Acotacion del intervalo donde esta U*/
		ei = 0;
		ed = FSonido;

		stComprVol CV(FTuboExtremo[0].Entropia, *FCC, FGamma, FSonido, FGasto, FSeccionTubo, __cons::PRef, __cons::ARef);
		FVelocity = FindRoot(CV, ei, ed);

		/* printf("ERROR: TCCCompresorVolumetrico::CalculaCondicionContorno No hay convergencia en el compresor volumetrico en la condicion de contorno: %d.\n",FNumeroCC);
		 printf("Repasar los datos. El compresor esta en condiciones supersonicas.\n");
		 throw Exception("ERROR: TCCCompresorVolumetrico::CalculaCondicionContorno No hay convergencia en el compresor volumetrico.");*/

		/* Obtencion de las variables de Riemann */ // Tal y como esta planteada esta BC, el flujo siempre sera entrante al tubo.
		FTuboExtremo[0].Entropia = CV.entropia;
		*FCC = FSonido - FGamma3 * FVelocity;
		*FCD = FSonido + FGamma3 * FVelocity;

// Transporte de Especies Quimicas
		if(*FCC > *FCD) {     // Flujo saliente del tubo
			for(int j = 0; j < FNumeroEspecies - 2; j++) {
				FFraccionMasicaEspecie[j] = FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC, j);
				FraccionMasicaAcum += FFraccionMasicaEspecie[j];
			}
			FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
			FFraccionMasicaEspecie[FNumeroEspecies - 1] = FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC, FNumeroEspecies - 1);
		} else if(*FCD > *FCC) {   // Flujo entrante al tubo
			for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
				FFraccionMasicaEspecie[j] = FComposicion[j];
			}
		}
		/* La ultima opcion es que *FCC=*FCD. En este caso el flujo esta parado y la fraccion masica
		 de las especies permanece constante en dicho instante */

		AcumulaResultadosMediosCV(Time);

	} catch(exception &N) {
		std::cout << "ERROR: TCCCompresorVolumetrico::CalculaCondicionContorno en la condicion de contorno: " << FNumeroCC <<
				  std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//void TCCCompresorVolumetrico::PutNumeroCV(int valor)
//{
//try
//{
//if(!asgNumeroCV){
//     FNumeroCV=valor;
//     asgNumeroCV=true;
//}else{
//     std::cout << "ERROR: Este Compressor Volumetrico ya tiene numero asignado" << std::endl;
//     throw Exception("");
//}
//}
//catch(Exception &N)
//{
//std::cout << "ERROR: TCCCompresorVolumetrico::PutNumeroCV en la BC " << FNumeroCC << std::endl;
//std::cout << "Tipo de error: " << N.what() << std::endl;
//throw Exception(N.what());
//}
//}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCompresorVolumetrico::ReadAverageResultsCV(const char *FileWAM, fpos_t &filepos) {
	try {
		int nvars = 0, var = 0;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &nvars);
		for(int i = 0; i < nvars; i++) {
			fscanf(fich, "%d ", &var);
			switch(var) {
			case 0:
				FResMediosCV.Potencia = true;
				break;
			case 1:
				FResMediosCV.Massflow = true;
				break;
			case 2:
				FResMediosCV.Pressure = true;
				break;
			default:
				std::cout << "Resultados medios en CV(BC) " << FNumeroCC << " no implementados " << std::endl;
			}
		}

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TCCCompresorVolumetrico::ReadAverageResultsCV en la BC " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCompresorVolumetrico::CabeceraResultadosMedCV(stringstream& medoutput) {
	try {
//FILE *fich=fopen(FileSALIDA,"a");

		std::string Label;

		if(FResMediosCV.Potencia) {
			Label = "\t" + PutLabel(401) + std::to_string(FNumeroCC) + PutLabel(903);
			medoutput << Label.c_str();
		}
		if(FResMediosCV.Massflow) {
			Label = "\t" + PutLabel(402) + std::to_string(FNumeroCC) + PutLabel(904);
			medoutput << Label.c_str();
		}
		if(FResMediosCV.Pressure) {
			Label = "\t" + PutLabel(402) + std::to_string(FNumeroCC) + PutLabel(908);
			medoutput << Label.c_str();
		}

//fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TCCCompresorVolumetrico::CabeceraResultadosMedCV en la BC " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCompresorVolumetrico::ImprimeResultadosMedCV(stringstream& medoutput) {
	try {
//FILE *fich=fopen(FileSALIDA,"a");

		if(FResMediosCV.Potencia)
			medoutput << "\t" << FResMediosCV.PotenciaMED;
		if(FResMediosCV.Potencia)
			medoutput << "\t" << FResMediosCV.GastoMED;
		if(FResMediosCV.Potencia)
			medoutput << "\t" << FResMediosCV.PresionMED;

//fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TCCCompresorVolumetrico::ImprimerResultadosMedCV en la BC " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCompresorVolumetrico::IniciaMedias() {
	try {

		FResMediosCV.PotenciaSUM = 0.;
		FResMediosCV.GastoSUM = 0.;
		FResMediosCV.PresionSUM = 0.;
		FResMediosCV.TiempoSUM = 0.;
		FResMediosCV.Tiempo0 = 0.;

	} catch(exception &N) {
		std::cout << "ERROR: TCCCompresorVolumetrico::IniciaMedias en la BC: " << FNumeroCC << std::endl;
//std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCompresorVolumetrico::ResultadosMediosCV() {
	try {
		if(FResMediosCV.Potencia) {
			FResMediosCV.PotenciaMED = FResMediosCV.PotenciaSUM / FResMediosCV.TiempoSUM;
			FResMediosCV.PotenciaSUM = 0.;
		}
		if(FResMediosCV.Massflow) {
			FResMediosCV.GastoMED = FResMediosCV.GastoSUM / FResMediosCV.TiempoSUM;
			FResMediosCV.GastoSUM = 0.;
		}
		if(FResMediosCV.Pressure) {
			FResMediosCV.PresionMED = FResMediosCV.PresionSUM / FResMediosCV.TiempoSUM;
			FResMediosCV.PresionSUM = 0.;
		}
		FResMediosCV.TiempoSUM = 0;
	} catch(exception &N) {
		std::cout << "ERROR: TCCCompresorVolumetrico::ResultadosMediosCV en la BC: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCompresorVolumetrico::AcumulaResultadosMediosCV(double Actual) {
	try {

		double Delta = Actual - FResMediosCV.Tiempo0;

		if(FResMediosCV.Potencia) {
			FResMediosCV.PotenciaSUM += FPotencia * Delta;
		}
		if(FResMediosCV.Massflow) {
			FResMediosCV.GastoSUM += FGasto * Delta;
		}
		if(FResMediosCV.Pressure) {
			FResMediosCV.PresionSUM += FPressure * Delta;
		}

		FResMediosCV.TiempoSUM += Delta;
		FResMediosCV.Tiempo0 = Delta;
	} catch(exception &N) {
		std::cout << "ERROR: TCCCompresorVolumetrico::AcumulaResultadosMediosCV en la BC: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCompresorVolumetrico::LeeResultadosInstantCV(const char *FileWAM, fpos_t &filepos) {
	int nvars = 0, var = 0;

	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &nvars);
		for(int i = 0; i < nvars; i++) {
			fscanf(fich, "%d ", &var);
			switch(var) {
			case 0:
				FResInstantCV.Potencia = true;
				break;
			case 1:
				FResInstantCV.Massflow = true;
				break;
			case 2:
				FResInstantCV.Pressure = true;
				break;
			default:
				std::cout << "Resultados medios en CV(BC) " << FNumeroCC << " no implementados " << std::endl;
			}
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TCCCompresorVolumetrico::LeeResultadosInstantCV en la BC " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCompresorVolumetrico::CabeceraResultadosInstantCV(stringstream& insoutput) {
	try {
//FILE *fich=fopen(FileSALIDA,"a");
		std::string Label;

		if(FResInstantCV.Potencia) {
			Label = "\t" + PutLabel(401) + std::to_string(FNumeroCC) + PutLabel(903);
			insoutput << Label.c_str();
		}
		if(FResInstantCV.Massflow) {
			Label = "\t" + PutLabel(402) + std::to_string(FNumeroCC) + PutLabel(904);
			insoutput << Label.c_str();
		}
		if(FResInstantCV.Pressure) {
			Label = "\t" + PutLabel(403) + std::to_string(FNumeroCC) + PutLabel(908);
			insoutput << Label.c_str();
		}
//fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TCCCompresorVolumetrico::CabeceraResultadosInstantCV en la BC " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCompresorVolumetrico::ResultadosInstantCV() {
	try {
		if(FResInstantCV.Potencia)
			FResInstantCV.PotenciaINS = FPotencia;
		if(FResInstantCV.Massflow)
			FResInstantCV.GastoINS = FGasto;
		if(FResInstantCV.Pressure)
			FResInstantCV.PresionINS = FPressure;

	} catch(exception &N) {
		std::cout << "ERROR: TCCCompresorVolumetrico::ResultadosInstantCV en la BC " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCompresorVolumetrico::ImprimeResultadosInstantCV(stringstream& insoutput) {
	try {
//FILE *fich=fopen(FileSALIDA,"a");

		if(FResInstantCV.Potencia)
			insoutput << "\t" << FResInstantCV.PotenciaINS;
		if(FResInstantCV.Massflow)
			insoutput << "\t" << FResInstantCV.GastoINS;
		if(FResInstantCV.Pressure)
			insoutput << "\t" << FResInstantCV.PresionINS;

//fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TCCCompresorVolumetrico::ImprimeResultadosInstantCV en la BC " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)
