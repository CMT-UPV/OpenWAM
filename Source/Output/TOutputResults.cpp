// ---------------------------------------------------------------------------

#pragma hdrstop

#include "TOutputResults.h"

TOutputResults::TOutputResults() {

	FAvgOutput.str("");
	AvgEngine = NULL;
	FInsOutput.str("");
	FPlotThisCycle = true;
	FFirstTime = true;
	InsHeaderCreated = false;
	FWriteSpaceTime = false;
	FFileCountI = 1;
	WriteInsHeader = false;
}

TOutputResults::~TOutputResults() {

}

void TOutputResults::ReadAverageResults(const char* FileWAM, fpos_t& filepos,
		TTubo** Pipe, bool EngineBlock, TBloqueMotor** Engine,
		TDeposito **Plenum, TEjeTurbogrupo** Axis, TCompresor** Compressor,
		TTurbina** Turbine, TCondicionContorno** BC, TDPF** DPF,
		TCCCompresorVolumetrico** Root, TVenturi** Venturi, TSensor** Sensor,
		TController** Controller, int TotalCycles, char* ModelName) {

	GetName(ModelName, FAvgFilename, "AVG.DAT");

	FILE *FileInput = fopen(FileWAM, "r");
	fsetpos(FileInput, &filepos);

	// fpos_t filepos;

	int intAuxiliar;
	fscanf(FileInput, "%d ", &intAuxiliar);
	switch (intAuxiliar) {
	case 0:
		FTypeOfInsResults = nmLastCyle;
		FMultipleFiles = false;
		break;
	case 1:
		FTypeOfInsResults = nmAllCyclesIndependent;
		FMultipleFiles = true;
		FCharacters = int(log10((float) TotalCycles));
		break;
	case 2:
		FTypeOfInsResults = nmAllCyclesConcatenated;
		FMultipleFiles = false;
		break;
	case 3:
		FTypeOfInsResults = nmEveryNCycles;
		FMultipleFiles = true;
		fscanf(FileInput, "%d ", &FCyclePeriod);
		FCharacters = int(log10(float(TotalCycles) / FCyclePeriod));
		break;
	}

	// ! Read average results in pipes
	int NumPipesAvg;
	int PipeID;
	fscanf(FileInput, "%d ", &NumPipesAvg);
	fscanf(FileInput, "%d ", &intAuxiliar);
	for (int i = 0; i < NumPipesAvg; i++) {
		fscanf(FileInput, "%d ", &PipeID);
		AvgPipe.push_back(Pipe[PipeID - 1]);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		AvgPipe[i]->ReadAverageResultsTubo(FileWAM, filepos, EngineBlock);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	// ! Read average results in cylinders
	int NumCylindersAvg;
	int CylinderID;
	fscanf(FileInput, "%d ", &NumCylindersAvg);
	for (int i = 0; i < NumCylindersAvg; i++) {
		fscanf(FileInput, "%d ", &CylinderID);
		AvgCylinder.push_back(Engine[0]->GetCilindro(CylinderID - 1));
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		AvgCylinder[i]->ReadAverageResultsCilindro(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	// ! Read average results in the engine
	int NumEnginesAvg;
	int EngineID;
	fscanf(FileInput, "%d ", &NumEnginesAvg);
	if (NumEnginesAvg > 0) {
		AvgEngine = Engine[0];
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		AvgEngine->ReadAverageResultsBloqueMotor(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	// !Read average results in plenums
	int NumPlenumsAvg;
	int PlenumID;
	fscanf(FileInput, "%d ", &NumPlenumsAvg);
	for (int i = 0; i < NumPlenumsAvg; i++) {
		fscanf(FileInput, "%d ", &PlenumID);
		AvgPlenum.push_back(Plenum[PlenumID - 1]);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		AvgPlenum[i]->ReadAverageResultsDep(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	// !Read average results in turbocharger axis
	int NumAxisAvg;
	int AxisID;
	fscanf(FileInput, "%d ", &NumAxisAvg);
	for (int i = 0; i < NumAxisAvg; i++) {
		fscanf(FileInput, "%d ", &AxisID);
		AvgAxis.push_back(Axis[AxisID - 1]);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		AvgAxis[i]->ReadAverageResultsEje(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	// !Read average results in compressors
	int NumCompressorAvg;
	int CompressorID;
	fscanf(FileInput, "%d ", &NumCompressorAvg);
	for (int i = 0; i < NumCompressorAvg; i++) {
		fscanf(FileInput, "%d ", &CompressorID);
		AvgCompressor.push_back(Compressor[CompressorID - 1]);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		AvgCompressor[i]->LeeDatosGraficasMedias(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	// !Read average results in turbines
	int NumTurbineAvg;
	int TurbineID;
	fscanf(FileInput, "%d ", &NumTurbineAvg);
	for (int i = 0; i < NumTurbineAvg; i++) {
		fscanf(FileInput, "%d ", &TurbineID);
		AvgTurbine.push_back(Turbine[TurbineID - 1]);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		AvgTurbine[i]->ReadAverageResultsTurb(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	// !Read average results in valves
	int NumValvesAvg;
	int ValveID;
	fscanf(FileInput, "%d ", &NumValvesAvg);
	for (int i = 0; i < NumValvesAvg; i++) {
		fscanf(FileInput, "%d ", &ValveID);
		if (BC[ValveID - 1]->getTipoCC() == nmIntakeValve
				|| BC[ValveID - 1]->getTipoCC() == nmExhaustValve) {
			AvgValve.push_back(
					dynamic_cast<TCCCilindro*>(BC[ValveID - 1])->getValvula());
		} else if (BC[ValveID - 1]->getTipoCC() == nmPipeToPlenumConnection) {
			AvgValve.push_back(
					dynamic_cast<TCCDeposito*>(BC[ValveID - 1])->getValvula());
		} else if (BC[ValveID - 1]->getTipoCC() == nmUnionEntreDepositos) {
			AvgValve.push_back(
					dynamic_cast<TCCUnionEntreDepositos*>(BC[ValveID - 1])->getValvula());
		} else
			printf(
					"ERROR: There is no valves asigned in the connection number %d\n",
					ValveID);
		AvgValveNode.push_back(ValveID);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		AvgValve[i]->LeeDatosGraficasMED(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);

	}

	// !Read average results in root compressors
	int NumRootsAvg;
	int RootID;
	fscanf(FileInput, "%d ", &NumRootsAvg);

	for (int i = 0; i < NumRootsAvg; ++i) {
		fscanf(FileInput, "%d ", &RootID);
		AvgRoot.push_back(Root[RootID - 1]);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		AvgRoot[i]->ReadAverageResultsCV(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	// !Read average results in venturis
	int NumVenturisAvg;
	int VenturiID;
	fscanf(FileInput, "%d ", &NumVenturisAvg);

	for (int i = 0; i < NumVenturisAvg; ++i) {
		fscanf(FileInput, "%d ", &VenturiID);
		AvgVenturi.push_back(Venturi[VenturiID - 1]);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		AvgVenturi[i]->ReadAverageResultsVenturi(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	// !Read average results in connections between plenums
	int NumConnectionsAvg;
	int ConnectionID;
	fscanf(FileInput, "%d ", &NumConnectionsAvg);

	for (int i = 0; i < NumConnectionsAvg; ++i) {
		fscanf(FileInput, "%d ", &ConnectionID);
		if (BC[ConnectionID - 1]->getTipoCC() == nmUnionEntreDepositos) {
			AvgConnection.push_back(
					dynamic_cast<TCCUnionEntreDepositos*>(BC[ConnectionID - 1]));
		} else {
			printf("ERROR: Connection %d\n does not connect two plenums",
					ConnectionID);
		}
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		AvgConnection[i]->ReadAverageResultsUED(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	// !Read average results in DPF
#ifdef ParticulateFilter
	int NumDPFAvg;
	int DPFID;
	fscanf ( FileInput,"%d ", &NumDPFAvg );
	for ( int i = 0; i < NumDPFAvg; i++ ) {
		fscanf ( FileInput,"%d ",&DPFID );
		AvgDPF.push_back ( DPF[DPFID-1] );
		fgetpos ( FileInput, &filepos );
		fclose ( FileInput );
		AvgDPF[i]->LeeResultadosMediosDPF ( FileWAM,filepos );
		FileInput = fopen ( FileWAM,"r" );
		fsetpos ( FileInput, &filepos );
	}
#endif

	// !Read average results in sensors
	int NumSensorAvg;
	int SensorID;
	fscanf(FileInput, "%d ", &NumSensorAvg);
	for (int i = 0; i < NumSensorAvg; ++i) {
		fscanf(FileInput, "%d ", &SensorID);
		AvgSensor.push_back(Sensor[SensorID - 1]);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		AvgSensor[i]->LeeResultadosMedSensor(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	// !Read average results in controllers
	int NumControllersAvg;
	int ControllerID;
	fscanf(FileInput, "%d ", &NumControllersAvg);

	for (int i = 0; i < NumControllersAvg; ++i) {
		fscanf(FileInput, "%d ", &ControllerID);
		AvgController.push_back(Controller[ControllerID - 1]);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		AvgController[i]->LeeResultadosMedControlador(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}
	fgetpos(FileInput, &filepos);
	fclose(FileInput);

}

void TOutputResults::HeaderAverageResults(stEspecies *SpeciesName,
		TCalculoExtern* EXTERN, bool ThereIsDLL) {

	FAvgOutput << PutLabel(704);
	FAvgOutput << "\t" << PutLabel(701);

	// ! Header average results in cylinders.

	for (Uint i = 0; i < AvgCylinder.size(); ++i) {
		AvgCylinder[i]->HeaderAverageResultsCilindro(FAvgOutput, SpeciesName);
	}

	// ! Header average results in the engine.
	if (AvgEngine != NULL) {
		AvgEngine->HeaderAverageResultsBloqueMotor(FAvgOutput);
	}

	// ! Header average results in plenums
	for (Uint i = 0; i < AvgPlenum.size(); ++i) {
		AvgPlenum[i]->HeaderAverageResultsDep(FAvgOutput, SpeciesName);
	}

	// ! Header average results in pipes
	for (Uint i = 0; i < AvgPipe.size(); i++) {
		AvgPipe[i]->HeaderAverageResults(FAvgOutput, SpeciesName);
	}

	// ! Header average results in valves
	for (Uint i = 0; i < AvgValve.size(); i++) {
		AvgValve[i]->CabeceraGraficaMED(FAvgOutput, AvgValveNode[i]);
	}

	// ! Header average results in venturis
	for (Uint i = 0; i < AvgVenturi.size(); i++) {
		AvgVenturi[i]->HeaderAverageResultsVenturi(FAvgOutput);
	}

	// ! Header average results in turbocharger.
	for (Uint i = 0; i < AvgAxis.size(); ++i) {
		AvgAxis[i]->CabeceraResultadosMedEje(FAvgOutput);
	}

	// ! Header averge results in compressors.
	for (Uint i = 0; i < AvgCompressor.size(); ++i) {
		AvgCompressor[i]->CabeceraGraficasMedias(FAvgOutput);
	}

	// ! Header average results in turbines

	for (Uint i = 0; i < AvgTurbine.size(); i++) {
		AvgTurbine[i]->CabeceraResultadosMedTurb(FAvgOutput);
	}

	// ! Header average results in root compressors
	for (Uint i = 0; i < AvgRoot.size(); i++) {
		AvgRoot[i]->CabeceraResultadosMedCV(FAvgOutput);
	}

	// ! Header average results in connections between plenums
	for (Uint i = 0; i < AvgConnection.size(); i++) {
		AvgConnection[i]->HeaderAverageResultsUED(FAvgOutput);
	}

	// ! Header average results in connections between DPF.
#ifdef ParticulateFilter
	for ( Uint i=0; i<AvgDPF.size(); i++ ) {
		AvgDPF[i]->CabeceraResultadosMedios ( FAvgOutput,SpeciesName );
	}
#endif

	// ! Header average results in sensors
	for (Uint i = 0; i < AvgSensor.size(); i++) {
		AvgSensor[i]->CabeceraResultadosMedSensor(FAvgOutput);
	}

	// ! Header average results in controllers
	for (Uint i = 0; i < AvgController.size(); i++) {
		AvgController[i]->CabeceraResultadosMedControlador(FAvgOutput);
	}

	// ! Average results in external calculations
	if (ThereIsDLL) {
		EXTERN->ImprimeCabeceraMedias(FAvgOutput);
	}
}

void TOutputResults::OutputAverageResults(double AcumulatedTime,
		TCalculoExtern* EXTERN, bool ThereIsDLL) {

	float pasafloat;

	pasafloat = (float) (AcumulatedTime);

	FAvgOutput << std::endl;
	FAvgOutput << AvgEngine->getCiclo();
	FAvgOutput << "\t";
	FAvgOutput << pasafloat;

	// ! Average results in cylinders

	for (Uint i = 0; i < AvgCylinder.size(); ++i) {
		AvgCylinder[i]->CalculaResultadosMediosCilindro();
		AvgCylinder[i]->ImprimeResultadosMediosCilindro(FAvgOutput);
	}

	// ! Average results in the engine
	if (AvgEngine != NULL) {
		AvgEngine->ResultadosMediosBloqueMotor();
		AvgEngine->ImprimeResultadosMediosBloqueMotor(FAvgOutput);
	}

	// ! Average results in plenums

	for (Uint i = 0; i < AvgPlenum.size(); ++i) {
		AvgPlenum[i]->ResultadosMediosDep();
		AvgPlenum[i]->ImprimeResultadosMediosDep(FAvgOutput);
	}

	// ! Average results in pipes
	for (Uint i = 0; i < AvgPipe.size(); ++i) {
		AvgPipe[i]->ImprimeResultadosMedios(FAvgOutput);
	}

	// ! Average results in valves.
	for (Uint i = 0; i < AvgValve.size(); i++) {
		AvgValve[i]->ImprimeGraficaMED(FAvgOutput);
	}

	// ! Average results in venturis
	for (Uint i = 0; i < AvgVenturi.size(); i++) {
		AvgVenturi[i]->ResultadosMediosVenturi();
		AvgVenturi[i]->ImprimeResultadosMediosVenturi(FAvgOutput);
	}

	// ! Average results in turbocharger axis
	for (Uint i = 0; i < AvgAxis.size(); ++i) {
		AvgAxis[i]->ResultadosMediosEje();
		AvgAxis[i]->ImprimeResultadosMedEje(FAvgOutput);
	}

	// ! Average results in compressors.
	for (Uint i = 0; i < AvgCompressor.size(); ++i) {
		AvgCompressor[i]->ImprimeGraficasMedias(FAvgOutput);
	}

	// ! Average results in turbines.
	for (Uint i = 0; i < AvgTurbine.size(); i++) {
		AvgTurbine[i]->CalculaResultadosMediosTurb();
		AvgTurbine[i]->ImprimeResultadosMedTurb(FAvgOutput);
	}

	// ! Average results in root compressors.
	for (Uint i = 0; i < AvgRoot.size(); i++) {
		AvgRoot[i]->ResultadosMediosCV();
		AvgRoot[i]->ImprimeResultadosMedCV(FAvgOutput);
	}

	// ! Average results in connections.
	for (Uint i = 0; i < AvgConnection.size(); i++) {
		AvgConnection[i]->ResultadosMediosUED();
		AvgConnection[i]->ImprimeResultadosMediosUED(FAvgOutput);
	}

	// ! Average results in DPF.
#ifdef ParticulateFilter
	for ( int i=0; i<AvgConnection.size(); i++ ) {
		AvgDPF[i]->ImprimeResultadosMedios ( FAvgOutput );
	}
#endif

	// ! Average results in sensors.
	for (Uint i = 0; i < AvgSensor.size(); i++) {
		AvgSensor[i]->ResultadosMediosSensor();
		AvgSensor[i]->ImprimeResultadosMedSensor(FAvgOutput);
	}

	// ! Average results in controllers
	for (Uint i = 0; i < AvgController.size(); i++) {
		AvgController[i]->ResultadosMediosController();
		AvgController[i]->ImprimeResultadosMedControlador(FAvgOutput);
	}

	// ! Average results in external calculations.
	if (ThereIsDLL) {
		EXTERN->CalculaMedias();
		EXTERN->ImprimeGraficosMedias(FAvgOutput);
	}

}

void TOutputResults::CopyAverageResultsToFile(int mode) {

	if (mode == 0) { // ! Create new file o rewrite if it exists
		FFileAvg.open(FAvgFilename, fstream::out);
	} else { // ! Create new file or append if it exists
		FFileAvg.open(FAvgFilename, fstream::out | fstream::app);
	}

	FFileAvg << FAvgOutput.str();

	FFileAvg.close();

	FAvgOutput.str("");
}

void TOutputResults::CopyInstananeousResultsToFile(int mode) {

	if (FInsOutput.str() != "" && (WriteInsHeader || mode == 1)) {

		char FileName[300];

		if (FMultipleFiles) {
			ConvertCharacter(FFileCountI, FFileCountC, FCharacters);

			strcat(FFileCountC, ".DAT");
			GetName(FInsFilename, FileName, FFileCountC);
		} else {
			GetName(FInsFilename, FileName, ".DAT");
		}

		if (WriteInsHeader) {
			FFileIns.open(FileName, fstream::out);
		} else {
#ifdef WriteINS
			FFileIns.open ( FileName, fstream::out | fstream::app );
#endif
#ifndef WriteINS
			FFileIns.open(FileName, fstream::out);
#endif
		}

		FFileIns << FInsOutput.str();

		FFileIns.close();

		FInsOutput.str("");

		FFileCountI++;
	}
}

void TOutputResults::ReadInstantaneousResults(const char* FileWAM,
		fpos_t &filepos, TBloqueMotor** Engine, TDeposito** Plenum,
		TTubo** Pipe, TVenturi** Venturi, TCondicionContorno** BC, TDPF** DPF,
		TEjeTurbogrupo** Turbo, TCompresor** Compressor, TTurbina** Turbine,
		TCCCompresorVolumetrico** Root, TCondicionContorno** BCWasteGate,
		int NumberOfWasteGates, TCondicionContorno** BCReedValve,
		int NumberOfReedValves, TSensor** Sensor, TController** Controller,
		char* ModelName) {

	GetName(ModelName, FInsFilename, "INS.DAT");

	int err = remove(FInsFilename);

	FILE *FileInput = fopen(FileWAM, "r");
	fsetpos(FileInput, &filepos);

	// ! Instantaneous results in cylinders
	int NumCylindersIns;
	int CylinderID;
	fscanf(FileInput, "%d ", &NumCylindersIns);
	for (int i = 0; i < NumCylindersIns; ++i) {
		fscanf(FileInput, "%d ", &CylinderID);
		InsCylinder.push_back(Engine[0]->GetCilindro(CylinderID - 1));
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		InsCylinder[i]->ReadInstantaneousResultsCilindro(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	// ! Instantaneous results in plenums
	int NumPlenumsIns;
	int PlenumID;
	fscanf(FileInput, "%d ", &NumPlenumsIns);
	for (int i = 0; i < NumPlenumsIns; i++) {
		fscanf(FileInput, "%d ", &PlenumID);
		InsPlenum.push_back(Plenum[PlenumID - 1]);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		InsPlenum[i]->ReadInstantaneousResultsDep(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	// ! Instantaneous results in pipes
	int NumPipesIns;
	int PipeID;
	fscanf(FileInput, "%d ", &NumPipesIns);
	/* Dato para WAMer. Numero de resultados instantaneos en tubos */
	int numeroparawamer;
	fscanf(FileInput, "%d ", &numeroparawamer);
	for (int i = 0; i < NumPipesIns; i++) {
		fscanf(FileInput, "%d ", &PipeID);
		InsPipe.push_back(Pipe[PipeID - 1]);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		InsPipe[i]->ReadInstantaneousResultsTubo(FileWAM, filepos, Engine);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	// ! Instantaneous results in venturis
	int NumVenturisIns;
	int VenturiID;
	fscanf(FileInput, "%d ", &NumVenturisIns);
	for (int i = 0; i < NumVenturisIns; i++) {
		fscanf(FileInput, "%d ", &VenturiID);
		InsVenturi.push_back(Venturi[VenturiID - 1]);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		InsVenturi[i]->LeeResultadosInstantVenturi(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	// ! Instantaneous results in connections
	int NumValvesIns;
	int ValveID;
	fscanf(FileInput, "%d", &NumValvesIns);
	for (int i = 0; i < NumValvesIns; i++) {
		fscanf(FileInput, "%d ", &ValveID);
		if (BC[ValveID - 1]->getTipoCC() == nmIntakeValve
				|| BC[ValveID - 1]->getTipoCC() == nmExhaustValve) {
			InsValve.push_back(
					dynamic_cast<TCCCilindro*>(BC[ValveID - 1])->getValvula());
		} else if (BC[ValveID - 1]->getTipoCC() == nmPipeToPlenumConnection) {
			InsValve.push_back(
					dynamic_cast<TCCDeposito*>(BC[ValveID - 1])->getValvula());
		} else if (BC[ValveID - 1]->getTipoCC() == nmUnionEntreDepositos) {
			InsValve.push_back(
					dynamic_cast<TCCUnionEntreDepositos*>(BC[ValveID - 1])->getValvula());
		} else
			printf(
					"ERROR: There is no valves asigned in the connection number %d\n",
					ValveID);
		InsValveNode.push_back(ValveID);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		InsValve[i]->LeeDatosGraficasINS(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	// ! Instantaneous results turbochargers.
	int NumTurboIns;
	int TurboID;
	fscanf(FileInput, "%d", &NumTurboIns);
	for (int i = 0; i < NumTurboIns; i++) {
		fscanf(FileInput, "%d", &TurboID);
		InsTurbo.push_back(Turbo[TurboID - 1]);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		InsTurbo[i]->ReadInstantaneousResultsEje(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	int NumCompressorIns;
	int CompressorID;
	fscanf(FileInput, "%d ", &NumCompressorIns);
	for (int i = 0; i < NumCompressorIns; i++) {
		fscanf(FileInput, "%d ", &CompressorID);
		InsCompressor.push_back(Compressor[CompressorID - 1]);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		InsCompressor[i]->LeeDatosGraficasInstantaneas(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	int NumTurbineIns;
	int TurbineID;
	fscanf(FileInput, "%d ", &NumTurbineIns);
	for (int i = 0; i < NumTurbineIns; i++) {
		fscanf(FileInput, "%d ", &TurbineID);
		InsTurbine.push_back(Turbine[TurbineID - 1]);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		InsTurbine[i]->LeeResultadosInstantTurb(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	int NumRootsIns;
	int RootID;
	fscanf(FileInput, "%d ", &NumRootsIns);
	for (int i = 0; i < NumRootsIns; ++i) {
		fscanf(FileInput, "%d ", &RootID);
		InsRoot.push_back(Root[RootID - 1]);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		InsRoot[i]->LeeResultadosInstantCV(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	// !Read instantaneous results in connections between plenums
	int NumConnectionsIns;
	int ConnectionID;
	fscanf(FileInput, "%d ", &NumConnectionsIns);
	for (int i = 0; i < NumConnectionsIns; ++i) {
		fscanf(FileInput, "%d ", &ConnectionID);
		if (BC[ConnectionID - 1]->getTipoCC() == nmUnionEntreDepositos) {
			InsConnection.push_back(
					dynamic_cast<TCCUnionEntreDepositos*>(BC[ConnectionID - 1]));
		} else {
			printf("ERROR: Connection %d\n does not connect two plenums",
					ConnectionID);
		}
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		InsConnection[i]->LeeResultadosInstantUED(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	//
	// // RESULTADOS INSTANTANEOS WASTE-GATE.

	int NumWasteGateIns;
	int WasteGateID;
	bool valido = false;
	fscanf(FileInput, "%d ", &NumWasteGateIns);
	for (int i = 0; i < NumWasteGateIns; i++) {
		fscanf(FileInput, "%d ", &WasteGateID);
		for (int j = 0; j < NumberOfWasteGates; j++) {
			if (BCWasteGate[j]->getNumeroCC() == WasteGateID) {
				valido = true;
				fgetpos(FileInput, &filepos);
				fclose(FileInput);
				if (BCWasteGate[j]->getTipoCC() == nmPipeToPlenumConnection) {
					InsWasteGate.push_back(
							dynamic_cast<TWasteGate*>(dynamic_cast<TCCDeposito*>(BCWasteGate[j])->getValvula()));
				} else if (BCWasteGate[j]->getTipoCC()
						== nmUnionEntreDepositos) {
					InsWasteGate.push_back(
							dynamic_cast<TWasteGate*>(dynamic_cast<TCCUnionEntreDepositos*>(BCWasteGate[j])->getValvula()));
				}
				InsWasteGate[i]->LeeDatosGraficasINS(FileWAM, filepos);
				FileInput = fopen(FileWAM, "r");
				fsetpos(FileInput, &filepos);
			}
		}
		if (!valido)
			printf(
					"ERROR: A WasteGate does not exist in connection number %d\n",
					WasteGateID);
		valido = false;
	}
	//
	// // RESULTADOS INSTANTANEOS LAMINA.
	int NumReedIns;
	int ReedID;
	fscanf(FileInput, "%d ", &NumReedIns);
	for (int i = 0; i < NumReedIns; i++) {
		fscanf(FileInput, "%d ", &ReedID);
		for (int j = 0; j < NumberOfReedValves; j++) {
			if (BCReedValve[j]->getNumeroCC() == ReedID) {
				valido = true;
				fgetpos(FileInput, &filepos);
				fclose(FileInput);
				if (BCReedValve[j]->getTipoCC() == nmPipeToPlenumConnection) {
					InsReedValve.push_back(
							dynamic_cast<TLamina*>(dynamic_cast<TCCDeposito*>(BCReedValve[j])->getValvula()));
				} else if (BCReedValve[j]->getTipoCC()
						== nmUnionEntreDepositos) {
					InsReedValve.push_back(
							dynamic_cast<TLamina*>(dynamic_cast<TCCUnionEntreDepositos*>(BCReedValve[j])->getValvula()));
				}
				InsReedValve[i]->LeeDatosGraficasINS(FileWAM, filepos);
				FileInput = fopen(FileWAM, "r");
				fsetpos(FileInput, &filepos);
			}
		}
		if (!valido)
			printf(
					"ERROR: A reed valve does not exist in connection number %d\n",
					ReedID);
		valido = false;

	}

	// !Read instantaneous results in DPF.
#ifdef ParticulateFilter
	int NumDPFIns;
	int DPFID;
	fscanf ( FileInput, "%d ", &NumDPFIns );
	for ( int i = 0; i < NumDPFIns; ++i ) {
		fscanf ( FileInput, "%d ", &DPFID );
		InsDPF.push_back ( DPF[DPFID - 1] );
		fgetpos ( FileInput, &filepos );
		fclose ( FileInput );
		InsDPF[i]->LeeResultadosInstantaneosDPF ( FileWAM,filepos );
		FileInput=fopen ( FileWAM,"r" );
		fsetpos ( FileInput, &filepos );
	}
#endif

	// !Read instantaneous results in sensors
	int NumSensorIns;
	int SensorID;
	fscanf(FileInput, "%d ", &NumSensorIns);
	for (int i = 0; i < NumSensorIns; ++i) {
		fscanf(FileInput, "%d ", &SensorID);
		InsSensor.push_back(Sensor[SensorID - 1]);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		InsSensor[i]->LeeResultadosInsSensor(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	// !Read instantaneous results in controllers

	int NumControllersIns;
	int ControllerID;
	fscanf(FileInput, "%d ", &NumControllersIns);

	for (int i = 0; i < NumControllersIns; ++i) {
		fscanf(FileInput, "%d ", &ControllerID);
		InsController.push_back(Controller[ControllerID - 1]);
		fgetpos(FileInput, &filepos);
		fclose(FileInput);
		InsController[i]->LeeResultadosInsControlador(FileWAM, filepos);
		FileInput = fopen(FileWAM, "r");
		fsetpos(FileInput, &filepos);
	}

	fgetpos(FileInput, &filepos);
	fclose(FileInput);

}

void TOutputResults::ReadSpaceTimeResults(const char* FileWAM, fpos_t &filepos,
		TTubo** Pipe, TBloqueMotor** Engine, TDeposito **Plenum) {

	// Numero de elementos en los que se grafica
	int FNumMagnitudesEspTemp;
	int NumCilEspTemp;
	int NumDepEspTemp;
	int NumTubEspTemp;

	FILE *FileInput = fopen(FileWAM, "r");
	fsetpos(FileInput, &filepos);

	fscanf(FileInput, "%d ", &FNumMagnitudesEspTemp);
	if (FNumMagnitudesEspTemp != 0) {
		fscanf(FileInput, "%d %d %d ", &NumCilEspTemp, &NumDepEspTemp,
				&NumTubEspTemp);

		int CylinderID;
		for (int i = 0; i < NumCilEspTemp; ++i) {
			fscanf(FileInput, "%d ", &CylinderID);
			STCylinder.push_back(Engine[0]->GetCilindro(CylinderID - 1));
		}

		int PlenumID;
		for (int i = 0; i < NumDepEspTemp; ++i) {
			fscanf(FileInput, "%d ", &PlenumID);
			STPlenum.push_back(Plenum[PlenumID - 1]);
		}

		int PipeID;
		for (int i = 0; i < NumTubEspTemp; ++i) {
			fscanf(FileInput, "%d ", &PipeID);
			STPipe.push_back(Pipe[PipeID - 1]);
		}

		// Magnitudes que se grafican
		int ParameterID;
		for (int i = 0; i < FNumMagnitudesEspTemp; ++i) {
			fscanf(FileInput, "%d ", &ParameterID);
			FParameterSpaceTime.push_back(ParameterID);
		}
	}

	fgetpos(FileInput, &filepos);
	fclose(FileInput);

}

void TOutputResults::DoSpaceTimeFiles(int SpeciesNumber) {

	if (FParameterSpaceTime.size() > 0) {
		int k;
		if (SpeciesNumber == 3) {
			for (int j = 0; j <= 299; ++j) {
				salpre[j] = '\0';
				saltem[j] = '\0';
				salvel[j] = '\0';
				salair[j] = '\0';
				salYAireFresco[j] = '\0';
				salYGasQuemado[j] = '\0';
				salGastoAireFresco[j] = '\0';
				salGastoGasQuemado[j] = '\0';
			}
		} else if (SpeciesNumber == 4) {
			for (int j = 0; j <= 299; ++j) {
				salpre[j] = '\0';
				saltem[j] = '\0';
				salvel[j] = '\0';
				salair[j] = '\0';
				salYCombustible[j] = '\0';
				salYAireFresco[j] = '\0';
				salYGasQuemado[j] = '\0';
				salGastoCombustible[j] = '\0';
				salGastoAireFresco[j] = '\0';
				salGastoGasQuemado[j] = '\0';
			}
		} else if (SpeciesNumber == 9) {
			for (int j = 0; j <= 299; ++j) {
				salpre[j] = '\0';
				saltem[j] = '\0';
				salvel[j] = '\0';
				salair[j] = '\0';
				salYO2[j] = '\0';
				salYN2[j] = '\0';
				salYCO2[j] = '\0';
				salYH2O[j] = '\0';
				salYCO[j] = '\0';
				salYHC[j] = '\0';
				salYSoot[j] = '\0';
				salYNOx[j] = '\0';
				salGastoO2[j] = '\0';
				salGastoN2[j] = '\0';
				salGastoCO2[j] = '\0';
				salGastoH2O[j] = '\0';
				salGastoCO[j] = '\0';
				salGastoHC[j] = '\0';
				salGastoSoot[j] = '\0';
				salGastoNOx[j] = '\0';
			}
		} else if (SpeciesNumber == 10) {
			for (int j = 0; j <= 299; ++j) {
				salpre[j] = '\0';
				saltem[j] = '\0';
				salvel[j] = '\0';
				salair[j] = '\0';
				salYO2[j] = '\0';
				salYN2[j] = '\0';
				salYCO2[j] = '\0';
				salYH2O[j] = '\0';
				salYCO[j] = '\0';
				salYHC[j] = '\0';
				salYSoot[j] = '\0';
				salYCombustible[j] = '\0';
				salYNOx[j] = '\0';
				salGastoO2[j] = '\0';
				salGastoN2[j] = '\0';
				salGastoCO2[j] = '\0';
				salGastoH2O[j] = '\0';
				salGastoCO[j] = '\0';
				salGastoHC[j] = '\0';
				salGastoSoot[j] = '\0';
				salGastoCombustible[j] = '\0';
				salGastoNOx[j] = '\0';
			}
		}

		for (unsigned int i = 0; i < FParameterSpaceTime.size(); ++i) {
			switch (FParameterSpaceTime[i]) {
			case 0:
				k = 0;
				while (FInsFilename[k] != '.') {
					salpre[k] = FInsFilename[k];
					++k;
				}
				strcat(salpre, "_pre.DAT");
				FileOutPressure = fopen(salpre, "w");
				break;
			case 1:
				k = 0;
				while (FInsFilename[k] != '.') {
					saltem[k] = FInsFilename[k];
					++k;
				}
				strcat(saltem, "_tem.DAT");
				FileOutTemp = fopen(saltem, "w");
				break;
			case 2:
				k = 0;
				while (FInsFilename[k] != '.') {
					salvel[k] = FInsFilename[k];
					++k;
				}
				strcat(salvel, "_vel.DAT");
				FileOutVel = fopen(salvel, "w");
				break;
			case 3:
				k = 0;
				while (FInsFilename[k] != '.') {
					salair[k] = FInsFilename[k];
					++k;
				}
				strcat(salair, "_air.DAT");
				FileOutFlow = fopen(salair, "w");
				break;
			case 4:
				k = 0;
				if (SpeciesNumber == 3) {
					while (FInsFilename[k] != '.') {
						salYGasQuemado[k] = FInsFilename[k];
						salYAireFresco[k] = FInsFilename[k];
						++k;
					}
					strcat(salYGasQuemado, "_YGQ.DAT");
					strcat(salYAireFresco, "_YAF.DAT");
					FOutYBurntGas = fopen(salYGasQuemado, "w");
					FOutYFreshAir = fopen(salYAireFresco, "w");

				} else if (SpeciesNumber == 4) {
					while (FInsFilename[k] != '.') {
						salYGasQuemado[k] = FInsFilename[k];
						salYCombustible[k] = FInsFilename[k];
						salYAireFresco[k] = FInsFilename[k];
						++k;
					}
					strcat(salYGasQuemado, "_YGQ.DAT");
					strcat(salYCombustible, "_Ycomb.DAT");
					strcat(salYAireFresco, "_YAF.DAT");
					FOutYBurntGas = fopen(salYGasQuemado, "w");
					FOutYFuel = fopen(salYCombustible, "w");
					FOutYFreshAir = fopen(salYAireFresco, "w");

				} else if (SpeciesNumber == 9) {
					while (FInsFilename[k] != '.') {
						salYO2[k] = FInsFilename[k];
						salYCO2[k] = FInsFilename[k];
						salYH2O[k] = FInsFilename[k];
						salYHC[k] = FInsFilename[k];
						salYSoot[k] = FInsFilename[k];
						salYNOx[k] = FInsFilename[k];
						salYCO[k] = FInsFilename[k];
						salYN2[k] = FInsFilename[k];
						++k;
					}
					strcat(salYO2, "_YO2.DAT");
					strcat(salYCO2, "_YCO2.DAT");
					strcat(salYH2O, "_YH2O.DAT");
					strcat(salYHC, "_YHC.DAT");
					strcat(salYSoot, "_YSoot.DAT");
					strcat(salYNOx, "_YNOx.DAT");
					strcat(salYCO, "_YCO.DAT");
					strcat(salYN2, "_YN2.DAT");
					FOutYO2 = fopen(salYO2, "w");
					FOutYCO2 = fopen(salYCO2, "w");
					FOutYH2O = fopen(salYH2O, "w");
					FOutYHC = fopen(salYHC, "w");
					FOutYSoot = fopen(salYSoot, "w");
					FOutYNOx = fopen(salYNOx, "w");
					FOutYCO = fopen(salYCO, "w");
					FOutYN2 = fopen(salYN2, "w");

				} else if (SpeciesNumber == 10) {
					while (FInsFilename[k] != '.') {
						salYO2[k] = FInsFilename[k];
						salYCO2[k] = FInsFilename[k];
						salYH2O[k] = FInsFilename[k];
						salYHC[k] = FInsFilename[k];
						salYSoot[k] = FInsFilename[k];
						salYNOx[k] = FInsFilename[k];
						salYCO[k] = FInsFilename[k];
						salYCombustible[k] = FInsFilename[k];
						salYN2[k] = FInsFilename[k];
						++k;
					}
					strcat(salYO2, "_YO2.DAT");
					strcat(salYCO2, "_YCO2.DAT");
					strcat(salYH2O, "_YH2O.DAT");
					strcat(salYHC, "_YHC.DAT");
					strcat(salYSoot, "_YSoot.DAT");
					strcat(salYNOx, "_YNOx.DAT");
					strcat(salYCO, "_YCO.DAT");
					strcat(salYCombustible, "_YComb.DAT");
					strcat(salYN2, "_YN2.DAT");
					FOutYO2 = fopen(salYO2, "w");
					FOutYCO2 = fopen(salYCO2, "w");
					FOutYH2O = fopen(salYH2O, "w");
					FOutYHC = fopen(salYHC, "w");
					FOutYSoot = fopen(salYSoot, "w");
					FOutYNOx = fopen(salYNOx, "w");
					FOutYCO = fopen(salYCO, "w");
					FOutYFuel = fopen(salYCombustible, "w");
					FOutYN2 = fopen(salYN2, "w");
				}
				break;
			case 5:
				k = 0;
				if (SpeciesNumber == 3) {
					while (FInsFilename[k] != '.') {
						salGastoGasQuemado[k] = FInsFilename[k];
						salGastoAireFresco[k] = FInsFilename[k];
						++k;
					}
					strcat(salGastoGasQuemado, "_GastoGQ.DAT");
					strcat(salGastoAireFresco, "_GastoAF.DAT");
					FOutFlowBurntGas = fopen(salGastoGasQuemado, "w");
					FOutFlowFreshAir = fopen(salGastoAireFresco, "w");

				} else if (SpeciesNumber == 4) {
					while (FInsFilename[k] != '.') {
						salGastoGasQuemado[k] = FInsFilename[k];
						salGastoCombustible[k] = FInsFilename[k];
						salGastoAireFresco[k] = FInsFilename[k];
						++k;
					}
					strcat(salGastoGasQuemado, "_GastoGQ.DAT");
					strcat(salGastoCombustible, "_Gastocomb.DAT");
					strcat(salGastoAireFresco, "_GastoAF.DAT");
					FOutFlowBurntGas = fopen(salGastoGasQuemado, "w");
					FOutFlowFuel = fopen(salGastoCombustible, "w");
					FOutFlowFreshAir = fopen(salGastoAireFresco, "w");

				} else if (SpeciesNumber == 9) {
					while (FInsFilename[k] != '.') {
						salGastoO2[k] = FInsFilename[k];
						salGastoCO2[k] = FInsFilename[k];
						salGastoH2O[k] = FInsFilename[k];
						salGastoHC[k] = FInsFilename[k];
						salGastoSoot[k] = FInsFilename[k];
						salGastoNOx[k] = FInsFilename[k];
						salGastoCO[k] = FInsFilename[k];
						salGastoN2[k] = FInsFilename[k];
						++k;
					}
					strcat(salGastoO2, "_GastoO2.DAT");
					strcat(salGastoCO2, "_GastoCO2.DAT");
					strcat(salGastoH2O, "_GastoH2O.DAT");
					strcat(salGastoHC, "_GastoHC.DAT");
					strcat(salGastoSoot, "_GastoSoot.DAT");
					strcat(salGastoNOx, "_GastoNOx.DAT");
					strcat(salGastoCO, "_GastoCO.DAT");
					strcat(salGastoN2, "_GastoN2.DAT");
					FOutFlowO2 = fopen(salGastoO2, "w");
					FOutFlowCO2 = fopen(salGastoCO2, "w");
					FOutFlowH2O = fopen(salGastoH2O, "w");
					FOutFlowHC = fopen(salGastoHC, "w");
					FOutFlowSoot = fopen(salGastoSoot, "w");
					FOutFlowNOx = fopen(salGastoNOx, "w");
					FOutFlowCO = fopen(salGastoCO, "w");
					FOutFlowN2 = fopen(salGastoN2, "w");

				} else if (SpeciesNumber == 10) {
					while (FInsFilename[k] != '.') {
						salGastoO2[k] = FInsFilename[k];
						salGastoCO2[k] = FInsFilename[k];
						salGastoH2O[k] = FInsFilename[k];
						salGastoHC[k] = FInsFilename[k];
						salGastoSoot[k] = FInsFilename[k];
						salGastoNOx[k] = FInsFilename[k];
						salGastoCO[k] = FInsFilename[k];
						salGastoCombustible[k] = FInsFilename[k];
						salGastoN2[k] = FInsFilename[k];
						++k;
					}
					strcat(salGastoO2, "_GastoO2.DAT");
					strcat(salGastoCO2, "_GastoCO2.DAT");
					strcat(salGastoH2O, "_GastoH2O.DAT");
					strcat(salGastoHC, "_GastoHC.DAT");
					strcat(salGastoSoot, "_GastoSoot.DAT");
					strcat(salGastoNOx, "_GastoNOx.DAT");
					strcat(salGastoCO, "_GastoCO.DAT");
					strcat(salGastoCombustible, "_GastoComb.DAT");
					strcat(salGastoN2, "_GastoN2.DAT");
					FOutFlowO2 = fopen(salGastoO2, "w");
					FOutFlowCO2 = fopen(salGastoCO2, "w");
					FOutFlowH2O = fopen(salGastoH2O, "w");
					FOutFlowHC = fopen(salGastoHC, "w");
					FOutFlowSoot = fopen(salGastoSoot, "w");
					FOutFlowNOx = fopen(salGastoNOx, "w");
					FOutFlowCO = fopen(salGastoCO, "w");
					FOutFlowFuel = fopen(salGastoCombustible, "w");
					FOutFlowN2 = fopen(salGastoN2, "w");
				}
				break;
			}

		}
	}
}

void TOutputResults::HeaderSpaceTimeResults(double thmax, double grmax,
		double agincr, int SpeciesNumber)

		{

	for (unsigned int i = 0; i < FParameterSpaceTime.size(); ++i) {
		switch (FParameterSpaceTime[i]) {
		case 0: // PRESION
			fprintf(FileOutPressure, "%d %d %d\n", STCylinder.size(),
					STPlenum.size(), STPipe.size());
			for (unsigned int j = 0; j < STCylinder.size(); ++j) {
				fprintf(FileOutPressure, "%d ",
						STCylinder[j]->getNumeroCilindro());
			}
			fprintf(FileOutPressure, "\n");
			for (unsigned int j = 0; j < STPlenum.size(); ++j) {
				fprintf(FileOutPressure, "%d ",
						STPlenum[j]->getNumeroDeposito());
			}
			fprintf(FileOutPressure, "\n");
			for (unsigned int j = 0; j < STPipe.size(); ++j) {
				fprintf(FileOutPressure, "%d ", STPipe[j]->getNumeroTubo());
			}
			fprintf(FileOutPressure, "\n");
			for (unsigned int j = 0; j < STPipe.size(); ++j) {
				fprintf(FileOutPressure, "%d ", STPipe[j]->getNin());
			}
			fprintf(FileOutPressure, "\n%d", (int) ((thmax - grmax) / agincr));
			break;
		case 1: // TEMPERATURA
			fprintf(FileOutTemp, "%d %d %d\n", STCylinder.size(),
					STPlenum.size(), STPipe.size());
			for (unsigned int j = 0; j < STCylinder.size(); ++j) {
				fprintf(FileOutTemp, "%d ", STCylinder[j]->getNumeroCilindro());
			}
			fprintf(FileOutTemp, "\n");
			for (unsigned int j = 0; j < STPlenum.size(); ++j) {
				fprintf(FileOutTemp, "%d ", STPlenum[j]->getNumeroDeposito());
			}
			fprintf(FileOutTemp, "\n");
			for (unsigned int j = 0; j < STPipe.size(); ++j) {
				fprintf(FileOutTemp, "%d ", STPipe[j]->getNumeroTubo());
			}
			fprintf(FileOutTemp, "\n");
			for (unsigned int j = 0; j < STPipe.size(); ++j) {
				fprintf(FileOutTemp, "%d ", STPipe[j]->getNin());
			}
			fprintf(FileOutTemp, "\n%d", (int) ((thmax - grmax) / agincr));
			break;
		case 2: // VELOCIDAD
			fprintf(FileOutVel, "%d %d %d\n", STCylinder.size(),
					STPlenum.size(), STPipe.size());
			for (unsigned int j = 0; j < STCylinder.size(); ++j) {
				fprintf(FileOutVel, "%d ", STCylinder[j]->getNumeroCilindro());
			}
			fprintf(FileOutVel, "\n");
			for (unsigned int j = 0; j < STPlenum.size(); ++j) {
				fprintf(FileOutVel, "%d ", STPlenum[j]->getNumeroDeposito());
			}
			fprintf(FileOutVel, "\n");
			for (unsigned int j = 0; j < STPipe.size(); ++j) {
				fprintf(FileOutVel, "%d ", STPipe[j]->getNumeroTubo());
			}
			fprintf(FileOutVel, "\n");
			for (unsigned int j = 0; j < STPipe.size(); ++j) {
				fprintf(FileOutVel, "%d ", STPipe[j]->getNin());
			}
			fprintf(FileOutVel, "\n%d", (int) ((thmax - grmax) / agincr));
			break;
		case 3: // GASTO
			fprintf(FileOutFlow, "%d %d %d\n", STCylinder.size(),
					STPlenum.size(), STPipe.size());
			for (unsigned int j = 0; j < STCylinder.size(); ++j) {
				fprintf(FileOutFlow, "%d ", STCylinder[j]->getNumeroCilindro());
			}
			fprintf(FileOutFlow, "\n");
			for (unsigned int j = 0; j < STPlenum.size(); ++j) {
				fprintf(FileOutFlow, "%d ", STPlenum[j]->getNumeroDeposito());
			}
			fprintf(FileOutFlow, "\n");
			for (unsigned int j = 0; j < STPipe.size(); ++j) {
				fprintf(FileOutFlow, "%d ", STPipe[j]->getNumeroTubo());
			}
			fprintf(FileOutFlow, "\n");
			for (unsigned int j = 0; j < STPipe.size(); ++j) {
				fprintf(FileOutFlow, "%d ", STPipe[j]->getNin());
			}
			fprintf(FileOutFlow, "\n%d", (int) ((thmax - grmax) / agincr));
			break;
		case 4: // FRACCION MASICA DE ESPECIES
			if (SpeciesNumber == 3) {
				fprintf(FOutYBurntGas, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutYFreshAir, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				for (unsigned int j = 0; j < STCylinder.size(); ++j) {
					fprintf(FOutYBurntGas, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutYFreshAir, "%d ",
							STCylinder[j]->getNumeroCilindro());
				}
				fprintf(FOutYBurntGas, "\n");
				fprintf(FOutYFreshAir, "\n");
				for (unsigned int j = 0; j < STPlenum.size(); ++j) {
					fprintf(FOutYBurntGas, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutYFreshAir, "%d ",
							STPlenum[j]->getNumeroDeposito());
				}
				fprintf(FOutYBurntGas, "\n");
				fprintf(FOutYFreshAir, "\n");
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					fprintf(FOutYBurntGas, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutYFreshAir, "%d ", STPipe[j]->getNumeroTubo());
				}
				fprintf(FOutYBurntGas, "\n");
				fprintf(FOutYFreshAir, "\n");
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					fprintf(FOutYBurntGas, "%d ", STPipe[j]->getNin());
					fprintf(FOutYFreshAir, "%d ", STPipe[j]->getNin());
				}
				fprintf(FOutYBurntGas, "\n%d",
						(int) ((thmax - grmax) / agincr));
				fprintf(FOutYFreshAir, "\n%d",
						(int) ((thmax - grmax) / agincr));

			} else if (SpeciesNumber == 4) {
				fprintf(FOutYBurntGas, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutYFuel, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutYFreshAir, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				for (unsigned int j = 0; j < STCylinder.size(); ++j) {
					fprintf(FOutYBurntGas, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutYFuel, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutYFreshAir, "%d ",
							STCylinder[j]->getNumeroCilindro());
				}
				fprintf(FOutYBurntGas, "\n");
				fprintf(FOutYFuel, "\n");
				fprintf(FOutYFreshAir, "\n");
				for (unsigned int j = 0; j < STPlenum.size(); ++j) {
					fprintf(FOutYBurntGas, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutYFuel, "%d ", STPlenum[j]->getNumeroDeposito());
					fprintf(FOutYFreshAir, "%d ",
							STPlenum[j]->getNumeroDeposito());
				}
				fprintf(FOutYBurntGas, "\n");
				fprintf(FOutYFuel, "\n");
				fprintf(FOutYFreshAir, "\n");
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					fprintf(FOutYBurntGas, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutYFuel, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutYFreshAir, "%d ", STPipe[j]->getNumeroTubo());
				}
				fprintf(FOutYBurntGas, "\n");
				fprintf(FOutYFuel, "\n");
				fprintf(FOutYFreshAir, "\n");
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					fprintf(FOutYBurntGas, "%d ", STPipe[j]->getNin());
					fprintf(FOutYFuel, "%d ", STPipe[j]->getNin());
					fprintf(FOutYFreshAir, "%d ", STPipe[j]->getNin());
				}
				fprintf(FOutYBurntGas, "\n%d",
						(int) ((thmax - grmax) / agincr));
				fprintf(FOutYFuel, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutYFreshAir, "\n%d",
						(int) ((thmax - grmax) / agincr));

			} else if (SpeciesNumber == 9) {
				fprintf(FOutYO2, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutYCO2, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutYH2O, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutYHC, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutYSoot, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutYNOx, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutYCO, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutYN2, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				for (unsigned int j = 0; j < STCylinder.size(); ++j) {
					fprintf(FOutYO2, "%d ", STCylinder[j]->getNumeroCilindro());
					fprintf(FOutYCO2, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutYH2O, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutYHC, "%d ", STCylinder[j]->getNumeroCilindro());
					fprintf(FOutYSoot, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutYNOx, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutYCO, "%d ", STCylinder[j]->getNumeroCilindro());
					fprintf(FOutYN2, "%d ", STCylinder[j]->getNumeroCilindro());
				}
				fprintf(FOutYO2, "\n");
				fprintf(FOutYCO2, "\n");
				fprintf(FOutYH2O, "\n");
				fprintf(FOutYHC, "\n");
				fprintf(FOutYSoot, "\n");
				fprintf(FOutYNOx, "\n");
				fprintf(FOutYCO, "\n");
				fprintf(FOutYN2, "\n");
				for (unsigned int j = 0; j < STPlenum.size(); ++j) {
					fprintf(FOutYO2, "%d ", STPlenum[j]->getNumeroDeposito());
					fprintf(FOutYCO2, "%d ", STPlenum[j]->getNumeroDeposito());
					fprintf(FOutYH2O, "%d ", STPlenum[j]->getNumeroDeposito());
					fprintf(FOutYHC, "%d ", STPlenum[j]->getNumeroDeposito());
					fprintf(FOutYSoot, "%d ", STPlenum[j]->getNumeroDeposito());
					fprintf(FOutYNOx, "%d ", STPlenum[j]->getNumeroDeposito());
					fprintf(FOutYCO, "%d ", STPlenum[j]->getNumeroDeposito());
					fprintf(FOutYN2, "%d ", STPlenum[j]->getNumeroDeposito());
				}
				fprintf(FOutYO2, "\n");
				fprintf(FOutYCO2, "\n");
				fprintf(FOutYH2O, "\n");
				fprintf(FOutYHC, "\n");
				fprintf(FOutYSoot, "\n");
				fprintf(FOutYNOx, "\n");
				fprintf(FOutYCO, "\n");
				fprintf(FOutYN2, "\n");
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					fprintf(FOutYO2, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutYCO2, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutYH2O, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutYHC, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutYSoot, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutYNOx, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutYCO, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutYN2, "%d ", STPipe[j]->getNumeroTubo());
				}
				fprintf(FOutYO2, "\n");
				fprintf(FOutYCO2, "\n");
				fprintf(FOutYH2O, "\n");
				fprintf(FOutYHC, "\n");
				fprintf(FOutYSoot, "\n");
				fprintf(FOutYNOx, "\n");
				fprintf(FOutYCO, "\n");
				fprintf(FOutYN2, "\n");
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					fprintf(FOutYO2, "%d ", STPipe[j]->getNin());
					fprintf(FOutYCO2, "%d ", STPipe[j]->getNin());
					fprintf(FOutYH2O, "%d ", STPipe[j]->getNin());
					fprintf(FOutYHC, "%d ", STPipe[j]->getNin());
					fprintf(FOutYSoot, "%d ", STPipe[j]->getNin());
					fprintf(FOutYNOx, "%d ", STPipe[j]->getNin());
					fprintf(FOutYCO, "%d ", STPipe[j]->getNin());
					fprintf(FOutYN2, "%d ", STPipe[j]->getNin());
				}
				fprintf(FOutYO2, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutYCO2, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutYH2O, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutYHC, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutYSoot, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutYNOx, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutYCO, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutYN2, "\n%d", (int) ((thmax - grmax) / agincr));

			} else if (SpeciesNumber == 10) {
				fprintf(FOutYO2, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutYCO2, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutYH2O, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutYHC, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutYSoot, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutYNOx, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutYCO, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutYFuel, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutYN2, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				for (unsigned int j = 0; j < STCylinder.size(); ++j) {
					fprintf(FOutYO2, "%d ", STCylinder[j]->getNumeroCilindro());
					fprintf(FOutYCO2, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutYH2O, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutYHC, "%d ", STCylinder[j]->getNumeroCilindro());
					fprintf(FOutYSoot, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutYNOx, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutYCO, "%d ", STCylinder[j]->getNumeroCilindro());
					fprintf(FOutYFuel, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutYN2, "%d ", STCylinder[j]->getNumeroCilindro());
				}
				fprintf(FOutYO2, "\n");
				fprintf(FOutYCO2, "\n");
				fprintf(FOutYH2O, "\n");
				fprintf(FOutYHC, "\n");
				fprintf(FOutYSoot, "\n");
				fprintf(FOutYNOx, "\n");
				fprintf(FOutYCO, "\n");
				fprintf(FOutYFuel, "\n");
				fprintf(FOutYN2, "\n");
				for (unsigned int j = 0; j < STPlenum.size(); ++j) {
					fprintf(FOutYO2, "%d ", STPlenum[j]->getNumeroDeposito());
					fprintf(FOutYCO2, "%d ", STPlenum[j]->getNumeroDeposito());
					fprintf(FOutYH2O, "%d ", STPlenum[j]->getNumeroDeposito());
					fprintf(FOutYHC, "%d ", STPlenum[j]->getNumeroDeposito());
					fprintf(FOutYSoot, "%d ", STPlenum[j]->getNumeroDeposito());
					fprintf(FOutYNOx, "%d ", STPlenum[j]->getNumeroDeposito());
					fprintf(FOutYCO, "%d ", STPlenum[j]->getNumeroDeposito());
					fprintf(FOutYFuel, "%d ", STPlenum[j]->getNumeroDeposito());
					fprintf(FOutYN2, "%d ", STPlenum[j]->getNumeroDeposito());
				}
				fprintf(FOutYO2, "\n");
				fprintf(FOutYCO2, "\n");
				fprintf(FOutYH2O, "\n");
				fprintf(FOutYHC, "\n");
				fprintf(FOutYSoot, "\n");
				fprintf(FOutYNOx, "\n");
				fprintf(FOutYCO, "\n");
				fprintf(FOutYFuel, "\n");
				fprintf(FOutYN2, "\n");
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					fprintf(FOutYO2, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutYCO2, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutYH2O, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutYHC, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutYSoot, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutYNOx, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutYCO, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutYFuel, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutYN2, "%d ", STPipe[j]->getNumeroTubo());
				}
				fprintf(FOutYO2, "\n");
				fprintf(FOutYCO2, "\n");
				fprintf(FOutYH2O, "\n");
				fprintf(FOutYHC, "\n");
				fprintf(FOutYSoot, "\n");
				fprintf(FOutYNOx, "\n");
				fprintf(FOutYCO, "\n");
				fprintf(FOutYFuel, "\n");
				fprintf(FOutYN2, "\n");
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					fprintf(FOutYO2, "%d ", STPipe[j]->getNin());
					fprintf(FOutYCO2, "%d ", STPipe[j]->getNin());
					fprintf(FOutYH2O, "%d ", STPipe[j]->getNin());
					fprintf(FOutYHC, "%d ", STPipe[j]->getNin());
					fprintf(FOutYSoot, "%d ", STPipe[j]->getNin());
					fprintf(FOutYNOx, "%d ", STPipe[j]->getNin());
					fprintf(FOutYCO, "%d ", STPipe[j]->getNin());
					fprintf(FOutYFuel, "%d ", STPipe[j]->getNin());
					fprintf(FOutYN2, "%d ", STPipe[j]->getNin());
				}
				fprintf(FOutYO2, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutYCO2, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutYH2O, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutYHC, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutYSoot, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutYNOx, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutYCO, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutYFuel, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutYN2, "\n%d", (int) ((thmax - grmax) / agincr));
			}
			break;
		case 5: // GASTO MASICO DE ESPECIES
			if (SpeciesNumber == 3) {
				fprintf(FOutFlowBurntGas, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutFlowFreshAir, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				for (unsigned int j = 0; j < STCylinder.size(); ++j) {
					fprintf(FOutFlowBurntGas, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutFlowFreshAir, "%d ",
							STCylinder[j]->getNumeroCilindro());
				}
				fprintf(FOutFlowBurntGas, "\n");
				fprintf(FOutFlowFreshAir, "\n");
				for (unsigned int j = 0; j < STPlenum.size(); ++j) {
					fprintf(FOutFlowBurntGas, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutFlowFreshAir, "%d ",
							STPlenum[j]->getNumeroDeposito());
				}
				fprintf(FOutFlowBurntGas, "\n");
				fprintf(FOutFlowFreshAir, "\n");
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					fprintf(FOutFlowBurntGas, "%d ",
							STPipe[j]->getNumeroTubo());
					fprintf(FOutFlowFreshAir, "%d ",
							STPipe[j]->getNumeroTubo());
				}
				fprintf(FOutFlowBurntGas, "\n");
				fprintf(FOutFlowFreshAir, "\n");
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					fprintf(FOutFlowBurntGas, "%d ", STPipe[j]->getNin());
					fprintf(FOutFlowFreshAir, "%d ", STPipe[j]->getNin());
				}
				fprintf(FOutFlowBurntGas, "\n%d",
						(int) ((thmax - grmax) / agincr));
				fprintf(FOutFlowFreshAir, "\n%d",
						(int) ((thmax - grmax) / agincr));

			} else if (SpeciesNumber == 4) {
				fprintf(FOutFlowBurntGas, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutFlowFuel, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutFlowFreshAir, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				for (unsigned int j = 0; j < STCylinder.size(); ++j) {
					fprintf(FOutFlowBurntGas, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutFlowFuel, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutFlowFreshAir, "%d ",
							STCylinder[j]->getNumeroCilindro());
				}
				fprintf(FOutFlowBurntGas, "\n");
				fprintf(FOutFlowFuel, "\n");
				fprintf(FOutFlowFreshAir, "\n");
				for (unsigned int j = 0; j < STPlenum.size(); ++j) {
					fprintf(FOutFlowBurntGas, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutFlowFuel, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutFlowFreshAir, "%d ",
							STPlenum[j]->getNumeroDeposito());
				}
				fprintf(FOutFlowBurntGas, "\n");
				fprintf(FOutFlowFuel, "\n");
				fprintf(FOutFlowFreshAir, "\n");
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					fprintf(FOutFlowBurntGas, "%d ",
							STPipe[j]->getNumeroTubo());
					fprintf(FOutFlowFuel, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutFlowFreshAir, "%d ",
							STPipe[j]->getNumeroTubo());
				}
				fprintf(FOutFlowBurntGas, "\n");
				fprintf(FOutFlowFuel, "\n");
				fprintf(FOutFlowFreshAir, "\n");
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					fprintf(FOutFlowBurntGas, "%d ", STPipe[j]->getNin());
					fprintf(FOutFlowFuel, "%d ", STPipe[j]->getNin());
					fprintf(FOutFlowFreshAir, "%d ", STPipe[j]->getNin());
				}
				fprintf(FOutFlowBurntGas, "\n%d",
						(int) ((thmax - grmax) / agincr));
				fprintf(FOutFlowFuel, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutFlowFreshAir, "\n%d",
						(int) ((thmax - grmax) / agincr));

			} else if (SpeciesNumber == 9) {
				fprintf(FOutFlowO2, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutFlowCO2, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutFlowH2O, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutFlowHC, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutFlowSoot, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutFlowNOx, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutFlowCO, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutFlowN2, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				for (unsigned int j = 0; j < STCylinder.size(); ++j) {
					fprintf(FOutFlowO2, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutFlowCO2, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutFlowH2O, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutFlowHC, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutFlowSoot, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutFlowNOx, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutFlowCO, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutFlowN2, "%d ",
							STCylinder[j]->getNumeroCilindro());
				}
				fprintf(FOutFlowO2, "\n");
				fprintf(FOutFlowCO2, "\n");
				fprintf(FOutFlowH2O, "\n");
				fprintf(FOutFlowHC, "\n");
				fprintf(FOutFlowSoot, "\n");
				fprintf(FOutFlowNOx, "\n");
				fprintf(FOutFlowCO, "\n");
				fprintf(FOutFlowN2, "\n");
				for (unsigned int j = 0; j < STPlenum.size(); ++j) {
					fprintf(FOutFlowO2, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutFlowCO2, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutFlowH2O, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutFlowHC, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutFlowSoot, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutFlowNOx, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutFlowCO, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutFlowN2, "%d ",
							STPlenum[j]->getNumeroDeposito());
				}
				fprintf(FOutFlowO2, "\n");
				fprintf(FOutFlowCO2, "\n");
				fprintf(FOutFlowH2O, "\n");
				fprintf(FOutFlowHC, "\n");
				fprintf(FOutFlowSoot, "\n");
				fprintf(FOutFlowNOx, "\n");
				fprintf(FOutFlowCO, "\n");
				fprintf(FOutFlowN2, "\n");
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					fprintf(FOutFlowO2, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutFlowCO2, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutFlowH2O, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutFlowHC, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutFlowSoot, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutFlowNOx, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutFlowCO, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutFlowN2, "%d ", STPipe[j]->getNumeroTubo());
				}
				fprintf(FOutFlowO2, "\n");
				fprintf(FOutFlowCO2, "\n");
				fprintf(FOutFlowH2O, "\n");
				fprintf(FOutFlowHC, "\n");
				fprintf(FOutFlowSoot, "\n");
				fprintf(FOutFlowNOx, "\n");
				fprintf(FOutFlowCO, "\n");
				fprintf(FOutFlowN2, "\n");
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					fprintf(FOutFlowO2, "%d ", STPipe[j]->getNin());
					fprintf(FOutFlowCO2, "%d ", STPipe[j]->getNin());
					fprintf(FOutFlowH2O, "%d ", STPipe[j]->getNin());
					fprintf(FOutFlowHC, "%d ", STPipe[j]->getNin());
					fprintf(FOutFlowSoot, "%d ", STPipe[j]->getNin());
					fprintf(FOutFlowNOx, "%d ", STPipe[j]->getNin());
					fprintf(FOutFlowCO, "%d ", STPipe[j]->getNin());
					fprintf(FOutFlowN2, "%d ", STPipe[j]->getNin());
				}
				fprintf(FOutFlowO2, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutFlowCO2, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutFlowH2O, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutFlowHC, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutFlowSoot, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutFlowNOx, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutFlowCO, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutFlowN2, "\n%d", (int) ((thmax - grmax) / agincr));

			} else if (SpeciesNumber == 10) {
				fprintf(FOutFlowO2, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutFlowCO2, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutFlowH2O, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutFlowHC, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutFlowSoot, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutFlowNOx, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutFlowCO, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutFlowFuel, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				fprintf(FOutFlowN2, "%d %d %d\n", STCylinder.size(),
						STPlenum.size(), STPipe.size());
				for (unsigned int j = 0; j < STCylinder.size(); ++j) {
					fprintf(FOutFlowO2, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutFlowCO2, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutFlowH2O, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutFlowHC, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutFlowSoot, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutFlowNOx, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutFlowCO, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutFlowFuel, "%d ",
							STCylinder[j]->getNumeroCilindro());
					fprintf(FOutFlowN2, "%d ",
							STCylinder[j]->getNumeroCilindro());
				}
				fprintf(FOutFlowO2, "\n");
				fprintf(FOutFlowCO2, "\n");
				fprintf(FOutFlowH2O, "\n");
				fprintf(FOutFlowHC, "\n");
				fprintf(FOutFlowSoot, "\n");
				fprintf(FOutFlowNOx, "\n");
				fprintf(FOutFlowCO, "\n");
				fprintf(FOutFlowFuel, "\n");
				fprintf(FOutFlowN2, "\n");
				for (unsigned int j = 0; j < STPlenum.size(); ++j) {
					fprintf(FOutFlowO2, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutFlowCO2, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutFlowH2O, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutFlowHC, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutFlowSoot, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutFlowNOx, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutFlowCO, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutFlowFuel, "%d ",
							STPlenum[j]->getNumeroDeposito());
					fprintf(FOutFlowN2, "%d ",
							STPlenum[j]->getNumeroDeposito());
				}
				fprintf(FOutFlowO2, "\n");
				fprintf(FOutFlowCO2, "\n");
				fprintf(FOutFlowH2O, "\n");
				fprintf(FOutFlowHC, "\n");
				fprintf(FOutFlowSoot, "\n");
				fprintf(FOutFlowNOx, "\n");
				fprintf(FOutFlowCO, "\n");
				fprintf(FOutFlowFuel, "\n");
				fprintf(FOutFlowN2, "\n");
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					fprintf(FOutFlowO2, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutFlowCO2, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutFlowH2O, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutFlowHC, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutFlowSoot, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutFlowNOx, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutFlowCO, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutFlowFuel, "%d ", STPipe[j]->getNumeroTubo());
					fprintf(FOutFlowN2, "%d ", STPipe[j]->getNumeroTubo());
				}
				fprintf(FOutFlowO2, "\n");
				fprintf(FOutFlowCO2, "\n");
				fprintf(FOutFlowH2O, "\n");
				fprintf(FOutFlowHC, "\n");
				fprintf(FOutFlowSoot, "\n");
				fprintf(FOutFlowNOx, "\n");
				fprintf(FOutFlowCO, "\n");
				fprintf(FOutFlowFuel, "\n");
				fprintf(FOutFlowN2, "\n");
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					fprintf(FOutFlowO2, "%d ", STPipe[j]->getNin());
					fprintf(FOutFlowCO2, "%d ", STPipe[j]->getNin());
					fprintf(FOutFlowH2O, "%d ", STPipe[j]->getNin());
					fprintf(FOutFlowHC, "%d ", STPipe[j]->getNin());
					fprintf(FOutFlowSoot, "%d ", STPipe[j]->getNin());
					fprintf(FOutFlowNOx, "%d ", STPipe[j]->getNin());
					fprintf(FOutFlowCO, "%d ", STPipe[j]->getNin());
					fprintf(FOutFlowFuel, "%d ", STPipe[j]->getNin());
					fprintf(FOutFlowN2, "%d ", STPipe[j]->getNin());
				}
				fprintf(FOutFlowO2, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutFlowCO2, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutFlowH2O, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutFlowHC, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutFlowSoot, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutFlowNOx, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutFlowCO, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutFlowFuel, "\n%d", (int) ((thmax - grmax) / agincr));
				fprintf(FOutFlowN2, "\n%d", (int) ((thmax - grmax) / agincr));
			}
			break;
		}
	}
}

void TOutputResults::PrintSpaceTimeResults(bool EngineBlock, double Theta,
		double SimulationDuration, TBloqueMotor **Engine, int SpeciesNumber)

		{

	if (FWriteSpaceTime) {
		float pasafloat = 0.;
		double m, RegimenFicticio;

		if (EngineBlock) {
			m = floor(Theta / Engine[0]->getAngTotalCiclo());
			pasafloat = (float) (Theta - (m * Engine[0]->getAngTotalCiclo()));
		} else {
			RegimenFicticio = 720. / 6. / SimulationDuration;
			m = floor(Theta / 720.);
			pasafloat = (float) (Theta - (m * 720.));
			pasafloat = pasafloat / 6. / RegimenFicticio;
		}
		for (unsigned int i = 0; i < FParameterSpaceTime.size(); ++i) {
			switch (FParameterSpaceTime[i]) {
			case 0:
				fprintf(FileOutPressure, "\n%g", pasafloat);
				for (unsigned int j = 0; j < STCylinder.size(); ++j) {
					fprintf(FileOutPressure, " %g",
							STCylinder[j]->getPressure());
				}
				for (unsigned int j = 0; j < STPlenum.size(); ++j) {
					fprintf(FileOutPressure, " %g", STPlenum[j]->getPressure());
				}
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					for (int k = 0; k < STPipe[j]->getNin(); ++k) {
						fprintf(FileOutPressure, " %g",
								STPipe[j]->GetPresion(k));
					}
				}
				break;
			case 1:
				fprintf(FileOutTemp, "\n%g", pasafloat);
				for (unsigned int j = 0; j < STCylinder.size(); ++j) {
					fprintf(FileOutTemp, " %g",
							STCylinder[j]->getTemperature());
				}
				for (unsigned int j = 0; j < STPlenum.size(); ++j) {
					fprintf(FileOutTemp, " %g", STPlenum[j]->getTemperature());
				}
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					for (int k = 0; k < STPipe[j]->getNin(); ++k) {
						double temp = pow2(STPipe[j]->GetAsonido(k) * ARef)
								/ (STPipe[j]->GetGamma(k)
										* STPipe[j]->GetRMezcla(k)) - 273.;
						fprintf(FileOutTemp, " %g", temp);
					}
				}
				break;
			case 2:
				fprintf(FileOutVel, "\n%g", pasafloat);
				for (unsigned int j = 0; j < STCylinder.size(); ++j) {
					fprintf(FileOutVel, " %g", 0.0);
				}
				for (unsigned int j = 0; j < STPlenum.size(); ++j) {
					fprintf(FileOutVel, " %g", 0.0);
				}
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					for (int k = 0; k < STPipe[j]->getNin(); ++k) {
						double vel = STPipe[j]->GetVelocidad(k) * ARef;
						fprintf(FileOutVel, " %g", vel);
					}
				}
				break;
			case 3:
				fprintf(FileOutFlow, "\n%g", pasafloat);
				for (unsigned int j = 0; j < STCylinder.size(); ++j) {
					fprintf(FileOutFlow, " %g", 0.0);
				}
				for (unsigned int j = 0; j < STPlenum.size(); ++j) {
					fprintf(FileOutFlow, " %g", 0.0);
				}
				for (unsigned int j = 0; j < STPipe.size(); ++j) {
					for (int k = 0; k < STPipe[j]->getNin(); ++k) {
						if (STPipe[j]->getFormulacionLeyes() == nmConArea) {
							fprintf(FileOutFlow, " %g", STPipe[j]->GetU0(1, k));
						} else {
							double massflow = STPipe[j]->GetPresion(k) * 1e5
									/ pow2(STPipe[j]->GetAsonido(k) * ARef)
									* STPipe[j]->GetGamma(k)
									* STPipe[j]->GetVelocidad(k) * ARef
									* STPipe[j]->GetDiametro(k) * Pi / 4.;
							fprintf(FileOutFlow, " %g", massflow);
						}
					}
				}
				break;
			case 4:
				if (SpeciesNumber == 3) {
					fprintf(FOutYBurntGas, "\n%g", pasafloat);
					fprintf(FOutYFreshAir, "\n%g", pasafloat);
					for (unsigned int j = 0; j < STCylinder.size(); ++j) {
						fprintf(FOutYBurntGas, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(0));
						fprintf(FOutYFreshAir, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(1));
					}
					for (unsigned int j = 0; j < STPlenum.size(); ++j) {
						fprintf(FOutYBurntGas, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(0));
						fprintf(FOutYFreshAir, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(1));
					}
					for (unsigned int j = 0; j < STPipe.size(); ++j) {
						for (int k = 0; k < STPipe[j]->getNin(); ++k) {
							fprintf(FOutYBurntGas, " %g",
									STPipe[j]->GetFraccionMasica(k, 0));
							fprintf(FOutYFreshAir, " %g",
									STPipe[j]->GetFraccionMasica(k, 1));
						}
					}

				} else if (SpeciesNumber == 4) {
					fprintf(FOutYBurntGas, "\n%g", pasafloat);
					fprintf(FOutYFuel, "\n%g", pasafloat);
					fprintf(FOutYFreshAir, "\n%g", pasafloat);
					for (unsigned int j = 0; j < STCylinder.size(); ++j) {
						fprintf(FOutYBurntGas, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(0));
						fprintf(FOutYFuel, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(1));
						fprintf(FOutYFreshAir, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(2));
					}
					for (unsigned int j = 0; j < STPlenum.size(); ++j) {
						fprintf(FOutYBurntGas, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(0));
						fprintf(FOutYFuel, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(1));
						fprintf(FOutYFreshAir, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(2));
					}
					for (unsigned int j = 0; j < STPipe.size(); ++j) {
						for (int k = 0; k < STPipe[j]->getNin(); ++k) {
							fprintf(FOutYBurntGas, " %g",
									STPipe[j]->GetFraccionMasica(k, 0));
							fprintf(FOutYFuel, " %g",
									STPipe[j]->GetFraccionMasica(k, 1));
							fprintf(FOutYFreshAir, " %g",
									STPipe[j]->GetFraccionMasica(k, 2));
						}
					}

				} else if (SpeciesNumber == 9) {
					fprintf(FOutYO2, "\n%g", pasafloat);
					fprintf(FOutYCO2, "\n%g", pasafloat);
					fprintf(FOutYH2O, "\n%g", pasafloat);
					fprintf(FOutYHC, "\n%g", pasafloat);
					fprintf(FOutYSoot, "\n%g", pasafloat);
					fprintf(FOutYNOx, "\n%g", pasafloat);
					fprintf(FOutYCO, "\n%g", pasafloat);
					fprintf(FOutYN2, "\n%g", pasafloat);
					for (unsigned int j = 0; j < STCylinder.size(); ++j) {
						fprintf(FOutYO2, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(0));
						fprintf(FOutYCO2, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(1));
						fprintf(FOutYH2O, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(2));
						fprintf(FOutYHC, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(3));
						fprintf(FOutYSoot, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(4));
						fprintf(FOutYNOx, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(5));
						fprintf(FOutYCO, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(6));
						fprintf(FOutYN2, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(7));
					}
					for (unsigned int j = 0; j < STPlenum.size(); ++j) {
						fprintf(FOutYO2, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(0));
						fprintf(FOutYCO2, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(1));
						fprintf(FOutYH2O, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(2));
						fprintf(FOutYHC, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(3));
						fprintf(FOutYSoot, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(4));
						fprintf(FOutYNOx, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(5));
						fprintf(FOutYCO, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(6));
						fprintf(FOutYN2, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(7));
					}
					for (unsigned int j = 0; j < STPipe.size(); ++j) {
						for (int k = 0; k < STPipe[j]->getNin(); ++k) {
							fprintf(FOutYO2, " %g",
									STPipe[j]->GetFraccionMasica(k, 0));
							fprintf(FOutYCO2, " %g",
									STPipe[j]->GetFraccionMasica(k, 1));
							fprintf(FOutYH2O, " %g",
									STPipe[j]->GetFraccionMasica(k, 2));
							fprintf(FOutYHC, " %g",
									STPipe[j]->GetFraccionMasica(k, 3));
							fprintf(FOutYSoot, " %g",
									STPipe[j]->GetFraccionMasica(k, 4));
							fprintf(FOutYNOx, " %g",
									STPipe[j]->GetFraccionMasica(k, 5));
							fprintf(FOutYCO, " %g",
									STPipe[j]->GetFraccionMasica(k, 6));
							fprintf(FOutYN2, " %g",
									STPipe[j]->GetFraccionMasica(k, 7));
						}
					}

				} else if (SpeciesNumber == 10) {
					fprintf(FOutYO2, "\n%g", pasafloat);
					fprintf(FOutYCO2, "\n%g", pasafloat);
					fprintf(FOutYH2O, "\n%g", pasafloat);
					fprintf(FOutYHC, "\n%g", pasafloat);
					fprintf(FOutYSoot, "\n%g", pasafloat);
					fprintf(FOutYNOx, "\n%g", pasafloat);
					fprintf(FOutYCO, "\n%g", pasafloat);
					fprintf(FOutYFuel, "\n%g", pasafloat);
					fprintf(FOutYN2, "\n%g", pasafloat);
					for (unsigned int j = 0; j < STCylinder.size(); ++j) {
						fprintf(FOutYO2, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(0));
						fprintf(FOutYCO2, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(1));
						fprintf(FOutYH2O, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(2));
						fprintf(FOutYHC, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(3));
						fprintf(FOutYSoot, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(4));
						fprintf(FOutYNOx, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(5));
						fprintf(FOutYCO, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(6));
						fprintf(FOutYFuel, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(7));
						fprintf(FOutYN2, " %g",
								STCylinder[j]->GetFraccionMasicaEspecie(8));
					}
					for (unsigned int j = 0; j < STPlenum.size(); ++j) {
						fprintf(FOutYO2, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(0));
						fprintf(FOutYCO2, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(1));
						fprintf(FOutYH2O, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(2));
						fprintf(FOutYHC, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(3));
						fprintf(FOutYSoot, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(4));
						fprintf(FOutYNOx, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(5));
						fprintf(FOutYCO, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(6));
						fprintf(FOutYFuel, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(7));
						fprintf(FOutYN2, " %g",
								STPlenum[j]->GetFraccionMasicaEspecie(8));
					}
					for (unsigned int j = 0; j < STPipe.size(); ++j) {
						for (int k = 0; k < STPipe[j]->getNin(); ++k) {
							fprintf(FOutYO2, " %g",
									STPipe[j]->GetFraccionMasica(k, 0));
							fprintf(FOutYCO2, " %g",
									STPipe[j]->GetFraccionMasica(k, 1));
							fprintf(FOutYH2O, " %g",
									STPipe[j]->GetFraccionMasica(k, 2));
							fprintf(FOutYHC, " %g",
									STPipe[j]->GetFraccionMasica(k, 3));
							fprintf(FOutYSoot, " %g",
									STPipe[j]->GetFraccionMasica(k, 4));
							fprintf(FOutYNOx, " %g",
									STPipe[j]->GetFraccionMasica(k, 5));
							fprintf(FOutYCO, " %g",
									STPipe[j]->GetFraccionMasica(k, 6));
							fprintf(FOutYFuel, " %g",
									STPipe[j]->GetFraccionMasica(k, 7));
							fprintf(FOutYN2, " %g",
									STPipe[j]->GetFraccionMasica(k, 8));
						}
					}
				}
				break;
			case 5:
				double massflow;
				if (SpeciesNumber == 3) {
					fprintf(FOutFlowBurntGas, "\n%g", pasafloat);
					fprintf(FOutFlowFreshAir, "\n%g", pasafloat);
					for (unsigned int j = 0; j < STCylinder.size(); ++j) {
						fprintf(FOutFlowBurntGas, " %g", 0.0);
						fprintf(FOutFlowFreshAir, " %g", 0.0);
					}
					for (unsigned int j = 0; j < STPlenum.size(); ++j) {
						fprintf(FOutFlowBurntGas, " %g", 0.0);
						fprintf(FOutFlowFreshAir, " %g", 0.0);
					}
					for (unsigned int j = 0; j < STPipe.size(); ++j) {
						for (int k = 0; k < STPipe[j]->getNin(); ++k) {
							if (STPipe[j]->getFormulacionLeyes() == nmConArea) {
								massflow = STPipe[j]->GetU0(1, k);
							} else {
								massflow = STPipe[j]->GetPresion(k) * 1e5
										/ pow2(STPipe[j]->GetAsonido(k) * ARef)
										* STPipe[j]->GetGamma(k)
										* STPipe[j]->GetVelocidad(k) * ARef
										* STPipe[j]->GetDiametro(k) * Pi / 4.;
							}
							fprintf(FOutFlowBurntGas, " %g",
									STPipe[j]->GetFraccionMasica(k, 0)
											* massflow);
							fprintf(FOutFlowFreshAir, " %g",
									STPipe[j]->GetFraccionMasica(k, 1)
											* massflow);
						}
					}

				} else if (SpeciesNumber == 4) {
					fprintf(FOutFlowBurntGas, "\n%g", pasafloat);
					fprintf(FOutFlowFuel, "\n%g", pasafloat);
					fprintf(FOutFlowFreshAir, "\n%g", pasafloat);
					for (unsigned int j = 0; j < STCylinder.size(); ++j) {
						fprintf(FOutFlowBurntGas, " %g", 0.0);
						fprintf(FOutFlowFuel, " %g", 0.0);
						fprintf(FOutFlowFreshAir, " %g", 0.0);
					}
					for (unsigned int j = 0; j < STPlenum.size(); ++j) {
						fprintf(FOutFlowBurntGas, " %g", 0.0);
						fprintf(FOutFlowFuel, " %g", 0.0);
						fprintf(FOutFlowFreshAir, " %g", 0.0);
					}
					for (unsigned int j = 0; j < STPipe.size(); ++j) {
						for (int k = 0; k < STPipe[j]->getNin(); ++k) {
							if (STPipe[j]->getFormulacionLeyes() == nmConArea) {
								massflow = STPipe[j]->GetU0(1, k);
							} else {
								massflow = STPipe[j]->GetPresion(k) * 1e5
										/ pow2(STPipe[j]->GetAsonido(k) * ARef)
										* STPipe[j]->GetGamma(k)
										* STPipe[j]->GetVelocidad(k) * ARef
										* STPipe[j]->GetDiametro(k) * Pi / 4.;
							}
							fprintf(FOutFlowBurntGas, " %g",
									STPipe[j]->GetFraccionMasica(k, 0)
											* massflow);
							fprintf(FOutFlowFuel, " %g",
									STPipe[j]->GetFraccionMasica(k, 1)
											* massflow);
							fprintf(FOutFlowFreshAir, " %g",
									STPipe[j]->GetFraccionMasica(k, 2)
											* massflow);
						}
					}

				} else if (SpeciesNumber == 9) {
					fprintf(FOutFlowO2, "\n%g", pasafloat);
					fprintf(FOutFlowCO2, "\n%g", pasafloat);
					fprintf(FOutFlowH2O, "\n%g", pasafloat);
					fprintf(FOutFlowHC, "\n%g", pasafloat);
					fprintf(FOutFlowSoot, "\n%g", pasafloat);
					fprintf(FOutFlowNOx, "\n%g", pasafloat);
					fprintf(FOutFlowCO, "\n%g", pasafloat);
					fprintf(FOutFlowN2, "\n%g", pasafloat);
					for (unsigned int j = 0; j < STCylinder.size(); ++j) {
						fprintf(FOutFlowO2, " %g", 0.0);
						fprintf(FOutFlowCO2, " %g", 0.0);
						fprintf(FOutFlowH2O, " %g", 0.0);
						fprintf(FOutFlowHC, " %g", 0.0);
						fprintf(FOutFlowSoot, " %g", 0.0);
						fprintf(FOutFlowNOx, " %g", 0.0);
						fprintf(FOutFlowCO, " %g", 0.0);
						fprintf(FOutFlowN2, " %g", 0.0);
					}
					for (unsigned int j = 0; j < STPlenum.size(); ++j) {
						fprintf(FOutFlowO2, " %g", 0.0);
						fprintf(FOutFlowCO2, " %g", 0.0);
						fprintf(FOutFlowH2O, " %g", 0.0);
						fprintf(FOutFlowHC, " %g", 0.0);
						fprintf(FOutFlowSoot, " %g", 0.0);
						fprintf(FOutFlowNOx, " %g", 0.0);
						fprintf(FOutFlowCO, " %g", 0.0);
						fprintf(FOutFlowN2, " %g", 0.0);
					}
					for (unsigned int j = 0; j < STPipe.size(); ++j) {
						for (int k = 0; k < STPipe[j]->getNin(); ++k) {
							if (STPipe[j]->getFormulacionLeyes() == nmConArea) {
								massflow = STPipe[j]->GetU0(1, k);
							} else {
								massflow = STPipe[j]->GetPresion(k) * 1e5
										/ pow2(STPipe[j]->GetAsonido(k) * ARef)
										* STPipe[j]->GetGamma(k)
										* STPipe[j]->GetVelocidad(k) * ARef
										* STPipe[j]->GetDiametro(k) * Pi / 4.;
							}
							fprintf(FOutFlowO2, " %g",
									STPipe[j]->GetFraccionMasica(k, 0)
											* massflow);
							fprintf(FOutFlowCO2, " %g",
									STPipe[j]->GetFraccionMasica(k, 1)
											* massflow);
							fprintf(FOutFlowH2O, " %g",
									STPipe[j]->GetFraccionMasica(k, 2)
											* massflow);
							fprintf(FOutFlowHC, " %g",
									STPipe[j]->GetFraccionMasica(k, 3)
											* massflow);
							fprintf(FOutFlowSoot, " %g",
									STPipe[j]->GetFraccionMasica(k, 4)
											* massflow);
							fprintf(FOutFlowNOx, " %g",
									STPipe[j]->GetFraccionMasica(k, 5)
											* massflow);
							fprintf(FOutFlowCO, " %g",
									STPipe[j]->GetFraccionMasica(k, 6)
											* massflow);
							fprintf(FOutFlowN2, " %g",
									STPipe[j]->GetFraccionMasica(k, 7)
											* massflow);
						}
					}

				} else if (SpeciesNumber == 10) {
					fprintf(FOutFlowO2, "\n%g", pasafloat);
					fprintf(FOutFlowCO2, "\n%g", pasafloat);
					fprintf(FOutFlowH2O, "\n%g", pasafloat);
					fprintf(FOutFlowHC, "\n%g", pasafloat);
					fprintf(FOutFlowSoot, "\n%g", pasafloat);
					fprintf(FOutFlowNOx, "\n%g", pasafloat);
					fprintf(FOutFlowCO, "\n%g", pasafloat);
					fprintf(FOutFlowFuel, "\n%g", pasafloat);
					fprintf(FOutFlowN2, "\n%g", pasafloat);
					for (unsigned int j = 0; j < STCylinder.size(); ++j) {
						fprintf(FOutFlowO2, " %g", 0.0);
						fprintf(FOutFlowCO2, " %g", 0.0);
						fprintf(FOutFlowH2O, " %g", 0.0);
						fprintf(FOutFlowHC, " %g", 0.0);
						fprintf(FOutFlowSoot, " %g", 0.0);
						fprintf(FOutFlowNOx, " %g", 0.0);
						fprintf(FOutFlowCO, " %g", 0.0);
						fprintf(FOutFlowFuel, " %g", 0.0);
						fprintf(FOutFlowN2, " %g", 0.0);
					}
					for (unsigned int j = 0; j < STPlenum.size(); ++j) {
						fprintf(FOutFlowO2, " %g", 0.0);
						fprintf(FOutFlowCO2, " %g", 0.0);
						fprintf(FOutFlowH2O, " %g", 0.0);
						fprintf(FOutFlowHC, " %g", 0.0);
						fprintf(FOutFlowSoot, " %g", 0.0);
						fprintf(FOutFlowNOx, " %g", 0.0);
						fprintf(FOutFlowCO, " %g", 0.0);
						fprintf(FOutFlowFuel, " %g", 0.0);
						fprintf(FOutFlowN2, " %g", 0.0);
					}
					for (unsigned int j = 0; j < STPipe.size(); ++j) {
						for (int k = 0; k < STPipe[j]->getNin(); ++k) {
							if (STPipe[j]->getFormulacionLeyes() == nmConArea) {
								massflow = STPipe[j]->GetU0(1, k);
							} else {
								massflow = STPipe[j]->GetPresion(k) * 1e5
										/ pow2(STPipe[j]->GetAsonido(k) * ARef)
										* STPipe[j]->GetGamma(k)
										* STPipe[j]->GetVelocidad(k) * ARef
										* STPipe[j]->GetDiametro(k) * Pi / 4.;
							}
							fprintf(FOutFlowO2, " %g",
									STPipe[j]->GetFraccionMasica(k, 0)
											* massflow);
							fprintf(FOutFlowCO2, " %g",
									STPipe[j]->GetFraccionMasica(k, 1)
											* massflow);
							fprintf(FOutFlowH2O, " %g",
									STPipe[j]->GetFraccionMasica(k, 2)
											* massflow);
							fprintf(FOutFlowHC, " %g",
									STPipe[j]->GetFraccionMasica(k, 3)
											* massflow);
							fprintf(FOutFlowSoot, " %g",
									STPipe[j]->GetFraccionMasica(k, 4)
											* massflow);
							fprintf(FOutFlowNOx, " %g",
									STPipe[j]->GetFraccionMasica(k, 5)
											* massflow);
							fprintf(FOutFlowCO, " %g",
									STPipe[j]->GetFraccionMasica(k, 6)
											* massflow);
							fprintf(FOutFlowFuel, " %g",
									STPipe[j]->GetFraccionMasica(k, 7)
											* massflow);
							fprintf(FOutFlowN2, " %g",
									STPipe[j]->GetFraccionMasica(k, 8)
											* massflow);
						}
					}
				}
				break;
			}
		}

	}
}

void TOutputResults::HeaderInstantaneousResults(TCalculoExtern *EXTERN,
		bool ThereIsDLL, bool EngineBlock, stEspecies *SpeciesName) {

	if (WriteInsHeader) {

		AnsiString Label;

		FInsOutput << "Time";

		if (EngineBlock) {
			Label = PutLabel(702);
			FInsOutput << "\t" << Label.c_str();
		}

		// std::cout << (*FInsOutput).str() << std::endl;

		// fflush(fg);
		// fclose(fg);

		// CABECERA RESULTADOS INSTANTANEOS CILINDROS.
		for (unsigned int i = 0; i < InsCylinder.size(); ++i) {
			InsCylinder[i]->HeaderInstantaneousResultsCilindro(FInsOutput,
					SpeciesName);
		}

		// CABECERA RESULTADOS INSTANTANEOS DEPOSITOS.
		for (unsigned int i = 0; i < InsPlenum.size(); ++i) {
			InsPlenum[i]->HeaderInstantaneousResultsDep(FInsOutput,
					SpeciesName);
		}

		// CABECERA RESULTADOS INSTANTANEOS TUBOS.

		for (unsigned int i = 0; i < InsPipe.size(); i++) {
			InsPipe[i]->HeaderInstantaneousResults(FInsOutput, SpeciesName);
		}

		// CABECERA RESULTADOS INSTANTANEOS DPF.
#ifdef ParticulateFilter
		for ( unsigned int i=0; i<InsDPF.size(); i++ ) {
			InsDPF[i]->CabeceraResultadosInstantaneos ( FInsOutput, SpeciesName );
		}
#endif

		// CABECERA RESULTADOS INSTANTANEOS VENTURIS.

		for (unsigned int i = 0; i < InsVenturi.size(); i++) {
			InsVenturi[i]->CabeceraResultadosInstantVenturi(FInsOutput);
		}

		// CABECERA RESULTADOS INSTANTANEOS EJES.

		for (unsigned int i = 0; i < InsTurbo.size(); ++i) {
			InsTurbo[i]->HeaderInstantaneousResultsEje(FInsOutput);
		}

		// CABECERA RESULTADOS INSTANTANEOS TURBINAS.
		for (unsigned int i = 0; i < InsTurbine.size(); i++) {
			InsTurbine[i]->CabeceraResultadosInstantTurb(FInsOutput);
		}

		// CABECERA RESULTADOS INSTANTANEOS COMPRESOR.
		for (unsigned int i = 0; i < InsCompressor.size(); ++i) {
			InsCompressor[i]->CabeceraGraficasInstantaneas(FInsOutput);
		}

		// CABECERA RESULTADOS INSTANTANEOS VALVULAS.

		for (unsigned int i = 0; i < InsValve.size(); i++) {
			InsValve[i]->CabeceraGraficaINS(FInsOutput, InsValveNode[i]);
		}

		// CABECERA RESULTADOS INSTANTANEOS WASTE-GATES.
		for (unsigned int i = 0; i < InsWasteGate.size(); i++) {
			InsWasteGate[i]->CabeceraGraficaINS(FInsOutput, i);
		}

		// CABECERA RESULTADOS INSTANTANEOS LAMINAS.
		for (unsigned int i = 0; i < InsReedValve.size(); i++) {
			InsReedValve[i]->CabeceraGraficaINS(FInsOutput, i);
		}

		// CABECERA RESULTADOS INSTANTANEOS COMPRESOR VOLUMETRICO (TORNILLO).
		for (unsigned int i = 0; i < InsRoot.size(); i++) {
			InsRoot[i]->CabeceraResultadosInstantCV(FInsOutput);
		}

		// CABECERA RESULTADOS INSTANTANEOS UNION ENTRE DEPOSITOS.
		for (unsigned int i = 0; i < InsConnection.size(); i++) {
			InsConnection[i]->CabeceraResultadosInstantUED(FInsOutput);
		}

		// CABECERA RESULTADOS INSTANTANEOS SENSOR.
		for (unsigned int i = 0; i < InsSensor.size(); i++) {
			InsSensor[i]->CabeceraResultadosInsSensor(FInsOutput);
		}

		// CABECERA RESULTADOS INSTANTANEOS CONTROLADOR.
		for (unsigned int i = 0; i < InsController.size(); i++) {
			InsController[i]->CabeceraResultadosInsControlador(FInsOutput);
		}

		// CABECERA RESULTADOS INSTANTANEOS CALCULOS EXTERNOS.
		if (ThereIsDLL) {
			EXTERN->ImprimeCabeceraInstantaneas(FInsOutput);
		}
	}

}

void TOutputResults::ConvertCharacter(int confile, char confile1[],
		int Characters) {
	try {
		int i, j = 0;

		i = 0;
		do {
			confile1[Characters - i] = (char) (confile % 10) + '0';
			++i;
		} while ((confile /= 10) > 0);
		confile1[Characters + 1] = '\0';

		if (i < Characters + 1) {
			for (j = 0; j <= (Characters - i); j++) {
				confile1[j] = '0';
			}
		}
	} catch (Exception & N) {
		std::cout << "ERROR: ConvertCharacter" << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

void TOutputResults::PlotThisCycle(TBloqueMotor* Engine, int TotalCycles) {

	switch (FTypeOfInsResults) {
	case nmLastCyle:
		if ((Engine->getCiclo() + 1) == TotalCycles - 1) {
			FPlotThisCycle = true;
		} else {
			FPlotThisCycle = false;
		}
		break;
	case nmEveryNCycles:
		if ((Engine->getCiclo() + 1) % FCyclePeriod == 0) {
			FPlotThisCycle = true;
		} else {
			FPlotThisCycle = false;
		}
	default:
		FPlotThisCycle = true;
	}
}

void TOutputResults::PlotControl(double Theta0, double Theta,
		double CycleDuration) {

	int n1, n0;

	if (FFirstTime) {
		FFirstTime = false;
		FControlAngle1 = 0;
		while (FControlAngle1 <= Theta) {
			FControlAngle1 += FInsPeriod;
		}
		if (FControlAngle1 > Theta + 10) {
			FControlAngle1 = Theta + 10;
		}
		FControlAngle0 = FControlAngle1 - FInsPeriod;
	}
	while (FControlAngle1 < Theta0) {
		FControlAngle1 = FControlAngle1 + FInsPeriod;
		FControlAngle0 = FControlAngle1 - FInsPeriod;
	}
	if (FControlAngle1 > Theta0 && FControlAngle1 <= Theta) {
		FControlAngle0 = FControlAngle1;
		FControlAngle1 += FInsPeriod;
		if (fmod(FControlAngle1, CycleDuration) < FInsPeriod) {
			FControlAngle1 -= fmod(FControlAngle1, CycleDuration);
		}
		if (FPlotThisCycle)
			FPlotIns = true;
		else
			FPlotIns = false;
	} else {
		FPlotIns = false;
	}
}

void TOutputResults::OutputInstantaneousResults(TCalculoExtern *EXTERN,
		bool ThereIsDLL, bool EngineBlock, double Theta, TBloqueMotor* Engine,
		double Time) {

	if (FPlotIns) {
		float pasafloat, m;
		double RegimenFicticio;

		FInsOutput << std::endl;
		FInsOutput << Time;

		if (EngineBlock) {
			m = int(Theta / Engine->getAngTotalCiclo());
			pasafloat = (float) (Theta - (m * Engine->getAngTotalCiclo()));
			FInsOutput << "\t" << pasafloat;
		}

		// IMPRIME RESULTADOS INSTANTANEOS CILINDROS.
		for (unsigned int i = 0; i < InsCylinder.size(); ++i) {
			InsCylinder[i]->CalculaResultadosInstantaneosCilindro();
			InsCylinder[i]->ImprimeResultadosInstantaneosCilindro(FInsOutput);
		}

		// IMPRIME RESULTADOS INSTANTANEOS DEPOSITOS.
		for (unsigned int i = 0; i < InsPlenum.size(); ++i) {
			InsPlenum[i]->ResultadosInstantaneosDep();
			InsPlenum[i]->ImprimeResultadosInstantaneosDep(FInsOutput);
		}

		// IMPRIME RESULTADOS INSTANTANEOS TUBOS.

		for (unsigned int i = 0; i < InsPipe.size(); i++) {
			InsPipe[i]->CalculaResultadosInstantaneos();
			InsPipe[i]->ImprimeResultadosInstantaneos(FInsOutput);
		}

		// IMPRIME RESULTADOS INSTANTANEOS VENTURIS.

		for (unsigned int i = 0; i < InsVenturi.size(); i++) {
			InsVenturi[i]->CalculaResultadosVenturi();
			InsVenturi[i]->ImprimeResultadosInstantVenturi(FInsOutput);
		}

		// IMPRIME RESULTADOS INSTANTANEOS EJES.

		for (unsigned int i = 0; i < InsTurbo.size(); ++i) {
			InsTurbo[i]->ResultadosInstantEje();
			InsTurbo[i]->ImprimeResultadosInstantaneosEje(FInsOutput);
		}

		// IMPRIME RESULTADOS INSTANTANEOS TURBINAS.
		for (unsigned int i = 0; i < InsTurbine.size(); i++) {
			InsTurbine[i]->ResultadosInstantTurb();
			InsTurbine[i]->ImprimeResultadosInstantTurb(FInsOutput);
		}

		// IMPRIME RESULTADOS INSTANTANEOS COMPRESOR.
		for (unsigned int i = 0; i < InsCompressor.size(); ++i) {
			InsCompressor[i]->CalculaInstantaneos();
			InsCompressor[i]->ImprimeGraficasInstantaneas(FInsOutput);
		}

		// RESULTADOS INSTANTANEOS EN DPF.
#ifdef ParticulateFilter
		for ( int i=0; i<InsDPF.size(); i++ ) {
			InsDPF[i]->CalculaResultadosInstantaneos();
			InsDPF[i]->ImprimeResultadosInstantaneos ( FInsOutput );
		}
#endif

		// IMPRIME RESULTADOS INSTANTANEOS VALVULAS.

		for (unsigned int i = 0; i < InsValve.size(); i++) {
			InsValve[i]->ImprimeGraficaINS(FInsOutput);
		}

		// IMPRIME RESULTADOS INSTANTANEOS WASTE-GATES.
		for (unsigned int i = 0; i < InsWasteGate.size(); i++) {
			InsWasteGate[i]->ImprimeGraficaINS(FInsOutput);
		}

		// IMPRIME RESULTADOS INSTANTANEOS LAMINAS.
		for (unsigned int i = 0; i < InsReedValve.size(); i++) {
			InsReedValve[i]->ImprimeGraficaINS(FInsOutput);
		}

		// IMPRIME RESULTADOS INSTANTANEOS COMPRESOR VOLUMETRICO (TORNILLO).
		for (unsigned int i = 0; i < InsRoot.size(); i++) {
			InsRoot[i]->ResultadosInstantCV();
			InsRoot[i]->ImprimeResultadosInstantCV(FInsOutput);
		}

		// IMPRIME RESULTADOS INSTANTANEOS UNION ENTRE DEPOSITOS.
		for (unsigned int i = 0; i < InsConnection.size(); i++) {
			InsConnection[i]->ResultadosInstantUED();
			InsConnection[i]->ImprimeResultadosInstantUED(FInsOutput);
		}

		// IMPRIME RESULTADOS INSTANTANEOS SENSOR.
		for (unsigned int i = 0; i < InsSensor.size(); i++) {
			InsSensor[i]->ResultadosInstantSensor();
			InsSensor[i]->ImprimeResultadosInsSensor(FInsOutput);
		}

		// IMPRIME RESULTADOS INSTANTANEOS CONTROLADOR.
		for (unsigned int i = 0; i < InsController.size(); i++) {
			InsController[i]->ResultadosInstantController();
			InsController[i]->ImprimeResultadosInsControlador(FInsOutput);
		}

		// IMPRIME RESULTADOS INSTANTANEOS CALCULOS EXTERNOS.
		if (ThereIsDLL) {
			EXTERN->ImprimeGraficosInstantaneas(FInsOutput);
		}
#ifdef WriteINS
		CopyInstananeousResultsToFile ( 1 );
#endif
	}

}

void TOutputResults::WriteInstantaneous(bool EngineBlock, double Angle,
		double AngStep, TBloqueMotor* Engine, int TotalCycles) {

	if (EngineBlock) {
		switch (FTypeOfInsResults) {
		case nmLastCyle:
			if ((Engine->getCiclo() + 1) == TotalCycles - 1) {
				FPlotThisCycle = true;
				if (Angle - AngStep < 0)
					WriteInsHeader = true;
				else
					WriteInsHeader = false;
			} else {
				FPlotThisCycle = false;
				WriteInsHeader = false;
			}

			break;
		case nmAllCyclesIndependent:
			FPlotThisCycle = true;
			if (Angle - AngStep < 0)
				WriteInsHeader = true;
			else
				WriteInsHeader = false;
			break;
		case nmAllCyclesConcatenated:
			FPlotThisCycle = true;
			if (InsHeaderCreated) {
				WriteInsHeader = false;
			} else {
				WriteInsHeader = true;
				InsHeaderCreated = true;
			}
			break;
		case nmEveryNCycles:

			if ((Engine->getCiclo() + 1) % FCyclePeriod == 0
					&& Engine->getCiclo() > 0) {
				FPlotThisCycle = true;
				if (Angle - AngStep < 0) {
					WriteInsHeader = true;
				} else {
					WriteInsHeader = false;
				}
			} else {
				FPlotThisCycle = false;
			}
		}
	} else {
		if (InsHeaderCreated) {
			WriteInsHeader = false;
		} else {
			WriteInsHeader = true;
			InsHeaderCreated = true;
		}
	}

}

void TOutputResults::WriteSpaceTime(bool EngineBlock, TBloqueMotor* Engine,
		int TotalCycles) {
	if (EngineBlock) {
		if ((Engine->getCiclo() + 1) == TotalCycles - 1) {
			FWriteSpaceTime = true;
		}
	} else {
		FWriteSpaceTime = true;
	}
}

// ---------------------------------------------------------------------------

#pragma package(smart_init)
