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

#include "TTable.h"

//---------------------------------------------------------------------------

TTable::TTable(int i) :
	TController(nmCtlPID, i) {

	/*fError_ant=0;
	 fTime_ant=0;
	 fInicio=true; */
}
TTable::~TTable() {

}

double TTable::Output(double Time) {

}

void TTable::LeeController(const char *FileWAM, fpos_t &filepos) {

	int xnum = 0, ynum = 0;
	double x = 0., y = 0., z = 0.;
	std::vector<double> zz;

	FILE *fich = fopen(FileWAM, "r");
	fsetpos(fich, &filepos);

	fscanf(fich, "%d ", &fDimensiones);
	fscanf(fich, "%d ", &xnum);
	for(int i = 0; i < xnum; i++) {
		fscanf(fich, "%d ", &x);
		fX_map.push_back(x);
	}
	if(fDimensiones == 1) {
		for(int i = 0; i < xnum; i++) {
			fscanf(fich, "%d ", &y);
			fY_map.push_back(y);
		}
	} else if(fDimensiones == 2) {

	}

	fgetpos(fich, &filepos);
	fclose(fich);

}

void TTable::AsignaObjetos(TSensor **Sensor, TController **Controller) {
//std::cout << "entro";

	FSensor.push_back(Sensor[FSensorID[0] - 1]);

//fSetPointController=Controller[fSetPointControllerID-1];

}

void TTable::LeeResultadosMedControlador(const char *FileWAM, fpos_t &filepos) {
	try {
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
			case 1:
				FResMediosCtrl.Error = true;
				break;
			case 2:
				FResMediosCtrl.POutput = true;
				break;
			case 3:
				FResMediosCtrl.IOutput = true;
				break;
			case 4:
				FResMediosCtrl.DOutput = true;
				break;
			case 5:
				FResMediosCtrl.Output_filt = true;
				break;
			default:
				std::cout << "Resultados medios en Controlador " << fID << " no implementados " << std::endl;
			}
		}

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TTable::LeeResultadosControlador en el controlador " << fID << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TTable::LeeResultadosInsControlador(const char *FileWAM, fpos_t &filepos) {
	try {
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
			case 1:
				FResInstantCtrl.Error = true;
				break;
			case 2:
				FResInstantCtrl.POutput = true;
				break;
			case 3:
				FResInstantCtrl.IOutput = true;
				break;
			case 4:
				FResInstantCtrl.DOutput = true;
				break;
			case 5:
				FResInstantCtrl.Output_filt = true;
				break;
			default:
				std::cout << "Resultados instantaneos en Controlador " << fID << " no implementados " << std::endl;
			}
		}

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TTable::LeeResultadosInsControlador en el controlador " << fID << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TTable::CabeceraResultadosMedControlador(stringstream *medoutput) {
	try {
		std::string Label;

		if(FResMediosCtrl.Output) {
			Label = "\t" + PutLabel(705) + std::to_string(fID) + PutLabel(901);
			*medoutput << Label.c_str();
		}
		if(FResMediosCtrl.Error) {
			Label = "\t" + PutLabel(706) + std::to_string(fID) + PutLabel(901);
			*medoutput << Label.c_str();
		}
		if(FResMediosCtrl.POutput) {
			Label = "\t" + PutLabel(709) + std::to_string(fID) + PutLabel(901);
			*medoutput << Label.c_str();
		}
		if(FResMediosCtrl.IOutput) {
			Label = "\t" + PutLabel(710) + std::to_string(fID) + PutLabel(901);
			*medoutput << Label.c_str();
		}
		if(FResMediosCtrl.DOutput) {
			Label = "\t" + PutLabel(711) + std::to_string(fID) + PutLabel(901);
			*medoutput << Label.c_str();
		}
		if(FResMediosCtrl.Output_filt) {
			Label = "\t" + PutLabel(712) + std::to_string(fID) + PutLabel(901);
			*medoutput << Label.c_str();
		}

	} catch(exception &N) {
		std::cout << "ERROR: TTable::CabeceraResultadosMedControlador en el controlador " << fID << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TTable::CabeceraResultadosInsControlador(stringstream *insoutput) {
	try {
		std::string Label;

		if(FResInstantCtrl.Output) {
			Label = "\t" + PutLabel(705) + std::to_string(fID) + PutLabel(901);
			*insoutput << Label.c_str();
		}
		if(FResInstantCtrl.Error) {
			Label = "\t" + PutLabel(706) + std::to_string(fID) + PutLabel(901);
			*insoutput << Label.c_str();
		}
		if(FResInstantCtrl.POutput) {
			Label = "\t" + PutLabel(709) + std::to_string(fID) + PutLabel(901);
			*insoutput << Label.c_str();
		}
		if(FResInstantCtrl.IOutput) {
			Label = "\t" + PutLabel(710) + std::to_string(fID) + PutLabel(901);
			*insoutput << Label.c_str();
		}
		if(FResInstantCtrl.DOutput) {
			Label = "\t" + PutLabel(711) + std::to_string(fID) + PutLabel(901);
			*insoutput << Label.c_str();
		}
		if(FResInstantCtrl.Output_filt) {
			Label = "\t" + PutLabel(712) + std::to_string(fID) + PutLabel(901);
			*insoutput << Label.c_str();
		}
	} catch(exception &N) {
		std::cout << "ERROR: TTable::CabeceraResultadosInsControlador en el controlador " << fID << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TTable::ImprimeResultadosMedControlador(stringstream *medoutput) {
	try {
		std::string Label;

		if(FResMediosCtrl.Output) {
			*medoutput << "\t" << FResMediosCtrl.OutputMED;
		}
		if(FResMediosCtrl.Error) {
			*medoutput << "\t" << FResMediosCtrl.ErrorMED;
		}
		if(FResMediosCtrl.POutput) {
			*medoutput << "\t" << FResMediosCtrl.POutputMED;
		}
		if(FResMediosCtrl.IOutput) {
			*medoutput << "\t" << FResMediosCtrl.IOutputMED;
		}
		if(FResMediosCtrl.DOutput) {
			*medoutput << "\t" << FResMediosCtrl.DOutputMED;
		}
		if(FResMediosCtrl.Output_filt) {
			*medoutput << "\t" << FResMediosCtrl.Output_filtMED;
		}
	} catch(exception &N) {
		std::cout << "ERROR: TTable::ImprimeResultadosMedControlador en el controlador " << fID << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TTable::ImprimeResultadosInsControlador(stringstream *insoutput) {
	try {
		std::string Label;

		if(FResInstantCtrl.Output) {
			*insoutput << "\t" << FResInstantCtrl.OutputINS;
		}
		if(FResInstantCtrl.Error) {
			*insoutput << "\t" << FResInstantCtrl.ErrorINS;
		}
		if(FResInstantCtrl.POutput) {
			*insoutput << "\t" << FResInstantCtrl.POutputINS;
		}
		if(FResInstantCtrl.IOutput) {
			*insoutput << "\t" << FResInstantCtrl.IOutputINS;
		}
		if(FResInstantCtrl.DOutput) {
			*insoutput << "\t" << FResInstantCtrl.DOutputINS;
		}
		if(FResInstantCtrl.Output_filt) {
			*insoutput << "\t" << FResInstantCtrl.Output_filtINS;
		}
	} catch(exception &N) {
		std::cout << "ERROR: TTable::CabeceraResultadosInsControlador en el controlador " << fID << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TTable::IniciaMedias() {
	try {

		FResMediosCtrl.OutputSUM = 0.;
		FResMediosCtrl.ErrorSUM = 0.;
		FResMediosCtrl.POutputSUM = 0.;
		FResMediosCtrl.IOutputSUM = 0.;
		FResMediosCtrl.DOutputSUM = 0.;
		FResMediosCtrl.Output_filtSUM = 0.;
		FResMediosCtrl.TiempoSUM = 0.;
		FResMediosCtrl.Tiempo0 = 0.;

	} catch(exception &N) {
		std::cout << "ERROR: TTable::IniciaMedias en el controlador: " << fID << std::endl;
//std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TTable::ResultadosMediosController() {
	try {

		if(FResMediosCtrl.Output) {
			FResMediosCtrl.OutputMED = FResMediosCtrl.OutputSUM / FResMediosCtrl.TiempoSUM;
			FResMediosCtrl.OutputSUM = 0.;
		}
		if(FResMediosCtrl.Error) {
			FResMediosCtrl.ErrorMED = FResMediosCtrl.ErrorSUM / FResMediosCtrl.TiempoSUM;
			FResMediosCtrl.ErrorSUM = 0.;
		}
		if(FResMediosCtrl.POutput) {
			FResMediosCtrl.POutputMED = FResMediosCtrl.POutputSUM / FResMediosCtrl.TiempoSUM;
			FResMediosCtrl.POutputSUM = 0.;
		}
		if(FResMediosCtrl.IOutput) {
			FResMediosCtrl.IOutputMED = FResMediosCtrl.IOutputSUM / FResMediosCtrl.TiempoSUM;
			FResMediosCtrl.IOutputSUM = 0.;
		}
		if(FResMediosCtrl.DOutput) {
			FResMediosCtrl.DOutputMED = FResMediosCtrl.DOutputSUM / FResMediosCtrl.TiempoSUM;
			FResMediosCtrl.DOutputSUM = 0.;
		}
		if(FResMediosCtrl.Output_filt) {
			FResMediosCtrl.Output_filtMED = FResMediosCtrl.Output_filtSUM / FResMediosCtrl.TiempoSUM;
			FResMediosCtrl.Output_filtSUM = 0.;
		}
		FResMediosCtrl.TiempoSUM = 0;

	} catch(exception &N) {
		std::cout << "ERROR: TTable::ResultadosMediosController en el eje: " << fID << std::endl;
//std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TTable::AcumulaResultadosMediosController(double Actual) {
	try {
		/* Lo que se hace en esta funcion se realiza dentro del calculo del eje, para asi poder
		 llevar a cabo la salida de resultados medios por pantalla. */
		double Delta = Actual - FResMediosCtrl.Tiempo0;

		if(FResMediosCtrl.Output) {
			FResMediosCtrl.OutputSUM += fOutput * Delta;
		}
		if(FResMediosCtrl.Error) {
			FResMediosCtrl.ErrorSUM += fError * Delta;
		}
		if(FResMediosCtrl.POutput) {
			FResMediosCtrl.POutputSUM += fpact * Delta;
		}
		if(FResMediosCtrl.IOutput) {
			FResMediosCtrl.IOutputSUM += fiact * Delta;
		}
		if(FResMediosCtrl.DOutput) {
			FResMediosCtrl.DOutputSUM += fdact * Delta;
		}
		if(FResMediosCtrl.Output_filt) {
			FResMediosCtrl.Output_filtSUM += fOutput_filt * Delta;
		}
		FResMediosCtrl.TiempoSUM += Delta;
		FResMediosCtrl.Tiempo0 = Actual;

	} catch(exception &N) {
		std::cout << "ERROR: TTable::AcumulaResultadosMediosController en el eje: " << fID << std::endl;
//std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TTable::ResultadosInstantController() {
	try {
		if(FResInstantCtrl.Output)
			FResInstantCtrl.OutputINS = fOutput;
		if(FResInstantCtrl.Error)
			FResInstantCtrl.ErrorINS = fError;
		if(FResInstantCtrl.POutput)
			FResInstantCtrl.POutputINS = fpact;
		if(FResInstantCtrl.IOutput)
			FResInstantCtrl.IOutputINS = fiact;
		if(FResInstantCtrl.DOutput)
			FResInstantCtrl.DOutputINS = fdact;
		if(FResInstantCtrl.Output_filt)
			FResInstantCtrl.Output_filtINS = fOutput_filt;

	} catch(exception &N) {
		std::cout << "ERROR: TTable::ResultadosInstantController en el eje " << fID << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

#pragma package(smart_init)
