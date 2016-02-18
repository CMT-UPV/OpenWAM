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
#ifndef TBloqueMotorH
#define TBloqueMotorH
#include <iostream>

#include "TCilindro4T.h"
#include "TCilindro2T.h"
#include "Constantes.h"
#include "Globales.h"
#include "TController.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
class TTubo;

class TBloqueMotor {
  private:

	double FTemperaturaAmbiente;
	double FPresionAmbiente;

	TTubo *FTuboRendVol;
	int FNodoMedio;
	int FNumTuboRendVol; /* Pipe de la admision al que se refiere el rendimiento volumetrico */

	std::vector<stLeyQuemadoBD> FLeyQuemadoBD;

	double FLQRegMax;
	double FLQMfMax;
	double FLQMaMax;

	int FTipoDatosIny;
	int FNumeroInyecciones;
	dVector FAngIny;
	dVector FTIny;
	dVector FPercentIny;
	double FAngIniIny;
	double FTStepIny;
	double FAStepIny;
	int xnum;
	dVector FY_dat;
	dVector FX_dat;
	double FFuelTasaInt;
	int TipoInterp; // Tipo de interpolación: 1. Lineal, 2. Hermite (spinlines), 3. Step

	double fOutput;
	Base_interp *fDatosTasa;		//!< Struct to interpolate within the table
	nmTipoInterpolacion fTipo;			//!< Type of interpolation

	double FTime;

	bool FACT;
	double FMixtureProcessCte;

	nmTipoModelado FTipoModelado;

	stWoschni FWoschni;
	stGeometria FGeom;
	stPropTermicas FParedCulata;
	stPropTermicas FParedPiston;
	stPropTermicas FParedCilindro;
	stRoadLoad FCoefRoadLoad;
	stInjectionSys FInjectionSys;

	std::vector<stInjecPulse> FInjecPulse;

	stTemperaturasPared FTempInicial;
	nmCalculoPared FCalculoPared;

	stResMediosMotor FResMediosMotor;

	nmTipoDesfase FTipoDesfase;
	int FNumeroCiclosSinInerciaTermica;

	int FNumeroLeyesQuemado;
	int FNWiebes;

	double *FDesfase;
	// double FRegimen;
	double FPresionInicialRCA;
	double FMasaInicial;
	double FPresionAAE;
	double FMasaFuel;
	double FDosadoInicial;
	double FRendimientoCombustion;
	double FPoderCalorifico;
	double FDensidadCombustible;

	// double FAjusteTranCalAdm;
	// double FAjusteTranCalEsc;
	double FParPotMax;
	double FTempRefrigerante;

	TCilindro **FCilindro;

	double FAngTotalCiclo;

	nmTipoMotor FTipoMotor;
	nmTipoCombustion FCombustible;
	nmCalculoPAAE FCalculoDePAAE;

	stPerdMecanicas FPerMec;

	double FMasaTotalVehiculo;
	double FInerciaTotal;
	double FRelCajaCambios;
	double FRelTrasmision;
	double FRendCajaCambios;
	double FRadioRueda;
	double FAnguloCarretera;
	double FPendiente;
	double FCoeficienteInercias;
	double FRoadLoad;
	double FParResistente;
	double FParPerdidasMecanicas;
	double FParMotor;

	// double FTheta;

	double FTrabajoNetoMotor;
	double FTrabajoBombeoMotor;
	double FPMNMotor;
	double FPMBMotor;
	double FPMIMotor;
	double FPMEMotor;
	double FPMPMMotor;
	double FParEfectivo;
	double FPotenciaEfectiva;
	double FRendimientoEfectivo;
	double FRendimientoIndicado;
	double FConsumoEspecifico;
	double FVelocidadVehiculo;

	double FAFRMedio;

	// int FCiclo; // Controla el numero de ciclo de la ejecucion. (para salida de resultados)
	bool FPrimeravezAcumulaFuel; // Variable para controlar en el 1er ciclo la salida en resultados de MFuel.
	bool FPrimeravezAcumulaMasaAtrapada; // Variable para controlar en el 1er ciclo la salida en resultados del Rend Volumetrico.

	// Transporte de Especies Quimicas
	nmTipoCalculoEspecies FCalculoEspecies;
	int FNumeroEspecies;
	bool FImponerComposicionAE;
	double *FComposicionInicial;
	double *FComposicionAtmosfera;
	nmCalculoGamma FCalculoGamma;
	bool FHayEGR;
	int FIntEGR;

	TController *FRPMController;
	int FRPMControllerID;
	bool FRPMControlled;

	TController *FMfController;
	int FMfControllerID;
	bool FMfControlled;

	// void PutTheta(double valor);

	// void PutRegimen(double valor);

	// void PutCiclo(int valor);

	// ---------------------------------------------------------------------------
	// ---------------------------------------------------------------------------

  protected:

	// ---------------------------------------------------------------------------
	// ---------------------------------------------------------------------------

  public:

	bool getACT() {
		return FACT;
	}
	;

	double getMixtureProcessCte() {
		return FMixtureProcessCte;
	}
	;

	stGeometria getGeometria() {
		return FGeom;
	}
	;

	stWoschni getWoschni() {
		return FWoschni;
	}
	;

	void PutRegimen(double valor) {
		try {

			FRegimen = valor;

		} catch(exception & N) {
			std::cout << "ERROR: TBloqueMotor::PutRegimen en el EngineBlock. " << std::endl;
			std::cout << "Tipo de error: " << N.what() << std::endl;
			throw Exception(N.what());
		}
	}

	double FRegimen;

	double getRegimen() {
		return FRegimen;
	}

	double getMasaFuel() {
		return FMasaFuel;
	}
	;

	void setMasaFuel(double fuel) {
		FMasaFuel = fuel;
	}

	double getMasaInicial() {
		return FMasaInicial;
	}
	;

	double getPresionAAE() {
		return FPresionAAE;
	}
	;

	double getPresionInicial() {
		return FPresionInicialRCA;
	}
	;

	stTemperaturasPared getTempInicial() {
		return FTempInicial;
	}
	;

	void PutTheta(double valor) {
		try {
			FTheta = valor;
		} catch(exception & N) {
			std::cout << "ERROR: TBloqueMotor::PutTheta en el EngineBlock. " << std::endl;
			std::cout << "Tipo de error: " << N.what() << std::endl;
			throw Exception(N.what());
		}
	}

	double FTheta;

	double getTheta() {
		return FTheta;
	}

	void PutCiclo(int valor) {
		try {

			FCiclo = valor;

		} catch(exception & N) {
			std::cout << "ERROR: TBloqueMotor::PutCiclo en el EngineBlock. " << std::endl;
			std::cout << "Tipo de error: " << N.what() << std::endl;
			throw Exception(N.what());
		}
	}

	int FCiclo; // Controla el numero de ciclo de la ejecucion. (para salida de resultados)

	int getCiclo() {
		return FCiclo;
	}

	double getAngTotalCiclo() {
		return FAngTotalCiclo;
	}
	;

	double GetDesfase(int i);

	nmTipoMotor getEngineType() {
		return FTipoMotor;
	}
	;

	nmTipoCombustion getCombustible() {
		return FCombustible;
	}
	;

	double getPoderCalorifico() {
		return FPoderCalorifico;
	}
	;

	double getDensidadCombustible() {
		return FDensidadCombustible;
	}
	;

	double getRendimientoCombustion() {
		return FRendimientoCombustion;
	}
	;

	double getDosadoInicial() {
		return FDosadoInicial;
	}
	;

	stPropTermicas getParedPiston() {
		return FParedPiston;
	}
	;

	stPropTermicas getParedCulata() {
		return FParedCulata;
	}
	;

	stPropTermicas getParedCilindro() {
		return FParedCilindro;
	}
	;

	nmCalculoPared getCalculoPared() {
		return FCalculoPared;
	}
	;

	double FAjusteTranCalAdm;

	double getAjusteTranCalAdm() {
		return FAjusteTranCalAdm;
	}
	void PutATCAdm(double valor);

	double FAjusteTranCalEsc;

	double getAjusteTranCalEsc() {
		return FAjusteTranCalEsc;
	}

	void PutATCEsc(double valor);

	double getParPotMax() {
		return FParPotMax;
	}
	;

	const std::vector<stLeyQuemadoBD> getLeyQuemadoBD() {
		return FLeyQuemadoBD;
	}
	;

	double getLQRegMax() {
		return FLQRegMax;
	}
	;

	double getLQMfMax() {
		return FLQMfMax;
	}
	;

	double getLQMaMax() {
		return FLQMaMax;
	}
	;

	double getPresionAmb() {
		return FPresionAmbiente;
	}
	;

	double getTemperaturaAmb() {
		return FTemperaturaAmbiente;
	}
	;

	double getTempRefrigerante() {
		return FTempRefrigerante;
	}
	;

	int getNumCiclosSinInerciaTermica() {
		return FNumeroCiclosSinInerciaTermica;
	}
	;

	int getNumTuboRendVol() {
		return FNumTuboRendVol;
	}
	;

	TCilindro* GetCilindro(int i);

	TTubo* getTuboRendVol() {
		return FTuboRendVol;
	}
	;

	int getNodoMedio() {
		return FNodoMedio;
	}
	;

	double getAFRMedio() {
		return FAFRMedio;
	}
	;

	nmCalculoPAAE getCalculoDePAAE() {
		return FCalculoDePAAE;
	}
	;

	nmTipoCalculoEspecies getSpeciesModel() {
		return FCalculoEspecies;
	}
	;

	int getSpeciesNumber() {
		return FNumeroEspecies;
	}
	;

	bool getImponerComposicionAE() {
		return FImponerComposicionAE;
	}
	;

	double GetComposicionInicial(int i);

	double GetComposicionAtmosfera(int i);

	nmCalculoGamma getGammaCalculation() {
		return FCalculoGamma;
	}
	;

	stInjectionSys getInjectionSys() {
		return FInjectionSys;
	}
	;

	stInjecPulse getInjecPulse(int i) {
		return FInjecPulse[i];
	}
	;

	int getFTipoDatosIny() {
		return FTipoDatosIny;
	}
	;

	int getFNumeroInyecciones() {
		return FNumeroInyecciones;
	}
	;

	dVector getFAngIny() {
		return FAngIny;
	}
	;

	dVector getFTIny() {
		return FTIny;
	}
	;

	dVector getFPercentIny() {
		return FPercentIny;
	}
	;

	double getFAngIniIny() {
		return FAngIniIny;
	}
	;

	double getFAStepIny() {
		return FAStepIny;
	}
	;

//	dVector getFY_dat() {
//		return FY_dat;
//	};
//
//	dVector getFX_dat() {
//		return FX_dat;
//	};
//
//	int getxnum() {
//		return xnum;
//	};

	TBloqueMotor(double AmbientPressure, double AmbientTemperature, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
				 nmCalculoGamma GammaCalculation, bool ThereIsEGR);

	~TBloqueMotor();

	void LeeMotor(const char *FileWAM, fpos_t &filepos, nmTipoModelado& SimulationType, int CiclosSinInerciaTermica,
				  nmTipoMotor EngineType, double *AtmosphericComposition);

	void IniciaAnguloCalculo();

	void ReadAverageResultsBloqueMotor(const char *FileWAM, fpos_t &filepos);

	void HeaderAverageResultsBloqueMotor(std::stringstream& medoutput);

	void ImprimeResultadosMediosBloqueMotor(std::stringstream& medoutput);

	void ResultadosMediosBloqueMotor();

	void AcumulaResultadosMediosBloqueMotor(double TActual, int CilindroActual);

	void PrestacionesMotor();

	void ModeloDeVehiculo(double Time);

	void AsignacionTuboRendVol(TTubo **Pipe);

	void IniciaVarCilindro();

	void AsignRPMController(TController **Controller);

	void AsignMfController(TController **Controller);

	void NewInjectionData(double Time);

	double TasaInyInterp(double Angle);

};

// ---------------------------------------------------------------------------
#endif
