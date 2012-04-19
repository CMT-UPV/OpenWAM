/* --------------------------------------------------------------------------------*\
==========================|
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

#ifndef TCCCompresorVolumetricoH
#define TCCCompresorVolumetricoH

#include "TCondicionContorno.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class TCCCompresorVolumetrico : public TCondicionContorno {
private:

	// int FNumeroCV;     // Número Compressor Volumétrico (de tornillo)
	bool asgNumeroCV;

	double FRelacionVelocidadesCV; // Relación de velocidades compresor volumétrico - motor.
	double FPresionCV; // Presión de aspiración.
	double FTemperaturaCV; // Temperature de aspiración.

	// Declaración de los coeficientes de caudal, temperatura y potencia del compresor volumétrico.
	double FC1Caudal, FC2Caudal, FC3Caudal;
	double FC1Temperatura, FC2Temperatura, FC3Temperatura;
	double FC1Potencia, FC2Potencia, FC3Potencia, FC4Potencia, FC5Potencia, FC6Potencia;

	int FNodoFin; // Nodo en el extremo del tubo que está en la condición de contorno.
	int FIndiceCC; // Posición del vector para tomar datos del tubo para la BC (0 Nodo izquierdo; 1 Nodo derecho)
	double *FCC; // Característica conocida del tubo.
	double *FCD; // Característica desconocida del tubo.
	double FSeccionTubo; // Diámetro del tubo en la condición de contorno.

	double FGasto; // Massflow volumétrico del compresor en kg/s.
	double FDensidad; // Density del aire en la aspiración.
	double FTemperature; // Temperature del gas entrante en ºC.
	double FPotencia;
	double FPressure;
	double FRegimen;
	double FVelocity;

	double FSonido; // Velocity del sonido en el tubo.

	stResMediosCV FResMediosCV;
	stResInstantCV FResInstantCV;

	nmRegimenValv FControlRegimen;

	double FGamma3; // Son expresiones con Gamma. Se usan estas variables para no calcularlas tantas veces por instante de tiempo en la misma función.
	double FGamma4;
	double *FComposicion;

	// FUNCIONES PRIVADAS

	// void PutNumeroCV(int valor);

public:

	void PutNumeroCV(int valor) {

		if (!asgNumeroCV) {
			FNumeroCV = valor;
			asgNumeroCV = true;
		}
		else {
			std::cout << "ERROR: Este Compressor Volumétrico ya tiene número asignado" << std::endl;
			throw Exception("");
		}

	}

	int FNumeroCV;

	// Número Compressor Volumétrico (de tornillo)
	int getNumeroCV() {
		return FNumeroCV;
	}

	double getPotenciaCV() {
		return FPotencia;
	};

	TCCCompresorVolumetrico(nmTypeBC TipoCC, int numCC,
		nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
		nmCalculoGamma GammaCalculation, bool ThereIsEGR);

	~TCCCompresorVolumetrico();

	void CalculaCondicionContorno(double Time);

	void LeeCCCompresorVol(char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe,
		bool HayMotor);

	void ObtencionValoresInstantaneos(double ene);

	void ReadAverageResultsCV(char *FileWAM, fpos_t &filepos);

	void CabeceraResultadosMedCV(stringstream& medoutput);

	void ImprimeResultadosMedCV(stringstream& medoutput);

	void ResultadosMediosCV();

	void AcumulaResultadosMediosCV(double Actual);

	void LeeResultadosInstantCV(char *FileWAM, fpos_t &filepos);

	void CabeceraResultadosInstantCV(stringstream& insoutput);

	void ImprimeResultadosInstantCV(stringstream& insoutput);

	void ResultadosInstantCV();

	void IniciaMedias();

};

#endif

