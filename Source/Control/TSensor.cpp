/* --------------------------------------------------------------------------------*\
|==========================|
 |\\   /\ /\   // O pen     | OpenWAM: The Open Source 1D Gas-Dynamic Code
 | \\ |  X  | //  W ave     |
 |  \\ \/_\/ //   A ction   | CMT-Motores Termicos / Universidad Politecnica Valencia
 |   \\/   \//    M odel    |
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

#include "TSensor.h"
#include "TTubo.h"
#include "TDeposito.h"
#include "TBloqueMotor.h"

// ---------------------------------------------------------------------------

TSensor::TSensor(int i) {
	FNumeroSensor = i;
	FInicia = false;
	FPeriod = 0.;
	FTime0 = 0.;
}

TSensor::~TSensor() {

}

void TSensor::ReadSensor(const char *FileWAM, fpos_t &filepos) {
	int obj = 0, prm = 0;

	FILE *fich = fopen(FileWAM, "r");
	fsetpos(fich, &filepos);

	fscanf(fich, "%d %d", &obj, &prm);
	if(obj != 0)
		fscanf(fich, "%d ", &FObjectID);
	switch(obj) {
	case 0:
		FObjectSensed = nmSensEjec;
		switch(prm) {
		case 0:
			FParameterSensed = nmTime;
			break;
		default:
			std::cout << "ERROR: Parametro " << prm << "en el objeto " << obj << "no valido, sensor: " << FNumeroSensor <<
					  std::endl;
		}
		break;
	case 1:
		FObjectSensed = nmSensTubo;
		fscanf(fich, "%lf ", &FDistancia);
		switch(prm) {
		case 1:
			FParameterSensed = nmPressure;
			break;
		case 2:
			FParameterSensed = nmTemperature;
			break;
		case 3:
			FParameterSensed = nmMassFlow;
			break;
		default:
			std::cout << "ERROR: Parametro " << prm << "en el objeto " << obj << "no valido, sensor: " << FNumeroSensor <<
					  std::endl;
		}
		break;
	case 2:
		FObjectSensed = nmSensDeposito;
		switch(prm) {
		case 1:
			FParameterSensed = nmPressure;
			break;
		case 2:
			FParameterSensed = nmTemperature;
			break;
		default:
			std::cout << "ERROR: Parametro " << prm << "en el objeto " << obj << "no valido, sensor: " << FNumeroSensor <<
					  std::endl;
		}
		break;
	case 3:
		FObjectSensed = nmSensMotor;
		switch(prm) {
		case 4:
			FParameterSensed = nmFuel;
			break;
		case 5:
			FParameterSensed = nmEngSpeed;
			break;
		default:
			std::cout << "ERROR: Parametro " << prm << "en el objeto " << obj << "no valido, sensor: " << FNumeroSensor <<
					  std::endl;
		}
		break;
	default:
		std::cout << "ERROR: Objeto " << obj << "no valido, sensor: " << FNumeroSensor << std::endl;
	}
	fscanf(fich, "%lf %lf ", &FDelay, &FGain);

	fgetpos(fich, &filepos);
	fclose(fich);
}

void TSensor::AsignaObjeto(TObject *Object) {
	int nin = 0;
	double tmp = 0.;

	FObjectPointer = Object;
	if(FObjectSensed == nmSensTubo) {
		tmp = (double)(((TTubo*) FObjectPointer)->getNin() - 1) * FDistancia / ((TTubo*) FObjectPointer)->getLongitudTotal();
		FNode0 = floor(tmp);
		FNode1 = ceil(tmp);
		FDelta = (FDistancia - (double) FNode0 * ((TTubo*) FObjectPointer)->getXRef()) / ((TTubo*) FObjectPointer)->getXRef();
	}
}

double TSensor::Output() {
	return FCurrentValue;
}

void TSensor::ActualizaMedida(double Time) {
	double tmp0 = 0., tmp1 = 0.;
	bool Update = false;
	double deltaT = 0.;

	double TimeStep = Time - FTime0;

	FPeriod = FPeriod + TimeStep;

	if(FPeriod >= 0.001) {
		Update = true;
		deltaT = FPeriod;
		FPeriod = 0;
	}

	if(Update || !FInicia) {
		switch(FObjectSensed) {
		case nmSensEjec:
			switch(FParameterSensed) {
			case nmTime:
				FRealValue = Time;
				break;
			}
			FCurrentValue = FRealValue;
			break;
		case nmSensTubo:
			switch(FParameterSensed) {
			case nmPressure:
				FRealValue = ((TTubo*) FObjectPointer)->GetPresion(FNode0) * (1 - FDelta) + ((TTubo*) FObjectPointer)->GetPresion(
								 FNode1) * FDelta;
				break;
			case nmTemperature:
				tmp0 = pow2(((TTubo*) FObjectPointer)->GetAsonido(FNode0) * __cons::ARef) / ((TTubo*) FObjectPointer)->GetGamma(
						   FNode0) / ((TTubo*) FObjectPointer)->GetRMezcla(FNode0);
				tmp1 = pow2(((TTubo*) FObjectPointer)->GetAsonido(FNode1) * __cons::ARef) / ((TTubo*) FObjectPointer)->GetGamma(
						   FNode1) / ((TTubo*) FObjectPointer)->GetRMezcla(FNode1);
				FRealValue = tmp0 * (1 - FDelta) + tmp1 * FDelta;
				break;
			case nmMassFlow:
				tmp0 = ((TTubo*) FObjectPointer)->GetDensidad(FNode0) * ((TTubo*) FObjectPointer)->GetVelocidad(
						   FNode0) * __cons::ARef * ((TTubo*) FObjectPointer)->GetArea(FNode0);
				tmp1 = ((TTubo*) FObjectPointer)->GetDensidad(FNode1) * ((TTubo*) FObjectPointer)->GetVelocidad(
						   FNode1) * __cons::ARef * ((TTubo*) FObjectPointer)->GetArea(FNode1);
				FRealValue = tmp0 * (1 - FDelta) + tmp1 * FDelta;
				break;
			}
			break;
		case nmSensDeposito:
			switch(FParameterSensed) {
			case nmPressure:
				FRealValue = ((TDeposito*) FObjectPointer)->getPressure();
				break;
			case nmTemperature:
				FRealValue = ((TDeposito*) FObjectPointer)->getTemperature();
				break;
			}
			break;
		case nmSensMotor:
			switch(FParameterSensed) {
			case nmFuel:
				FRealValue = ((TBloqueMotor*) FObjectPointer)->getMasaFuel();
				break;
			case nmEngSpeed:
				FRealValue = ((TBloqueMotor*) FObjectPointer)->getRegimen();
				break;
			}
		}
	}

	if(!FInicia) {
		FPrevValue = FRealValue;
		FInicia = true;
	}
	if(!Update) {
		FCurrentValue = FPrevValue;
	} else {
		FCurrentValue = ((2 * FDelay - deltaT) * FPrevValue + deltaT * FGain * (FRealValue + FPrevRealValue)) /
						(2 * FDelay + deltaT);
	}
	FPrevValue = FCurrentValue;
	FPrevRealValue = FRealValue;
	FTime0 = Time;

	AcumulaResultadosMediosSensor(Time);

	// out=FCurrentValue;
	// return out;
}

void TSensor::LeeResultadosMedSensor(const char *FileWAM, fpos_t &filepos) {
	try {
		int nvars = 0, var = 0;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &nvars);
		for(int i = 0; i < nvars; i++) {
			fscanf(fich, "%d ", &var);
			switch(var) {
			case 0:
				FResMediosSensor.Output = true;
				break;
			case 1:
				FResMediosSensor.Input = true;
				break;
			default:
				std::cout << "Resultados medios en Controlador " << FNumeroSensor << " no implementados " << std::endl;
			}
		}

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TPIDController::LeeResultadosMedSensor en el controlador " << FNumeroSensor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TSensor::LeeResultadosInsSensor(const char *FileWAM, fpos_t &filepos) {
	try {
		int nvars = 0, var = 0;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &nvars);
		for(int i = 0; i < nvars; i++) {
			fscanf(fich, "%d ", &var);
			switch(var) {
			case 0:
				FResInstantSensor.Output = true;
				break;
			case 1:
				FResInstantSensor.Input = true;
				break;
			default:
				std::cout << "Resultados instantaneos en Sensor " << FNumeroSensor << " no implementados " << std::endl;
			}
		}

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TPIDController::LeeResultadosInsSensor en el Sensor " << FNumeroSensor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TSensor::CabeceraResultadosMedSensor(stringstream& medoutput) {
	try {
		std::string Label;

		if(FResMediosSensor.Output) {
			Label = "\t" + PutLabel(707) + std::to_string(FNumeroSensor) + PutLabel(901);
			medoutput << Label.c_str();
		}
		if(FResMediosSensor.Input) {
			Label = "\t" + PutLabel(708) + std::to_string(FNumeroSensor) + PutLabel(901);
			medoutput << Label.c_str();
		}

	} catch(exception & N) {
		std::cout << "ERROR: TPIDController::CabeceraResultadosMedSensor en el Sensor " << FNumeroSensor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TSensor::CabeceraResultadosInsSensor(stringstream& insoutput) {
	try {
		std::string Label;

		if(FResInstantSensor.Output) {
			Label = "\t" + PutLabel(707) + std::to_string(FNumeroSensor) + PutLabel(901);
			insoutput << Label.c_str();
		}
		if(FResInstantSensor.Input) {
			Label = "\t" + PutLabel(708) + std::to_string(FNumeroSensor) + PutLabel(901);
			insoutput << Label.c_str();
		}
	} catch(exception & N) {
		std::cout << "ERROR: TPIDController::CabeceraResultadosInsSensor en el Sensor " << FNumeroSensor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TSensor::ImprimeResultadosMedSensor(stringstream& medoutput) {
	try {
		std::string Label;

		if(FResMediosSensor.Output) {
			medoutput << "\t" << FResMediosSensor.OutputMED;
		}
		if(FResMediosSensor.Input) {
			medoutput << "\t" << FResMediosSensor.InputMED;
		}

	} catch(exception & N) {
		std::cout << "ERROR: TPIDController::ImprimeResultadosMedSensor en el Sensor " << FNumeroSensor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TSensor::ImprimeResultadosInsSensor(stringstream& insoutput) {
	try {
		std::string Label;

		if(FResInstantSensor.Output) {
			insoutput << "\t" << FResInstantSensor.OutputINS;
		}
		if(FResInstantSensor.Input) {
			insoutput << "\t" << FResInstantSensor.InputINS;
		}
	} catch(exception & N) {
		std::cout << "ERROR: TPIDController::CabeceraResultadosInsSensor en el Sensor " << FNumeroSensor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TSensor::IniciaMedias() {
	try {

		FResMediosSensor.OutputSUM = 0.;
		FResMediosSensor.InputSUM = 0.;
		FResMediosSensor.TiempoSUM = 0.;
		FResMediosSensor.Tiempo0 = 0.;

	} catch(exception & N) {
		std::cout << "ERROR: TPIDController::IniciaMedias en el Sensor: " << FNumeroSensor << std::endl;
		// std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TSensor::ResultadosMediosSensor() {
	try {

		if(FResMediosSensor.Output) {
			FResMediosSensor.OutputMED = FResMediosSensor.OutputSUM / FResMediosSensor.TiempoSUM;
			FResMediosSensor.OutputSUM = 0.;
		}
		if(FResMediosSensor.Input) {
			FResMediosSensor.InputMED = FResMediosSensor.InputSUM / FResMediosSensor.TiempoSUM;
			FResMediosSensor.InputSUM = 0.;
		}
		FResMediosSensor.TiempoSUM = 0;

	} catch(exception & N) {
		std::cout << "ERROR: TPIDController::ResultadosMediosSensor en el eje: " << FNumeroSensor << std::endl;
		// std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TSensor::AcumulaResultadosMediosSensor(double Actual) {
	try {
		/* Lo que se hace en esta funcion se realiza dentro del calculo del eje, para asi poder
		 llevar a cabo la salida de resultados medios por pantalla. */
		double Delta = Actual - FResMediosSensor.Tiempo0;

		if(FResMediosSensor.Output) {
			FResMediosSensor.OutputSUM += FCurrentValue * Delta;
		}
		if(FResMediosSensor.Input) {
			FResMediosSensor.InputSUM += FRealValue * Delta;
		}
		FResMediosSensor.TiempoSUM += Delta;
		FResMediosSensor.Tiempo0 = Actual;

	} catch(exception & N) {
		std::cout << "ERROR: TSensor::AcumulaResultadosMediosSensor en el eje: " << FNumeroSensor << std::endl;
		// std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TSensor::ResultadosInstantSensor() {
	try {
		if(FResInstantSensor.Output)
			FResInstantSensor.OutputINS = FCurrentValue;
		if(FResInstantSensor.Input)
			FResInstantSensor.InputINS = FRealValue;

	} catch(exception & N) {
		std::cout << "ERROR: TPIDController::ResultadosInstantSensor en el eje " << FNumeroSensor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

int TSensor::ObjectID() {
	return FObjectID;
}
;

#pragma package(smart_init)
