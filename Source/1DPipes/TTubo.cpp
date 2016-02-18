/* --------------------------------------------------------------------------------*\
|===========================|
 | \\   /\ /\   // O pen     | OpenWAM: The Open Source 1D Gas-Dynamic Code
 |  \\ |  X  | //  W ave     |
 |   \\ \/_\/ //   A ction   | CMT-Motores Termicos / Universidad Politecnica Valencia
 |    \\/   \//    M odel    |
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

/**
 * @file TTubo.cpp
 * @author Francisco Jose Arnau <farnau@mot.upv.es>
 *
 * @section LICENSE
 *
 * This file is part of OpenWAM.
 *
 * OpenWAM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenWAM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenWAM.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @section DESCRIPTION
 * This file defines a finite differences pipe.
 */

// ---------------------------------------------------------------------------
#pragma hdrstop

#include "TTubo.h"
#include "TBloqueMotor.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TTubo::TTubo(int SpeciesNumber, int j, double SimulationDuration, TBloqueMotor **Engine,
			 nmTipoCalculoEspecies SpeciesModel, nmCalculoGamma GammaCalculation, bool ThereIsEGR) {

	if(Engine != NULL) {
		FAnguloTotalCiclo = Engine[0]->getAngTotalCiclo();
	} else if(Engine == NULL) {
		FAnguloTotalCiclo = 720.;
		FRegimenFicticio = 720. / 6. / SimulationDuration;
	}

#ifdef ParticulateFilter
	FDPFEntradaTubo = NULL;
	FDPFSalidaTubo = NULL;
	FHayDPFNodoIzq = false;
	FHayDPFNodoDer = false;
#endif

	FHayEGR = ThereIsEGR;
	if(FHayEGR)
		FIntEGR = 0;
	else
		FIntEGR = 1;

	FSUMTime = 0.;
	FCicloTubo = 0;
	FNumeroTubo = j + 1;

	FNumeroEspecies = SpeciesNumber;
	FCalculoEspecies = SpeciesModel;
	FCalculoGamma = GammaCalculation;
	FNumEcuaciones = 3 + (FNumeroEspecies - 1 - FIntEGR);

	FComposicionInicial = NULL;
	FFraccionMasicaEspecie = NULL;
	FFraccionMasicaCC = NULL;
	FVelocidadCC = NULL;
	FDensidadCC = NULL;
	FAreaCC = NULL;
	FGamma = NULL;
	FRMezcla = NULL;
	FGamma1 = NULL;
	FGamma3 = NULL;
	FGamma4 = NULL;
	FGamma5 = NULL;
	FGamma6 = NULL;

	FDExtTramo = NULL;
	FLTramo = NULL;
	FDiametroTubo = NULL;
	FDiametroD12 = NULL;
	FDiametroS12 = NULL;
	FDerLin = NULL;
	FDerLin12 = NULL;
	FDerLinArea = NULL;
	FDerLinArea12 = NULL;
	FArea = NULL;
	FArea12 = NULL;
	FPresion0 = NULL;
	FAsonido0 = NULL;
	FVelocidad0 = NULL;
	FVelocidadMedia = NULL;
	FAsonidoMedia = NULL;
	FPresionMedia = NULL;
	FPresion1 = NULL;
	FAsonido1 = NULL;
	FVelocidad1 = NULL;
	FUt = NULL;
	FU0 = NULL;
	FU0Sum = NULL;
	FU0Medio = NULL;
	FU1 = NULL;
	FU12 = NULL;
	FW = NULL;
	FV1 = NULL;
	FV2 = NULL;
	FUfct0 = NULL;
	FUfct1 = NULL;
	FUfctd = NULL;
	FUfctad = NULL;
	Ffl = NULL;
	FdU = NULL;
	FDeltaFCTd = NULL;
	FflU = NULL;
	FaU = NULL;
	FVelPro = NULL;
	FCoefTurbulencia = NULL;
	FSUMTPTuboPro = NULL;
	FTPTubo = NULL;
	FTParedAnt = NULL;
	ResultadosMedios = NULL;
	ResultInstantaneos = NULL;
	FNumResMedios = 0;
	FNumResInstant = 0;
	FNumDistSensores = 0;
	FIntercooler = false;
	FConcentrico = false;
	FMod.Modelo = nmLaxWendroff;
	FMod.SubModelo = nmNinguno;
	FMod.OpcionSubModelo = nmNinguna;
	FMod.FormulacionLeyes = nmSinArea;
	/* FTVD=NULL;
	 FTVDdU=NULL;
	 FTVDpp=NULL;
	 FTVDpn=NULL;
	 FTVDrp=NULL;
	 FTVDrn=NULL;
	 FTVDphp=NULL;
	 FTVDphn=NULL;
	 FTVDGp=NULL;
	 FTVDGn=NULL; */
	FCourantLocal = NULL;
	Fhi = NULL;
	Fhe = NULL;
	Frho = NULL;
	FRe = NULL;
	FTVD.Bmas = NULL;
	FTVD.Bvector = NULL;
	FTVD.Bmen = NULL;
	FTVD.Qmatrix = NULL;
	FTVD.Pmatrix = NULL;
	FTVD.gflux = NULL;
	FTVD.Alpha = NULL;
	FTVD.Beta = NULL;
	FTVD.DeltaU = NULL;
	FTVD.DeltaB = NULL;
	FTVD.DeltaW = NULL;
	FTVD.hLandaD = NULL;
	FTVD.LandaD = NULL;
	FTVD.Phi = NULL;
	FTVD.R = NULL;
	FTVD.W = NULL;

	FCapa = NULL;

	FResistRadInt = NULL;
	FResistRadExt = NULL;
	FResistAxiAnt = NULL;
	FResistAxiPos = NULL;
	FCapInt = NULL;
	FCapMed = NULL;
	FCapExt = NULL;

	FVelocidadDim = NULL;
	FAsonidoDim = NULL;
	FTemperature = NULL;
	FFlowMass = NULL;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TTubo::~TTubo() {

	DestruyeVector(FVelocidadDim);
	DestruyeVector(FAsonidoDim);
	DestruyeVector(FTemperature);
	DestruyeVector(FFlowMass);

	DestruyeVector(FComposicionInicial);

	// if(FComposicionInicial) delete[] FComposicionInicial;
	if(FFraccionMasicaEspecie != NULL) {
		for(int i = 0; i < FNin; i++)
			delete[] FFraccionMasicaEspecie[i];
		delete[] FFraccionMasicaEspecie;
	}
	if(FFraccionMasicaCC != NULL) {
		for(int i = 0; i < 2; i++)
			delete[] FFraccionMasicaCC[i];
		delete[] FFraccionMasicaCC;
	}
	if(FVelocidadCC != NULL)
		delete[] FVelocidadCC;
	if(FDensidadCC != NULL)
		delete[] FDensidadCC;
	if(FAreaCC != NULL)
		delete[] FAreaCC;
	if(FGamma != NULL)
		delete[] FGamma;
	if(FRMezcla != NULL)
		delete[] FRMezcla;
	if(FGamma1 != NULL)
		delete[] FGamma1;
	if(FGamma3 != NULL)
		delete[] FGamma3;
	if(FGamma4 != NULL)
		delete[] FGamma4;
	if(FGamma5 != NULL)
		delete[] FGamma5;
	if(FGamma6 != NULL)
		delete[] FGamma6;

	if(FDExtTramo != NULL)
		delete[] FDExtTramo;
	if(FLTramo != NULL)
		delete[] FLTramo;
	if(FDiametroTubo != NULL)
		delete[] FDiametroTubo;
	if(FDiametroD12 != NULL)
		delete[] FDiametroD12;
	if(FDiametroS12 != NULL)
		delete[] FDiametroS12;
	if(FDerLin != NULL)
		delete[] FDerLin;
	if(FDerLin12 != NULL)
		delete[] FDerLin12;
	if(FDerLinArea != NULL)
		delete[] FDerLinArea;
	if(FDerLinArea12 != NULL)
		delete[] FDerLinArea12;
	if(FArea != NULL)
		delete[] FArea;
	if(FArea12 != NULL)
		delete[] FArea12;
	if(FPresion0 != NULL)
		delete[] FPresion0;
	if(FAsonido0 != NULL)
		delete[] FAsonido0;
	if(FVelocidad0 != NULL)
		delete[] FVelocidad0;
	if(FVelocidadMedia != NULL)
		delete[] FVelocidadMedia;
	if(FAsonidoMedia != NULL)
		delete[] FAsonidoMedia;
	if(FPresionMedia != NULL)
		delete[] FPresionMedia;
	if(FPresion1 != NULL)
		delete[] FPresion1;
	if(FAsonido1 != NULL)
		delete[] FAsonido1;
	if(FVelocidad1 != NULL)
		delete[] FVelocidad1;
	if(Fhi != NULL)
		delete[] Fhi;
	if(Fhe != NULL)
		delete[] Fhe;
	if(Frho != NULL)
		delete[] Frho;
	if(FRe != NULL)
		delete[] FRe;

	if(FUt != NULL) {
		for(int i = 0; i < 3; i++)
			delete[] FUt[i];
		delete[] FUt;
	}
	if(FU0 != NULL) {
		for(int i = 0; i < FNumEcuaciones; i++)
			delete[] FU0[i];
		delete[] FU0;
	}
	if(FU0Sum != NULL) {
		for(int i = 0; i < FNumEcuaciones; i++)
			delete[] FU0Sum[i];
		delete[] FU0Sum;
	}
	if(FU0Medio != NULL) {
		for(int i = 0; i < FNumEcuaciones; i++)
			delete[] FU0Medio[i];
		delete[] FU0Medio;
	}
	if(FU1 != NULL) {
		for(int i = 0; i < FNumEcuaciones; i++)
			delete[] FU1[i];
		delete[] FU1;
	}
	if(FU12 != NULL) {
		for(int i = 0; i < FNumEcuaciones; i++)
			delete[] FU12[i];
		delete[] FU12;
	}
	if(FW != NULL) {
		for(int i = 0; i < FNumEcuaciones; i++)
			delete[] FW[i];
		delete[] FW;
	}
	if(FV1 != NULL) {
		for(int i = 0; i < FNumEcuaciones; i++)
			delete[] FV1[i];
		delete[] FV1;
	}
	if(FV2 != NULL) {
		for(int i = 0; i < FNumEcuaciones; i++)
			delete[] FV2[i];
		delete[] FV2;
	}
	if(FUfct0 != NULL) {
		for(int i = 0; i < FNumEcuaciones; i++)
			delete[] FUfct0[i];
		delete[] FUfct0;
	}
	if(FUfct1 != NULL) {
		for(int i = 0; i < FNumEcuaciones; i++)
			delete[] FUfct1[i];
		delete[] FUfct1;
	}
	if(FUfctd != NULL) {
		for(int i = 0; i < FNumEcuaciones; i++)
			delete[] FUfctd[i];
		delete[] FUfctd;
	}
	if(FUfctad != NULL) {
		for(int i = 0; i < FNumEcuaciones; i++)
			delete[] FUfctad[i];
		delete[] FUfctad;
	}
	if(Ffl != NULL) {
		for(int i = 0; i < FNumEcuaciones; i++)
			delete[] Ffl[i];
		delete[] Ffl;
	}
	if(FdU != NULL) {
		for(int i = 0; i < FNumEcuaciones; i++)
			delete[] FdU[i];
		delete[] FdU;
	}
	if(FDeltaFCTd != NULL) {
		for(int i = 0; i < FNumEcuaciones; i++)
			delete[] FDeltaFCTd[i];
		delete[] FDeltaFCTd;
	}
	if(FflU != NULL) {
		for(int i = 0; i < FNumEcuaciones; i++)
			delete[] FflU[i];
		delete[] FflU;
	}
	if(FaU != NULL) {
		for(int i = 0; i < FNumEcuaciones; i++)
			delete[] FaU[i];
		delete[] FaU;
	}
	if(FVelPro != NULL)
		delete[] FVelPro;
	if(FCoefTurbulencia != NULL)
		delete[] FCoefTurbulencia;
	if(FSUMTPTuboPro != NULL) {
		for(int i = 0; i < 2; i++) {
			for(int j = 0; j < 3; j++) {
				delete[] FSUMTPTuboPro[i][j];
			}
			delete[] FSUMTPTuboPro[i];
		}
		delete[] FSUMTPTuboPro;
	}
	if(FTPTubo != NULL) {
		for(int i = 0; i < 3; i++)
			delete[] FTPTubo[i];
		delete[] FTPTubo;
	}
	if(FTParedAnt != NULL) {
		for(int i = 0; i < 3; i++)
			delete[] FTParedAnt[i];
		delete[] FTParedAnt;
	}

	for(int i = 0; i < FNumResMedios; i++) {
		if(ResultadosMedios[i].FraccionSUM != NULL)
			delete[] ResultadosMedios[i].FraccionSUM;
		if(ResultadosMedios[i].FraccionMED != NULL)
			delete[] ResultadosMedios[i].FraccionMED;
	}
	if(ResultadosMedios != NULL)
		delete[] ResultadosMedios;
	for(int i = 0; i < FNumResInstant; i++) {
		if(ResultInstantaneos[i].FraccionINS != NULL)
			delete[] ResultInstantaneos[i].FraccionINS;
	}
	if(ResultInstantaneos != NULL)
		delete[] ResultInstantaneos;

	if(FCourantLocal != NULL)
		delete[] FCourantLocal;

	for(int i = 0; i < FNumEcuaciones; i++) {
		for(int k = 0; k < FNumEcuaciones; k++) {
			if(FTVD.Pmatrix != NULL)
				delete[] FTVD.Pmatrix[i][k];
			if(FTVD.Qmatrix != NULL)
				delete[] FTVD.Qmatrix[i][k];
		}
		if(FTVD.Pmatrix != NULL)
			delete[] FTVD.Pmatrix[i];
		if(FTVD.Qmatrix != NULL)
			delete[] FTVD.Qmatrix[i];
	}
	for(int i = 0; i < FNumEcuaciones; i++) {
		if(FTVD.Bmas != NULL)
			delete[] FTVD.Bmas[i];
		if(FTVD.Bmen != NULL)
			delete[] FTVD.Bmen[i];
		if(FTVD.Bvector != NULL)
			delete[] FTVD.Bvector[i];
		if(FTVD.gflux != NULL)
			delete[] FTVD.gflux[i];
		if(FTVD.Alpha != NULL)
			delete[] FTVD.Alpha[i];
		if(FTVD.Beta != NULL)
			delete[] FTVD.Beta[i];
		if(FTVD.DeltaU != NULL)
			delete[] FTVD.DeltaU[i];
		if(FTVD.DeltaB != NULL)
			delete[] FTVD.DeltaB[i];
		if(FTVD.DeltaW != NULL)
			delete[] FTVD.DeltaW[i];
		if(FTVD.hLandaD != NULL)
			delete[] FTVD.hLandaD[i];
		if(FTVD.LandaD != NULL)
			delete[] FTVD.LandaD[i];
		if(FTVD.Phi != NULL)
			delete[] FTVD.Phi[i];
		if(FTVD.W != NULL)
			delete[] FTVD.W[i];
		if(FTVD.R != NULL)
			delete[] FTVD.R[i];
	}

	if(FTVD.Pmatrix != NULL)
		delete[] FTVD.Pmatrix;
	if(FTVD.Qmatrix != NULL)
		delete[] FTVD.Qmatrix;
	if(FTVD.Bmas != NULL)
		delete[] FTVD.Bmas;
	if(FTVD.Bmen != NULL)
		delete[] FTVD.Bmen;
	if(FTVD.Bvector != NULL)
		delete[] FTVD.Bvector;
	if(FTVD.gflux != NULL)
		delete[] FTVD.gflux;
	if(FTVD.Alpha != NULL)
		delete[] FTVD.Alpha;
	if(FTVD.Beta != NULL)
		delete[] FTVD.Beta;
	if(FTVD.DeltaU != NULL)
		delete[] FTVD.DeltaU;
	if(FTVD.DeltaB != NULL)
		delete[] FTVD.DeltaB;
	if(FTVD.DeltaW != NULL)
		delete[] FTVD.DeltaW;
	if(FTVD.hLandaD != NULL)
		delete[] FTVD.hLandaD;
	if(FTVD.LandaD != NULL)
		delete[] FTVD.LandaD;
	if(FTVD.Phi != NULL)
		delete[] FTVD.Phi;
	if(FTVD.W != NULL)
		delete[] FTVD.W;
	if(FTVD.R != NULL)
		delete[] FTVD.R;

	if(FCapa != NULL)
		delete[] FCapa;

	if(FResistRadInt != NULL)
		delete[] FResistRadInt;
	if(FResistRadExt != NULL)
		delete[] FResistRadExt;
	if(FResistAxiAnt != NULL)
		delete[] FResistAxiAnt;
	if(FResistAxiPos != NULL)
		delete[] FResistAxiPos;
	if(FCapInt != NULL)
		delete[] FCapInt;
	if(FCapMed != NULL)
		delete[] FCapMed;
	if(FCapExt != NULL)
		delete[] FCapExt;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::LeeDatosGeneralesTubo(const char *FileWAM, fpos_t &filepos) {
#ifdef usetry
	try {
#endif
		int TipTC = 0, Concentrico;
		int metodo[4];
		double fracciontotal = 0.;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d %d %d %d", &FNodoIzq, &FNodoDer, &FNTramos, &FNumeroConductos);
		fscanf(fich, "%lf ", &FFriccion);
		fscanf(fich, "%lf %lf %lf %lf ", &FTIniParedTub, &FTini, &FPini, &FVelMedia);
		fscanf(fich, "%d %lf %lf ", &TipTC, &FCoefAjusTC, &FCoefAjusFric);

		switch(TipTC) {
		case 1:
			FTipoTransCal = nmTuboAdmision;
			break;
		case 2:
			FTipoTransCal = nmTuboEscape;
			break;
		case 3:
			FTipoTransCal = nmPipaEscape;
			break;
		case 4:
			FTipoTransCal = nmPipaAdmision;
			break;
		}

		FComposicionInicial = new double[FNumeroEspecies - FIntEGR];
		for(int i = 0; i < FNumeroEspecies - 1; i++) {
			fscanf(fich, "%lf ", &FComposicionInicial[i]);
			fracciontotal += FComposicionInicial[i];
		}
		if(FHayEGR) {
			if(FCalculoEspecies == nmCalculoCompleto) {
				if(FComposicionInicial[0] > 0.2)
					FComposicionInicial[FNumeroEspecies - 1] = 0.;
				else
					FComposicionInicial[FNumeroEspecies - 1] = 1.;
			} else {
				if(FComposicionInicial[0] > 0.5)
					FComposicionInicial[FNumeroEspecies - 1] = 1.;
				else
					FComposicionInicial[FNumeroEspecies - 1] = 0.;
			}
		}

		if(fracciontotal > 1 + 1.e-10 && fracciontotal < 1 - 1e-10) {
			std::cout << "ERROR: Total mass fraction must be equal to 1. Check input data for pipe  " << FNumeroTubo << std::endl;
			throw Exception(" ");
		}

		fscanf(fich, "%lf %d ", &FMallado, &FTctpt);

		switch(FTctpt) {
		case 0:
			FTipoCalcTempPared = nmVariableConInerciaTermica;
			break;
		case 1:
			FTipoCalcTempPared = nmVariableSinInerciaTermica;
			break;
		case 2:
			FTipoCalcTempPared = nmTempConstante;
			break;
		}

		fscanf(fich, "%d ", &metodo[0]);

		if(metodo[0] == 0) {
			FMod.Modelo = nmLaxWendroff;
			fscanf(fich, "%d ", &metodo[1]);
			if(metodo[1] == 0) {
				// Lax&Wendroff
				metodo[3] = 1;
				switch(metodo[3]) {
				case 0:
					FMod.FormulacionLeyes = nmSinArea;
					break;
				case 1:
					FMod.FormulacionLeyes = nmConArea;
					break;
				}
			}

			if(metodo[1] == 1) {
				// Lax&Wendroff + FCT
				FMod.FormulacionLeyes = nmConArea;
				FMod.SubModelo = nmFCT;
				fscanf(fich, "%d ", &metodo[2]);
				switch(metodo[2]) {
				case 0:
					FMod.OpcionSubModelo = nmDDNAD;
					FMod.Difusion = nmDamping;
					FMod.Antidifusion = nmNaive;
					break;
				case 1:
					FMod.OpcionSubModelo = nmDDPAD;
					FMod.Difusion = nmDamping;
					FMod.Antidifusion = nmPhoenical;
					break;
				case 2:
					FMod.OpcionSubModelo = nmDDEAD;
					FMod.Difusion = nmDamping;
					FMod.Antidifusion = nmExplicit;
					break;
				case 3:
					FMod.OpcionSubModelo = nmDSNAD;
					FMod.Difusion = nmSmoothing;
					FMod.Antidifusion = nmNaive;
					break;
				case 4:
					FMod.OpcionSubModelo = nmDSPAD;
					FMod.Difusion = nmSmoothing;
					FMod.Antidifusion = nmPhoenical;
					break;
				case 5:
					FMod.OpcionSubModelo = nmDSEAD;
					FMod.Difusion = nmSmoothing;
					FMod.Antidifusion = nmPhoenical;
					break;
				}
			}
		} else if(metodo[0] == 2) {
			FMod.Modelo = nmTVD;
			FMod.FormulacionLeyes = nmConArea;
		}

		fscanf(fich, "%lf ", &FCourant);

		fgetpos(fich, &filepos);
		fclose(fich);
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::LeeDatosGeneralesTubo en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::LeeDatosGeometricosTubo(const char *FileWAM, fpos_t &filepos, double ene, int tipomallado,
									TBloqueMotor **Engine) {
	double EspesorPrin = 0.;
	int EsPrincipal = 0, refrigerante = 0, EsFluida = 0;
	int datoWAMer = 0;

	FDExtTramo = new double[FNTramos + 1];
	FLTramo = new double[FNTramos + 1];
#ifdef usetry
	try {
#endif

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		switch(tipomallado) {
		case 1:
			FTipoMallado = nmDistancia;
			break;
		case 2:
			FTipoMallado = nmAngular;
			break;
		}

		if(FTipoMallado == nmAngular && ene < 0.) {
			std::cout << "ERROR: El mallado no puede ser angular al no existir motor.Pipe: " << FNumeroTubo << std::endl;
			throw Exception("");
		}

		fscanf(fich, "%lf ", &FDExtTramo[0]);
		for(int i = 1; i <= FNTramos; i++) {
			fscanf(fich, "%lf %lf ", &FLTramo[i], &FDExtTramo[i]);
		}
		CalculoPuntosMalla(ene);

		if(FTipoCalcTempPared != nmTempConstante) {
			FResistRadInt = new double[FNin];
			FResistRadExt = new double[FNin];
			FResistAxiAnt = new double[FNin];
			FResistAxiPos = new double[FNin];
			FCapInt = new double[FNin];
			FCapMed = new double[FNin];
			FCapExt = new double[FNin];

			if(Engine == NULL) {
				fscanf(fich, "%lf %d ", &FDuracionCiclo, &FNumCiclosSinInerciaTermica);
			}

			if(FTipoTransCal != nmPipaEscape && FTipoTransCal != nmPipaAdmision) {
				fscanf(fich, "%lf %lf %d ", &FCoefExt, &FEmisividad, &refrigerante);
				switch(refrigerante) {
				case 0:
					FTipRefrig = nmAire;
					break;
				case 1:
					FTipRefrig = nmAgua;
					break;
				}
				if(FTipRefrig == nmAgua)
					fscanf(fich, "%lf ", &FTRefrigerante); /* Esta en degC */
			} else {
				FCoefExt = 1.;
				FEmisividad = 0.5;
				fscanf(fich, "%d", &datoWAMer);
			}

			fscanf(fich, "%d ", &FNumCapas);
			FCapa = new stCapa[FNumCapas];
			EspesorPrin = 0.;
			for(int i = 0; i < FNumCapas; i++) {
				fscanf(fich, "%d %d ", &EsPrincipal, &EsFluida);
				EsPrincipal == 0 ? FCapa[i].EsPrincipal = false : FCapa[i].EsPrincipal = true;
				EsFluida == 0 ? FCapa[i].EsFluida = false : FCapa[i].EsFluida == true;
				if(FCapa[i].EsFluida) {
					fscanf(fich, "%lf %lf ", &FCapa[i].EmisividadInterior, &FCapa[i].EmisividadExterior);
				} else {
					fscanf(fich, "%lf %lf %lf %lf ", &FCapa[i].Density, &FCapa[i].CalorEspecifico, &FCapa[i].Conductividad,
						   &FCapa[i].Espesor);
					fscanf(fich, "%lf %lf ", &FCapa[i].EmisividadInterior, &FCapa[i].EmisividadExterior);
				}
				if(FCapa[i].EsPrincipal) {
					FEspesorIntPrin = EspesorPrin;
					FEspesorPrin = FCapa[i].Espesor;
					FDensidadPrin = FCapa[i].Density;
					FCalEspPrin = FCapa[i].CalorEspecifico;
					FConductPrin = FCapa[i].Conductividad;
					EspesorPrin = 0.;
				} else {
					EspesorPrin += FCapa[i].Espesor;
				}
			}
			// }
			FEspesorExtPrin = EspesorPrin;
		} else if(FTipoTransCal == nmPipaEscape || FTipoTransCal == nmPipaAdmision) {
			fscanf(fich, "%d", &datoWAMer);
		}

		fgetpos(fich, &filepos);
		fclose(fich);
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::LeeDatosGeometricoTubo en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculoPuntosMalla(double ene) {
#ifdef usetry
	try {
#endif
		double *FLTotalTramo;
		double xx = 0.;

		FLTotalTramo = new double[FNTramos + 1];

		FLTramo[0] = 0.;
		FLTotalTramo[0] = FLTramo[0];

		for(int i = 1; i <= FNTramos; i++)
			FLTotalTramo[i] = FLTotalTramo[i - 1] + FLTramo[i];
		FLongitudTotal = FLTotalTramo[FNTramos];

		if(FTipoMallado == nmDistancia)
			xx = FLongitudTotal / FMallado + 0.5;
		if(FTipoMallado == nmAngular)
			xx = FLongitudTotal / 650. / FMallado * 6. * ene + 0.5;
		if(FTipoMallado != nmDistancia && FTipoMallado != nmAngular) {
			std::cout << "WARNING: No se ha definido una forma correcta de mallado en el tubo: " << FNumeroTubo << std::endl;
		}

		FNin = (int) xx + 1;
		if(FNin < 3)
			FNin = 3;

		FXref = FLongitudTotal / (double)(FNin - 1);

		int ii = 0;
		FDiametroTubo = new double[FNin];
		FArea = new double[FNin];
		for(int i = 1; i < FNin - 1; i++) {
			do {
				ii = ii + 1;
			} while(FLTotalTramo[ii] < i * FXref);
			ii = ii - 1;
			double r1 = i * FXref - FLTotalTramo[ii];
			FDiametroTubo[i] = Interpola(FDExtTramo[ii], FDExtTramo[ii + 1], FLTramo[ii + 1], r1);
			FArea[i] = __geom::Circle_area(FDiametroTubo[i]);
		}
		FDiametroTubo[0] = FDExtTramo[0];
		FArea[0] = __geom::Circle_area(FDiametroTubo[0]);
		FDiametroTubo[FNin - 1] = FDExtTramo[FNTramos];
		FArea[FNin - 1] = __geom::Circle_area(FDiametroTubo[FNin - 1]);

		FDiametroD12 = new double[FNin];
		FDiametroS12 = new double[FNin];
		FArea12 = new double[FNin];
		for(int i = 0; i < FNin - 1; i++) {
			FDiametroD12[i] = (FDiametroTubo[i + 1] + FDiametroTubo[i]) / 2.;
			FDiametroS12[i] = sqrt((pow2(FDiametroTubo[i + 1]) + pow2(FDiametroTubo[i])) / 2.);
			FArea12[i] = (FArea[i + 1] + FArea[i]) / 2.;
		}

		delete[] FLTotalTramo;
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculoPuntosMalla en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::ComunicacionTubo_CC(TCondicionContorno **BC) {
#ifdef usetry
	try {
#endif

		for(int i = 0; i < BC[FNodoIzq - 1]->getNumeroTubosCC(); i++) {
			if(FNumeroTubo == BC[FNodoIzq - 1]->GetTuboExtremo(i).Pipe->getNumeroTubo()) {
				FTuboCCNodoIzq = i;
			}
		}

		for(int i = 0; i < BC[FNodoDer - 1]->getNumeroTubosCC(); i++) {
			if(FNumeroTubo == BC[FNodoDer - 1]->GetTuboExtremo(i).Pipe->getNumeroTubo()) {
				FTuboCCNodoDer = i;
			}
		}
#ifdef usetry
	}

	catch(exception & N) {
		std::cout << "ERROR: TTubo::ComunicacionTubo_CC en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#ifdef ParticulateFilter

void TTubo::ComunicacionDPF(TCondicionContorno **CC, TDeposito **Deposito) {
	try {
		int numDeposito = 0;
		bool PrimeraVez = false;

		if(FTipoCalcTempPared != nmTempConstante) {
			FTipoCanal = new int[2];
			if(CC[FNodoIzq - 1]->getTipoCC() == nmPipeToPlenumConnection) {
				numDeposito = dynamic_cast<TCCDeposito*>(CC[FNodoIzq - 1])
							  ->getNumeroDeposito();
				for(int k = 0; k < Deposito[numDeposito - 1]->getNUniones();
					k++) {
					if(Deposito[numDeposito - 1]->GetCCDeposito(k)->getUnionDPF
					   () && !PrimeraVez) {
						FHayDPFNodoIzq = true;
						FTipoCanal[0] = Deposito[numDeposito - 1]->GetCCDeposito
										(k)->GetTuboExtremo(0).TipoCanal;
						FDPFEntradaTubo = Deposito[numDeposito - 1]
										  ->GetCCDeposito(k)->GetTuboExtremo(0).DPF;
						if(Deposito[numDeposito - 1]->GetCCDeposito(k)
						   ->GetTuboExtremo(0).TipoExtremo == nmLeft) {
							FNodoDPFEntrada = 0;
						} else {
							FNodoDPFEntrada = FDPFEntradaTubo->GetCanal(0, 0)
											  ->getNin() - 1;
						}
						PrimeraVez = true;
					}
				}
			}
			PrimeraVez = false;
			if(CC[FNodoDer - 1]->getTipoCC() == nmPipeToPlenumConnection) {
				numDeposito = dynamic_cast<TCCDeposito*>(CC[FNodoDer - 1])
							  ->getNumeroDeposito();
				for(int k = 0; k < Deposito[numDeposito - 1]->getNUniones();
					k++) {
					if(Deposito[numDeposito - 1]->GetCCDeposito(k)->getUnionDPF
					   () && !PrimeraVez) {
						FHayDPFNodoDer = true;
						FTipoCanal[1] = Deposito[numDeposito - 1]->GetCCDeposito
										(k)->GetTuboExtremo(0).TipoCanal;
						FDPFSalidaTubo = Deposito[numDeposito - 1]
										 ->GetCCDeposito(k)->GetTuboExtremo(0).DPF;
						if(Deposito[numDeposito - 1]->GetCCDeposito(k)
						   ->GetTuboExtremo(0).TipoExtremo == nmLeft) {
							FNodoDPFSalida = 0;
						} else {
							FNodoDPFSalida = FDPFSalidaTubo->GetCanal(0, 0)
											 ->getNin() - 1;
						}
						PrimeraVez = true;
					}
				}
			}
		}

	} catch(Exception & N) {
		std::cout << "ERROR: TTubo::ComunicacionDPF en el Tubo " <<
				  FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

#endif
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::IniciaVariablesFundamentalesTubo() {
#ifdef usetry
	try {
#endif
		double RMezclaIni = 0., CpMezclaIni = 0., CvMezclaIni = 0., GammaIni = 0.;

		FPresion0 = new double[FNin];
		FAsonido0 = new double[FNin];
		FVelocidad0 = new double[FNin];
		FPresion1 = new double[FNin];
		FAsonido1 = new double[FNin];
		FVelocidadMedia = new double[FNin];
		FPresionMedia = new double[FNin];
		FAsonidoMedia = new double[FNin];
		FVelocidad1 = new double[FNin];

		FVelocidadDim = new double[FNin];
		FAsonidoDim = new double[FNin];
		FTemperature = new double[FNin];
		FFlowMass = new double[FNin];

		FFraccionMasicaEspecie = new double*[FNin];
		for(int i = 0; i < FNin; i++)
			FFraccionMasicaEspecie[i] = new double[FNumeroEspecies - FIntEGR];
		FFraccionMasicaCC = new double*[2];
		for(int i = 0; i < 2; i++)
			FFraccionMasicaCC[i] = new double[FNumeroEspecies - FIntEGR];
		FVelocidadCC = new double[2];
		FDensidadCC = new double[2];
		FAreaCC = new double[2];
		FGamma = new double[FNin];
		FGamma1 = new double[FNin];
		FGamma3 = new double[FNin];
		FGamma4 = new double[FNin];
		FGamma5 = new double[FNin];
		FGamma6 = new double[FNin];
		FRMezcla = new double[FNin];

		FUt = new double*[3];
		for(int i = 0; i < 3; i++)
			FUt[i] = new double[FNin];
		FU0 = new double*[FNumEcuaciones];
		for(int i = 0; i < FNumEcuaciones; i++)
			FU0[i] = new double[FNin];
		FU0Sum = new double*[FNumEcuaciones];
		for(int i = 0; i < FNumEcuaciones; i++)
			FU0Sum[i] = new double[FNin];
		for(int i = 0; i < FNin; i++) {
			for(int k = 0; k < FNumEcuaciones; k++) {
				FU0Sum[k][i] = 0;
			}
		}
		FU0Medio = new double*[FNumEcuaciones];
		for(int i = 0; i < FNumEcuaciones; i++)
			FU0Medio[i] = new double[FNin];
		FU1 = new double*[FNumEcuaciones];
		for(int i = 0; i < FNumEcuaciones; i++)
			FU1[i] = new double[FNin];
		FU12 = new double*[FNumEcuaciones];
		for(int i = 0; i < FNumEcuaciones; i++)
			FU12[i] = new double[FNin];
		FW = new double*[FNumEcuaciones];
		for(int i = 0; i < FNumEcuaciones; i++)
			FW[i] = new double[FNin];
		FV1 = new double*[FNumEcuaciones];
		for(int i = 0; i < FNumEcuaciones; i++)
			FV1[i] = new double[FNin];
		FV2 = new double*[FNumEcuaciones];
		for(int i = 0; i < FNumEcuaciones; i++)
			FV2[i] = new double[FNin];
		if(FMod.SubModelo == nmFCT) {
			FUfct0 = new double*[FNumEcuaciones];
			for(int i = 0; i < FNumEcuaciones; i++)
				FUfct0[i] = new double[FNin + 1];
			FUfct1 = new double*[FNumEcuaciones];
			for(int i = 0; i < FNumEcuaciones; i++)
				FUfct1[i] = new double[FNin + 1];
			FUfctd = new double*[FNumEcuaciones];
			for(int i = 0; i < FNumEcuaciones; i++)
				FUfctd[i] = new double[FNin + 1];
			FUfctad = new double*[FNumEcuaciones];
			for(int i = 0; i < FNumEcuaciones; i++)
				FUfctad[i] = new double[FNin + 1];
			Ffl = new double*[FNumEcuaciones];
			for(int i = 0; i < FNumEcuaciones; i++)
				Ffl[i] = new double[FNin + 1];
			FdU = new double*[FNumEcuaciones];
			for(int i = 0; i < FNumEcuaciones; i++)
				FdU[i] = new double[FNin + 1];
			FDeltaFCTd = new double*[FNumEcuaciones];
			for(int i = 0; i < FNumEcuaciones; i++)
				FDeltaFCTd[i] = new double[FNin + 1];
			FflU = new double*[FNumEcuaciones];
			for(int i = 0; i < FNumEcuaciones; i++)
				FflU[i] = new double[FNin + 1];
			FaU = new double*[FNumEcuaciones];
			for(int i = 0; i < FNumEcuaciones; i++)
				FaU[i] = new double[FNin + 1];
		}

		Frho = new double[FNin];
		FRe = new double[FNin];

		if(FMod.Modelo == nmTVD)
			DimensionaTVD();

		FDerLin = new double[FNin - 1];
		FDerLin12 = new double[FNin - 1];

		FDerLinArea = new double[FNin - 1];
		FDerLinArea12 = new double[FNin - 1];

		FTime0 = 0.;
		FTime1 = 0.;
		FDeltaTime = 0.;

		for(int i = 0; i < FNin - 1; i++) {
			FDerLin[i] = DerLinF(FDiametroTubo[i], FDiametroTubo[i + 1], FXref);
			FDerLinArea[i] = DerLinFArea(FArea[i], FArea[i + 1], FXref);
		}

		for(int i = 1; i < FNin - 1; i++) {
			FDerLin12[i] = DerLinF(FDiametroD12[i], FDiametroD12[i - 1], FXref);
			FDerLinArea12[i] = DerLinFArea(FArea12[i - 1], FArea12[i], FXref);
		}

		// Calculo de Gamma y R para la composicion inicial.
		if(FCalculoEspecies == nmCalculoCompleto) {

			RMezclaIni = CalculoCompletoRMezcla(FComposicionInicial[0], FComposicionInicial[1], FComposicionInicial[2], 0,
												FCalculoGamma, nmMEP);
			CpMezclaIni = CalculoCompletoCpMezcla(FComposicionInicial[0], FComposicionInicial[1], FComposicionInicial[2], 0,
												  __units::degCToK(FTini), FCalculoGamma, nmMEP);
			GammaIni = CalculoCompletoGamma(RMezclaIni, CpMezclaIni, FCalculoGamma);

		} else if(FCalculoEspecies == nmCalculoSimple) {

			RMezclaIni = CalculoSimpleRMezcla(FComposicionInicial[0], FComposicionInicial[1], FCalculoGamma, nmMEP);
			CvMezclaIni = CalculoSimpleCvMezcla(__units::degCToK(FTini), FComposicionInicial[0], FComposicionInicial[1],
												FCalculoGamma, nmMEP);
			GammaIni = CalculoSimpleGamma(RMezclaIni, CvMezclaIni, FCalculoGamma);

		}

		for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
			FFraccionMasicaCC[0][j] = FComposicionInicial[j];
			FFraccionMasicaCC[1][j] = FComposicionInicial[j];
		}

		double viscgas = 1.4615e-6 * pow150(__units::degCToK(FTini)) / (__units::degCToK(FTini) + 110.4);

		for(int i = 0; i < FNin; i++) {

			for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
				FFraccionMasicaEspecie[i][j] = FComposicionInicial[j];
			}
			FRMezcla[i] = RMezclaIni;
			FGamma[i] = GammaIni;
			FGamma1[i] = __Gamma::G1(FGamma[i]);
			FGamma3[i] = __Gamma::G3(FGamma[i]);
			FGamma4[i] = __Gamma::G4(FGamma[i]);
			FGamma5[i] = __Gamma::G5(FGamma[i]);
			FGamma6[i] = __Gamma::G6(FGamma[i]);

			FPresion0[i] = FPini;
			FTemperature[i] = __units::degCToK(FTini);
			FAsonidoDim[i] = sqrt(__units::degCToK(FTini) * FGamma[i] * FRMezcla[i]);
			FAsonido0[i] = FAsonidoDim[i] / __cons::ARef;
			FVelocidadDim[i] = FVelMedia;
			FVelocidad0[i] = FVelMedia / __cons::ARef;
			FPresion1[i] = FPini;
			FAsonido1[i] = FAsonido0[i];
			FVelocidad1[i] = FVelocidad0[i];
			FFlowMass[i] = FArea[i] * FVelocidadDim[i] * __units::BarToPa(FPresion0[i]) / FRMezcla[i] / FTemperature[i];

			if(FMod.FormulacionLeyes == nmSinArea) {
				Transforma1(FVelocidad0[i], FAsonido0[i], FPresion0[i], FU0, FGamma[i], FGamma1[i], FFraccionMasicaEspecie[i], i);

				Transforma1(FVelocidad1[i], FAsonido1[i], FPresion1[i], FU1, FGamma[i], FGamma1[i], FFraccionMasicaEspecie[i], i);

				Frho[i] = FU0[0][i];
			} else if(FMod.FormulacionLeyes == nmConArea) {
				Transforma1Area(FVelocidad0[i], FAsonido0[i], FPresion0[i], FU0, FArea[i], FGamma[i], FGamma1[i],
								FFraccionMasicaEspecie[i], i);

				Transforma1Area(FVelocidad1[i], FAsonido1[i], FPresion1[i], FU1, FArea[i], FGamma[i], FGamma1[i],
								FFraccionMasicaEspecie[i], i);

				Frho[i] = FU0[0][i] / FArea[i];
			}

			else {
				std::cout << "ERROR: El tipo de formulacion de las leyes de conservacion no esta bien definido" << std::endl;
				throw Exception("");
			}
			FRe[i] = Frho[i] * FVelocidadDim[i] * FDiametroTubo[i] / viscgas;
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::IniciaVariableFundamentalesTubo en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::ActualizaPropiedadesGas() {
#ifdef usetry
	try {
#endif
		for(int i = 0; i < FNin; i++) {
			FTemperature[i] = pow2(FAsonidoDim[i]) / (FGamma[i] * FRMezcla[i]);

			// Calculo de Gamma y R a partir de la composicion en cada nodo.
			if(FCalculoEspecies == nmCalculoSimple) {

				FRMezcla[i] = CalculoSimpleRMezcla(FFraccionMasicaEspecie[i][0], 0, FCalculoGamma, nmMEP);
				double CvMezcla = CalculoSimpleCvMezcla(FTemperature[i], FFraccionMasicaEspecie[i][0], 0, FCalculoGamma, nmMEP);
				FGammaN = CalculoSimpleGamma(FRMezcla[i], CvMezcla, FCalculoGamma);
				if(abs(FGammaN - FGamma[i]) > 0.025) {
					FGamma[i];
				} else {
					FGamma[i] = 0.9995 * FGamma[i] + 0.0005 * FGammaN;
				}
			} else {

				FRMezcla[i] = CalculoCompletoRMezcla(FFraccionMasicaEspecie[i][0], FFraccionMasicaEspecie[i][1],
													 FFraccionMasicaEspecie[i][2], 0, FCalculoGamma, nmMEP);
				double CpMezcla = CalculoCompletoCpMezcla(FFraccionMasicaEspecie[i][0], FFraccionMasicaEspecie[i][1],
								  FFraccionMasicaEspecie[i][2], 0, FTemperature[i], FCalculoGamma, nmMEP);
				FGammaN = CalculoCompletoGamma(FRMezcla[i], CpMezcla, FCalculoGamma);
				if(abs(FGammaN - FGamma[i]) > 0.025) {
					FGamma[i];
				} else {
					FGamma[i] = 0.9995 * FGamma[i] + 0.0005 * FGammaN;
				}

			}

			FGamma1[i] = __Gamma::G1(FGamma[i]);
			FGamma3[i] = __Gamma::G3(FGamma[i]);
			FGamma4[i] = __Gamma::G4(FGamma[i]);
			FGamma5[i] = __Gamma::G5(FGamma[i]);
			FGamma6[i] = __Gamma::G6(FGamma[i]);

			Frho[i] = __units::BarToPa(FPresion0[i]) / FRMezcla[i] / FTemperature[i];
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculoPropiedadesGas en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::Transforma1(const double& v, const double& a, const double& p, double **U, const double& Gamma,
						const double& Gamma1, double *Yespecie, const int& i) {
#ifdef usetry
	try {
#endif
		double V = v * __cons::ARef;
		double Ppa = __units::BarToPa(p);

		U[0][i] = Gamma * Ppa / pow2(a * __cons::ARef);
		U[1][i] = U[0][i] * V;
		U[2][i] = Ppa / Gamma1 + U[1][i] * V / 2.0;
		for(int j = 3; j < 3 + (FNumeroEspecies - 2); j++) {
			U[j][i] = U[0][i] * Yespecie[j - 3];
		}
		if(FHayEGR)
			U[3 + (FNumeroEspecies - 2)][i] = U[0][i] * Yespecie[FNumeroEspecies - 1];
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::Transforma1 en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::Transforma1Area(const double& v, const double& a, const double& p, double **U, const double& area,
							const double& Gamma, const double& Gamma1, double *Yespecie, const int& i) {
#ifdef usetry
	try {
#endif
		double APpa = area * __units::BarToPa(p);
		double V = v * __cons::ARef;

		U[0][i] = Gamma * APpa / pow2(a * __cons::ARef);
		U[1][i] = U[0][i] * V;
		U[2][i] = APpa / Gamma1 + U[1][i] * V / 2.0;
		for(int j = 3; j < 3 + (FNumeroEspecies - 2); j++) {
			U[j][i] = U[0][i] * Yespecie[j - 3];
		}
		if(FHayEGR)
			U[3 + (FNumeroEspecies - 2)][i] = U[0][i] * Yespecie[FNumeroEspecies - 1];

#ifdef usetry
	}

	catch(exception & N) {
		std::cout << "ERROR: TTubo::Transforma1Area en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::Transforma2(double& v, double& a, double& p, double **U, const double& Gamma, const double& Gamma1,
						double *Yespecie, const int& i) {
#ifdef usetry
	try {
#endif
		double fraccionmasicaacum = 0.;
		double V = U[1][i] / U[0][i];
		v = V / __cons::ARef;
		double P = (U[2][i] - U[1][i] * V / 2.0) * Gamma1;
		p = __units::PaToBar(P);
		a = sqrt(Gamma * P / U[0][i] / __cons::ARef2);
		if(v > 1e200 || v < -1e200) {
			std::cout << "ERROR: Valor de velocidad no valido" << std::endl;
			throw Exception("Error Velociad");
		}
		if(p > 1e200 || p < 0) {
			std::cout << "ERROR: Valor de presion no valido en el tubo " << FNumeroTubo << " nodo " << i << std::endl;
			throw Exception("Error presion");
		}
		if(a > 1e200 || a < 0) {
			std::cout << "ERROR: Valor de velocidad del sonido no valido" << std::endl;
			throw Exception("Error velocidad del sonido");
		}

		// Solucion del Transporte de Especies Quimicas.
		for(int j = 0; j < FNumeroEspecies - 2; j++) {
			Yespecie[j] = U[j + 3][i] / U[0][i];
			fraccionmasicaacum += Yespecie[j];
		}
		Yespecie[FNumeroEspecies - 2] = 1. - fraccionmasicaacum;
		if(FHayEGR)
			Yespecie[FNumeroEspecies - 1] = U[FNumeroEspecies - 2 + 3][i] / U[0][i];
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::Transforma2 en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::Transforma2Area(double& v, double& a, double& p, double **U, const double& area, const double& Gamma,
							const double& Gamma1, double *Yespecie, const int& i) {
#ifdef usetry
	try {
#endif
		double fraccionmasicaacum = 0.;
		if(U[0][i] < 0) {
			std::cout << "ERROR: Calculation in pipe " << FNumeroTubo << " is unstable" << std::endl;
			if(FMod.Modelo == nmLaxWendroff)
				std::cout << "       Try to use TVD scheme for this pipe" << std::endl;
			else
				std::cout << "       Check the input data" << std::endl;
			throw Exception("ERROR: the pipe calculation is unstable");
		}

		double V = U[1][i] / U[0][i];
		double P = (U[2][i] - U[1][i] * V / 2.0) * Gamma1;

		v = V / __cons::ARef;
		p = __units::PaToBar(P) / area;
		a = sqrt(Gamma * P / U[0][i] / __cons::ARef2);

		for(int j = 0; j < FNumeroEspecies - 2; j++) {
			Yespecie[j] = U[j + 3][i] / U[0][i];
			fraccionmasicaacum += Yespecie[j];
		}
		Yespecie[FNumeroEspecies - 2] = 1. - fraccionmasicaacum;
		if(FHayEGR)
			Yespecie[FNumeroEspecies - 1] = U[FNumeroEspecies - 2 + 3][i] / U[0][i];
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::Transforma2Area en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::Transforma3Area(double **Ufct, double **U, double Area, double Gamma, double Gamma1, double Gamma6, int i) {
#ifdef usetry
	try {
#endif

		Ufct[0][i] = U[1][i]; // Massflow
		Ufct[1][i] = Gamma * U[2][i] / U[0][i] - pow2(U[1][i]) * Gamma1 / 2. / U[0][i] / U[0][i];
		Ufct[2][i] = ((U[2][i] - pow2(U[1][i]) / U[0][i] / 2.) * Gamma1) * pow(
						 (1. + (Gamma1 / 2.) * pow2(U[1][i]) / U[0][i] / U[0][i] / (Gamma * ((U[2][i] - pow2(
									 U[1][i]) / U[0][i] / 2.) * Gamma1) / U[0][i])), Gamma * Gamma6) / Area;

		for(int j = 3; j < FNumEcuaciones; j++) {
			Ufct[j][i] = U[j][i] * U[1][i] / U[0][i];
			// Massflow de cada especie.
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::Transforma3Area en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::Transforma4Area(double **U1, double **Ufctd, double Area, double Gamma, double Gamma1, double Gamma3,
							double Gamma4, double Gamma6, int i) {
	double error = 0., fu = 0., dfu = 0., vel = 0., vel1 = 0.;
	double v, a, p, *Y;
	bool peta = false;
	// double pruebadefuego1,pruebadefuego2;
#ifdef usetry
	try {
#endif

		Y = new double[FNumeroEspecies - 1];
		// secante = false;
		vel = Ufctd[0][i] / (U1[0][i]);
		error = 1.;

		// Newton Raphson
		while(error > 0.00000001) {

			fu = vel - Ufctd[0][i] / (Area * Ufctd[2][i] * Gamma4) * pow(2. * Ufctd[1][i],
					(Gamma / Gamma1)) * pow((2. * Ufctd[1][i] - pow2(vel)), -Gamma6);

			dfu = 1. - Ufctd[0][i] * vel / (Area * Ufctd[2][i] * Gamma) * pow(2. * Ufctd[1][i],
					(Gamma / Gamma1)) * pow((2. * Ufctd[1][i] - pow2(vel)), -Gamma / Gamma1);

			vel1 = vel - fu / dfu;
			error = fabs(vel1 - vel);
			vel = vel1;
		}

		if(!peta) {
			v = vel / __cons::ARef;
			a = sqrt(Gamma1 * (Ufctd[1][i] - (vel * vel) / 2.)) / __cons::ARef;
			p = Ufctd[2][i] / pow((1 + Gamma3 * pow2(v / a)), Gamma / Gamma1) / 1.e5;
			for(int j = 0; j < FNumeroEspecies - 1 - FIntEGR; j++) {
				if(Ufctd[0][i] != 0.) {
					Y[j] = Ufctd[j + 3][i] / Ufctd[0][i];
					// Fraccion masica de cada especie.
				} else {
					Y[j] = FFraccionMasicaEspecie[i][j];
				}
			}

			Transforma1Area(v, a, p, U1, Area, Gamma, Gamma1, Y, i);
		}

		delete[] Y;
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::Transforma4Area en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::IniciaVariablesTransmisionCalor(TCondicionContorno **BC, TBloqueMotor **Engine, double AmbientTemperature) {
	double dist1 = 0., dist2 = 0.;
#ifdef usetry
	try {
#endif

		Fhi = new double[FNin];
		Fhe = new double[FNin];
		FVelPro = new double[FNin];
		FCoefTurbulencia = new double[FNin];
		FSUMTPTuboPro = new double**[2];
		for(int i = 0; i < 2; i++)
			FSUMTPTuboPro[i] = new double*[3];
		for(int i = 0; i < 2; i++) {
			for(int j = 0; j < 3; j++) {
				FSUMTPTuboPro[i][j] = new double[FNin];
			}
		}
		FTPTubo = new double*[3];
		for(int i = 0; i < 3; i++)
			FTPTubo[i] = new double[FNin];
		FTParedAnt = new double*[3];
		for(int i = 0; i < 3; i++)
			FTParedAnt[i] = new double[FNin];

		for(int i = 0; i < FNin; i++) {
			dist1 = FXref * (double) i + BC[FNodoIzq - 1]->getPosicionNodo();
			dist2 = FXref * (double)(FNin - 1 - i) + BC[FNodoDer - 1]->getPosicionNodo();
			if(Minimo(dist1, dist2) >= 10000.)
				FCoefTurbulencia[i] = 1.;
			else
				FCoefTurbulencia[i] = 1. + 3. * exp(-Minimo(dist1, dist2) / (4 * FDiametroTubo[i]));
			for(int j = 0; j < 3; j++) {
				FTPTubo[j][i] = FTIniParedTub;
				FTParedAnt[j][i] = FTIniParedTub;
			}
			for(int k = 0; k < 2; k++) {
				for(int j = 0; j < 3; j++) {
					FSUMTPTuboPro[k][j][i] = 0.;
				}
			}
			FVelPro[i] = 5.;
			Fhi[i] = 0.;
			Fhe[i] = 0.;

		}
		if(FTipoCalcTempPared != nmTempConstante) {
			if(FTipoTransCal == nmPipaAdmision || FTipoTransCal == nmPipaEscape) {
				FTExt = __units::degCToK(Engine[0]->getTempRefrigerante());
			} else {
				if(FTipRefrig == nmAgua)
					FTExt = __units::degCToK(FTRefrigerante);
				else
					FTExt = __units::degCToK(AmbientTemperature);
			}
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::IniciaVariablesTransmisionCalor en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::EstabilidadMetodoCalculo() {
	double VTotalMax = 0., VTotalNodo = 0.;
#ifdef usetry
	try {
#endif

		VTotalMax = (FAsonidoDim[0] + fabs(FVelocidadDim[0]));
		for(int i = 0; i < FNin; i++) {
			VTotalNodo = (FAsonidoDim[i] + fabs(FVelocidadDim[i]));
			if(VTotalNodo > VTotalMax)
				VTotalMax = VTotalNodo;
		}
		FDeltaTime = FCourant * FXref / VTotalMax;

		if(FMod.Modelo == nmTVD) {
			TVD_Estabilidad();
		}

		FTime0 = FTime1;
		FTime1 = FTime0 + FDeltaTime;
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::EstabilidadMetodoCalculo en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculaVariablesFundamentales() {
#ifdef usetry
	try {
#endif
		if(FMod.Modelo == nmLaxWendroff && FMod.FormulacionLeyes == nmSinArea)
			LaxWendroff();
		else if(FMod.Modelo == nmLaxWendroff && FMod.FormulacionLeyes == nmConArea) {
			LaxWendroffArea();
			if(FMod.SubModelo == nmFCT) {
				// ReduccionFlujoSubsonicoFCT();
				FluxCorrectedTransport();
			}
		} else if(FMod.Modelo == nmTVD) {
			TVD_Limitador();
		} else {
			std::cout << "ERROR: Metodo de calculo no implementado" << std::endl;
			throw Exception("");
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaVariablesFundamentales en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::LaxWendroff() {
#ifdef usetry
	try {
#endif
		int Nodos = 0;
		double x1, x2, x3, x4, *hi12, *rho12, *Re12, *TPTubo12, *Gamma12, *Rmezcla12, *Gamma1_12;

		hi12 = new double[FNin - 1];
		rho12 = new double[FNin - 1];
		Re12 = new double[FNin - 1];
		TPTubo12 = new double[FNin - 1];
		Gamma12 = new double[FNin - 1];
		Rmezcla12 = new double[FNin - 1];
		Gamma1_12 = new double[FNin - 1];

		double dtdx = FDeltaTime / FXref;
		double dt2 = FDeltaTime / 2;

		// Paso Primero
		Nodos = FNin;

		CalculaFlujo(FU0, FW, FGamma, FGamma1, Nodos);

		CalculaFuente1(FU0, FV1, FGamma, FGamma1, Nodos);

		CalculaFuente2(FU0, FV2, FDiametroTubo, Fhi, Frho, FRe, FTPTubo[0], FGamma, FRMezcla, FGamma1, Nodos);

		for(int i = 0; i < FNin - 1; i++) {
			for(int j = 0; j < FNumEcuaciones; j++) {
				x1 = FU0[j][i] + FU0[j][i + 1];
				x2 = -dtdx * (FW[j][i + 1] - FW[j][i]);
				x3 = -dt2 * (FV1[j][i + 1] + FV1[j][i]) * FDerLin[i];
				x4 = -dt2 * (FV2[j][i + 1] + FV2[j][i]);
				FU12[j][i] = (x1 + x2 + x3 + x4) / 2;
			}
		}

		// Paso segundo
		for(int i = 0; i < FNin - 1; i++) {
			hi12[i] = (Fhi[i] + Fhi[i + 1]) / 2.;
			rho12[i] = (Frho[i] + Frho[i + 1]) / 2.;
			Re12[i] = (FRe[i] + FRe[i + 1]) / 2.;
			TPTubo12[i] = (FTPTubo[0][i] + FTPTubo[0][i + 1]) / 2.;
			Gamma12[i] = (FGamma[i] + FGamma[i + 1]) / 2.;
			Rmezcla12[i] = (FRMezcla[i] + FRMezcla[i + 1]) / 2.;
			Gamma1_12[i] = (FGamma1[i] + FGamma1[i + 1]) / 2.;
		}
		Nodos = FNin - 1;
		CalculaFlujo(FU12, FW, Gamma12, Gamma1_12, Nodos);

		CalculaFuente1(FU12, FV1, Gamma12, Gamma1_12, Nodos);

		CalculaFuente2(FU12, FV2, FDiametroD12, hi12, rho12, Re12, TPTubo12, Gamma12, Rmezcla12, Gamma1_12, Nodos);

		for(int i = 1; i < FNin - 1; i++) {
			for(int j = 0; j < FNumEcuaciones; j++) {
				x1 = FU0[j][i];
				x2 = -dtdx * (FW[j][i] - FW[j][i - 1]);
				x3 = -dt2 * (FV1[j][i] + FV1[j][i - 1]) * FDerLin12[i];
				x4 = -dt2 * (FV2[j][i] + FV2[j][i - 1]);
				FU1[j][i] = x1 + x2 + x3 + x4;
			}
		}

		// Liberacion de memoria en vectores locales.
		delete[] hi12;
		delete[] rho12;
		delete[] Re12;
		delete[] TPTubo12;
		delete[] Gamma12;
		delete[] Rmezcla12;
		delete[] Gamma1_12;
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::LaxWendrof en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::FluxCorrectedTransport() {
	double c1 = 0., c2 = 0., c3 = 0., c4 = 0., sign = 0.;
#ifdef usetry
	try {
#endif
		if(FNin > 3) {
			for(int k = 0; k < FNumEcuaciones; k++) {
				FU1[k][0] = FU0[k][0];
				FU1[k][FNin - 1] = FU0[k][FNin - 1];
			}
			// Transformacion de variables
			for(int i = 0; i < FNin; i++) {
				Transforma3Area(FUfct0, FU0, FArea[i], FGamma[i], FGamma1[i], FGamma6[i], i);

				Transforma3Area(FUfct1, FU1, FArea[i], FGamma[i], FGamma1[i], FGamma6[i], i);

			}

			// Etapa de Difusion
			if(FMod.Difusion == nmDamping) {
				for(int i = 0; i < FNin - 1; i++) {
					for(int k = 0; k < FNumEcuaciones; k++) {
						Ffl[k][i] = (FUfct0[k][i + 1] - FUfct0[k][i]) / 8.;
					}
				}
			} else if(FMod.Difusion == nmSmoothing) {
				for(int i = 0; i < FNin - 1; i++) {
					for(int k = 0; k < FNumEcuaciones; k++) {
						Ffl[k][i] = (FUfct1[k][i + 1] - FUfct1[k][i]) / 8.;
					}
				}
			} else {
				std::cout << "ERROR: Metodo de Difusion mal definido para el FCT en el tubo n. " << FNumeroTubo << std::endl;
				throw Exception("");
			}

			for(int i = 1; i < FNin - 1; i++) {
				for(int k = 0; k < FNumEcuaciones; k++) {
					FdU[k][i] = Ffl[k][i] - Ffl[k][i - 1];
					FUfctd[k][i] = FUfct1[k][i] + FdU[k][i];
				}
			}

			for(int k = 0; k < FNumEcuaciones; k++) {
				FUfctd[k][0] = FUfct1[k][0];
				FUfctd[k][FNin - 1] = FUfct1[k][FNin - 1];
			}

			// Etapa de Antidifusion
			if(FMod.Antidifusion == nmExplicit) {
				for(int i = 1; i < FNin; i++) {
					for(int k = 0; k < FNumEcuaciones; k++) {
						FDeltaFCTd[k][i] = FUfctd[k][i] - FUfctd[k][i - 1];
					}
				}
			} else if(FMod.Antidifusion == nmNaive) {
				for(int i = 1; i < FNin; i++) {
					for(int k = 0; k < FNumEcuaciones; k++) {
						FDeltaFCTd[k][i] = FUfct0[k][i] - FUfct0[k][i - 1];
					}
				}
			} else if(FMod.Antidifusion == nmPhoenical) {
				for(int i = 1; i < FNin; i++) {
					for(int k = 0; k < FNumEcuaciones; k++) {
						FDeltaFCTd[k][i] = FUfct1[k][i] - FUfct1[k][i - 1];
					}
				}
			} else {
				std::cout << "ERROR: Metodo de Antidifusion mal definido para el FCT en el tubo n. " << FNumeroTubo << std::endl;
				throw Exception("");
			}

			for(int k = 0; k < FNumEcuaciones; k++) {
				FDeltaFCTd[k][0] = FDeltaFCTd[k][1];
				FDeltaFCTd[k][FNin] = FDeltaFCTd[k][FNin - 1];
			}
			for(int i = 1; i < FNin - 2; i++) {
				for(int k = 0; k < FNumEcuaciones; k++) {
					if(FDeltaFCTd[k][i + 1] >= 0.) {
						sign = 1.;
					} else {
						sign = -1.;
					}
					c1 = 5. * sign * FDeltaFCTd[k][i] / 8.;
					c2 = fabs(FDeltaFCTd[k][i + 1]) / 8.;
					c3 = 5. * sign * FDeltaFCTd[k][i + 2] / 8.;
					if(c1 < c2)
						c4 = c1;
					else
						c4 = c2;
					if(c3 < c4)
						c4 = c3;
					if(c4 > 0.)
						FflU[k][i] = sign * c4;
					else
						FflU[k][i] = 0.;
				}
			}

			// PipeEnd Izquierdo
			for(int k = 0; k < FNumEcuaciones; k++) {
				if(FDeltaFCTd[k][1] >= 0.) {
					sign = 1.;
				} else {
					sign = -1.;
				}
				c2 = fabs(FDeltaFCTd[k][1]) / 8.;
				c3 = 5. * sign * FDeltaFCTd[k][2] / 8.;
				if(c2 < c3)
					c4 = c2;
				else
					c4 = c3;
				if(c4 > 0.)
					FflU[k][0] = sign * c4;
				else
					FflU[k][0] = 0.;
			}
			// PipeEnd Derecho
			for(int k = 0; k < FNumEcuaciones; k++) {
				if(FDeltaFCTd[k][FNin - 2] >= 0.) {
					sign = 1.;
				} else {
					sign = -1.;
				}
				c1 = 5. * sign * FDeltaFCTd[k][FNin - 2] / 8.;
				c2 = fabs(FDeltaFCTd[k][FNin - 1]) / 8.;
				if(c1 < c2)
					c4 = c1;
				else
					c4 = c2;
				if(c4 > 0.)
					FflU[k][FNin - 2] = sign * c4;
				else
					FflU[k][FNin - 2] = 0.;
			}

			for(int i = 1; i < FNin - 1; i++) {
				for(int k = 0; k < FNumEcuaciones; k++) {
					FaU[k][i] = -FflU[k][i] + FflU[k][i - 1];
					FUfctad[k][i] = FUfctd[k][i] + FaU[k][i];
				}
				Transforma4Area(FU1, FUfctad, FArea[i], FGamma[i], FGamma1[i], FGamma3[i], FGamma4[i], FGamma6[i], i);
			}

		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::FluxCorrectedTransport en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::LaxWendroffArea() {
#ifdef usetry
	try {
#endif
		int Nodos = 0;
		double x1, x2, x3, x4, *hi12, *rho12, *Re12, *TPTubo12, *Gamma12, *Rmezcla12, *Gamma1_12;

		hi12 = new double[FNin - 1];
		rho12 = new double[FNin - 1];
		Re12 = new double[FNin - 1];
		TPTubo12 = new double[FNin - 1];
		Gamma12 = new double[FNin - 1];
		Rmezcla12 = new double[FNin - 1];
		Gamma1_12 = new double[FNin - 1];

		double dtdx = FDeltaTime / FXref;
		double dt2 = FDeltaTime / 2.;

		// Paso Primero
		Nodos = FNin;

		CalculaFlujo(FU0, FW, FGamma, FGamma1, Nodos);

		CalculaFuente1Area(FU0, FV1, FArea, FGamma1, Nodos);

		CalculaFuente2Area(FU0, FV2, FArea, Fhi, Frho, FRe, FTPTubo[0], FGamma, FRMezcla, FGamma1, Nodos);

		for(int i = 0; i < FNin - 1; i++) {
			for(int j = 0; j < FNumEcuaciones; j++) {
				x1 = (FU0[j][i] + FU0[j][i + 1]);
				x2 = -dtdx * (FW[j][i + 1] - FW[j][i]);
				x3 = -dt2 * (FV1[j][i + 1] + FV1[j][i]) * FDerLinArea[i];
				x4 = -dt2 * (FV2[j][i + 1] + FV2[j][i]);
				FU12[j][i] = (x1 + x2 + x3 + x4) / 2.;
			}
		}

		// Paso segundo
		for(int i = 0; i < FNin - 1; i++) {
			hi12[i] = (Fhi[i] + Fhi[i + 1]) / 2.;
			rho12[i] = (Frho[i] + Frho[i + 1]) / 2.;
			Re12[i] = (FRe[i] + FRe[i + 1]) / 2.;
			TPTubo12[i] = (FTPTubo[0][i] + FTPTubo[0][i + 1]) / 2.;
			Gamma12[i] = (FGamma[i] + FGamma[i + 1]) / 2.;
			Rmezcla12[i] = (FRMezcla[i] + FRMezcla[i + 1]) / 2.;
			Gamma1_12[i] = (FGamma1[i] + FGamma1[i + 1]) / 2.;
		}
		Nodos = FNin - 1;
		CalculaFlujo(FU12, FW, Gamma12, Gamma1_12, Nodos);

		CalculaFuente1Area(FU12, FV1, FArea12, Gamma1_12, Nodos);

		CalculaFuente2Area(FU12, FV2, FArea12, hi12, rho12, Re12, TPTubo12, Gamma12, Rmezcla12, Gamma1_12, Nodos);

		for(int i = 1; i < FNin - 1; i++) {
			for(int j = 0; j < FNumEcuaciones; j++) {
				x1 = FU0[j][i];
				x2 = -dtdx * (FW[j][i] - FW[j][i - 1]);
				x3 = -dt2 * (FV1[j][i] + FV1[j][i - 1]) * FDerLinArea12[i];
				x4 = -dt2 * (FV2[j][i] + FV2[j][i - 1]);
				FU1[j][i] = x1 + x2 + x3 + x4;
			}
		}

		// Liberacion de memoria en vectores locales.
		delete[] hi12;
		delete[] rho12;
		delete[] Re12;
		delete[] TPTubo12;
		delete[] Gamma12;
		delete[] Rmezcla12;
		delete[] Gamma1_12;
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::LaxWendroffArea en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculaFlujo(double **U, double **W, double *Gamma, double *Gamma1, int Nodos) {
#ifdef usetry
	try

	{
#endif
		double U1U0 = 0.;

		for(int i = 0; i < Nodos; i++) {
			if(!DoubEqZero(U[1][i])) {
				U1U0 = U[1][i] / U[0][i];

				W[0][i] = U[1][i];
				W[1][i] = U[2][i] * Gamma1[i] - (Gamma[i] - 3.0) * U[1][i] * U1U0 / 2.;
				W[2][i] = Gamma[i] * U[2][i] * U1U0 - Gamma1[i] * U[1][i] * pow2(U1U0) / 2.;
				for(int j = 3; j < FNumEcuaciones; j++) {
					W[j][i] = U[j][i] * U1U0;
				}
			} else {
				W[0][i] = 0;
				W[1][i] = U[2][i] * Gamma1[i];
				W[2][i] = 0;
				for(int j = 3; j < FNumEcuaciones; j++) {
					W[j][i] = 0;
				}
			}
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaFlujo en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculaFuente1(double **U, double **V1, double *Gamma, double *Gamma1, int Nodos) {
#ifdef usetry
	try {
#endif
		double U1U0 = 0.;
		for(int i = 0; i < Nodos; i++) {
			U1U0 = U[1][i] / U[0][i];

			V1[0][i] = U[1][i];
			V1[1][i] = U[1][i] * U1U0;
			V1[2][i] = Gamma[i] * U[2][i] * U1U0 - Gamma1[i] * U[1][i] * pow2(U1U0) / 2.;
			for(int j = 3; j < FNumEcuaciones; j++) {
				V1[j][i] = U[j][i] * U1U0;
			}
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaFuente1 en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculaFuente1Area(double **U, double **V1, double *Area, double *Gamma1, int Nodos) {
	double p = 0.;
#ifdef usetry
	try {
#endif
		for(int i = 0; i < Nodos; i++) {
			if(DoubEqZero(U[1][i])) {
				p = (U[2][i]) * Gamma1[i] / Area[i];
			} else {
				p = (U[2][i] - pow2(U[1][i]) / U[0][i] / 2.0) * Gamma1[i] / Area[i];
			}

			V1[0][i] = 0.;
			V1[1][i] = -p;
			V1[2][i] = 0.;
			for(int j = 3; j < FNumEcuaciones; j++) {
				V1[j][i] = 0.;
			}
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaFuente1Area en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculaFuente2(double **U, double **V2, double *diame, double *hi, double *rho, double *Re,
						   double *TempParedTubo, double *Gamma, double *Rmezcla, double *Gamma1, int Nodos) {
	double v = 0, a = 0., p = 0., tgas = 0., g = 0., q = 0., f = 0.;
#ifdef usetry
	try {
#endif
		for(int i = 0; i < Nodos; i++) {
			/* paso de las variables en funcion de la velocidad,asonido y presion */
			v = U[1][i] / U[0][i];
			p = (U[2][i] - U[1][i] * v / 2.0) * Gamma1[i];
			a = sqrt(Gamma[i] * p / U[0][i]);
			if(v > 1e200 || v < -1e200) {
				std::cout << "ERROR: Valor de velocidad no valido" << std::endl;
				throw Exception("Error Velociad");
			}
			if(p > 1e200 || p < 0) {
				std::cout << "ERROR: Valor de presion no valido en el tubo " << FNumeroTubo << " nodo " << i << std::endl;
				throw Exception("Error presion");
			}
			if(a > 1e200 || a < 0) {
				std::cout << "ERROR: Valor de velocidad del sonido no valido" << std::endl;
				throw Exception("Error velocidad del sonido");
			}

			/* calculo factor friccion */
			if(DoubEqZero(v) || DoubEqZero(FCoefAjusFric)) {
				g = 0.;
			} else {
				Colebrook(FFriccion, diame[i], f, Re[i]);
				g = f * v * v * v / fabs(v) * 2 / diame[i] * FCoefAjusFric;
			}

			/* determinacion factor calor */
			if(DoubEqZero(FCoefAjusTC)) {
				q = 0.;
			} else {
				tgas = a * a / (Gamma[i] * Rmezcla[i]);
				TransmisionCalor(tgas, diame[i], q, hi[i], rho[i], TempParedTubo[i]);
				q = q * FCoefAjusTC;
			}

			/* asignacion a V2 */
			V2[0][i] = 0.0;
			V2[1][i] = U[0][i] * g;
			V2[2][i] = -U[0][i] * q;
			for(int j = 3; j < FNumEcuaciones; j++) {
				V2[j][i] = 0.;
			}
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaFuente1 en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculaFuente2Area(double **U, double **V2, double *Area, double *hi, double *rho, double *Re,
							   double *TempParedTubo, double *Gamma, double *Rmezcla, double *Gamma1, int Nodos) {
	double v = 0., a = 0., pA = 0., tgas = 0., g = 0., q = 0., f = 0.;
	double diame = 0.;
#ifdef usetry
	try {
#endif
		for(int i = 0; i < Nodos; i++) {
			/* paso de las variables en funcion de la velocidad,asonido y presion */
			v = U[1][i] / U[0][i];
			pA = (U[2][i] - U[1][i] * v / 2.0) * Gamma1[i];
			a = sqrt(Gamma[i] * pA / U[0][i]);

			diame = sqrt(Area[i] * __cons::_4_Pi);
			/* calculo factor friccion */
			if(DoubEqZero(v) || DoubEqZero(FCoefAjusFric)) {
				g = 0.;
			} else {
				Colebrook(FFriccion, diame, f, Re[i]);
				g = f * v * v * v / fabs(v) * 2 / diame * FCoefAjusFric;
			}

			/* determinacion factor calor */
			if(DoubEqZero(FCoefAjusTC)) {  /* q=0 */
				q = 0.;
			} else {
				tgas = a * a / (Gamma[i] * Rmezcla[i]);
				TransmisionCalor(tgas, diame, q, hi[i], rho[i], TempParedTubo[i]);
				q = q * FCoefAjusTC;
			}

			/* asignacion a V2 */
			V2[0][i] = 0.0;
			V2[1][i] = U[0][i] * g;
			V2[2][i] = -U[0][i] * q;
			for(int j = 3; j < FNumEcuaciones; j++) {
				V2[j][i] = 0.;
			}
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaFuente1 en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::Colebrook(double rug, double dia, double& f, double Re) {
	double temp = 0., temp2 = 0.;

	double pow_approx = 0.;

	if(Re > 2000) {
		if(Re > 130000.) {
			temp = rug / (3700.0 * dia) + 5.74 / pow(Re, 0.9);
		} else if(Re > 50000.) {
			// pow(Re, 0.9) approximation between Re = 50000 and Re = 130000
			// with less than 0.015 % of error.
			pow_approx = 7.04596786e-13 * pow3(Re) - 3.55878012e-07 * pow2(Re) + 3.34536053e-01 * Re + 1.02365893e+03;
			temp = rug / (3700.0 * dia) + 5.74 / pow_approx;
		} else if(Re > 20000.) {
			// pow(Re, 0.9) approximation between Re = 20000 and Re = 50000
			// with less than 0.015 % of error.
			pow_approx = 5.09028031e-12 * pow3(Re) - 1.00134262e-06 * pow2(Re) + 3.67476796e-01 * Re + 4.40171892e+02;
			temp = rug / (3700.0 * dia) + 5.74 / pow_approx;
		} else if(Re > 10000.) {
			// pow(Re, 0.9) approximation between Re = 10000 and Re = 20000
			// with less than 0.015 % of error.
			pow_approx = 2.92727184e-11 * pow3(Re) - 2.48693415e-06 * pow2(Re) + 3.98901290e-01 * Re + 2.11628209e+02;
			temp = rug / (3700.0 * dia) + 5.74 / pow_approx;
		} else if(Re > 4000.) {
			// pow(Re, 0.9) approximation between Re = 4000 and Re = 10000
			// with less than 0.015 % of error.
			pow_approx = 1.49478492e-10 * pow3(Re) - 5.88098006e-06 * pow2(Re) + 4.31645206e-01 * Re + 1.03406848e+02;
			temp = rug / (3700.0 * dia) + 5.74 / pow_approx;
		} else {
			temp = rug / (3700.0 * dia) + 0.00328895476345;
		}
		if(temp > 0.042) {
			temp2 = pow2(log10(temp));
		} else if(temp > 0.019) {
			// pow2(log10(temp)) approximation between temp = 0.019 and
			// temp = 0.042 with less than 0.02 % of error.
			temp2 = 6.76495011e+05 * pow4(temp) - 1.07370937e+05 * pow3(temp) + 6.95435632e+03 * pow2(
						temp) - 2.44427511e+02 * temp + 5.74410691e+00;
		} else if(temp > 0.008) {
			// pow2(log10(temp)) approximation between temp = 0.008 and
			// temp = 0.019 with less than 0.02 % of error.
			temp2 = 2.08516955e+07 * pow4(temp) - 1.46552875e+06 * pow3(temp) + 4.20051994e+04 * pow2(
						temp) - 6.58067557e+02 * temp + 7.63733377e+00;
		} else if(temp > 0.00329) {
			// pow2(log10(temp)) approximation between temp = 0.00329 and
			// temp = 0.008 with less than 0.02 % of error.
			temp2 = 7.84991530e+08 * pow4(temp) - 2.30952512e+07 * pow3(temp) + 2.77321091e+05 * pow2(
						temp) - 1.83296385e+03 * temp + 9.92239603e+00;
		} else if(temp > 0.0013) {
			// pow2(log10(temp)) approximation between temp = 0.0013 and
			// temp = 0.00329 with less than 0.02 % of error.
			temp2 = 3.29050277e+10 * pow4(temp) - 3.93727194e+08 * pow3(temp) + 1.92219159e+06 * pow2(
						temp) - 5.18844300e+03 * temp + 1.25952908e+01;
		} else if(temp > 0.0005) {
			// pow2(log10(temp)) approximation between temp = 0.0005 and
			// temp = 0.0013 with less than 0.02 % of error.
			temp2 = 1.57146781e+12 * pow4(temp) - 7.37699295e+09 * pow3(temp) + 1.41288296e+07 * pow2(
						temp) - 1.50182432e+04 * temp + 1.56956366e+01;
		} else if(temp > 0.00019) {
			// pow2(log10(temp)) approximation between temp = 0.00019 and
			// temp = 0.0005 with less than 0.02 % of error.
			temp2 = 8.09890844e+13 * pow4(temp) - 1.45825030e+11 * pow3(temp) + 1.07169184e+08 * pow2(
						temp) - 4.38671762e+04 * temp + 1.92059658e+01;
		} else if(temp > 0.00007) {
			// pow2(log10(temp)) approximation between temp = 0.00007 and
			// temp = 0.00019 with less than 0.02 % of error.
			temp2 = 4.47104917e+15 * pow4(temp) - 3.03334424e+12 * pow3(temp) + 8.39454836e+08 * pow2(
						temp) - 1.29630353e+05 * temp + 2.31540981e+01;
		} else {
			temp2 = pow2(log10(temp));
		}
		f = 0.0625 / temp2;
	} else if(Re > 1) {
		f = 32. / Re;
	} else {
		f = 32.;
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::TransmisionCalor(double tgas, double diametro, double& q, double hi, double rho, double Tw) {
#ifdef usetry
	try {
#endif

		if(hi != 0) {
			q = 4. * hi * (__units::degCToK(Tw) - tgas) / rho / diametro;
		} else
			q = 0.;
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::TransmisionCalor en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

inline double TTubo::DerLinF(double d1, double d2, double xref) {
	// double dm,ret_val;
#ifdef usetry
	try {
#endif

		return 2. * (d2 - d1) / ((d1 + d2) / 2.0) / xref;

#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::DerLinF en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

inline double TTubo::DerLinFArea(double area1, double area2, double xref) {

#ifdef usetry
	try {
#endif
		return (area2 - area1) / xref;

#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::DerLinFArea en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::ActualizaValoresNuevos(TCondicionContorno **BC) {
#ifdef usetry
	try {
#endif

		double a = 0., v = 0., p = 0.;
		double LandaIzq = 0., BetaIzq = 0., EntropiaIzq = 0.;
		double LandaDer = 0., BetaDer = 0., EntropiaDer = 0.;
		double *YIzq, *YDer;

		YIzq = new double[FNumeroEspecies - FIntEGR];
		YDer = new double[FNumeroEspecies - FIntEGR];

		LandaIzq = BC[FNodoIzq - 1]->GetTuboExtremo(FTuboCCNodoIzq).Landa;
		BetaIzq = BC[FNodoIzq - 1]->GetTuboExtremo(FTuboCCNodoIzq).Beta;
		EntropiaIzq = BC[FNodoIzq - 1]->GetTuboExtremo(FTuboCCNodoIzq).Entropia;

		TransformaContorno(LandaIzq, BetaIzq, EntropiaIzq, a, v, p, 1, FGamma1[0], FGamma3[0], FGamma4[0], FGamma5[0]);
		if(BC[FNodoIzq - 1]->getTipoCC() == nmBranch) {
			if(v < 0.) {
				for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
					YIzq[i] = FFraccionMasicaCC[0][i];
				}
			} else {
				for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
					YIzq[i] = BC[FNodoIzq - 1]->GetFraccionMasicaEspecie(i);
				}
			}
		} else {
			for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
				YIzq[i] = BC[FNodoIzq - 1]->GetFraccionMasicaEspecie(i);
			}
		}

		if(FMod.FormulacionLeyes == nmSinArea)
			Transforma1(v, a, p, FU1, FGamma[0], FGamma1[0], YIzq, 0);
		else if(FMod.FormulacionLeyes == nmConArea)
			Transforma1Area(v, a, p, FU1, FArea[0], FGamma[0], FGamma1[0], YIzq, 0);

		LandaDer = BC[FNodoDer - 1]->GetTuboExtremo(FTuboCCNodoDer).Landa;
		BetaDer = BC[FNodoDer - 1]->GetTuboExtremo(FTuboCCNodoDer).Beta;
		EntropiaDer = BC[FNodoDer - 1]->GetTuboExtremo(FTuboCCNodoDer).Entropia;

		TransformaContorno(LandaDer, BetaDer, EntropiaDer, a, v, p, 1, FGamma1[FNin - 1], FGamma3[FNin - 1], FGamma4[FNin - 1],
						   FGamma5[FNin - 1]);
		if(BC[FNodoDer - 1]->getTipoCC() == nmBranch) {
			if(v > 0.) {
				for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
					YDer[i] = FFraccionMasicaCC[1][i];
				}
			} else {
				for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
					YDer[i] = BC[FNodoDer - 1]->GetFraccionMasicaEspecie(i);
				}
			}
		} else {
			for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
				YDer[i] = BC[FNodoDer - 1]->GetFraccionMasicaEspecie(i);
			}
		}

		if(FMod.FormulacionLeyes == nmSinArea)
			Transforma1(v, a, p, FU1, FGamma[FNin - 1], FGamma1[FNin - 1], YDer, FNin - 1);
		else if(FMod.FormulacionLeyes == nmConArea)
			Transforma1Area(v, a, p, FU1, FArea[FNin - 1], FGamma[FNin - 1], FGamma1[FNin - 1], YDer, FNin - 1);

		if(FMod.FormulacionLeyes == nmSinArea) {
			for(int i = 0; i < FNin; i++) {
				Transforma2(FVelocidad0[i], FAsonido0[i], FPresion0[i], FU1, FGamma[i], FGamma1[i], FFraccionMasicaEspecie[i], i);
				for(int k = 0; k < FNumEcuaciones; k++) {
					FU0[k][i] = FU1[k][i];
				}
				FVelocidadDim[i] = FVelocidad0[i] * __cons::ARef;
				FAsonidoDim[i] = FAsonido0[i] * __cons::ARef;
				FFlowMass[i] = FU0[1][i] * FArea[i];
			}
		} else if(FMod.FormulacionLeyes == nmConArea) {
			for(int i = 0; i < FNin; i++) {
				Transforma2Area(FVelocidad0[i], FAsonido0[i], FPresion0[i], FU1, FArea[i], FGamma[i], FGamma1[i],
								FFraccionMasicaEspecie[i], i);
				for(int k = 0; k < FNumEcuaciones; k++) {
					FU0[k][i] = FU1[k][i];
				}
				FVelocidadDim[i] = FVelocidad0[i] * __cons::ARef;
				FAsonidoDim[i] = FAsonido0[i] * __cons::ARef;
				FFlowMass[i] = FU0[1][i];
				if(FVelocidadDim[i] > FAsonidoDim[i] + 1.0e-10) {
					printf("Supersonic flow in pipe: %d node: %d, Mach = %lf\n", FNumeroTubo, i, FVelocidadDim[i] / FAsonidoDim[i]);
				}

			}
		}

		delete[] YIzq;
		delete[] YDer;
#ifdef usetry
	}

	catch(exception & N) {
		std::cout << "ERROR: TTubo::ValoresDeContorno tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::TransformaContorno(double& L, double& B, double& E, double& a, double& v, double& p, const int& modo,
							   const double& Gamma1, const double& Gamma3, const double& Gamma4, const double& Gamma5) {
#ifdef usetry
	try {
#endif
		if(modo == 0) {
			L = (a + Gamma3 * v);
			B = (a - Gamma3 * v);
			E = a / pow(p, Gamma5);
		} else {
			a = (L + B) / 2.;
			v = (L - B) / Gamma1;
			p = pow(a / E, Gamma4);
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::TransformaContorno en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::ReduccionFlujoSubsonico() {
	double Machx = 0., Machy = 0., Velocidady = 0., Sonidoy = 0.;
#ifdef usetry
	try {
#endif

		for(int i = 0; i < FNin; i++) {
			Machx = FVelocidad0[i] / FAsonido0[i];
			if(-1. >= Machx || Machx > 1.) {
				Machy = Machx / fabs(Machx) * sqrt((pow2(Machx) + 2. / FGamma1[i]) / (FGamma4[i] * pow2(Machx) - 1.));
				Sonidoy = FAsonido0[i] * sqrt((FGamma1[i] / 2. * pow2(Machx) + 1.) / (FGamma1[i] / 2. * pow2(Machy) + 1.));

				Velocidady = Sonidoy * Machy;
				FAsonido0[i] = Sonidoy;
				FVelocidad0[i] = Velocidady;
			}
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::ReduccionFlujoSubsonico en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::ReduccionFlujoSubsonicoFCT() {
	double Machx = 0., Machy = 0., Velocidady = 0., Sonidoy = 0.;
	double velocidad = 0., asonido = 0., presion = 0.;
#ifdef usetry
	try {
#endif

		for(int i = 1; i < FNin - 1; i++) {
			velocidad = FU1[1][i] / FU1[0][i] / __cons::ARef;
			presion = __units::PaToBar((FU1[2][i] - FU1[1][i] * velocidad * __cons::ARef / 2.0) * FGamma1[i] / FArea[i]);
			asonido = sqrt(FGamma[i] * __units::BarToPa(presion) * FArea[i] / FU1[0][i] / __cons::ARef2);
			Machx = velocidad / asonido;
			if(-1. >= Machx || Machx > 1.) {
				Machy = Machx / fabs(Machx) * sqrt((pow2(Machx) + 2. / FGamma1[i]) / (FGamma4[i] * pow2(Machx) - 1.));
				Sonidoy = asonido * sqrt((FGamma1[i] / 2. * pow2(Machx) + 1.) / (FGamma1[i] / 2. * pow2(Machy) + 1.));

				Velocidady = Sonidoy * Machy;
				asonido = Sonidoy;
				velocidad = Velocidady;
				FU1[0][i] = FGamma[i] * __units::BarToPa(presion) / pow2(asonido * __cons::ARef) * FArea[i];
				FU1[1][i] = FU1[0][i] * velocidad * __cons::ARef;
				FU1[2][i] = FArea[i] * __units::BarToPa(presion) / FGamma1[i] + FU1[1][i] * velocidad * __cons::ARef / 2.0;

			}
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::ReduccionFlujoSubsonicoFCT en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::ReadAverageResultsTubo(const char *FileWAM, fpos_t &filepos, bool HayMotor) {
	int NumVars = 0, TipoVar = 0;
#ifdef usetry
	try {
#endif

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &FNumResMedios);
		ResultadosMedios = new stResMediosTubo[FNumResMedios];
		FTiempoMedSUM = 0.;
		FControlResMed = 1;

		for(int i = 0; i < FNumResMedios; i++) {
			ResultadosMedios[i].TemperaturaGas = false;
			ResultadosMedios[i].TemperaturaGasSUM = 0.;
			ResultadosMedios[i].TemperaturaGasMED = 0;
			ResultadosMedios[i].Pressure = false;
			ResultadosMedios[i].PresionSUM = 0.;
			ResultadosMedios[i].PresionMED = 0.;
			ResultadosMedios[i].Velocity = false;
			ResultadosMedios[i].VelocidadSUM = 0.;
			ResultadosMedios[i].VelocidadMED = 0.;
			ResultadosMedios[i].Massflow = false;
			ResultadosMedios[i].GastoSUM = 0.;
			ResultadosMedios[i].GastoMED = 0.;
			ResultadosMedios[i].TemperaturaInternaPared = false;
			ResultadosMedios[i].TemperaturaInternaParedSUM = 0.;
			ResultadosMedios[i].TemperaturaInternaParedMED = 0.;
			ResultadosMedios[i].TemperaturaIntermediaPared = false;
			ResultadosMedios[i].TemperaturaIntermediaParedSUM = 0.;
			ResultadosMedios[i].TemperaturaIntermediaParedMED = 0.;
			ResultadosMedios[i].TemperaturaExternaPared = false;
			ResultadosMedios[i].TemperaturaExternaParedSUM = 0.;
			ResultadosMedios[i].TemperaturaExternaParedMED = 0.;
			ResultadosMedios[i].NITmedio = false;
			ResultadosMedios[i].NITmedioSUM = 0;
			ResultadosMedios[i].NITmedioMED = 0;
			ResultadosMedios[i].CoefPelInterior = false;
			ResultadosMedios[i].CoefPelInteriorSUM = 0.;
			ResultadosMedios[i].CoefPelInteriorMED = 0.;
			ResultadosMedios[i].FraccionMasicaEspecies = false;
			ResultadosMedios[i].FraccionSUM = new double[FNumeroEspecies - FIntEGR];
			ResultadosMedios[i].FraccionMED = new double[FNumeroEspecies - FIntEGR];
			for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
				ResultadosMedios[i].FraccionSUM[j] = 0.;
				ResultadosMedios[i].FraccionMED[j] = 0.;
			}
			ResultadosMedios[i].PonderacionSUM = 0.;
			ResultadosMedios[i].GastoPonderacionSUM = 0.;

			fscanf(fich, "%lf %d ", &ResultadosMedios[i].Distancia, &NumVars);

			for(int j = 0; j < NumVars; j++) {
				fscanf(fich, "%d ", &TipoVar);
				switch(TipoVar) {
				case 0:
					ResultadosMedios[i].TemperaturaGas = true;
					break;
				case 1:
					ResultadosMedios[i].Pressure = true;
					break;
				case 2:
					ResultadosMedios[i].Velocity = true;
					break;
				case 3:
					ResultadosMedios[i].Massflow = true;
					break;
				case 4:
					ResultadosMedios[i].TemperaturaInternaPared = true;
					break;
				case 5:
					ResultadosMedios[i].TemperaturaIntermediaPared = true;
					break;
				case 6:
					ResultadosMedios[i].TemperaturaExternaPared = true;
					break;
				case 7:
					ResultadosMedios[i].NITmedio = true;
					if(!HayMotor) {
						std::cout << "ERROR: No puedes pedir el NIT como resultado si no hay motor" << std::endl;
						throw Exception(" ");
					}
					break;
				case 8:
					ResultadosMedios[i].CoefPelInterior = true;
					break;
				case 9:
					ResultadosMedios[i].FraccionMasicaEspecies = true;
					break;
				default:
					std::cout << "WARNING: El tipo de variable seleccionada para la salida de resultados medios no es valida" << std::endl;
				};

			}

		}

		fgetpos(fich, &filepos);
		fclose(fich);
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::ReadAverageResults en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::HeaderAverageResults(stringstream& medoutput, stEspecies *DatosEspecies) const {
#ifdef usetry
	try {
#endif

		std::string Label;
		std::ostringstream TextDist;
		TextDist.precision(8);

		for(int i = 0; i < FNumResMedios; i++) {
			TextDist << ResultadosMedios[i].Distancia;

			if(ResultadosMedios[i].TemperaturaGas) {
				Label = "\t" + PutLabel(303) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							910);
				medoutput << Label.c_str();
			}
			if(ResultadosMedios[i].Pressure) {
				Label = "\t" + PutLabel(301) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							908);
				medoutput << Label.c_str();
			}
			if(ResultadosMedios[i].Velocity) {
				Label = "\t" + PutLabel(302) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							909);
				medoutput << Label.c_str();
			}
			if(ResultadosMedios[i].Massflow) {
				Label = "\t" + PutLabel(304) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							904);
				medoutput << Label.c_str();
			}
			if(ResultadosMedios[i].TemperaturaInternaPared) {
				Label = "\t" + PutLabel(310) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							910);
				medoutput << Label.c_str();
			}
			if(ResultadosMedios[i].TemperaturaIntermediaPared) {
				Label = "\t" + PutLabel(311) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							910);
				medoutput << Label.c_str();
			}
			if(ResultadosMedios[i].TemperaturaExternaPared) {
				Label = "\t" + PutLabel(312) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							910);
				medoutput << Label.c_str();
			}
			if(ResultadosMedios[i].NITmedio) {
				Label = "\t" + PutLabel(309) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							903);
				medoutput << Label.c_str();
			}
			if(ResultadosMedios[i].CoefPelInterior) {
				Label = "\t" + PutLabel(313) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							911);
				medoutput << Label.c_str();
			}
			if(ResultadosMedios[i].FraccionMasicaEspecies) {
				for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
					Label = "\t" + PutLabel(314) + DatosEspecies[j].Nombre + PutLabel(318) + std::to_string(FNumeroTubo) + PutLabel(
								316) + TextDist.str() + PutLabel(317) + PutLabel(901);
					medoutput << Label.c_str();
				}
			}
		}

#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::HeaderAverageResults en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::ImprimeResultadosMedios(stringstream& medoutput) const {
#ifdef usetry
	try {
#endif

		// FILE *fich=fopen(FileSALIDA,"a");

		for(int i = 0; i < FNumResMedios; i++) {
			if(ResultadosMedios[i].TemperaturaGas)
				medoutput << "\t" << __units::KTodegC(ResultadosMedios[i].TemperaturaGasMED);
			if(ResultadosMedios[i].Pressure)
				medoutput << "\t" << ResultadosMedios[i].PresionMED;
			if(ResultadosMedios[i].Velocity)
				medoutput << "\t" << ResultadosMedios[i].VelocidadMED;
			if(ResultadosMedios[i].Massflow)
				medoutput << "\t" << ResultadosMedios[i].GastoMED;
			if(ResultadosMedios[i].TemperaturaInternaPared)
				medoutput << "\t" << ResultadosMedios[i].TemperaturaInternaParedMED;
			if(ResultadosMedios[i].TemperaturaIntermediaPared)
				medoutput << "\t" << ResultadosMedios[i].TemperaturaIntermediaParedMED;
			if(ResultadosMedios[i].TemperaturaExternaPared)
				medoutput << "\t" << ResultadosMedios[i].TemperaturaExternaParedMED;
			if(ResultadosMedios[i].NITmedio)
				medoutput << "\t" << ResultadosMedios[i].NITmedioMED;
			if(ResultadosMedios[i].CoefPelInterior)
				medoutput << "\t" << ResultadosMedios[i].CoefPelInteriorMED;
			if(ResultadosMedios[i].FraccionMasicaEspecies) {
				for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
					medoutput << "\t" << ResultadosMedios[i].FraccionMED[j];
				}
			}
		}

#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::ResultadosMedios en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void TTubo::ReadInstantaneousResultsTubo(const char *FileWAM, fpos_t &filepos, bool HayMotor) {
	int NumVars = 0, TipoVar = 0;
#ifdef usetry
	try {
#endif

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &FNumResInstant); // Puntos del tubo en los que se piden resultados instantaneos
		ResultInstantaneos = new stResInstantTubo[FNumResInstant];

		for(int i = 0; i < FNumResInstant; i++) {
			ResultInstantaneos[i].Pressure = false;
			ResultInstantaneos[i].PresionINS = 0.;
			ResultInstantaneos[i].Velocity = false;
			ResultInstantaneos[i].VelocidadINS = 0.;
			ResultInstantaneos[i].TemperaturaGas = false;
			ResultInstantaneos[i].TemperaturaGasINS = 0.;
			ResultInstantaneos[i].FlujoMasico = false;
			ResultInstantaneos[i].FlujoMasicoINS = 0.;
			ResultInstantaneos[i].VelocidadDerecha = false;
			ResultInstantaneos[i].VelocidadDerechaINS = 0.;
			ResultInstantaneos[i].VelocidadIzquierda = false;
			ResultInstantaneos[i].VelocidadIzquierdaINS = 0.;
			ResultInstantaneos[i].PresionDerecha = false;
			ResultInstantaneos[i].PresionDerechaINS = 0.;
			ResultInstantaneos[i].PresionIzquierda = false;
			ResultInstantaneos[i].PresionIzquierdaINS = 0.;
			ResultInstantaneos[i].NIT = false;
			ResultInstantaneos[i].NITINS = 0.;
			ResultInstantaneos[i].TemperaturaInternaPared = false;
			ResultInstantaneos[i].TemperaturaInternaParedINS = 0.;
			ResultInstantaneos[i].TemperaturaIntermediaPared = false;
			ResultInstantaneos[i].TemperaturaIntermediaParedINS = 0.;
			ResultInstantaneos[i].TemperaturaExternaPared = false;
			ResultInstantaneos[i].TemperaturaExternaParedINS = 0.;
			ResultInstantaneos[i].CoefPelInterior = false;
			ResultInstantaneos[i].CoefPelInteriorINS = 0.;
			ResultInstantaneos[i].FraccionMasicaEspecies = false;
			ResultInstantaneos[i].FraccionINS = new double[FNumeroEspecies - FIntEGR];
			ResultInstantaneos[i].Gamma = false;
			for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
				ResultInstantaneos[i].FraccionINS[j] = 0.;
			}

			fscanf(fich, "%lf %d ", &ResultInstantaneos[i].Distancia, &NumVars);

			for(int j = 0; j < NumVars; j++) {
				fscanf(fich, "%d ", &TipoVar);
				switch(TipoVar) {
				case 0:
					ResultInstantaneos[i].Pressure = true;
					break;
				case 1:
					ResultInstantaneos[i].Velocity = true;
					break;
				case 2:
					ResultInstantaneos[i].TemperaturaGas = true;
					break;
				case 3:
					ResultInstantaneos[i].FlujoMasico = true;
					break;
				case 4:
					ResultInstantaneos[i].VelocidadDerecha = true;
					break;
				case 5:
					ResultInstantaneos[i].VelocidadIzquierda = true;
					break;
				case 6:
					ResultInstantaneos[i].PresionDerecha = true;
					break;
				case 7:
					ResultInstantaneos[i].PresionIzquierda = true;
					break;
				case 8:
					ResultInstantaneos[i].NIT = true;
					if(!HayMotor) {
						std::cout << "ERROR: No puedes pedir el NIT como resultado si no hay motor" << std::endl;
						throw Exception(" ");
					}
					break;
				case 9:
					ResultInstantaneos[i].TemperaturaInternaPared = true;
					break;
				case 10:
					ResultInstantaneos[i].TemperaturaIntermediaPared = true;
					break;
				case 11:
					ResultInstantaneos[i].TemperaturaExternaPared = true;
					break;
				case 12:
					ResultInstantaneos[i].CoefPelInterior = true;
					break;
				case 13:
					ResultInstantaneos[i].FraccionMasicaEspecies = true;
					break;
				case 14:
					ResultInstantaneos[i].Gamma = true;
					break;
				default:
					std::cout << "WARNING: El tipo de variable seleccionada para la salida de resultados instantaneos no es valida" <<
							  std::endl;
				}
			}
		}

		fgetpos(fich, &filepos);
		fclose(fich);
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::ReadInstantaneousResults en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::HeaderInstantaneousResults(stringstream& insoutput, stEspecies *DatosEspecies) const {
#ifdef usetry
	try {
#endif

		std::string Label;
		std::ostringstream TextDist;
		TextDist.precision(8);

		for(int i = 0; i < FNumResInstant; i++) {
			TextDist << ResultInstantaneos[i].Distancia;
			if(ResultInstantaneos[i].Pressure) {
				Label = "\t" + PutLabel(301) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							908);
				insoutput << Label.c_str();
			}
			if(ResultInstantaneos[i].Velocity) {
				Label = "\t" + PutLabel(302) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							909);
				insoutput << Label.c_str();
			}
			if(ResultInstantaneos[i].TemperaturaGas) {
				Label = "\t" + PutLabel(303) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							910);
				insoutput << Label.c_str();
			}
			if(ResultInstantaneos[i].FlujoMasico) {
				Label = "\t" + PutLabel(304) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							904);
				insoutput << Label.c_str();
			}
			if(ResultInstantaneos[i].VelocidadDerecha) {
				Label = "\t" + PutLabel(305) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							909);
				insoutput << Label.c_str();
			}
			if(ResultInstantaneos[i].VelocidadIzquierda) {
				Label = "\t" + PutLabel(306) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							909);
				insoutput << Label.c_str();
			}
			if(ResultInstantaneos[i].PresionDerecha) {
				Label = "\t" + PutLabel(307) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							908);
				insoutput << Label.c_str();
			}
			if(ResultInstantaneos[i].PresionIzquierda) {
				Label = "\t" + PutLabel(308) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							908);
				insoutput << Label.c_str();
			}
			if(ResultInstantaneos[i].NIT) {
				Label = "\t" + PutLabel(309) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							903);
				insoutput << Label.c_str();
			}
			if(ResultInstantaneos[i].TemperaturaInternaPared) {
				Label = "\t" + PutLabel(310) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							910);
				insoutput << Label.c_str();
			}
			if(ResultInstantaneos[i].TemperaturaIntermediaPared) {
				Label = "\t" + PutLabel(311) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							910);
				insoutput << Label.c_str();
			}
			if(ResultInstantaneos[i].TemperaturaExternaPared) {
				Label = "\t" + PutLabel(312) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							910);
				insoutput << Label.c_str();
			}
			if(ResultInstantaneos[i].CoefPelInterior) {
				Label = "\t" + PutLabel(313) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							911);
				insoutput << Label.c_str();
			}
			if(ResultInstantaneos[i].FraccionMasicaEspecies) {
				for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
					Label = "\t" + PutLabel(314) + DatosEspecies[j].Nombre + PutLabel(318) + std::to_string(FNumeroTubo) + PutLabel(
								316) + TextDist.str() + PutLabel(317) + PutLabel(901);
					insoutput << Label.c_str();
				}
			}
			if(ResultInstantaneos[i].Gamma) {
				Label = "\t" + PutLabel(315) + std::to_string(FNumeroTubo) + PutLabel(316) + TextDist.str() + PutLabel(317) + PutLabel(
							901);
				insoutput << Label.c_str();
			}
		}

#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::HeaderInstantaneousResults en el tubo nº: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::ImprimeResultadosInstantaneos(stringstream& insoutput) const {
#ifdef usetry
	try {
#endif

		for(int i = 0; i < FNumResInstant; i++) {
			if(ResultInstantaneos[i].Pressure)
				insoutput << "\t" << ResultInstantaneos[i].PresionINS;
			if(ResultInstantaneos[i].Velocity)
				insoutput << "\t" << ResultInstantaneos[i].VelocidadINS;
			if(ResultInstantaneos[i].TemperaturaGas)
				insoutput << "\t" << ResultInstantaneos[i].TemperaturaGasINS;
			if(ResultInstantaneos[i].FlujoMasico)
				insoutput << "\t" << ResultInstantaneos[i].FlujoMasicoINS;
			if(ResultInstantaneos[i].VelocidadDerecha)
				insoutput << "\t" << ResultInstantaneos[i].VelocidadDerechaINS;
			if(ResultInstantaneos[i].VelocidadIzquierda)
				insoutput << "\t" << ResultInstantaneos[i].VelocidadIzquierdaINS;
			if(ResultInstantaneos[i].PresionDerecha)
				insoutput << "\t" << ResultInstantaneos[i].PresionDerechaINS;
			if(ResultInstantaneos[i].PresionIzquierda)
				insoutput << "\t" << ResultInstantaneos[i].PresionIzquierdaINS;
			if(ResultInstantaneos[i].NIT)
				insoutput << "\t" << ResultInstantaneos[i].NITINS;
			if(ResultInstantaneos[i].TemperaturaInternaPared)
				insoutput << "\t" << ResultInstantaneos[i].TemperaturaInternaParedINS;
			if(ResultInstantaneos[i].TemperaturaIntermediaPared)
				insoutput << "\t" << ResultInstantaneos[i].TemperaturaIntermediaParedINS;
			if(ResultInstantaneos[i].TemperaturaExternaPared)
				insoutput << "\t" << ResultInstantaneos[i].TemperaturaExternaParedINS;
			if(ResultInstantaneos[i].CoefPelInterior)
				insoutput << "\t" << ResultInstantaneos[i].CoefPelInteriorINS;
			if(ResultInstantaneos[i].FraccionMasicaEspecies) {
				for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
					insoutput << "\t" << ResultInstantaneos[i].FraccionINS[j];
				}
			}
			if(ResultInstantaneos[i].Gamma)
				insoutput << "\t" << ResultInstantaneos[i].GammaINS;

		}

#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::ResultadosInstantaneos en el tubo nº: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculaResultadosMedios(double Theta) {
	double dist = 0., Vble = 0., d = 0., Rmezcla = 0., Gamma = 0., GastoPonderacion = 0.;
	int n1 = 0, n2 = 0;
#ifdef usetry
	try {
#endif

		FTiempoMedSUM += FDeltaTime;

		for(int i = 0; i < FNumResMedios; i++) {
			dist = ResultadosMedios[i].Distancia / FXref;
			n1 = (int) floor(dist);
			if(n1 >= FNin - 1) {
				if(ResultadosMedios[i].TemperaturaGas || ResultadosMedios[i].FraccionMasicaEspecies) {
					GastoPonderacion = fabs(FFlowMass[FNin - 1]);
					ResultadosMedios[i].PonderacionSUM += FFlowMass[FNin - 1] * FDeltaTime;
					ResultadosMedios[i].GastoPonderacionSUM += GastoPonderacion;
				}
				if(ResultadosMedios[i].TemperaturaGas)
					ResultadosMedios[i].TemperaturaGasSUM += (FTemperature[FNin - 1]) * GastoPonderacion;
				if(ResultadosMedios[i].Pressure)
					ResultadosMedios[i].PresionSUM += FPresion0[FNin - 1] * FDeltaTime;
				if(ResultadosMedios[i].Velocity)
					ResultadosMedios[i].VelocidadSUM += FVelocidadDim[FNin - 1] * FDeltaTime;
				if(ResultadosMedios[i].Massflow) {
					ResultadosMedios[i].GastoSUM += FFlowMass[i] * FDeltaTime;
				}
				if(ResultadosMedios[i].TemperaturaInternaPared)
					ResultadosMedios[i].TemperaturaInternaParedSUM += FTPTubo[0][FNin - 1] * FDeltaTime;
				if(ResultadosMedios[i].TemperaturaIntermediaPared)
					ResultadosMedios[i].TemperaturaIntermediaParedSUM += FTPTubo[1][FNin - 1] * FDeltaTime;
				if(ResultadosMedios[i].TemperaturaExternaPared)
					ResultadosMedios[i].TemperaturaExternaParedSUM += FTPTubo[2][FNin - 1] * FDeltaTime;
				if(ResultadosMedios[i].NITmedio) {
					double nit = CalculaNIT(FAsonido0[FNin - 1], FVelocidad0[FNin - 1], FPresion0[FNin - 1], FDiametroTubo[FNin - 1],
											FGamma[FNin - 1], FRMezcla[FNin - 1]);
					ResultadosMedios[i].NITmedioSUM += nit * FDeltaTime;
				}
				if(ResultadosMedios[i].CoefPelInterior) {
					ResultadosMedios[i].CoefPelInteriorSUM += Fhi[FNin - 1] * FDeltaTime;
				}
				if(ResultadosMedios[i].FraccionMasicaEspecies) {
					for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
						ResultadosMedios[i].FraccionSUM[j] += FFraccionMasicaEspecie[FNin - 1][j] * GastoPonderacion * FDeltaTime;
					}
				}
			} else {
				n2 = n1 + 1;
				d = dist - (double) n1;

				if(ResultadosMedios[i].TemperaturaGas || ResultadosMedios[i].FraccionMasicaEspecies) {
					GastoPonderacion = fabs(Interpola(FFlowMass[n1], FFlowMass[n2], 1., d));
					ResultadosMedios[i].GastoPonderacionSUM += GastoPonderacion;
					ResultadosMedios[i].PonderacionSUM += GastoPonderacion * FDeltaTime;
				}

				if(ResultadosMedios[i].TemperaturaGas)
					Vble = Interpola(FTemperature[n1], FTemperature[n2], 1., d);
				ResultadosMedios[i].TemperaturaGasSUM += Vble * GastoPonderacion;
				if(ResultadosMedios[i].Pressure) {
					Vble = Interpola(FPresion0[n1], FPresion0[n2], 1., d);
					ResultadosMedios[i].PresionSUM += Vble * FDeltaTime;
				}
				if(ResultadosMedios[i].Velocity) {
					Vble = Interpola(FVelocidad0[n1], FVelocidad0[n2], 1., d);
					ResultadosMedios[i].VelocidadSUM += Vble * __cons::ARef * FDeltaTime;
				}
				if(ResultadosMedios[i].Massflow) {
					Vble = Interpola(FFlowMass[n1], FFlowMass[n2], 1., d);
					ResultadosMedios[i].GastoSUM += Vble * FDeltaTime;
				}
				if(ResultadosMedios[i].TemperaturaInternaPared) {
					Vble = Interpola(FTPTubo[0][n1], FTPTubo[0][n2], 2., d);
					ResultadosMedios[i].TemperaturaInternaParedSUM += Vble * FDeltaTime;
				}
				if(ResultadosMedios[i].TemperaturaIntermediaPared) {
					Vble = Interpola(FTPTubo[1][n1], FTPTubo[1][n2], 2., d);
					ResultadosMedios[i].TemperaturaIntermediaParedSUM += Vble * FDeltaTime;
				}
				if(ResultadosMedios[i].TemperaturaExternaPared) {
					Vble = Interpola(FTPTubo[2][n1], FTPTubo[2][n2], 2., d);
					ResultadosMedios[i].TemperaturaExternaParedSUM += Vble * FDeltaTime;
				}
				if(ResultadosMedios[i].NITmedio) {
					double a = Interpola(FAsonido0[n1], FAsonido0[n2], 1., d);
					double v = Interpola(FVelocidad0[n1], FVelocidad0[n2], 1., d);
					double p = Interpola(FPresion0[n1], FPresion0[n2], 1., d);
					double diam = Interpola(FDiametroTubo[n1], FDiametroTubo[n2], 1., d);
					Gamma = Interpola(FGamma[n1], FGamma[n2], 1., d);
					Rmezcla = Interpola(FRMezcla[n1], FRMezcla[n2], 1., d);
					double nit = CalculaNIT(a, v, p, diam, Gamma, Rmezcla);
					ResultadosMedios[i].NITmedioSUM += nit * FDeltaTime;
				}
				if(ResultadosMedios[i].CoefPelInterior) {
					Vble = Interpola(Fhi[n1], Fhi[n2], 1., d);
					ResultadosMedios[i].CoefPelInteriorSUM += Vble * FDeltaTime;
				}
				if(ResultadosMedios[i].FraccionMasicaEspecies) {
					for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
						Vble = Interpola(FFraccionMasicaEspecie[n1][j], FFraccionMasicaEspecie[n2][j], 1., d);
						ResultadosMedios[i].FraccionSUM[j] += Vble * GastoPonderacion * FDeltaTime;
					}
				}

			}
		}

		if(Theta > FControlResMed * FAnguloTotalCiclo) {

			for(int i = 0; i < FNumResMedios; i++) {
				if(ResultadosMedios[i].Pressure) {
					ResultadosMedios[i].PresionMED = ResultadosMedios[i].PresionSUM / FTiempoMedSUM;
					ResultadosMedios[i].PresionSUM = 0.;
				}
				if(ResultadosMedios[i].TemperaturaGas) {
					if(ResultadosMedios[i].GastoPonderacionSUM != 0)
						ResultadosMedios[i].TemperaturaGasMED = ResultadosMedios[i].TemperaturaGasSUM / ResultadosMedios[i].GastoPonderacionSUM;
					else
						ResultadosMedios[i].TemperaturaGasMED = 0;
					ResultadosMedios[i].TemperaturaGasSUM = 0.;
					ResultadosMedios[i].GastoPonderacionSUM = 0.;
				}
				if(ResultadosMedios[i].Velocity) {
					ResultadosMedios[i].VelocidadMED = ResultadosMedios[i].VelocidadSUM / FTiempoMedSUM;
					ResultadosMedios[i].VelocidadSUM = 0.;
				}
				if(ResultadosMedios[i].Massflow) {
					ResultadosMedios[i].GastoMED = ResultadosMedios[i].GastoSUM / FTiempoMedSUM;
					ResultadosMedios[i].GastoSUM = 0.;
				}
				if(ResultadosMedios[i].TemperaturaInternaPared) {
					ResultadosMedios[i].TemperaturaInternaParedMED = ResultadosMedios[i].TemperaturaInternaParedSUM / FTiempoMedSUM;
					ResultadosMedios[i].TemperaturaInternaParedSUM = 0.;
				}
				if(ResultadosMedios[i].TemperaturaIntermediaPared) {
					ResultadosMedios[i].TemperaturaIntermediaParedMED = ResultadosMedios[i].TemperaturaIntermediaParedSUM / FTiempoMedSUM;
					ResultadosMedios[i].TemperaturaIntermediaParedSUM = 0.;
				}
				if(ResultadosMedios[i].TemperaturaExternaPared) {
					ResultadosMedios[i].TemperaturaExternaParedMED = ResultadosMedios[i].TemperaturaExternaParedSUM / FTiempoMedSUM;
					ResultadosMedios[i].TemperaturaExternaParedSUM = 0.;
				}
				if(ResultadosMedios[i].NITmedio) {
					ResultadosMedios[i].NITmedioMED = ResultadosMedios[i].NITmedioSUM / FTiempoMedSUM;
					ResultadosMedios[i].NITmedioSUM = 0.;
				}
				if(ResultadosMedios[i].CoefPelInterior) {
					ResultadosMedios[i].CoefPelInteriorMED = ResultadosMedios[i].CoefPelInteriorSUM / FTiempoMedSUM;
					ResultadosMedios[i].CoefPelInteriorSUM = 0.;
				}
				if(ResultadosMedios[i].FraccionMasicaEspecies) {
					for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
						if(DoubEqZero(ResultadosMedios[i].PonderacionSUM)) {
							ResultadosMedios[i].FraccionMED[j] = 0.;
						} else {
							ResultadosMedios[i].FraccionMED[j] = ResultadosMedios[i].FraccionSUM[j] / ResultadosMedios[i].PonderacionSUM;
						}
						ResultadosMedios[i].FraccionSUM[j] = 0.;
					}
					ResultadosMedios[i].PonderacionSUM = 0.;
				}
			}
			FTiempoMedSUM = 0.;
			FControlResMed = FControlResMed + 1.;
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaResultadosMedios en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculaResultadosInstantaneos() {
	double dist = 0., d = 0., ason = 0., vel = 0., Aa = 0., ason1 = 0., vel1 = 0., Aa1 = 0., ason2 = 0., vel2 = 0.,
		   Aa2 = 0.;
	int n1 = 0, n2 = 0;
#ifdef usetry
	try {
#endif

		if(FNumResInstant != 0) {
			for(int i = 0; i < FNumResInstant; i++) {
				dist = ResultInstantaneos[i].Distancia / FXref;
				n1 = (int) floor(dist);
				if(n1 >= FNin - 1) {
					if(ResultInstantaneos[i].Pressure)
						ResultInstantaneos[i].PresionINS = FPresion0[FNin - 1];
					if(ResultInstantaneos[i].Velocity)
						ResultInstantaneos[i].VelocidadINS = FVelocidad0[FNin - 1] * __cons::ARef;
					if(ResultInstantaneos[i].TemperaturaGas) {
						double temp = __units::KTodegC(pow2(FAsonido0[FNin - 1] * __cons::ARef) / (FGamma[FNin - 1] * FRMezcla[FNin - 1]));
						ResultInstantaneos[i].TemperaturaGasINS = temp;
					}
					if(ResultInstantaneos[i].FlujoMasico) {
						ResultInstantaneos[i].FlujoMasicoINS = FFlowMass[FNin - 1];
					}
					if(ResultInstantaneos[i].VelocidadDerecha || ResultInstantaneos[i].VelocidadIzquierda
					   || ResultInstantaneos[i].PresionDerecha || ResultInstantaneos[i].PresionIzquierda) {
						ason = FAsonidoDim[FNin - 1];
						vel = FGamma1[FNin - 1] / 2 * FVelocidadDim[FNin - 1];
						Aa = ason / pow(FPresion0[FNin - 1], FGamma5[FNin - 1]);
					}
					if(ResultInstantaneos[i].VelocidadDerecha) {
						double VelDer = ((ason + vel) - Aa) / FGamma1[FNin - 1];
						ResultInstantaneos[i].VelocidadDerechaINS = VelDer;
					}
					if(ResultInstantaneos[i].VelocidadIzquierda) {
						double VelIzq = -((ason - vel) - Aa) / FGamma1[FNin - 1];
						ResultInstantaneos[i].VelocidadIzquierdaINS = VelIzq;
					}
					if(ResultInstantaneos[i].PresionDerecha) {
						double PreDer = pow(((ason + vel) / Aa + 1) / 2., FGamma4[FNin - 1]);
						ResultInstantaneos[i].PresionDerechaINS = PreDer;
					}
					if(ResultInstantaneos[i].PresionIzquierda) {
						double PreIzq = pow(((ason - vel) / Aa + 1) / 2., FGamma4[FNin - 1]);
						ResultInstantaneos[i].PresionIzquierdaINS = PreIzq;
					}
					if(ResultInstantaneos[i].NIT) {
						double nit = CalculaNIT(FAsonido0[FNin - 1], FVelocidad0[FNin - 1], FPresion0[FNin - 1], FDiametroTubo[FNin - 1],
												FGamma[FNin - 1], FRMezcla[FNin - 1]);
						ResultInstantaneos[i].NITINS = nit;
					}
					if(ResultInstantaneos[i].TemperaturaInternaPared)
						ResultInstantaneos[i].TemperaturaInternaParedINS = FTPTubo[0][FNin - 1];
					if(ResultInstantaneos[i].TemperaturaIntermediaPared)
						ResultInstantaneos[i].TemperaturaIntermediaParedINS = FTPTubo[1][FNin - 1];
					if(ResultInstantaneos[i].TemperaturaExternaPared)
						ResultInstantaneos[i].TemperaturaExternaParedINS = FTPTubo[2][FNin - 1];
					if(ResultInstantaneos[i].CoefPelInterior)
						ResultInstantaneos[i].CoefPelInteriorINS = Fhi[FNin - 1];
					if(ResultInstantaneos[i].FraccionMasicaEspecies) {
						for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
							ResultInstantaneos[i].FraccionINS[j] = FFraccionMasicaEspecie[FNin - 1][j];
						}
					}
					if(ResultInstantaneos[i].Gamma)
						ResultInstantaneos[i].GammaINS = FGamma[FNin - 1];
				} else {
					n2 = n1 + 1;
					d = dist - (double) n1;
					if(ResultInstantaneos[i].Pressure) {
						double pres = Interpola(FPresion0[n1], FPresion0[n2], 1., d);
						ResultInstantaneos[i].PresionINS = pres;
					}
					if(ResultInstantaneos[i].Velocity) {
						double vel = Interpola(FVelocidadDim[n1], FVelocidadDim[n2], 1., d);
						ResultInstantaneos[i].VelocidadINS = vel;
					}
					if(ResultInstantaneos[i].TemperaturaGas) {
						double temp1 = FTemperature[n1];
						double temp2 = FTemperature[n2];
						double temp = Interpola(temp1, temp2, 1., d);
						ResultInstantaneos[i].TemperaturaGasINS = __units::KTodegC(temp);
					}
					if(ResultInstantaneos[i].FlujoMasico) {
						// double area1=pow(FDiametroTubo[n1],2)*__cons::Pi_4;
						// double dens1=FGamma[n1]*FPresion0[n1]*1e5/pow(FAsonido0[n1]*__cons::ARef,2.);
						double gto1 = FFlowMass[n1];
						// double area2=pow(FDiametroTubo[n2],2)*__cons::Pi_4;
						// double dens2=FGamma[n2]*FPresion0[n2]*1e5/pow(FAsonido0[n2]*__cons::ARef,2.);
						double gto2 = FFlowMass[n2];
						double gto = Interpola(gto1, gto2, 1., d);
						ResultInstantaneos[i].FlujoMasicoINS = gto;
					}
					if(ResultInstantaneos[i].VelocidadDerecha || ResultInstantaneos[i].VelocidadIzquierda
					   || ResultInstantaneos[i].PresionDerecha || ResultInstantaneos[i].PresionIzquierda) {
						ason1 = FAsonidoDim[n1];
						vel1 = FGamma1[n1] / 2 * FVelocidadDim[n1];
						Aa1 = ason1 / pow(FPresion0[n1], FGamma5[n1]);
						ason2 = FAsonidoDim[n2];
						vel2 = FGamma1[n2] / 2 * FVelocidadDim[n2];
						Aa2 = ason2 / pow(FPresion0[n2], FGamma5[n2]);
					}
					if(ResultInstantaneos[i].VelocidadDerecha) {
						double VelDer1 = ((ason1 + vel1) - Aa1) / FGamma1[n1];
						double VelDer2 = ((ason2 + vel2) - Aa2) / FGamma1[n2];
						double VelDer = Interpola(VelDer1, VelDer2, 1., d);
						ResultInstantaneos[i].VelocidadDerechaINS = VelDer;
					}
					if(ResultInstantaneos[i].VelocidadIzquierda) {
						double VelIzq1 = -((ason1 - vel1) - Aa1) / FGamma1[n1];
						double VelIzq2 = -((ason2 - vel2) - Aa2) / FGamma1[n2];
						double VelIzq = Interpola(VelIzq1, VelIzq2, 1., d);
						ResultInstantaneos[i].VelocidadIzquierdaINS = VelIzq;
					}
					if(ResultInstantaneos[i].PresionDerecha) {
						double PreDer1 = pow(((ason1 + vel1) / Aa1 + 1) / 2., FGamma4[n1]);
						double PreDer2 = pow(((ason2 + vel2) / Aa2 + 1) / 2., FGamma4[n2]);
						double PreDer = Interpola(PreDer1, PreDer2, 1., d);
						ResultInstantaneos[i].PresionDerechaINS = PreDer;
					}
					if(ResultInstantaneos[i].PresionIzquierda) {
						double PreIzq1 = pow(((ason1 - vel1) / Aa1 + 1) / 2., FGamma4[n1]);
						double PreIzq2 = pow(((ason2 - vel2) / Aa2 + 1) / 2., FGamma4[n2]);
						double PreIzq = Interpola(PreIzq1, PreIzq2, 1., d);
						ResultInstantaneos[i].PresionIzquierdaINS = PreIzq;
					}
					if(ResultInstantaneos[i].NIT) {
						double nit1 = CalculaNIT(FAsonido0[n1], FVelocidad0[n1], FPresion0[n1], FDiametroTubo[n1], FGamma[n1], FRMezcla[n1]);
						double nit2 = CalculaNIT(FAsonido0[n2], FVelocidad0[n2], FPresion0[n2], FDiametroTubo[n2], FGamma[n2], FRMezcla[n2]);
						double nit = Interpola(nit1, nit2, 1., d);
						ResultInstantaneos[i].NITINS = nit;
					}
					if(ResultInstantaneos[i].TemperaturaInternaPared) {
						double TP = Interpola(FTPTubo[0][n1], FTPTubo[0][n2], 1., d);
						ResultInstantaneos[i].TemperaturaInternaParedINS = TP;
					}
					if(ResultInstantaneos[i].TemperaturaIntermediaPared) {
						double TP = Interpola(FTPTubo[1][n1], FTPTubo[1][n2], 1., d);
						ResultInstantaneos[i].TemperaturaIntermediaParedINS = TP;
					}
					if(ResultInstantaneos[i].TemperaturaExternaPared) {
						double TP = Interpola(FTPTubo[2][n1], FTPTubo[2][n2], 1., d);
						ResultInstantaneos[i].TemperaturaExternaParedINS = TP;
					}
					if(ResultInstantaneos[i].CoefPelInterior) {
						double hi = Interpola(Fhi[n1], Fhi[n2], 1., d);
						ResultInstantaneos[i].CoefPelInteriorINS = hi;
					}
					if(ResultInstantaneos[i].FraccionMasicaEspecies) {
						for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
							double Fraccion = Interpola(FFraccionMasicaEspecie[n1][j], FFraccionMasicaEspecie[n2][j], 1., d);
							ResultInstantaneos[i].FraccionINS[j] = Fraccion;
						}
					}
					if(ResultInstantaneos[i].Gamma) {
						double gamma = Interpola(FGamma[n1], FGamma[n2], 1., d);
						ResultInstantaneos[i].GammaINS = gamma;
					}
				}
			}

		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaResultadosInstantaneos en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TTubo::CalculaNIT(double a, double v, double p, double d, double Gamma, double R) {
#ifdef usetry
	try {
#endif
		double kp = 0.;
		double tem0 = 0., pre0 = 0., area = 0., gto = 0., nit = 0., V = 0., A = 0., V2 = 0., A2 = 0.;

		kp = Gamma * R / (Gamma - 1);
		V = v * __cons::ARef;
		V2 = V * V;
		A = a * __cons::ARef;
		A2 = A * A;
		tem0 = A2 / (Gamma * R) + V2 / 2. / kp;
		pre0 = __units::BarToPa(p) * pow((1 + V2 / 2. / kp / A2), (kp / 287.));
		area = __geom::Circle_area(d);
		gto = Gamma * __units::BarToPa(p) * area * V / A2;
		nit = gto * kp * tem0 * (1 - pow(pre0 / 100000., (-287. / kp)));
		return nit;
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaNIT en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculaCoeficientePeliculaExterior(TBloqueMotor **Engine, double AmbientPressure,
		double AmbientTemperature) {
#ifdef usetry
	try {
#endif

		double dtem, temed, rhog, viscext, Re, Pr, cond, viscpared, vel, n1, n2, L;

		if(FTipoCalcTempPared != nmTempConstante && FCoefAjusTC != 0) {

			if(FTipoTransCal == nmTuboAdmision || FTipoTransCal == nmTuboEscape) {
				for(int i = 0; i < FNin; i++) {
					dtem = fabs(__units::degCToK(FTPTubo[2][i]) - FTExt);
					temed = (__units::degCToK(FTPTubo[2][i]) + FTExt) / 2.;

					// Calculo del coeficiente de pelicula exterior

					// Calculo de las caracteristicas del refrigerante
					switch(FTipRefrig) {
					case nmAire:
						// Condiciones del aire a temperatura media (temed) y 1m/s
						rhog = __units::BarToPa(AmbientPressure) / __R::Air / temed;
						// Density del aire atmosferico (se considera R=cte=287)
						viscext = 1.4615e-6 * pow150(temed) / (temed + 110.4);
						Pr = 0.7;
						cond = (-8.39061e-09 * temed + 7.05256e-05) * temed + 6.51528e-03;
						Re = rhog * 1. * (FDiametroTubo[i] + 2 * (FEspesorIntPrin + FEspesorPrin + FEspesorExtPrin)) / viscext;
						break;
					case nmAgua:
						// Condiciones de agua saturada a (temed) y 1m/s
						rhog = 980;
						if(temed > 373.) {
							viscext = 0.000282;
							Pr = 1.76;
							cond = 0.6775;
						} else {
							viscext = ((-2.632351E-09 * temed + 2.737629E-06) * temed - 9.530709E-04) * temed + 1.114642E-01;
							Pr = ((-2.022269E-05 * temed + 2.106518E-02) * temed - 7.340298E+00) * temed + 8.581110E+02;
							cond = ((9.496332E-09 * temed - 1.707697E-05) * temed + 9.183462E-03) * temed - 8.626578E-01;
						}
						Re = rhog * 1. * (FDiametroTubo[i] + 2 * (FEspesorIntPrin + FEspesorPrin + FEspesorExtPrin)) / viscext;
						break;
					default:
						std::cout << "WARNING: Tipo de refrigeracion mal definida en el tubo: " << FNumeroTubo << std::endl;
					}
					// Termino de conveccion de Churchill Bernstein
					if((2e4 < Re) && (Re < 4e5)) {
						n1 = 0.5;
						n2 = 1.;
					} else {
						n1 = 0.625;
						n2 = 0.8;
					}
					Fhe[i] = 0.3 + 0.62 * sqrt(Re) * cbrt(Pr) / pow025(1 + pow(0.4 / Pr, 0.666666)) * pow(1 + pow(Re / 282000, n1),
							 n2) * cond / FDiametroTubo[i];

					// Termino de radiacion
					if(dtem != 0.) {
						Fhe[i] = Fhe[i] + __cons::Sigma * FEmisividad * (pow4(__units::degCToK(FTPTubo[2][i])) - pow4(FTExt)) / dtem;
					}
					Fhe[i] = Fhe[i] * FCoefExt;
				}
			} else {
				temed = __units::degCToK(Engine[0]->getTempRefrigerante());
				rhog = 980.;
				viscext = ((-2.632351E-09 * temed + 2.737629E-06) * temed - 9.530709E-04) * temed + 1.114642E-01;
				L = 1.5 * Engine[0]->getGeometria().Diametro;
				// Longitud caracteristica del conducto refrigerante.
				vel = 5.64268e-7 * Engine[0]->getRegimen() / 60. * Engine[0]->getParPotMax() / Engine[0]->getGeometria().NCilin / pow(L,
						2.);
				Re = rhog * fabs(vel) * L / 2.3 / viscext;
				Pr = ((-2.022269E-05 * temed + 2.106518E-02) * temed - 7.340298E+00) * temed + 8.581110E+02;
				cond = ((9.496332E-09 * temed - 1.707697E-05) * temed + 9.183462E-03) * temed - 8.626578E-01;

				for(int i = 0; i < FNin; i++) {
					// Condiciones del agua de refrigerante del motor.
					// Termino de conveccion de Seider-Tate con el termino de Cipolla.

					if(FTPTubo[2][i] > 100.) {
						viscpared = 0.000282;
					} else {
						double Tp2 = __units::degCToK(FTPTubo[2][i]);
						viscpared = ((-2.632351E-09 * Tp2 + 2.737629E-06) * Tp2 - 9.530709E-04) * Tp2 + 1.114642E-01;
					}

					Fhe[i] = 0.027 * (1 + 24.2 / pow(2.3, 0.7) / pow025(Re)) * pow(Re, 0.8) * cbrt(Pr) * pow(viscext / viscpared,
							 0.14) * cond / (L / 2.3);
				}
			}
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaCoeficientePeliculaExterior en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculaResistenciasdePared(TCondicionContorno **BC) {
#ifdef usetry
	try {
#endif
		double Dext, Dint, DIntPrin /* ,Tmed */, Text, Tint, Rcond, Rrad, UnionEspes, UnionConduct, Cap;
		bool EsInterior;

		if(FTipoCalcTempPared != nmTempConstante && FCoefAjusTC != 0 && !FConcentrico) {
			for(int i = 0; i < FNin; i++) {
				// Calculo de las resistencias termicas radiales.
				EsInterior = true;
				Dint = FDiametroTubo[i];
				FResistRadInt[i] = 0;
				FResistRadExt[i] = 0;
				for(int j = 0; j < FNumCapas; j++) {
					Dext = Dint + 2 * FCapa[j].Espesor;
					if(FCapa[j].EsPrincipal == false) {
						Rcond = log(Dext / Dint) / __cons::Pi_x_2 / FCapa[j].Conductividad / FXref;
						if(EsInterior) {
							// Calculo de la resistencia termica radial interior.
							if(FCapa[j].EsFluida) {
								// Capa de camara de fluido.
								Text = __units::degCToK(FTPTubo[1][i]);
								Tint = __units::degCToK(FTPTubo[0][i]);
								// Tmed=(Text+Tint)/2;
								if(Tint != Text) {
									Rrad = (1 / FCapa[j].EmisividadInterior + Dint / Dext * (1 / FCapa[j].EmisividadExterior - 1)) /
										   (__cons::Sigma * __cons::Pi * Dint * FXref) * (Tint - Text) / (pow4(
													   Tint) - pow4(Text));
									FResistRadInt[i] += 1 / (1 / Rcond + 1 / Rrad);
								} else {
									FResistRadInt[i] += Rcond;
								}
							} else {
								// Capa de material.
								FResistRadInt[i] += Rcond;
							}
						} else {
							// Calculo de la resistencia termica radial exterior.
							// Calculo de la resistencia termica radial interior.
							if(FCapa[j].EsFluida) {
								// Capa de camara de fluido.
								Text = __units::degCToK(FTPTubo[2][i]);
								Tint = __units::degCToK(FTPTubo[1][i]);
								if(Tint != Text) {
									Rrad = (1 / FCapa[j].EmisividadInterior + Dint / Dext * (1 / FCapa[j].EmisividadExterior - 1)) /
										   (__cons::Sigma * __cons::Pi * Dint * FXref) * (Tint - Text) / (pow4(
													   Tint) - pow4(Text));
									FResistRadExt[i] += 1 / (1 / Rcond + 1 / Rrad);
								} else {
									FResistRadExt[i] += Rcond;
								}
							} else {
								// Capa de material.
								FResistRadExt[i] += Rcond;
							}
						}
					} else {
						// Calculo de la resistencia termica radial exterior e interior de la capa principal.
						FResistRadInt[i] += log((Dint + FCapa[j].Espesor) / Dint) / __cons::Pi_x_2 / FCapa[j].Conductividad / FXref;
						FResistRadExt[i] += log(Dext / (Dint + FCapa[j].Espesor)) / __cons::Pi_x_2 / FCapa[j].Conductividad / FXref;
						EsInterior = false;
					}
					Dint = Dext;
				}

				// Calculo de las resistencias termicas axiales.
				FResistAxiAnt[i] = 0;
				FResistAxiPos[i] = 0;
				DIntPrin = FDiametroTubo[i] + 2 * FEspesorIntPrin;
				if(i == 0) {
					if(BC[FNodoIzq - 1]->getTipoCC() == nmPipesConnection) {
						UnionEspes = dynamic_cast<TCCUnionEntreTubos*>(BC[FNodoIzq - 1])->getEspesor();
						UnionConduct = dynamic_cast<TCCUnionEntreTubos*>(BC[FNodoIzq - 1])->getConductividad();
						if(UnionConduct > 0) {
							FResistAxiAnt[i] = UnionEspes / UnionConduct / (__cons::Pi * (DIntPrin + FEspesorPrin) * FEspesorPrin);
						}
					}
					FResistAxiPos[i] = FXref / FConductPrin / (__cons::Pi * (DIntPrin + FEspesorPrin) * FEspesorPrin);
				}
#ifdef ParticulateFilter
				else if(BC[FNodoIzq - 1]->getTipoCC()
						== nmPipeToPlenumConnection) {
					if(FHayDPFNodoIzq) {
						if(FTipoCanal[0] == 0) {
							// Junction to a DPF inlet channel
							FResistAxiAnt[i] = FDPFEntradaTubo->getAjustRAxAnt
											   () /
											   (Pi * FDPFEntradaTubo->getConductividadMetal
												() * (FDPFEntradaTubo->getDiametroEfect()
													  + 2 * FDPFEntradaTubo->getEspesorAislante
													  () + 2 * FDPFEntradaTubo->getEspesorAire()
													  + FDPFEntradaTubo->getEspesorMetal())
												* FDPFEntradaTubo->getEspesorMetal());
						} else { // Junction to a DPF outlet channel
							FResistAxiAnt[i] = FDPFEntradaTubo->getAjustRAxPos
											   () /
											   (Pi * FDPFEntradaTubo->getConductividadMetal
												() * (FDPFEntradaTubo->getDiametroEfect()
													  + 2 * FDPFEntradaTubo->getEspesorAislante
													  () + 2 * FDPFEntradaTubo->getEspesorAire()
													  + FDPFEntradaTubo->getEspesorMetal())
												* FDPFEntradaTubo->getEspesorMetal());
						}
					}
				}
#endif
				else if(i == FNin - 1) {
					FResistAxiAnt[i] = FXref / FConductPrin / (__cons::Pi * (DIntPrin + FEspesorPrin) * FEspesorPrin);
					if(BC[FNodoDer - 1]->getTipoCC() == nmPipesConnection) {
						UnionEspes = dynamic_cast<TCCUnionEntreTubos*>(BC[FNodoDer - 1])->getEspesor();
						UnionConduct = dynamic_cast<TCCUnionEntreTubos*>(BC[FNodoDer - 1])->getConductividad();
						if(UnionConduct > 0) {
							FResistAxiPos[i] = UnionEspes / UnionConduct / (__cons::Pi * (DIntPrin + FEspesorPrin) * FEspesorPrin);
						}
					}
#ifdef ParticulateFilter
					else if(BC[FNodoDer - 1]->getTipoCC()
							== nmPipeToPlenumConnection) {
						if(FHayDPFNodoDer) {
							if(FTipoCanal[1] == 0) {
								// Junction to a DPF inlet channel
								FResistAxiPos[i]
									= FDPFSalidaTubo->getAjustRAxAnt() /
									  (Pi * FDPFSalidaTubo->getConductividadMetal
									   () * (FDPFSalidaTubo->getDiametroEfect()
											 + 2 *
											 FDPFSalidaTubo->getEspesorAislante()
											 + 2 * FDPFSalidaTubo->getEspesorAire()
											 + FDPFSalidaTubo->getEspesorMetal())
									   * FDPFSalidaTubo->getEspesorMetal());
							} else { // Junction to a DPF outlet channel
								FResistAxiPos[i]
									= FDPFSalidaTubo->getAjustRAxPos() /
									  (Pi * FDPFSalidaTubo->getConductividadMetal
									   () * (FDPFSalidaTubo->getDiametroEfect()
											 + 2 *
											 FDPFSalidaTubo->getEspesorAislante()
											 + 2 * FDPFSalidaTubo->getEspesorAire()
											 + FDPFSalidaTubo->getEspesorMetal())
									   * FDPFSalidaTubo->getEspesorMetal());
							}
						}
					}
#endif
				} else {
					FResistAxiAnt[i] = FXref / FConductPrin / (__cons::Pi * (DIntPrin + FEspesorPrin) * FEspesorPrin);
					FResistAxiPos[i] = FXref / FConductPrin / (__cons::Pi * (DIntPrin + FEspesorPrin) * FEspesorPrin);
				}

				// Calculo de las capacidades termicas.
				EsInterior = true;
				Dint = FDiametroTubo[i];
				FCapInt[i] = 0.;
				FCapMed[i] = 0.;
				FCapExt[i] = 0.;
				for(int j = 0; j < FNumCapas; j++) {
					Dext = Dint + 2 * FCapa[j].Espesor;
					if(FCapa[j].EsPrincipal == false) {
						if(EsInterior) {
							// Calculo de la capacidad termica interior.
							if(!FCapa[j].EsFluida) {
								// Capa de material.
								Cap = FCapa[j].Density * FCapa[j].CalorEspecifico * __geom::Ring_area(Dint, Dext) * FXref;
								FCapInt[i] += Cap;
							}
						} else {
							// Calculo de la capacidad termica exterior.
							if(!FCapa[j].EsFluida) {
								// Capa de material.
								Cap = FCapa[j].Density * FCapa[j].CalorEspecifico * __geom::Ring_area(Dint, Dext) * FXref;
								FCapExt[i] += Cap;
							}
						}
					} else {
						// Calculo de la capacidad termica exterior, media e interior de la capa principal.
						FCapInt[i] += FDensidadPrin * FCalEspPrin * __geom::Ring_area(DIntPrin, DIntPrin + 0.5 * FEspesorPrin) * FXref;
						FCapMed[i] = FDensidadPrin * FCalEspPrin * __geom::Ring_area(DIntPrin + 0.5 * FEspesorPrin,
									 DIntPrin + 1.5 * FEspesorPrin) * FXref;
						FCapExt[i] += FDensidadPrin * FCalEspPrin * __geom::Ring_area(DIntPrin + 1.5 * FEspesorPrin,
									  DIntPrin + 2 * FEspesorPrin) * FXref;
						EsInterior = false;
					}
					Dint = Dext;
				}
			}
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaResistenciasdePared en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculaCoeficientePeliculaInterior(TCondicionContorno **BC) {
#ifdef usetry
	try {
#endif
		double Tg = 0., cesp = 0., viscgas = 0., cond = 0., viscpared = 0.;

		if(FCoefAjusTC != 0) {
			for(int i = 0; i < FNin; i++) {

				Tg = FTemperature[i];
				cesp = FGamma[i] * FRMezcla[i] / (FGamma[i] - 1);
				viscgas = 1.4615e-6 * pow150(Tg) / (Tg + 110.4);
				cond = viscgas * cesp / 0.709;
				if(DoubEqZero(FVelPro[i])) {
					FRe[i] = 1E-06; // Para que nunca valga 0 y evitar divisiones por cero.
				} else {
					FRe[i] = Frho[i] * FVelPro[i] * FDiametroTubo[i] / viscgas;
				}

				// Calculo del coeficiente de pelicula interior
				switch(FTipoTransCal) {
				case nmTuboAdmision:
					// Termino de conveccion correlacion Depcick - Assanis
					Fhi[i] = 0.0694 * pow075(FRe[i]) * cond / FDiametroTubo[i];
					break;
				case nmPipaAdmision:
					// Termino de conveccion correlacion Depcick - Assanis
					Fhi[i] = 0.0694 * pow075(FRe[i]) * cond / FDiametroTubo[i];
					break;
				case nmTuboEscape:
					// Termino de conveccion correlacion M. Reyes
					Fhi[i] = 1.6 * pow(FRe[i], 0.4) * cond / FDiametroTubo[i] * FCoefTurbulencia[i];
					break;
				case nmPipaEscape:
					// Termino de conveccion correlacion de Caton + M. Reyes
					viscpared = 1.4615e-6 * pow150(__units::degCToK(FTPTubo[0][i])) / (__units::degCToK(FTPTubo[0][i]) + 110.4);
					Fhi[i] = 0.1 * pow(FRe[i], 0.8) * 0.709 * pow(viscgas / viscpared, 0.14) * cond / FDiametroTubo[i];
					break;
				default:
					std::cout << "WARNING: Transmision de calor mal definida en el tubo: " << FNumeroTubo << std::endl;
				}
			}
		} else if(FCoefAjusFric != 0) {
			for(int i = 0; i < FNin; i++) {
				Tg = FTemperature[i];
				viscgas = 1.4615e-6 * pow150(Tg) / (Tg + 110.4);
				FRe[i] = Frho[i] * FVelPro[i] * FDiametroTubo[i] / viscgas;
			}
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaCoeficientePeliculaInterior en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculaTemperaturaPared(TBloqueMotor **Engine, double Theta, double CrankAngle, TCondicionContorno **BC) {
	double Tg = 0.;
	double zzz = 0., czz = 0., cz1 = 0., uq1 = 0.;
	double DeltaTTPared = 0.;
	double Tpant0 = 0., Tpant1 = 0., Tpant2 = 0., Tpantant = 0., Tpantpos = 0., Text = 0., Ri = 0., Re = 0., ErrorTp = 0.;
	bool EsPrimeraVez;
	int extremo = 0, nodo = 0;
#ifdef usetry
	try {
#endif

		DeltaTTPared = FTime1 - FTime0;

		for(int i = 0; i < FNin; i++) {
			FTParedAnt[0][i] = FTPTubo[0][i];
			FTParedAnt[1][i] = FTPTubo[1][i];
			FTParedAnt[2][i] = FTPTubo[2][i];

			zzz = 0.013 / DeltaTTPared;
			czz = 2 / (zzz + 1);
			uq1 = fabs(FVelocidadDim[i]);
			if(FTipoTransCal == nmTuboEscape)
				cz1 = czz;
			else
				cz1 = 1;
			FVelPro[i] = cz1 * uq1 + (1 - cz1) * FVelPro[i];
		}

		if(FTipoCalcTempPared != nmTempConstante && FCoefAjusTC != 0) {
			// Tiene que existir motor.
			if(Theta > FAnguloTotalCiclo) {
				FSUMTime += DeltaTTPared;
			}
			for(int i = 0; i < FNin; i++) {
				// Establece la temperatura del gas en el interior del conducto.
				Tg = FTemperature[i];

				// Establece la temperatura del fluido en el exterior del conducto.
				if(FTipoTransCal == nmTuboAdmision || FTipoTransCal == nmTuboEscape) {
					Text = FTExt;
				} else {
					Text = __units::degCToK(Engine[0]->getTempRefrigerante());
				}

				// Establece la temperatura de la pared en el instante de calculo anterior...
				// ...en los nodos anterior y posterior.
				// Tpantant=FTParedAnt[1][i]+273.;
				// Tpantpos=FTParedAnt[1][i]+273.;
				if(i == 0) {
					if(BC[FNodoIzq - 1]->getTipoCC() == nmPipesConnection) {
						if(dynamic_cast<TCCUnionEntreTubos*>(BC[FNodoIzq - 1])->getConductividad() > 0) {
							if(BC[FNodoIzq - 1]->GetTuboExtremo(0).Pipe->getNumeroTubo() == FNumeroTubo) {
								extremo = 1;
								if(BC[FNodoIzq - 1]->GetTuboExtremo(extremo).TipoExtremo == nmLeft) {
									nodo = 0;
								} else {
									nodo = BC[FNodoIzq - 1]->GetTuboExtremo(extremo).Pipe->getNin() - 1;
								}
							} else {
								extremo = 0;
								if(BC[FNodoIzq - 1]->GetTuboExtremo(extremo).TipoExtremo == nmLeft) {
									nodo = 0;
								} else {
									nodo = BC[FNodoIzq - 1]->GetTuboExtremo(extremo).Pipe->getNin() - 1;
								}
							}
							Tpantant = __units::degCToK(BC[FNodoIzq - 1]->GetTuboExtremo(extremo).Pipe->GetTPTuboAnt(1, nodo));
						}
					}
#ifdef ParticulateFilter
					else if(BC[FNodoIzq - 1]->getTipoCC() == nmPipeToPlenumConnection) {
						if(FHayDPFNodoIzq) {
							Tpantant = __units::degCToK(FDPFEntradaTubo->GetTSuperficie
														(FNodoDPFEntrada, 2));
						}
					}
#endif
					Tpantpos = __units::degCToK(FTParedAnt[1][i + 1]);
				} else if(i == FNin - 1) {
					Tpantant = __units::degCToK(FTParedAnt[1][i - 1]);
					if(BC[FNodoDer - 1]->getTipoCC() == nmPipesConnection) {
						if(dynamic_cast<TCCUnionEntreTubos*>(BC[FNodoDer - 1])->getConductividad() > 0) {
							if(BC[getNodoDer() - 1]->GetTuboExtremo(0).Pipe->getNumeroTubo() == FNumeroTubo) {
								extremo = 1;
								if(BC[FNodoDer - 1]->GetTuboExtremo(extremo).TipoExtremo == nmLeft) {
									nodo = 0;
								} else {
									nodo = BC[FNodoDer - 1]->GetTuboExtremo(extremo).Pipe->getNin() - 1;
								}
							} else {
								extremo = 0;
								if(BC[FNodoDer - 1]->GetTuboExtremo(extremo).TipoExtremo == nmLeft) {
									nodo = 0;
								} else {
									nodo = BC[FNodoDer - 1]->GetTuboExtremo(extremo).Pipe->getNin() - 1;
								}
							}
							Tpantpos = __units::degCToK(BC[FNodoDer - 1]->GetTuboExtremo(extremo).Pipe->GetTPTuboAnt(1, nodo));
						}
					}
#ifdef ParticulaFilter
					else if(BC[FNodoDer - 1]->getTipoCC() == nmPlenum) {
						if(FHayDPFNodoDer) {
							Tpantpos = __units::degCToK(FDPFSalidaTubo->GetTSuperficie
														(FNodoDPFSalida, 2));
						}
					}
#endif
				} else {
					Tpantant = __units::degCToK(FTParedAnt[1][i - 1]);
					Tpantpos = __units::degCToK(FTParedAnt[1][i + 1]);
				}
				// ...en los nodos interior, medio y exterior.
				Tpant0 = __units::degCToK(FTParedAnt[0][i]);
				Tpant1 = __units::degCToK(FTParedAnt[1][i]);
				Tpant2 = __units::degCToK(FTParedAnt[2][i]);

				// Resistencias termicas interior y exterior.
				if(DoubEqZero(Fhi[i])) {
					Ri = 100000000.;
				} else {
					Ri = 1 / __cons::Pi / FDiametroTubo[i] / Fhi[i] / FCoefAjusTC / FXref;
				}
				if(DoubEqZero(Fhe[i])) {
					Re = 100000000.;
				} else {
					Re = 1 / __cons::Pi / (FDiametroTubo[i] + 2 * (FEspesorIntPrin + FEspesorPrin + FEspesorExtPrin)) / Fhe[i] / FXref;
				}
				// Calculo de las temperaturas de pared.
				FTPTubo[2][i] = DeltaTTPared / FCapExt[i] * (1 / FResistRadExt[i] * (Tpant1 - Tpant2) + 1 / Re *
								(Text - Tpant2)) + Tpant2;
				if(FResistAxiAnt[i] > 0. && FResistAxiPos[i] > 0.) {
					FTPTubo[1][i] = DeltaTTPared / FCapMed[i] * (1 / FResistRadInt[i] * (Tpant0 - Tpant1) + 1 / FResistRadExt[i] *
									(Tpant2 - Tpant1) + 1 / FResistAxiAnt[i] * (Tpantant - Tpant1) + 1 / FResistAxiPos[i] * (Tpantpos - Tpant1)) + Tpant1;
				} else if(FResistAxiAnt[i] > 0.) {
					FTPTubo[1][i] = DeltaTTPared / FCapMed[i] * (1 / FResistRadInt[i] * (Tpant0 - Tpant1) + 1 / FResistRadExt[i] *
									(Tpant2 - Tpant1) + 1 / FResistAxiAnt[i] * (Tpantant - Tpant1)) + Tpant1;
				} else if(FResistAxiPos[i] > 0.) {
					FTPTubo[1][i] = DeltaTTPared / FCapMed[i] * (1 / FResistRadInt[i] * (Tpant0 - Tpant1) + 1 / FResistRadExt[i] *
									(Tpant2 - Tpant1) + 1 / FResistAxiPos[i] * (Tpantpos - Tpant1)) + Tpant1;
				} else {
					FTPTubo[1][i] = DeltaTTPared / FCapMed[i] * (1 / FResistRadInt[i] * (Tpant0 - Tpant1) + 1 / FResistRadExt[i] *
									(Tpant2 - Tpant1)) + Tpant1;
				}
				FTPTubo[0][i] = DeltaTTPared / FCapInt[i] * (1 / Ri * (Tg - Tpant0) + 1 / FResistRadInt[i] *
								(Tpant1 - Tpant0)) + Tpant0;
				for(int k = 0; k < 3; k++) {
					FTPTubo[k][i] = __units::KTodegC(FTPTubo[k][i]);
				}

				// Si el tipo de calculo es sin inercia termica o lleva menos de "NumCiclosSinInerciaTermica" ciclos calculando...
				if(FTipoCalcTempPared == nmVariableSinInerciaTermica
				   || Theta / FAnguloTotalCiclo <= Engine[0]->getNumCiclosSinInerciaTermica()) {
					if(Fhi[i] != 0. && Theta > FAnguloTotalCiclo) {
						// Sumatorio de h*Tg*incrt intermedio(para el calculo de la integral).
						FSUMTPTuboPro[0][1][i] += 1 / (Ri + FResistRadInt[i]) * Tg * DeltaTTPared;
						// Sumatorio de h*incrt intermedio(para el calculo de la integral).
						FSUMTPTuboPro[1][1][i] += 1 / (Ri + FResistRadInt[i]) * DeltaTTPared;
						// Sumatorio de h*Tg*incrt interior(para el calculo de la integral).
						FSUMTPTuboPro[0][0][i] += 1 / Ri * Tg * DeltaTTPared;
						// Sumatorio de h*incrt interior(para el calculo de la integral).
						FSUMTPTuboPro[1][0][i] += 1 / Ri * DeltaTTPared;
					}
				}

			}

			// Si esta al final del ciclo...
			if(FCicloTubo != Engine[0]->getCiclo() && FSUMTime > 0.) {
				// ...si (el tipo de calculo es sin inercia termica o lleva menos de "NumCiclosSinInerciaTermica" ciclos calculando) y esta en el segundo ciclo, calcula la temperatura de convergencia
				if((FTipoCalcTempPared == nmVariableSinInerciaTermica
					|| Theta / FAnguloTotalCiclo <= Engine[0]->getNumCiclosSinInerciaTermica()) && Theta > FAnguloTotalCiclo + 1) {
					ErrorTp = 1.;
					EsPrimeraVez = true;
					while(ErrorTp >=
						  1) {  // Itera hasta conseguir una diferencia entre las temperaturas de pared menor a 1degC entre Steps.
						ErrorTp = 0.;
						for(int i = 0; i < FNin; i++) {
							// Establece la temperatura de la pared en el instante de calculo anterior...
							// ...en los nosdos anterior y posterior.
							Tpantant = __units::degCToK(FTPTubo[1][i]);
							Tpantpos = __units::degCToK(FTPTubo[1][i]);
							if(i == 0) {
								if(BC[FNodoIzq - 1]->getTipoCC() == nmPipesConnection) {
									if(dynamic_cast<TCCUnionEntreTubos*>(BC[FNodoIzq - 1])->getConductividad() > 0) {
										if(BC[FNodoIzq - 1]->GetTuboExtremo(0).Pipe->getNumeroTubo() == FNumeroTubo) {
											extremo = 1;
											if(BC[FNodoIzq - 1]->GetTuboExtremo(extremo).TipoExtremo == nmLeft) {
												nodo = 0;
											} else {
												nodo = BC[FNodoIzq - 1]->GetTuboExtremo(extremo).Pipe->getNin() - 1;
											}
										} else {
											extremo = 0;
											if(BC[FNodoIzq - 1]->GetTuboExtremo(extremo).TipoExtremo == nmLeft) {
												nodo = 0;
											} else {
												nodo = BC[FNodoIzq - 1]->GetTuboExtremo(extremo).Pipe->getNin() - 1;
											}
										}
										Tpantant = __units::degCToK(BC[FNodoIzq - 1]->GetTuboExtremo(extremo).Pipe->GetTPTuboAnt(1, nodo));
									}
								}
#ifdef ParticulateFilter
								else if
								(BC[FNodoIzq - 1]->getTipoCC()
								 == nmPipeToPlenumConnection) {
									if(FHayDPFNodoIzq) {
										Tpantant =
											__units::degCToK(FDPFEntradaTubo->GetTSuperficie
															 (FNodoDPFEntrada, 2));
									}
								}
#endif
								Tpantpos = __units::degCToK(FTPTubo[1][i + 1]);
							} else if(i == FNin - 1) {
								Tpantant = __units::degCToK(FTPTubo[1][i - 1]);
								if(BC[FNodoDer - 1]->getTipoCC() == nmPipesConnection) {
									if(dynamic_cast<TCCUnionEntreTubos*>(BC[FNodoDer - 1])->getConductividad() > 0) {
										if(BC[FNodoDer - 1]->GetTuboExtremo(0).Pipe->getNumeroTubo() == FNumeroTubo) {
											extremo = 1;
											if(BC[FNodoDer - 1]->GetTuboExtremo(extremo).TipoExtremo == nmLeft) {
												nodo = 0;
											} else {
												nodo = BC[FNodoDer - 1]->GetTuboExtremo(extremo).Pipe->getNin() - 1;
											}
										} else {
											extremo = 0;
											if(BC[FNodoDer - 1]->GetTuboExtremo(extremo).TipoExtremo == nmLeft) {
												nodo = 0;
											} else {
												nodo = BC[FNodoDer - 1]->GetTuboExtremo(extremo).Pipe->getNin() - 1;
											}
										}
										Tpantpos = __units::degCToK(BC[FNodoDer - 1]->GetTuboExtremo(extremo).Pipe->GetTPTuboAnt(1, nodo));
									}
								}
#ifdef ParticulateFilter
								else if
								(BC[FNodoDer - 1]->getTipoCC()
								 == nmPipeToPlenumConnection) {
									if(FHayDPFNodoDer) {
										Tpantpos =
											__units::degCToK(FDPFSalidaTubo->GetTSuperficie
															 (FNodoDPFSalida, 2));
									}
								}
#endif
							} else {
								Tpantant = __units::degCToK(FTPTubo[1][i - 1]);
								Tpantpos = __units::degCToK(FTPTubo[1][i + 1]);
							}

							// ...en los nodos interior, medio y exterior.
							Tpant0 = __units::degCToK(FTPTubo[0][i]);
							Tpant1 = __units::degCToK(FTPTubo[1][i]);
							Tpant2 = __units::degCToK(FTPTubo[2][i]);

							if(EsPrimeraVez) {
								FTPTubo[1][i] = (FSUMTime * 1 / (Re + FResistRadExt[i]) * Text + FSUMTPTuboPro[0][1][i]) / (FSUMTime * 1 /
												(Re + FResistRadExt[i]) + FSUMTPTuboPro[1][1][i]);
							} else {
								if(FResistAxiAnt[i] > 0. && FResistAxiPos[i] > 0.) {
									FTPTubo[1][i] = (FSUMTime * (1 / (Re + FResistRadExt[i]) * Text + 1 / FResistAxiAnt[i] * Tpantant + 1 / FResistAxiPos[i]
																 * Tpantpos) + FSUMTPTuboPro[0][1][i]) / (FSUMTime * (1 / (Re + FResistRadExt[i]) + 1 / FResistAxiAnt[i] + 1 /
																		 FResistAxiPos[i]) + FSUMTPTuboPro[1][1][i]);
								} else if(FResistAxiAnt[i] > 0.) {
									FTPTubo[1][i] = (FSUMTime * (1 / (Re + FResistRadExt[i]) * Text + 1 / FResistAxiAnt[i] * Tpantant) +
													 FSUMTPTuboPro[0][1][i]) / (FSUMTime * (1 / (Re + FResistRadExt[i]) + 1 / FResistAxiAnt[i]) + FSUMTPTuboPro[1][1][i]);
								} else if(FResistAxiPos[i] > 0.) {
									FTPTubo[1][i] = (FSUMTime * (1 / (Re + FResistRadExt[i]) * Text + 1 / FResistAxiPos[i] * Tpantpos) +
													 FSUMTPTuboPro[0][1][i]) / (FSUMTime * (1 / (Re + FResistRadExt[i]) + 1 / FResistAxiPos[i]) + FSUMTPTuboPro[1][1][i]);
								} else {
									FTPTubo[1][i] = (FSUMTime * (1 / (Re + FResistRadExt[i]) * Text) + FSUMTPTuboPro[0][1][i]) / (FSUMTime * (1 /
													(Re + FResistRadExt[i])) + FSUMTPTuboPro[1][1][i]);
								}
							}
							FTPTubo[0][i] = ((FSUMTime * 1 / FResistRadInt[i] * Tpant1 + FSUMTPTuboPro[0][0][i]) /
											 (FSUMTime * 1 / FResistRadInt[i] + FSUMTPTuboPro[1][0][i]));
							FTPTubo[2][i] = (FSUMTime * (1 / Re * Text + 1 / FResistRadExt[i] * Tpant1)) / (FSUMTime *
											(1 / Re + 1 / FResistRadExt[i]));
							if(ErrorTp < fabs(Tpant1 - FTPTubo[1][i])) {
								ErrorTp = fabs(Tpant1 - FTPTubo[1][i]);
							}
							for(int k = 0; k < 3; k++) {
								FTPTubo[k][i] = __units::KTodegC(FTPTubo[k][i]);
							}
						}
						EsPrimeraVez = false;
					}
				}
				for(int i = 0; i < FNin; i++) {
					for(int j = 0; j < 2; j++) {
						for(int k = 0; k < 3; k++) {
							FSUMTPTuboPro[j][k][i] = 0.;
						}
					}
				}
			}
		}

		if(FTipoCalcTempPared != nmTempConstante && FCoefAjusTC != 0) {
			if(FCicloTubo != Engine[0]->getCiclo()) {
				FSUMTime = 0.;
				FCicloTubo = Engine[0]->getCiclo();
			}
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaTemperaturaPared en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculaTemperaturaParedSinMotor(TCondicionContorno **BC) {
	double Tg = 0.;
	double zzz = 0., czz = 0., cz1 = 0., uq1 = 0.;
	double DeltaTTPared = 0.;
	double Tpant0 = 0., Tpant1 = 0., Tpant2 = 0., Tpantant = 0., Tpantpos = 0., Text = 0., Ri = 0., Re = 0., ErrorTp = 0.;
	bool EsPrimeraVez;
	int extremo = 0, nodo = 0;

#ifdef usetry
	try {
#endif

		DeltaTTPared = FTime1 - FTime0;

		for(int i = 0; i < FNin; i++) {
			FTParedAnt[0][i] = FTPTubo[0][i];
			FTParedAnt[1][i] = FTPTubo[1][i];
			FTParedAnt[2][i] = FTPTubo[2][i];

			zzz = 0.013 / DeltaTTPared;
			czz = 2 / (zzz + 1);
			uq1 = fabs(FVelocidadDim[i]);
			if(FTipoTransCal == nmTuboEscape)
				cz1 = czz;
			else
				cz1 = 1;
			FVelPro[i] = cz1 * uq1 + (1 - cz1) * FVelPro[i];
		}

		if(FTipoCalcTempPared != nmTempConstante && FCoefAjusTC != 0) {
			FCicloActual = FTime1 / FDuracionCiclo;
			if(FTime1 > FDuracionCiclo) {
				FSUMTime += DeltaTTPared;
			}
			for(int i = 0; i < FNin; i++) {
				// Establece la temperatura del gas en el interior del conducto.
				Tg = FTemperature[i];

				// Establece la temperatura del fluido en el exterior del conducto.
				// Como no hay motor quito los if referentes a tipo de tubo.
				Text = FTExt;

				// Establece la temperatura de la pared en el instante de calculo anterior...
				// ...en los nodos anterior y posterior.
				// Tpantant=FTParedAnt[1][i]+273.;
				// Tpantpos=FTParedAnt[1][i]+273.;
				if(i == 0) {
					if(BC[FNodoIzq - 1]->getTipoCC() == nmPipesConnection) {
						if(dynamic_cast<TCCUnionEntreTubos*>(BC[FNodoIzq - 1])->getConductividad() > 0) {
							if(BC[FNodoIzq - 1]->GetTuboExtremo(0).Pipe->getNumeroTubo() == FNumeroTubo) {
								extremo = 1;
								if(BC[FNodoIzq - 1]->GetTuboExtremo(extremo).TipoExtremo == nmLeft) {
									nodo = 0;
								} else {
									nodo = BC[FNodoIzq - 1]->GetTuboExtremo(extremo).Pipe->getNin() - 1;
								}
							} else {
								extremo = 0;
								if(BC[FNodoIzq - 1]->GetTuboExtremo(extremo).TipoExtremo == nmLeft) {
									nodo = 0;
								} else {
									nodo = BC[FNodoIzq - 1]->GetTuboExtremo(extremo).Pipe->getNin() - 1;
								}
							}
							Tpantant = __units::degCToK(BC[FNodoIzq - 1]->GetTuboExtremo(extremo).Pipe->GetTPTuboAnt(1, nodo));
						}
					}
#ifdef ParticulateFilter
					else if(BC[FNodoIzq - 1]->getTipoCC() == nmPipeToPlenumConnection) {
						if(FHayDPFNodoIzq) {
							Tpantant = __units::degCToK(FDPFEntradaTubo->GetTSuperficie
														(FNodoDPFEntrada, 2));
						}
					}
#endif
					Tpantpos = __units::degCToK(FTParedAnt[1][i + 1]);
				} else if(i == FNin - 1) {
					Tpantant = __units::degCToK(FTParedAnt[1][i - 1]);
					if(BC[FNodoDer - 1]->getTipoCC() == nmPipesConnection) {
						if(dynamic_cast<TCCUnionEntreTubos*>(BC[FNodoDer - 1])->getConductividad() > 0) {
							if(BC[getNodoDer() - 1]->GetTuboExtremo(0).Pipe->getNumeroTubo() == FNumeroTubo) {
								extremo = 1;
								if(BC[FNodoDer - 1]->GetTuboExtremo(extremo).TipoExtremo == nmLeft) {
									nodo = 0;
								} else {
									nodo = BC[FNodoDer - 1]->GetTuboExtremo(extremo).Pipe->getNin() - 1;
								}
							} else {
								extremo = 0;
								if(BC[FNodoDer - 1]->GetTuboExtremo(extremo).TipoExtremo == nmLeft) {
									nodo = 0;
								} else {
									nodo = BC[FNodoDer - 1]->GetTuboExtremo(extremo).Pipe->getNin() - 1;
								}
							}
							Tpantpos = __units::degCToK(BC[FNodoDer - 1]->GetTuboExtremo(extremo).Pipe->GetTPTuboAnt(1, nodo));
						}
					}
#ifdef ParticulateFilter
					else if(BC[FNodoDer - 1]->getTipoCC() == nmPipeToPlenumConnection) {
						if(FHayDPFNodoDer) {
							Tpantpos = __units::degCToK(FDPFSalidaTubo->GetTSuperficie
														(FNodoDPFSalida, 2));
						}
					}
#endif
				} else {
					Tpantant = __units::degCToK(FTParedAnt[1][i - 1]);
					Tpantpos = __units::degCToK(FTParedAnt[1][i + 1]);
				}
				// ...en los nodos interior, medio y exterior.
				Tpant0 = __units::degCToK(FTParedAnt[0][i]);
				Tpant1 = __units::degCToK(FTParedAnt[1][i]);
				Tpant2 = __units::degCToK(FTParedAnt[2][i]);

				// Resistencias termicas interior y exterior.
				if(DoubEqZero(Fhi[i])) {
					Ri = 100000000.;
				} else {
					Ri = 1 / __cons::Pi / FDiametroTubo[i] / Fhi[i] / FCoefAjusTC / FXref;
				}
				if(DoubEqZero(Fhe[i])) {
					Re = 100000000.;
				} else {
					Re = 1 / __cons::Pi / (FDiametroTubo[i] + 2 * (FEspesorIntPrin + FEspesorPrin + FEspesorExtPrin)) / Fhe[i] / FXref;
				}
				// Calculo de las temperaturas de pared.
				FTPTubo[2][i] = DeltaTTPared / FCapExt[i] * (1 / FResistRadExt[i] * (Tpant1 - Tpant2) + 1 / Re *
								(Text - Tpant2)) + Tpant2;
				if(FResistAxiAnt[i] > 0. && FResistAxiPos[i] > 0.) {
					FTPTubo[1][i] = DeltaTTPared / FCapMed[i] * (1 / FResistRadInt[i] * (Tpant0 - Tpant1) + 1 / FResistRadExt[i] *
									(Tpant2 - Tpant1) + 1 / FResistAxiAnt[i] * (Tpantant - Tpant1) + 1 / FResistAxiPos[i] * (Tpantpos - Tpant1)) + Tpant1;
				} else if(FResistAxiAnt[i] > 0.) {
					FTPTubo[1][i] = DeltaTTPared / FCapMed[i] * (1 / FResistRadInt[i] * (Tpant0 - Tpant1) + 1 / FResistRadExt[i] *
									(Tpant2 - Tpant1) + 1 / FResistAxiAnt[i] * (Tpantant - Tpant1)) + Tpant1;
				} else if(FResistAxiPos[i] > 0.) {
					FTPTubo[1][i] = DeltaTTPared / FCapMed[i] * (1 / FResistRadInt[i] * (Tpant0 - Tpant1) + 1 / FResistRadExt[i] *
									(Tpant2 - Tpant1) + 1 / FResistAxiPos[i] * (Tpantpos - Tpant1)) + Tpant1;
				} else {
					FTPTubo[1][i] = DeltaTTPared / FCapMed[i] * (1 / FResistRadInt[i] * (Tpant0 - Tpant1) + 1 / FResistRadExt[i] *
									(Tpant2 - Tpant1)) + Tpant1;
				}
				FTPTubo[0][i] = DeltaTTPared / FCapInt[i] * (1 / Ri * (Tg - Tpant0) + 1 / FResistRadInt[i] *
								(Tpant1 - Tpant0)) + Tpant0;
				for(int k = 0; k < 3; k++) {
					FTPTubo[k][i] = __units::KTodegC(FTPTubo[k][i]);
				}

				// Si el tipo de calculo es sin inercia termica o lleva menos de "NumCiclosSinInerciaTermica" ciclos calculando...
				if(FTipoCalcTempPared == nmVariableSinInerciaTermica || FCicloActual <= FNumCiclosSinInerciaTermica) {
					if(FTime1 > FDuracionCiclo) {
						// Sumatorio de h*Tg*incrt intermedio(para el calculo de la integral).
						FSUMTPTuboPro[0][1][i] += 1 / (Ri + FResistRadInt[i]) * Tg * DeltaTTPared;
						// Sumatorio de h*incrt intermedio(para el calculo de la integral).
						FSUMTPTuboPro[1][1][i] += 1 / (Ri + FResistRadInt[i]) * DeltaTTPared;
						// Sumatorio de h*Tg*incrt interior(para el calculo de la integral).
						FSUMTPTuboPro[0][0][i] += 1 / Ri * Tg * DeltaTTPared;
						// Sumatorio de h*incrt interior(para el calculo de la integral).
						FSUMTPTuboPro[1][0][i] += 1 / Ri * DeltaTTPared;
					}
				}

			}

			// Si esta al final del ciclo...
			if(FCicloTubo != FCicloActual && FSUMTime > 0. && FCicloActual > 1) {
				// ...si (el tipo de calculo es sin inercia termica o lleva menos de "NumCiclosSinInerciaTermica" ciclos calculando) y esta en el segundo ciclo, calcula la temperatura de convergencia
				if((FTipoCalcTempPared == nmVariableSinInerciaTermica || FCicloActual <= FNumCiclosSinInerciaTermica)
				   && FTime1 > FDuracionCiclo) {
					ErrorTp = 1.;
					EsPrimeraVez = true;
					while(ErrorTp >=
						  1) {  // Itera hasta conseguir una diferencia entre las temperaturas de pared menor a 1degC entre Steps.
						ErrorTp = 0.;
						for(int i = 0; i < FNin; i++) {
							// Establece la temperatura de la pared en el instante de calculo anterior...
							// ...en los nosdos anterior y posterior.
							Tpantant = __units::degCToK(FTPTubo[1][i]);
							Tpantpos = __units::degCToK(FTPTubo[1][i]);
							if(i == 0) {
								if(BC[FNodoIzq - 1]->getTipoCC() == nmPipesConnection) {
									if(dynamic_cast<TCCUnionEntreTubos*>(BC[FNodoIzq - 1])->getConductividad() > 0) {
										if(BC[FNodoIzq - 1]->GetTuboExtremo(0).Pipe->getNumeroTubo() == FNumeroTubo) {
											extremo = 1;
											if(BC[FNodoIzq - 1]->GetTuboExtremo(extremo).TipoExtremo == nmLeft) {
												nodo = 0;
											} else {
												nodo = BC[FNodoIzq - 1]->GetTuboExtremo(extremo).Pipe->getNin() - 1;
											}
										} else {
											extremo = 0;
											if(BC[FNodoIzq - 1]->GetTuboExtremo(extremo).TipoExtremo == nmLeft) {
												nodo = 0;
											} else {
												nodo = BC[FNodoIzq - 1]->GetTuboExtremo(extremo).Pipe->getNin() - 1;
											}
										}
										Tpantant = __units::degCToK(BC[FNodoIzq - 1]->GetTuboExtremo(extremo).Pipe->GetTPTuboAnt(1, nodo));
									}
								}
#ifdef ParticulateFilter
								else if
								(BC[FNodoIzq - 1]->getTipoCC()
								 == nmPipeToPlenumConnection) {
									if(FHayDPFNodoIzq) {
										Tpantant =
											__units::degCToK(FDPFEntradaTubo->GetTSuperficie
															 (FNodoDPFEntrada, 2));
									}
								}
#endif
								Tpantpos = __units::degCToK(FTPTubo[1][i + 1]);
							} else if(i == FNin - 1) {
								Tpantant = __units::degCToK(FTPTubo[1][i - 1]);
								if(BC[FNodoDer - 1]->getTipoCC() == nmPipesConnection) {
									if(dynamic_cast<TCCUnionEntreTubos*>(BC[FNodoDer - 1])->getConductividad() > 0) {
										if(BC[FNodoDer - 1]->GetTuboExtremo(0).Pipe->getNumeroTubo() == FNumeroTubo) {
											extremo = 1;
											if(BC[FNodoDer - 1]->GetTuboExtremo(extremo).TipoExtremo == nmLeft) {
												nodo = 0;
											} else {
												nodo = BC[FNodoDer - 1]->GetTuboExtremo(extremo).Pipe->getNin() - 1;
											}
										} else {
											extremo = 0;
											if(BC[FNodoDer - 1]->GetTuboExtremo(extremo).TipoExtremo == nmLeft) {
												nodo = 0;
											} else {
												nodo = BC[FNodoDer - 1]->GetTuboExtremo(extremo).Pipe->getNin() - 1;
											}
										}
										Tpantpos = __units::degCToK(BC[FNodoDer - 1]->GetTuboExtremo(extremo).Pipe->GetTPTuboAnt(1, nodo));
									}
								}
#ifdef ParticulateFilter
								else if
								(BC[FNodoDer - 1]->getTipoCC()
								 == nmPipeToPlenumConnection) {
									if(FHayDPFNodoDer) {
										Tpantpos =
											__units::degCToK(FDPFSalidaTubo->GetTSuperficie
															 (FNodoDPFSalida, 2));
									}
								}
#endif
							} else {
								Tpantant = __units::degCToK(FTPTubo[1][i - 1]);
								Tpantpos = __units::degCToK(FTPTubo[1][i + 1]);
							}

							// ...en los nodos interior, medio y exterior.
							Tpant0 = __units::degCToK(FTPTubo[0][i]);
							Tpant1 = __units::degCToK(FTPTubo[1][i]);
							Tpant2 = __units::degCToK(FTPTubo[2][i]);

							if(EsPrimeraVez) {
								FTPTubo[1][i] = (FSUMTime / (Re + FResistRadExt[i]) * Text + FSUMTPTuboPro[0][1][i]) / (FSUMTime /
												(Re + FResistRadExt[i]) + FSUMTPTuboPro[1][1][i]);
							} else {
								if(FResistAxiAnt[i] > 0. && FResistAxiPos[i] > 0.) {
									FTPTubo[1][i] = (FSUMTime * (1 / (Re + FResistRadExt[i]) * Text + 1 / FResistAxiAnt[i] * Tpantant + 1 / FResistAxiPos[i]
																 * Tpantpos) + FSUMTPTuboPro[0][1][i]) / (FSUMTime * (1 / (Re + FResistRadExt[i]) + 1 / FResistAxiAnt[i] + 1 /
																		 FResistAxiPos[i]) + FSUMTPTuboPro[1][1][i]);
								} else if(FResistAxiAnt[i] > 0.) {
									FTPTubo[1][i] = (FSUMTime * (1 / (Re + FResistRadExt[i]) * Text + 1 / FResistAxiAnt[i] * Tpantant) +
													 FSUMTPTuboPro[0][1][i]) / (FSUMTime * (1 / (Re + FResistRadExt[i]) + 1 / FResistAxiAnt[i]) + FSUMTPTuboPro[1][1][i]);
								} else if(FResistAxiPos[i] > 0.) {
									FTPTubo[1][i] = (FSUMTime * (1 / (Re + FResistRadExt[i]) * Text + 1 / FResistAxiPos[i] * Tpantpos) +
													 FSUMTPTuboPro[0][1][i]) / (FSUMTime * (1 / (Re + FResistRadExt[i]) + 1 / FResistAxiPos[i]) + FSUMTPTuboPro[1][1][i]);
								} else {
									FTPTubo[1][i] = (FSUMTime * (1 / (Re + FResistRadExt[i]) * Text) + FSUMTPTuboPro[0][1][i]) / (FSUMTime * (1 /
													(Re + FResistRadExt[i])) + FSUMTPTuboPro[1][1][i]);
								}
							}
							FTPTubo[0][i] = ((FSUMTime / FResistRadInt[i] * Tpant1 + FSUMTPTuboPro[0][0][i]) / (FSUMTime / FResistRadInt[i] +
											 FSUMTPTuboPro[1][0][i]));
							FTPTubo[2][i] = (FSUMTime * (1 / Re * Text + 1 / FResistRadExt[i] * Tpant1)) / (FSUMTime *
											(1 / Re + 1 / FResistRadExt[i]));
							if(ErrorTp < fabs(Tpant1 - FTPTubo[1][i])) {
								ErrorTp = fabs(Tpant1 - FTPTubo[1][i]);
							}
							for(int k = 0; k < 3; k++) {
								FTPTubo[k][i] = __units::KTodegC(FTPTubo[k][i]);
							}
						}
						EsPrimeraVez = false;
					}
				}
				for(int i = 0; i < FNin; i++) {
					for(int j = 0; j < 2; j++) {
						for(int k = 0; k < 3; k++) {
							FSUMTPTuboPro[j][k][i] = 0.;
						}
					}
				}
			}
			if(FCicloTubo != FCicloActual) {
				FCicloTubo = FCicloActual;
				FSUMTime = 0.;
			}
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaTemperaturaParedSinMotor en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::SalidaGeneralTubos(stEspecies *DatosEspecies) const {
#ifdef usetry
	try {
#endif
		if(FNumResMedios > 0) {
			printf("__________________________________\n");
			printf("  AVERAGE RESULTS IN PIPES        \n");
			printf("__________________________________\n\n\n");
			for(int i = 0; i < FNumResMedios; i++) {
				std::cout << "Average results in point " << i << " place in pipe " << FNumeroTubo;
				std::cout << " at " << ResultadosMedios[i].Distancia << " metres from left end" << std::endl;
				if(ResultadosMedios[i].TemperaturaGas)
					std::cout << "Average Temperature = " << ResultadosMedios[i].TemperaturaGasMED << " C" << std::endl;
				if(ResultadosMedios[i].Pressure)
					std::cout << "Average Pressure    = " << ResultadosMedios[i].PresionMED << " bar" << std::endl;
				if(ResultadosMedios[i].Velocity)
					std::cout << "Average Velocity    = " << ResultadosMedios[i].VelocidadMED << " m/s" << std::endl;
				if(ResultadosMedios[i].Massflow)
					std::cout << "Average Massflow    = " << ResultadosMedios[i].GastoMED << " kg/s" << std::endl;
				if(ResultadosMedios[i].NITmedio)
					std::cout << "NIT Medio         = " << ResultadosMedios[i].NITmedioMED << " Watts" << std::endl << std::endl;
				// std::cout << "Fraccion Masica Media de " << DatosEspecies[i].Nombre << ": " << ResultadosMedios[i].FraccionMED[i] << std::endl;
			}
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::SalidaGeneralTubos en el tubo nº: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

inline double TTubo::Maximo(double x, double y) {
	return x > y ? x : y;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

inline double TTubo::Minimo(double x, double y) {
	return x > y ? y : x;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::AjustaPaso(double TimeEndStep) {
#ifdef usetry
	try {
#endif
		FTime1 = TimeEndStep;
		FDeltaTime = FTime1 - FTime0;
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::AjustaPaso en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculaCaracteristicasExtremos(TCondicionContorno **BC, double DeltaTiempo) {
#ifdef usetry
	try {
#endif

		if(FVelocidad0[0] <= 0) {
			BC[FNodoIzq - 1]->PutEntropia(FTuboCCNodoIzq,
										  Interpola_Entropia(BC[FNodoIzq - 1]->GetTuboExtremo(FTuboCCNodoIzq).TipoExtremo, DeltaTiempo));
		}
		BC[FNodoIzq - 1]->PutBeta(FTuboCCNodoIzq,
								  Interpola_Caracteristica(BC[FNodoIzq - 1]->GetTuboExtremo(FTuboCCNodoIzq).Entropia, 1, 0, DeltaTiempo));

		if(FVelocidad0[FNin - 1] >= 0) {
			BC[FNodoDer - 1]->PutEntropia(FTuboCCNodoDer,
										  Interpola_Entropia(BC[FNodoDer - 1]->GetTuboExtremo(FTuboCCNodoDer).TipoExtremo, DeltaTiempo));
		}
		BC[FNodoDer - 1]->PutLanda(FTuboCCNodoDer,
								   Interpola_Caracteristica(BC[FNodoDer - 1]->GetTuboExtremo(FTuboCCNodoDer).Entropia, -1, getNin() - 1, DeltaTiempo));
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaCaracteristicasExtremos en el tubo: " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TTubo::Interpola_Entropia(nmPipeEnd TipoExtremoTubo, double DeltaTiempo) {
#ifdef usetry
	try {
#endif

		int signo = 1;
		int extremo = 0;
		int indiceCC = 0;

		if(TipoExtremoTubo == nmRight) {   // PipeEnd Derecho
			signo = -1;
			extremo = FNin - 1;
			indiceCC = 1;
		}
		double dtdx = DeltaTiempo / FXref;
		int ind = extremo;
		double entropia = 0.;
		double velocidadp = 0.;
		double asonidop = 0.;

		if(DeltaTiempo < 1e-15 || DoubEqZero(FVelocidadDim[extremo])) {

			Calculo_Entropia(entropia, velocidadp, extremo, 0., signo, DeltaTiempo, indiceCC);

		} else {
			int ind1 = ind + signo;

			stPathOrigin PathOrigin(FU0[0][ind], FU0[1][ind], FU0[0][ind1], FU0[1][ind1], dtdx, signo);

			double dist = zbrent(PathOrigin, 0., 1., 1e-5);

			Calculo_Entropia(entropia, velocidadp, ind, dist, signo, DeltaTiempo, indiceCC);

		}

		return entropia / __cons::ARef;
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::Interpola_Entropia: " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::Calculo_Entropia(double& entropia, double& velocidadp, int ind, double dist, int signo, double DeltaTiempo,
							 int indiceCC) {
#ifdef usetry
	try {
#endif

		int ind1 = ind + signo;

		double w0 = FU0[0][ind];
		double w1 = FU0[1][ind];
		double w2 = FU0[2][ind];
		double gammap = FGamma[ind];
		double Rmezclap = FRMezcla[ind];
		double diamep = FDiametroTubo[ind];

		double tptubop = FTPTubo[0][ind];
		double hip = Fhi[ind];
		double rhop = Frho[ind];
		double Rep = FRe[ind];

		for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
			FFraccionMasicaCC[indiceCC][j] = FFraccionMasicaEspecie[ind][j];
		}

		if(dist > 0. || dist < 1.) {
			w0 = Interpola(FU0[0][ind], FU0[0][ind1], 1., dist);
			w1 = Interpola(FU0[1][ind], FU0[1][ind1], 1., dist);
			w2 = Interpola(FU0[2][ind], FU0[2][ind1], 1., dist);
			gammap = Interpola(FGamma[ind], FGamma[ind1], 1., dist);
			Rmezclap = Interpola(FRMezcla[ind], FRMezcla[ind1], 1., dist);
			diamep = Interpola(FDiametroTubo[ind], FDiametroTubo[ind1], 1., dist);
			if(FCoefAjusTC != 0 || FCoefAjusFric != 0) {
				tptubop = Interpola(FTPTubo[0][ind], FTPTubo[0][ind1], 1., dist);
				hip = Interpola(Fhi[ind], Fhi[ind1], 1., dist);
				Rep = Interpola(FRe[ind], FRe[ind1], 1., dist);
			}

			for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
				FFraccionMasicaCC[indiceCC][j] = Interpola(FFraccionMasicaEspecie[ind][j], FFraccionMasicaEspecie[ind1][j], 1., dist);
			}
		}

		double gamma1p = __Gamma::G1(gammap);
		double gamma3p = __Gamma::G3(gammap);
		double gamma5p = __Gamma::G5(gammap);
		velocidadp = w1 / w0;
		rhop = w0 / __geom::Circle_area(diamep);
		double asonidop = sqrt(gammap * gamma1p * (w2 / w0 - pow2(velocidadp) / 2));
		double presionp = __units::PaToBar((w2 - pow2(w1) / 2. / w0) * gamma1p / __geom::Circle_area(diamep));
		double entropiap = asonidop / pow(presionp, gamma5p);
		entropia = entropiap;

		FAreaCC[indiceCC] = FArea[ind];
		FDensidadCC[indiceCC] = rhop;
		FVelocidadCC[indiceCC] = velocidadp; // Se que no hace falta hacerlo asi,pero me queda más ordenado.

		/* variacion de la entropia debida a la transmision del calor */
		/* ------------------------------------------ */
		if(!DoubEqZero(FCoefAjusTC)) {

			double q = 0;
			double tgasp = pow2(asonidop) / (gammap * Rmezclap);

			TransmisionCalor(tgasp, diamep, q, hip, rhop, tptubop);

			// Las siguientes expresiones estan en la Tesis de Corberan. Pagina 23
			double dacal = gamma3p * entropiap * q * FCoefAjusTC * DeltaTiempo / pow2(asonidop);

			entropia += dacal;
		}

		/* variacion de la entropia debida al termino de friccion */
		/* --------------------------------------- */
		if(!DoubEqZero(velocidadp) && !DoubEqZero(FCoefAjusFric)) {

			double f = 0.;
			double velabs = fabs(velocidadp);
			Colebrook(FFriccion, diamep, f, Rep);
			double dafric = gamma1p * FCoefAjusFric * f * entropiap * pow3(velabs) * DeltaTiempo / (diamep * pow2(asonidop));
			entropia += dafric;
		}
#ifdef usetry
	}

	catch(exception & N) {
		std::cout << "ERROR: TTubo::Calculo_Entropia " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TTubo::Interpola_Caracteristica(double entropia, int signo, int extremo, double DeltaTiempo) {
#ifdef usetry
	try {
#endif

		double dtdx = DeltaTiempo / FXref;
		int ind = extremo;
		double caracteristica = 0.;
		double velocidadp = 0.;
		double asonidop = 0.;

		if(DeltaTiempo < 1e-15) {
			Calculo_Caracteristica(caracteristica, velocidadp, asonidop, ind, 0., signo, entropia, DeltaTiempo);
		} else {

			dtdx = DeltaTiempo / FXref;

			int ind1 = ind + signo;

			stCharOrigin CharOrigin(FU0[0][ind], FU0[1][ind], FU0[2][ind], FU0[0][ind1], FU0[1][ind1], FU0[2][ind1], FGamma[ind],
									FGamma[ind1], dtdx, signo);

			double dist = zbrent(CharOrigin, 0., 1., 1e-5);

			Calculo_Caracteristica(caracteristica, velocidadp, asonidop, ind, dist, signo, entropia, DeltaTiempo);
		}
		return caracteristica / __cons::ARef;
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::Interpola_Caracteristica " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::Calculo_Caracteristica(double& caracteristica, double& velocidadp, double& asonidop, int ind, double dist,
								   int signo, double entropia, double DeltaTiempo) {
#ifdef usetry
	try {
#endif

		int ind1 = ind + signo;

		double w0 = FU0[0][ind];
		double w1 = FU0[1][ind];
		double w2 = FU0[2][ind];
		double gammap = FGamma[ind];
		double Rmezclap = FRMezcla[ind];
		double diamep = FDiametroTubo[ind];

		double tptubop = FTPTubo[0][ind];
		double hip = Fhi[ind];
		double rhop = Frho[ind];
		double Rep = FRe[ind];

		if(dist < 1. && dist > 0) {

			w0 = Interpola(FU0[0][ind], FU0[0][ind1], 1., dist);
			w1 = Interpola(FU0[1][ind], FU0[1][ind1], 1., dist);
			w2 = Interpola(FU0[2][ind], FU0[2][ind1], 1., dist);
			gammap = Interpola(FGamma[ind], FGamma[ind1], 1., dist);
			Rmezclap = Interpola(FRMezcla[ind], FRMezcla[ind1], 1., dist);
			diamep = Interpola(FDiametroTubo[ind], FDiametroTubo[ind1], 1., dist);
			if(FCoefAjusTC != 0 || FCoefAjusFric != 0) {
				tptubop = Interpola(FTPTubo[0][ind], FTPTubo[0][ind1], 1., dist);
				hip = Interpola(Fhi[ind], Fhi[ind1], 1., dist);
				rhop = Interpola(Frho[ind], Frho[ind1], 1., dist);
				Rep = Interpola(FRe[ind], FRe[ind1], 1., dist);
			}

		}
		double gamma1p = __Gamma::G1(gammap);
		double gamma3p = __Gamma::G3(gammap);
		double gamma5p = __Gamma::G5(gammap);
		velocidadp = w1 / w0;
		asonidop = sqrt(gammap * gamma1p * (w2 / w0 - pow2(velocidadp) / 2));
		caracteristica = asonidop - signo * gamma3p * velocidadp;

		// Las siguientes expresiones se pueden encontrar en la Tesis de Corberan
		// Pagina 22
		/* variacion debida a la transmision del calor */
		/* ------------------------------------------ */
		if(FCoefAjusTC != 0) {

			double q = 0;

			double tgasp = pow2(asonidop) / (gammap * Rmezclap);

			TransmisionCalor(tgasp, diamep, q, hip, rhop, tptubop);

			double dacal = gamma3p * gamma1p * DeltaTiempo * q * FCoefAjusTC / asonidop;

			caracteristica += dacal;

		}
		/* variacion debida a la variacion entropia */
		/* ---------------------------------------- */

		double presionp = __units::PaToBar((w2 - pow2(w1) / 2. / w0) * gamma1p / __geom::Circle_area(diamep));
		double entropiap = asonidop / pow(presionp, gamma5p);
		double increentropia = entropia * __cons::ARef - entropiap;
		double daen = asonidop * increentropia / entropiap;
		caracteristica += daen;

		/* variacion debida al cambio de seccion */
		/* ------------------------------------- */
		if(FDiametroTubo[ind1] != FDiametroTubo[ind]) {
			double daar = 0.;
			if(signo == 1) {
				daar = -gamma3p * asonidop * velocidadp * 2 * (FDiametroTubo[ind1] - FDiametroTubo[ind]) * DeltaTiempo /
					   (diamep * FXref);
			} else if(signo == -1) {
				daar = -gamma3p * asonidop * velocidadp * 2 * (FDiametroTubo[ind] - FDiametroTubo[ind1]) * DeltaTiempo /
					   (diamep * FXref);
			}
			caracteristica += daar;
		}

		/* variacion debida al termino de friccion */
		/* --------------------------------------- */

		if(velocidadp != 0. && FCoefAjusFric != 0.) {

			double velabs = fabs(velocidadp);
			double f = 0;

			Colebrook(FFriccion, diamep, f, Rep);

			double dafric = signo * gamma1p * (1. + signo * gamma1p * velocidadp / asonidop) * f * FCoefAjusFric * pow3(
								velocidadp) * DeltaTiempo / (diamep * velabs);

			caracteristica += dafric;
		}

#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::Calculo_Caracteristica " << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::InicializaCaracteristicas(TCondicionContorno **BC) {
#ifdef usetry
	try {
#endif

		BC[FNodoIzq - 1]->PutLanda(FTuboCCNodoIzq, FAsonido0[0] + FGamma3[0] * FVelocidad0[0]);
		BC[FNodoIzq - 1]->PutBeta(FTuboCCNodoIzq, FAsonido0[0] - FGamma3[0] * FVelocidad0[0]);
		BC[FNodoIzq - 1]->PutEntropia(FTuboCCNodoIzq, FAsonido0[0] / pow(FPresion0[0], FGamma5[0]));

		BC[FNodoDer - 1]->PutLanda(FTuboCCNodoDer, FAsonido0[FNin - 1] + FGamma3[FNin - 1] * FVelocidad0[FNin - 1]);
		BC[FNodoDer - 1]->PutBeta(FTuboCCNodoDer, FAsonido0[FNin - 1] - FGamma3[FNin - 1] * FVelocidad0[FNin - 1]);
		BC[FNodoDer - 1]->PutEntropia(FTuboCCNodoDer, FAsonido0[FNin - 1] / pow(FPresion0[FNin - 1], FGamma5[FNin - 1]));
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::InicializaCaracteristicas tubo:" << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculaB() {
#ifdef usetry
	try {
#endif
		double v = 0., p = 0., f = 0., tgas = 0., g = 0., q = 0., diamemed = 0., Rm = 0., Rm1 = 0.;
		double Remed, gamma, gamma1, Rmed, himed, rhomed, twallmed, Vmed, H1, H2, Hmed, Amed, rhoAmed;

		for(int i = 0; i < FNin - 1; i++) {

			FTVD.Bvector[0][i] = 0.;

			FTVD.Bvector[1][i] = 0.;

			FTVD.Bvector[2][i] = 0.;

			if(FArea[i] != FArea[i + 1] || FCoefAjusFric != 0 || FCoefAjusTC != 0) {

				Rm = sqrtRhoA[i + 1] / sqrtRhoA[i + 1];
				Rm1 = Rm + 1;
				gamma = (Rm * FGamma[i + 1] + FGamma[i]) / Rm1;
				gamma1 = gamma - 1;
				// gamma2 = gamma1 / 2;

				Vmed = (Rm * FVelocidadDim[i + 1] + FVelocidadDim[i]) / Rm1;

				if(FArea[i] != FArea[i + 1] || FCoefAjusTC != 0) {

					H1 = 0.5 * FVelocidadDim[i] * FVelocidadDim[i] + FAsonidoDim[i] * FAsonidoDim[i] / gamma1;
					H2 = 0.5 * FVelocidadDim[i + 1] * FVelocidadDim[i + 1] + FAsonidoDim[i + 1] * FAsonidoDim[i + 1] / gamma1;
					Hmed = (Rm * H2 + H1) / Rm1;
					Amed = sqrt(gamma1 * (Hmed - 0.5 * Vmed * Vmed));
					rhomed = sqrt(Frho[i] * Frho[i + 1]);

				}
				if(FCoefAjusFric != 0 || FCoefAjusTC != 0) {
					rhoAmed = sqrtRhoA[i + 1] * sqrtRhoA[i + 1];
				}

				if(FArea[i] != FArea[i + 1]) {
					FTVD.Bvector[1][i] += rhomed * Amed * Amed / gamma * (FArea[i] - FArea[i + 1]);
				}

				if(FCoefAjusFric != 0) {
					Remed = 0.5 * (FRe[i] + FRe[i + 1]);
					if(Remed > 1e-6) {
						Colebrook(FFriccion, FDiametroD12[i], f, Remed);
						if(Vmed >= 0.)
							g = f * Vmed * Vmed * 2 / FDiametroD12[i] * FCoefAjusFric;
						else
							g = -f * Vmed * Vmed * 2 / FDiametroD12[i] * FCoefAjusFric;

						FTVD.Bvector[1][i] += FXref * g * rhoAmed;
					}
				}

				if(FCoefAjusTC != 0) {
					Rmed = 0.5 * (FRMezcla[i] + FRMezcla[i + 1]);
					himed = 0.5 * (Fhi[i] + Fhi[i + 1]);
					twallmed = 0.5 * (FTPTubo[0][i] + FTPTubo[0][i + 1]);

					tgas = Amed * Amed / gamma / Rmed;

					TransmisionCalor(tgas, FDiametroD12[i], q, himed, rhomed, twallmed);

					q = q * FCoefAjusTC;

					FTVD.Bvector[2][i] = -FXref * q * rhoAmed;

				}

			}

		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaB tubo:" << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculaBmen() {
#ifdef usetry
	try {
#endif
		double v, p, f, tgas, g, q, diamemed, B, Rm, Rm1, gamma, gamma1, Vmed, Amed, H1, H2, Hmed, rhoAmed;
		double Remed = 0., Rmed = 0., himed = 0., rhomed = 0., twallmed = 0.;

		for(int i = 1; i < FNin; i++) {

			FTVD.Bmen[0][i] = 0.;

			FTVD.Bmen[1][i] = 0.;

			FTVD.Bmen[2][i] = 0.;

			if(FArea[i] != FArea12[i - 1] || FCoefAjusFric != 0 || FCoefAjusTC != 0) {

				rhoAmed = sqrt(sqrtRhoA[i - 1] * pow3(sqrtRhoA[i]));
				B = FU0[0][i] + rhoAmed + sqrtRhoA[i] * sqrtRhoA[i - 1];
				Rm = B / sqrt(pow3(sqrtRhoA[i - 1]) * sqrtRhoA[i]);
				Rm1 = Rm + 1;
				gamma = (Rm * FGamma[i] + FGamma[i - 1]) / Rm1;
				gamma1 = gamma - 1;
				// gamma2 = gamma1 / 2;

				Vmed = (Rm * FVelocidadDim[i] + FVelocidadDim[i - 1]) / Rm1;

				if(FArea[i] != FArea12[i - 1] || FCoefAjusTC != 0) {

					H1 = 0.5 * FVelocidadDim[i] * FVelocidadDim[i] + FAsonidoDim[i] * FAsonidoDim[i] / gamma1;
					H2 = 0.5 * FVelocidadDim[i - 1] * FVelocidadDim[i - 1] + FAsonidoDim[i - 1] * FAsonidoDim[i - 1] / gamma1;
					Hmed = (Rm * H1 + H2) / Rm1;
					Amed = sqrt(gamma1 * (Hmed - 0.5 * Vmed * Vmed));
					rhomed = sqrt(Frho[i] * sqrt(Frho[i] * Frho[i - 1]));

				}
				if(FCoefAjusFric != 0 || FCoefAjusTC != 0) {

					diamemed = 0.75 * FDiametroTubo[i] + 0.25 * FDiametroTubo[i - 1];
				}

				if(FArea[i] != FArea12[i - 1]) {
					FTVD.Bmen[1][i] += rhomed * Amed * Amed / gamma * (FArea12[i - 1] - FArea[i]);
				}

				if(FCoefAjusFric != 0) {
					Remed = (Rm * FRe[i] + FRe[i - 1]) / Rm1;
					if(Remed > 1e-6) {
						Colebrook(FFriccion, diamemed, f, Remed);
						if(Vmed >= 0.)
							g = f * Vmed * Vmed * 2 / diamemed * FCoefAjusFric;
						else
							g = -f * Vmed * Vmed * 2 / diamemed * FCoefAjusFric;

						FTVD.Bmen[1][i] += FXref * g * rhoAmed;
					}
				}

				if(FCoefAjusTC != 0) {
					Rmed = (Rm * FRMezcla[i] + FRMezcla[i - 1]) / Rm1;
					himed = (Rm * Fhi[i] + Fhi[i - 1]) / Rm1;
					twallmed = (Rm * FTPTubo[0][i] + FTPTubo[0][i - 1]) / Rm1;

					tgas = Amed * Amed / gamma / Rmed;

					TransmisionCalor(tgas, diamemed, q, himed, rhomed, twallmed);

					q = q * FCoefAjusTC;

					FTVD.Bmen[2][i] = -FXref * q * rhoAmed;

				}

			}
		}

#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaBmen tubo:" << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculaBmas() {
#ifdef usetry
	try {
#endif
		double v, p, f, tgas, g, q, diamemed, B, Rm, Rm1, gamma, gamma1, Vmed, Amed, H1, H2, Hmed, rhoAmed;
		double Remed = 0., Rmed = 0., himed = 0., rhomed = 0., twallmed = 0.;

		for(int i = 0; i < FNin - 1; i++) {

			FTVD.Bmas[0][i] = 0.;

			FTVD.Bmas[1][i] = 0.;

			FTVD.Bmas[2][i] = 0.;

			if(FArea[i] != FArea12[i] || FCoefAjusFric != 0 || FCoefAjusTC != 0) {

				rhoAmed = sqrt(sqrtRhoA[i + 1] * pow3(sqrtRhoA[i]));
				B = FU0[0][i] + rhoAmed + sqrtRhoA[i] * sqrtRhoA[i + 1];
				Rm = B / sqrt(pow3(sqrtRhoA[i + 1]) * sqrtRhoA[i]);
				Rm1 = Rm + 1;
				gamma = (Rm * FGamma[i] + FGamma[i + 1]) / Rm1;
				gamma1 = gamma - 1;
				// gamma2 = gamma1 / 2;

				Vmed = (Rm * FVelocidadDim[i] + FVelocidadDim[i + 1]) / Rm1;

				if(FArea[i] != FArea12[i] || FCoefAjusTC != 0) {

					H1 = 0.5 * FVelocidadDim[i] * FVelocidadDim[i] + FAsonidoDim[i] * FAsonidoDim[i] / gamma1;
					H2 = 0.5 * FVelocidadDim[i + 1] * FVelocidadDim[i + 1] + FAsonidoDim[i + 1] * FAsonidoDim[i + 1] / gamma1;
					Hmed = (Rm * H1 + H2) / Rm1;
					Amed = sqrt(gamma1 * (Hmed - 0.5 * Vmed * Vmed));
					rhomed = sqrt(Frho[i] * sqrt(Frho[i] * Frho[i + 1]));

				}
				if(FCoefAjusFric != 0 || FCoefAjusTC != 0) {

					diamemed = 0.75 * FDiametroTubo[i] + 0.25 * FDiametroTubo[i + 1];
				}

				if(FArea[i] != FArea12[i]) {
					FTVD.Bmas[1][i] += rhomed * Amed * Amed / gamma * (FArea[i] - FArea12[i]);
				}

				if(FCoefAjusFric != 0) {
					Remed = (Rm * FRe[i] + FRe[i + 1]) / Rm1;
					if(Remed > 1e-6) {
						Colebrook(FFriccion, diamemed, f, Remed);
						if(Vmed >= 0.)
							g = f * Vmed * Vmed * 2 / diamemed * FCoefAjusFric;
						else
							g = -f * Vmed * Vmed * 2 / diamemed * FCoefAjusFric;

						FTVD.Bmas[1][i] += FXref * g * rhoAmed;
					}
				}

				if(FCoefAjusTC != 0) {
					Rmed = (Rm * FRMezcla[i] + FRMezcla[i + 1]) / Rm1;
					himed = (Rm * Fhi[i] + Fhi[i + 1]) / Rm1;
					twallmed = (Rm * FTPTubo[0][i] + FTPTubo[0][i + 1]) / Rm1;

					tgas = Amed * Amed / gamma / Rmed;

					TransmisionCalor(tgas, diamemed, q, himed, rhomed, twallmed);

					q = q * FCoefAjusTC;

					FTVD.Bmas[2][i] = -FXref * q * rhoAmed;

				}

			}

		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaBmas tubo:" << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::CalculaMatrizJacobiana() {
#ifdef usetry
	try {
#endif
		double Rmed = 0., Vmed = 0., Vmed2 = 0., Hmed = 0., Amed = 0., Amed2 = 0., gamma = 0., H1 = 0., H2 = 0., Rmed1 = 0.;
		double *Ymed, gamma1, gamma2, gaAmed2;

		Ymed = new double[FNumeroEspecies - 1 - FIntEGR];

		for(int i = 0; i < FNin - 1; i++) {

			RoeConstants();

			// Calculo de las medias de Roe
			Rmed = sqrtRhoA[i + 1] / sqrtRhoA[i + 1];
			Rmed1 = Rmed + 1;
			gamma = (Rmed * FGamma[i + 1] + FGamma[i]) / Rmed1;
			gamma1 = gamma - 1;
			gamma2 = gamma1 / 2;
			H1 = 0.5 * FVelocidadDim[i] * FVelocidadDim[i] + FAsonidoDim[i] * FAsonidoDim[i] / gamma1;
			H2 = 0.5 * FVelocidadDim[i + 1] * FVelocidadDim[i + 1] + FAsonidoDim[i + 1] * FAsonidoDim[i + 1] / gamma1;
			Vmed = (Rmed * FVelocidadDim[i + 1] + FVelocidadDim[i]) / Rmed1;
			Vmed2 = Vmed * Vmed;
			Hmed = (Rmed * H2 + H1) / Rmed1;
			Amed2 = gamma1 * (Hmed - 0.5 * Vmed2);
			Amed = sqrt(Amed2);
			for(int j = 0; j < FNumeroEspecies - 1 - FIntEGR; j++) {
				Ymed[j] = (Rmed * FFraccionMasicaEspecie[i + 1][j] + FFraccionMasicaEspecie[i][j]) / Rmed1;
			}

			FTVD.Pmatrix[1][0][i] = Vmed - Amed;
			FTVD.Pmatrix[1][1][i] = Vmed;
			FTVD.Pmatrix[1][2][i] = Vmed + Amed;

			FTVD.Pmatrix[2][0][i] = Hmed - Vmed * Amed;
			FTVD.Pmatrix[2][1][i] = Vmed2 / 2.;
			FTVD.Pmatrix[2][2][i] = Hmed + Vmed * Amed;

			gaAmed2 = gamma2 / Amed2;
			// Calculo de la matriz Q (autovectores a la derecha)
			FTVD.Qmatrix[0][0][i] = (Vmed / Amed + gaAmed2 * Vmed2) / 2.;
			FTVD.Qmatrix[0][1][i] = -0.5 / Amed - gaAmed2 * Vmed;
			FTVD.Qmatrix[0][2][i] = gaAmed2;

			FTVD.Qmatrix[1][0][i] = 1. - gaAmed2 * Vmed2;
			FTVD.Qmatrix[1][1][i] = gamma1 * Vmed / Amed2;
			FTVD.Qmatrix[1][2][i] = -gamma1 / Amed2;

			FTVD.Qmatrix[2][0][i] = -(Vmed / Amed + gaAmed2 * Vmed2) / 2.;
			FTVD.Qmatrix[2][1][i] = 0.5 / Amed - gaAmed2 * Vmed;
			FTVD.Qmatrix[2][2][i] = gaAmed2;

			// Valores propios de la matriz jacobiana
			FTVD.Alpha[0][i] = FTVD.Pmatrix[1][0][i];
			FTVD.Alpha[1][i] = Vmed;
			FTVD.Alpha[2][i] = FTVD.Pmatrix[1][2][i];

			for(int j = 3; j < FNumEcuaciones; j++) {
				FTVD.Alpha[j][i] = (Vmed);

			}
		}
		delete[] Ymed;
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::CalculaMatrizJacobiana tubo:" << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::TVD_Estabilidad() {
#ifdef usetry
	try {
#endif
		double VTotalMax = 0.;
		double VLocal = 0., DeltaT_tvd = 0.;

		CalculaFlujo(FU0, FTVD.W, FGamma, FGamma1, FNin);

		CalculaMatrizJacobiana();

		CalculaB();

		for(int i = 0; i < FNin - 1; i++) {
			for(int k = 0; k < 3; ++k) {
				FTVD.DeltaU[k][i] = FTVD.Qmatrix[k][0][i] * (FU0[0][i + 1] - FU0[0][i]) + FTVD.Qmatrix[k][1][i] *
									(FU0[1][i + 1] - FU0[1][i]) + FTVD.Qmatrix[k][2][i] * (FU0[2][i + 1] - FU0[2][i]);
				FTVD.DeltaB[k][i] = FTVD.Qmatrix[k][0][i] * FTVD.Bvector[0][i] + FTVD.Qmatrix[k][1][i] * FTVD.Bvector[1][i] +
									FTVD.Qmatrix[k][2][i] * FTVD.Bvector[2][i];
				FTVD.DeltaW[k][i] = FTVD.Qmatrix[k][0][i] * (FTVD.W[0][i + 1] - FTVD.W[0][i] + FTVD.Bvector[0][i]) +
									FTVD.Qmatrix[k][1][i] * (FTVD.W[1][i + 1] - FTVD.W[1][i] + FTVD.Bvector[1][i]) + FTVD.Qmatrix[k][2][i] *
									(FTVD.W[2][i + 1] - FTVD.W[2][i] + FTVD.Bvector[2][i]);
				if(fabs(FTVD.DeltaU[k][i]) < 1e-3) {
					FTVD.Beta[k][i] = FTVD.DeltaB[k][i] / 1e-3;
				} else {
					FTVD.Beta[k][i] = FTVD.DeltaB[k][i] / FTVD.DeltaU[k][i];
				}
				// if (FNumeroTubo == 63){
				// printf("%d ",k);
				// printf("%lf ", FTVD.DeltaB[k][i] );
				// printf("%lf ", FTVD.DeltaU[k][i] );
				// if(FTVD.DeltaU[k][i]!=0) printf("%lf ",FTVD.DeltaB[k][i] / FTVD.DeltaU[k][i] );
				// printf("\n ");
				// }
				if(FTVD.Alpha[k][i] + FTVD.Beta[k][i] != 0) {
					if((VLocal = fabs(FTVD.Alpha[k][i]) + fabs(FTVD.Beta[k][i])) > VTotalMax) {
						VTotalMax = VLocal;
					}
				}
			}
			for(int k = 3; k < FNumEcuaciones; k++) {
				FTVD.Beta[k][i] = 0.;
			}
		}
		DeltaT_tvd = FCourant * FXref / VTotalMax;
		if(DeltaT_tvd < FDeltaTime)
			FDeltaTime = DeltaT_tvd;
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::TVD_Estabilidad tubo:" << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::TVD_Limitador() {
#ifdef usetry
	try {
#endif
		double dtdx = FDeltaTime / FXref;

		CalculaBmas();

		CalculaBmen();

		for(int i = 0; i < FNin - 1; ++i) {
			// Calculo de las variables en el sistema de coordenadas Q
			for(int k = 0; k < FNumEcuaciones; k++) {
				FTVD.LandaD[k][i] = dtdx * (FTVD.Alpha[k][i] + FTVD.Beta[k][i]);
				if(FTVD.LandaD[k][i] >= 0.) {
					FTVD.hLandaD[k][i] = 1;
				} else {
					FTVD.hLandaD[k][i] = -1;
				}
			}
		}
		for(int i = 1; i < FNin - 2; ++i) {
			for(int k = 0; k < 3; k++) {
				double den = (((double) FTVD.hLandaD[k][i] - FTVD.LandaD[k][i]) * FTVD.DeltaW[k][i]);
				double num = ((double) FTVD.hLandaD[k][i - FTVD.hLandaD[k][i]] - FTVD.LandaD[k][i - FTVD.hLandaD[k][i]]) *
							 (FTVD.DeltaW[k][i - FTVD.hLandaD[k][i]]);
				if(fabs(den) > 1e-10)
					FTVD.R[k][i] = num / den;
				else
					FTVD.R[k][i] = 1;
			}

			for(int k = 3; k < FNumEcuaciones; k++) {
				double num = ((double) FTVD.hLandaD[k][i - FTVD.hLandaD[k][i]] - FTVD.LandaD[k][i - FTVD.hLandaD[k][i]]) *
							 (FTVD.W[k][i + 1 - FTVD.hLandaD[k][i]] - FTVD.W[k][i - FTVD.hLandaD[k][i]]);
				double den = ((double) FTVD.hLandaD[k][i] - FTVD.LandaD[k][i]) * (FTVD.W[k][i + 1] - FTVD.W[k][i]);
				if(fabs(den) > 1e-10)
					FTVD.R[k][i] = num / den;
				else
					FTVD.R[k][i] = 1;
			}
		}
		for(int k = 0; k < FNumEcuaciones; k++) {
			FTVD.R[k][0] = FTVD.R[k][1];
			FTVD.R[k][FNin - 2] = FTVD.R[k][FNin - 3];
		}
		for(int i = 0; i < FNin - 1; ++i) {
			for(int k = 0; k < 3; k++) {
				FTVD.Phi[k][i] = (double) FTVD.hLandaD[k][i] - Limita(FTVD.R[k][i]) * ((double) FTVD.hLandaD[k][i] - FTVD.LandaD[k][i]);
			}
		}

		for(int i = 0; i < FNin - 1; ++i) {
			for(int k = 0; k < 3; ++k) {
				FTVD.gflux[k][i] = 0.5 * (FTVD.W[k][i] + FTVD.W[k][i + 1] - FTVD.Bmas[k][i] + FTVD.Bmen[k][i + 1] -
										  (FTVD.Pmatrix[k][0][i] * FTVD.Phi[0][i] * FTVD.DeltaW[0][i] + FTVD.Pmatrix[k][1][i] * FTVD.Phi[1][i] * FTVD.DeltaW[1][i]
										   + FTVD.Pmatrix[k][2][i] * FTVD.Phi[2][i] * FTVD.DeltaW[2][i]));
			}
			for(int k = 3; k < FNumEcuaciones; k++) {
				FTVD.gflux[k][i] = 0.5 * (FTVD.W[k][i] + FTVD.W[k][i + 1] - (double) FTVD.hLandaD[k][i] *
										  (FTVD.W[k][i + 1] - FTVD.W[k][i])) + 0.5 * Limita(FTVD.R[k][i]) * ((double) FTVD.hLandaD[k][i] - FTVD.LandaD[k][i]) *
								   (FTVD.W[k][i + 1] - FTVD.W[k][i]);
			}

		}
		for(int i = 1; i < FNin - 1; ++i) {
			for(int k = 0; k < FNumEcuaciones; k++) {
				FU1[k][i] = FU0[k][i] - dtdx * ((FTVD.gflux[k][i] - FTVD.gflux[k][i - 1]) + (FTVD.Bmen[k][i] + FTVD.Bmas[k][i]));
			}
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::TVD_Limitador tubo:" << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

inline double TTubo::Limita(double r) {
	double ret_val = 0.;
	// ------Van Albada
	// double ret_val=(r*r+r)/(1+r*r);

	// ------Van Leer

#ifdef __BORLANDC__
	if(r != r)
		ret_val = 1;
	else
		ret_val = (fabs(r) + r) / (1. + fabs(r));

	if(ret_val != ret_val)
		ret_val = 1.;
#else
	if(std::isnan(r)) {
		if(signbit(r))
			ret_val = 1;
		else
			ret_val = 1;
	} else
		ret_val = (fabs(r) + r) / (1. + fabs(r));

	if(std::isnan(ret_val)) {
		if(signbit(r))
			ret_val = 1;
		else
			ret_val = 1;
	}
#endif

	// ------Minmod
	// double ret_val=Minimo(2*fabs(r),1.);
	// double ret_val1=Minimo(2*r,1.);
	// double ret_val=Maximo(0,ret_val1);

	// ------Limitador de Roe (minmod)
	// double ret_val1=Minimo(r,1);
	// double ret_val=Maximo(0,ret_val1);

	// ------Limitador Roe Superbee
	// double ret_val1=Minimo(2*r,1);
	// double ret_val2=Minimo(r,2);
	// double ret_val3=Maximo(0,ret_val1);
	// double ret_val=Maximo(ret_val2,ret_val3);

	// ------Limitador de Osher (phi=1.5)
	// double ret_val1=Minimo(r,2);
	// double ret_val=Maximo(0,ret_val1);

	// ------Limitador de Sweby (phi=1.5)
	// double ret_val1=Minimo(2*r,1);
	// double ret_val2=Minimo(r,2);
	// double ret_val3=Maximo(0,ret_val1);
	// double ret_val=Maximo(ret_val2,ret_val3);

	// ------Limitador de Ospre (WAterson & Deconinck)
	// double ret_val=1.5*(r*r+r)/(r*r+r+1.);

	// ------Limitador de MC de van Leer
	// double ret_val1=Minimo(0.5+0.5*r,2);
	// double ret_val2=Minimo(ret_val1,2*r);
	// double ret_val=Maximo(0,ret_val2);

	// ------Limitador de UMIST
	// double ret_val1=Minimo(0.75+0.25*r,2);
	// double ret_val2=Minimo(ret_val1,0.25+0.75*r);
	// double ret_val3=Minimo(ret_val2,2*r);
	// double ret_val=Maximo(0,ret_val3);

	// ------Limitador de Koren
	// double ret_val1=Minimo((1+2*r)/3,2);
	// double ret_val2=Minimo(ret_val1,2*r);
	// double ret_val=Maximo(0,ret_val2);

	return ret_val;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TTubo::DimensionaTVD() {
#ifdef usetry
	try {
#endif

		FTVD.Bmas = new double*[FNumEcuaciones];
		FTVD.Bvector = new double*[FNumEcuaciones];
		FTVD.Bmen = new double*[FNumEcuaciones];
		FTVD.gflux = new double*[FNumEcuaciones];
		FTVD.Alpha = new double*[FNumEcuaciones];
		FTVD.Beta = new double*[FNumEcuaciones];
		FTVD.DeltaU = new double*[FNumEcuaciones];
		FTVD.DeltaB = new double*[FNumEcuaciones];
		FTVD.DeltaW = new double*[FNumEcuaciones];
		FTVD.hLandaD = new int*[FNumEcuaciones];
		FTVD.LandaD = new double*[FNumEcuaciones];
		FTVD.Phi = new double*[FNumEcuaciones];
		FTVD.R = new double*[FNumEcuaciones];
		FTVD.W = new double*[FNumEcuaciones];
		FTVD.Qmatrix = new double**[FNumEcuaciones];
		FTVD.Pmatrix = new double**[FNumEcuaciones];

		sqrtRhoA = new double[FNin];

		for(int k = 0; k < FNumEcuaciones; ++k) {
			FTVD.Bmas[k] = new double[FNin];
			FTVD.Bvector[k] = new double[FNin];
			FTVD.Bmen[k] = new double[FNin];
			FTVD.gflux[k] = new double[FNin];
			FTVD.Alpha[k] = new double[FNin];
			FTVD.Beta[k] = new double[FNin];
			FTVD.DeltaU[k] = new double[FNin];
			FTVD.DeltaB[k] = new double[FNin];
			FTVD.DeltaW[k] = new double[FNin];
			FTVD.hLandaD[k] = new int[FNin];
			FTVD.LandaD[k] = new double[FNin];
			FTVD.Phi[k] = new double[FNin];
			FTVD.R[k] = new double[FNin];
			FTVD.W[k] = new double[FNin];
		}

		for(int k = 0; k < FNumEcuaciones; ++k) {
			FTVD.Qmatrix[k] = new double*[FNumEcuaciones];
			FTVD.Pmatrix[k] = new double*[FNumEcuaciones];

			for(int i = 0; i < FNumEcuaciones; i++) {
				FTVD.Qmatrix[k][i] = new double[FNin];
				FTVD.Pmatrix[k][i] = new double[FNin];
			}
		}
		for(int i = 0; i < FNin; i++) {

			for(int k = 3; k < FNumEcuaciones; k++) {
				for(int j = 3; j < FNumEcuaciones; j++) {
					FTVD.Pmatrix[k][j][i] = 0.;
				}
			}
			FTVD.Pmatrix[0][0][i] = 1.;
			FTVD.Pmatrix[0][1][i] = 1.;
			FTVD.Pmatrix[0][2][i] = 1.;
			for(int j = 3; j < FNumEcuaciones; j++) {
				FTVD.Pmatrix[j][j][i] = 1.;
			}

			for(int k = 3; k < FNumEcuaciones; k++) {
				for(int j = 3; j < FNumEcuaciones; j++) {
					FTVD.Qmatrix[k][j][i] = 0.;
				}
			}
			for(int j = 3; j < FNumEcuaciones; j++) {
				FTVD.Qmatrix[j][j][i] = 1.;
			}
		}
#ifdef usetry
	} catch(exception & N) {
		std::cout << "ERROR: TTubo::TVD_Limitador tubo:" << FNumeroTubo << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
#endif
}

void TTubo::RoeConstants() {

	sqrtRhoA[0] = sqrt(FU1[0][0]);
	for(int i = 0; i < FNin - 1; i++) {
		sqrtRhoA[i + 1] = sqrt(FU1[0][i + 1]);
	}
}

double TTubo::GetArea(int i) const {
	return FArea[i];
}

double TTubo::GetAsonido(int i) const {
	return FAsonido0[i];
}

double TTubo::GetCoefTurbulencia(int i) const {
	return FCoefTurbulencia[i];
}

double TTubo::GetCpMezcla(int i) const {
	return FCpMezcla[i];
}

double TTubo::GetCvMezcla(int i) const {
	return FCvMezcla[i];
}

double TTubo::getDeltaTime() const {
	return FDeltaTime;
}

double TTubo::GetDensidad(int i) const {
	return Frho[i];
}

double TTubo::GetDiametro(int i) const {
	return FDiametroTubo[i];
}

double TTubo::GetFraccionMasicaInicial(int i) const {
	return FComposicionInicial[i];
}

double TTubo::GetGamma(int i) const {
	return FGamma[i];
}

double TTubo::getLongitudTotal() const {
	return FLongitudTotal;
}

double TTubo::getMallado() const {
	return FMallado;
}

int TTubo::getNodoDer() const {
	return FNodoDer;
}

int TTubo::getNodoIzq() const {
	return FNodoIzq;
}

int TTubo::getNumeroTubo() const {
	return FNumeroTubo;
}

double TTubo::GetPresion(int i) const {
	return FPresion0[i];
}

double TTubo::getPresionInicial() const {
	return FPini;
}

double TTubo::GetRMezcla(int i) const {
	return FRMezcla[i];
}

double TTubo::getTemperaturaInicial() const {
	return FTini;
}

double TTubo::getTempWallIni() const {
	return FTIniParedTub;
}

double TTubo::getTime0() const {
	return FTime0;
}

double TTubo::getTime1() const {
	return FTime1;
}

double TTubo::GetTPTubo(int j, int i) const {
	// j is the wall node and i is the cell
	return FTPTubo[j][i];
}

double TTubo::GetTPTuboAnt(int j, int i) const {
	return FTParedAnt[j][i];
}

double TTubo::GetVelocidad(int i) const {
	return FVelocidad0[i];
}

double TTubo::getVelocidadMedia() const {
	return FVelMedia;
}

double TTubo::GetVelPro(int i) const {
	return FVelPro[i];
}

void TTubo::PutDeltaTime(double valor) {
	FDeltaTime = valor;
}

void TTubo::PutTime0(double valor) {
	FTime0 = valor;
}

void TTubo::PutTime1(double valor) {
	FTime1 = valor;
}

void TTubo::PutTPTubo(int k, int i, double valor) {
	FTPTubo[k][i] = valor;
}

void TTubo::PutVelPro(int i, double valor) {
	FVelPro[i] = valor;
}

#pragma package(smart_init)
