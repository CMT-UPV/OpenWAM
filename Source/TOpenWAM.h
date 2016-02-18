/*--------------------------------------------------------------------------------*\
|==========================|
 |\\   /\ /\   // O pen     | OpenWAM: The Open Source 1D Gas-Dynamic Code
 | \\ |  X  | //  W ave     |
 |  \\ \/_\/ //   A ction   | CMT-Motores Termicos / Universidad Politecnica Valencia
 |   \\/   \//    M odel    |
 ----------------------------------------------------------------------------------
 | License
 |
 |	This file is part of OpenWAM.
 |
 |	OpenWAM is free software: you can redistribute it and/or modify
 |	it under the terms of the GNU General Public License as published by
 |	the Free Software Foundation, either version 3 of the License, or
 |	(at your option) any later version.
 |
 |	OpenWAM is distributed in the hope that it will be useful,
 |	but WITHOUT ANY WARRANTY; without even the implied warranty of
 |	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 |	GNU General Public License for more details.
 |
 |	You should have received a copy of the GNU General Public License
 |	along with OpenWAM.  If not, see <http://www.gnu.org/licenses/>.
 |
 \*--------------------------------------------------------------------------------*/

// ---------------------------------------------------------------------------
#ifndef TOpenWAMH
#define TOpenWAMH

#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <regex>
#include <fstream>
#pragma hdrstop
#include "Globales.h"

#include "TTimeControl.h"

// ENGINE BLOCK AND CYLINDERS
#include "TBloqueMotor.h"
#include "TCilindro4T.h"

// COMPRESSOR
#include "TCompresorDep.h"
#include "TCompTubDep.h"
#include "TCompTubos.h"

// EXTERNAL CALCULATIONS
#include "TCalculoExtern.h"
#include "TRemansoMatlab.h"
#include "TCoefDescarga.h"
#include "TControlFuel.h"

// VALVES
#include "TCDExterno.h"
#include "TEstatorTurbina.h"
#include "TRotorTurbina.h"
#include "TWasteGate.h"
#include "TValvulaContr.h"
#include "TDiscoRotativo.h"
#include "TLumbrera.h"
#include "TCDFijo.h"
#include "TValvula4T.h"
#include "TLamina.h"
#include "TMariposa.h"

// PIPES
#include "TTubo.h"

// PLENUMS
#include "TDepVolVariable.h"
#include "TDepVolCte.h"
#include "TTurbinaSimple.h"
#include "TTurbinaTwin.h"
#include "TVenturi.h"
#include "TUnionDireccional.h"

// BOUNDARY CONDITIONS
#include "TCCDescargaExtremoAbierto.h"
#include "TCCExtremoAnecoico.h"
#include "TCCExtremoCerrado.h"
#include "TCCPulso.h"
#include "TCCCilindro.h"
#include "TCCUnionEntreTubos.h"
#include "TCCPerdidadePresion.h"
#include "TCCDeposito.h"
#include "TCCRamificacion.h"
#include "TCCExtremoInyeccion.h"
#include "TCCEntradaCompresor.h"
#include "TCCUnionEntreDepositos.h"
#include "TCCCompresorVolumetrico.h"
#include "TCCCompresor.h"
#include "TCCPreVble.h"
#include "TCFDConnection.h"
#include "TCCExternalConnection.h"
#include "TCCExternalConnectionVol.h"

// TURBOCHARGER AXIS
#include "TEjeTurbogrupo.h"

// DIESEL PARTICULATE FILTER
#ifdef ParticulateFilter
#include "TDPF.h"
#include "TCanalDPF.h"
#endif

// CONCENTRIC 1D ELEMENTS
#ifdef ConcentricElement
#include "TConcentricoTubos.h"
#include "TConcentricoDPF.h"
#endif

// CONTROL DEVICES
#include "TSensor.h"
#include "TPIDController.h"
#include "TTable1D.h"
#include "TDecisor.h"
#include "TGain.h"

// OUTPUT RESULTS
#include "TOutputResults.h"
#define completo 1

/* ! \def gestorcom
 Allow the communication with WAMer
 */

#include <sys/timeb.h>

#ifdef __BORLANDC__
#define gestorcom true
#define graphicalout true
#else
//#define gestorcom 0
//#define graphicalout 0
#endif

#ifdef gestorcom
#include "TCGestorWAM.h"
#endif

class TOpenWAM {
  private:

#ifdef gestorcom

	TCGestorWAM *GestorWAM;
#endif

	std::string tzstr;
	struct timeb begining, final, current;

	stRun Run;

	stDatosTGV *DatosTGV;
	std::string fileinput;

	FILE *FileInput;
	// !< Pointers to input and output files.
	FILE *fc; // !< Pointers to input and output files.

	//char fileinput[8];

	TBloqueMotor** Engine;
	TCompresor** Compressor;
	TCalculoExtern* EXTERN;
	TEjeTurbogrupo** Axis;

	// ! ARRAY OF TYPES OF VALVES
	TTipoValvula** TypeOfValve;

	// ! POINTERS ARRAY TO VALVES TYPE TURBINE STATOR
	TEstatorTurbina*** StatorTurbine;
	// ! POINTERS ARRAY TO VALVES TYPE TURBINE ROTOR
	TRotorTurbina** RotorTurbine;
	// ! POINTERS ARRAY TO EXTERNAL CONNECTIONS
	TTipoValvula** CCCalcExtern;
	TTipoValvula** BCButerflyValve;

	// ! ARRAY OF PIPES
	TTubo** Pipe;

	// ! ARRAY OF CONCENTRIC ELEMENTS
#ifdef ConcentricElement
	TConcentrico** Concentric;
#endif

	// ! ARRAY OF DPFs
#ifdef ParticulateFilter
	TDPF** DPF;
#endif

	// ! ARRAYS OF PLENUMS
	TDeposito** Plenum;
	TTurbina** Turbine;
	TVenturi** Venturi;

	// ! ARRAYS OF BOUNDARY CONDITIONS
	TCondicionContorno** BC;
	TCondicionContorno** BCIntakeValve;
	TCondicionContorno** BCExhaustValve;
	TCondicionContorno** BCReedValve;
	TCondicionContorno** BCWasteGate;

	TCCExternalConnection** BCExtConnection;
	TCCExternalConnectionVol** BCExtConnectionVol;

	TCCCompresorVolumetrico** VolumetricCompressor;
	TCCDescargaExtremoAbierto** MatlabDischarge;
	TCCExtremoInyeccion** InjectionEnd;
	TCCPerdidadePresion **PerdidaPresion;

	// !OUTPUT OBJECT
	TOutputResults* Output;

	// ! CONTROL PARAMETERS
	bool FirstIteration;
	int JStepMax;
	int JStepMaxDPF;
	int JCurrent;
	int JCurrentDPF;
	double TimeEndStep;
	double DeltaTPlenums;
	bool Independent;
	bool Is_EndStep;
	bool PipeStepMax;
	bool DPFStepMax;
	bool TimeMinPipe;
	bool TimeMinDPF;

	double CrankAngle;
	double AcumulatedTime;
	double Theta;
	double Theta0;

	// ! SPECIES MODEL PARAMETERS

	stEspecies* SpeciesName;
	int SpeciesNumber;

	nmTipoCalculoEspecies SpeciesModel;

	double* AtmosphericComposition;

	nmTipoCombustible FuelType;
	nmCalculoGamma GammaCalculation;

	// ! GENERAL PARAMETERS
	nmTipoMotor EngineType;

	nmTipoModelado SimulationType;
	bool ThereIsEGR;
	bool ThereIsFuel;
	int OpenWAMVersion;
	int Steps;
	int Increment;
	float Percentage;
	double ThetaIni;
	double ene;
	double agincr;
	double thmax;
	double grmax;
	double SimulationDuration;
	int CyclesWithoutThemalInertia;
	double AmbientPressure;
	double AmbientTemperature;
	bool ConvergenceFirstTime;

	// ! DOES THE ENGINE BLOCK EXIST?
	bool EngineBlock;

	// ! NUMBER OF PIPES
	int NumberOfPipes;

	// ! NUMBER OF CONCENTRIC ELEMENTS
	int NumberOfConcentrics;

	// ! NUMBER OF DIESEL PARTICULATE FILTERS
	int NumberOfDPF;

	// ! VALVES PARAMETERS
	int NumberOfValves;
	int NumberOfReedValves;
	int NumberOfWasteGates;
	int NumberOfExternalCalculatedValves;

	// ! CONNECTIONS PARAMETERS
	int NumberOfConnections;
	int NumberOfVolumetricCompressors;
	int NumberOfExhaustValves;
	int NumberOfIntakeValves;
	int NumberOfCompressorsConnections;
	int NumberOfInjectionEnds;
	int NumberOfConectionsBetweenPlenums;
	int NumberOfButerflyValves;

	// ! NUMBER OF PLENUMS
	int NumberOfPlenums;

	// ! NUMBER OF VENTURIS
	int NumberOfVenturis;

	// ! NUMBER OF DIRECTIONAL JUNCIONS
	int NumberOfDirectionalJunctions;

	// ! PARAMETER FOR THE CONTROL UNIT
	int NumberOfSensors;

	TSensor **Sensor;

	int NumberOfControllers;

	TController **Controller;

	// ! EXTERNAL CALCULATION PARAMETERS
	bool ThereIsDLL;
	int controlvalv;
	int nematlab;

	// ! TURBINE PARAMETERS
	int NumberOfTurbines;
	int CountVGT;

	// ! NUMBER OF TURBOCHARGER AXIS
	int NumberOfAxis;

	// ! NUMBER OF COMPRESSORS
	int NumberOfCompressors;

	// ! NUMBER OF PRESSURE LOSSES
	int NumTCCPerdidaPresion;

	int fi_num_threads; ///< Available threads for CalculateFlowIndependent.

	/**
	 * @brief Assigns the number of threads for CalculateFlowIndependent.
	 *
	 * As CalculateFlowFlowIndependent can use up to 3 threads, it counts
	 * the number of available processors and sets fi_num_threads to 1, 2
	 * or 3 accordingly.  Also, if OMP_NUM_THREADS is set to 2 or 1, it
	 * observes it.
	 */
	void InitFlowIndependentNumThreads();

	void CleanLabelsX();

	void CleanLabels();

	void ReadGeneralData();

	void ReadEngine();

	void ReadPipes();

	void ReadDPF();

	void ReadConcentric();

	void ReadValves();

	void ReadPlenums();

	void ReadCompressors();

	void ReadConnections();

	void ReadTurbochargerAxis();

	void ReadSensors();

	void ReadControllers();

	void ReadOutput(char* FileName);

	void ReadDataDLL();

	void RunningControl();

	void InitializeRunningAngles();

	void AllocateVGTData();

	void CalculateNewHeatPositions();

	void CalculateDistance(int NodoOrigen, int NodoFin, double Longitud, int NumberOfPlenums, int NumberOfPipes,
						   int NumberOfConnections, TTubo **Pipe, TCondicionContorno **BC);

	int SelectPipe(TTubo **Pipe, int NumberOfPipes, int nodo1, int nodo2);

	void MethodStability();

	void SearchMinimumTimeStep();

	void StudyInflowOutflowMass();

	void SearchMinimumTime(int LNumDepInicial, double* LTMinimo, TDeposito **LPlenum);

	void SearchMinimumTimeGroup(double *LTMinimo, int LNumDeposito, TDeposito **LPlenum);

	void FixTimeStep();

	void FixTimeStepExternal(double deltat);

	void RecalculateStability();

	void SolveAdjacentElements(int PipeEnd, double TiempoActual);

	void SolveBranch(int NumDeposito, double TiempoActual);

	void UpdateEngine();

	void SolveRoadLoadModel();

	void RecalculateStabilitySolver();

	void UpdateTurbocharger();

	void comunica_wam_dll();

	void ModificacionControlEjecucion();

	void Actuadores();

  public:

	TOpenWAM();

	~TOpenWAM();

	void ReadInputData(char* FileName);

	void InitializeParameters();

	void ConnectFlowElements();

	void ConnectControlElements();

	void InitialHeatTransferParameters();

	void DetermineTimeStepIndependent();

	void DetermineTimeStepCommon();

	void DetermineTimeStep(double t);

	void InitializeOutput();

	void CalculateFlowIndependent();

	void CalculateFlowCommon();

	void ManageOutput();

	bool CalculationEnd();

	void Progress();

	void ProgressBegin();

	void ProgressEnd();

	void NewEngineCycle();

	void GeneralOutput();

	bool IsIndependent() {
		return Independent;
	}
	;

	void UpdateExternalBoundary(int i, double U0, double U1, double T0, double T1, double P0, double P1, double t);

	void UpdateExternalBoundary(int i, double U0, double T0, double P0, double t);

	void InitiateExternalBoundary(int i, double D0, double D1, double dX);

	void InitiateExternalBoundary(int i, double D0, double dX);

	void LoadNewData(int i, double* p, double* T, double* u);

	bool GetIs_EndStep();

};
// ---------------------------------------------------------------------------
#endif
