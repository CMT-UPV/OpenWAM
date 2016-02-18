/* --------------------------------------------------------------------------------*\
|==========================|
 |\\   /\ /\   // O pen     | OpenWAM: The Open Source 1D Gas-Dynamic Code
 | \\ |  X  | //  W ave     |
 |  \\ \/_\/ //   A ction   | CMT-Motores Termicos / Universidad Politecnica Valencia
 |   \\/   \//    M odel    |
 |----------------------------------------------------------------------------------
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
#ifndef TCilindroH
#define TCilindroH

#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif

#include "Constantes.h"
#include "Globales.h"
//#include "simstruc.h"
// #include "engine.h"
//#include "act_def.h"
//#include "ACT_v_8_2.cpp"
#include "ACT_Sub_DLL.h"
#include "INJECTION_RATE.cpp"
#include<malloc.h>
// typedef double mxArray;
#include "TController.h"

class TBloqueMotor;
class TCondicionContorno;

enum nmValvula {
	nmAdmision = 0, nmEscape = 1
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class TCilindro {
  private:

  protected:

	// Parametro de entrada del ACT
	double *Fengine_parameters;
	double *Fengine_model_constants;
	double *Ftest_variables;
	double *Finjection_rate;
	double *FCAD_injection_rate;
	int Fsize_inlet_inj;
	double *FCAD_exit;
	double *FHRF_exit;
	double *FROHR_exit;
	double *Fp_cyl_exit;
	double *Fdp_da_cyl_exit;
	double *FT_cyl_exit;
	double *FH_cooler_exit;
	double *Fmean_var_exit;
	double *Fheat_transfer;
	double *Finjection_rate_exit;
	double *Faccum_injection_rate_exit;
	//double *FSpecies_EVO;
	double *FSpecies_IVC;
	sINtype dataIN;
	sOUTtype dataOUT;

	// Parametros de entrada del modelo de Tasa
	int FIN;
	double *FSOP;
	double *FMFI;
	double *FSOI;
	double *FEOI;
	int FCAI;
	double *FA_TASA;
	double *FB_TASA;
	double *FC_TASA;
	double *FD_TASA;

	double FPresionMedAdm;
	double FPresionMedEsc;
	double FTimeAcumAct;

	nmCombustion FCalcComb;

	int FNumeroCilindro;
	TBloqueMotor *FMotor;

	bool FCicloCerrado; // Controla si el cilindro se encuentra en ciclo cerrado (true) o abierto (false)

	// double FMasaFuel;
	double FTasaFuel;
	stDistribucion FDistribucion;

	double FCm;

	// Datos DLL
	bool FHaydll;
	bool FModComb;

	// Asignacion de Valvulas.
	int FNumeroUnionesAdm;
	int FNumeroUnionesEsc;
	TCondicionContorno **FCCValvulaAdm;
	TCondicionContorno **FCCValvulaEsc;
	double FMomentoAngular;
	double FMomentoAngularAdm;
	double FMomentoAngularEsc;
	double Fctorbadmp; // Coeficiente de torbellino medio en el cilindro.

	// Variables Temporales
	double FAnguloActual;
	double FAnguloAnterior;
	double FDeltaAngulo;
	double FTime1;
	double FTime0;
	double FDeltaT;
	double FDesfase;
	double FAnguloComb;
	double FAnguloComb0;
	double FAnguloRetrasoCombustion;
	int FNumeroCiclo;
	double FTiempoCiclo;

	// Variables Termodinamicas
	double FMasaValvAdm;
	double FMasa;
	double FPressure;
	double FPresionCarter;
	double FTemperature;
	double FAsonido;
	double FMasa0;
	double FPresion0;
	double FPreMed; // Media entre FPressure y FPresion0.
	double FTemperatura0;
	double FAsonido0;
	double FVolumen;
	double FVolumen0;
	double FVolumenCA;
	double FMasaAtrapada;
	double FAcumMasaPorAdm;
	double FMasaPorAdmision;
	double FAcumMasaPorEsc;
	double FMasaPorEscape;
	double FAcumMasaEGR;
	double FMasaEGR;
	double FPresionRCA;
	double Fecg0;
	double Fecg;
	double FecgTotal;
	double FecgInt;

	double FParInstantaneo;
	double FTrabajoNeto;
	double FTrabajoBombeo;
	double FTrabajoNetoACUM;
	double FTrabajoBombeoACUM;
	double FPMN;
	double FPMB;
	double FPMI;
	double FDensidadReferencia;
	double FDensidadReferenciaSUM;

	// Variables Combustion
	double FIniComb;
	double FFinComb;
	double *FAngDesplazado;
	double FMfint; // kg/cc    Combustible para la interpolacion de la leyes.
	double FMaint; // kg/cc    Masa de aire para la interpolacion de la leyes.
	double FRegInt; // rpm      Regimen para la interpolacion de las leyes.

	double FNumIny;
	dVector FAnguloInjeccion;
	dVector FTInyeccion;
	dVector FPercentInyeccion;
	double FFuelAcum;
	int ind;

	bool FInyeccion;
	bool FInyeccionPil;
	double FFuelTotal;
	double FFuelInstant;
	double FFuelInstantPil;
	double FMasaBlowBy;
	double FGastoBlowBy;

	double FNIT;
	double FAFR;
	double FSwirl;
	double FSwirlSUM;
	double FWoma;

	// Variable de la ecuacion de Woschni
	double Fequis;
	double Fc1;
	double Fc2;
	double Fcucm;
	double Fc1rc;
	double Fh;
	double Fratioctm;
	double FKctm;
	double FCu;

	double FCalorLiberadoCiclo; // Variable para la impresion por pantalla del resultado.

	stTemperaturasPared *FTempPared;
	stCalor FCalor;
	bool FPrimeraCombustion;
	bool FPrimerInstanteCicloCerrado;

	stValvulasCilindro *FValvAdm;
	stValvulasCilindro *FValvEsc;

	int FNumVarMed;
	int FNumVarIns;

	stResMediosCilindro FResMediosCilindro;
	stResInstantCilindro FResInstantCilindro;

	// Variable de control para el calculo del cilindro.
	// bool FCalculadoPaso;

	// Modelo de Cortocircuito
	double FAlphaEscape;
	double FAlphaAdmision;
	double FMasaCortocircuito;
	double FGastoCortocircuito;

	// Variables del transporte de especies quimicas.
	double FMfquem;
	double FMairequem;
	// double *FFraccionMasicaEspecie;
	dVector FFraccionMasicaEspecie;
	dVector FComposicionSaliente;
	dVector FComposicionCicloCerrado; // 0 -> Quemados, 1 -> Combustible, 2 -> Aire Fresco
	dVector FMasaEspecieCicloCerrado;
	dVector FMasaEspecie;
	double FFraccionMasicaEspecieFuel; // Para calculo completo de especies

	// double GetFraccionMasicaEspecie(int i);
	double GetAireFresco();

	double FGamma;
	double FRMezcla;
	double FCpMezcla;
	double FCvMezcla;
	double FGamma1;
	double FGamma2;
	double FGamma4;
	double FGamma6;
	double FGammaCil; // Se usa cuando se opta por calculo en tubos con gamma constante.
	bool FHayEGR;
	int FIntEGR;
	dVector FFraccionComienzoCicloCerrado;
	double FUfgasoil;
	double FHcl;

	double FRelacionMolarH2O_O2;
	double FRelacionMolarN2_O2;
	double FMolesCombQuemado;
	double FXComb;
	double FYComb;
	double FZComb;
	double FMasaO2Reactivos;
	double FMasaH2OReactivos;
	double FMasaN2Reactivos;
	double FMasaH2OProductos;
	double FMasaN2Productos;
	double FMasaCO2Productos;
	double FDosadoEstequiometrico;

	int FMfControllerID;
	bool FMfControlled;
	TController *FMfController;

	double CalculaVolumen(double AnguloActual);

	void InicioFinCombustion();

	double fun_wiebe(double x, int ley, int wiebe);

	double fql(double x, int ley);

	double CalculaCalorLiberado(double x);

	double FuncionGamma(double T, double X);

	void CalculaFuelMEP(double MasaAire);

	double CalculaCTorbAdmProm();

	double CalculaCm();

	void CalculoNIT();

	void CalculaMomentoAngular();

	void CalculaSWIRL();

	void CalculoVariablesValvulas();

	void CalculaTemperaturasPared();

	// void PutMasaFuel(double valor);

	// void PutInicioComb(double valor);

	// void PutFinComb(double valor);

	// void PutFQL(double valor);

	// void PutHayDLL(bool valor);

	// void PutModComb(bool valor);

	// void PutCalculadoPaso(bool valor);

	// void PutNumeroInyecciones(int valor);

	// void PutPresionInyeccion(double valor);

	// void PutSOP(int i, double valor);

	// void PutMasaFuelPorInyeccion(int i, double valor);

	// void PutFraccionMasicaEspecie(int i, double valor);

  public:

	void PutFraccionMasicaEspecie(int i, double valor) {
		FFraccionMasicaEspecie[i] = valor;
	}
	;

	double GetFraccionMasicaEspecie(int i) {
		return FFraccionMasicaEspecie[i];
	}
	;

	double getParInstantaneo() {
		return FParInstantaneo;
	}
	;

	double getTrabajoNeto() {
		return FTrabajoNeto;
	}
	;

	double getTrabajoBombeo() {
		return FTrabajoBombeo;
	}
	;

	double getPressure() {
		return FPressure;
	}
	;

	double getMasa() {
		return FMasa;
	}
	;

	double getSpeedsound() {
		return FAsonido;
	}
	;

	double getMomentoAngular() {
		return FMomentoAngular;
	}
	;

	double getMasaValvAdm() {
		return FMasaValvAdm;
	}
	;

	void PutMasaFuel(double valor) {
		FMasaFuel = valor;
	}

	double FMasaFuel;

	double getMasaFuel() {
		return FMasaFuel;
	}

	double getMasaPorAdmision() {
		return FMasaPorAdmision;
	}
	;

	double getMasaAtrapada() {
		return FMasaAtrapada;
	}
	;

	stDistribucion getDistribucion() {
		return FDistribucion;
	}
	;

	double getAnguloActual() {
		return FAnguloActual;
	}
	;

	double getAnguloAnterior() {
		return FAnguloAnterior;
	}
	;

	double getSwirlSUM() {
		return FSwirlSUM;
	}
	;

	int getNumeroUnionesAdm() {
		return FNumeroUnionesAdm;
	}
	;

	int getNumeroUnionesEsc() {
		return FNumeroUnionesEsc;
	}
	;

	int getNumeroCilindro() {
		return FNumeroCilindro;
	}
	;

	TCondicionContorno* GetCCValvulaAdm(int i);

	TCondicionContorno* GetCCValvulaEsc(int i);

	double getAFR() {
		return FAFR;
	}
	;

	double getTiempoActual() {
		return FTime1;
	}
	;

	double getTemperature() {
		return FTemperature;
	}
	;

	void PutInicioComb(double valor) {
		FIniComb = valor;
	}

	void PutFinComb(double valor) {
		FFinComb = valor;
	}

	void PutFQL(double valor) {
		FCalor.FQL = valor;
	}

	double getPreMed() {
		return FPreMed;
	}
	;

	double getVolumen() {
		return FVolumen;
	}
	;

	double getVolumen0() {
		return FVolumen0;
	}
	;

	double getGamma() {
		return FGamma;
	}
	;

	void PutHayDLL(bool valor) {
		FHaydll = valor;
	}

	void PutModComb(bool valor) {
		FModComb = valor;
	}

	void PutCalculadoPaso(bool valor) {
		FCalculadoPaso = valor;
	}

	bool FCalculadoPaso;

	bool getCalculadoPaso() {
		return FCalculadoPaso;
	}

	bool FSaturado;

	double GetComposicionSaliente(int i);

	double getFraccionAireFresco() {
		return GetAireFresco();
	}
	;

	void PutNumeroInyecciones(int valor) {
		FIN = valor;
	}
	;
	void PutPresionInyeccion(double valor);

	void PutSOP(int i, double valor) {
		FSOP[i] = valor;
	}
	;

	void PutMasaFuelPorInyeccion(int i, double valor) {
		FMFI[i] = valor;
	}
	;

	void PutMfControllerID(int ControllerID) {
		FMfControllerID = ControllerID;
		FMfControlled = true;
	}
	;

	TCilindro(TBloqueMotor *Engine, bool ThereIsEGR);

	~TCilindro();

	virtual void ActualizaPropiedades(double TiempoActual) = 0;

	void ReadAverageResultsCilindro(const char *FileWAM, fpos_t &filepos);

	void HeaderAverageResultsCilindro(std::stringstream& medoutput, stEspecies *DatosEspecies);

	void ImprimeResultadosMediosCilindro(std::stringstream& medoutput);

	void AcumulaResultadosMediosCilindro(double TActual);

	void CalculaResultadosMediosCilindro();

	void ReadInstantaneousResultsCilindro(const char *FileWAM, fpos_t &filepos);

	void HeaderInstantaneousResultsCilindro(std::stringstream& insoutput, stEspecies *DatosEspecies);

	void ImprimeResultadosInstantaneosCilindro(std::stringstream& insoutput);

	void CalculaResultadosInstantaneosCilindro();

	void IniciaVariables();

	void AsignacionCC(TCondicionContorno **BC, int numCC);

	void CalculaVariablesResultados();

	void SalidaGeneralCilindros();

	void ControlDesfase(double n0);

	void DefineCombustion();

	void AsignMfController(TController **Controller);

};

// ---------------------------------------------------------------------------
#endif
