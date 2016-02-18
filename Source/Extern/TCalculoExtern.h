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
#ifndef TCalculoExternH
#define TCalculoExternH
#define maxnd 80 /*n. maximo de nodos que llegan a depositos*/

// #include "engine.h"
#include "Constantes.h"
#include "Globales.h"

#include <string.h>
#include <cstdio>
#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
//#include <cmath>
#include <fstream>

// class TMatlab;
class TTubo;
class TDeposito;
class TCCDeposito;
class TCondicionContorno;
class TTGV;
class TControlFuel;
class TEGRV;
class TRegimenMotor;
class Tfql;
class TEjeTurbogrupo;
class TTurbina;
class TVenturi;
class TBloqueMotor;
class TRemansoMatlab;
class TCoefDescarga;
class TAjusteTransCalorCil;
class TFraccionMasicaCilindro;
class TControlInyeccion;
class TControlK;

enum nmTipoSenTubo {
	nmPrTubo = 0, nmVelTubo = 1, nmTempTubo = 2, nmGastoTubo = 3, nmN2Tubo = 4, nmO2Tubo = 5, nmCO2Tubo = 6, nmH2OTubo = 7, nmHCTubo = 8, nmSootTubo = 9, nmNOxTubo = 10, nmCOTubo = 11, nmAireFrTubo = 12, nmGasQuemadoTubo = 13, nmEGRTubo = 14, nmCombustibleTubo = 15
};

enum nmTipoSenDep {
	nmPrDep = 0, nmTempDep = 1, nmN2Dep = 2, nmO2Dep = 3, nmCO2Dep = 4, nmH2ODep = 5, nmHCDep = 6, nmSootDep = 7, nmNOxDep = 8, nmCODep = 9, nmAireFrDep = 10, nmGasQuemadoDep = 11, nmEGRDep = 12, nmCombustibleDep = 13
};

enum nmTipoSenTG {
	nmRTCTG = 0
};

enum nmTipoSenTurbina {
	nmRelaci1 = 0, nmRelaci2 = 1
};

enum nmTipoSenCil {
	nmPrCil = 0, nmTempCil = 1, nmMfCil = 2, nmAFRCil = 3
};

enum nmTipoSenVent {
	nmPrEntVent = 0, nmPrGarVent = 1, nmVelEntVent = 2, nmVelLatVent = 3, nmGastoEntVent = 4, nmGastoLatVent = 5
};

enum nmTipoSenMotor {
	nmAngulo = 0, nmReg = 1, nmTiempo = 2
};

enum nmTipoSenUED {
	nmGasto = 0
};

struct stSensorTubos {
	int tubo;
	int nodos;
	double deltax;
	double distancia;
	double ctetiempo;
	double ganancia;
	double valact;
	double valant;
	double valreal;
	double valrealant;
	nmTipoSenTubo tipo;
};

struct stSensorDep {
	int deposito;
	double ctetiempo;
	double ganancia;
	double valact;
	double valant;
	double valreal;
	double valrealant;
	nmTipoSenDep tipo;
};

struct stSensorTG {
	int turbogrupo;
	double ctetiempo;
	double ganancia;
	double valact;
	double valant;
	double valreal;
	double valrealant;
	nmTipoSenTG tipo;
};

struct stSensorTurbina {
	int turbina;
	double ctetiempo;
	double ganancia;
	double valact;
	double valant;
	double valreal;
	double valrealant;
	nmTipoSenTurbina tipo;
};

struct stSensorCil {
	int cilindro;
	double ctetiempo;
	double ganancia;
	double valact;
	double valant;
	double valreal;
	double valrealant;
	nmTipoSenCil tipo;
};

struct stSensorVent {
	int venturi;
	double ctetiempo;
	double ganancia;
	double valact;
	double valant;
	double valreal;
	double valrealant;
	nmTipoSenVent tipo;
};

struct stSensorMotor {
	int motor;
	double ctetiempo;
	double ganancia;
	double valact;
	double valant;
	double valreal;
	double valrealant;
	nmTipoSenMotor tipo;
};

struct stSensorUED {
	int CCUED;
	double ctetiempo;
	double ganancia;
	double valact;
	double valant;
	double valreal;
	double valrealant;
	nmTipoSenUED tipo;
};

class TCalculoExtern {
  private:
	// Variables
	int FNumeroInMat; 		// Numero de entradas a matlab
	int FNumeroOutMat; 		// Numero de salidas de matlab

	int FNInputsdll; 		// Numero de entradas de la dll
	int FNOutputsdll; 		// Numero de salidas de la dll

	int FNSensTubos; 		// Numero de sensores en tubos
	int FNSensDepositos; 	// Numero de sensores en depositos
	int FNSensTurbogrupo; 	// Numero de sensores en turbogrupos
	int FNSensTurbina; 		// Numero de sensores en turbina
	int FNSensMotor; 		// Numero de sensores en el motor
	int FNSensVenturis; 	// Numero de sensores en venturis
	int FNSensCilindros; 	// Numero de sensores en cilindros
	int FNSensUED; 			// Numero de sensores en uniones entre depositos

	double *FInputs; 		// Vector de comunicacion WAM -> DLL
	double *FOutputs; 		// Vector de comunicacion DLL -> WAM
	double *FEntrada; 		// Vector de comunicacion DLL -> Matlab
	double *FSalida; 		// Vector de comunicacion Matlab -> DLL
	// TMatlab *ECU;        // Objeto TMatlab para la programacion de la ECU
	TTGV **FTGV; 			// Objeto para el calculo de la TGV
	TControlFuel *FFuel; 	// Objeto para el calculo del Fuel
	TEGRV *FEGRV; 			// Objeto para el calculo de la valvula de EGR.
	TRegimenMotor *FRegimen; // Objeto para el calculo del Regimen del motor.
	Tfql *Ffql; // Objeto para el calculo de las Leyes de Liberacion de Calor.
	TRemansoMatlab **FRemansoMatlab; // Objeto para el calculo de la presion y temperatura en un deposito de remanso.
	TCoefDescarga **FCoefDescarga; // Objeto para el calculo del coeficiente de descarga de una union a deposito/cilindro
	TAjusteTransCalorCil
	*FAjusteTransCalorCil; // Objeto para el calculo de los coeficientes de ajuste de transmision de calor en admision y escape
	TFraccionMasicaCilindro *FFraccionMasicaCilindro; // Objeto para imponer la fraccion masica a la apertura del cilindro.
	TControlInyeccion
	*FControlInyeccion; // Objeto para determinar el numero de inyecciones, la presion de inyeccion y la duracion y masa inyectada en cada inyeccion.
	TControlK **FControlK; // Objeto para calcular la variacion de K con el Re en cada CC de Perdida de Presion
	char *FRutaTrabajo;

	// Indice que indica la posicion de cada especie en el vector de fraccion masica.
	int FIndiceTuboN2, FIndiceTuboO2, FIndiceTuboCO2, FIndiceTuboH2O, FIndiceTuboHC, FIndiceTuboSoot, FIndiceTuboNOx,
		FIndiceTuboCO, FIndiceTuboAireFresco, FIndiceTuboGasQuemado, FIndiceTuboEGR,
		FIndiceTuboComb;
	int FIndiceDepN2, FIndiceDepO2, FIndiceDepCO2, FIndiceDepH2O, FIndiceDepHC, FIndiceDepSoot, FIndiceDepNOx, FIndiceDepCO,
		FIndiceDepAireFresco, FIndiceDepGasQuemado, FIndiceDepEGR, FIndiceDepComb;

	bool Fhayecu, Fcontroliny, Fcontrolmfcomb, Fmodcomb, Fajustbaraba, FFraccionMasicaEspeciesCil;
	bool FCalculoK;
	int Fngrafmat;
	double Ftmuestreoecu, FTiempo, FTiempomuestreo, FTiempoSum;

	int Fntact;

	int Fnematlab; // N. de depositos de remanso matlab
	int Fnumunion; // N. de uniones controladas

	// Variables para la determinacion de la correlacion utilizada en las turbinas.
	double CoefCorrCd[2], CoefCorrRend[2];
	// N. de turbinas.
	int Fcontadortgv;
	int FNumPerdidasPresion;

	/* Analisis Convergencia de la Ejecucion */
	bool FConvergencia;
	bool FPrimeraVez;

	double *FParametros; // Parametros para interpolacion dFQL

	double FMasaFuel;
	double FMasaFuelDeseado;
	double FFuelIni;
	double FCDEGRIni;
	double FCDEstatorIni;
	double FCDRotorIni;
	double FRendimientoIni;
	double FTiempoPrevio;

	nmCalculoGamma FCalculoGamma;

	stSensorTubos *FSensorTubo;
	stSensorDep *FSensorDep;
	stSensorTG *FSensorTG;
	stSensorTurbina *FSensorTurbina;
	stSensorCil *FSensorCil;
	stSensorVent *FSensorVent;
	stSensorMotor *FSensorMotor;
	stSensorUED *FSensorUED;

	// Variable para salida de resultados medios/instantaneos

	double PosicionTurbina;
	double PosicionTurbinaSUM;
	double SensorGastoAdmision;
	double SensorGastoAdmisionSUM;
	double SensorP2;
	double SensorP2SUM;
	double SensorP3;
	double SensorP3SUM;
	double SensorP4;
	double SensorP4SUM;
	double SensorRegimenTurbo;
	double SensorRegimenTurboSUM;
	double SensorRelCinematica;
	double SensorRelCinematicaSUM;

	void PutInputdll(int i, double valor);

	TControlFuel* GetFuel();

  public:
	// ---------------------------------------------------------------------------
	// Recuerda que aqui se encuentran las llamadas entre wam y la dll, siempre
	// que cambies algo aqui deberas recompilar el wam y modificar las llamadas
	// ---------------------------------------------------------------------------
	// Variables
	bool getajustbaraba() {
		return Fajustbaraba;
	}
	;

	// Funciones
	double GetOutput_dll(int i);

	bool getcontrolmfcomb() {
		return Fcontrolmfcomb;
	}
	;

	int getngrafmat() {
		return Fngrafmat;
	}
	;

	bool getmodcomb() {
		return Fmodcomb;
	}
	;

	bool getcontroliny() {
		return Fcontroliny;
	}
	;

	bool getFraccionMasicaEspeciesCil() {
		return FFraccionMasicaEspeciesCil;
	}
	;

	bool getControlK() {
		return FCalculoK;
	}
	;

	TRemansoMatlab* GetTRemansoMatlab(int i);

	TCoefDescarga* GetTCoefDescarga(int i);

	TControlFuel* getFuel() {
		return GetFuel();
	}
	;

	bool getConvergencia() {
		return FConvergencia;
	}
	;

	// Funciones
	TCalculoExtern();

	~TCalculoExtern();

	void LlamadaECU(double DeltaT, TBloqueMotor **Engine);

	void FinECU();

	// void LeeFicherosDLL(FILE *fich,int controlvalv,int nematlab,
	// int ncilin,int nunmat,int CountVGT);

	void LeeFicherosDLL(const char *FileWAM, fpos_t &filepos, int controlvalv, int nematlab, int ncilin, int nunmat,
						int CountVGT, int numespecies, int NumeroPerdidasPresion);

	// Funciones para la lectura de sensores
	void Lee_Sens_Tubos(const char *FileWAM, fpos_t &filepos, TTubo **Pipe, nmTipoCalculoEspecies SpeciesModel,
						bool ThereIsEGR, bool HayCombustible);

	void Lee_Sens_Dep(const char *FileWAM, fpos_t &filepos, TDeposito **Plenum, nmTipoCalculoEspecies SpeciesModel,
					  bool ThereIsEGR, bool HayCombustible);

	void Lee_Sens_TG(const char *FileWAM, fpos_t &filepos, TEjeTurbogrupo **Axis);

	void Lee_Sens_Turbina(const char *FileWAM, fpos_t &filepos, TTurbina **Turbine);

	void Lee_Sens_Cil(const char *FileWAM, fpos_t &filepos, TBloqueMotor **Engine);

	void Lee_Sens_Vent(const char *FileWAM, fpos_t &filepos, TVenturi **Venturi);

	void Lee_Sens_Motor(const char *FileWAM, fpos_t &filepos, double CrankAngle, double ene, double AcumulatedTime);

	void Lee_Sens_UED(const char *FileWAM, fpos_t &filepos, TCondicionContorno **BC);

	void Lectura_Datos_Adicionales(const char *FileWAM, fpos_t &filepos);

	void IniciaEntradaDLL();

	// Funciones para el calculo de sensores
	void Calculo_Sensores_Tubos(TTubo **Pipe, double deltaT);

	void Calculo_Sensores_Deposito(TDeposito **Plenum, double deltaT);

	void Calculo_Sensores_TG(double deltaT, TEjeTurbogrupo **Axis);

	void Calculo_Sensores_Turbina(double deltaT, TTurbina **Turbine);

	void Calculo_Sensores_Cilindro(double deltaT, TBloqueMotor **Engine);

	void Calculo_Sensores_Venturi(double deltaT, TVenturi **Venturi);

	void Calculo_Sensores_Motor(double deltaT, TBloqueMotor **Engine, double AcumulatedTime);

	void Calculo_Sensores_UED(double deltaT, TCondicionContorno **BC);

	// Funcion de interpolacion
	double xit_(double vizq, double vder, double axid, double xif);

	// Funciones para graficar
	void InicializaMedias();

	void AcumulaMedias(double DeltaT);

	void CalculaMedias();

	void ImprimeCabeceraMedias(stringstream& medoutput);

	void ImprimeGraficosMedias(stringstream& medoutput);

	void ImprimeCabeceraInstantaneas(stringstream& insoutput);

	void ImprimeGraficosInstantaneas(stringstream& insoutput);

	void ObtenerRutaTrabajo(const char *origin);

};

// ---------------------------------------------------------------------------
#endif

