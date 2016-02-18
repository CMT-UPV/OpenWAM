/* --------------------------------------------------------------------------------*\
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
 \*-------------------------------------------------------------------------------- */

// ---------------------------------------------------------------------------
#pragma hdrstop

#include "TOpenWAM.h"
#ifdef _WIN32
#include <Windows.h>
#endif
#include "Version.h"

TOpenWAM::TOpenWAM() {

#ifdef gestorcom
	GestorWAM = NULL;
	GestorWAM = new TCGestorWAM();
	GestorWAM->Init();
#endif
	tzstr = "TZ=PST8PDT";

	DatosTGV = NULL;
	fc = NULL;

	Engine = NULL;
	Compressor = NULL;
	EXTERN = NULL;
	Axis = NULL;

	// ! ARRAY OF TYPES OF VALVES
	TypeOfValve = NULL;

	// ! POINTERS ARRAY TO VALVES TYPE TURBINE STATOR
	StatorTurbine = NULL;
	// ! POINTERS ARRAY TO VALVES TYPE TURBINE ROTOR
	RotorTurbine = NULL;
	// ! POINTERS ARRAY TO EXTERNAL CONNECTIONS
	CCCalcExtern = NULL;
	BCButerflyValve = NULL;

	// ! ARRAY OF PIPES
	Pipe = NULL;

	// ! ARRAY OF CONCENTRIC ELEMENTS
#ifdef ConcentricElement
	Concentric = NULL;
#endif

	// ! ARRAY OF DPFs
#ifdef ParticulateFilter
	DPF = NULL;
#endif

	// ! ARRAYS OF PLENUMS
	Plenum = NULL;
	Turbine = NULL;
	Venturi = NULL;

	// ! ARRAYS OF BOUNDARY CONDITIONS
	BC = NULL;
	BCIntakeValve = NULL;
	BCExhaustValve = NULL;
	BCReedValve = NULL;
	BCWasteGate = NULL;

	VolumetricCompressor = NULL;
	MatlabDischarge = NULL;
	InjectionEnd = NULL;
	PerdidaPresion = NULL;

	// !OUTPUT OBJECT
	Output = NULL;

	CrankAngle = 0.;
	AcumulatedTime = 0.;
	Theta = 0.;
	Theta0 = 0.;

	// ! SPECIES MODEL PARAMETERS

	SpeciesName = NULL;
	SpeciesNumber = 0;

	AtmosphericComposition = NULL;

	// ! GENERAL PARAMETERS
	SimulationType = nmEstacionario;
	ThereIsEGR = false;
	ThereIsFuel = false;
	OpenWAMVersion = 0;
	Steps = 0;
	Increment = 0;
	Percentage = 0;
	ThetaIni = 0.;
	ene = 0.;
	agincr = 0.;
	thmax = 0.;
	grmax = 0.;
	SimulationDuration = 0.;
	CyclesWithoutThemalInertia = 0;
	AmbientPressure = 0.;
	AmbientTemperature = 0.;
	ConvergenceFirstTime = false;

	Independent = true;
	PipeStepMax = false;
	DPFStepMax = false;
	TimeMinPipe = false;
	TimeMinDPF = false;

	// ! DOES THE ENGINE BLOCK EXIST?
	EngineBlock = false;

	// ! NUMBER OF PIPES
	NumberOfPipes = 0;

	// ! NUMBER OF CONCENTRIC ELEMENTS
	NumberOfConcentrics = 0;

	// ! NUMBER OF DIESEL PARTICULATE FILTERS
	NumberOfDPF = 0;

	// ! VALVES PARAMETERS
	NumberOfValves = 0;
	NumberOfReedValves = 0;
	NumberOfWasteGates = 0;
	NumberOfExternalCalculatedValves = 0;

	// ! CONNECTIONS PARAMETERS
	NumberOfConnections = 0;
	NumberOfVolumetricCompressors = 0;
	NumberOfExhaustValves = 0;
	NumberOfIntakeValves = 0;
	NumberOfCompressorsConnections = 0;
	NumberOfInjectionEnds = 0;
	NumberOfConectionsBetweenPlenums = 0;
	NumberOfButerflyValves = 0;

	// ! NUMBER OF PLENUMS
	NumberOfPlenums = 0;

	// ! NUMBER OF VENTURIS
	NumberOfVenturis = 0;

	// ! NUMBER OF DIRECTIONAL JUNCIONS
	NumberOfDirectionalJunctions = 0;

	// ! PARAMETER FOR THE CONTROL UNIT
	NumberOfSensors = 0;

	Sensor = NULL;

	NumberOfControllers = 0;

	Controller = NULL;

	// ! EXTERNAL CALCULATION PARAMETERS
	ThereIsDLL = false;
	controlvalv = 0;
	nematlab = 0;

	// ! TURBINE PARAMETERS
	NumberOfTurbines = 0;
	CountVGT = 0;

	// ! NUMBER OF TURBOCHARGER AXIS
	NumberOfAxis = 0;

	// ! NUMBER OF COMPRESSORS
	NumberOfCompressors = 0;

	Steps = 0;
	TimeEndStep = 0.;

	InitFlowIndependentNumThreads();
}

TOpenWAM::~TOpenWAM() {
#ifdef completo
	if(ThereIsDLL) {
		EXTERN->FinECU();
	}
#endif

	printf("INFO: *** CMT : SIMULATION FINISHED CORRECTLY ***\n");

#ifdef gestorcom
	if(GestorWAM != NULL)
		GestorWAM->ProcesoTranscurrido(100);
#endif

	if(EXTERN != NULL)
		delete EXTERN;

	// Liberacion memoria dinamica Depositos.
	if(NumberOfPlenums > 0 && Plenum != NULL) {
		for(int i = 0; i < NumberOfPlenums; i++)
			delete Plenum[i];
		delete[] Plenum;
	}

	if(NumberOfTurbines > 0 && Turbine != NULL)
		delete[] Turbine;

	if(NumberOfVenturis > 0 && Venturi != NULL)
		delete[] Venturi;

	// Liberacion memoria dinamica Compressor.
	if(NumberOfCompressors > 0 && Compressor != NULL) {
		for(int i = 0; i < NumberOfCompressors; i++)
			delete Compressor[i];
		delete[] Compressor;
	}

	// Liberacion memoria dinamica Tubos.
	if(NumberOfPipes > 0 && Pipe != NULL) {
		for(int i = 0; i < NumberOfPipes; i++)
			delete Pipe[i];
		delete[] Pipe;
	}

	// Liberacion memoria dinamica Concentricos.
#ifdef ConcentricElement
	if(NumberOfConcentrics > 0 && Concentric != NULL) {
		for(int i = 0; i < NumberOfConcentrics; i++)
			delete Concentric[i];
		delete[]Concentric;
	}
#endif

	// Liberacion memoria dinamica DPFs.
#ifdef ParticulateFilter
	if(NumberOfDPF > 0 && DPF != NULL) {
		for(int i = 0; i < NumberOfDPF; i++)
			delete DPF[i];
		delete[]DPF;
	}
#endif

	// Liberacion memoria dinamica Condiciones de Contorno.
	if(NumberOfConnections > 0 && *BC != NULL) {
		for(int i = 0; i < NumberOfConnections; i++)
			delete BC[i];
		delete[] BC;
	}

	// Liberacion memoria dinamica Axis de Turbogrupo.
	if(NumberOfAxis > 0 && Axis != NULL) {
		for(int i = 0; i < NumberOfAxis; i++)
			delete Axis[i];
		delete[] Axis;
	}

	// Liberacion memoria dinamica Engine.
	if(EngineBlock && Engine != NULL) {
		delete Engine[0];
		delete[] Engine;
	}

	if(NumberOfIntakeValves > 0 && BCIntakeValve != NULL)
		delete[] BCIntakeValve;

	if(NumberOfExhaustValves > 0 && BCExhaustValve != NULL)
		delete[] BCExhaustValve;

	if(NumberOfWasteGates > 0 && BCWasteGate != NULL)
		delete[] BCWasteGate;

	if(NumberOfReedValves > 0 && BCReedValve != NULL)
		delete[] BCReedValve;

	if(NumberOfVolumetricCompressors > 0 && VolumetricCompressor != NULL)
		delete[] VolumetricCompressor;

	if(nematlab > 0 && MatlabDischarge != NULL)
		delete[] MatlabDischarge;

	if(NumberOfInjectionEnds > 0 && InjectionEnd != NULL)
		delete[] InjectionEnd;

	if(NumTCCPerdidaPresion > 0 && PerdidaPresion != NULL)
		delete[] PerdidaPresion;

	if(SpeciesName != NULL)
		delete[] SpeciesName;

	if(AtmosphericComposition != NULL)
		delete[] AtmosphericComposition;

	if(DatosTGV != NULL)
		delete[] DatosTGV;

	if(TypeOfValve != NULL) {
		for(int i = 0; i < NumberOfValves; i++) {
			delete TypeOfValve[i];
		}
		delete[] TypeOfValve;
	}

	if(StatorTurbine != NULL) {
		for(int i = 0; i < NumberOfTurbines; i++) {
			delete[] StatorTurbine[i];
		}
		delete[] StatorTurbine;
	}

	if(RotorTurbine != NULL)
		delete[] RotorTurbine;

	if(CCCalcExtern != NULL)
		delete[] CCCalcExtern;

	if(BCButerflyValve != NULL)
		delete[] BCButerflyValve;

#ifdef gestorcom
	if(GestorWAM != NULL) {
		GestorWAM->NuevoMensaje("Simulation finished correctly.");
		GestorWAM->Terminar();
		delete GestorWAM;
	}
#endif

}

void TOpenWAM::CleanLabelsX() {

	FILE *fetmp;

	//strcpy(fileinput.c_str(), "tmp.wam");

	fileinput = "tmp.wam";

	fetmp = fopen(fileinput.c_str(), "w");

	int cc = 0, cc2 = 0;
	bool label;
	char line[800];
	char linenew[800];
	while(!feof(FileInput)) {
		fgets(line, 800, FileInput);
		cc = 0;
		cc2 = 0;
		label = false;
		while(line[cc] == ' ' || line[cc] == '\t') {
			cc++;
		}
		while(line[cc] != '\n' && cc < 800) {
			if(label) {
				if(line[cc] == '>') {
					label = false;
				}
				cc++;
			} else {
				if(line[cc] == '<') {
					label = true;
				} else {
					linenew[cc2] = line[cc];
					cc2++;
				}
				cc++;
			}
		}
		linenew[cc2] = '\n';
		for(int i = cc2 + 1; i < 800; ++i) {
			linenew[i] = '\0';
		}
		if(linenew[0] != '\n') {
			fputs(linenew, fetmp);
		}
	}

	fclose(fetmp);
	fclose(FileInput);

}

void TOpenWAM::CleanLabels() {

	std::regex labelregex("<.*>");
	std::regex blregex("^[:s:]*\\r$");
	std::regex blregex2("^[:s:]*$");

	ifstream sFileInput;
	ofstream sFileOutput;
	string strline, strline_out;

	sFileInput.open(fileinput.c_str());
	fileinput.append(".tmp");
	sFileOutput.open(fileinput.c_str());

	while(!sFileInput.eof()) {
		std::getline(sFileInput, strline);
		strline_out = std::regex_replace(strline, labelregex, "");
		if(std::regex_match(strline_out, blregex) == false && std::regex_match(strline_out, blregex2) == false) {
			sFileOutput << strline_out << std::endl;
		}
	}
	sFileInput.close();
	sFileOutput.close();
}

void TOpenWAM::ReadInputData(char* FileName) {

	fpos_t fileposition;

	FileInput = fopen(FileName, "r");
	if(FileInput == NULL) {
		printf("ERROR: The input file do not exist.\n");
#ifdef gestorcom
		if(GestorWAM != NULL) {
			GestorWAM->NuevoMensaje("The input file do not exist.");
			GestorWAM->Terminar();
			delete GestorWAM;
		}
#endif
		exit(EXIT_FAILURE);
	}

#ifdef _WIN32

	fclose(FileInput);

	fileinput = FileName;

	CleanLabels();

#else
	CleanLabelsX();
#endif

	FileInput = fopen(fileinput.c_str(), "r");
	if(!FileInput) {
		std::perror("File opening failed: ");
		exit(EXIT_FAILURE);
	}

#ifdef gestorcom
	if(GestorWAM != NULL)
		GestorWAM->NuevoMensaje("Reading input data.");
#endif
	// -----------------------------------------------------------------------------
	// -----------------------------------------------------------------------------

	std::cout << "=======================" << std::endl;
	std::cout << " OpenWAM " << VERSION_PRINT << std::endl;
	std::cout << "=======================" << std::endl << std::endl;

	std::cout << "THE MODEL IS READING THE INPUT DATA" << std::endl << std::endl;

	// -----------------------------------------------------------------------------
	// -----------------------------------------------------------------------------

	fscanf(FileInput, "%d", &OpenWAMVersion);
	if(OpenWAMVersion != vers) {
		printf("ERROR: THE WAM VERSION IS NOT CORRECT FOR THESE INPUT DATA\n\n");
		printf("       OpenWAM version: %d", vers);
		printf("       File version: %d", OpenWAMVersion);
		exit(1);
	}
	int ind = 0;
	fscanf(FileInput, "%d ", &ind);
	ind == 0 ? Independent = false : Independent = true;

	ReadGeneralData();

	ReadEngine();

	ReadPipes();

#ifdef ParticulateFilter
	ReadDPF();
#endif

#ifdef ConcentricElement
	ReadConcentric();
#endif

	ReadValves();

	ReadPlenums();

	ReadCompressors();

	ReadConnections();

	ReadTurbochargerAxis();

	ReadSensors();

	ReadControllers();

	ReadOutput(FileName);

	fgetpos(FileInput, &fileposition);
	fclose(FileInput);

	for(int i = 0; i < NumberOfConnections; i++) {
		if(BC[i]->getTipoCC() == nmCompresor) {
			dynamic_cast<TCCCompresor*>(BC[i])->ReadCompressorData(fileinput.c_str(), fileposition, Compressor);
		}
	}

	FileInput = fopen(fileinput.c_str(), "r");
	fsetpos(FileInput, &fileposition);

	int dll = 0;
	fscanf(FileInput, "%d", &dll);
	dll == 0 ? ThereIsDLL = false : ThereIsDLL = true;
	if(ThereIsDLL) {
		EXTERN = new TCalculoExtern();

		ReadDataDLL();
	}
	printf("INFO: The input file data has been readed correctly\n\n");
#ifdef gestorcom
	if(GestorWAM != NULL)
		GestorWAM->NuevoMensaje("Performing preliminar calculations...");
#endif

	fclose(FileInput);
	if(remove(fileinput.c_str()) != 0)
		perror("WARNING: Error deleting file\n");
	else
		puts("INFO: File successfully deleted\n");
}

void TOpenWAM::ReadDataDLL() {
	try {
		fpos_t filepos;
		fgetpos(FileInput, &filepos);
		fclose(FileInput);

		if(Engine != NULL) {
			EXTERN->LeeFicherosDLL(fileinput.c_str(), filepos, controlvalv, nematlab, Engine[0]->getGeometria().NCilin,
								   NumberOfExternalCalculatedValves, CountVGT, SpeciesNumber,
								   NumTCCPerdidaPresion);
		} else {
			EXTERN->LeeFicherosDLL(fileinput.c_str(), filepos, controlvalv, nematlab, 0, NumberOfExternalCalculatedValves, CountVGT,
								   SpeciesNumber, NumTCCPerdidaPresion);
		}
		if(Pipe != NULL)
			EXTERN->Lee_Sens_Tubos(fileinput.c_str(), filepos, Pipe, SpeciesModel, ThereIsEGR, ThereIsFuel);
		if(Plenum != NULL)
			EXTERN->Lee_Sens_Dep(fileinput.c_str(), filepos, Plenum, SpeciesModel, ThereIsEGR, ThereIsFuel);
		if(Axis != NULL)
			EXTERN->Lee_Sens_TG(fileinput.c_str(), filepos, Axis);
		if(Turbine != NULL)
			EXTERN->Lee_Sens_Turbina(fileinput.c_str(), filepos, Turbine);
		if(Engine != NULL)
			EXTERN->Lee_Sens_Cil(fileinput.c_str(), filepos, Engine);
		if(Venturi != NULL)
			EXTERN->Lee_Sens_Vent(fileinput.c_str(), filepos, Venturi);
		if(Engine != NULL)
			EXTERN->Lee_Sens_Motor(fileinput.c_str(), filepos, Theta, Engine[0]->getRegimen(), AcumulatedTime);
		if(NumberOfConectionsBetweenPlenums != 0)
			EXTERN->Lee_Sens_UED(fileinput.c_str(), filepos, BC);
		EXTERN->Lectura_Datos_Adicionales(fileinput.c_str(), filepos);
		EXTERN->IniciaEntradaDLL();

		FileInput = fopen(fileinput.c_str(), "r");
		fsetpos(FileInput, &filepos);

	} catch(exception & N) {
		stringstream err;
		std::cout << "ERROR: ReadDataDLL" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		err << "ERROR: ReadDataDLL" << N.what();
		throw Exception(err.str());
	}
}

void TOpenWAM::ReadGeneralData() {
	try {

		int hayBQ = 0;
		int tipociclo = 0, tipomod = 0, tipocalculoespecies = 0;
		double fracciontotal = 0.;
		int haycombustible, tipocombustible, tipogamma, EGR, IntEGR = 1;
		stEspecies *DatEsp;
		double *CompAtmosfera;

		fscanf(FileInput, "%lf %lf ", &agincr, &SimulationDuration);
		fscanf(FileInput, "%lf %lf ", &AmbientPressure, &AmbientTemperature);
		fscanf(FileInput, "%d %d ", &tipocalculoespecies, &tipogamma);

		switch(tipogamma) {
		case 0:
			GammaCalculation = nmGammaConstante;
			break;
		case 1:
			GammaCalculation = nmComposicion;
			break;
		case 2:
			GammaCalculation = nmComposicionTemperatura;
			break;
		}

		fscanf(FileInput, "%d ", &hayBQ);

		hayBQ == 0 ? EngineBlock = false : EngineBlock = true;

		if(EngineBlock) {
			fscanf(FileInput, "%d %d %d", &tipociclo, &tipomod, &EGR);
			if(tipociclo == 1) {
				EngineType = nm2T;
			} else {
				EngineType = nm4T;
			}
			EGR == 0 ? ThereIsEGR = false : ThereIsEGR = true;
			EGR == 0 ? IntEGR = 1 : IntEGR = 0;
			switch(tipomod) {
			case 0:
				SimulationType = nmEstacionario;
				break;
			case 1:
				SimulationType = nmTransitorioCarga;
				break;
			case 2:
				SimulationType = nmTransitorioRegimen;
				break;
			case 3:
				SimulationType = nmTransitorioRegimenExterno;
				break;
			}
			if(SimulationType != nmEstacionario) {
				fscanf(FileInput, "%d", &CyclesWithoutThemalInertia);
			}
		}

		switch(tipocalculoespecies) {
		case 0:
			SpeciesModel = nmCalculoSimple;
			break;
		case 1:
			SpeciesModel = nmCalculoCompleto;
			break;
		}

		if(SpeciesModel == nmCalculoCompleto) {
			fscanf(FileInput, "%d ", &haycombustible);
			haycombustible == 0 ? ThereIsFuel = false : ThereIsFuel = true;
			if(haycombustible == 1) {   // Fuel injection is considered
				fscanf(FileInput, "%d ", &tipocombustible);
				switch(tipocombustible) {
				case 0:
					FuelType = nmDiesel;
					break;
				case 1:
					FuelType = nmGasolina;
					break;
				}
				SpeciesNumber = 10;
			} else if(haycombustible == 0) {   // Fuel injection is not considered
				SpeciesNumber = 9;
			}

			DatEsp = new stEspecies[SpeciesNumber - IntEGR];

			// DatEsp[0].Nombre = new char[15];
			DatEsp[0].Nombre = "O2";
			DatEsp[0].R = 259.825; // J/kgK

			// DatEsp[1].Nombre = new char[15];
			DatEsp[1].Nombre = "CO2";
			DatEsp[1].R = 188.9207; // J/kgK

			// DatEsp[2].Nombre = new char[15];
			DatEsp[2].Nombre = "H2O";
			DatEsp[2].R = 461.398; // J/kgK

			// DatEsp[3].Nombre = new char[15];
			DatEsp[3].Nombre = "HC";
			DatEsp[3].R = 638.58; // J/kgK

			// DatEsp[4].Nombre = new char[15];
			DatEsp[4].Nombre = "Soot";
			DatEsp[4].R = 692.866; // J/kgK   Se asimila al carbono.

			// DatEsp[5].Nombre = new char[15];
			DatEsp[5].Nombre = "NOx";
			DatEsp[5].R = 277.1466; // J/kgK

			// DatEsp[6].Nombre = new char[15];
			DatEsp[6].Nombre = "CO";
			DatEsp[6].R = 296.837; // J/kgK

			if(haycombustible == 1) {
				if(FuelType == nmDiesel) {
					// DatEsp[7].Nombre = new char[15];
					DatEsp[7].Nombre = "Diesel";
					DatEsp[7].R = 55.95; // J/kgK
				} else if(FuelType == nmGasolina) {
					// DatEsp[7].Nombre = new char[15];
					DatEsp[7].Nombre = "Gasolina";
					DatEsp[7].R = 72.425; // J/kgK
				}
				// DatEsp[8].Nombre = new char[15];
				DatEsp[8].Nombre = "N2";
				DatEsp[8].R = 296.837; // J/kgK

				if(ThereIsEGR) {
					// DatEsp[9].Nombre = new char[15];
					DatEsp[9].Nombre = "EGR";
					DatEsp[9].R = 287.; // J/kgK
				}
			} else {
				// DatEsp[7].Nombre = new char[15];
				DatEsp[7].Nombre = "N2";
				DatEsp[7].R = 296.837; // J/kgK

				if(ThereIsEGR) {
					// DatEsp[8].Nombre = new char[15];
					DatEsp[8].Nombre = "EGR";
					DatEsp[8].R = 287.; // J/kgK
				}
			}

		} else if(SpeciesModel == nmCalculoSimple) {
			fscanf(FileInput, "%d ", &haycombustible);
			if(haycombustible == 1) {   // Existe inyeccion de combustible
				fscanf(FileInput, "%d ", &tipocombustible);
				switch(tipocombustible) {
				case 0:
					FuelType = nmDiesel;
					break;
				case 1:
					FuelType = nmGasolina;
					break;
				}
				SpeciesNumber = 4;
			} else if(haycombustible == 0) {
				// No existe inyeccion de combustible
				SpeciesNumber = 3;
			}
			DatEsp = new stEspecies[SpeciesNumber - IntEGR];

			DatEsp[0].Nombre = new char[15];
			DatEsp[0].Nombre = "GasesQuemados";
			DatEsp[0].R = 285.4; // J/kgK

			if(haycombustible == 1) {
				if(FuelType == nmDiesel) {
					// DatEsp[1].Nombre = new char[15];
					DatEsp[1].Nombre = "Diesel";
					DatEsp[1].R = 55.95; // J/kgK
				} else if(FuelType == nmGasolina) {
					// DatEsp[1].Nombre = new char[15];
					DatEsp[1].Nombre = "Gasolina";
					DatEsp[1].R = 72.425; // J/kgK
				}
				// DatEsp[2].Nombre = new char[15];
				DatEsp[2].Nombre = "Aire";
				DatEsp[2].R = 287.; // J/kgK

				if(ThereIsEGR) {
					// DatEsp[3].Nombre = new char[15];
					DatEsp[3].Nombre = "EGR";
					DatEsp[3].R = 287.; // J/kgK
				}
			} else {
				// DatEsp[1].Nombre = new char[15];
				DatEsp[1].Nombre = "Aire";
				DatEsp[1].R = 287.; // J/kgK.

				if(ThereIsEGR) {
					// DatEsp[2].Nombre = new char[15];
					DatEsp[2].Nombre = "EGR";
					DatEsp[2].R = 287.; // J/kgK
				}
			}
		}
		SpeciesName = DatEsp;

		// A continuacion se lee la composicion del aire atmosferico

		CompAtmosfera = new double[SpeciesNumber - IntEGR];
		for(int i = 0; i < SpeciesNumber - 1; i++) {
			fscanf(FileInput, "%lf ", &CompAtmosfera[i]);

			fracciontotal += CompAtmosfera[i];
		}
		if(ThereIsEGR)
			CompAtmosfera[SpeciesNumber - 1] = 0.;
		if(fracciontotal != 1.) {
			std::cout << "ERROR: The total mass fraction must be equal to 1. Check your input data " << std::endl;
			throw Exception(" ");
		}

		AtmosphericComposition = CompAtmosfera;

	} catch(exception & N) {
		std::cout << "ERROR: ReadGeneralData" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TOpenWAM::ReadEngine()

{
	try {
		fpos_t filepos;

		if(EngineBlock) {
			fgetpos(FileInput, &filepos);
			fclose(FileInput);
			Engine = new TBloqueMotor*[1];
			Engine[0] = new TBloqueMotor(AmbientPressure, AmbientTemperature, SpeciesModel, SpeciesNumber, GammaCalculation,
										 ThereIsEGR);
			Engine[0]->LeeMotor(fileinput.c_str(), filepos, SimulationType, CyclesWithoutThemalInertia, EngineType,
								AtmosphericComposition);
			FileInput = fopen(fileinput.c_str(), "r");
			fsetpos(FileInput, &filepos);
		}

	} catch(exception & N) {
		std::cout << "ERROR: ReadEngine " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TOpenWAM::ReadPipes() {
	try {
		fpos_t filepos;
		int tipomallado = 0;

		fscanf(FileInput, "%d ", &NumberOfPipes);
		Pipe = new TTubo*[NumberOfPipes];
		printf("Number of pipes: %d\n", NumberOfPipes);
		tipomallado = 1;

		fgetpos(FileInput, &filepos);
		fclose(FileInput);

		for(int i = 0; i < NumberOfPipes; i++) {
			Pipe[i] = new TTubo(SpeciesNumber, i, SimulationDuration, Engine, SpeciesModel, GammaCalculation, ThereIsEGR);
			Pipe[i]->LeeDatosGeneralesTubo(fileinput.c_str(), filepos);
			if(EngineBlock) {
				Pipe[i]->LeeDatosGeometricosTubo(fileinput.c_str(), filepos, Engine[0]->getRegimen(), tipomallado, Engine);
			} else {
				Pipe[i]->LeeDatosGeometricosTubo(fileinput.c_str(), filepos, -1., tipomallado, Engine);
			}
			cout << "INFO: Pipe n. " << i + 1 << " - N. of cells " << Pipe[i]->getNin() << " - Mesh size = " <<
				 Pipe[i]->getMallado() << " m." << endl;
//					i + 1, Pipe[i]->getNin(), Pipe[i]->getMallado());
		}

		FileInput = fopen(fileinput.c_str(), "r");
		fsetpos(FileInput, &filepos);
	} catch(exception & N) {
		std::cout << "ERROR: ReadPipes" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TOpenWAM::ReadDPF() {
	try {
#ifdef ParticulateFilter
		fpos_t filepos;

		fscanf(FileInput, "%d ", &NumberOfDPF);
		DPF = new TDPF*[NumberOfDPF];
		printf("Number of DPF: %d\n", NumberOfDPF);

		fgetpos(FileInput, &filepos);
		fclose(FileInput);

		for(int i = 0; i < NumberOfDPF; i++) {
			DPF[i] = new TDPF(i + 1, Engine, SpeciesNumber);
			DPF[i]->LeeDatosDPF(fileinput.c_str(), filepos, SpeciesModel,
								GammaCalculation, ThereIsEGR, Engine);
		}

		FileInput = fopen(fileinput.c_str(), "r");
		fsetpos(FileInput, &filepos);
#endif
	} catch(exception & N) {
		std::cout << "ERROR: ReadDPF" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TOpenWAM::ReadConcentric() {
	try {
#ifdef ConcentricElement
		fpos_t filepos;
		int numducts = 0;

		fscanf(FileInput, "%d ", &NumberOfConcentrics);
		Concentric = new TConcentrico*[NumberOfConcentrics];
		printf("Number of concentrics: %d\n", NumberOfConcentrics);

		for(int i = 0; i < NumberOfConcentrics; i++) {
			fscanf(FileInput, "%d ", &numducts);
			fgetpos(FileInput, &filepos);
			fclose(FileInput);
			if(numducts == 2) {
				Concentric[i] = new TConcentricoTubos(i);
#ifdef ParticulateFilter
				Concentric[i]->LeeDatosTuboConcentrico(fileinput.c_str(), filepos,
													   Pipe, DPF);
#else
				Concentric[i]->LeeDatosTuboConcentrico(fileinput.c_str(), filepos,
													   Pipe, NULL);
#endif
			} else if(numducts == 1) {
				Concentric[i] = new TConcentricoDPF(i);
#ifdef ParticulateFilter
				Concentric[i]->LeeDatosTuboConcentrico(fileinput.c_str(), filepos,
													   Pipe, DPF);
#else
				Concentric[i]->LeeDatosTuboConcentrico(fileinput.c_str(), filepos,
													   Pipe, NULL);
#endif
			}
			FileInput = fopen(fileinput.c_str(), "r");
			fsetpos(FileInput, &filepos);
		}
#endif
	} catch(exception & N) {
		std::cout << "ERROR: ReadConcentric" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TOpenWAM::ReadValves() {
	try {
		fscanf(FileInput, "%d ", &NumberOfValves);
		TypeOfValve = new TTipoValvula*[NumberOfValves];
		int val = 0;
		int NumTCDFijo = 0;
		int NumTValvula4T = 0;
		int NumberOfReedValves = 0;
		int NumTDiscoRotativo = 0;
		int NumTLumbrera = 0;
		int NumTValvulaContr = 0;
		int NumberOfWasteGates = 0;
		int NumTEstatorTurbina = 0;
		int NumTRotorTurbina = 0;
		int NumTCDExterno = 0;
		int NumberOfButerflyValves = 0;
		int tipval = 0;
		fpos_t filepos;

		for(int i = 0; i < NumberOfValves; ++i) {
			fscanf(FileInput, "%d ", &tipval);
			switch(tipval) {
			case 0:
				TypeOfValve[i] = new TCDFijo();
				val = NumTCDFijo;
				NumTCDFijo++;
				break;
			case 1:
				TypeOfValve[i] = new TValvula4T();
				val = NumTValvula4T;
				NumTValvula4T++;
				break;
			case 2:
				TypeOfValve[i] = new TLamina();
				val = NumberOfReedValves;
				NumberOfReedValves++;
				break;
			case 3:
				TypeOfValve[i] = new TDiscoRotativo();
				val = NumTDiscoRotativo;
				NumTDiscoRotativo++;
				break;
			case 4:
				TypeOfValve[i] = new TLumbrera(Engine[0]->getGeometria().Biela, Engine[0]->getGeometria().Carrera);
				val = NumTLumbrera;
				NumTLumbrera++;
				break;
			case 5:
				TypeOfValve[i] = new TValvulaContr();
				val = NumTValvulaContr;
				NumTValvulaContr++;
				break;
			case 6:
				TypeOfValve[i] = new TWasteGate();
				val = NumberOfWasteGates;
				NumberOfWasteGates++;
				break;
			case 7:
				TypeOfValve[i] = new TEstatorTurbina();
				val = NumTEstatorTurbina;
				NumTEstatorTurbina++;
				break;
			case 8:
				TypeOfValve[i] = new TRotorTurbina();
				val = NumTRotorTurbina;
				NumTRotorTurbina++;
				break;
			case 9:
				controlvalv = 1;
				TypeOfValve[i] = new TCDExterno();
				val = NumTCDExterno;
				NumTCDExterno++;
				break;
			case 10:
				TypeOfValve[i] = new TMariposa();
				val = NumberOfButerflyValves;
				NumberOfButerflyValves++;
				break;
			}
			fgetpos(FileInput, &filepos);
			fclose(FileInput);
			if(!EngineBlock) {
				TypeOfValve[i]->LeeDatosIniciales(fileinput.c_str(), filepos, val, EngineBlock, NULL);
			} else {
				TypeOfValve[i]->LeeDatosIniciales(fileinput.c_str(), filepos, val, EngineBlock, Engine[0]);
			}
			FileInput = fopen(fileinput.c_str(), "r");
			fsetpos(FileInput, &filepos);
		}

	} catch(exception & N) {
		stringstream err;
		std::cout << "ERROR: ReadValves" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		err << "ERROR: ReadValves" << N.what();
		throw Exception(err.str());
	}
}

void TOpenWAM::ReadPlenums() {

	try {
		fpos_t filepos;
		int tipoDep, ncv = 0;
		int numeroturbina = 0, numeroventuri = 0;

		/* PARAMETERS USED BY WAMMER */
		int numeroturbinas = 0, numeroventuris = 0, numerounionesdireccionales = 0;

		fscanf(FileInput, "%d ", &NumberOfPlenums);
		/* PARAMETERS USED BY WAMMER */
		fscanf(FileInput, "%d %d %d", &numeroturbinas, &numeroventuris, &numerounionesdireccionales);

		fgetpos(FileInput, &filepos);
		fclose(FileInput);

		NumberOfTurbines = 0;
		NumberOfVenturis = 0;
		NumberOfDirectionalJunctions = 0;
		if(NumberOfPlenums != 0) {
			Plenum = new TDeposito*[NumberOfPlenums];
		}
		if(NumberOfPlenums != 0) {
			for(int i = 0; i < NumberOfPlenums; ++i) {
				FileInput = fopen(fileinput.c_str(), "r");
				fsetpos(FileInput, &filepos);
				fscanf(FileInput, "%d ", &tipoDep);
				fgetpos(FileInput, &filepos);
				fclose(FileInput);
				switch(tipoDep) {
				case 0:
					Plenum[i] = new TDepVolCte(i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					Plenum[i]->LeeDatosGeneralesDepositos(fileinput.c_str(), filepos);
					break;
				case 1:
					Plenum[i] = new TDepVolVariable(i, ncv, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					Plenum[i]->LeeDatosGeneralesDepositos(fileinput.c_str(), filepos);
					dynamic_cast<TDepVolVariable*>(Plenum[i])->LeeDatosDepVolVariable(fileinput.c_str(), filepos, EngineBlock);
					ncv++;
					break;
				case 2:
					FileInput = fopen(fileinput.c_str(), "r");
					fsetpos(FileInput, &filepos);
					fscanf(FileInput, "%d ", &numeroturbina);
					Plenum[i] = new TTurbinaSimple(i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					dynamic_cast<TTurbina*>(Plenum[i])->PutNumeroTurbina(numeroturbina);
					fgetpos(FileInput, &filepos);
					fclose(FileInput);
					Plenum[i]->LeeDatosGeneralesDepositos(fileinput.c_str(), filepos);
					dynamic_cast<TTurbina*>(Plenum[i])->LeeTurbina(fileinput.c_str(), filepos);
					dynamic_cast<TTurbina*>(Plenum[i])->IniciaMedias();
					NumberOfTurbines = NumberOfTurbines + 1;
					break;
				case 3:
					FileInput = fopen(fileinput.c_str(), "r");
					fsetpos(FileInput, &filepos);
					fscanf(FileInput, "%d ", &numeroturbina);
					Plenum[i] = new TTurbinaTwin(i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					dynamic_cast<TTurbina*>(Plenum[i])->PutNumeroTurbina(numeroturbina);
					fgetpos(FileInput, &filepos);
					fclose(FileInput);
					Plenum[i]->LeeDatosGeneralesDepositos(fileinput.c_str(), filepos);
					dynamic_cast<TTurbina*>(Plenum[i])->LeeTurbina(fileinput.c_str(), filepos);
					dynamic_cast<TTurbina*>(Plenum[i])->IniciaMedias();
					NumberOfTurbines = NumberOfTurbines + 1;
					break;
				case 4:
					FileInput = fopen(fileinput.c_str(), "r");
					fsetpos(FileInput, &filepos);
					fscanf(FileInput, "%d ", &numeroventuri);
					Plenum[i] = new TVenturi(i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					dynamic_cast<TVenturi*>(Plenum[i])->PutNumeroVenturi(numeroventuri);
					fgetpos(FileInput, &filepos);
					fclose(FileInput);
					NumberOfVenturis = NumberOfVenturis + 1;
					Plenum[i]->LeeDatosGeneralesDepositos(fileinput.c_str(), filepos);
					dynamic_cast<TVenturi*>(Plenum[i])->LeeDatosVenturi(fileinput.c_str(), filepos);
					break;
				case 5:
					NumberOfDirectionalJunctions = NumberOfDirectionalJunctions + 1;
					Plenum[i] = new TUnionDireccional(i, NumberOfDirectionalJunctions, SpeciesModel, SpeciesNumber, GammaCalculation,
													  ThereIsEGR);
					Plenum[i]->LeeDatosGeneralesDepositos(fileinput.c_str(), filepos);
					dynamic_cast<TUnionDireccional*>(Plenum[i])->LeeDatosUnionDireccional(fileinput.c_str(), filepos);
					break;
				}

			}
		}
		FileInput = fopen(fileinput.c_str(), "r");
		fsetpos(FileInput, &filepos);

		if(NumberOfTurbines > 0)
			Turbine = new TTurbina*[NumberOfTurbines];
		for(int i = 0; i < NumberOfTurbines; i++) {
			for(int j = 0; j < NumberOfPlenums; j++) {
				if(Plenum[j]->getTipoDeposito() == nmTurbinaSimple || Plenum[j]->getTipoDeposito() == nmTurbinaTwin) {
					if(i + 1 == dynamic_cast<TTurbina*>(Plenum[j])->getNumeroTurbina()) {
						Turbine[i] = dynamic_cast<TTurbina*>(Plenum[j]);
					}
				}
			}
		}
		if(NumberOfVenturis > 0)
			Venturi = new TVenturi*[NumberOfVenturis];
		for(int i = 0; i < NumberOfVenturis; i++) {
			for(int j = 0; j < NumberOfPlenums; j++) {
				if(Plenum[j]->getTipoDeposito() == nmVenturi) {
					if(dynamic_cast<TVenturi*>(Plenum[j])->getNumeroVenturi() == i + 1) {
						Venturi[i] = dynamic_cast<TVenturi*>(Plenum[j]);
					}
				}
			}
		}
	} catch(exception & N) {
		std::cout << "ERROR: ReadPlenums " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TOpenWAM::ReadCompressors() {
	try {
		fpos_t filepos;
		int TipoCompresor = 0;
		int haydeposito = 0, numid = 0, numid1 = 0, numid2 = 0;

		fscanf(FileInput, "%d ", &NumberOfCompressors);
		Compressor = new TCompresor*[NumberOfCompressors];
		fgetpos(FileInput, &filepos);
		fclose(FileInput);

		for(int j = 0; j < NumberOfCompressors; j++) {
			FileInput = fopen(fileinput.c_str(), "r");
			fsetpos(FileInput, &filepos);
			fscanf(FileInput, "%d ", &TipoCompresor);
			if(TipoCompresor == 0) {
				/* Lectura para Wamer */
				fscanf(FileInput, "%d ", &haydeposito);
				if(haydeposito == 1)
					fscanf(FileInput, "%d ", &numid);
			}
			if(TipoCompresor == 1) {
				/* Lectura para Wamer */
				fscanf(FileInput, "%d %d", &numid1, &numid2);
			}
			fgetpos(FileInput, &filepos);
			fclose(FileInput);
			switch(TipoCompresor) {
			case 0: /* Pipe - Deposito */
				Compressor[j] = new TCompTubDep(j, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
				(dynamic_cast<TCompTubDep*>(Compressor[j]))->LeeCompresor(fileinput.c_str(), filepos);
				break;
			case 1: /* Entre Depositos */
				Compressor[j] = new TCompresorDep(j, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
				(dynamic_cast<TCompresorDep*>(Compressor[j]))->LeeCompresor(fileinput.c_str(), filepos);
				break;
			case 2: /* Entre Tubos */
				Compressor[j] = new TCompTubos(j, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
				(dynamic_cast<TCompTubos*>(Compressor[j]))->LeeCompresor(fileinput.c_str(), filepos);
				break;
			}
		}
		FileInput = fopen(fileinput.c_str(), "r");
		fsetpos(FileInput, &filepos);
	} catch(exception & N) {
		std::cout << "ERROR: ReadCompressors " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TOpenWAM::ReadConnections() {
	try {
		fpos_t filepos;
		int numerocv = 0, contador = 0;
		int NumTCCDescargaExtremoAbierto = 0, NumTCCExtremoCerrado = 0, NumTCCExtremoAnecoico = 0, NumTCCPulso = 0,
			NumTCCUnionEntreTubos = 0, NumTCCCilindro = 0, NumTCCDeposito = 0,
			NumTCCRamificacion = 0, NumTCCEntradaCompresor = 0, NumTCCPreVble = 0;
		NumTCCPerdidaPresion = 0;
		int TipoCC = 0;
		int numerovalvula = 0, quevalv;
		int NumTCCExternalConnection = 0;

		// ! PARAMETERS USED BY WAMer
		int numnodosimples, numpulsos, numnododep, numperdpresion, numcomprtornillo, numextremosinyeccion,
			numnodoentredepositos, numentradacompresor, numentradapresionestatica;

		fscanf(FileInput, "%d ", &NumberOfConnections);
		// ! PARAMETERS USED BY WAMer
		fscanf(FileInput, "%d %d %d %d %d %d %d %d %d", &numnodosimples, &numpulsos, &numnododep, &numperdpresion,
			   &numcomprtornillo, &numextremosinyeccion, &numnodoentredepositos,
			   &numentradacompresor, &numentradapresionestatica);

		BC = new TCondicionContorno*[NumberOfConnections];
		printf("Number of boundary condition: %d\n", NumberOfConnections);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		if(NumberOfConnections != 0) {
			for(int i = 0; i <= NumberOfConnections - 1; ++i) {
				FileInput = fopen(fileinput.c_str(), "r");
				fsetpos(FileInput, &filepos);
				fscanf(FileInput, "%d ", &TipoCC);
				fgetpos(FileInput, &filepos);
				fclose(FileInput);
				switch(TipoCC) {
				case 0:
					BC[i] = new TCCDescargaExtremoAbierto(nmOpenEndAtmosphere, i, SpeciesModel, SpeciesNumber, GammaCalculation,
														  ThereIsEGR);
					NumTCCDescargaExtremoAbierto++;
#ifdef ParticulateFilter
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes,
											Pipe, NumberOfDPF, DPF);
#else
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes, Pipe, NumberOfDPF, NULL);
#endif
					BC[i]->AsignAmbientConditions(AmbientTemperature, AmbientPressure, AtmosphericComposition);
					break;
				case 1:
					BC[i] = new TCCDescargaExtremoAbierto(nmOpenEndReservoir, i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					NumTCCDescargaExtremoAbierto++;
#ifdef ParticulateFilter
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes,
											Pipe, NumberOfDPF, DPF);
#else
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes, Pipe, NumberOfDPF, NULL);
#endif
					break;
				case 2:
					BC[i] = new TCCDescargaExtremoAbierto(nmOpenEndCalcExtern, i, SpeciesModel, SpeciesNumber, GammaCalculation,
														  ThereIsEGR);
					NumTCCDescargaExtremoAbierto++;
					nematlab++;
#ifdef ParticulateFilter
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes,
											Pipe, NumberOfDPF, DPF);
#else
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes, Pipe, NumberOfDPF, NULL);
#endif
					break;
				case 3:
					BC[i] = new TCCExtremoCerrado(nmClosedEnd, i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					NumTCCExtremoCerrado++;
					BC[i]->AsignaTubos(NumberOfPipes, Pipe);
					break;
				case 4:
					BC[i] = new TCCExtremoAnecoico(nmAnechoicEnd, i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					NumTCCExtremoAnecoico++;
					BC[i]->AsignaTubos(NumberOfPipes, Pipe);
					break;
				case 5:
					BC[i] = new TCCPulso(nmIncidentPressurWave, i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					NumTCCPulso++;
#ifdef ParticulateFilter
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes,
											Pipe, NumberOfDPF, DPF);
#else
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes, Pipe, NumberOfDPF, NULL);
#endif
					break;
				case 6:
					BC[i] = new TCCUnionEntreTubos(nmPipesConnection, i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					NumTCCUnionEntreTubos++;
#ifdef ParticulateFilter
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes,
											Pipe, NumberOfDPF, DPF);
#else
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes, Pipe, NumberOfDPF, NULL);
#endif
					break;
				case 7:
					BC[i] = new TCCCilindro(nmIntakeValve, i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					NumTCCCilindro++;
					NumberOfIntakeValves++;
#ifdef ParticulateFilter
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes,
											Pipe, NumberOfDPF, DPF);
#else
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes, Pipe, NumberOfDPF, NULL);
#endif
					break;
				case 8:
					BC[i] = new TCCCilindro(nmExhaustValve, i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					NumTCCCilindro++;
					NumberOfExhaustValves++;
#ifdef ParticulateFilter
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes,
											Pipe, NumberOfDPF, DPF);
#else
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes, Pipe, NumberOfDPF, NULL);
#endif
					break;
				case 9:
					BC[i] = new TCCPerdidadePresion(nmLinearPressureLoss, i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					NumTCCPerdidaPresion++;
#ifdef ParticulateFilter
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes,
											Pipe, NumberOfDPF, DPF);
#else
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes, Pipe, NumberOfDPF, NULL);
#endif
					break;
				case 10:
					BC[i] = new TCCPerdidadePresion(nmQuadraticPressureLoss, i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					NumTCCPerdidaPresion++;
#ifdef ParticulateFilter
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes,
											Pipe, NumberOfDPF, DPF);
#else
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes, Pipe, NumberOfDPF, NULL);
#endif
					break;
				case 11:
					BC[i] = new TCCDeposito(nmPipeToPlenumConnection, i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					NumTCCDeposito++;
#ifdef ParticulateFilter
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes,
											Pipe, NumberOfDPF, DPF);
#else
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes, Pipe, NumberOfDPF, NULL);
#endif
					break;
				case 12:
					BC[i] = new TCCRamificacion(nmBranch, i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					NumTCCRamificacion++;
					BC[i]->AsignaTubos(NumberOfPipes, Pipe);
					break;
				case 13:
					FileInput = fopen(fileinput.c_str(), "r");
					fsetpos(FileInput, &filepos);
					fscanf(FileInput, "%d ", &numerocv);
					fgetpos(FileInput, &filepos);
					fclose(FileInput);
					BC[i] = new TCCCompresorVolumetrico(nmVolumetricCompressor, i, SpeciesModel, SpeciesNumber, GammaCalculation,
														ThereIsEGR);
					dynamic_cast<TCCCompresorVolumetrico*>(BC[i])->PutNumeroCV(numerocv);
					NumberOfVolumetricCompressors++;
					dynamic_cast<TCCCompresorVolumetrico*>(BC[i])->LeeCCCompresorVol(fileinput.c_str(), filepos, NumberOfPipes, Pipe,
							EngineBlock);
					dynamic_cast<TCCCompresorVolumetrico*>(BC[i])->IniciaMedias();
					break;
				case 14:
					BC[i] = new TCCExtremoInyeccion(nmInjectionEnd, i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					NumberOfInjectionEnds++;
#ifdef ParticulateFilter
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes,
											Pipe, NumberOfDPF, DPF);
#else
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes, Pipe, NumberOfDPF, NULL);
#endif
					break;
				case 15:
					BC[i] = new TCCEntradaCompresor(nmEntradaCompre, i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					NumTCCEntradaCompresor++;
#ifdef ParticulateFilter
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes,
											Pipe, NumberOfDPF, DPF);
#else
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes, Pipe, NumberOfDPF, NULL);
#endif
					break;
				case 16:
					BC[i] = new TCCUnionEntreDepositos(nmUnionEntreDepositos, i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					NumberOfConectionsBetweenPlenums++;
					dynamic_cast<TCCUnionEntreDepositos*>(BC[i])->LeeUEDepositos(fileinput.c_str(), filepos, Independent);
					break;
				case 17:
					BC[i] = new TCCCompresor(nmCompresor, i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					NumberOfCompressorsConnections++;
					dynamic_cast<TCCCompresor*>(BC[i])->LeeNumeroCompresor(fileinput.c_str(), filepos);
					break;
				case 18:
					BC[i] = new TCCPreVble(nmPresionVble, i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
					NumTCCPreVble++;
#ifdef ParticulateFilter
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes,
											Pipe, NumberOfDPF, DPF);
#else
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes, Pipe, NumberOfDPF, NULL);
#endif
					break;
				case 19:
					BC[i] = new TCFDConnection(nmCFDConnection, i, SpeciesModel, SpeciesNumber, GammaCalculation, ThereIsEGR);
#ifdef ParticulateFilter
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes,
											Pipe, NumberOfDPF, DPF);
#else
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes, Pipe, NumberOfDPF, NULL);
#endif
					break;
				case 20:
					BC[i] = new TCCExternalConnectionVol(nmExternalConnection, i, SpeciesModel, SpeciesNumber, GammaCalculation,
														 ThereIsEGR);
					NumTCCExternalConnection++;
#ifdef ParticulateFilter
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes,
											Pipe, NumberOfDPF, DPF);
#else
					BC[i]->ReadBoundaryData(fileinput.c_str(), filepos, NumberOfPipes, Pipe, NumberOfDPF, NULL);
#endif
					break;
				}
				if(BC[i]->getTipoCC() == nmIntakeValve || BC[i]->getTipoCC() == nmExhaustValve
				   || BC[i]->getTipoCC() == nmPipeToPlenumConnection || BC[i]->getTipoCC() == nmUnionEntreDepositos) {
					FileInput = fopen(fileinput.c_str(), "r");
					fsetpos(FileInput, &filepos);
					fscanf(FileInput, "%d ", &quevalv);
					fgetpos(FileInput, &filepos);
					fclose(FileInput);
					if(BC[i]->getTipoCC() == nmIntakeValve || BC[i]->getTipoCC() == nmExhaustValve) {
						dynamic_cast<TCCCilindro*>(BC[i])->AsignaTipoValvula(TypeOfValve, quevalv, numerovalvula);
					} else if(BC[i]->getTipoCC() == nmPipeToPlenumConnection) {
						dynamic_cast<TCCDeposito*>(BC[i])->AsignaTipoValvula(TypeOfValve, quevalv, numerovalvula);
						if(TypeOfValve[quevalv - 1]->getTypeOfValve() == nmLamina)
							NumberOfReedValves++;
						if(TypeOfValve[quevalv - 1]->getTypeOfValve() == nmWasteGate)
							NumberOfWasteGates++;
						if(TypeOfValve[quevalv - 1]->getTypeOfValve() == nmCalcExtern)
							NumberOfExternalCalculatedValves++;
						if(TypeOfValve[quevalv - 1]->getTypeOfValve() == nmMariposa)
							NumberOfButerflyValves++;
					} else if(BC[i]->getTipoCC() == nmUnionEntreDepositos) {
						dynamic_cast<TCCUnionEntreDepositos*>(BC[i])->AsignaTipoValvula(TypeOfValve, quevalv, numerovalvula);
						if(TypeOfValve[quevalv - 1]->getTypeOfValve() == nmLamina)
							NumberOfReedValves++;
						if(TypeOfValve[quevalv - 1]->getTypeOfValve() == nmWasteGate)
							NumberOfWasteGates++;
						if(TypeOfValve[quevalv - 1]->getTypeOfValve() == nmCalcExtern)
							NumberOfExternalCalculatedValves++;
						if(TypeOfValve[quevalv - 1]->getTypeOfValve() == nmMariposa)
							NumberOfButerflyValves++;
					}
					numerovalvula++;
				}

			}
		}
		FileInput = fopen(fileinput.c_str(), "r");
		fsetpos(FileInput, &filepos);

		if(NumberOfIntakeValves > 0) {
			BCIntakeValve = new TCondicionContorno*[NumberOfIntakeValves];
			contador = 0;
			for(int j = 0; j < NumberOfConnections; j++) {
				if(BC[j]->getTipoCC() == nmIntakeValve) {
					BCIntakeValve[contador] = dynamic_cast<TCCCilindro*>(BC[j]);
					contador++;
				}
			}
		}

		if(NumberOfExhaustValves > 0) {
			BCExhaustValve = new TCondicionContorno*[NumberOfExhaustValves];
			contador = 0;
			for(int j = 0; j < NumberOfConnections; j++) {
				if(BC[j]->getTipoCC() == nmExhaustValve) {
					BCExhaustValve[contador] = dynamic_cast<TCCCilindro*>(BC[j]);
					contador++;
				}
			}
		}

		if(NumberOfVolumetricCompressors > 0) {
			VolumetricCompressor = new TCCCompresorVolumetrico*[NumberOfVolumetricCompressors];
			contador = 0;
			for(int j = 0; j < NumberOfConnections; j++) {
				if(BC[j]->getTipoCC() == nmVolumetricCompressor) {
					if(contador + 1 == dynamic_cast<TCCCompresorVolumetrico*>(BC[j])->getNumeroCV()) {
						VolumetricCompressor[contador] = dynamic_cast<TCCCompresorVolumetrico*>(BC[j]);
						contador++;
					}
				}
			}
		}

		if(NumTCCPerdidaPresion > 0) {
			PerdidaPresion = new TCCPerdidadePresion*[NumTCCPerdidaPresion];
			contador = 0;
			for(int j = 0; j < NumberOfConnections; j++) {
				if(BC[j]->getTipoCC() == nmLinearPressureLoss || BC[j]->getTipoCC() == nmQuadraticPressureLoss) {
					PerdidaPresion[contador] = dynamic_cast<TCCPerdidadePresion*>(BC[j]);
					contador++;
				}
			}
		}

		if(nematlab > 0) {
			MatlabDischarge = new TCCDescargaExtremoAbierto*[nematlab];
			contador = 0;
			for(int j = 0; j < NumberOfConnections; j++) {
				if(BC[j]->getTipoCC() == nmOpenEndCalcExtern) {
					MatlabDischarge[contador] = dynamic_cast<TCCDescargaExtremoAbierto*>(BC[j]);
					contador++;
				}
			}
		}

		if(NumberOfInjectionEnds > 0) {
			InjectionEnd = new TCCExtremoInyeccion*[NumberOfInjectionEnds];
			contador = 0;
			for(int j = 0; j < NumberOfConnections; j++) {
				if(BC[j]->getTipoCC() == nmInjectionEnd) {
					InjectionEnd[contador] = dynamic_cast<TCCExtremoInyeccion*>(BC[j]);
					contador++;
				}
			}
		}

		if(NumberOfReedValves > 0) {
			BCReedValve = new TCondicionContorno*[NumberOfReedValves];
			contador = 0;
			for(int j = 0; j < NumberOfConnections; j++) {
				if(BC[j]->getTipoCC() == nmPipeToPlenumConnection) {
					if(dynamic_cast<TCCDeposito*>(BC[j])->getValvula()->getTypeOfValve() == nmLamina) {
						BCReedValve[contador] = dynamic_cast<TCCDeposito*>(BC[j]);
						contador++;
					}
				} else if(BC[j]->getTipoCC() == nmUnionEntreDepositos) {
					if(dynamic_cast<TCCUnionEntreDepositos*>(BC[j])->getValvula()->getTypeOfValve() == nmLamina) {
						BCReedValve[contador] = dynamic_cast<TCCUnionEntreDepositos*>(BC[j]);
						contador++;
					}
				}
			}
		}

		if(NumberOfWasteGates > 0) {
			BCWasteGate = new TCondicionContorno*[NumberOfWasteGates];
			contador = 0;
			for(int j = 0; j < NumberOfConnections; j++) {
				if(BC[j]->getTipoCC() == nmPipeToPlenumConnection) {
					if(dynamic_cast<TCCDeposito*>(BC[j])->getValvula()->getTypeOfValve() == nmWasteGate) {
						BCWasteGate[contador] = BC[j];
						contador++;
					}
				} else if(BC[j]->getTipoCC() == nmUnionEntreDepositos) {
					if(dynamic_cast<TCCUnionEntreDepositos*>(BC[j])->getValvula()->getTypeOfValve() == nmWasteGate) {
						BCWasteGate[contador] = BC[j];
						contador++;
					}
				}
			}
		}

		if(NumberOfExternalCalculatedValves > 0) {
			CCCalcExtern = new TTipoValvula*[NumberOfExternalCalculatedValves];
			contador = 0;
			for(int j = 0; j < NumberOfConnections; j++) {
				if(BC[j]->getTipoCC() == nmPipeToPlenumConnection) {
					if(dynamic_cast<TCCDeposito*>(BC[j])->getValvula()->getTypeOfValve() == nmCalcExtern) {
						CCCalcExtern[contador] = dynamic_cast<TCCDeposito*>(BC[j])->getValvula();
						contador++;
					}
				} else if(BC[j]->getTipoCC() == nmUnionEntreDepositos) {
					if(dynamic_cast<TCCUnionEntreDepositos*>(BC[j])->getValvula()->getTypeOfValve() == nmCalcExtern) {
						CCCalcExtern[contador] = dynamic_cast<TCCDeposito*>(BC[j])->getValvula();
						contador++;
					}
				}
			}
		}

		if(NumberOfButerflyValves > 0) {
			BCButerflyValve = new TTipoValvula*[NumberOfButerflyValves];
			contador = 0;
			for(int j = 0; j < NumberOfConnections; j++) {
				if(BC[j]->getTipoCC() == nmPipeToPlenumConnection) {
					if(dynamic_cast<TCCDeposito*>(BC[j])->getValvula()->getTypeOfValve() == nmMariposa) {
						BCButerflyValve[contador] = dynamic_cast<TCCDeposito*>(BC[j])->getValvula();
						contador++;
					}
				} else if(BC[j]->getTipoCC() == nmUnionEntreDepositos) {
					if(dynamic_cast<TCCUnionEntreDepositos*>(BC[j])->getValvula()->getTypeOfValve() == nmMariposa) {
						BCButerflyValve[contador] = dynamic_cast<TCCDeposito*>(BC[j])->getValvula();
						contador++;
					}
				}
			}
		}
		if(NumTCCExternalConnection > 0) {
			BCExtConnectionVol = new TCCExternalConnectionVol*[NumTCCExternalConnection];
			bool *Asigned;
			Asigned = new bool[NumTCCExternalConnection];

			for(int i = 0; i < NumTCCExternalConnection; ++i) {
				Asigned[i] = false;
			}

			for(int i = 0; i < NumberOfConnections; ++i) {
				if(BC[i]->getTipoCC() == nmExternalConnection) {
					int ID = dynamic_cast<TCCExternalConnectionVol*>(BC[i])->GetID() - 1;
					if(Asigned[ID] == true) {
						std::cout << "ERROR: There are two external connection with the same ID" << std::endl;
					}
					Asigned[ID] = true;

					if(ID >= NumTCCExternalConnection) {
						std::cout << "ERROR: Wrong ID for the external connection node " << i << std::endl;
					}
					BCExtConnectionVol[ID] = dynamic_cast<TCCExternalConnectionVol*>(BC[i]);
				}
			}
		}

	} catch(exception & N) {
		std::cout << " ERROR : ReadConnections " << std::endl;
		std::cout << " Tipo de error : " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TOpenWAM::ReadTurbochargerAxis() {
	try {
		fpos_t filepos;

		fscanf(FileInput, " %d ", &NumberOfAxis);
		if(NumberOfAxis != 0) {
			Axis = new TEjeTurbogrupo*[NumberOfAxis];
		}
		fgetpos(FileInput, &filepos);
		fclose(FileInput);

		if(NumberOfAxis != 0) {
			for(int i = 0; i < NumberOfAxis; ++i) {
				if(EngineBlock) {
					Axis[i] = new TEjeTurbogrupo(i, Engine[0]->getGeometria().NCilin);
				} else
					Axis[i] = new TEjeTurbogrupo(i, 0);
				Axis[i]->ReadTurbochargerAxis(fileinput.c_str(), filepos, Compressor, Turbine);
				Axis[i]->IniciaMedias();
			}
		}

		FileInput = fopen(fileinput.c_str(), "r");
		fsetpos(FileInput, &filepos);
	} catch(exception & N) {
		std::cout << " ERROR : ReadTurbochargerAxis " << std::endl;
		std::cout << " Tipo de error : " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TOpenWAM::ReadSensors() {
	fpos_t filepos;

	fscanf(FileInput, " %d ", &NumberOfSensors);

	fgetpos(FileInput, &filepos);
	fclose(FileInput);

	if(NumberOfSensors > 0) {
		Sensor = new TSensor*[NumberOfSensors];
		for(int i = 0; i < NumberOfSensors; i++) {
			Sensor[i] = new TSensor(i);
			Sensor[i]->ReadSensor(fileinput.c_str(), filepos);
			Sensor[i]->IniciaMedias();
		}
	}
	FileInput = fopen(fileinput.c_str(), "r");
	fsetpos(FileInput, &filepos);
}

void TOpenWAM::ReadControllers() {
	int ctrl = 0;
	fpos_t filepos;

	fscanf(FileInput, " %d ", &NumberOfControllers);
	if(NumberOfControllers > 0) {
		Controller = new TController*[NumberOfControllers];
		for(int i = 0; i < NumberOfControllers; i++) {
			fscanf(FileInput, " %d ", &ctrl);
			switch(ctrl) {
			case 1:
				Controller[i] = new TPIDController(i);
				break;
			case 2:
				Controller[i] = new TTable1D(i);
				break;
			case 3:
				Controller[i] = new TDecisor(i);
				break;
			case 4:
				Controller[i] = new TGain(i);
				break;
			}
			fgetpos(FileInput, &filepos);
			fclose(FileInput);
			Controller[i]->LeeController(fileinput.c_str(), filepos);
			Controller[i]->IniciaMedias();
			FileInput = fopen(fileinput.c_str(), "r");
			fsetpos(FileInput, &filepos);
		}
	}
}

void TOpenWAM::ReadOutput(char* FileName) {

	fpos_t filepos;
	fgetpos(FileInput, &filepos);
	fclose(FileInput);

	Output = new TOutputResults();

	// OUTPUT ->
#ifdef ParticulateFilter
	Output->ReadAverageResults(fileinput.c_str(), filepos, Pipe, EngineBlock, Engine,
							   Plenum, Axis, Compressor, Turbine, BC, DPF, VolumetricCompressor,
							   Venturi, Sensor, Controller, SimulationDuration, FileName);

	Output->ReadInstantaneousResults(fileinput.c_str(), filepos, Engine, Plenum, Pipe,
									 Venturi, BC, DPF, Axis, Compressor, Turbine, VolumetricCompressor,
									 BCWasteGate, NumberOfWasteGates, BCReedValve, NumberOfReedValves,
									 Sensor, Controller, FileName);
#else
	Output->ReadAverageResults(fileinput.c_str(), filepos, Pipe, EngineBlock, Engine, Plenum, Axis, Compressor, Turbine, BC,
							   NULL, VolumetricCompressor, Venturi, Sensor, Controller,
							   SimulationDuration, FileName);

	Output->ReadInstantaneousResults(fileinput.c_str(), filepos, Engine, Plenum, Pipe, Venturi, BC, NULL, Axis, Compressor,
									 Turbine, VolumetricCompressor, BCWasteGate, NumberOfWasteGates, BCReedValve,
									 NumberOfReedValves, Sensor, Controller, FileName);
#endif

	Output->ReadSpaceTimeResults(fileinput.c_str(), filepos, Pipe, Engine, Plenum);

	FileInput = fopen(FileName, "r");
	fsetpos(FileInput, &filepos);
}

void TOpenWAM::InitializeParameters() {

	RunningControl();

	InitializeRunningAngles();

	CalculateNewHeatPositions();

	for(int j = 0; j < NumberOfPipes; ++j) {
		Pipe[j]->IniciaVariablesFundamentalesTubo();
		Pipe[j]->InicializaCaracteristicas(BC);
		Pipe[j]->IniciaVariablesTransmisionCalor(BC, Engine, AmbientTemperature);
		Pipe[j]->CalculaCoeficientePeliculaInterior(BC);
		Pipe[j]->EstabilidadMetodoCalculo();
	}
#ifdef ParticulateFilter
	for(int i = 0; i < NumberOfDPF; i++) {
		DPF[i]->IniciaVariablesTransmisionCalor(AmbientTemperature);
		for(int j = 0; j < DPF[i]->getNumeroHacesCanales(); j++) {
			(DPF[i]->GetCanal(j, 0))->IniciaVariablesFundamentalesCanalDPF();
			(DPF[i]->GetCanal(j, 0))->InicializaCaracteristicas(BC);
			(DPF[i]->GetCanal(j, 0))->CalculaCoeficientePeliculaInterior();
			(DPF[i]->GetCanal(j, 1))->IniciaVariablesFundamentalesCanalDPF();
			(DPF[i]->GetCanal(j, 1))->InicializaCaracteristicas(BC);
			(DPF[i]->GetCanal(j, 1))->CalculaCoeficientePeliculaInterior();
			if(j == DPF[i]->getNumeroHacesCanales() - 1) {
				(DPF[i]->GetCanal(j, 0))->CalculaCoeficientePeliculaExterior
				(AmbientPressure);
				(DPF[i]->GetCanal(j, 1))->CalculaCoeficientePeliculaExterior
				(AmbientPressure);
			}
#ifdef ConcentricElement
			DPF[i]->CalculoResistenciaTC_First_Time(j, Pipe, Concentric);
#else
			DPF[i]->CalculoResistenciaTC_First_Time(j, Pipe, NULL);
#endif
		}
#ifdef ConcentricElement
		DPF[i]->InicializaDPF(NumberOfConcentrics, Concentric);
#else
		DPF[i]->InicializaDPF(NumberOfConcentrics, NULL);
#endif
		DPF[i]->CalculoEstabilidadDPF();
	}
#endif

	FirstIteration = true;

#ifdef ConcentricElement
	for(int i = 0; i < NumberOfConcentrics; i++) {
		Concentric[i]->CalculaResistenciasdePared(BC);
	}
#endif

	if(!Independent) {
		for(int i = 0; i < NumberOfConnections; i++) {
			BC[i]->TuboCalculandose(10000);
		}
	}

	AllocateVGTData();

	for(int i = 0; i < NumberOfPlenums; i++) {
		if(Plenum[i]->getTipoDeposito() == nmDepVolVble) {
			dynamic_cast<TDepVolVariable*>(Plenum[i])->IniciaVolumen(Theta);
		}
	}

	if(EngineBlock) {
		for(int i = 0; i < NumberOfPlenums; i++) {
			if(Plenum[i]->getTipoDeposito() == nmDepVolVble) {
				dynamic_cast<TDepVolVariable*>(Plenum[i])->UpdateSpeed(Engine[0]->getRegimen());
			}
		}

		Engine[0]->IniciaVarCilindro();
		Engine[0]->AsignacionTuboRendVol(Pipe);

		if((Engine[0]->getNumTuboRendVol() > NumberOfPipes) || Engine[0]->getNumTuboRendVol() <= 0) {
			printf(" ERROR : The intake pipe selectec for calculating \n ");
			printf(" the volumetric efficieny is not correct(pipe n. %d)\n ", Engine[0]->getTuboRendVol()->getNumeroTubo());
			throw Exception(" ERROR : The pipe selected for calculating the volumetric efficiency is not correct ");
		}
		if(ThereIsDLL) {
			if(EXTERN->getmodcomb()) {
				for(int i = 0; i < Engine[0]->getGeometria().NCilin; i++) {
					Engine[0]->GetCilindro(i)->PutHayDLL(true);
					Engine[0]->GetCilindro(i)->PutModComb(true);
				}
			} else {
				for(int i = 0; i < Engine[0]->getGeometria().NCilin; i++) {
					Engine[0]->GetCilindro(i)->PutHayDLL(true);
				}
			}

		}
		for(int i = 0; i < Engine[0]->getGeometria().NCilin; i++) {
			Engine[0]->GetCilindro(i)->DefineCombustion();
		}
	}
	for(int i = 0; i < NumberOfCompressors; i++) {
		Compressor[i]->Initialize();
	}

	for(int i = 0; i < NumberOfAxis; i++) {
		Axis[i]->InterpolaValoresMapa();
		Axis[i]->InitizlizeHTM(AmbientTemperature);
	}

	for(int i = 0; i < NumberOfConnections; i++) {
		if(BC[i]->getTipoCC() == nmPipeToPlenumConnection) {
			dynamic_cast<TCCDeposito*>(BC[i])->IniciaGamma();
		}
		if(BC[i]->getTipoCC() == nmPipeToPlenumConnection && EngineBlock) {
			TTipoValvula* val = dynamic_cast<TCCDeposito*>(BC[i])->getValvula();
			if(val->getTypeOfValve() == nmDiscoRotativo) {
				dynamic_cast<TDiscoRotativo*>(val)->PutAngle0(Engine[0]->getTheta());
			}
		}
	}

	ThetaIni = Theta;
}

void TOpenWAM::RunningControl() {
	try {
		double regimenficticio = 0.;

		if(!EngineBlock) {
			/* No hay motor pero Theta controla el funcionamiento del WAM. DE MOMENTO (pedro,paco) */
			Run.CycleDuration = 720;
			regimenficticio = 720. / 6 / SimulationDuration;
			thmax = 720.;
			grmax = 0.;
			agincr *= 6. * regimenficticio;

		} else {
			Run.CycleDuration = Engine[0]->getAngTotalCiclo();
			thmax = SimulationDuration * Engine[0]->getAngTotalCiclo();
			grmax = thmax - Engine[0]->getAngTotalCiclo();

		}
	} catch(exception & N) {
		std::cout << " ERROR : RunningControl " << std::endl;
		std::cout << " Tipo de error : " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TOpenWAM::InitializeRunningAngles() {
	try {

		if(!EngineBlock) {
			/* Inicio angulos de la ejecucion */
			Theta = 0.;
		} else {
			/* Inicio angulos de la ejecucion */
			Engine[0]->IniciaAnguloCalculo();
			Theta = Engine[0]->getTheta();
		}

		CrankAngle = Theta;
		Theta0 = 0.;

	} catch(exception & N) {
		std::cout << " ERROR : InitizalizeRunningAngles " << std::endl;
		std::cout << " Tipo de error : " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TOpenWAM::AllocateVGTData() {
	try {

		CountVGT = 0;
		int tgv = 0;
		int entr = 0;

		StatorTurbine = new TEstatorTurbina * *[NumberOfTurbines];
		RotorTurbine = new TRotorTurbina*[NumberOfTurbines];
		for(int i = 0; i < NumberOfTurbines; i++) {
			StatorTurbine[i] = new TEstatorTurbina*[Turbine[i]->getNumeroEntradas()];
			for(int j = 0; j < NumberOfConnections; j++) {
				if(BC[j]->getTipoCC() == nmPipeToPlenumConnection) {
					if(dynamic_cast<TCCDeposito*>(BC[j])->getValvula()->getTypeOfValve() == nmStator) {
						if(dynamic_cast<TEstatorTurbina*>(dynamic_cast<TCCDeposito*>(BC[j])->getValvula())->getNumeroTurbina() == i + 1) {
							entr = dynamic_cast<TEstatorTurbina*>(dynamic_cast<TCCDeposito*>(BC[j])->getValvula())->getNumeroEntrada() - 1;
							StatorTurbine[i][entr] = dynamic_cast<TEstatorTurbina*>(dynamic_cast<TCCDeposito*>(BC[j])->getValvula());
						}
					} else if(dynamic_cast<TCCDeposito*>(BC[j])->getValvula()->getTypeOfValve() == nmRotor) {
						if(dynamic_cast<TRotorTurbina*>(dynamic_cast<TCCDeposito*>(BC[j])->getValvula())->getNumeroTurbina() == i + 1) {
							RotorTurbine[i] = dynamic_cast<TRotorTurbina*>(dynamic_cast<TCCDeposito*>(BC[j])->getValvula());
							if(RotorTurbine[i]->getTipoRotor() == nmRotVariable) {
								CountVGT += 1;
							}
						}
					}
				}
			}
		}
		if(CountVGT != 0) {
			DatosTGV = new stDatosTGV[CountVGT];
			for(int i = 0; i < NumberOfTurbines; i++) {
				if(RotorTurbine[i]->getTipoRotor() == nmRotVariable) {
					// En su momento asignar al objeto turbina correspondiente el namero de TGV que le corresponde.Falta hacer.26-12-05
					DatosTGV[tgv].Entradas = Turbine[i]->getNumeroEntradas();
					DatosTGV[tgv].Turbine = i;
					DatosTGV[tgv].Estator = new TTipoValvula*[Turbine[i]->getNumeroEntradas()];
					DatosTGV[tgv].Rendimiento = new double[Turbine[i]->getNumeroEntradas()];
					for(int j = 0; j < Turbine[i]->getNumeroEntradas(); ++j) {
						if(Turbine[i]->GetCCEntrada(j)->getTipoCC() == nmPipeToPlenumConnection) {
							DatosTGV[tgv].Estator[j] = dynamic_cast<TCCDeposito*>(Turbine[i]->GetCCEntrada(j))->getValvula();
						} else if(Turbine[i]->GetCCEntrada(j)->getTipoCC() == nmUnionEntreDepositos) {
							DatosTGV[tgv].Estator[j] = dynamic_cast<TCCUnionEntreDepositos*>(Turbine[i]->GetCCEntrada(j))->getValvula();
						}
						DatosTGV[tgv].Rendimiento[j] = 0;
						if(Turbine[i]->GetCCSalida(0)->getTipoCC() == nmPipeToPlenumConnection) {
							DatosTGV[tgv].Rotor = dynamic_cast<TCCDeposito*>(Turbine[i]->GetCCSalida(0))->getValvula();
						} else
							printf(" ERROR : Tubine output %d is not a connection plenum - pipe.", i + 1);
						tgv++;
					}
					Turbine[i]->AllocateDatosTGV(DatosTGV);
				}
			}
		}
	} catch(exception & N) {
		stringstream err;
		std::cout << " ERROR : AllocateVGTData " << std::endl;
		std::cout << " Tipo de error : " << N.what() << std::endl;
		err << " ERROR : dimensionado_tgvwam " << N.what();
		throw Exception(err.str());
	}
}

void TOpenWAM::ConnectFlowElements() {

	if(EngineBlock) {

		for(int i = 0; i < Engine[0]->getGeometria().NCilin; i++) {
			dynamic_cast<TCilindro*>(Engine[0]->GetCilindro(i))->AsignacionCC(BC, NumberOfConnections);
		}
		for(int i = 0; i < NumberOfConnections; i++) {
			if(BC[i]->getTipoCC() == nmIntakeValve || BC[i]->getTipoCC() == nmExhaustValve) {
				dynamic_cast<TCCCilindro*>(BC[i])->AsignaCilindro(Engine[0]);
			}
		}
	}

	for(int i = 0; i < NumberOfConnections; i++) {
		if(BC[i]->getTipoCC() == nmCompresor) {
			dynamic_cast<TCCCompresor*>(BC[i])->AsignData(Plenum, NumberOfPipes, Pipe, BC, NumberOfConnections,
					AtmosphericComposition, Compressor, AmbientTemperature, AmbientPressure);
		}
	}

	for(int i = 0; i < NumberOfPlenums; i++) {
		Plenum[i]->AsignacionCC(BC, NumberOfConnections);
		if(Plenum[i]->getTipoDeposito() == nmTurbinaSimple || Plenum[i]->getTipoDeposito() == nmTurbinaTwin) {
			dynamic_cast<TTurbina*>(Plenum[i])->AsignaEntradaSalidaCC();
		} else if(Plenum[i]->getTipoDeposito() == nmVenturi) {
			dynamic_cast<TVenturi*>(Plenum[i])->AsignaEntradaSalidaLateralCC();
		} else if(Plenum[i]->getTipoDeposito() == nmUnionDireccional) {
			dynamic_cast<TUnionDireccional*>(Plenum[i])->AsignaCCUnionDireccional();
		}
	}

	for(int i = 0; i < NumberOfConnections; i++) {
		if(BC[i]->getTipoCC() == nmPipeToPlenumConnection) {
			dynamic_cast<TCCDeposito*>(BC[i])->AsignaDeposito(Plenum);
		} else if(BC[i]->getTipoCC() == nmUnionEntreDepositos) {
			dynamic_cast<TCCUnionEntreDepositos*>(BC[i])->AsignaDepositos(Plenum);
		}
	}

	for(int i = 0; i < NumberOfPipes; i++) {
		Pipe[i]->ComunicacionTubo_CC(BC);
	}

	for(int i = 0; i < NumberOfCompressors; i++) {
		Compressor[i]->AsignAcousticElements(Pipe, Plenum);
	}

	for(int i = 0; i < NumberOfTurbines; i++) {
		Turbine[i]->AsignAcousticElements(Pipe);
	}

#ifdef ParticulateFilter
	for(int i = 0; i < NumberOfDPF; i++) {
		for(int j = 0; j < DPF[i]->getNumeroHacesCanales(); j++) {
			DPF[i]->GetCanal(j, 0)->ComunicacionCanal_CC(BC);
			DPF[i]->GetCanal(j, 1)->ComunicacionCanal_CC(BC);
		}
		DPF[i]->ComunicacionTubos(BC, NumberOfConnections);
	}
#endif
}

void TOpenWAM::ConnectControlElements() {
	int ID = 0;

	// Asign elements to sensor
	for(int i = 0; i < NumberOfSensors; i++) {

		switch(Sensor[i]->ObjectSensed()) {
		case nmSensTubo:
			ID = Sensor[i]->ObjectID();
			Sensor[i]->AsignaObjeto((TObject*) Pipe[ID - 1]);
			break;
		case nmSensDeposito:
			ID = Sensor[i]->ObjectID();
			Sensor[i]->AsignaObjeto((TObject*) Plenum[ID - 1]);
			break;
		case nmSensMotor:
			Sensor[i]->AsignaObjeto((TObject*) Engine[0]);
			break;
		}
	}

	// Asign output sensor to controllers
	for(int i = 0; i < NumberOfControllers; ++i) {
		Controller[i]->AsignaObjetos(Sensor, Controller);
	}

	// Asign controllers to elments.
	for(int i = 0; i < NumberOfTurbines; i++) {
		Turbine[i]->AsignaRackController(Controller);
	}
	for(int i = 0; i < NumberOfButerflyValves; i++) {
		dynamic_cast<TMariposa*>(BCButerflyValve[i])->AsignaLevController(Controller);
	}
	for(int i = 0; i < NumberOfIntakeValves; i++) {
		if(dynamic_cast<TCCCilindro*>(BCIntakeValve[i])->getValvula()->getTypeOfValve() == nmValvula4T) {
			dynamic_cast<TValvula4T*>(dynamic_cast<TCCCilindro*>(BCIntakeValve[i])->getValvula())->AsignaLevController(Controller);
		}
	}
	for(int i = 0; i < NumberOfExhaustValves; i++) {
		if(dynamic_cast<TCCCilindro*>(BCExhaustValve[i])->getValvula()->getTypeOfValve() == nmValvula4T) {
			dynamic_cast<TValvula4T*>(dynamic_cast<TCCCilindro*>(BCExhaustValve[i])->getValvula())->AsignaLevController(Controller);
		}
	}

	if(EngineBlock) {
		Engine[0]->AsignRPMController(Controller);
		Engine[0]->AsignMfController(Controller);
	}

	if(NumberOfAxis > 0)
		Axis[0]->AsignaRPMController(Controller);
}

void TOpenWAM::InitialHeatTransferParameters() {

	CalculateNewHeatPositions();

	for(int i = 0; i < NumberOfPipes; i++) {
		Pipe[i]->IniciaVariablesTransmisionCalor(BC, Engine, AmbientTemperature);
		Pipe[i]->CalculaCoeficientePeliculaInterior(BC);
	}
#ifdef ParticulateFilter
	for(int i = 0; i < NumberOfDPF; i++) {
		DPF[i]->IniciaVariablesTransmisionCalor(AmbientTemperature);
		for(int j = 0; j < DPF[i]->getNumeroHacesCanales(); j++) {
			(DPF[i]->GetCanal(j, 0))->CalculaCoeficientePeliculaInterior();
			(DPF[i]->GetCanal(j, 1))->CalculaCoeficientePeliculaInterior();
			if(j == DPF[i]->getNumeroHacesCanales() - 1) {
				(DPF[i]->GetCanal(j, 0))->CalculaCoeficientePeliculaExterior
				(AmbientPressure);
				(DPF[i]->GetCanal(j, 1))->CalculaCoeficientePeliculaExterior
				(AmbientPressure);
			}
#ifdef ConcentricElement
			DPF[i]->CalculoResistenciaTC_First_Time(j, Pipe, Concentric);
#else
			DPF[i]->CalculoResistenciaTC_First_Time(j, Pipe, NULL);
#endif
		}

	}
#endif
}

void TOpenWAM::CalculateNewHeatPositions()

{
	try {
		int NodoOrigen = 0;
		int NodoFin = 0;
		bool Encontrado = false;

		for(int i = 0; i < NumberOfConnections; i++) {
			if(BC[i]->getTipoCC() == nmExhaustValve) {
				BC[i]->PutPosicionNodo(0.);
			} else {
				BC[i]->PutPosicionNodo(100000.);
			}
		}

		for(int i = 0; i < NumberOfExhaustValves; i++) {
			NodoOrigen = BCExhaustValve[i]->getNumeroCC();

			// PIPES
			for(int j = 0; j < NumberOfPipes; j++) {
				if(Pipe[j]->getNodoDer() == NodoOrigen) {
					NodoFin = Pipe[j]->getNodoIzq();
					Encontrado = true;
				} else if(Pipe[j]->getNodoIzq() == NodoOrigen) {
					NodoFin = Pipe[j]->getNodoDer();
					Encontrado = true;
				}
				if(Encontrado) {
					CalculateDistance(NodoOrigen, NodoFin, Pipe[j]->getLongitudTotal(), NumberOfPlenums, NumberOfPipes, NumberOfConnections,
									  Pipe, BC);
				}
				Encontrado = false;
			}

			// PLENUMS
			for(int i = 0; i < NumberOfPlenums; i++) {
				if(BC[NodoOrigen - 1]->getTipoCC() == nmPipeToPlenumConnection) {
					if(dynamic_cast<TCCDeposito*>(BC[NodoOrigen - 1])->getNumeroDeposito() == i + 1) {
						for(int k = 0; k < NumberOfConnections; k++) {
							if(BC[k]->getTipoCC() == nmPipeToPlenumConnection) {
								if(dynamic_cast<TCCDeposito*>(BC[NodoOrigen - 1])->getNumeroDeposito() == dynamic_cast<TCCDeposito*>
								   (BC[k])->getNumeroDeposito()) {
									NodoFin = k + 1;
									CalculateDistance(NodoOrigen, NodoFin, 0., NumberOfPlenums, NumberOfPipes, NumberOfConnections, Pipe, BC);
								}
							}
							if(BC[k]->getTipoCC() == nmUnionEntreDepositos) {
								if(dynamic_cast<TCCDeposito*>(BC[NodoOrigen - 1])->getNumeroDeposito() == dynamic_cast<TCCUnionEntreDepositos*>
								   (BC[k])->getNumeroDeposito1()
								   || dynamic_cast<TCCDeposito*>(BC[NodoOrigen - 1])->getNumeroDeposito() == dynamic_cast<TCCUnionEntreDepositos*>
								   (BC[k])->getNumeroDeposito2()) {
									NodoFin = k + 1;
									CalculateDistance(NodoOrigen, NodoFin, 0., NumberOfPlenums, NumberOfPipes, NumberOfConnections, Pipe, BC);
								}
							}
						}
					}
				}
				if(BC[NodoOrigen - 1]->getTipoCC() == nmUnionEntreDepositos) {
					if(dynamic_cast<TCCUnionEntreDepositos*>(BC[NodoOrigen - 1])->getNumeroDeposito1() == i + 1) {
						for(int k = 0; k < NumberOfConnections; k++) {
							if(BC[k]->getTipoCC() == nmPipeToPlenumConnection) {
								if(dynamic_cast<TCCUnionEntreDepositos*>(BC[NodoOrigen - 1])->getNumeroDeposito1() == dynamic_cast<TCCDeposito*>
								   (BC[k])->getNumeroDeposito()) {
									NodoFin = k + 1;
									CalculateDistance(NodoOrigen, NodoFin, 0., NumberOfPlenums, NumberOfPipes, NumberOfConnections, Pipe, BC);
								}
							}
							if(BC[k]->getTipoCC() == nmUnionEntreDepositos) {
								if(dynamic_cast<TCCUnionEntreDepositos*>(BC[NodoOrigen - 1])->getNumeroDeposito1() ==
								   dynamic_cast<TCCUnionEntreDepositos*>(BC[k])->getNumeroDeposito1()
								   || dynamic_cast<TCCUnionEntreDepositos*>(BC[NodoOrigen - 1])->getNumeroDeposito1() ==
								   dynamic_cast<TCCUnionEntreDepositos*>(BC[k])->getNumeroDeposito2()) {
									NodoFin = k + 1;
									CalculateDistance(NodoOrigen, NodoFin, 0., NumberOfPlenums, NumberOfPipes, NumberOfConnections, Pipe, BC);
								}
							}
						}
					} else if(dynamic_cast<TCCUnionEntreDepositos*>(BC[NodoOrigen - 1])->getNumeroDeposito2() == i + 1) {
						for(int k = 0; k < NumberOfConnections; k++) {
							if(BC[k]->getTipoCC() == nmPipeToPlenumConnection) {
								if(dynamic_cast<TCCUnionEntreDepositos*>(BC[NodoOrigen - 1])->getNumeroDeposito2() == dynamic_cast<TCCDeposito*>
								   (BC[k])->getNumeroDeposito()) {
									NodoFin = k + 1;
									CalculateDistance(NodoOrigen, NodoFin, 0., NumberOfPlenums, NumberOfPipes, NumberOfConnections, Pipe, BC);
								}
							}
							if(BC[k]->getTipoCC() == nmUnionEntreDepositos) {
								if(dynamic_cast<TCCUnionEntreDepositos*>(BC[NodoOrigen - 1])->getNumeroDeposito2() ==
								   dynamic_cast<TCCUnionEntreDepositos*>(BC[k])->getNumeroDeposito1()
								   || dynamic_cast<TCCUnionEntreDepositos*>(BC[NodoOrigen - 1])->getNumeroDeposito2() ==
								   dynamic_cast<TCCUnionEntreDepositos*>(BC[k])->getNumeroDeposito2()) {
									NodoFin = k + 1;
									CalculateDistance(NodoOrigen, NodoFin, 0., NumberOfPlenums, NumberOfPipes, NumberOfConnections, Pipe, BC);
								}
							}
						}
					}
				}
			}
		}

	} catch(exception & N) {
		std::cout << " ERROR : CalculateNewHeatPositions " << std::endl;
		std::cout << " Tipo de error : " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TOpenWAM::CalculateDistance(int LNodoOrigen, int LNodoFin, double LLongitud, int LNumberOfPlenums,
								 int LNumberOfPipes, int LNumberOfConnections, TTubo **LPipe, TCondicionContorno **LBC) {

	try {
		int NodoOrigen1 = 0, NodoFin1 = 0;
		double Long = 0.;

		int j = SelectPipe(LPipe, LNumberOfPipes, LNodoOrigen, LNodoFin);
		if(LPipe[j]->getTipoTransCal() == 3 || LLongitud == 0.)
			Long = 0.;
		else
			Long = LLongitud;

		if(LBC[LNodoFin - 1]->getPosicionNodo() > LBC[LNodoOrigen - 1]->getPosicionNodo() + Long) {
			LBC[LNodoFin - 1]->PutPosicionNodo(LBC[LNodoOrigen - 1]->getPosicionNodo() + Long);

			// PIPES
			for(int i = 0; i < LNumberOfPipes; i++) {
				if(LPipe[i]->getNodoDer() == LNodoFin && LPipe[i]->getNodoIzq() != LNodoOrigen) {
					NodoOrigen1 = LNodoFin;
					NodoFin1 = LPipe[i]->getNodoIzq();
					CalculateDistance(NodoOrigen1, NodoFin1, LPipe[i]->getLongitudTotal(), LNumberOfPlenums, LNumberOfPipes,
									  LNumberOfConnections, LPipe, LBC);
				} else if(LPipe[i]->getNodoIzq() == LNodoFin && LPipe[i]->getNodoDer() != LNodoOrigen) {
					NodoOrigen1 = LNodoFin;
					NodoFin1 = LPipe[i]->getNodoDer();
					CalculateDistance(NodoOrigen1, NodoFin1, LPipe[i]->getLongitudTotal(), LNumberOfPlenums, LNumberOfPipes,
									  LNumberOfConnections, LPipe, LBC);
				}
			}

			// PLENUMS
			for(int i = 0; i < LNumberOfPlenums; i++) {
				if(LBC[LNodoOrigen - 1]->getTipoCC() == nmPipeToPlenumConnection) {
					if(dynamic_cast<TCCDeposito*>(LBC[LNodoOrigen - 1])->getNumeroDeposito() == i + 1) {
						for(int k = 0; k < LNumberOfConnections; k++) {
							if(LBC[k]->getTipoCC() == nmPipeToPlenumConnection) {
								if(dynamic_cast<TCCDeposito*>(LBC[LNodoOrigen - 1])->getNumeroDeposito() == dynamic_cast<TCCDeposito*>
								   (LBC[k])->getNumeroDeposito()) {
									NodoOrigen1 = LNodoFin;
									NodoFin1 = k + 1;
									CalculateDistance(NodoOrigen1, NodoFin1, 0, LNumberOfPlenums, LNumberOfPipes, LNumberOfConnections, LPipe, LBC);
								}
							}
							if(LBC[k]->getTipoCC() == nmUnionEntreDepositos) {
								if(dynamic_cast<TCCDeposito*>(LBC[LNodoOrigen - 1])->getNumeroDeposito() == dynamic_cast<TCCUnionEntreDepositos*>
								   (LBC[k])->getNumeroDeposito1()
								   || dynamic_cast<TCCDeposito*>(LBC[LNodoOrigen - 1])->getNumeroDeposito() == dynamic_cast<TCCUnionEntreDepositos*>
								   (LBC[k])->getNumeroDeposito2()) {
									NodoOrigen1 = LNodoFin;
									NodoFin1 = k + 1;
									CalculateDistance(NodoOrigen1, NodoFin1, 0, LNumberOfPlenums, LNumberOfPipes, LNumberOfConnections, LPipe, LBC);
								}
							}
						}
					}

				}
				if(LBC[LNodoOrigen - 1]->getTipoCC() == nmUnionEntreDepositos) {
					if(dynamic_cast<TCCUnionEntreDepositos*>(LBC[LNodoOrigen - 1])->getNumeroDeposito1() == i + 1) {
						for(int k = 0; k < LNumberOfConnections; k++) {
							if(LBC[k]->getTipoCC() == nmPipeToPlenumConnection) {
								if(dynamic_cast<TCCUnionEntreDepositos*>(LBC[LNodoOrigen - 1])->getNumeroDeposito1() == dynamic_cast<TCCDeposito*>
								   (LBC[k])->getNumeroDeposito()) {
									NodoOrigen1 = LNodoFin;
									NodoFin1 = k + 1;
									CalculateDistance(NodoOrigen1, NodoFin1, 0, LNumberOfPlenums, LNumberOfPipes, LNumberOfConnections, LPipe, LBC);
								}
							}
							if(LBC[k]->getTipoCC() == nmUnionEntreDepositos) {
								if(dynamic_cast<TCCUnionEntreDepositos*>(LBC[LNodoOrigen - 1])->getNumeroDeposito1() ==
								   dynamic_cast<TCCUnionEntreDepositos*>(LBC[k])->getNumeroDeposito1()
								   || dynamic_cast<TCCUnionEntreDepositos*>(LBC[LNodoOrigen - 1])->getNumeroDeposito1() ==
								   dynamic_cast<TCCUnionEntreDepositos*>(LBC[k])->getNumeroDeposito2()) {
									NodoOrigen1 = LNodoFin;
									NodoFin1 = k + 1;
									CalculateDistance(NodoOrigen1, NodoFin1, 0, LNumberOfPlenums, LNumberOfPipes, LNumberOfConnections, LPipe, LBC);
								}
							}
						}
					} else if(dynamic_cast<TCCUnionEntreDepositos*>(LBC[LNodoOrigen - 1])->getNumeroDeposito2() == i + 1) {
						for(int k = 0; k < LNumberOfConnections; k++) {
							if(LBC[k]->getTipoCC() == nmPipeToPlenumConnection) {
								if(dynamic_cast<TCCUnionEntreDepositos*>(LBC[LNodoOrigen - 1])->getNumeroDeposito2() == dynamic_cast<TCCDeposito*>
								   (LBC[k])->getNumeroDeposito()) {
									NodoOrigen1 = LNodoFin;
									NodoFin1 = k + 1;
									CalculateDistance(NodoOrigen1, NodoFin1, 0, LNumberOfPlenums, LNumberOfPipes, LNumberOfConnections, LPipe, LBC);
								}
							}
							if(LBC[k]->getTipoCC() == nmUnionEntreDepositos) {
								if(dynamic_cast<TCCUnionEntreDepositos*>(LBC[LNodoOrigen - 1])->getNumeroDeposito2() ==
								   dynamic_cast<TCCUnionEntreDepositos*>(LBC[k])->getNumeroDeposito1()
								   || dynamic_cast<TCCUnionEntreDepositos*>(LBC[LNodoOrigen - 1])->getNumeroDeposito2() ==
								   dynamic_cast<TCCUnionEntreDepositos*>(LBC[k])->getNumeroDeposito2()) {
									NodoOrigen1 = LNodoFin;
									NodoFin1 = k + 1;
									CalculateDistance(NodoOrigen1, NodoFin1, 0, LNumberOfPlenums, LNumberOfPipes, LNumberOfConnections, LPipe, LBC);
								}
							}
						}
					}
				}

			}
		}
	} catch(exception & N) {
		std::cout << " ERROR : CalculateDistance " << std::endl;
		std::cout << " Tipo de error : " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

int TOpenWAM::SelectPipe(TTubo **LPipe, int LNumberOfPipes, int Lnodo1, int Lnodo2) {

	for(int i = 0; i < LNumberOfPipes; i++) {
		if((LPipe[i]->getNodoDer() == Lnodo1 && LPipe[i]->getNodoIzq() == Lnodo2) || (LPipe[i]->getNodoDer() == Lnodo2
				&& LPipe[i]->getNodoIzq() == Lnodo1)) {
			return i;
		}
	}
	return 0;

}

void TOpenWAM::Progress() {
	Percentage = (float)((Theta - ThetaIni) / (thmax - ThetaIni) * 100.);
	Increment = int(Percentage);
	if(Increment > Steps) {
		std::cout << std::endl;
		std::cout << "===================================" << std::endl;
		std::cout << "Progress : " << Percentage << "% " << std::endl;
		std::cout << "-----------------------------------" << std::endl;

#ifdef gestorcom
		if(GestorWAM != NULL)
			GestorWAM->ProcesoTranscurrido(Percentage);
#endif
		++Steps;
		ftime(&current);
		float tiempoac = (current.time - begining.time) * 1000 + (current.millitm - begining.millitm);
		float tiempoest = tiempoac * 100 / Percentage - tiempoac;
		int seg = int(tiempoest / 1000.);
		int min = int(seg / 60.);
		int hor = int(min / 60.);
		int mil = int(tiempoest) - seg * 1000;
		seg = seg - min * 60;
		min = min - hor * 60;
		printf("Time left to the end: %d:%02d:%02d,%03d \n", hor, min, seg, mil);
		std::cout << "===================================" << std::endl;
		std::cout << std::endl;
	}
}

void TOpenWAM::DetermineTimeStepIndependent() {

	MethodStability();

	StudyInflowOutflowMass();

	FixTimeStep();

	RecalculateStability();

}

void TOpenWAM::DetermineTimeStepCommon() {

	MethodStability();

	SearchMinimumTimeStep();

	if(JCurrent != -2) {
		TimeEndStep = Pipe[JCurrent]->getTime1();

		Run.TimeStep = Pipe[JCurrent]->getTime1() - Pipe[JCurrent]->getTime0();
	} else {
#ifdef ParticulateFilter
		TimeEndStep = DPF[JCurrentDPF]->getTime1DPF();
		Run.TimeStep = DPF[JCurrentDPF]->getTime1DPF() - DPF[JCurrentDPF]
					   ->getTime0DPF();
#endif
	}
	for(int j = 0; j < NumberOfPipes; j++) {
		Pipe[j]->AjustaPaso(TimeEndStep);
	}
#ifdef ParticulateFilter
	for(int j = 0; j < NumberOfDPF; j++) {
		DPF[j]->AjustaPaso(TimeEndStep);
	}
#endif
	StudyInflowOutflowMass();

	FixTimeStep();

}

void TOpenWAM::DetermineTimeStep(double t) {

	MethodStability();

	SearchMinimumTimeStep();

	TimeEndStep = Pipe[JCurrent]->getTime1();

	Run.TimeStep = Pipe[JCurrent]->getTime1() - Pipe[JCurrent]->getTime0();

	for(int j = 0; j < NumberOfPipes; j++) {
		Pipe[j]->AjustaPaso(TimeEndStep);
	}

	StudyInflowOutflowMass();

	FixTimeStepExternal(t - Pipe[JCurrent]->getTime0());

}

void TOpenWAM::MethodStability() {
	double TiempoFinPaso0 = 0.;

	TiempoFinPaso0 = TimeEndStep;

	for(int j = 0; j < NumberOfPipes; j++) {
		if(Pipe[j]->getTime1() >= TimeEndStep) {
			TimeEndStep = Pipe[j]->getTime1();
			PipeStepMax = true;
			JStepMax = j;
		}
		DPFStepMax = false;
		JStepMaxDPF = -1;
	}

#ifdef ParticulateFilter
	for(int j = 0; j < NumberOfDPF; j++) {
		if(DPF[j]->getTime1DPF() >= TimeEndStep) {
			TimeEndStep = DPF[j]->getTime1DPF();
			JStepMaxDPF = j;
			DPFStepMax = true;
			PipeStepMax = false;
			JStepMax = -1;
		}
	}
#endif

#ifdef ConcentricElement
	for(int j = 0; j < NumberOfConcentrics; j++) {
		if(Concentric[j]->GetHayDPF()) {
			if(Concentric[j]->GetTiempoDPF() > Concentric[j]->GetTiempo(0)) {
				Concentric[j]->PutTiempoDPF(Concentric[j]->GetTiempo(0));
			} else {
				Concentric[j]->PutTiempo(0, Concentric[j]->GetTiempoDPF());
			}
		} else {
			if(Concentric[j]->GetTiempo(0) > Concentric[j]->GetTiempo(1)) {
				Concentric[j]->PutTiempo(0, Concentric[j]->GetTiempo(1));
			} else {
				Concentric[j]->PutTiempo(1, Concentric[j]->GetTiempo(0));
			}
		}
	}
#endif

	Run.TimeStep = TimeEndStep - TiempoFinPaso0;
	FirstIteration = false;

}

void TOpenWAM::SearchMinimumTimeStep() {

	double TMinimo = 1e5;
	if(PipeStepMax) {
		TMinimo = Pipe[JStepMax]->getTime1();
		JCurrent = JStepMax;
		JCurrentDPF = -2;
		TimeMinPipe = true;
		TimeMinDPF = false;
	} else {
#ifdef ParticulateFilter
		TMinimo = DPF[JStepMaxDPF]->getTime1DPF();
		JCurrentDPF = JStepMaxDPF;
		JCurrent = -2;
		TimeMinPipe = false;
		TimeMinDPF = true;
#endif
	}

	for(int j = NumberOfPipes - 1; j >= 0; j--) {
		if(Pipe[j]->getTime1() <= TMinimo && j != JStepMax) {
			TMinimo = Pipe[j]->getTime1();
			JCurrent = j;
			JCurrentDPF = -2;
			TimeMinPipe = true;
			TimeMinDPF = false;
		}
	}
#ifdef ParticulateFilter
	for(int j = 0; j < NumberOfDPF; j++) {
		if(DPF[j]->getTime1DPF() <= TMinimo && j != JStepMaxDPF) {
			TMinimo = DPF[j]->getTime1DPF();
			JCurrent = -2;
			JCurrentDPF = j;
			TimeMinPipe = false;
			TimeMinDPF = true;
		}
	}
#endif
}

void TOpenWAM::StudyInflowOutflowMass() {
	try {
		double smadd = 0, cociente, gasta = 0, gaste = 0;
		int i = 0;
		bool masacil, masadep;
		double TMinimo = 0.;
		DeltaTPlenums = Run.TimeStep;
		cociente = 0.;

		do {
			masacil = true;
			masadep = true;

			i = 0;
			if(EngineBlock) {
				do {
					gasta = 0.;
					for(int j = 0; j < Engine[0]->GetCilindro(i)->getNumeroUnionesAdm(); ++j) {
						gasta += dynamic_cast<TCCCilindro*>(Engine[0]->GetCilindro(i)->GetCCValvulaAdm(j))->getMassflow();
					}
					gaste = 0.;
					for(int j = 0; j < Engine[0]->GetCilindro(i)->getNumeroUnionesEsc(); ++j) {
						gaste += dynamic_cast<TCCCilindro*>(Engine[0]->GetCilindro(i)->GetCCValvulaEsc(j))->getMassflow();
					}
					if((Engine[0]->GetCilindro(i)->getMasa() - DeltaTPlenums * (gasta + gaste)) * 1e3 <= 1e-4) {
						masacil = false;
					}
					++i;
				} while(masacil && i < Engine[0]->getGeometria().NCilin);
			}
			if(masacil && NumberOfPlenums != 0) {

				i = 0;
				do {
					SearchMinimumTime(i + 1, &TMinimo, Plenum);
					smadd = Plenum[i]->CriterioEstabilidad(TMinimo);
					for(int j = 0; j < NumberOfPlenums; j++) {
						Plenum[j]->PutRealizado(false);
					}
					if(smadd * 1e3 <= 1e-4) {
						masadep = false;
						printf(" WARNING : Plenum %d with critical conditions \n ", i + 1);
					}
					cociente = smadd / Plenum[i]->getMasa();
					++i;
				} while(masadep && i < NumberOfPlenums && cociente < 2.);
			}
			if(!masacil || !masadep || (cociente >= 2.)) {
				if(!masacil)
					printf(" WARNING : there is no mass in the cylinder \n ");
				if(!masadep)
					printf(" WARNING : there is no mass in the plenum \n ");
				if(cociente >= 2.)
					printf(" WARNING::Mass increment in a plenum too big \n ");
				std::cout << " Time step reduction in Theta = " << Theta << std::endl;
				std::cout << " Original time step : " << DeltaTPlenums << std::endl;
				DeltaTPlenums *= .95;
				std::cout << " New time step : " << DeltaTPlenums << std::endl;
				if(DeltaTPlenums <= 2e-7) {
					if(cociente >= 2)
						printf(" ERROR : plenum n. %d too small \n ", i + 1);
					printf(" ERROR : in time step \n ");
					throw Exception(" ERROR : in time step ");
				}
			}
		} while(!masacil || !masadep || (cociente >= 2.));
	} catch(exception & N) {
		std::cout << " ERROR : StudyInflowOutflowMass " << std::endl;
		std::cout << " Tipo de error : " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TOpenWAM::SearchMinimumTime(int LNumDepInicial, double *LTMinimo, TDeposito **LPlenum) {
	try {
		int NumDepSiguiente = 0;

		*LTMinimo = 100000.;

		if(!LPlenum[LNumDepInicial - 1]->getEstudioEstabilidadRealizado()) {
			if(LPlenum[LNumDepInicial - 1]->getNUniones() != 0) {
				for(int i = 0; i < LPlenum[LNumDepInicial - 1]->getNUniones(); i++) {
					if(LPlenum[LNumDepInicial - 1]->GetCCDeposito(i)->getUnionDPF()) {
#ifdef ParticulateFilter
						if(LPlenum[LNumDepInicial - 1]->GetCCDeposito(i)
						   ->GetTuboExtremo(0).DPF->getTime1DPF()
						   < *LTMinimo) {
							*LTMinimo = LPlenum[LNumDepInicial - 1]
										->GetCCDeposito(i)->GetTuboExtremo(0)
										.DPF->getTime1DPF();
						}
#endif
					} else {
						if(LPlenum[LNumDepInicial - 1]->GetCCDeposito(i)->GetTuboExtremo(0).Pipe->getTime1() < *LTMinimo) {
							*LTMinimo = LPlenum[LNumDepInicial - 1]->GetCCDeposito(i)->GetTuboExtremo(0).Pipe->getTime1();
						}
					}
				}
			}
		}
		LPlenum[LNumDepInicial - 1]->PutRealizado(true);

		if(LPlenum[LNumDepInicial - 1]->getNUnionesED() != 0) {
			for(int j = 0; j < LPlenum[LNumDepInicial - 1]->getNUnionesED(); j++) {
				if(LPlenum[LNumDepInicial - 1]->GetCCUnionEntreDep(j)->getTipoCC() == nmUnionEntreDepositos) {

					if(dynamic_cast<TCCUnionEntreDepositos*>(LPlenum[LNumDepInicial - 1]->GetCCUnionEntreDep(
								j))->getNumeroDeposito1() == LNumDepInicial) {
						NumDepSiguiente = dynamic_cast<TCCUnionEntreDepositos*>(LPlenum[LNumDepInicial - 1]->GetCCUnionEntreDep(
											  j))->getNumeroDeposito2();
					} else
						NumDepSiguiente = dynamic_cast<TCCUnionEntreDepositos*>(LPlenum[LNumDepInicial - 1]->GetCCUnionEntreDep(
											  j))->getNumeroDeposito1();

				}
				if(LPlenum[LNumDepInicial - 1]->GetCCUnionEntreDep(j)->getTipoCC() == nmCompresor) {
					if(dynamic_cast<TCCCompresor*>(LPlenum[LNumDepInicial - 1]->GetCCUnionEntreDep(j))->getNumeroDepositoRot() ==
					   LNumDepInicial) {
						NumDepSiguiente = dynamic_cast<TCCCompresor*>(LPlenum[LNumDepInicial - 1]->GetCCUnionEntreDep(
											  j))->getNumeroDepositoEst();
					} else
						NumDepSiguiente = dynamic_cast<TCCCompresor*>(LPlenum[LNumDepInicial - 1]->GetCCUnionEntreDep(
											  j))->getNumeroDepositoRot();
				}

				if(!LPlenum[NumDepSiguiente - 1]->getEstudioEstabilidadRealizado()) {
					SearchMinimumTimeGroup(LTMinimo, NumDepSiguiente, LPlenum);
				}
			}
		}

	} catch(exception & N) {
		std::cout << " ERROR : SearchMinimumTime : " << std::endl;
		std::cout << " Tipo de error : " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TOpenWAM::SearchMinimumTimeGroup(double *LTMinimo, int LNumDeposito, TDeposito **LPlenum) {
	try {
		int NumDepSiguiente = 0;

		if(LPlenum[LNumDeposito - 1]->getNUniones() != 0) {
			for(int i = 0; i < Plenum[LNumDeposito - 1]->getNUniones(); i++) {
				if(Plenum[LNumDeposito - 1]->GetCCDeposito(i)->getUnionDPF()) {
#ifdef ParticulateFilter
					if(Plenum[LNumDeposito - 1]->GetCCDeposito(i)
					   ->GetTuboExtremo(0).DPF->getTime1DPF() < *LTMinimo) {
						*LTMinimo = Plenum[LNumDeposito - 1]->GetCCDeposito(i)
									->GetTuboExtremo(0).DPF->getTime1DPF();
					}
#endif
				} else {
					if(Plenum[LNumDeposito - 1]->GetCCDeposito(i)->GetTuboExtremo(0).Pipe->getTime1() < *LTMinimo) {
						*LTMinimo = Plenum[LNumDeposito - 1]->GetCCDeposito(i)->GetTuboExtremo(0).Pipe->getTime1();
					}
				}
			}
		}

		Plenum[LNumDeposito - 1]->PutRealizado(true);

		if(Plenum[LNumDeposito - 1]->getNUnionesED() != 0) {
			for(int j = 0; j < Plenum[LNumDeposito - 1]->getNUnionesED(); j++) {
				if(Plenum[LNumDeposito - 1]->GetCCUnionEntreDep(j)->getTipoCC() == nmUnionEntreDepositos) {

					if(dynamic_cast<TCCUnionEntreDepositos*>(Plenum[LNumDeposito - 1]->GetCCUnionEntreDep(
								j))->getNumeroDeposito1() == LNumDeposito) {
						NumDepSiguiente = dynamic_cast<TCCUnionEntreDepositos*>(Plenum[LNumDeposito - 1]->GetCCUnionEntreDep(
											  j))->getNumeroDeposito2();
					} else
						NumDepSiguiente = dynamic_cast<TCCUnionEntreDepositos*>(Plenum[LNumDeposito - 1]->GetCCUnionEntreDep(
											  j))->getNumeroDeposito1();

				}
				if(Plenum[LNumDeposito - 1]->GetCCUnionEntreDep(j)->getTipoCC() == nmCompresor) {
					if(dynamic_cast<TCCCompresor*>(Plenum[LNumDeposito - 1]->GetCCUnionEntreDep(j))->getNumeroDepositoRot() ==
					   LNumDeposito) {
						NumDepSiguiente = dynamic_cast<TCCCompresor*>(Plenum[LNumDeposito - 1]->GetCCUnionEntreDep(j))->getNumeroDepositoEst();
					} else
						NumDepSiguiente = dynamic_cast<TCCCompresor*>(Plenum[LNumDeposito - 1]->GetCCUnionEntreDep(j))->getNumeroDepositoRot();
				}

				if(!Plenum[NumDepSiguiente - 1]->getEstudioEstabilidadRealizado()) {
					SearchMinimumTimeGroup(LTMinimo, NumDepSiguiente, Plenum);
				}
			}
		}

	} catch(exception & N) {
		std::cout << " ERROR : SearchMinimumTime en el deposito : " << std::endl;
		std::cout << " Tipo de error : " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TOpenWAM::FixTimeStep() {

	double TInicialPaso = 0., m = 0., RegimenFicticio = 0.;
	TInicialPaso = TimeEndStep - Run.TimeStep;
	if(DeltaTPlenums < Run.TimeStep) {
		TimeEndStep = TInicialPaso + DeltaTPlenums;
		for(int j = 0; j < NumberOfPipes; j++) {
			if(Pipe[j]->getTime1() > TimeEndStep) {
				Pipe[j]->AjustaPaso(TimeEndStep);
			}
			if(PipeStepMax) {
				JStepMax = j;
			}
		}
#ifdef ParticulateFilter
		for(int j = 0; j < NumberOfDPF; j++) {
			if(DPF[j]->getTime1DPF() > TimeEndStep) {
				DPF[j]->AjustaPaso(TimeEndStep);
			}
			if(DPFStepMax) {
				JStepMaxDPF = j;
			}
		}
#endif
		Run.TimeStep = DeltaTPlenums;
	}
	if(EngineBlock) {
		Run.AngleStep = Run.TimeStep * 6. * Engine[0]->getRegimen();
	} else {
		RegimenFicticio = 720. / 6. / SimulationDuration;
		Run.AngleStep = Run.TimeStep * 6. * RegimenFicticio;
	}
	Theta0 = Theta;
	Theta += Run.AngleStep;
	if(EngineBlock) {
		Engine[0]->PutTheta(Theta);
		m = floor(Theta / Engine[0]->getAngTotalCiclo());
		CrankAngle = Theta - m * Engine[0]->getAngTotalCiclo();
	} else {
		m = floor(Theta / 720.);
		CrankAngle = Theta - m * 720.;
	}
	AcumulatedTime += Run.TimeStep;

}

void TOpenWAM::FixTimeStepExternal(double deltat) {

	double TInicialPaso = 0., m = 0., RegimenFicticio = 0.;
	TInicialPaso = TimeEndStep - Run.TimeStep;

	if(deltat < Min(Run.TimeStep, DeltaTPlenums)) {
		Is_EndStep = true;
		Run.TimeStep = deltat;
		for(int j = 0; j < NumberOfPipes; j++) {
			Pipe[j]->AjustaPaso(TimeEndStep);
		}
	} else {
		Is_EndStep = false;
		if(DeltaTPlenums < Run.TimeStep) {
			Run.TimeStep = DeltaTPlenums;
			for(int j = 0; j < NumberOfPipes; j++) {
				Pipe[j]->AjustaPaso(TimeEndStep);
			}
		}

	}
	JStepMax = NumberOfPipes - 1;

	if(EngineBlock) {
		Run.AngleStep = Run.TimeStep * 6. * Engine[0]->getRegimen();
	} else {
		RegimenFicticio = 720. / 6. / SimulationDuration;
		Run.AngleStep = Run.TimeStep * 6. * RegimenFicticio;
	}
	Theta0 = Theta;
	Theta += Run.AngleStep;
	if(EngineBlock) {
		Engine[0]->PutTheta(Theta);
		m = floor(Theta / Engine[0]->getAngTotalCiclo());
		CrankAngle = Theta - m * Engine[0]->getAngTotalCiclo();
	} else {
		m = floor(Theta / 720.);
		CrankAngle = Theta - m * 720.;
	}
	AcumulatedTime += Run.TimeStep;

}

void TOpenWAM::RecalculateStability() {

	double i = 0.;

	if(PipeStepMax) {
		for(int j = 0; j < NumberOfPipes; j++) {
			if((Pipe[JStepMax]->getTime1() - Pipe[j]->getTime0()) < (Pipe[j]->getTime1() - Pipe[j]->getTime0())) {
				Pipe[j]->PutTime1(Pipe[JStepMax]->getTime1());
			} else {
				i = 0.;
				while((Pipe[j]->getTime1() - Pipe[j]->getTime0()) < (Pipe[JStepMax]->getTime1() - Pipe[JStepMax]->getTime0()) / pow(2.,
						i)) {
					i++;
				}
				Pipe[j]->PutTime1(Pipe[j]->getTime0() + (Pipe[JStepMax]->getTime1() - Pipe[JStepMax]->getTime0()) / pow(2., i));
			}
			Pipe[j]->PutDeltaTime(Pipe[j]->getTime1() - Pipe[j]->getTime0());
		}
#ifdef ParticulateFilter
		for(int j = 0; j < NumberOfDPF; j++) {
			if((Pipe[JStepMax]->getTime1() - DPF[j]->getTime0DPF()) <
			   (DPF[j]->getTime1DPF() - DPF[j]->getTime0DPF())) {
				DPF[j]->putTime1DPF(Pipe[JStepMax]->getTime1());
			} else {
				i = 0.;
				while((DPF[j]->getTime1DPF() - DPF[j]->getTime0DPF()) <
					  (Pipe[JStepMax]->getTime1() - Pipe[JStepMax]->getTime0())
					  / pow(2., i)) {
					i++;
				}
				DPF[j]->putTime1DPF
				(DPF[j]->getTime0DPF() + (Pipe[JStepMax]->getTime1()
										  - Pipe[JStepMax]->getTime0()) / pow(2., i));
			}
			DPF[j]->putDeltaTimeDPF(DPF[j]->getTime1DPF() - DPF[j]->getTime0DPF
									());
			for(int k = 0; k < DPF[j]->getNumeroHacesCanales(); k++) {
				(DPF[j]->GetCanal(k, 0))->putTime1(DPF[j]->getTime1DPF());
				(DPF[j]->GetCanal(k, 0))->putDeltaTime
				(DPF[j]->getDeltaTimeDPF());
				(DPF[j]->GetCanal(k, 1))->putTime1(DPF[j]->getTime1DPF());
				(DPF[j]->GetCanal(k, 1))->putDeltaTime
				(DPF[j]->getDeltaTimeDPF());
			}
		}
#endif
	} else if(DPFStepMax) {
#ifdef ParticulateFilter
		for(int j = 0; j < NumberOfPipes; j++) {
			if((DPF[JStepMaxDPF]->getTime1DPF() - Pipe[j]->getTime0()) <
			   (Pipe[j]->getTime1() - Pipe[j]->getTime0())) {
				Pipe[j]->PutTime1(DPF[JStepMaxDPF]->getTime1DPF());
			} else {
				i = 0.;
				while((Pipe[j]->getTime1() - Pipe[j]->getTime0()) <
					  (DPF[JStepMaxDPF]->getTime1DPF() - DPF[JStepMaxDPF]
					   ->getTime0DPF()) / pow(2., i)) {
					i++;
				}
				Pipe[j]->PutTime1(Pipe[j]->getTime0() +
								  (DPF[JStepMaxDPF]->getTime1DPF() - DPF[JStepMaxDPF]
								   ->getTime0DPF()) / pow(2., i));
			}
			Pipe[j]->PutDeltaTime(Pipe[j]->getTime1() - Pipe[j]->getTime0());
		}

		for(int j = 0; j < NumberOfDPF; j++) {
			if((DPF[JStepMaxDPF]->getTime1DPF() - DPF[j]->getTime0DPF()) <
			   (DPF[j]->getTime1DPF() - DPF[j]->getTime0DPF())) {
				DPF[j]->putTime1DPF(DPF[JStepMaxDPF]->getTime1DPF());
			} else {
				i = 0.;
				while((DPF[j]->getTime1DPF() - DPF[j]->getTime0DPF()) <
					  (DPF[JStepMaxDPF]->getTime1DPF() - DPF[JStepMaxDPF]
					   ->getTime0DPF()) / pow(2., i)) {
					i++;
				}
				DPF[j]->putTime1DPF(DPF[j]->getTime0DPF() +
									(DPF[JStepMaxDPF]->getTime1DPF() - DPF[JStepMaxDPF]
									 ->getTime0DPF()) / pow(2., i));
			}
			DPF[j]->putDeltaTimeDPF(DPF[j]->getTime1DPF() - DPF[j]->getTime0DPF
									());
			for(int k = 0; k < DPF[j]->getNumeroHacesCanales(); k++) {
				(DPF[j]->GetCanal(k, 0))->putTime1(DPF[j]->getTime1DPF());
				(DPF[j]->GetCanal(k, 0))->putDeltaTime
				(DPF[j]->getDeltaTimeDPF());
				(DPF[j]->GetCanal(k, 1))->putTime1(DPF[j]->getTime1DPF());
				(DPF[j]->GetCanal(k, 1))->putDeltaTime
				(DPF[j]->getDeltaTimeDPF());
			}
		}
#endif
	}

}

void TOpenWAM::RecalculateStabilitySolver() {

	double i = 0.;

	if(PipeStepMax) {
		if(TimeMinPipe) {
			if((Pipe[JStepMax]->getTime1() - Pipe[JCurrent]->getTime0()) < (Pipe[JCurrent]->getTime1() -
					Pipe[JCurrent]->getTime0())) {
				Pipe[JCurrent]->PutTime1(Pipe[JStepMax]->getTime1());
			} else {
				i = 0;
				while((Pipe[JCurrent]->getTime1() - Pipe[JCurrent]->getTime0()) < (Pipe[JStepMax]->getTime1() -
						Pipe[JStepMax]->getTime0()) / pow(2., i)) {
					i++;
				}
			}
			Pipe[JCurrent]->PutDeltaTime(Pipe[JCurrent]->getTime1() - Pipe[JCurrent]->getTime0());
		} else if(TimeMinDPF) {
#ifdef ParticulateFilter
			if((Pipe[JStepMax]->getTime1() - DPF[JCurrentDPF]->getTime0DPF())
			   < (DPF[JCurrentDPF]->getTime1DPF() - DPF[JCurrentDPF]
				  ->getTime0DPF())) {
				DPF[JCurrentDPF]->putTime1DPF(Pipe[JStepMax]->getTime1());
			} else {
				i = 0;
				while((DPF[JCurrentDPF]->getTime1DPF() - DPF[JCurrentDPF]
					   ->getTime0DPF()) < (Pipe[JStepMax]->getTime1()
										   - Pipe[JStepMax]->getTime0()) / pow(2., i)) {
					i++;
				}
				DPF[JCurrentDPF]->putTime1DPF
				(DPF[JCurrentDPF]->getTime0DPF() +
				 (Pipe[JStepMax]->getTime1() - Pipe[JStepMax]->getTime0())
				 / pow(2., i));
			}
			DPF[JCurrentDPF]->putDeltaTimeDPF
			(DPF[JCurrentDPF]->getTime1DPF() - DPF[JCurrentDPF]
			 ->getTime0DPF());
			for(int j = 0; j < DPF[JCurrentDPF]->getNumeroHacesCanales(); j++) {
				(DPF[JCurrentDPF]->GetCanal(j, 0))->putTime1
				(DPF[JCurrentDPF]->getTime1DPF());
				(DPF[JCurrentDPF]->GetCanal(j, 0))->putDeltaTime
				(DPF[JCurrentDPF]->getDeltaTimeDPF());
				(DPF[JCurrentDPF]->GetCanal(j, 1))->putTime1
				(DPF[JCurrentDPF]->getTime1DPF());
				(DPF[JCurrentDPF]->GetCanal(j, 1))->putDeltaTime
				(DPF[JCurrentDPF]->getDeltaTimeDPF());
			}
#endif
		}
	} else if(DPFStepMax) {
#ifdef ParticulateFilter
		if(TimeMinPipe) {
			if((DPF[JStepMaxDPF]->getTime1DPF() - Pipe[JCurrent]->getTime0())
			   < (Pipe[JCurrent]->getTime1() - Pipe[JCurrent]->getTime0())) {
				Pipe[JCurrent]->PutTime1(DPF[JStepMaxDPF]->getTime1DPF());
			} else {
				i = 0;
				while((Pipe[JCurrent]->getTime1() - Pipe[JCurrent]->getTime0()
					  ) < (DPF[JStepMaxDPF]->getTime1DPF() - DPF[JStepMaxDPF]
						   ->getTime0DPF()) / pow(2., i)) {
					i++;
				}
				Pipe[JCurrent]->PutTime1(Pipe[JCurrent]->getTime0() +
										 (DPF[JStepMaxDPF]->getTime1DPF() - DPF[JStepMaxDPF]
										  ->getTime0DPF()) / pow(2., i));
			}
			Pipe[JCurrent]->PutDeltaTime
			(Pipe[JCurrent]->getTime1() - Pipe[JCurrent]->getTime0());
		} else if(TimeMinDPF) {
			if((DPF[JStepMaxDPF]->getTime1DPF() - DPF[JCurrentDPF]->getTime0DPF
				()) < (DPF[JCurrentDPF]->getTime1DPF() - DPF[JCurrentDPF]
					   ->getTime0DPF())) {
				DPF[JCurrentDPF]->putTime1DPF(DPF[JStepMaxDPF]->getTime1DPF());
			} else {
				i = 0;
				while((DPF[JCurrentDPF]->getTime1DPF() - DPF[JCurrentDPF]
					   ->getTime0DPF()) < (DPF[JStepMaxDPF]->getTime1DPF()
										   - DPF[JStepMaxDPF]->getTime0DPF()) / pow(2., i)) {
					i++;
				}
				DPF[JCurrentDPF]->putTime1DPF
				(DPF[JCurrentDPF]->getTime0DPF() +
				 (DPF[JStepMaxDPF]->getTime1DPF() - DPF[JStepMaxDPF]
				  ->getTime0DPF()) / pow(2., i));
			}
			DPF[JCurrentDPF]->putDeltaTimeDPF
			(DPF[JCurrentDPF]->getTime1DPF() - DPF[JCurrentDPF]
			 ->getTime0DPF());
			for(int j = 0; j < DPF[JCurrentDPF]->getNumeroHacesCanales(); j++) {
				(DPF[JCurrentDPF]->GetCanal(j, 0))->putTime1
				(DPF[JCurrentDPF]->getTime1DPF());
				(DPF[JCurrentDPF]->GetCanal(j, 0))->putDeltaTime
				(DPF[JCurrentDPF]->getDeltaTimeDPF());
				(DPF[JCurrentDPF]->GetCanal(j, 1))->putTime1
				(DPF[JCurrentDPF]->getTime1DPF());
				(DPF[JCurrentDPF]->GetCanal(j, 1))->putDeltaTime
				(DPF[JCurrentDPF]->getDeltaTimeDPF());
			}
		}
#endif
	}

}

void TOpenWAM::InitializeOutput() {
	Output->DoSpaceTimeFiles(SpeciesNumber);

	Output->HeaderSpaceTimeResults(thmax, grmax, agincr, SpeciesNumber);

	Output->HeaderAverageResults(SpeciesName, EXTERN, ThereIsDLL);

	Output->CopyAverageResultsToFile(0);

	Output->PutInsPeriod(agincr);
	// <- OUTPUT

	if(ThereIsDLL) {
		EXTERN->InicializaMedias();
	}
#ifdef gestorcom
	if(GestorWAM != NULL)
		GestorWAM->CabeceraResMediosActualizada();
#endif
}

void TOpenWAM::CalculateFlowIndependent() {

	int OneDEnd = 0;

	do {
		// ! Search which pipe must be calculated
		SearchMinimumTimeStep();
		if(TimeMinPipe) {
			#pragma omp parallel for private(OneDEnd) num_threads(fi_num_threads)
			for(int i = -1; i < 2; i++) {
				if(i == -1) {
					// ! Solver for the flow in the pipe
					Pipe[JCurrent]->CalculaVariablesFundamentales();
				} else {
					// ! Calculation of the boundary conditions at the pipe end
					if(i == 0)
						OneDEnd = Pipe[JCurrent]->getNodoIzq();
					if(i == 1)
						OneDEnd = Pipe[JCurrent]->getNodoDer();

					BC[OneDEnd - 1]->CalculaCaracteristicas(Pipe[JCurrent]->getTime1());

					BC[OneDEnd - 1]->TuboCalculandose(JCurrent);

					if(BC[OneDEnd - 1]->getTipoCC() == nmVolumetricCompressor) {
						dynamic_cast<TCCCompresorVolumetrico*>(BC[OneDEnd - 1])->ObtencionValoresInstantaneos(ene);

					} else if(BC[OneDEnd - 1]->getTipoCC() == nmInjectionEnd) {
						dynamic_cast<TCCExtremoInyeccion*>(BC[OneDEnd - 1])->ObtencionValoresInstantaneos(Theta);

					} else if(BC[OneDEnd - 1]->getTipoCC() == nmCompresor) {

						dynamic_cast<TCCCompresor*>(BC[OneDEnd - 1])->ObtencionValoresInstantaneos(Theta, Pipe[JCurrent]->getTime1());
					}

					BC[OneDEnd - 1]->CalculaCondicionContorno(Pipe[JCurrent]->getTime1());

					SolveAdjacentElements(OneDEnd, Pipe[JCurrent]->getTime1());
				}
			}
			Pipe[JCurrent]->ActualizaValoresNuevos(BC);

			Pipe[JCurrent]->ActualizaPropiedadesGas();

			Pipe[JCurrent]->ReduccionFlujoSubsonico();

			Pipe[JCurrent]->CalculaCoeficientePeliculaInterior(BC);
			if(!EngineBlock) {
				Pipe[JCurrent]->CalculaCoeficientePeliculaExterior(Engine, AmbientPressure, AmbientTemperature);
				Pipe[JCurrent]->CalculaResistenciasdePared(BC);
			} else {
				if(Engine[0]->getCiclo() < 1) {
					Pipe[JCurrent]->CalculaCoeficientePeliculaExterior(Engine, AmbientPressure, AmbientTemperature);
					Pipe[JCurrent]->CalculaResistenciasdePared(BC);
				}
			}
			if(!Pipe[JCurrent]->getConcentrico()) {
				if(EngineBlock) {
					Pipe[JCurrent]->CalculaTemperaturaPared(Engine, Theta, CrankAngle, BC);
				} else {
					Pipe[JCurrent]->CalculaTemperaturaParedSinMotor(BC);
				}
			} else {
#ifdef ParticulateFilter
				for(int j = 0; j < NumberOfConcentrics; j++) {
					if(Pipe[JCurrent]->getNumeroTubo() == Concentric[j]
					   ->GetNumTuboExterno()) {
						Concentric[j]->CalculaTemperaturaPared(Engine, Theta,
															   BC);
					}
				}
#endif
			}
			Pipe[JCurrent]->CalculaResultadosMedios(Theta);
			Pipe[JCurrent]->EstabilidadMetodoCalculo();
			if(Pipe[JCurrent]->getTime0() < TimeEndStep) {
				RecalculateStabilitySolver();
			}
		} else if(TimeMinDPF) {
#ifdef ParticulateFilter
			for(int j = 0; j < DPF[JCurrentDPF]->getNumeroHacesCanales(); j++) {
				for(int k = 0; k < 2; k++) {
					(DPF[JCurrentDPF]->GetCanal(j, k))
					->CalculaVariablesFundamentales();
					// printf("%lf\n",DPF[JActualDPF]->Time1DPF);
					double IncrementoTiempo = DPF[JCurrentDPF]->getTime1DPF()
											  - DPF[JCurrentDPF]->getTime0DPF();
					(DPF[JCurrentDPF]->GetCanal(j, k))
					->CalculaCaracteristicasExtremos(BC,
													 IncrementoTiempo);

					/* Calculo de las Condiciones de Contorno en los extremos de los canales */
					for(int i = 0; i < 2; i++) {
						if(i == 0)
							OneDEnd = (DPF[JCurrentDPF]->GetCanal(j, k))
									  ->getNodoIzq();
						if(i == 1)
							OneDEnd = (DPF[JCurrentDPF]->GetCanal(j, k))
									  ->getNodoDer();

						if(OneDEnd != 0) {
							BC[OneDEnd - 1]->CalculaCondicionContorno
							(DPF[JCurrentDPF]->getTime1DPF());
						}
					}
					(DPF[JCurrentDPF]->GetCanal(j, k))->ActualizaValoresNuevos
					(BC);
					(DPF[JCurrentDPF]->GetCanal(j, k))->ActualizaPropiedadesGas
					();

					(DPF[JCurrentDPF]->GetCanal(j, k))->ReduccionFlujoSubsonico
					();

				}
				SolveAdjacentElements(OneDEnd, DPF[JCurrentDPF]->getTime1DPF());

				if(!EngineBlock) {
					if(DPF[JCurrentDPF]->getTime1DPF() < DPF[JCurrentDPF]
					   ->getDuracionCiclo()) {
						if(j == DPF[JCurrentDPF]->getNumeroHacesCanales() - 1) {
							(DPF[JCurrentDPF]->GetCanal(j, 0))
							->CalculaCoeficientePeliculaExterior
							(AmbientPressure);
						}
#ifdef ConcentricElement
						DPF[JCurrentDPF]->CalculoResistenciaTC(j, Pipe,
															   Concentric);
#else
						DPF[JCurrentDPF]->CalculoResistenciaTC(j, Pipe, NULL);
#endif
					}
				} else if(Engine[0]->getCiclo() < 1) {
					if(j == DPF[JCurrentDPF]->getNumeroHacesCanales() - 1) {
						(DPF[JCurrentDPF]->GetCanal(j, 0))
						->CalculaCoeficientePeliculaExterior
						(AmbientPressure);
					}
#ifdef ConcentricElement
					DPF[JCurrentDPF]->CalculoResistenciaTC(j, Pipe, Concentric);
#else
					DPF[JCurrentDPF]->CalculoResistenciaTC(j, Pipe, NULL);
#endif
				}
			}
#ifdef ConcentricElement
			DPF[JCurrentDPF]->CalculoTransmisionCalor(Engine, Theta, Pipe,
					Concentric);
#else
			DPF[JCurrentDPF]->CalculoTransmisionCalor(Engine, Theta, Pipe,
					NULL);
#endif
			DPF[JCurrentDPF]->CalculoSubmodelos();
			DPF[JCurrentDPF]->CalculaResultadosMedios(Theta);

			DPF[JCurrentDPF]->CalculoEstabilidadDPF();
			if(DPF[JCurrentDPF]->getTime0DPF() < TimeEndStep) {
				RecalculateStabilitySolver();
			}
#endif
		}
		for(int j = 0; j < NumberOfSensors; j++) {
			Sensor[j]->ActualizaMedida(Pipe[JStepMax]->getTime1());
		}
	} while(JCurrent != JStepMax && JCurrentDPF != JStepMaxDPF);
	// Loop end for all pipes.

	if(EngineBlock) {
		UpdateEngine();
		SolveRoadLoadModel();
	}

	for(int i = 0; i < NumberOfPlenums; i++) {
		if(SimulationType == nmTransitorioRegimen || SimulationType == nmTransitorioRegimenExterno
		   || !(Plenum[i]->getCalculadoPaso())) {
			if(PipeStepMax) {
				if(Plenum[i]->getTipoDeposito() == nmDepVolVble) {
					Plenum[i]->UpdateProperties0DModel(Pipe[JStepMax]->getTime0());
				} else {
					Plenum[i]->ActualizaTiempo(Pipe[JStepMax]->getTime0());
				}
			} else if(DPFStepMax) {
#ifdef ParticulateFilter
				if(Plenum[i]->getTipoDeposito() == nmDepVolVble) {
					Plenum[i]->UpdateProperties0DModel
					(DPF[JStepMaxDPF]->getTime0DPF());
				} else {
					Plenum[i]->ActualizaTiempo(DPF[JStepMaxDPF]->getTime0DPF());
				}
#endif
			}
		}
	}

	UpdateTurbocharger();
}

void TOpenWAM::SolveAdjacentElements(int OneDEnd, double TiempoActual) {
	try {
		int NumDepInicial = 0, NumeroCilindro = 0;
		int NumDepSiguiente = 0;
		bool CalculaElementosAdyacentes = false;
		bool CalculoCilindro = false;
		bool CalculoDeposito = false;
		int indiceUED = 0;

		if(BC[OneDEnd - 1]->getTipoCC() == nmPipeToPlenumConnection) {
			CalculaElementosAdyacentes = true;
			CalculoDeposito = true;
		} else if(BC[OneDEnd - 1]->getTipoCC() == nmCompresor) {
			if(dynamic_cast<TCCCompresor*>(BC[OneDEnd - 1])->getCompressor()->getModeloCompresor() == nmCompOriginal) {
				if(dynamic_cast<TCCCompresor*>(BC[OneDEnd - 1])->getEntradaCompresor() == nmPlenum) {
					CalculaElementosAdyacentes = true;
					CalculoDeposito = true;
				}
			}
		} else if(BC[OneDEnd - 1]->getTipoCC() == nmIntakeValve || BC[OneDEnd - 1]->getTipoCC() == nmExhaustValve) {
			CalculaElementosAdyacentes = true;
			CalculoCilindro = true;
		}

		if(CalculaElementosAdyacentes) {

			/* Se determina el namero de deposito o cilindro al que pertenece la condicion de contorno current */
			if(BC[OneDEnd - 1]->getTipoCC() == nmPipeToPlenumConnection) {
				NumDepInicial = dynamic_cast<TCCDeposito*>(BC[OneDEnd - 1])->getPlenum()->getNumeroDeposito();
				if(dynamic_cast<TCCDeposito*>(BC[OneDEnd - 1])->getValvula()->getCDTubVol() == 0
				   && dynamic_cast<TCCDeposito*>(BC[OneDEnd - 1])->getValvula()->getCDVolTub() == 0) {
					Plenum[NumDepInicial - 1]->PutCalculadoPaso(false);
				} else
					Plenum[NumDepInicial - 1]->PutCalculadoPaso(true);
			} else if(BC[OneDEnd - 1]->getTipoCC() == nmCompresor) {
				if(dynamic_cast<TCCCompresor*>(BC[OneDEnd - 1])->getCompressor()->getModeloCompresor() == nmCompOriginal) {
					if(dynamic_cast<TCCCompresor*>(BC[OneDEnd - 1])->getEntradaCompresor() == nmPlenum) {
						NumDepInicial = dynamic_cast<TCCCompresor*>(BC[OneDEnd - 1])->getPlenum()->getNumeroDeposito();
						Plenum[NumDepInicial - 1]->PutCalculadoPaso(true);
					}
				}
			} else if(BC[OneDEnd - 1]->getTipoCC() == nmIntakeValve || BC[OneDEnd - 1]->getTipoCC() == nmExhaustValve) {
				NumeroCilindro = dynamic_cast<TCCCilindro*>(BC[OneDEnd - 1])->getNumeroCilindro();
				if(dynamic_cast<TCCCilindro*>(BC[OneDEnd - 1])->getValvula()->getCDTubVol() == 0
				   && dynamic_cast<TCCCilindro*>(BC[OneDEnd - 1])->getValvula()->getCDVolTub() == 0) {
					Engine[0]->GetCilindro(NumeroCilindro - 1)->PutCalculadoPaso(false);
				} else
					Engine[0]->GetCilindro(NumeroCilindro - 1)->PutCalculadoPaso(true);
			}

			/* Si se trata de un deposito de volumen constante o variable se inicia la busqueda de condiciones de contorno de tipo
			 union entre depositos o compresores entre depositos que posea, para realizar su calculo. Esta busqueda  se realiza
			 de forma recursiva hasta que se llega a otro tubo. */
			if(CalculoDeposito) {
				if(Plenum[NumDepInicial - 1]->getCalculadoPaso()) {
					if(Plenum[NumDepInicial - 1]->getTipoDeposito() == nmDepVolCte
					   || Plenum[NumDepInicial - 1]->getTipoDeposito() == nmDepVolVble) {
						for(int i = 0; i < Plenum[NumDepInicial - 1]->getNUnionesED(); i++) {
							if(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(i)->getTipoCC() == nmCompresor) {
								if(dynamic_cast<TCCCompresor*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(i))->getCompressor()->getModeloCompresor()
								   == nmCompPlenums && dynamic_cast<TCCCompresor*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(
											   i))->getInstanteCalculo() != TiempoActual) {

									/* Aqui se determina el siguiente deposito en el que se continuara la busqueda */
									if(dynamic_cast<TCCCompresor*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(i))->getNumeroDepositoRot() ==
									   NumDepInicial) {
										NumDepSiguiente = dynamic_cast<TCCCompresor*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(i))->getNumeroDepositoEst();
									} else
										NumDepSiguiente = dynamic_cast<TCCCompresor*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(i))->getNumeroDepositoRot();

									Plenum[NumDepSiguiente - 1]->PutCalculadoPaso(true);

									/* Calculo de la condicion de contorno compresor entre depositos encontrada */
									dynamic_cast<TCCCompresor*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(i))->ObtencionValoresInstantaneos(Theta,
											TiempoActual);
									Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(i)->CalculaCondicionContorno(TiempoActual);
									dynamic_cast<TCCCompresor*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(i))->PutInstanteCalculo(TiempoActual);
								}
								/* Comienzo de la busqueda recursiva en el siguiente deposito */
								if(Plenum[NumDepSiguiente - 1]->getCalculadoPaso()) {
									SolveBranch(NumDepSiguiente, TiempoActual);
								}
							}
							if(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(i)->getTipoCC() == nmUnionEntreDepositos
							   && dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(
										   i))->getInstanteCalculo() != TiempoActual) {

								/* Aqui se determina el siguiente deposito en el que se continuara la busqueda */
								if(dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(
											i))->getNumeroDeposito1() == NumDepInicial) {
									NumDepSiguiente = dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(
														  i))->getNumeroDeposito2();
								} else
									NumDepSiguiente = dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(
														  i))->getNumeroDeposito1();

								for(int j = 0; j < Plenum[NumDepSiguiente - 1]->getNUnionesED(); j++) {
									if(Plenum[NumDepSiguiente - 1]->GetCCUnionEntreDep(j)->getTipoCC() == nmCompresor) {
										if(dynamic_cast<TCCCompresor*>(Plenum[NumDepSiguiente - 1]->GetCCUnionEntreDep(j))->getNumeroCC() ==
										   dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(
													   i))->getNumeroCC()) {
											indiceUED = j;
										}
									} else if(Plenum[NumDepSiguiente - 1]->GetCCUnionEntreDep(j)->getTipoCC() == nmUnionEntreDepositos) {
										if(dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDepSiguiente - 1]->GetCCUnionEntreDep(
													j))->getNumeroCC() == dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(
																i))->getNumeroCC()) {
											indiceUED = j;
										}
									}
								}
								if(dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDepSiguiente - 1]->GetCCUnionEntreDep(
											indiceUED))->getValvula()->getCDTubVol() == 0
								   && dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDepSiguiente - 1]->GetCCUnionEntreDep(
											   indiceUED))->getValvula()->getCDVolTub() == 0) {
									Plenum[NumDepSiguiente - 1]->PutCalculadoPaso(false);
								} else
									Plenum[NumDepSiguiente - 1]->PutCalculadoPaso(true);

								/* Calculo de la condicion de contorno union entre depositos encontrada */
								if(dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(
											i))->getValvula()->getTypeOfValve() != nmCDFijo) {
									if(EngineBlock) {
										dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(i))->CalculaCoeficientesDescarga(
											TiempoActual, Engine[0]->getMasaFuel(),
											Engine[0]->getRegimen());
									} else {
										dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(i))->CalculaCoeficientesDescarga(
											TiempoActual);
									}
								}

								dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(i))->PutInstanteCalculo(
									TiempoActual);
								Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(i)->CalculaCondicionContorno(TiempoActual);
								dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(i))->AcumulaResultadosMediosUED(
									TiempoActual);
								dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDepInicial - 1]->GetCCUnionEntreDep(i))->getValvula()->AcumulaCDMedio(
									TiempoActual);

								/* Comienzo de la busqueda recursiva en el siguiente deposito */
								if(Plenum[NumDepSiguiente - 1]->getCalculadoPaso()) {
									SolveBranch(NumDepSiguiente, TiempoActual);
								}
							}

						}
					}
				}
			}

			if(CalculoCilindro) {
				if(Engine[0]->GetCilindro(NumeroCilindro - 1)->getCalculadoPaso()) {
					Engine[0]->GetCilindro(NumeroCilindro - 1)->ActualizaPropiedades(TiempoActual);
					Engine[0]->GetCilindro(NumeroCilindro - 1)->CalculaVariablesResultados();
					Engine[0]->GetCilindro(NumeroCilindro - 1)->AcumulaResultadosMediosCilindro(TiempoActual);
					Engine[0]->AcumulaResultadosMediosBloqueMotor(TiempoActual, NumeroCilindro);
				}
			} else if(Plenum[NumDepInicial - 1]->getCalculadoPaso()) {
				/* Calculo del Deposito en el instante de calculo del Pipe Actual */
				Plenum[NumDepInicial - 1]->UpdateProperties0DModel(TiempoActual);
			}
		}

	} catch(exception & N) {
		std::cout << " ERROR : SolveAdjacentElements " << std::endl;
		std::cout << " Tipo de error : " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TOpenWAM::SolveBranch(int NumDeposito, double TiempoActual) {

	try {

		bool ExisteDepSiguiente = false;
		int NumDepSiguiente = 0;
		int indiceUED = 0;

		for(int i = 0; i < Plenum[NumDeposito - 1]->getNUnionesED(); i++) {
			if(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(i)->getTipoCC() == nmCompresor) {
				if(dynamic_cast<TCCCompresor*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(i))->getCompressor()->getModeloCompresor() ==
				   nmCompPlenums && dynamic_cast<TCCCompresor*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(
							   i))->getInstanteCalculo() != TiempoActual) {

					/* Aqui se determina el siguiente deposito en el que se continuara la busqueda */
					if(dynamic_cast<TCCCompresor*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(i))->getNumeroDepositoRot() == NumDeposito) {
						NumDepSiguiente = dynamic_cast<TCCCompresor*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(i))->getNumeroDepositoEst();
					} else
						NumDepSiguiente = dynamic_cast<TCCCompresor*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(i))->getNumeroDepositoRot();
					ExisteDepSiguiente = true;

					Plenum[NumDepSiguiente - 1]->PutCalculadoPaso(true);

					/* Calculo de la condicion de contorno compresor entre depositos encontrada */
					dynamic_cast<TCCCompresor*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(i))->ObtencionValoresInstantaneos(Theta,
							TiempoActual);
					Plenum[NumDeposito - 1]->GetCCUnionEntreDep(i)->CalculaCondicionContorno(TiempoActual);
					dynamic_cast<TCCCompresor*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(i))->PutInstanteCalculo(TiempoActual);
				}
			}
			if(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(i)->getTipoCC() == nmUnionEntreDepositos
			   && dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(
						   i))->getInstanteCalculo() != TiempoActual) {

				/* Aqui se determina el siguiente deposito en el que se continuara la busqueda */
				if(dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(
							i))->getNumeroDeposito1() == NumDeposito) {
					NumDepSiguiente = dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(
										  i))->getNumeroDeposito2();
				} else
					NumDepSiguiente = dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(
										  i))->getNumeroDeposito1();
				ExisteDepSiguiente = true;

				for(int j = 0; j < Plenum[NumDepSiguiente - 1]->getNUnionesED(); j++) {
					if(Plenum[NumDepSiguiente - 1]->GetCCUnionEntreDep(j)->getTipoCC() == nmCompresor) {
						if(dynamic_cast<TCCCompresor*>(Plenum[NumDepSiguiente - 1]->GetCCUnionEntreDep(j))->getNumeroCC() ==
						   dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(
									   i))->getNumeroCC()) {
							indiceUED = j;
						}
					} else if(Plenum[NumDepSiguiente - 1]->GetCCUnionEntreDep(j)->getTipoCC() == nmUnionEntreDepositos) {
						if(dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDepSiguiente - 1]->GetCCUnionEntreDep(
									j))->getNumeroCC() == dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(
												i))->getNumeroCC()) {
							indiceUED = j;
						}
					}
				}
				if(dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDepSiguiente - 1]->GetCCUnionEntreDep(
							indiceUED))->getValvula()->getCDTubVol() == 0
				   && dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDepSiguiente - 1]->GetCCUnionEntreDep(
							   indiceUED))->getValvula()->getCDVolTub() == 0) {
					Plenum[NumDepSiguiente - 1]->PutCalculadoPaso(false);
				} else
					Plenum[NumDepSiguiente - 1]->PutCalculadoPaso(true);

				/* Calculo de la condicion de contorno compresor entre depositos encontrada */
				if(dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(
							i))->getValvula()->getTypeOfValve() != nmCDFijo) {
					if(EngineBlock) {
						dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(i))->CalculaCoeficientesDescarga(
							TiempoActual, Engine[0]->getMasaFuel(),
							Engine[0]->getRegimen());
					} else {
						dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(i))->CalculaCoeficientesDescarga(
							TiempoActual);
					}
				}

				dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(i))->PutInstanteCalculo(TiempoActual);
				Plenum[NumDeposito - 1]->GetCCUnionEntreDep(i)->CalculaCondicionContorno(TiempoActual);
				dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(i))->AcumulaResultadosMediosUED(
					TiempoActual);
				dynamic_cast<TCCUnionEntreDepositos*>(Plenum[NumDeposito - 1]->GetCCUnionEntreDep(i))->getValvula()->AcumulaCDMedio(
					TiempoActual);

			}
			if(ExisteDepSiguiente) {
				if(Plenum[NumDepSiguiente - 1]->getCalculadoPaso()) {
					SolveBranch(NumDepSiguiente, TiempoActual);
				}
			}
		}

		Plenum[NumDeposito - 1]->UpdateProperties0DModel(TiempoActual);

	} catch(exception & N) {
		std::cout << " ERROR : SolveBranch " << std::endl;
		std::cout << " Tipo de error : " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TOpenWAM::UpdateEngine() {
	for(int i = 0; i < Engine[0]->getGeometria().NCilin; i++) {
		if(SimulationType == nmTransitorioRegimen || SimulationType == nmTransitorioRegimenExterno
		   || !(Engine[0]->GetCilindro(i)->getCalculadoPaso())) {
			if(PipeStepMax) {
				if(Engine[0]->GetCilindro(i)->getTiempoActual() < Pipe[JStepMax]->getTime0()) {
					Engine[0]->GetCilindro(i)->ActualizaPropiedades(Pipe[JStepMax]->getTime0());
					Engine[0]->GetCilindro(i)->CalculaVariablesResultados();
					Engine[0]->GetCilindro(i)->AcumulaResultadosMediosCilindro(Pipe[JStepMax]->getTime0());
					Engine[0]->AcumulaResultadosMediosBloqueMotor(Pipe[JStepMax]->getTime0(), i + 1);
					for(int j = 0; j < Engine[0]->GetCilindro(i)->getNumeroUnionesAdm(); j++) {
						dynamic_cast<TCCCilindro*>(Engine[0]->GetCilindro(i)->GetCCValvulaAdm(j))->ActualizaAnguloValvula(
							Pipe[JStepMax]->getTime0(), Engine[0]->getRegimen());
					}
					for(int j = 0; j < Engine[0]->GetCilindro(i)->getNumeroUnionesEsc(); j++) {
						dynamic_cast<TCCCilindro*>(Engine[0]->GetCilindro(i)->GetCCValvulaEsc(j))->ActualizaAnguloValvula(
							Pipe[JStepMax]->getTime0(), Engine[0]->getRegimen());
					}
				}
			} else if(DPFStepMax) {
#ifdef ParticulateFilter
				if(Engine[0]->GetCilindro(i)->getTiempoActual()
				   < DPF[JStepMaxDPF]->getTime0DPF()) {
					Engine[0]->GetCilindro(i)->ActualizaPropiedades
					(DPF[JStepMaxDPF]->getTime0DPF());
					Engine[0]->GetCilindro(i)->CalculaVariablesResultados();
					Engine[0]->GetCilindro(i)->AcumulaResultadosMediosCilindro
					(DPF[JStepMaxDPF]->getTime0DPF());
					Engine[0]->AcumulaResultadosMediosBloqueMotor
					(DPF[JStepMaxDPF]->getTime0DPF(), i + 1);
					for(int j = 0;
						j < Engine[0]->GetCilindro(i)->getNumeroUnionesAdm();
						j++) {
						dynamic_cast<TCCCilindro*>
						(Engine[0]->GetCilindro(i)->GetCCValvulaAdm(j))
						->ActualizaAnguloValvula
						(DPF[JStepMaxDPF]->getTime0DPF(),
						 Engine[0]->getRegimen());
					}
					for(int j = 0;
						j < Engine[0]->GetCilindro(i)->getNumeroUnionesEsc();
						j++) {
						dynamic_cast<TCCCilindro*>
						(Engine[0]->GetCilindro(i)->GetCCValvulaEsc(j))
						->ActualizaAnguloValvula
						(DPF[JStepMaxDPF]->getTime0DPF(),
						 Engine[0]->getRegimen());
					}
				}
#endif
			}
		} else if(Engine[0]->GetCilindro(i)->getCalculadoPaso()) {
			/* !
			 If in this time sted the cylinder has been calculate in CalculaElementosAdyacentes
			 this parameter is asigned as false
			 */Engine[0]->GetCilindro(i)->PutCalculadoPaso(false);
		}
	}
}

void TOpenWAM::SolveRoadLoadModel() {
	Engine[0]->ModeloDeVehiculo(AcumulatedTime);
	if(SimulationType == nmTransitorioRegimen || SimulationType == nmTransitorioRegimenExterno) {
		// ! Update speed of variable volume plenums connected to the engine.
		for(int i = 0; i < NumberOfPlenums; i++) {
			if(Plenum[i]->getTipoDeposito() == nmDepVolVble) {
				dynamic_cast<TDepVolVariable*>(Plenum[i])->UpdateSpeed(Engine[0]->getRegimen());
			}
		}

	}
}

void TOpenWAM::UpdateTurbocharger() {
	for(int i = 0; i < NumberOfAxis; i++) {
		Axis[i]->CalculaEjesTurbogrupo(Theta, SimulationType, AcumulatedTime, CrankAngle);
		Axis[i]->AcumulaResultadosMediosEje(AcumulatedTime);
	}
}

void TOpenWAM::CalculateFlowCommon() {

	for(int j = 0; j < NumberOfPipes; j++) {
		Pipe[j]->CalculaVariablesFundamentales();
		Pipe[j]->CalculaCaracteristicasExtremos(BC, Run.TimeStep);
	}
#ifdef ParticulateFilter
	for(int i = 0; i < NumberOfDPF; i++) {
		for(int j = 0; j < DPF[i]->getNumeroHacesCanales(); j++) {
			for(int k = 0; k < 2; k++) {
				(DPF[i]->GetCanal(j, k))->CalculaVariablesFundamentales();
				(DPF[i]->GetCanal(j, k))->CalculaCaracteristicasExtremos(BC,
						Run.TimeStep);
			}
		}
	}
#endif

	for(int i = 0; i < NumberOfConnections; i++) {
		if(BC[i]->getTipoCC() == nmVolumetricCompressor) {
			dynamic_cast<TCCCompresorVolumetrico*>(BC[i])->ObtencionValoresInstantaneos(ene);
		} else if(BC[i]->getTipoCC() == nmInjectionEnd) {
			dynamic_cast<TCCExtremoInyeccion*>(BC[i])->ObtencionValoresInstantaneos(Theta);
		} else if(BC[i]->getTipoCC() == nmCompresor) {
			dynamic_cast<TCCCompresor*>(BC[i])->ObtencionValoresInstantaneos(Theta, TimeEndStep);
		}
		BC[i]->CalculaCondicionContorno(TimeEndStep);

		if(BC[i]->getTipoCC() == nmUnionEntreDepositos) {
			dynamic_cast<TCCUnionEntreDepositos*>(BC[i])->AcumulaResultadosMediosUED(TimeEndStep);
			dynamic_cast<TCCUnionEntreDepositos*>(BC[i])->getValvula()->AcumulaCDMedio(TimeEndStep);
		}
	}

	for(int j = 0; j < NumberOfPipes; j++) {
		Pipe[j]->ActualizaValoresNuevos(BC);
		Pipe[j]->ActualizaPropiedadesGas();

		Pipe[j]->ReduccionFlujoSubsonico();

		Pipe[j]->CalculaCoeficientePeliculaInterior(BC);
		if(!EngineBlock) {
			Pipe[j]->CalculaCoeficientePeliculaExterior(Engine, AmbientPressure, AmbientTemperature);
			Pipe[j]->CalculaResistenciasdePared(BC);
		} else {
			if(Engine[0]->getCiclo() < 1) {
				Pipe[j]->CalculaCoeficientePeliculaExterior(Engine, AmbientPressure, AmbientTemperature);
				Pipe[j]->CalculaResistenciasdePared(BC);
			}
		}
		if(!Pipe[j]->getConcentrico()) {
			if(Engine != NULL) {
				Pipe[j]->CalculaTemperaturaPared(Engine, Theta, CrankAngle, BC);
			} else {
				Pipe[j]->CalculaTemperaturaParedSinMotor(BC);
			}
		} else {
#ifdef ConcentricElement
			for(int j = 0; j < NumberOfConcentrics; j++) {
				if(Pipe[j]->getNumeroTubo() == Concentric[j]->GetNumTuboExterno
				   ()) {
					Concentric[j]->CalculaTemperaturaPared(Engine, Theta, BC);
				}
			}
#endif
		}

		Pipe[j]->CalculaResultadosMedios(Theta);

		Pipe[j]->EstabilidadMetodoCalculo();
	}
#ifdef ParticulateFilter
	for(int i = 0; i < NumberOfDPF; i++) {
		for(int j = 0; j < DPF[i]->getNumeroHacesCanales(); j++) {
			for(int k = 0; k < 2; k++) {
				(DPF[i]->GetCanal(j, k))->ActualizaValoresNuevos(BC);
				(DPF[i]->GetCanal(j, k))->ActualizaPropiedadesGas();
				(DPF[i]->GetCanal(j, k))->ReduccionFlujoSubsonico();
			}
			if(!EngineBlock) {
				if(j == DPF[i]->getNumeroHacesCanales() - 1) {
					(DPF[i]->GetCanal(j, 0))->CalculaCoeficientePeliculaExterior
					(AmbientPressure);
				}
				DPF[i]->CalculoResistenciaTC(j, Pipe, Concentric);
			} else if(Engine[0]->getCiclo() < 1) {
				if(j == DPF[i]->getNumeroHacesCanales() - 1) {
					(DPF[i]->GetCanal(j, 0))->CalculaCoeficientePeliculaExterior
					(AmbientPressure);
				}
				DPF[i]->CalculoResistenciaTC(j, Pipe, Concentric);
			}
		}
		DPF[i]->CalculoTransmisionCalor(Engine, Theta, Pipe, Concentric);
		DPF[i]->CalculoSubmodelos();
		DPF[i]->CalculaResultadosMedios(Theta);

		DPF[i]->CalculoEstabilidadDPF();
	}
#endif
	for(int j = 0; j < NumberOfSensors; j++) {
		Sensor[j]->ActualizaMedida(Pipe[JStepMax]->getTime1());
	}

	if(EngineBlock) {
		UpdateEngine();
		SolveRoadLoadModel();
	}

	for(int i = 0; i < NumberOfPlenums; i++) {
		Plenum[i]->UpdateProperties0DModel(TimeEndStep);
	}

	UpdateTurbocharger();

	if(ThereIsDLL) {

		comunica_wam_dll();

		Actuadores();

	} /* fin haydll */
}

void TOpenWAM::ManageOutput() {
	if(Engine != NULL) {
		Output->WriteInstantaneous(EngineBlock, CrankAngle, Run.AngleStep, Engine[0], SimulationDuration);
	} else {
		Output->WriteInstantaneous(EngineBlock, CrankAngle, Run.AngleStep,
								   NULL, SimulationDuration);
	}

	Output->CopyInstananeousResultsToFile(0);

	Output->HeaderInstantaneousResults(EXTERN, ThereIsDLL, EngineBlock, SpeciesName);

#ifdef gestorcom
	if(GestorWAM != NULL)
		GestorWAM->CabeceraResInstantActualizada();
	if(GestorWAM != NULL)
		GestorWAM->FichResInstantActualizado();
#endif
	// OUTPUT ->
	Output->PlotControl(Theta0, Theta, Run.CycleDuration);

	if(EngineBlock) {

		Output->OutputInstantaneousResults(EXTERN, ThereIsDLL, EngineBlock, Theta, Engine[0], AcumulatedTime);

		Output->WriteSpaceTime(EngineBlock, Engine[0], Run.CycleDuration);

		Output->PrintSpaceTimeResults(EngineBlock, Theta, Run.CycleDuration, Engine, SpeciesNumber);

		if(CrankAngle - Run.AngleStep <= 0. && Theta >= 750.) {
			Output->OutputAverageResults(AcumulatedTime, EXTERN, ThereIsDLL);

			Output->CopyAverageResultsToFile(1);

#ifdef gestorcom
			if(GestorWAM != NULL)
				GestorWAM->FichResMediosActualizado();
#endif

			Engine[0]->PrestacionesMotor();

			for(int i = 0; i < Engine[0]->getGeometria().NCilin; i++) {
				Engine[0]->GetCilindro(i)->SalidaGeneralCilindros();
			}

			printf(" \n INFO : == CYCLE N. %d == \n \n ", Engine[0]->getCiclo() + 1);

		}
	} else {
		Output->OutputInstantaneousResults(EXTERN, ThereIsDLL, EngineBlock, Theta, NULL, AcumulatedTime);

		Output->WriteSpaceTime(EngineBlock, NULL, Run.CycleDuration);

		Output->PrintSpaceTimeResults(EngineBlock, Theta, Run.CycleDuration,
									  NULL, SpeciesNumber);
	}

}

void TOpenWAM::GeneralOutput() {

	Output->CopyInstananeousResultsToFile(1);

	if(EngineBlock) {
		Engine[0]->PrestacionesMotor();
		for(int i = 0; i < Engine[0]->getGeometria().NCilin; i++) {
			Engine[0]->GetCilindro(i)->SalidaGeneralCilindros();
		}
	}
	for(int i = 0; i < NumberOfPlenums; i++) {
		Plenum[i]->SalidaGeneralDep(SpeciesName);
	}
	for(int i = 0; i < NumberOfPipes; i++) {
		Pipe[i]->SalidaGeneralTubos(SpeciesName);
	}
}

bool TOpenWAM::CalculationEnd() {
	return Output->GetFControlAngle1() > thmax;
}

void TOpenWAM::ProgressBegin() {
	char* tzcharstring = new char[tzstr.length() + 1];
	std::strcpy(tzcharstring, tzstr.c_str());
	putenv(tzcharstring);

#ifdef gestorcom
	if(GestorWAM != NULL)
		GestorWAM->NuevoMensaje("Calculating main loop...");
#endif
	// tzset();
	ftime(&begining);
	printf(" Seconds since 1 / 1 / 1970 GMT : % ld \n ", begining.time);
	printf(" Thousandths of a second : %d \n ", begining.millitm);
}

void TOpenWAM::ProgressEnd() {

	ftime(&final);
	float tiempotot = (final.time - begining.time) * 1000 + (final.millitm - begining.millitm);

	int seg = int(tiempotot / 1000.);
	int min = int(seg / 60.);
	int hor = int(min / 60.);
	int mil = int(tiempotot) - seg * 1000;
	seg = seg - min * 60;
	min = min - hor * 60;
	std::cout << std::endl;
	std::cout << "===================================" << std::endl;
	printf("Time consumed: %d:%02d:%02d,%03d \n", hor, min, seg, mil);
	std::cout << "===================================" << std::endl;
	std::cout << std::endl;
}

void TOpenWAM::NewEngineCycle() {

	if(EngineBlock) {
		if(CrankAngle - Run.AngleStep <= 0. && Theta >= Engine[0]->getAngTotalCiclo()) {

			Engine[0]->PutCiclo(Engine[0]->getCiclo() + 1);
			for(int i = 0; i < NumberOfPipes; i++) {
				Pipe[i]->CalculaCoeficientePeliculaExterior(Engine, AmbientPressure, AmbientTemperature);
				Pipe[i]->CalculaResistenciasdePared(BC);
			}
		}
	}
	// Calculo de las propiedades de transmision de calor en el filtro de particulas cada paso de integracion global
#ifdef ParticulateFilter
	for(int i = 0; i < NumberOfDPF; i++) {
		if(DPF[i]->getCicloDPF() > 1) {
			for(int j = 0; j < DPF[i]->getNumeroHacesCanales(); j++) {
				(DPF[i]->GetCanal(j, 0))->CalculaCoeficientePeliculaInterior();
				(DPF[i]->GetCanal(j, 1))->CalculaCoeficientePeliculaInterior();
				if(j == DPF[i]->getNumeroHacesCanales() - 1) {
					(DPF[i]->GetCanal(j, 0))->CalculaCoeficientePeliculaExterior
					(AmbientPressure);
					(DPF[i]->GetCanal(j, 1))->CalculaCoeficientePeliculaExterior
					(AmbientPressure);
				}
				DPF[i]->CalculoResistenciaTC(j, Pipe, Concentric);
			}
		}
	}
#endif
}

void TOpenWAM::UpdateExternalBoundary(int i, double U0, double U1, double T0, double T1, double P0, double P1,
									  double t) {

	BCExtConnection[i]->UpdateCurrentExternalProperties(U0, U1, T0, T1, P0, P1, t);

}

void TOpenWAM::UpdateExternalBoundary(int i, double U0, double T0, double P0, double t) {

	BCExtConnectionVol[i - 1]->UpdateCurrentExternalProperties(U0, T0, P0, t);

}

void TOpenWAM::InitiateExternalBoundary(int i, double D0, double D1, double dX) {

	BCExtConnection[i]->AsignGeometricalData(D0, D1, dX);

}

void TOpenWAM::InitiateExternalBoundary(int i, double D0, double dX) {

	BCExtConnectionVol[i]->AsignGeometricalData(D0, dX);

}

void TOpenWAM::LoadNewData(int i, double* p, double* T, double* u) {

	BCExtConnectionVol[i]->LoadNewData(p, T, u);
}

bool TOpenWAM::GetIs_EndStep() {
	return Is_EndStep;
}

void TOpenWAM::comunica_wam_dll() {

	if(Pipe != NULL)
		EXTERN->Calculo_Sensores_Tubos(Pipe, Run.TimeStep);
	if(Plenum != NULL)
		EXTERN->Calculo_Sensores_Deposito(Plenum, Run.TimeStep);
	if(Axis != NULL)
		EXTERN->Calculo_Sensores_TG(Run.TimeStep, Axis);
	if(Turbine != NULL)
		EXTERN->Calculo_Sensores_Turbina(Run.TimeStep, Turbine);
	if(Engine != NULL)
		EXTERN->Calculo_Sensores_Cilindro(Run.TimeStep, Engine);
	if(Venturi != NULL)
		EXTERN->Calculo_Sensores_Venturi(Run.TimeStep, Venturi);
	if(Engine != NULL)
		EXTERN->Calculo_Sensores_Motor(Run.TimeStep, Engine, AcumulatedTime);
	EXTERN->LlamadaECU(Run.TimeStep, Engine);

	if(EXTERN->getConvergencia() && ConvergenceFirstTime == false) {
		ModificacionControlEjecucion();
		ConvergenceFirstTime = true;
	}

}

void TOpenWAM::ModificacionControlEjecucion() {
	int nuevaduracionejecucion = 0;

	if(!EngineBlock) {
		if(Theta < 700.) {
			thmax = Theta + 20.;
		}
		grmax = 0.;
	} else {
		nuevaduracionejecucion = (Engine[0]->getCiclo() + 1) + 10;
		thmax = nuevaduracionejecucion * Engine[0]->getAngTotalCiclo();
		grmax = thmax - Engine[0]->getAngTotalCiclo();
	}

}

void TOpenWAM::Actuadores()

{
	int compo = 0, contador = 0;
	/* Asignacion de actuadores a variables de WAM */

	/* Extremos matlab */
	if(nematlab != 0) {
		for(int i = 0; i < nematlab; ++i) {
			MatlabDischarge[i]->PutPresion(EXTERN->GetOutput_dll(compo));
			MatlabDischarge[i]->PutTemperatura(__units::degCToK(EXTERN->GetOutput_dll(compo + 1)));
			compo += 2;
		}
	}

	/* Coeficientes de descarga */
	if(controlvalv == 1) {
		for(int i = 0; i <= NumberOfExternalCalculatedValves - 1; ++i) {
			dynamic_cast<TCDExterno*>(CCCalcExtern[i])->PutCDEntMatlab(EXTERN->GetOutput_dll(compo));
			dynamic_cast<TCDExterno*>(CCCalcExtern[i])->PutCDSalMatlab(EXTERN->GetOutput_dll(compo + 1));
			dynamic_cast<TCDExterno*>(CCCalcExtern[i])->PutCTorMatlab(EXTERN->GetOutput_dll(compo + 2));
			compo += 3;
		}
	}

	/* Gasto de combustible */
	if(EXTERN->getcontrolmfcomb()) {
		for(int i = 0; i < Engine[0]->getGeometria().NCilin; ++i) {
			if(Engine[0]->GetCilindro(i)->getAnguloActual() > Engine[0]->GetCilindro(i)->getDistribucion().CA
			   && Engine[0]->GetCilindro(i)->getAnguloAnterior() <= Engine[0]->GetCilindro(i)->getDistribucion().CA) {
				Engine[0]->GetCilindro(i)->PutMasaFuel(EXTERN->GetOutput_dll(compo));
				/* Actuador de fuel del cilindro en cuestion */
			}
			compo += 1; /* Hay que hacer un bucle */
		}
	}

	/* Inyeccion */
	if(EXTERN->getcontroliny()) {
		for(int i = 0; i < Engine[0]->getGeometria().NCilin; ++i) {
			if(Engine[0]->GetCilindro(i)->getAnguloActual() > Engine[0]->GetCilindro(i)->getDistribucion().CA
			   && Engine[0]->GetCilindro(i)->getAnguloAnterior() <= Engine[0]->GetCilindro(i)->getDistribucion().CA) {
				Engine[0]->GetCilindro(i)->PutNumeroInyecciones(EXTERN->GetOutput_dll(compo));
				Engine[0]->GetCilindro(i)->PutPresionInyeccion(EXTERN->GetOutput_dll(compo + 1));
				contador = 0;
				for(int j = 0; j < 8; j++) {
					if(Engine[0]->getACT()) {
						// Significa que la combustion es con ACT
						Engine[0]->GetCilindro(i)->PutSOP(j, EXTERN->GetOutput_dll(compo + 2 + j + contador));
						Engine[0]->GetCilindro(i)->PutMasaFuelPorInyeccion(j, EXTERN->GetOutput_dll(compo + 3 + j + contador));
					}
					contador++;
				}
			}
			compo += 18; /* Hay que hacer un bucle */
		}
	}

	for(int i = 0; i < CountVGT; ++i) {
		dynamic_cast<TEstatorTurbina*>(DatosTGV[i].Estator[0])->PutCDVbl(EXTERN->GetOutput_dll(compo));
		dynamic_cast<TRotorTurbina*>(DatosTGV[i].Rotor)->PutCDVbl(EXTERN->GetOutput_dll(compo + 1));
		DatosTGV[i].Rendimiento[0] = EXTERN->GetOutput_dll(compo + 2);
		if(DatosTGV[i].Entradas == 2) {
			dynamic_cast<TEstatorTurbina*>(DatosTGV[i].Estator[1])->PutCDVbl(EXTERN->GetOutput_dll(compo + 3));
			dynamic_cast<TRotorTurbina*>(DatosTGV[i].Rotor)->PutCDVbl(EXTERN->GetOutput_dll(compo + 4));
			DatosTGV[i].Rendimiento[1] = EXTERN->GetOutput_dll(compo + 5);
		}
		compo += 6;
	}

	if(EXTERN->getajustbaraba()) {
		Engine[0]->PutATCAdm(EXTERN->GetOutput_dll(compo));
		Engine[0]->PutATCEsc(EXTERN->GetOutput_dll(compo + 1));
		compo += 2;
	}

	if(EXTERN->getmodcomb()) {
		for(int i = 0; i <= Engine[0]->getGeometria().NCilin - 1; ++i) {
			Engine[0]->GetCilindro(i)->PutFQL(EXTERN->GetOutput_dll(compo));
			Engine[0]->GetCilindro(i)->PutInicioComb(EXTERN->GetOutput_dll(compo + 1));
			Engine[0]->GetCilindro(i)->PutFinComb(EXTERN->GetOutput_dll(compo + 2));
			compo += 3;
		}
	}

	if(SimulationType == nmTransitorioRegimenExterno) {
		Engine[0]->PutRegimen(EXTERN->GetOutput_dll(compo));
		compo += 1;
	}

	if(EXTERN->getFraccionMasicaEspeciesCil()) {
		for(int i = 0; i < Engine[0]->getGeometria().NCilin; ++i) {
			if(Engine[0]->GetCilindro(i)->getAnguloActual() > Engine[0]->GetCilindro(i)->getDistribucion().AE
			   && Engine[0]->GetCilindro(i)->getAnguloAnterior() <= Engine[0]->GetCilindro(i)->getDistribucion().AE) {
				for(int j = 0; j < Engine[0]->getSpeciesNumber() - 1;
					j++) {  // Se pone -1 porque la ultima especie es siempre el EGR, y este no debe modificarlo el usuario.
					Engine[0]->GetCilindro(i)->PutFraccionMasicaEspecie(j, EXTERN->GetOutput_dll(compo));

				}
			}
			compo += Engine[0]->getSpeciesNumber();
		}

	}

	if(EXTERN->getControlK()) {
		for(int i = 0; i < NumTCCPerdidaPresion; ++i) {
			PerdidaPresion[i]->PutK(EXTERN->GetOutput_dll(compo));
			compo += 1;
		}
	}

	EXTERN->AcumulaMedias(Run.TimeStep);

}

void TOpenWAM::InitFlowIndependentNumThreads() {
	fi_num_threads = 1;
#ifdef WITH_OPENMP
	std::stringstream ss;
	int n_threads = 0;
	char const* env_value = getenv("OMP_NUM_THREADS");
	if(env_value == NULL) {
		n_threads = omp_get_num_procs();
	} else {
		ss.str(env_value);
		if(!(ss >> n_threads)) {
			// OMP_NUM_THREADS isn't a valid integer.
			n_threads = 1;
		}
	}
	if(n_threads > 2) {
		fi_num_threads = 3;
	} else if(n_threads > 1) {
		fi_num_threads = 2;
	}
#endif
}

// ---------------------------------------------------------------------------

#pragma package(smart_init)kage(smart_init)age(smart_init)
