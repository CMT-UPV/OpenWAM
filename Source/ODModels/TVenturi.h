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
#ifndef TVenturiH
#define TVenturiH
#include "TDepVolCteBase.h"

class TVenturi: public TDepVolCteBase {
  private:
	// ---------------------------------------------------------------------------
	// VARIABLES PRIVADAS
	// ---------------------------------------------------------------------------

	bool asgNumeroVenturi;
	// int FNumeroVenturi; // Numero del venturi
	int FNodoEntrada; // Numero del nodo de entrada
	int FNodoSalida; // Numero del nodo de salida
	int FNodoLateral; // Numero del nodo lateral
	int FUnionEntrada; // Numero de la union de entrada sobre el total de uniones del venturi
	int FUnionSalida; // Numero de la union de salida sobre el total de uniones del venturi
	int FUnionLateral; // Numero de la union lateral sobre el total de uniones del venturi
	/* int FSentidoSalida;    // Sentido del flujo en el nodo de salida.
	 int FSentidoEntrada;   // Sentido del flujo en el nodo de entrada.
	 int FSentidoLateral;   // Sentido del flujo en el nodo de lateral. */
	double FRelacionSecciones;
	double FRendimientoVenturi;
	double FPerdidasCalor;
	double FCoefEntrada; // Estas tres variables Ya no se usan. Se han sustitiudo por el
	double FCoefSalida; // coeficiente de recuperacion.Borrarlas tras validar. Pedro.
	double FCoefLateral;
	stResInstantVenturi FResInstantVenturi;
	stResMediosVenturi FResMediosVenturi;
	double FMach0;
	double FMach1;
	TCondicionContorno *FCCEntrada;
	TCondicionContorno *FCCSalida;
	TCondicionContorno *FCCLateral;

	// ---------------------------------------------------------------------------
	// FUNCIONES PRIVADAS
	// ---------------------------------------------------------------------------

	// void PutNumeroVenturi(int NumeroVenturi);

  protected:

  public:
	// ---------------------------------------------------------------------------
	// VARIABLES PUBLICAS
	// ---------------------------------------------------------------------------

	void PutNumeroVenturi(int NumeroVenturi) {
		if(!asgNumeroVenturi) {
			FNumeroVenturi = NumeroVenturi;
			asgNumeroVenturi = true;
		} else {
			std::cout << "ERROR: Este Venturi ya tiene numero asignado" << std::endl;
			throw Exception("");
		}
	}

	int FNumeroVenturi;
	// Numero del venturi
	int getNumeroVenturi() {
		return FNumeroVenturi;
	}

	int getNodoEntrada() {
		return FNodoEntrada;
	}
	;

	int getNodoSalida() {
		return FNodoSalida;
	}
	;

	int getNodoLateral() {
		return FNodoLateral;
	}
	;

	double getCoefEntrada() {
		return FCoefEntrada;
	}
	;

	double getCoefSalida() {
		return FCoefSalida;
	}
	;

	double getCoefLateral() {
		return FCoefLateral;
	}
	;

	double getPressure() {
		return FPressure;
	}
	;

	TCondicionContorno *getCCEntrada() {
		return FCCEntrada;
	}
	;

	TCondicionContorno *getCCLateral() {
		return FCCLateral;
	}
	;

	double getGamma() {
		return FGamma;
	}
	;

	double getgamma1() {
		return FGamma1;
	}
	;

	double getgamma3() {
		return FGamma3;
	}
	;

	// ---------------------------------------------------------------------------
	// FUNCIONES PUBLICAS
	// ---------------------------------------------------------------------------

	TVenturi(int i, nmTipoCalculoEspecies SpeciesModel, int numeroespecies, nmCalculoGamma GammaCalculation,
			 bool ThereIsEGR);

	~TVenturi();

	void ActualizaPropiedades(double TimeCalculo);

	void CalculaVenturi();

	void LeeResultadosInstantVenturi(const char *FileWAM, fpos_t &filepos);

	void CabeceraResultadosInstantVenturi(stringstream& insoutput);

	void ImprimeResultadosInstantVenturi(stringstream& insoutput);

	void CalculaResultadosVenturi();

	void ReadAverageResultsVenturi(const char *FileWAM, fpos_t &filepos);

	void HeaderAverageResultsVenturi(stringstream& medoutput);

	void AcumulaResultadosMediosVenturi(double Actual);

	void ResultadosMediosVenturi();

	void ImprimeResultadosMediosVenturi(stringstream& medoutput);

	void LeeDatosVenturi(const char *FileWAM, fpos_t &filepos);

	void AsignaEntradaSalidaLateralCC();

	void UpdateProperties0DModel(double TimeCalculo);

};

// ---------------------------------------------------------------------------
#endif

