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
#ifndef GlobalesH
#define GlobalesH

#include <vector>
#include <iostream>
#include <sstream>
#include <cstring>
//#include <system.hpp>
#include <ctime>
// #include <sys\timeb.h>
#include "Constantes.h"
#include "Math_wam.h"
#include "labels.hpp"
#ifndef __BORLANDC__
#include "Exception.hpp"
#define ffGeneral 3
#endif
#ifdef WITH_OPENMP
#include <omp.h>
#endif

// #include "StringDataBase.h"
// ---------------------------------------------------------------------------

#define vers 2200 /*n. de version de WAM*/

#define usetry 1
//#define WriteINS 1
//#define ConcentricElement 1
//#define ParticulateFilter 1
//#define tchtm 1

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

inline int matherr(struct exception *a) {

	return 0;
}

// ENUM�s

// ! Different ways to define how the turbine discharge coefficients are calculated
enum nmTurbineType {
	nmFixedTurbine, /* !< The turbine discharge coefficients are constants. */
	nmVariableGeometry, /* !< The turbine discharge coefficients are modified externally. */
	nmTurbineMap /* !< The turbine discharge coefficients are obtained from the turbine map. */
};

// ! Turbocharger speed calculation
enum nmAxisSpeedCalculation {
	nmFixed = 0, /* !< The turbocharger speed is constant during all the dimulation */
	nmVariable = 1 /* !< The turbocharger speed is calculated depending on the turbina and compressor work balance */
};

// ! Model used to calculate the compressor
enum nmCompressorModel {
	nmCompOriginal = 0, /* !< Use plenum to pipe compressor model */
	nmCompPlenums = 1, /* !< Use plenum to plenum compressor model */
	nmCompPipes = 2 /* !< Use pipe to pipe compressor model */
};

// enum nmTipoCompresor{nmCompDep=0,nmCompTub=1};

// ! Identify the boundary of the pipe
enum nmPipeEnd {
	nmLeft = 0, /* !< Left boundary of the pipe */
	nmRight = 1 /* !< Right boundary of the pipe */
};

// De donde toma el aire el compresor
// ! Define the compressor inlet
enum nmCompressorInlet {
	nmAtmosphere = 0, /* !< The compressor inlet is the ambient */
	nmPipe = 1, /* !< The compressor inlet is a pipe */
	nmPlenum = 2 /* !< The compressor inlet is a plenum */
};

// ! Define the type of boundary condition.
enum nmTypeBC {
	nmOpenEndAtmosphere = 0, nmOpenEndReservoir = 1, nmOpenEndCalcExtern = 2, nmClosedEnd = 3, nmAnechoicEnd = 4, nmIncidentPressurWave = 5, nmPipesConnection = 6, nmIntakeValve = 7, nmExhaustValve = 8, nmLinearPressureLoss = 9, nmQuadraticPressureLoss = 10, nmPipeToPlenumConnection = 11, nmBranch = 12, nmVolumetricCompressor = 13, nmInjectionEnd = 14, nmEntradaCompre = 15, nmUnionEntreDepositos = 16, nmCompresor = 17, nmPresionVble = 18, nmCFDConnection = 19, nmExternalConnection = 20
};

enum nmTipoDescarga {
	nmDescargaAtmosfera = 0, nmDescargaRemanso = 1, nmDescargaRemansoMatlab = 2
};

enum nmTipoValv {
	nmValvAdmision = 0, nmValvEscape = 1
};

enum nmCaso {
	nmFlujoEntranteSaltoSubcritico = 0, nmFlujoEntranteSaltoSupercritico = 1, nmFlujoSalienteSaltoSubcritico = 2, nmFlujoSalienteSaltoSupercritico = 3
};

enum nmTipoPP {
	nmPPLineal = 0, nmPPCuadratica = 1
};

enum nmSentidoFlujo {
	nmEntrante = 0, nmSaliente = 1, nmParado = 2
};

enum nmRendimientoTurbina {
	nmWatson = 0, nmPolinomio = 1, nmCalcExtRD = 2, nmRendMapa = 3
};

enum nmAjusteRdTurbina {
	nmConstante = 0, nmCalExtARD = 1
};

enum nmVolumenDeposito {
	nmDepVolCteBase = 0, nmDepVolVariable = 1
};

enum nmTipoDeposito {
	nmDepVolCte = 0, nmDepVolVble = 1, nmTurbinaSimple = 2, nmTurbinaTwin = 3, nmVenturi = 4, nmUnionDireccional = 5
};

enum nmTipoGas {
	nmAireFresco = 0, nmGasEscape = 1
};

//enum nmExtremoTubo {
//	nmLeft = 0, nmRight = 1
//};

enum nmModelo {
	nmLaxWendroff = 0, nmMacCormak = 1, nmCESE = 2, nmTVD = 3
};

enum nmSubModelo {
	nmNinguno = 0, nmFCT = 1
};

enum nmOpcionSubModelo {
	nmNinguna = 0, nmDDNAD = 1, nmDDPAD = 2, nmDDEAD = 3, nmDSNAD = 4, nmDSPAD = 5, nmDSEAD = 6, nmLimiter1 = 7, nmLimiter2 = 8
};

enum nmFormulacionLeyes {
	nmSimple = 0, nmSinArea = 1, nmConArea = 2
};

enum nmTipoFCT {
	nmDamping = 0, nmSmoothing = 1, nmExplicit = 2, nmNaive = 3, nmPhoenical = 4
};

enum nmTipoTransCal {
	nmPipaAdmision = 0, nmTuboAdmision = 1, nmTuboEscape = 2, nmPipaEscape = 3
};

enum nmRefrigerante {
	nmAire = 0, nmAgua = 1
};

enum nmTipoMallado {
	nmDistancia = 1, nmAngular = 2
};

enum nmTipoMotor {
	nm2T = 0, nm4T = 1
};

enum nmTipoDesfase {
	nmPersonalizado = 0, nmImpuesto = 1
};

enum nmTipoCombustion {
	nmMEC = 0, nmMEP = 1
};

enum nmCalculoPAAE {
	nmPAAECalculada = 0, nmPAAEImpuesta = 1
};

enum nmCalculoPared {
	nmSinInercia = 0, nmConInercia = 1, nmTempFija = 2
};

enum nmTipoModelado {
	nmEstacionario = 0, nmTransitorioCarga = 1, nmTransitorioRegimen = 2, nmTransitorioRegimenExterno = 3
};

enum nmRegimenValv {
	nmPropio = 0, nmMotor = 1
};

enum nmTipoCalcTempParedTubos {
	nmVariableConInerciaTermica = 0, nmVariableSinInerciaTermica = 1, nmTempConstante = 2
};

enum nmTipoCalculoEspecies {
	nmCalculoSimple = 0, nmCalculoCompleto = 1
};

enum nmTipoCombustible {
	nmDiesel = 0, nmGasolina = 1, nmBlend = 2, nmNone = 3
};

enum nmCalculoGamma {
	nmGammaConstante = 0, nmComposicion = 1, nmComposicionTemperatura = 2
};

enum nmCombustion {
	nmFQL = 0, nmEXT = 1, nmACT = 2
};

enum nmTipoControl {
	nmControlPadm = 0, nmControlMasaAire = 1
};

enum nmControlMethod {
	nmCtlConstant = 0, nmCtlPID = 1, nmCtlTable = 2, nmCtlSwitch = 3, nmGainCtrl = 4
};

enum nmInputObjet {
	nmInController = 0, nmInSensor = 1
};

enum nmObjectSensed {
	nmSensEjec = 0, nmSensTubo = 1, nmSensDeposito = 2, nmSensMotor = 3
};

enum nmParameterSensed {
	nmTime = 0, nmPressure = 1, nmTemperature = 2, nmMassFlow = 3, nmFuel = 4, nmEngSpeed = 5
};

enum nmTipoInterpolacion {
	nmLineal = 0, nmHermite = 1, nmSteps = 2
};

enum nmCompressorMapFormat {
	nmOldWAMmap = 0, nmSAMmap = 1
};

// DPF
enum nmSeccionCanal {
	nmCuadrada = 0, nmCircular = 1, nmRectangular = 2, nmTriangular = 3
};

enum nmTipoCanal {
	nmCanalEntrada = 0, nmCanalSalida = 1
};

// template<class T>
// double tmpInterp1(double X,T *X1,T *X2,int n){
// return *T[0];
// };

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class TTipoValvula;
class TTubo;
class TController;
class TDPF;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// ESTRUCTURAS

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stEspecies {
	std::string Nombre;
	double R;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stTVD {
	double **Bmas;
	double **Bvector;
	double **Bmen;
	double ***Qmatrix;
	double ***Pmatrix;
	double **gflux;
	double **Alpha;
	double **Beta;
	double **DeltaU;
	double **DeltaB;
	double **DeltaW;
	int **hLandaD;
	double **LandaD;
	double **Phi;
	double **R;
	double **W;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stCombExtern {
	double FQL;
	double Inicio;
	double Final;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stDatosTGV {
	int Turbine;
	int Entradas;
	TTipoValvula **Estator;
	TTipoValvula *Rotor;
	double *Rendimiento;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stGammas {
	double aref, gamma, gai123, gamm1, gams1, gas12, gas123, gads1, gar21, r, pi;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stRun {
	double TimeStep;
	double AngleStep;
	double CycleDuration;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stAcum {
	double Suma;
	double Valor;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResMedCil {
	stAcum TrabajoNeto;
	double PMN;
	stAcum TrabajoBombeo;
	double PMB;
	double Dosado;
	double DosadoRelativo;
	double Pureza;
	stAcum CalorCombustion;
	stAcum CalorCilindro;
	stAcum CalorCulata;
	stAcum CalorPiston;
	double PMI;
	double MasaAtrapada;
	double MasaPorAdmision;
	double MasaPorEscape;
	stAcum MasaBlowBy;
	stAcum TempCilIn;
	stAcum TempCilMed;
	stAcum TempCilOut;
	stAcum TempPisIn;
	stAcum TempPisMed;
	stAcum TempPisOut;
	stAcum TempCulIn;
	stAcum TempCulMed;
	stAcum TempCulOut;
	stAcum NIT;
	double AFR;
	double RendimientoVolumetrico;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResMedMot {
	stAcum ParNeto;
	double PMN;
	stAcum ParEfectivo;
	double PME;
	double Potencia;
	stAcum MasaAdmision;
	double MasaFuel;
	stAcum Regimen;
	double RendimientoVolumetrico;
	stAcum ParPerdidasMecanicas;
	stAcum ParResistente;
	stAcum VelocidadVehiculo;
	stAcum DensidadReferencia;
	double TrabajoNeto;
	double TrabajoBombeo;
	double PMB;
	double Dosado;
	double MasaAtrapada;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/* Estructura para complejos */
// struct complex{
// double r,i;
// };
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
/* Estructura para matriz compleja */
// struct submatriz{
// complex m11,m12,m21,m22;
// };
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
struct stMapa2D {
	int nx;
	int ny;
	double *x;
	double *y;
	double **z;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResInstantDep {
	bool Pressure;
	double PresionINS;
	bool Temperature;
	double TemperaturaINS;
	bool Volumen;
	double VolumenINS;
	bool Masa;
	double MasaINS;
	bool FraccionMasicaEspecies;
	double *FraccionINS;
	bool Gamma;
	double GammaINS;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResMediosDep {
	bool Pressure;
	double PresionSUM;
	double PresionMED;
	bool Temperature;
	double TemperaturaSUM;
	double TemperaturaMED;
	double Tiempo0;
	double TiempoSUM;
	bool FraccionMasicaEspecies;
	double *FraccionSUM;
	double *FraccionMED;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stSensoresDep {
	bool Pressure;
	double GanaPresion;
	double CtePresion;
	double SensPresion;
	double ValorPresion;
	double Valor0Presion;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResMediosTurbina {
	bool Trabajo;
	double TrabajoMED;
	double TrabajoSUM;
	bool Rendimiento;
	double RendimientoMED;
	bool RelaCinematica;
	double *RelaCinematicaMED;
	double RelaCinematicaGlobalMED;
	bool GastoCorregido;
	double *GastoCorregidoMED;
	double *GastoCorregidoSUM;
	bool RegimenCorregido;
	double *RegimenCorregidoMED;
	double *RegimenCorregidoSUM;
	bool RelacionExpansion;
	double *RelacionExpansionMED;
	double *RelacionExpansionSUM;
	double Tiempo0;
	double TiempoSUM;

};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResInstantTurbina {
	bool Potencia;
	double PotenciaINS;
	bool Rendimiento;
	double RendimientoINS;
	bool RelaCinematica;
	double *RelaCinematicaINS;
	bool GastoCorregido;
	double *GastoCorregidoINS;
	bool RegimenCorregido;
	double *RegimenCorregidoINS;
	bool RelacionExpansion;
	double *RelacionExpansionINS;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResInstantVenturi {
	bool PresionEntrada;
	double PresionEntradaINS;
	bool PresionGarganta;
	double PresionGargantaINS;
	bool MachEntrada;
	double MachEntradaINS;
	bool MachGarganta;
	double MachGargantaINS;
	bool VelEntrada;
	double VelEntradaINS;
	bool VelLateral;
	double VelLateralINS;
	bool GastoEntrada;
	double GastoEntradaINS;
	bool GastoLateral;
	double GastoLateralINS;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResMediosVenturi {
	double Tiempo0;
	double TiempoSUM;
	bool PresionEntrada;
	double PresionEntradaSUM;
	double PresionEntradaMED;
	bool PresionGarganta;
	double PresionGargantaSUM;
	double PresionGargantaMED;
	bool MachEntrada;
	double MachEntradaSUM;
	double MachEntradaMED;
	bool MachGarganta;
	double MachGargantaSUM;
	double MachGargantaMED;
	bool VelEntrada;
	double VelEntradaSUM;
	double VelEntradaMED;
	bool VelLateral;
	double VelLateralSUM;
	double VelLateralMED;
	bool GastoEntrada;
	double GastoEntradaSUM;
	double GastoEntradaMED;
	bool GastoLateral;
	double GastoLateralSUM;
	double GastoLateralMED;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResMediosCV {
	bool Potencia;
	bool Massflow;
	bool Pressure;
	double PotenciaMED;
	double GastoMED;
	double PresionMED;
	double PotenciaSUM;
	double GastoSUM;
	double PresionSUM;
	double Tiempo0;
	double TiempoSUM;

};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResInstantCV {
	bool Potencia;
	bool Massflow;
	bool Pressure;
	double PotenciaINS;
	double GastoINS;
	double PresionINS;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResInstantUED {
	bool Massflow;
	double GastoINS;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResMediosUED {
	bool Massflow;
	double GastoMED;
	double GastoSUM;
	double Tiempo0;
	double TiempoSUM;

};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResMediosEje {
	double RegimenSUM;
	double TiempoSUM;
	double Tiempo0;
	bool Regimen;
	double RegimenMED;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResInstantEje {
	bool Regimen;
	double RegimenINS;
	bool MechPower;
	double MechPowerINS;
	bool MechEff;
	double MechEffINS;
	bool NodeTemp;
	bool HeatFlow;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResMediosCtrl {
	bool Output;
	double OutputMED;
	double OutputSUM;
	bool Error;
	double ErrorMED;
	double ErrorSUM;
	bool POutput;
	double POutputMED;
	double POutputSUM;
	bool IOutput;
	double IOutputMED;
	double IOutputSUM;
	bool DOutput;
	double DOutputMED;
	double DOutputSUM;
	bool Output_filt;
	double Output_filtMED;
	double Output_filtSUM;
	double Tiempo0;
	double TiempoSUM;
};

struct stResInstantCtrl {
	bool Output;
	double OutputINS;
	bool Error;
	double ErrorINS;
	bool POutput;
	double POutputINS;
	bool IOutput;
	double IOutputINS;
	bool DOutput;
	double DOutputINS;
	bool Output_filt;
	double Output_filtINS;
};

struct stResMediosSensor {
	bool Input;
	double InputMED;
	double InputSUM;
	bool Output;
	double OutputMED;
	double OutputSUM;
	double Tiempo0;
	double TiempoSUM;
};

struct stResInstantSensor {
	bool Input;
	double InputINS;
	bool Output;
	double OutputINS;
};
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stTuboExtremo {
	TTubo *Pipe; // Pipe asociado al nodo
	nmPipeEnd TipoExtremo; // PipeEnd del tubo: derecho o izquierdo
	double Entropia;
	double Landa;
	double Beta;
	TDPF *DPF;
	int NumeroHaz;
	int TipoCanal;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stModeloCalculoTubos {
	nmModelo Modelo;
	nmSubModelo SubModelo;
	nmOpcionSubModelo OpcionSubModelo;
	nmTipoFCT Difusion;
	nmTipoFCT Antidifusion;
	nmFormulacionLeyes FormulacionLeyes;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResMediosTubo {
	double Distancia;
	bool TemperaturaGas;
	double TemperaturaGasMED;
	double TemperaturaGasSUM;
	bool Pressure;
	double PresionMED;
	double PresionSUM;
	bool Velocity;
	double VelocidadMED;
	double VelocidadSUM;
	bool Massflow;
	double GastoMED;
	double GastoSUM;
	bool TemperaturaInternaPared;
	double TemperaturaInternaParedMED;
	double TemperaturaInternaParedSUM;
	bool TemperaturaIntermediaPared;
	double TemperaturaIntermediaParedMED;
	double TemperaturaIntermediaParedSUM;
	bool TemperaturaExternaPared;
	double TemperaturaExternaParedMED;
	double TemperaturaExternaParedSUM;
	bool NITmedio;
	double NITmedioMED;
	double NITmedioSUM;
	bool CoefPelInterior;
	double CoefPelInteriorMED;
	double CoefPelInteriorSUM;
	bool FraccionMasicaEspecies;
	double *FraccionSUM;
	double *FraccionMED;
	double PonderacionSUM;
	double GastoPonderacionSUM;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResInstantTubo {
	double Distancia;
	bool Pressure;
	double PresionINS;
	bool Velocity;
	double VelocidadINS;
	bool TemperaturaGas;
	double TemperaturaGasINS;
	bool FlujoMasico;
	double FlujoMasicoINS;
	bool VelocidadDerecha;
	double VelocidadDerechaINS;
	bool VelocidadIzquierda;
	double VelocidadIzquierdaINS;
	bool PresionDerecha;
	double PresionDerechaINS;
	bool PresionIzquierda;
	double PresionIzquierdaINS;
	bool NIT;
	double NITINS;
	bool TemperaturaInternaPared;
	double TemperaturaInternaParedINS;
	bool TemperaturaIntermediaPared;
	double TemperaturaIntermediaParedINS;
	bool TemperaturaExternaPared;
	double TemperaturaExternaParedINS;
	bool CoefPelInterior;
	double CoefPelInteriorINS;
	bool FraccionMasicaEspecies;
	double *FraccionINS;
	bool Gamma;
	double GammaINS;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stSensoresTubo {
	double Distancia;
	bool Pressure;
	double GanaPresion;
	double CtePresion;
	double ValorPresion;
	double Valor0Presion;
	double SensPresion;
	bool Velocity;
	double GanaVelocidad;
	double CteVelocidad;
	double ValorVelocidad;
	double Valor0Velocidad;
	double SensVelocidad;
	bool Temperature;
	double GanaTemperatura;
	double CteTemperatura;
	double Valor0Temperatura;
	double ValorTemperatura;
	double SensTemperatura;
	bool GastoMasico;
	double GanaGastoMasico;
	double CteGastoMasico;
	double Valor0GastoMasico;
	double ValorGastoMasico;
	double SensGastoMasico;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stMediasCompresor {
	bool GraficaMedias;
	double Tiempo0;
	double TiempoSUM;
	bool GraficaTrabajo;
	double Trabajo;
	double TrabajoSUM;
	bool GraficaRelacionCompresion;
	double RelacionCompresion;
	double RelacionCompresionSUM;
	bool GraficaGasto;
	double Massflow;
	double GastoSUM;
	bool GraficaRendimiento;
	double Rendimiento;
	double RendimientoSUM;
	bool GraficaGastoCorregido;
	double GastoCorregido;
	double GastoCorregidoSUM;
	bool GraficaRegimenCorregido;
	double RegimenCorregido;
	double RegimenCorregidoSUM;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stInstantaneosCompresor {
	bool GraficaInstantaneas;
	bool GraficaPotencia;
	double Potencia;
	bool GraficaRendimiento;
	double Rendimiento;
	bool GraficaRelacionCompresion;
	double RelacionCompresion;
	bool GraficaGasto;
	double Massflow;
	bool GraficaGastoCorregido;
	double GastoCorregido;
	bool GraficaRegimenCorregido;
	double RegimenCorregido;
	bool GraficaGamma;
	double Gamma;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stSpline {
	double h;
	double dif;
	double d1;
	double d;
	double b;
	double ud1;
	double ud;
	double yp;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stOrtoPol {
	double s;
	double error;
	double pjm1;
	double pj;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stSpline2Tub {
	double h;
	double dif;
	double d1;
	double d;
	double b;
	double ud1;
	double ud;
	double yp;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stOrtoPol2Tub {
	double s;
	double error;
	double pjm1;
	double pj;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stDistribucion {
	double AA;
	double AE;
	double CA;
	double CE;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stGeometria {
	int NCilin;
	double Carrera;
	double Diametro;
	double Biela;
	double VCC;
	double CDBlowBy;
	double RelaCompresion;
	double AreaPiston;
	double AreaCulata;
	double DiametroBowl;
	double AlturaBowl;
	double DistanciaValvulas;
	double CilindradaUnitaria;
	double CilindradaTotal;
	double DiametroBulon;
	double AlturaCoronaPiston;
	double Excentricidad;
	double MasaBiela;
	double MasaPistonSegmentosBulon;
	double AreaBlowBy;
	double ModuloElasticidad;
	double CoefDeformaciones;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stInjectionSys {
	int HoleNumber;
	double HoleDiame;
	double CDHole;
	double InjectPressure;
	int InjectPCtrID;
	bool InjectPCtrd;
	TController *InjectPCtr;
	double PendOpen_A1;
	double PendOpen_A2;
	double LevMax_B1;
	double LevMax_B2;
	double PendClose_C1;
	double PendClose_C2;
	double Desfase_D1;
	double Desfase_D2;
	int NumPulsos;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stInjecPulse {
	double Angulo;
	double Masa; // Tanto por uno respecto a la masa total
	int CtrAngID;
	bool CtrAngd;
	TController *CtrAng;
	int CtrMasID;
	bool CtrMasd;
	TController *CtrMas;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stPropTermicas {
	double Espesor;
	double Density;
	double CalorEspecifico;
	double Conductividad;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stWiebe {
	double m;
	double C;
	double Alpha0;
	double IncAlpha;
	double Beta;
	double Inicia;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stLeyQuemadoBD {
	double ma;
	double mf;
	double n;
	double dist;
	double ley;
	int nwiebes;

	std::vector<stWiebe> Wiebes;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stPerdMecanicas {
	double Coef0;
	double Coef1;
	double Coef2;
	double Coef3;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stRoadLoad {
	double A0;
	double B0;
	double C0;
	double n;
	double cd;
	double rho;
	double A;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stWoschni {
	double cw1;
	double cw2;
	double cw3;
	double xpe;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResMediosMotor {
	bool ParNeto;
	double ParNetoMED;
	double ParNetoSUM;
	bool ParEfectivo;
	double ParEfectivoMED;
	double ParEfectivoSUM;
	bool ParEfectivoCiclo;
	double ParEfectivoCicloMED;
	bool TrabajoNeto;
	double TrabajoNetoSUM;
	double TrabajoNetoMED;
	bool TrabajoBombeo;
	double TrabajoBombeoSUM;
	double TrabajoBombeoMED;
	bool PMN;
	double PMNMED;
	bool PME;
	double PMEMED;
	bool PMNCiclo;
	double PMNCicloMED;
	bool PMECiclo;
	double PMECicloMED;
	bool PMICiclo;
	double PMICicloMED;
	bool PMBCiclo;
	double PMBCicloMED;
	bool Potencia;
	double PotenciaMED;
	bool PotenciaCiclo;
	double PotenciaCicloMED;
	bool MasaAdmision;
	double MasaAdmisionMED;
	double MasaAdmisionSUM;
	bool MasaFuel;
	double MasaFuelMED;
	double MasaFuelSUM;
	bool MasaAtrapada;
	double MasaAtrapadaMED;
	double FraccionAireFrescoMED;
	bool RegimenGiro;
	double RegimenGiroMED;
	double RegimenGiroSUM;
	bool RendimientoVolumetrico;
	double RendimientoVolumetricoMED;
	bool RendimientoVolumetricoAtm;
	double RendimientoVolumetricoAtmMED;
	bool ParPerdidasMecanicas;
	double ParPerdidasMecanicasMED;
	double ParPerdidasMecanicasSUM;
	bool ParResistente;
	double ParResistenteMED;
	double ParResistenteSUM;
	bool VelocidadVehiculo;
	double VelocidadVehiculoMED;
	double VelocidadVehiculoSUM;
	double DensidadReferenciaMED;
	double DensidadReferenciaSUM;
	double MasaTuboReferenciaSUM;
	double MasaTuboReferenciaMED;
	double GastoTuboReferenciaMED;
	double GastoTuboReferenciaSUM;
	bool Dosado;
	double DosadoMED;
	bool AFR;
	double AFRMED;
	bool Swirl;
	double SwirlMED;
	bool RendEfectivo;
	double RendEfectivoMED;
	bool RendIndicado;
	double RendIndicadoMED;
	bool ConsumoEspecifico;
	double ConsumoEspecificoMED;
	double TiempoSUM;
	double Tiempo0;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stTemperaturasPared {
	double Piston;
	double PistonSUMup;
	double PistonSUMdown;
	double Culata;
	double CulataSUMup;
	double CulataSUMdown;
	double Cylinder;
	double CilindroSUMup;
	double CilindroSUMdown;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stCalor {
	double TransPiston;
	double TransCilindro;
	double TransCulata;
	double TransTotal;
	double Liberado;
	double LiberadoTotal;
	double FQL;
	double FQL0;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResMediosCilindro {
	bool TrabajoNeto;
	double TrabajoNetoMED;
	double TrabajoNetoSUM;
	bool PresionMediaNeta;
	double PresionMediaNetaMED;
	bool TrabajoBombeo;
	double TrabajoBombeoMED;
	double TrabajoBombeoSUM;
	bool PresionMediaBombeo;
	double PresionMediaBombeoMED;
	bool CalorCombustion;
	double CalorCombustionSUM;
	double CalorCombustionMED;
	bool CalorCilindro;
	double CalorCilindroSUM;
	double CalorCilindroMED;
	bool CalorCulata;
	double CalorCulataSUM;
	double CalorCulataMED;
	bool CalorPiston;
	double CalorPistonSUM;
	double CalorPistonMED;
	bool PresionMediaIndicada;
	double PresionMediaIndicadaMED;
	bool MasaAtrapada;
	double MasaAtrapadaMED;
	bool TemperaturaCilindroInterna;
	double TemperaturaCilindroInternaSUM;
	double TemperaturaCilindroInternaMED;
	bool TemperaturaCilindroMedia;
	double TemperaturaCilindroMediaSUM;
	double TemperaturaCilindroMediaMED;
	bool TemperaturaCilindroExterna;
	double TemperaturaCilindroExternaSUM;
	double TemperaturaCilindroExternaMED;
	bool TemperaturaPistonInterna;
	double TemperaturaPistonInternaSUM;
	double TemperaturaPistonInternaMED;
	bool TemperaturaPistonMedia;
	double TemperaturaPistonMediaSUM;
	double TemperaturaPistonMediaMED;
	bool TemperaturaPistonExterna;
	double TemperaturaPistonExternaSUM;
	double TemperaturaPistonExternaMED;
	bool TemperaturaCulataInterna;
	double TemperaturaCulataInternaSUM;
	double TemperaturaCulataInternaMED;
	bool TemperaturaCulataMedia;
	double TemperaturaCulataMediaSUM;
	double TemperaturaCulataMediaMED;
	bool TemperaturaCulataExterna;
	double TemperaturaCulataExternaSUM;
	double TemperaturaCulataExternaMED;
	bool NITMedio;
	double *NITMED;
	double NITMedioSUM;
	double NITMedioMED;
	bool AFRMedio;
	double AFRMedioMED;
	bool MasaBlowBy;
	double MasaBlowBySUM;
	double MasaBlowByMED;
	bool MasaAdmision;
	double MasaAdmisionMED;
	bool MasaEscape;
	double MasaEscapeMED;
	bool TemperaturaMedia;
	double TemperaturaMediaMED;
	double TemperaturaMediaSUM;
	bool Swirl;
	double SwirlMED;
	bool RendVolumetrico;
	double RendVolumetricoMED;
	double DensidadReferenciaSUM;
	bool MasaCortocircuito;
	double MasaCortocircuitoMED;
	double MasaCortocircuitoSUM;
	double Tiempo0;
	double TiempoSUM;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResInstantCilindro {
	bool Pressure;
	double PresionINS;
	bool Temperature;
	double TemperaturaINS;
	bool MomentoAngularEsc;
	double *MomentoAngularEscINS;
	double MomentoAngularTotalEscINS;
	bool MomentoAngularAdm;
	double *MomentoAngularAdmINS;
	double MomentoAngularTotalAdmINS;
	bool GastoEsc;
	double *GastoEscINS;
	double GastoTotalEscINS;
	bool GastoAdm;
	double *GastoAdmINS;
	double GastoTotalAdmINS;
	bool MachEsc;
	double *MachEscINS;
	bool MachAdm;
	double *MachAdmINS;
	bool SeccionEfectivaAdm;
	double *SeccionEfectivaAdmINS;
	double SeccionEfectivaTotalAdmINS;
	bool SeccionEfectivaEsc;
	double *SeccionEfectivaEscINS;
	double SeccionEfectivaTotalEscINS;
	bool Masa;
	double MasaINS;
	bool Volumen;
	double VolumenINS;
	bool CoeficienteWoschni;
	double CoeficienteWoschniINS;
	bool TemperaturaCilindroInterna;
	double TemperaturaCilindroInternaINS;
	bool TemperaturaCilindroMedia;
	double TemperaturaCilindroMediaINS;
	bool TemperaturaCilindroExterna;
	double TemperaturaCilindroExternaINS;
	bool TemperaturaPistonInterna;
	double TemperaturaPistonInternaINS;
	bool TemperaturaPistonMedia;
	double TemperaturaPistonMediaINS;
	bool TemperaturaPistonExterna;
	double TemperaturaPistonExternaINS;
	bool TemperaturaCulataInterna;
	double TemperaturaCulataInternaINS;
	bool TemperaturaCulataMedia;
	double TemperaturaCulataMediaINS;
	bool TemperaturaCulataExterna;
	double TemperaturaCulataExternaINS;
	bool MasaCombustible;
	double MasaCombustibleINS;
	bool FQL;
	double FQLINS;
	bool NIT;
	double *NITINS;
	double NITTotalINS;
	bool ParInstantaneo;
	double ParInstantaneoINS;
	bool GastoCortocircuito;
	double GastoCortocircuitoINS;
	bool GastoBlowBy;
	double GastoBlowByINS;
	bool FraccionMasica;
	double *FraccionINS;
	bool Gamma;
	double GammaINS;
	bool HeatHead;
	double HeatHeadINS;
	bool HeatCyl;
	double HeatCylINS;
	bool HeatPis;
	double HeatPisINS;

};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stValvulasCilindro {
	double GastoSUM;
	double Massflow;
	double GastoSeccionSUM;
	double GastoSeccion;
	double MachSUM;
	double Mach;
	double RelacionPresionGargantaSUM;
	double RelacionPresionGarganta;
	double NIT;
	double NITSUM;
	double NITMED;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stCapa { // Capas de los tubos
	bool EsPrincipal;
	bool EsFluida;
	double Density;
	double CalorEspecifico;
	double Conductividad;
	double Espesor;
	double EmisividadInterior;
	double EmisividadExterior;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

struct stResMediosDPF {
	double Distancia;
	bool VelocidadParedCanalEntrada;
	double VelocidadParedCanalEntradaSUM;
	double VelocidadParedCanalEntradaMED;
	bool VelocidadParedCanalSalida;
	double VelocidadParedCanalSalidaSUM;
	double VelocidadParedCanalSalidaMED;
	bool MasaSoot;
	double MasaSootSUM;
	double MasaSootMED;
	bool EspesorSoot;
	double EspesorSootSUM;
	double EspesorSootMED;
	bool TemperaturaParedCS;
	double TemperaturaParedCSSUM;
	double TemperaturaParedCSMED;
	bool TemperaturaIntermediaPared;
	double TemperaturaIntermediaParedSUM;
	double TemperaturaIntermediaParedMED;
	bool TemperaturaParedCE;
	double TemperaturaParedCESUM;
	double TemperaturaParedCEMED;
	bool Kwall;
	double KwallSUM;
	double KwallMED;
	bool Ksoot;
	double KsootSUM;
	double KsootMED;
	bool Eficiencia;
	double EficienciaSUM;
	double EficienciaMED;
	bool Porosidad;
	double PorosidadSUM;
	double PorosidadMED;
	bool CoeficienteParticion;
	double CoeficienteParticionSUM;
	double CoeficienteParticionMED;
	bool DiametroUC;
	double DiametroUCSUM;
	double DiametroUCMED;
	bool Kreg1;
	double Kreg1SUM;
	double Kreg1MED;
	bool Kreg2;
	double Kreg2SUM;
	double Kreg2MED;
	bool Qreg;
	double QregSUM;
	double QregMED;
	bool Q1;
	double Q1SUM;
	double Q1MED;
	bool Q2;
	double Q2SUM;
	double Q2MED;
	bool TasaFraccionMasicaEspecies;
	double *TasaFraccionSUM;
	double *TasaFraccionMED;
	bool FraccionMasicaEspeciesSalida;
	double *FraccionSalidaSUM;
	double *FraccionSalidaMED;
	double PonderacionSUM;
	double GastoPonderacionSUM;
	bool TemperaturaExternaSuperficie;
	double TemperaturaExternaSuperficieSUM;
	double TemperaturaExternaSuperficieMED;
	bool TemperaturaMediaSuperficie;
	double TemperaturaMediaSuperficieSUM;
	double TemperaturaMediaSuperficieMED;
	bool TemperaturaInternaSuperficie;
	double TemperaturaInternaSuperficieSUM;
	double TemperaturaInternaSuperficieMED;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

struct stResInstantDPF {
	double Distancia;
	bool VelocidadParedCanalEntrada;
	double VelocidadParedCanalEntradaINS;
	bool VelocidadParedCanalSalida;
	double VelocidadParedCanalSalidaINS;
	bool MasaSoot;
	double MasaSootINS;
	bool EspesorSoot;
	double EspesorSootINS;
	bool TemperaturaParedCS;
	double TemperaturaParedCSINS;
	bool TemperaturaIntermediaPared;
	double TemperaturaIntermediaParedINS;
	bool TemperaturaParedCE;
	double TemperaturaParedCEINS;
	bool Kwall;
	double KwallINS;
	bool Ksoot;
	double KsootINS;
	bool Eficiencia;
	double EficienciaINS;
	bool Porosidad;
	double PorosidadINS;
	bool CoeficienteParticion;
	double CoeficienteParticionINS;
	bool DiametroUC;
	double DiametroUCINS;
	bool Kreg1;
	double Kreg1INS;
	bool Kreg2;
	double Kreg2INS;
	bool Qreg;
	double QregINS;
	bool Q1;
	double Q1INS;
	bool Q2;
	double Q2INS;
	bool TasaFraccionMasicaEspecies;
	double *TasaFraccionINS;
	bool FraccionMasicaEspeciesSalida;
	double *FraccionSalidaINS;
	bool TemperaturaExternaSuperficie;
	double TemperaturaExternaSuperficieINS;
	bool TemperaturaMediaSuperficie;
	double TemperaturaMediaSuperficieINS;
	bool TemperaturaInternaSuperficie;
	double TemperaturaInternaSuperficieINS;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// FUNCIONES

template<class T>
void DestruyeVector(T * pointer) {
	if(pointer != NULL) {
		delete[] pointer;
		pointer = NULL;
	}
}

template<class T>
void DestruyeMatriz(T * *pointer, int num) {
	if(pointer != NULL) {
		for(int i = 0; i < num; i++) {
			delete[] pointer[i];
		}
		delete[] pointer;
		pointer = NULL;
	}
}

/**
 * @brief Heat capacities ratio of air.
 *
 * Computes the heat capacities ratio of air, with some burnt fraction.
 *
 * @f[ \gamma = 1 + \cfrac{R}{c_v} @f]
 *
 * If @f$ \gamma @f$ is not a function of temperature, returns 1.4.
 *
 * @param RMezcla Gas constant. [J / (kg / K)]
 * @param CvMezcla Specific heat capacity at constant volume. [J / (kg / K)]
 * @param GammaCalculation Whether gamma is a function of the temperature
 * or not.
 * @return Heat capacities ratio.
 */
inline double CalculoSimpleGamma(double RMezcla, double CvMezcla, nmCalculoGamma GammaCalculation) {
	double g = __Gamma::G;

	if(GammaCalculation != nmGammaConstante) {
		g = 1. + RMezcla / CvMezcla;
	}

	return g;
}
;

/**
 * @brief Specific heat capacity at constant volume of air, simplified
 * version.
 *
 * Computes the specific heat capacity at constant volume of air, with
 * some burnt fraction.
 *
 * @f[
 * c_v = c_{v_{bf}} \cdot Y_{bf}
 * + c_{v_{da}} \cdot \left( 1 - Y_{wv} - Y \right)
 * + c_{v_{wv}} \cdot  Y_{wv}
 * @f]
 *
 * where @f$ c_v @f$ is the specific heat capacity at constant volume
 * and @f$ Y @f$ is the mass fraction.  If @f$ \gamma @f$ is not a function
 * of temperature, returns 717.5
 *
 * @param Temperature Air temperature. [K]
 * @param YQuemados Burnt gases mass fraction. [-]
 * @param GammaCalculation Whether gamma is a function of the temperature
 * or not.
 * @return Specific heat capacity at constant volume. [J / (kg * K)]
 */
inline double CalculoSimpleCvMezcla(double Temperature, double YQuemados, double YCombustible,
									nmCalculoGamma GammaCalculation, nmTipoCombustion TipoCombustible) {
	if(TipoCombustible == 0) {
		TipoCombustible == nmMEC;
	}
	double CvMezcla = __Gamma::Cv;
	if(GammaCalculation != nmGammaConstante) {
		double CvAire = 714.68;
		double CvQuemados = 759.67;
		double CvCombustible = 1496.92;
		double CvH2O = 1420.63;
		if(GammaCalculation == nmComposicionTemperatura) {
			double RaizdeT = sqrt(Temperature);

			CvAire = -10.4199 * RaizdeT + 2522.88 + (-67227.1 * RaizdeT + 917124.4 - 4174853.6 / RaizdeT) / Temperature;
			CvQuemados = 641.154 + Temperature * (0.43045 + Temperature * (-0.0001125 + Temperature * 8.979e-9));
			CvH2O = (22.605 - 0.09067 * RaizdeT + (-826.53 * RaizdeT + 13970.1 - 82114 / RaizdeT) / Temperature) * __R::H2O -
					__R::H2O;
			if(TipoCombustible == nmMEC) {
				//Diesel C10.8H18.7
				CvCombustible = -256.4 + Temperature * (6.95372 + Temperature * (-0.00404715 + Temperature * 0.000000910259)) +
								1458487 / (Temperature * Temperature);
			} else if(TipoCombustible == nmMEP) {
				// Octano from NIST data base (1 bar).
				CvCombustible = ((4.200E-07 * Temperature - 2.524E-03) * Temperature + 5.834E+00) * Temperature + 1.890E+02 -
								__R::Gasoline;
			}
		}
		//CvMezcla = CvQuemados * YQuemados + CvCombustible * YCombustible + (CvAire * (1 - YCombustible - YQuemados - 0.0164) + 0.0164 * CvH2O);
		//Sin Humedad en aire
		CvMezcla = CvQuemados * YQuemados + CvCombustible * YCombustible + (CvAire * (1 - YCombustible - YQuemados));
	}
	return CvMezcla;
}
;

inline double CalculoSimpleRMezcla(double YQuemados, double YCombustible, nmCalculoGamma GammaCalculation,
								   nmTipoCombustion TipoCombustible) {
	double R = 287;
	double RFuel = 0;
	if(TipoCombustible == nmMEP) {
		RFuel = __R::Gasoline;
	} else {
		RFuel = __R::Diesel;
	}
	if(GammaCalculation != nmGammaConstante) {
		//R = __R::Burnt * YQuemados + RFuel * YCombustible + (__R::Air * (1 - YQuemados - YCombustible - 0.0164) + 0.0164 * __R::H2O);
		//Sin humedad en aire
		R = __R::Burnt * YQuemados + RFuel * YCombustible + (__R::Air * (1 - YQuemados - YCombustible));
	}
	return R;
}
;

inline double CalculoCompletoGamma(double RMezcla, double CpMezcla, nmCalculoGamma GammaCalculation) {
	double Gamma = 1.4;

	if(GammaCalculation != nmGammaConstante) {
		Gamma = CpMezcla / (CpMezcla - RMezcla);
	}
	return Gamma;
}
;

inline double CalculoCompletoCpMezcla(double YO2, double YCO2, double YH2O, double YCombustible, double Temperature,
									  nmCalculoGamma GammaCalculation, nmTipoCombustion TipoCombustible) {
	double YN2 = 1 - YO2 - YCO2 - YH2O;
	double CpMezcla = 1004.5;
	if(TipoCombustible == 0) {
		TipoCombustible == nmMEC;
	}
	if(GammaCalculation != nmGammaConstante) {
		double CpN2 = 1039.82;
		double CpO2 = 912.54;
		double CpCO2 = 843.13;
		double CpH2O = 1856.93;
		double CpCombustible = 0;
		if(TipoCombustible == nmMEC) {
			CpCombustible = __R::Diesel + 1496.92;
		} else {
			CpCombustible = __R::Gasoline + 1496.92;
		}

		if(GammaCalculation == nmComposicionTemperatura) {
			double RaizdeT = sqrt(Temperature);
			// Temperature en Kelvin. Calculado segun la correlacion de JANAF.
			CpN2 = (12.531 - 0.05932 * RaizdeT + (-352.3 * RaizdeT + 5279.1 - 27358 / RaizdeT) / Temperature) * __R::N2;
			CpO2 = (-0.112 + 0.0479 * RaizdeT + (195.42 * RaizdeT - 4426.1 + 32538 / RaizdeT) / Temperature) * __R::O2;
			CpCO2 = (12.019 - 0.03566 * RaizdeT + (-142.34 * RaizdeT - 163.7 + 9470 / RaizdeT) / Temperature) * __R::CO2;
			CpH2O = (22.605 - 0.09067 * RaizdeT + (-826.53 * RaizdeT + 13970.1 - 82114 / RaizdeT) / Temperature) * __R::H2O;
			if(TipoCombustible == nmMEC) {
				//Diesel C10.8H18.7
				CpCombustible = __R::Diesel + (-256.4 + Temperature * (6.95372 + Temperature * (-0.00404715 + Temperature *
											   0.000000910259)) + 1458487 / (Temperature * Temperature)); //Cp = R + Cv
			} else if(TipoCombustible == nmMEP) {
				// Octano from NIST data base (1 bar).
//				CpCombustible = 4184
//					* (-24.078 + Temperature * (0.25663 - Temperature * (0.00020168 + 0.00000006475 * Temperature))
//						+ 580800 * RaizdeT) * __R::Gasoline / __R::Universal; //cv = cp - R
				CpCombustible = ((4.200E-07 * Temperature - 2.524E-03) * Temperature + 5.834E+00) * Temperature + 1.890E+02;

			}

		}
		CpMezcla = CpO2 * YO2 + CpCO2 * YCO2 + CpH2O * YH2O + CpN2 * (YN2 - 0.01292) + 520.32 * 0.01292 + CpCombustible *
				   YCombustible;
	}

	return CpMezcla;
}
;

inline double CalculoCompletoRMezcla(double YO2, double YCO2, double YH2O, double YCombustible,
									 nmCalculoGamma GammaCalculation, nmTipoCombustion TipoCombustible) {
	double R = 287;
	double RFuel = 0;
	if(TipoCombustible == nmMEP) {
		RFuel = __R::Gasoline;
	} else {
		RFuel = __R::Diesel;
	}
	if(GammaCalculation != nmGammaConstante) {
		R = __R::O2 * YO2 + __R::CO2 * YCO2 + __R::H2O * YH2O + RFuel * YCombustible + __R::N2 *
			(1 - YO2 - YCO2 - YH2O - YCombustible - 0.012) + 208.13 * 0.012; // El ultimo t�rmino es el Arg�n
	}
	return R;
}
;

inline double CalculoUfgasoil(double Temperature) {
	double Ufgasoil = 0.;
	Ufgasoil = -1234157.8 - 256.4 * __units::degCToK(Temperature) + 3.47686 * pow(__units::degCToK(Temperature),
			   2) - 0.00134905 * pow(__units::degCToK(Temperature), 3) + 0.000000227565 * pow(
				   __units::degCToK(Temperature), 4) - 1458487. / __units::degCToK(Temperature);
	return Ufgasoil;
}
;

inline double Interp1(double x, double *x1, double *x2, int n) {
	double ret_val, delta;
	int pos;

	if(x < x1[0]) {
		ret_val = x2[0];
	} else if(x > x1[n - 1]) {
		ret_val = x2[n - 1];
	} else {
		pos = 0;
		while(x > x1[pos + 1]) {
			pos++;
		}
		delta = (x - x1[pos]) / (x1[pos + 1] - x1[pos]);
		if(delta > 1 || delta < 0)
			std::cout << "ERROR: Interpolacion Interp1" << std::endl;
		ret_val = (1 - delta) * x2[pos] + delta * x2[pos + 1];
	}
	return ret_val;
}
;

inline double Interp1(double x, std::vector<double> x1, std::vector<double> x2, int n) {
	double ret_val, delta;
	int pos;

	if(x < x1[0]) {
		ret_val = x2[0];
	} else if(x > x1[n - 1]) {
		ret_val = x2[n - 1];
	} else {
		pos = 0;
		while(x > x1[pos + 1]) {
			pos++;
		}
		delta = (x - x1[pos]) / (x1[pos + 1] - x1[pos]);
		if(delta > 1 || delta < 0)
			std::cout << "ERROR: Interpolacion Interp1" << std::endl;
		ret_val = (1 - delta) * x2[pos] + delta * x2[pos + 1];
	}
	return ret_val;
}
;

inline double StepFunction(double x, std::vector<double> x1, std::vector<double> x2, int n) {
	double ret_val;
	int pos;

	if(x < x1[0]) {
		ret_val = x2[0];
	} else if(x > x1[n - 1]) {
		ret_val = x2[n - 1];
	} else {
		pos = 0;
		while(x > x1[pos + 1]) {
			pos++;
		}
		ret_val = x2[pos];
	}
	return ret_val;
}
;

inline double Interpolacion_bidimensional(double xref, double yref, double *Mapa_fila, double *Mapa_col, double **Mapa,
		int dimension_x, int dimension_y) {
	int y1, y2, x1, x2;
	double Valor_mapa_fila_yref_columna_x1, Valor_mapa_fila_yref_columna_x2, Valor_mapa_fila_yref_columna_xref;
	int contador_col = 0, contador_fila = 0;

	if(yref < Mapa_fila[0]) {

		if(xref < Mapa_col[0]) {
			Valor_mapa_fila_yref_columna_xref = Mapa[0][0];
		} else if(xref >= Mapa_col[dimension_x - 1]) {
			Valor_mapa_fila_yref_columna_xref = Mapa[0][dimension_x - 1];
		} else {
			while(Mapa_col[contador_col] <= xref) {
				x1 = contador_col;
				x2 = contador_col + 1;
				contador_col++;
			}
			Valor_mapa_fila_yref_columna_xref = Mapa[0][x1] + (Mapa[0][x1] - Mapa[0][x2]) * (Mapa_col[x1] - xref) /
												(Mapa_col[x1] - Mapa_col[x2]);
		}

	} else if(yref >= Mapa_fila[dimension_y - 1]) {

		if(xref < Mapa_col[0]) {
			Valor_mapa_fila_yref_columna_xref = Mapa[dimension_y - 1][0];
		} else if(xref >= Mapa_col[dimension_x - 1]) {
			Valor_mapa_fila_yref_columna_xref = Mapa[dimension_y - 1][dimension_x - 1];
		} else {
			while(Mapa_col[contador_col] <= xref) {
				x1 = contador_col;
				x2 = contador_col + 1;
				contador_col++;
			}
			Valor_mapa_fila_yref_columna_xref = Mapa[dimension_y - 1][x1] - (Mapa[dimension_y - 1][x1] - Mapa[dimension_y - 1][x2])
												* (Mapa_col[x1] - xref) / (Mapa_col[x1] - Mapa_col[x2]);
		}

	} else {

		while(Mapa_fila[contador_fila] <= yref) {
			y1 = contador_fila;
			y2 = contador_fila + 1;
			contador_fila++;
		}
		if(xref < Mapa_col[0]) {
			Valor_mapa_fila_yref_columna_xref = Mapa[y1][0] - (Mapa[y1][0] - Mapa[y2][0]) * (Mapa_fila[y1] - yref) /
												(Mapa_fila[y1] - Mapa_fila[y2]);
		} else if(xref >= Mapa_col[dimension_x - 1]) {
			Valor_mapa_fila_yref_columna_xref = Mapa[y1][dimension_x - 1] - (Mapa[y1][dimension_x - 1] - Mapa[y2][dimension_x - 1])
												* (Mapa_fila[y1] - yref) / (Mapa_fila[y1] - Mapa_fila[y2]);
		} else {
			while(Mapa_col[contador_col] <= xref) {
				x1 = contador_col;
				x2 = contador_col + 1;
				contador_col++;
			}
			/* Se interpolan los valores entre las dos columnas */
			Valor_mapa_fila_yref_columna_x1 = Mapa[y1][x1] - (Mapa[y1][x1] - Mapa[y2][x1]) * (Mapa_fila[y1] - yref) /
											  (Mapa_fila[y1] - Mapa_fila[y2]);
			Valor_mapa_fila_yref_columna_x2 = Mapa[y1][x2] - (Mapa[y1][x2] - Mapa[y2][x2]) * (Mapa_fila[y1] - yref) /
											  (Mapa_fila[y1] - Mapa_fila[y2]);

			/* A continuacion se interpola el valor entre las filas del mapa, para obtener el (valor en xref,yref) */
			Valor_mapa_fila_yref_columna_xref = Valor_mapa_fila_yref_columna_x1 - (Valor_mapa_fila_yref_columna_x1 -
												Valor_mapa_fila_yref_columna_x2) * (Mapa_col[x1] - xref) / (Mapa_col[x1] - Mapa_col[x2]);
		}

	}

	return Valor_mapa_fila_yref_columna_xref;
}
;

inline void Hermite(int n, double *x, double *y, double *sol) {
	double DeltaK, AlphaK, BetaK, TauK;

	for(int i = 1; i < n - 1; ++i) {
		sol[i] = (y[i] - y[i - 1]) / 2. / (x[i] - x[i - 1]) + (y[i + 1] - y[i]) / 2. / (x[i + 1] - x[i]);
	}
	sol[0] = (y[1] - y[0]) / (x[1] - x[0]);
	sol[n - 1] = (y[n - 1] - y[n - 2]) / (x[n - 1] - x[n - 2]);

	for(int i = 0; i < n - 1; i++) {
		DeltaK = (y[i + 1] - y[i]) / (x[i + 1] - x[i]);
		if(DeltaK == 0) {
			sol[i] = 0;
			sol[i + 1] = 0;
		} else {
			AlphaK = sol[i] / DeltaK;
			BetaK = sol[i + 1] / DeltaK;
			if(BetaK * BetaK + AlphaK * AlphaK > 9) {
				TauK = 3 / sqrt(BetaK * BetaK + AlphaK * AlphaK);
				sol[i] = TauK * AlphaK * DeltaK;
				sol[i + 1] = TauK * BetaK * DeltaK;
			}
		}
	}
}
;

inline void Hermite(int n, std::vector<double> x, std::vector<double> y, std::vector<double> *sol) {
	double DeltaK, AlphaK, BetaK, TauK;

	for(int i = 1; i < n - 1; ++i) {
		(*sol)[i] = (y[i] - y[i - 1]) / 2. / (x[i] - x[i - 1]) + (y[i + 1] - y[i]) / 2. / (x[i + 1] - x[i]);
	}
	(*sol)[0] = (y[1] - y[0]) / (x[1] - x[0]);
	(*sol)[n - 1] = (y[n - 1] - y[n - 2]) / (x[n - 1] - x[n - 2]);

	for(int i = 0; i < n - 1; i++) {
		DeltaK = (y[i + 1] - y[i]) / (x[i + 1] - x[i]);
		if(DeltaK == 0) {
			(*sol)[i] = 0;
			(*sol)[i + 1] = 0;
		} else {
			AlphaK = (*sol)[i] / DeltaK;
			BetaK = (*sol)[i + 1] / DeltaK;
			if(BetaK * BetaK + AlphaK * AlphaK > 9) {
				TauK = 3 / sqrt(BetaK * BetaK + AlphaK * AlphaK);
				(*sol)[i] = TauK * AlphaK * DeltaK;
				(*sol)[i + 1] = TauK * BetaK * DeltaK;
			}
		}
	}
}
;

inline double EvaluaHermite(double punto, int n, double *x, double *y, double *sol) {
	double ret_val, h00, h10, h01, h11, t2, t3, t, h;
	int k = 0;

	if(punto <= x[0]) {
		ret_val = y[0];
	} else if(punto >= x[n - 1]) {
		ret_val = y[n - 1];
	} else {
		while(x[k] < punto && k < n - 1) {
			++k;
		}
		h = (x[k] - x[k - 1]);
		t = (punto - x[k - 1]) / h;
		t2 = t * t;
		t3 = t2 * t;
		h00 = 2 * t3 - 3 * t2 + 1;
		h10 = t3 - 2 * t2 + t;
		h01 = -2 * t3 + 3 * t2;
		h11 = t3 - t2;
		ret_val = h00 * y[k - 1] + h * h10 * sol[k - 1] + h01 * y[k] + h * h11 * sol[k];
	}

	return ret_val;
}
;

inline double EvaluaHermite(double punto, int n, std::vector<double> x, std::vector<double> y,
							std::vector<double> sol) {
	double ret_val, h00, h10, h01, h11, t2, t3, t, h;
	int k = 0;

	if(punto <= x[0]) {
		ret_val = y[0];
	} else if(punto >= x[n - 1]) {
		ret_val = y[n - 1];
	} else {
		while(x[k] < punto && k < n - 1) {
			++k;
		}
		h = (x[k] - x[k - 1]);
		t = (punto - x[k - 1]) / h;
		t2 = t * t;
		t3 = t2 * t;
		h00 = 2 * t3 - 3 * t2 + 1;
		h10 = t3 - 2 * t2 + t;
		h01 = -2 * t3 + 3 * t2;
		h11 = t3 - t2;
		ret_val = h00 * y[k - 1] + h * h10 * sol[k - 1] + h01 * y[k] + h * h11 * sol[k];
	}

	return ret_val;
}
;

inline void GetName(char *origin, char *destination, const char *add) {

	int contpunto = (int) strlen(origin);
	for(int i = (int) strlen(origin) - 1; i >= 0; i--) {
		if(origin[i] == '.') {
			contpunto = i;
			break;
		} // ha encontrado el punto de la extension
	}
	for(int i = 0; i < contpunto; i++) {
		destination[i] = origin[i];
	}
	destination[contpunto] = '\0';

	strcat(destination, add);
}
;

inline void ReduceSubsonicFlow(double& a, double& v, double g) {
	double Machx = v / a;
	double g3 = (g - 1) / 2;
	double Machy = Machx / fabs(Machx) * sqrt((Machx * Machx + 1 / g3) / (g / g3 * pow(Machx, 2) - 1.));
	a = a * sqrt((g3 * Machx * Machx + 1.) / (g3 * Machy * Machy + 1.));

	v = a * Machy;
}
;

// int _matherr(struct _exception *a) {
//	// throw Exception(std::to_string(a->name)+"("+std::to_string(a->arg1)+","+std::to_string(a->arg2)+")");
//
//	if (a->arg2 != 0) {
//		MessageBeep(2);
//	}
//	return 0;
// }

#ifndef __BORLANDC__
class TObject {
};
#endif

#endif

