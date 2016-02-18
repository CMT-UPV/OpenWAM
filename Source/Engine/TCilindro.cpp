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

// ---------------------------------------------------------------------------
#pragma hdrstop

#include "TCilindro.h"
#include "TBloqueMotor.h"
#include "TTubo.h"
#include "TCCCilindro.h"
#include "TValvula4T.h"
#include "TLumbrera.h"

//#include <cmath>

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCilindro::TCilindro(TBloqueMotor *Engine, bool ThereIsEGR) {

	FMotor = Engine;

	FAngDesplazado = NULL;
	FTempPared = NULL;
	FTrabajoNeto = 0.;
	FSwirlSUM = 0.;

	FHayEGR = ThereIsEGR;
	if(FHayEGR)
		FIntEGR = 0;
	else
		FIntEGR = 1;

	FResInstantCilindro.FraccionINS = NULL;
	FResInstantCilindro.MomentoAngularEscINS = NULL;
	FResInstantCilindro.MomentoAngularAdmINS = NULL;
	FResInstantCilindro.GastoEscINS = NULL;
	FResInstantCilindro.GastoAdmINS = NULL;
	FResInstantCilindro.MachEscINS = NULL;
	FResInstantCilindro.MachAdmINS = NULL;
	FResInstantCilindro.SeccionEfectivaEscINS = NULL;
	FResInstantCilindro.SeccionEfectivaAdmINS = NULL;
	FResInstantCilindro.NITINS = NULL;
	FResMediosCilindro.NITMED = NULL;

	FCCValvulaAdm = NULL;
	FCCValvulaEsc = NULL;

	FValvEsc = NULL;
	FValvAdm = NULL;

	FSaturado = false;

	FInyeccion = false;

	FHaydll = false;
	FModComb = false;

	FIniComb = 0.;
	FFinComb = 0.;

	FMfControlled = false;

	if(FMotor->getACT()) {
		Fengine_parameters = (double*) malloc(25 * sizeof(double));
		Fengine_parameters[0] = FMotor->getGeometria().Diametro;
		// Piston diameter
		Fengine_parameters[1] = FMotor->getGeometria().Carrera; // Piston stroke
		Fengine_parameters[2] = FMotor->getGeometria().Carrera / 2.;
		// Crank length
		Fengine_parameters[3] = FMotor->getGeometria().Biela;
		// Connecting Rod Length
		Fengine_parameters[4] = FMotor->getGeometria().Excentricidad;
		// Piston Eccentricity
		Fengine_parameters[5] = FMotor->getGeometria().DiametroBulon;
		// Piston Axis Diameter
		Fengine_parameters[6] = FMotor->getGeometria().AlturaCoronaPiston;
		// Piston Crown Height
		Fengine_parameters[7] = FMotor->getGeometria().DiametroBowl;
		// Maximum Diameter of Bowl
		Fengine_parameters[8] = FMotor->getGeometria().DiametroBowl * FMotor->getGeometria().DiametroBowl * __cons::Pi_4 *
								FMotor->getGeometria().AlturaBowl;
		// Volume of Bowl
		Fengine_parameters[9] = FMotor->getGeometria().MasaBiela;
		// Mass connecting rod
		Fengine_parameters[10] = FMotor->getGeometria().MasaPistonSegmentosBulon;
		// Mass of piston + rings + piston axis
		Fengine_parameters[11] = FMotor->getGeometria().ModuloElasticidad;
		// Elasticity steel module
		Fengine_parameters[12] = FMotor->getGeometria().CoefDeformaciones;
		// Mechanical deformations coefficient
		Fengine_parameters[13] = FMotor->getGeometria().AreaBlowBy;
		// Coefficient of leak's section of blow-by
		Fengine_parameters[14] = FMotor->getGeometria().RelaCompresion;
		// Geometric compression ratio
		Fengine_parameters[15] = FMotor->getInjectionSys().HoleNumber;
		// Number of nozzle holes
		Fengine_parameters[16] = FMotor->getInjectionSys().HoleDiame;
		// Nozzle diameter
		Fengine_parameters[17] = FMotor->getInjectionSys().CDHole;
		// Discharge coefficient of the nozzle
		Fengine_parameters[18] = 2.15; // CTM
		Fengine_parameters[19] = FMotor->getWoschni().cw1;
		// Constant A to calculate CW1
		Fengine_parameters[20] = FMotor->getWoschni().cw2;
		// Constant B to calculate CW1
		Fengine_parameters[21] = 0.001; // Woshni calculus coefficient CW2
		Fengine_parameters[22] = FDistribucion.CA - 720.; // Inlet valve closing
		Fengine_parameters[23] = FDistribucion.AE; // Exhaust valve opening
		Fengine_parameters[24] = FMotor->getMixtureProcessCte();
		// Mixture process constant

		Fengine_model_constants = (double*) malloc(22 * sizeof(double));

		Fengine_model_constants[0] = -1.5; // Kmixture2
		Fengine_model_constants[1] = 13000; // Kpmx1
		Fengine_model_constants[2] = 500; // Kpmx2
		Fengine_model_constants[3] = 0.5; // Kpmx3
		Fengine_model_constants[4] = 0.5; // Kpmx4
		Fengine_model_constants[5] = 0.22; // Kpmx5
		Fengine_model_constants[6] = 1; // KID1
		Fengine_model_constants[7] = 0.03; // KID2
		Fengine_model_constants[8] = 5.47896; // KID3
		Fengine_model_constants[9] = 0.118895; // KID4
		Fengine_model_constants[10] = 0.0684351; // KID5
		Fengine_model_constants[11] = 1.23; // KNOx1
		Fengine_model_constants[12] = 0.203644; // KNOx2
		Fengine_model_constants[13] = 1; // KNOx_EC
		Fengine_model_constants[14] = 0.0023326; // KSOOTA1
		Fengine_model_constants[15] = 0.5; // KSOOTA2
		Fengine_model_constants[16] = 6296.9; // KSOOTA3
		Fengine_model_constants[17] = 1.8197; // KSOOTA4
		Fengine_model_constants[18] = 1; // KSOOTA5
		Fengine_model_constants[19] = 1.8; // KSOOTA6
		Fengine_model_constants[20] = 7050; // KSOOTA7
		Fengine_model_constants[21] = 0.01; // PMlhp_error

		Ftest_variables = (double*) malloc(18 * sizeof(double));

		Ftest_variables[0] = FMotor->getRegimen();
		Ftest_variables[1] = FMotor->getMasaInicial() * 1000;
		// Measured air mass
		Ftest_variables[2] = FMotor->getMasaInicial() * 1000;
		// In-cylinder air mass at inlet valve closing
		Ftest_variables[3] = 372.434622123441; // In-cylinder temperature at inlet valve closing
		Ftest_variables[4] = FMotor->getMasaFuel() * 1e6; // Fuel injected mass
		Ftest_variables[5] = FMotor->getInjectionSys().InjectPressure;
		// Injection pressure
		Ftest_variables[6] = 170100; // Inlet pressure
		Ftest_variables[7] = 188300; // Exhaust pressure
		Ftest_variables[8] = FMotor->getGeometria().CDBlowBy;
		// Blow-by coefficient
		Ftest_variables[9] = __units::BarToPa(FMotor->getPresionAmb());
		// Atmosphere pressure
		Ftest_variables[10] = 318.15; // Fuel injection temperature
		Ftest_variables[11] = 390.85; // Cylinder head temperature
		Ftest_variables[12] = 383.45; // Cylinder temperature
		Ftest_variables[13] = 457.65; // Piston temperature
		Ftest_variables[14] = 0; // NOx concentration at IVC
		Ftest_variables[15] = 0; // SOOT concentration at IVC
		Ftest_variables[16] = 0; // CO concentration at IVC
		Ftest_variables[17] = 0; // HC concentration at IVC

		Fsize_inlet_inj = 1801;
		FCAI = 1801;

		Finjection_rate = (double*) malloc(FCAI * sizeof(double));

		FCAD_injection_rate = (double*) malloc(FCAI * sizeof(double));

		FCAD_injection_rate[0] = -180;
		for(int counter = 1; counter < FCAI; counter++) {
			FCAD_injection_rate[counter] = FCAD_injection_rate[counter - 1] + ((360. / (FCAI - 1)));
		}

		FCAD_exit = (double*) malloc(FCAI * sizeof(double));

		FHRF_exit = (double*) malloc(FCAI * sizeof(double));

		FROHR_exit = (double*) malloc(FCAI * sizeof(double));

		Fp_cyl_exit = (double*) malloc(FCAI * sizeof(double));

		Fdp_da_cyl_exit = (double*) malloc(FCAI * sizeof(double));

		FT_cyl_exit = (double*) malloc(FCAI * sizeof(double));

		Finjection_rate_exit = (double*) malloc(FCAI * sizeof(double));

		Faccum_injection_rate_exit = (double*) malloc(FCAI * sizeof(double));

		FH_cooler_exit = (double*) malloc(FCAI * sizeof(double));

		Fmean_var_exit = (double*) malloc(11 * sizeof(double));

		Fheat_transfer = (double*) malloc(4 * sizeof(double));

		dataOUT.evol_Soot = (double*) malloc(FCAI * sizeof(double));
		dataOUT.evol_Soot_CIL = (double*) malloc(FCAI * sizeof(double));
		dataOUT.evol_Radiacion = (double*) malloc(FCAI * sizeof(double));
		dataOUT.HRF_PMX = (double*) malloc(FCAI * sizeof(double));
		dataOUT.evol_LOL = (double*) malloc(FCAI * sizeof(double));

		dataIN.KSOOTC1 = 1.2771738e-3;

		FIN = FMotor->getInjectionSys().NumPulsos;

		FSOP = (double*) malloc(8 * sizeof(double));

		for(int i = 0; i < 8; i++) {
			if(i < FIN) {
				FSOP[i] = FMotor->getInjecPulse(i).Angulo;
			} else {
				FSOP[i] = 180.;
			}
		}

		FMFI = (double*) malloc(8 * sizeof(double));

		for(int i = 0; i < 8; i++) {
			if(i < FIN) {
				FMFI[i] = FMotor->getInjecPulse(i).Masa * FMotor->getMasaFuel() * 1e6;
			} else {
				FMFI[i] = 0.;
			}
		}

		FSOI = (double*) malloc(8 * sizeof(double));
		FEOI = (double*) malloc(8 * sizeof(double));

		FA_TASA = (double*) malloc(2 * sizeof(double));

		FA_TASA[0] = FMotor->getInjectionSys().PendOpen_A1;
		FA_TASA[1] = FMotor->getInjectionSys().PendOpen_A2;

		FB_TASA = (double*) malloc(2 * sizeof(double));

		FB_TASA[0] = FMotor->getInjectionSys().LevMax_B1;
		FB_TASA[1] = FMotor->getInjectionSys().LevMax_B2;

		FC_TASA = (double*) malloc(2 * sizeof(double));

		FC_TASA[0] = FMotor->getInjectionSys().PendClose_C1;
		FC_TASA[1] = FMotor->getInjectionSys().PendClose_C2;

		FD_TASA = (double*) malloc(2 * sizeof(double));

		FD_TASA[0] = FMotor->getInjectionSys().Desfase_D1;
		FD_TASA[1] = FMotor->getInjectionSys().Desfase_D2;

		if(FMotor->getSpeciesNumber() == 9) {
			dataOUT.species_EVO[0] = FMotor->GetComposicionInicial(7); // N2
		} else if(FMotor->getSpeciesNumber() == 10) {
			dataOUT.species_EVO[0] = FMotor->GetComposicionInicial(8); // N2
		}
		dataOUT.species_EVO[1] = FMotor->GetComposicionInicial(0); // O2
		dataOUT.species_EVO[2] = FMotor->GetComposicionInicial(1); // CO2
		dataOUT.species_EVO[3] = FMotor->GetComposicionInicial(2); // H2O
		dataOUT.species_EVO[4] = FMotor->GetComposicionInicial(5); // NOx
		dataOUT.species_EVO[5] = FMotor->GetComposicionInicial(6); // CO
		dataOUT.species_EVO[6] = FMotor->GetComposicionInicial(4); // SOOT
		dataOUT.species_EVO[7] = FMotor->GetComposicionInicial(3); // HC

		FSpecies_IVC = (double*) malloc(8 * sizeof(double));

		if(FMotor->getSpeciesNumber() == 9) {
			FSpecies_IVC[0] = FMotor->GetComposicionInicial(7); // N2
		} else if(FMotor->getSpeciesNumber() == 10) {
			FSpecies_IVC[0] = FMotor->GetComposicionInicial(8); // N2
		}
		FSpecies_IVC[1] = FMotor->GetComposicionInicial(0); // O2
		FSpecies_IVC[2] = FMotor->GetComposicionInicial(1); // CO2
		FSpecies_IVC[3] = FMotor->GetComposicionInicial(2); // H2O
		FSpecies_IVC[4] = FMotor->GetComposicionInicial(5); // NOx
		FSpecies_IVC[5] = FMotor->GetComposicionInicial(6); // CO
		FSpecies_IVC[6] = FMotor->GetComposicionInicial(4); // SOOT
		FSpecies_IVC[7] = FMotor->GetComposicionInicial(3); // HC

		FPresionMedAdm = 0.;
		FPresionMedEsc = 0.;
		FTimeAcumAct = 0.;
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCilindro::~TCilindro() {

	if(FAngDesplazado != NULL)
		delete[] FAngDesplazado;
	if(FTempPared != NULL)
		delete[] FTempPared;

	if(FCCValvulaAdm != NULL)
		delete[] FCCValvulaAdm;
	if(FCCValvulaEsc != NULL)
		delete[] FCCValvulaEsc;

	if(FResInstantCilindro.FraccionINS != NULL)
		delete[] FResInstantCilindro.FraccionINS;
	if(FResInstantCilindro.MomentoAngularEscINS != NULL)
		delete[] FResInstantCilindro.MomentoAngularEscINS;
	if(FResInstantCilindro.MomentoAngularAdmINS != NULL)
		delete[] FResInstantCilindro.MomentoAngularAdmINS;
	if(FResInstantCilindro.GastoEscINS != NULL)
		delete[] FResInstantCilindro.GastoEscINS;
	if(FResInstantCilindro.GastoAdmINS != NULL)
		delete[] FResInstantCilindro.GastoAdmINS;
	if(FResInstantCilindro.MachEscINS != NULL)
		delete[] FResInstantCilindro.MachEscINS;
	if(FResInstantCilindro.MachAdmINS != NULL)
		delete[] FResInstantCilindro.MachAdmINS;
	if(FResInstantCilindro.SeccionEfectivaEscINS != NULL)
		delete[] FResInstantCilindro.SeccionEfectivaEscINS;
	if(FResInstantCilindro.SeccionEfectivaAdmINS != NULL)
		delete[] FResInstantCilindro.SeccionEfectivaAdmINS;
	if(FResInstantCilindro.NITINS != NULL)
		delete[] FResInstantCilindro.NITINS;
	if(FResMediosCilindro.NITMED != NULL)
		delete[] FResMediosCilindro.NITMED;

	if(FValvAdm != NULL)
		delete[] FValvAdm;
	if(FValvEsc != NULL)
		delete[] FValvEsc;

	if(FMotor->getACT()) {
		free(Fengine_parameters);
		free(Fengine_model_constants);
		free(Ftest_variables);
		free(Finjection_rate);
		free(FCAD_injection_rate);
		free(FCAD_exit);
		free(FHRF_exit);
		free(FROHR_exit);
		free(Fp_cyl_exit);
		free(Fdp_da_cyl_exit);
		free(FT_cyl_exit);
		free(FH_cooler_exit);
		free(Fmean_var_exit);
		free(Fheat_transfer);
		free(Finjection_rate_exit);
		free(Faccum_injection_rate_exit);
		free(FSOP);
		free(FMFI);
		free(FSOI);
		free(FEOI);
		free(FA_TASA);
		free(FB_TASA);
		free(FC_TASA);
		free(FD_TASA);
		free(FSpecies_IVC);
		free(dataOUT.evol_Soot);
		free(dataOUT.evol_Soot_CIL);
		free(dataOUT.evol_Radiacion);
		free(dataOUT.HRF_PMX);
		free(dataOUT.evol_LOL);
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro::AsignacionCC(TCondicionContorno **BC, int numCC) {
	try {
		bool UnionAsignada;

		/* Union Cylinder-Pipe */

		FNumeroUnionesAdm = 0;
		FNumeroUnionesEsc = 0;

		for(int i = 0; i < numCC; i++) {
			if(BC[i]->getTipoCC() == nmIntakeValve) {
				if(FNumeroCilindro == dynamic_cast<TCCCilindro*>(BC[i])->getNumeroCilindro()) {
					FNumeroUnionesAdm++;
				}
			} else if(BC[i]->getTipoCC() == nmExhaustValve) {
				if(FNumeroCilindro == dynamic_cast<TCCCilindro*>(BC[i])->getNumeroCilindro()) {
					FNumeroUnionesEsc++;
				}
			}
		}

		if(FNumeroUnionesAdm != 0)
			FCCValvulaAdm = new TCondicionContorno*[FNumeroUnionesAdm];
		if(FNumeroUnionesEsc != 0)
			FCCValvulaEsc = new TCondicionContorno*[FNumeroUnionesEsc];

		bool *CCasignadaAdm;
		CCasignadaAdm = new bool[numCC];
		for(int i = 0; i < numCC; i++)
			CCasignadaAdm[i] = false;

		for(int j = 0; j < FNumeroUnionesAdm; j++) {
			UnionAsignada = false;
			for(int i = 0; i < numCC; i++) {
				if(BC[i]->getTipoCC() == nmIntakeValve) {
					if(FNumeroCilindro == dynamic_cast<TCCCilindro*>(BC[i])->getNumeroCilindro() && !CCasignadaAdm[i] && !UnionAsignada) {
						FCCValvulaAdm[j] = BC[i];
						UnionAsignada = true;
						CCasignadaAdm[i] = true;
					}
				}
			}
		}

		delete[] CCasignadaAdm;

		bool *CCasignadaEsc;
		CCasignadaEsc = new bool[numCC];
		for(int i = 0; i < numCC; i++)
			CCasignadaEsc[i] = false;

		for(int j = 0; j < FNumeroUnionesEsc; j++) {
			UnionAsignada = false;
			for(int i = 0; i < numCC; i++) {
				if(BC[i]->getTipoCC() == nmExhaustValve) {
					if(FNumeroCilindro == dynamic_cast<TCCCilindro*>(BC[i])->getNumeroCilindro() && !CCasignadaEsc[i] && !UnionAsignada) {
						FCCValvulaEsc[j] = BC[i];
						UnionAsignada = true;
						CCasignadaEsc[i] = true;
					}
				}
			}
		}

		delete[] CCasignadaEsc;

		Fctorbadmp = CalculaCTorbAdmProm();
		FKctm = exp(-0.200679 * pow(Fctorbadmp, 0.431202));
		Fratioctm = pow2(FMotor->getGeometria().DiametroBowl / FMotor->getGeometria().Diametro) / FKctm;

		nmTipoValvula IntakeValveType = dynamic_cast<TCCCilindro*>(FCCValvulaAdm[0])->getValvula()->getTypeOfValve();
		switch(IntakeValveType) {
		case nmValvula4T:
			FDistribucion.AA = dynamic_cast<TValvula4T*>(dynamic_cast<TCCCilindro*>
							   (FCCValvulaAdm[0])->getValvula())->getAnguloApertura();
			FDistribucion.CA = dynamic_cast<TValvula4T*>(dynamic_cast<TCCCilindro*>
							   (FCCValvulaAdm[0])->getValvula())->getAnguloCierre();
			for(int i = 0; i < FNumeroUnionesAdm; i++) {
				if(dynamic_cast<TValvula4T*>(dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])->getValvula())->getAnguloApertura() <
				   FDistribucion.AA) {
					FDistribucion.AA = dynamic_cast<TValvula4T*>(dynamic_cast<TCCCilindro*>
									   (FCCValvulaAdm[i])->getValvula())->getAnguloApertura();
				}
				if(dynamic_cast<TValvula4T*>(dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])->getValvula())->getAnguloCierre() >
				   FDistribucion.CA) {
					FDistribucion.CA = dynamic_cast<TValvula4T*>(dynamic_cast<TCCCilindro*>
									   (FCCValvulaAdm[i])->getValvula())->getAnguloCierre();
				}
			}
			break;
		case nmLumbrera2T:
			FDistribucion.AA = dynamic_cast<TLumbrera*>(dynamic_cast<TCCCilindro*>
							   (FCCValvulaAdm[0])->getValvula())->getAnguloApertura();
			FDistribucion.CA = dynamic_cast<TLumbrera*>(dynamic_cast<TCCCilindro*>
							   (FCCValvulaAdm[0])->getValvula())->getAnguloCierre();
			for(int i = 0; i < FNumeroUnionesAdm; i++) {
				if(dynamic_cast<TLumbrera*>(dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])->getValvula())->getAnguloApertura() <
				   FDistribucion.AA) {
					FDistribucion.AA = dynamic_cast<TLumbrera*>(dynamic_cast<TCCCilindro*>
									   (FCCValvulaAdm[i])->getValvula())->getAnguloApertura();
				}
				if(dynamic_cast<TLumbrera*>(dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])->getValvula())->getAnguloCierre() >
				   FDistribucion.CA) {
					FDistribucion.CA = dynamic_cast<TLumbrera*>(dynamic_cast<TCCCilindro*>
									   (FCCValvulaAdm[i])->getValvula())->getAnguloCierre();
				}
			}
			break;
		default:
			std::cout << "ERROR: Intake valve is not a poppet valve nor a cylinder port " << std::endl;
		}

		nmTipoValvula ExhaustValveType = dynamic_cast<TCCCilindro*>(FCCValvulaEsc[0])->getValvula()->getTypeOfValve();
		switch(ExhaustValveType) {
		case nmValvula4T:
			FDistribucion.AE = dynamic_cast<TValvula4T*>(dynamic_cast<TCCCilindro*>
							   (FCCValvulaEsc[0])->getValvula())->getAnguloApertura();
			FDistribucion.CE = dynamic_cast<TValvula4T*>(dynamic_cast<TCCCilindro*>
							   (FCCValvulaEsc[0])->getValvula())->getAnguloCierre();
			for(int i = 0; i < FNumeroUnionesEsc; i++) {
				if(dynamic_cast<TValvula4T*>(dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])->getValvula())->getAnguloApertura() <
				   FDistribucion.AE) {
					FDistribucion.AE = dynamic_cast<TValvula4T*>(dynamic_cast<TCCCilindro*>
									   (FCCValvulaEsc[i])->getValvula())->getAnguloApertura();
				}
				if(dynamic_cast<TValvula4T*>(dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])->getValvula())->getAnguloCierre() >
				   FDistribucion.CE) {
					FDistribucion.CE = dynamic_cast<TValvula4T*>(dynamic_cast<TCCCilindro*>
									   (FCCValvulaEsc[i])->getValvula())->getAnguloCierre();
				}
			}
			break;
		case nmLumbrera2T:
			FDistribucion.AE = dynamic_cast<TLumbrera*>(dynamic_cast<TCCCilindro*>
							   (FCCValvulaEsc[0])->getValvula())->getAnguloApertura();
			FDistribucion.CE = dynamic_cast<TLumbrera*>(dynamic_cast<TCCCilindro*>
							   (FCCValvulaEsc[0])->getValvula())->getAnguloCierre();
			for(int i = 0; i < FNumeroUnionesEsc; i++) {
				if(dynamic_cast<TLumbrera*>(dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])->getValvula())->getAnguloApertura() <
				   FDistribucion.AE) {
					FDistribucion.AE = dynamic_cast<TLumbrera*>(dynamic_cast<TCCCilindro*>
									   (FCCValvulaEsc[i])->getValvula())->getAnguloApertura();
				}
				if(dynamic_cast<TLumbrera*>(dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])->getValvula())->getAnguloCierre() >
				   FDistribucion.CE) {
					FDistribucion.CE = dynamic_cast<TLumbrera*>(dynamic_cast<TCCCilindro*>
									   (FCCValvulaEsc[i])->getValvula())->getAnguloCierre();
				}
			}
			break;
		default:
			std::cout << "ERROR: Exhaust valve is not a poppet valve nor a cylinder port " << std::endl;
		}

		if(FMotor->getACT()) {
			Fengine_parameters[18] = Fctorbadmp; // CTM
			if(FMotor->getEngineType() == nm4T)
				Fengine_parameters[22] = FDistribucion.CA - 720;
			else
				Fengine_parameters[22] = FDistribucion.CA - 360;
			// Inlet valve closing
			Fengine_parameters[23] = FDistribucion.AE; // Exhaust valve opening
		}

		// Valor de alpha para el modelo de cortocircuito.
		if(FMotor->getEngineType() == nm4T) {
			FAlphaEscape = asin((dynamic_cast<TValvula4T*>(dynamic_cast<TCCCilindro*>
								 (FCCValvulaEsc[0])->getValvula())->getDiametro() / 2.) / FMotor->getGeometria().DistanciaValvulas);
			FAlphaAdmision = asin((dynamic_cast<TValvula4T*>(dynamic_cast<TCCCilindro*>
								   (FCCValvulaAdm[0])->getValvula())->getDiametro() / 2.) / FMotor->getGeometria().DistanciaValvulas);
		}

		if(FResInstantCilindro.MomentoAngularEsc) {
			FResInstantCilindro.MomentoAngularEscINS = new double[FNumeroUnionesEsc];
			for(int i = 0; i < FNumeroUnionesEsc; i++) {
				FResInstantCilindro.MomentoAngularEscINS[i] = 0.;
			}
		}
		if(FResInstantCilindro.MomentoAngularAdm) {
			FResInstantCilindro.MomentoAngularAdmINS = new double[FNumeroUnionesAdm];
			for(int i = 0; i < FNumeroUnionesAdm; i++) {
				FResInstantCilindro.MomentoAngularAdmINS[i] = 0.;
			}
		}
		if(FResInstantCilindro.GastoEsc) {
			FResInstantCilindro.GastoEscINS = new double[FNumeroUnionesEsc];
			for(int i = 0; i < FNumeroUnionesEsc; i++) {
				FResInstantCilindro.GastoEscINS[i] = 0.;
			}
		}
		if(FResInstantCilindro.GastoAdm) {
			FResInstantCilindro.GastoAdmINS = new double[FNumeroUnionesAdm];
			for(int i = 0; i < FNumeroUnionesAdm; i++) {
				FResInstantCilindro.GastoAdmINS[i] = 0.;
			}
		}
		if(FResInstantCilindro.MachEsc) {
			FResInstantCilindro.MachEscINS = new double[FNumeroUnionesEsc];
			for(int i = 0; i < FNumeroUnionesEsc; i++) {
				FResInstantCilindro.MachEscINS[i] = 0.;
			}
		}
		if(FResInstantCilindro.MachAdm) {
			FResInstantCilindro.MachAdmINS = new double[FNumeroUnionesAdm];
			for(int i = 0; i < FNumeroUnionesAdm; i++) {
				FResInstantCilindro.MachAdmINS[i] = 0;
			}
		}
		if(FResInstantCilindro.SeccionEfectivaEsc) {
			FResInstantCilindro.SeccionEfectivaEscINS = new double[FNumeroUnionesEsc];
			for(int i = 0; i < FNumeroUnionesEsc; i++) {
				FResInstantCilindro.SeccionEfectivaEscINS[i] = 0.;
			}
		}
		if(FResInstantCilindro.SeccionEfectivaAdm) {
			FResInstantCilindro.SeccionEfectivaAdmINS = new double[FNumeroUnionesAdm];
			for(int i = 0; i < FNumeroUnionesAdm; i++) {
				FResInstantCilindro.SeccionEfectivaAdmINS[i] = 0.;
			}
		}
		if(FResInstantCilindro.NIT) {
			FResInstantCilindro.NITINS = new double[FNumeroUnionesEsc];
			for(int i = 0; i < FNumeroUnionesEsc; i++) {
				FResInstantCilindro.NITINS[i] = 0.;
			}
		}
		if(FResMediosCilindro.NITMedio) {
			FResMediosCilindro.NITMED = new double[FNumeroUnionesEsc];
			for(int i = 0; i < FNumeroUnionesEsc; i++) {
				FResMediosCilindro.NITMED[i] = 0.;
			}
		}

		// Solo se usa el NIT de las valvulas de escape, pero como antes estaba como resultados por pantalla,
		// lo dejo por lo que pueda pasar, y al menos la estructura ya existe.
		FValvAdm = new stValvulasCilindro[FNumeroUnionesAdm];
		for(int i = 0; i < FNumeroUnionesAdm; i++) {
			FValvAdm[i].GastoSUM = 0.;
			FValvAdm[i].GastoSeccionSUM = 0.;
			FValvAdm[i].MachSUM = 0.;
			FValvAdm[i].RelacionPresionGargantaSUM = 0.;
		}

		FValvEsc = new stValvulasCilindro[FNumeroUnionesEsc];
		for(int i = 0; i < FNumeroUnionesEsc; i++) {
			FValvEsc[i].GastoSUM = 0.;
			FValvEsc[i].GastoSeccionSUM = 0.;
			FValvEsc[i].MachSUM = 0.;
			FValvEsc[i].RelacionPresionGargantaSUM = 0.;
			FValvEsc[i].NITSUM = 0.;
		}

	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::AsignacionCC en cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro::ReadAverageResultsCilindro(const char *FileWAM, fpos_t &filepos) {
	try {
		int Tipovar = 0;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		FResMediosCilindro.TrabajoNeto = false;
		FResMediosCilindro.TrabajoNetoMED = 0.;
		FResMediosCilindro.TrabajoNetoSUM = 0.;
		FResMediosCilindro.PresionMediaNeta = false;
		FResMediosCilindro.PresionMediaNetaMED = 0.;
		FResMediosCilindro.TrabajoBombeo = false;
		FResMediosCilindro.TrabajoBombeoMED = 0.;
		FResMediosCilindro.TrabajoBombeoSUM = 0.;
		FResMediosCilindro.PresionMediaBombeo = false;
		FResMediosCilindro.PresionMediaBombeoMED = 0.;
		FResMediosCilindro.CalorCombustion = false;
		FResMediosCilindro.CalorCombustionMED = 0.;
		FResMediosCilindro.CalorCombustionSUM = 0.;
		FResMediosCilindro.CalorCilindro = false;
		FResMediosCilindro.CalorCilindroMED = 0.;
		FResMediosCilindro.CalorCilindroSUM = 0.;
		FResMediosCilindro.CalorCulata = false;
		FResMediosCilindro.CalorCulataMED = 0.;
		FResMediosCilindro.CalorCulataSUM = 0.;
		FResMediosCilindro.CalorPiston = false;
		FResMediosCilindro.CalorPistonMED = 0.;
		FResMediosCilindro.CalorPistonSUM = 0.;
		FResMediosCilindro.PresionMediaIndicada = false;
		FResMediosCilindro.PresionMediaIndicadaMED = 0.;
		FResMediosCilindro.MasaAtrapada = false;
		FResMediosCilindro.MasaAtrapadaMED = 0.;
		FResMediosCilindro.TemperaturaCilindroInterna = false;
		FResMediosCilindro.TemperaturaCilindroInternaMED = 0.;
		FResMediosCilindro.TemperaturaCilindroInternaSUM = 0.;
		FResMediosCilindro.TemperaturaCilindroMedia = false;
		FResMediosCilindro.TemperaturaCilindroMediaMED = 0.;
		FResMediosCilindro.TemperaturaCilindroMediaSUM = 0.;
		FResMediosCilindro.TemperaturaCilindroExterna = false;
		FResMediosCilindro.TemperaturaCilindroExternaMED = 0.;
		FResMediosCilindro.TemperaturaCilindroExternaSUM = 0.;
		FResMediosCilindro.TemperaturaPistonInterna = false;
		FResMediosCilindro.TemperaturaPistonInternaMED = 0.;
		FResMediosCilindro.TemperaturaPistonInternaSUM = 0.;
		FResMediosCilindro.TemperaturaPistonMedia = false;
		FResMediosCilindro.TemperaturaPistonMediaMED = 0.;
		FResMediosCilindro.TemperaturaPistonMediaSUM = 0.;
		FResMediosCilindro.TemperaturaPistonExterna = false;
		FResMediosCilindro.TemperaturaPistonExternaMED = 0.;
		FResMediosCilindro.TemperaturaPistonExternaSUM = 0.;
		FResMediosCilindro.TemperaturaCulataInterna = false;
		FResMediosCilindro.TemperaturaCulataInternaMED = 0.;
		FResMediosCilindro.TemperaturaCulataInternaSUM = 0.;
		FResMediosCilindro.TemperaturaCulataMedia = false;
		FResMediosCilindro.TemperaturaCulataMediaMED = 0.;
		FResMediosCilindro.TemperaturaCulataMediaSUM = 0.;
		FResMediosCilindro.TemperaturaCulataExterna = false;
		FResMediosCilindro.TemperaturaCulataExternaMED = 0.;
		FResMediosCilindro.TemperaturaCulataExternaSUM = 0.;
		FResMediosCilindro.NITMedio = false;
		FResMediosCilindro.NITMedioMED = 0.;
		FResMediosCilindro.NITMedioSUM = 0.;
		FResMediosCilindro.AFRMedio = false;
		FResMediosCilindro.AFRMedioMED = 0.;
		FResMediosCilindro.MasaBlowBy = false;
		FResMediosCilindro.MasaBlowByMED = 0.;
		FResMediosCilindro.MasaBlowBySUM = 0.;
		FResMediosCilindro.MasaAdmision = false;
		FResMediosCilindro.MasaAdmisionMED = 0.;
		FResMediosCilindro.MasaEscape = false;
		FResMediosCilindro.MasaEscapeMED = 0.;
		FResMediosCilindro.TemperaturaMedia = false;
		FResMediosCilindro.TemperaturaMediaMED = 0.;
		FResMediosCilindro.TemperaturaMediaSUM = 0.;
		FResMediosCilindro.Swirl = false;
		FResMediosCilindro.SwirlMED = 0.;
		FResMediosCilindro.RendVolumetrico = false;
		FResMediosCilindro.RendVolumetricoMED = 0.;
		FResMediosCilindro.DensidadReferenciaSUM = 0.;
		FResMediosCilindro.MasaCortocircuito = false;
		FResMediosCilindro.MasaCortocircuitoMED = 0.;
		FResMediosCilindro.MasaCortocircuitoSUM = 0.;

		FResMediosCilindro.Tiempo0 = 0.;
		FResMediosCilindro.TiempoSUM = 0.;

		fscanf(fich, "%d ", &FNumVarMed);
		for(int i = 0; i < FNumVarMed; i++) {
			fscanf(fich, "%d ", &Tipovar);
			switch(Tipovar) {
			case 0:
				FResMediosCilindro.TrabajoNeto = true;
				break;
			case 1:
				FResMediosCilindro.PresionMediaNeta = true;
				break;
			case 2:
				FResMediosCilindro.TrabajoBombeo = true;
				break;
			case 3:
				FResMediosCilindro.PresionMediaBombeo = true;
				break;
			case 4:
				FResMediosCilindro.CalorCombustion = true;
				break;
			case 5:
				FResMediosCilindro.CalorCilindro = true;
				break;
			case 6:
				FResMediosCilindro.CalorCulata = true;
				break;
			case 7:
				FResMediosCilindro.CalorPiston = true;
				break;
			case 8:
				FResMediosCilindro.PresionMediaIndicada = true;
				break;
			case 9:
				FResMediosCilindro.MasaAtrapada = true;
				break;
			case 10:
				FResMediosCilindro.TemperaturaCilindroInterna = true;
				break;
			case 11:
				FResMediosCilindro.TemperaturaCilindroMedia = true;
				break;
			case 12:
				FResMediosCilindro.TemperaturaCilindroExterna = true;
				break;
			case 13:
				FResMediosCilindro.TemperaturaPistonInterna = true;
				break;
			case 14:
				FResMediosCilindro.TemperaturaPistonMedia = true;
				break;
			case 15:
				FResMediosCilindro.TemperaturaPistonExterna = true;
				break;
			case 16:
				FResMediosCilindro.TemperaturaCulataInterna = true;
				break;
			case 17:
				FResMediosCilindro.TemperaturaCulataMedia = true;
				break;
			case 18:
				FResMediosCilindro.TemperaturaCulataExterna = true;
				break;
			case 19:
				FResMediosCilindro.NITMedio = true;
				break;
			case 20:
				FResMediosCilindro.AFRMedio = true;
				break;
			case 21:
				FResMediosCilindro.MasaBlowBy = true;
				break;
			case 22:
				FResMediosCilindro.MasaAdmision = true;
				break;
			case 23:
				FResMediosCilindro.MasaEscape = true;
				break;
			case 24:
				FResMediosCilindro.MasaCortocircuito = true;
				break;
			case 25:
				FResMediosCilindro.TemperaturaMedia = true;
				break;
			case 26:
				FResMediosCilindro.Swirl = true;
				break;
			case 27:
				FResMediosCilindro.RendVolumetrico = true;
				break;
			default:
				std::cout << "Resultados medios en cilindro " << FNumeroCilindro << " no implementado " << std::endl;
			}
		}

		fgetpos(fich, &filepos);
		fclose(fich);

	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::ReadAverageResultsCilindro en cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro::HeaderAverageResultsCilindro(stringstream& medoutput, stEspecies *DatosEspecies) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");

		std::string Label;

		if(FNumVarMed > 0) {
			if(FResMediosCilindro.TrabajoNeto) {
				Label = "\t" + PutLabel(631) + std::to_string(FNumeroCilindro) + PutLabel(907);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.PresionMediaNeta) {
				Label = "\t" + PutLabel(632) + std::to_string(FNumeroCilindro) + PutLabel(908);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.TrabajoBombeo) {
				Label = "\t" + PutLabel(633) + std::to_string(FNumeroCilindro) + PutLabel(907);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.PresionMediaBombeo) {
				Label = "\t" + PutLabel(634) + std::to_string(FNumeroCilindro) + PutLabel(908);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.CalorCombustion) {
				Label = "\t" + PutLabel(635) + std::to_string(FNumeroCilindro) + PutLabel(907);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.CalorCilindro) {
				Label = "\t" + PutLabel(636) + std::to_string(FNumeroCilindro) + PutLabel(907);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.CalorCulata) {
				Label = "\t" + PutLabel(637) + std::to_string(FNumeroCilindro) + PutLabel(907);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.CalorPiston) {
				Label = "\t" + PutLabel(638) + std::to_string(FNumeroCilindro) + PutLabel(907);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.PresionMediaIndicada) {
				Label = "\t" + PutLabel(639) + std::to_string(FNumeroCilindro) + PutLabel(908);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.MasaAtrapada) {
				Label = "\t" + PutLabel(640) + std::to_string(FNumeroCilindro) + PutLabel(913);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.TemperaturaCilindroInterna) {
				Label = "\t" + PutLabel(641) + std::to_string(FNumeroCilindro) + PutLabel(910);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.TemperaturaCilindroMedia) {
				Label = "\t" + PutLabel(642) + std::to_string(FNumeroCilindro) + PutLabel(910);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.TemperaturaCilindroExterna) {
				Label = "\t" + PutLabel(643) + std::to_string(FNumeroCilindro) + PutLabel(910);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.TemperaturaPistonInterna) {
				Label = "\t" + PutLabel(644) + std::to_string(FNumeroCilindro) + PutLabel(910);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.TemperaturaPistonMedia) {
				Label = "\t" + PutLabel(645) + std::to_string(FNumeroCilindro) + PutLabel(910);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.TemperaturaPistonExterna) {
				Label = "\t" + PutLabel(646) + std::to_string(FNumeroCilindro) + PutLabel(910);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.TemperaturaCulataInterna) {
				Label = "\t" + PutLabel(647) + std::to_string(FNumeroCilindro) + PutLabel(910);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.TemperaturaCulataMedia) {
				Label = "\t" + PutLabel(648) + std::to_string(FNumeroCilindro) + PutLabel(910);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.TemperaturaCulataExterna) {
				Label = "\t" + PutLabel(649) + std::to_string(FNumeroCilindro) + PutLabel(910);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.NITMedio) {
				for(int i = 0; i < FNumeroUnionesEsc; i++) {
					Label = "\t" + PutLabel(650) + std::to_string(i) + PutLabel(651) + std::to_string(FNumeroCilindro) + PutLabel(903);
					medoutput << Label.c_str();
				}
				Label = "\t" + PutLabel(652) + std::to_string(FNumeroCilindro) + PutLabel(903);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.AFRMedio) {
				Label = "\t" + PutLabel(653) + std::to_string(FNumeroCilindro) + PutLabel(901);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.MasaBlowBy) {
				Label = "\t" + PutLabel(654) + std::to_string(FNumeroCilindro) + PutLabel(914);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.MasaAdmision) {
				Label = "\t" + PutLabel(655) + std::to_string(FNumeroCilindro) + PutLabel(914);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.MasaEscape) {
				Label = "\t" + PutLabel(656) + std::to_string(FNumeroCilindro) + PutLabel(914);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.MasaCortocircuito) {
				Label = "\t" + PutLabel(657) + std::to_string(FNumeroCilindro) + PutLabel(914);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.TemperaturaMedia) {
				Label = "\t" + PutLabel(658) + std::to_string(FNumeroCilindro) + PutLabel(910);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.Swirl) {
				Label = "\t" + PutLabel(659) + std::to_string(FNumeroCilindro) + PutLabel(901);
				medoutput << Label.c_str();
			}
			if(FResMediosCilindro.RendVolumetrico) {
				Label = "\t" + PutLabel(660) + std::to_string(FNumeroCilindro) + PutLabel(901);
				medoutput << Label.c_str();
			}
		}

		// fclose(fich);

	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::HeaderAverageResultsCilindro en cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro::ImprimeResultadosMediosCilindro(stringstream& medoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");

		if(FNumVarMed > 0) {
			if(FResMediosCilindro.TrabajoNeto)
				medoutput << "\t" << FResMediosCilindro.TrabajoNetoMED;
			if(FResMediosCilindro.PresionMediaNeta)
				medoutput << "\t" << FResMediosCilindro.PresionMediaNetaMED;
			if(FResMediosCilindro.TrabajoBombeo)
				medoutput << "\t" << FResMediosCilindro.TrabajoBombeoMED;
			if(FResMediosCilindro.PresionMediaBombeo)
				medoutput << "\t" << FResMediosCilindro.PresionMediaBombeoMED;
			if(FResMediosCilindro.CalorCombustion)
				medoutput << "\t" << FResMediosCilindro.CalorCombustionMED;
			if(FResMediosCilindro.CalorCilindro)
				medoutput << "\t" << FResMediosCilindro.CalorCilindroMED;
			if(FResMediosCilindro.CalorCulata)
				medoutput << "\t" << FResMediosCilindro.CalorCulataMED;
			if(FResMediosCilindro.CalorPiston)
				medoutput << "\t" << FResMediosCilindro.CalorPistonMED;
			if(FResMediosCilindro.PresionMediaIndicada)
				medoutput << "\t" << FResMediosCilindro.PresionMediaIndicadaMED;
			if(FResMediosCilindro.MasaAtrapada)
				medoutput << "\t" << FResMediosCilindro.MasaAtrapadaMED;
			if(FResMediosCilindro.TemperaturaCilindroInterna)
				medoutput << "\t" << FResMediosCilindro.TemperaturaCilindroInternaMED;
			if(FResMediosCilindro.TemperaturaCilindroMedia)
				medoutput << "\t" << FResMediosCilindro.TemperaturaCilindroMediaMED;
			if(FResMediosCilindro.TemperaturaCilindroExterna)
				medoutput << "\t" << FResMediosCilindro.TemperaturaCilindroExternaMED;
			if(FResMediosCilindro.TemperaturaPistonInterna)
				medoutput << "\t" << FResMediosCilindro.TemperaturaPistonInternaMED;
			if(FResMediosCilindro.TemperaturaPistonMedia)
				medoutput << "\t" << FResMediosCilindro.TemperaturaPistonMediaMED;
			if(FResMediosCilindro.TemperaturaPistonExterna)
				medoutput << "\t" << FResMediosCilindro.TemperaturaPistonExternaMED;
			if(FResMediosCilindro.TemperaturaCulataInterna)
				medoutput << "\t" << FResMediosCilindro.TemperaturaCulataInternaMED;
			if(FResMediosCilindro.TemperaturaCulataMedia)
				medoutput << "\t" << FResMediosCilindro.TemperaturaCulataMediaMED;
			if(FResMediosCilindro.TemperaturaCulataExterna)
				medoutput << "\t" << FResMediosCilindro.TemperaturaCulataExternaMED;
			if(FResMediosCilindro.NITMedio) {
				for(int i = 0; i < FNumeroUnionesEsc; i++) {
					medoutput << "\t" << FResMediosCilindro.NITMED[i];
				}
				medoutput << "\t" << FResMediosCilindro.NITMedioMED;
			}
			if(FResMediosCilindro.AFRMedio)
				medoutput << "\t" << FResMediosCilindro.AFRMedioMED;
			if(FResMediosCilindro.MasaBlowBy)
				medoutput << "\t" << FResMediosCilindro.MasaBlowByMED;
			if(FResMediosCilindro.MasaAdmision)
				medoutput << "\t" << FResMediosCilindro.MasaAdmisionMED;
			if(FResMediosCilindro.MasaEscape)
				medoutput << "\t" << FResMediosCilindro.MasaEscapeMED;
			if(FResMediosCilindro.MasaCortocircuito)
				medoutput << "\t" << FResMediosCilindro.MasaCortocircuitoMED;
			if(FResMediosCilindro.TemperaturaMedia)
				medoutput << "\t" << FResMediosCilindro.TemperaturaMediaMED;
			if(FResMediosCilindro.Swirl)
				medoutput << "\t" << FResMediosCilindro.SwirlMED;
			if(FResMediosCilindro.RendVolumetrico)
				medoutput << "\t" << FResMediosCilindro.RendVolumetricoMED;
		}

		// fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::ImprimeResultadosMediosCilindro " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro::AcumulaResultadosMediosCilindro(double TActual) {
	try {

		if(FNumVarMed > 0) {
			double DeltaT = TActual - FResMediosCilindro.Tiempo0;
			// double DeltaAngulo=360.*FMotor->PutRegimen(360.*FMotor->getRegimen()/60.*DeltaT);

			FResMediosCilindro.TrabajoNetoSUM += __units::BarToPa(FPreMed) * (FVolumen - FVolumen0);
			if(FAnguloActual > 180. && FAnguloActual < 540.) {
				FResMediosCilindro.TrabajoBombeoSUM += __units::BarToPa(FPreMed) * (FVolumen - FVolumen0);
			}

			if(FResMediosCilindro.CalorCombustion && DeltaT > 0.)
				//FResMediosCilindro.CalorCombustionSUM += FCalor.Liberado;
				if(FResMediosCilindro.CalorCilindro)
					FResMediosCilindro.CalorCilindroSUM += FCalor.TransCilindro * DeltaT;
			if(FResMediosCilindro.CalorCulata)
				FResMediosCilindro.CalorCulataSUM += FCalor.TransCulata * DeltaT;
			if(FResMediosCilindro.CalorPiston)
				FResMediosCilindro.CalorPistonSUM += FCalor.TransPiston * DeltaT;

			if(FResMediosCilindro.TemperaturaCilindroInterna)
				FResMediosCilindro.TemperaturaCilindroInternaSUM += FTempPared[0].Cylinder * DeltaT;
			if(FResMediosCilindro.TemperaturaCilindroMedia)
				FResMediosCilindro.TemperaturaCilindroMediaSUM += FTempPared[1].Cylinder * DeltaT;
			if(FResMediosCilindro.TemperaturaCilindroExterna)
				FResMediosCilindro.TemperaturaCilindroExternaSUM += FTempPared[2].Cylinder * DeltaT;

			if(FResMediosCilindro.TemperaturaCulataInterna)
				FResMediosCilindro.TemperaturaCulataInternaSUM += FTempPared[0].Culata * DeltaT;
			if(FResMediosCilindro.TemperaturaCulataMedia)
				FResMediosCilindro.TemperaturaCulataMediaSUM += FTempPared[1].Culata * DeltaT;
			if(FResMediosCilindro.TemperaturaCulataExterna)
				FResMediosCilindro.TemperaturaCulataExternaSUM += FTempPared[2].Culata * DeltaT;

			if(FResMediosCilindro.TemperaturaPistonInterna)
				FResMediosCilindro.TemperaturaPistonInternaSUM += FTempPared[0].Piston * DeltaT;
			if(FResMediosCilindro.TemperaturaPistonMedia)
				FResMediosCilindro.TemperaturaPistonMediaSUM += FTempPared[1].Piston * DeltaT;
			if(FResMediosCilindro.TemperaturaPistonExterna)
				FResMediosCilindro.TemperaturaPistonExternaSUM += FTempPared[2].Piston * DeltaT;

			if(FResMediosCilindro.NITMedio)
				FResMediosCilindro.NITMedioSUM += FNIT * DeltaT;
			if(FResMediosCilindro.MasaBlowBy)
				FResMediosCilindro.MasaBlowBySUM += FMasaBlowBy;
			if(FResMediosCilindro.MasaCortocircuito)
				FResMediosCilindro.MasaCortocircuitoSUM += FMasaCortocircuito;
			if(FResMediosCilindro.TemperaturaMedia)
				FResMediosCilindro.TemperaturaMediaSUM += FTemperature * FDeltaT;
			if(FResMediosCilindro.RendVolumetrico)
				FResMediosCilindro.DensidadReferenciaSUM += FDensidadReferencia * FDeltaT;

			FResMediosCilindro.TiempoSUM += DeltaT;
			FResMediosCilindro.Tiempo0 = TActual;
		}
	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::AcumulaResultadosMediosCilindro " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro::CalculaResultadosMediosCilindro() {
	try {

		if(FNumVarMed > 0) {
			if(FResMediosCilindro.TrabajoNeto || FResMediosCilindro.PresionMediaNeta || FResMediosCilindro.PresionMediaIndicada) {
				FResMediosCilindro.TrabajoNetoMED = FResMediosCilindro.TrabajoNetoSUM;
				FResMediosCilindro.TrabajoNetoSUM = 0.;
			}
			if(FResMediosCilindro.PresionMediaNeta || FResMediosCilindro.PresionMediaIndicada) {
				FResMediosCilindro.PresionMediaNetaMED = __units::PaToBar(FResMediosCilindro.TrabajoNetoMED /
						FMotor->getGeometria().CilindradaUnitaria);
			}
			if(FResMediosCilindro.TrabajoBombeo || FResMediosCilindro.PresionMediaBombeo
			   || FResMediosCilindro.PresionMediaIndicada) {
				FResMediosCilindro.TrabajoBombeoMED = FResMediosCilindro.TrabajoBombeoSUM;
				FResMediosCilindro.TrabajoBombeoSUM = 0.;
			}
			if(FResMediosCilindro.PresionMediaBombeo || FResMediosCilindro.PresionMediaIndicada) {
				FResMediosCilindro.PresionMediaBombeoMED = __units::PaToBar(-FResMediosCilindro.TrabajoBombeoMED /
						FMotor->getGeometria().CilindradaUnitaria);
			}
			if(FResMediosCilindro.CalorCombustion) {
				FResMediosCilindro.CalorCombustionMED = FResMediosCilindro.CalorCombustionSUM;
				FResMediosCilindro.CalorCombustionSUM = 0.;
			}
			if(FResMediosCilindro.CalorCilindro) {
				FResMediosCilindro.CalorCilindroMED = FResMediosCilindro.CalorCilindroSUM;
				FResMediosCilindro.CalorCilindroSUM = 0.;
			}
			if(FResMediosCilindro.CalorCulata) {
				FResMediosCilindro.CalorCulataMED = FResMediosCilindro.CalorCulataSUM;
				FResMediosCilindro.CalorCulataSUM = 0.;
			}
			if(FResMediosCilindro.CalorPiston) {
				FResMediosCilindro.CalorPistonMED = FResMediosCilindro.CalorPistonSUM;
				FResMediosCilindro.CalorPistonSUM = 0.;
			}
			if(FResMediosCilindro.PresionMediaIndicada) {
				FResMediosCilindro.PresionMediaIndicadaMED = FResMediosCilindro.PresionMediaNetaMED +
						FResMediosCilindro.PresionMediaBombeoMED;
			}
			if(FResMediosCilindro.MasaAtrapada) {
				FResMediosCilindro.MasaAtrapadaMED = FMasaAtrapada;
			}
			if(FResMediosCilindro.TemperaturaCilindroInterna) {
				FResMediosCilindro.TemperaturaCilindroInternaMED = FResMediosCilindro.TemperaturaCilindroInternaSUM /
						FResMediosCilindro.TiempoSUM;
				FResMediosCilindro.TemperaturaCilindroInternaSUM = 0.;
			}
			if(FResMediosCilindro.TemperaturaCilindroMedia) {
				FResMediosCilindro.TemperaturaCilindroMediaMED = FResMediosCilindro.TemperaturaCilindroMediaSUM /
						FResMediosCilindro.TiempoSUM;
				FResMediosCilindro.TemperaturaCilindroMediaSUM = 0.;
			}
			if(FResMediosCilindro.TemperaturaCilindroExterna) {
				FResMediosCilindro.TemperaturaCilindroExternaMED = FResMediosCilindro.TemperaturaCilindroExternaSUM /
						FResMediosCilindro.TiempoSUM;
				FResMediosCilindro.TemperaturaCilindroExternaSUM = 0.;
			}
			if(FResMediosCilindro.TemperaturaCulataInterna) {
				FResMediosCilindro.TemperaturaCulataInternaMED = FResMediosCilindro.TemperaturaCulataInternaSUM /
						FResMediosCilindro.TiempoSUM;
				FResMediosCilindro.TemperaturaCulataInternaSUM = 0.;
			}
			if(FResMediosCilindro.TemperaturaCulataMedia) {
				FResMediosCilindro.TemperaturaCulataMediaMED = FResMediosCilindro.TemperaturaCulataMediaSUM /
						FResMediosCilindro.TiempoSUM;
				FResMediosCilindro.TemperaturaCulataMediaSUM = 0.;
			}
			if(FResMediosCilindro.TemperaturaCulataExterna) {
				FResMediosCilindro.TemperaturaCulataExternaMED = FResMediosCilindro.TemperaturaCulataExternaSUM /
						FResMediosCilindro.TiempoSUM;
				FResMediosCilindro.TemperaturaCulataExternaSUM = 0.;
			}
			if(FResMediosCilindro.TemperaturaPistonInterna) {
				FResMediosCilindro.TemperaturaPistonInternaMED = FResMediosCilindro.TemperaturaPistonInternaSUM /
						FResMediosCilindro.TiempoSUM;
				FResMediosCilindro.TemperaturaPistonInternaSUM = 0.;
			}
			if(FResMediosCilindro.TemperaturaPistonMedia) {
				FResMediosCilindro.TemperaturaPistonMediaMED = FResMediosCilindro.TemperaturaPistonMediaSUM /
						FResMediosCilindro.TiempoSUM;
				FResMediosCilindro.TemperaturaPistonMediaSUM = 0.;
			}
			if(FResMediosCilindro.TemperaturaPistonExterna) {
				FResMediosCilindro.TemperaturaPistonExternaMED = FResMediosCilindro.TemperaturaPistonExternaSUM /
						FResMediosCilindro.TiempoSUM;
				FResMediosCilindro.TemperaturaPistonExternaSUM = 0.;
			}
			if(FResMediosCilindro.AFRMedio) {
				FResMediosCilindro.AFRMedioMED = FAFR;
			}
			if(FResMediosCilindro.NITMedio) {
				for(int i = 0; i < FNumeroUnionesEsc; i++) {
					FResMediosCilindro.NITMED[i] = FValvEsc[i].NITSUM / FResMediosCilindro.TiempoSUM;
					FValvEsc[i].NITSUM = 0.;
				}
				FResMediosCilindro.NITMedioMED = FResMediosCilindro.NITMedioSUM / FResMediosCilindro.TiempoSUM;
				FResMediosCilindro.NITMedioSUM = 0.;
			}

			if(FResMediosCilindro.MasaBlowBy) {
				FResMediosCilindro.MasaBlowByMED = FResMediosCilindro.MasaBlowBySUM;
				FResMediosCilindro.MasaBlowBySUM = 0.;
			}
			if(FResMediosCilindro.MasaAdmision) {
				FResMediosCilindro.MasaAdmisionMED = FMasaPorAdmision;
			}
			if(FResMediosCilindro.MasaEscape) {
				FResMediosCilindro.MasaEscapeMED = FMasaPorEscape;
			}
			if(FResMediosCilindro.MasaCortocircuito) {
				FResMediosCilindro.MasaCortocircuitoMED = FResMediosCilindro.MasaCortocircuitoSUM;
				FResMediosCilindro.MasaCortocircuitoSUM = 0.;
			}
			if(FResMediosCilindro.TemperaturaMedia) {
				FResMediosCilindro.TemperaturaMediaMED = FResMediosCilindro.TemperaturaMediaSUM / FResMediosCilindro.TiempoSUM;
				FResMediosCilindro.TemperaturaMediaSUM = 0.;
			}
			if(FResMediosCilindro.Swirl) {
				FResMediosCilindro.SwirlMED = FSwirlSUM / FResMediosCilindro.TiempoSUM;
				FSwirlSUM = 0.;
			}
			if(FResMediosCilindro.RendVolumetrico) {
				double DensidadReferencia = FResMediosCilindro.DensidadReferenciaSUM / FResMediosCilindro.TiempoSUM;
				FResMediosCilindro.RendVolumetricoMED = FMasaAtrapada / DensidadReferencia / FMotor->getGeometria().CilindradaUnitaria;
				FResMediosCilindro.DensidadReferenciaSUM = 0.;
			}

			FResMediosCilindro.TiempoSUM = 0;
		}

	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::CalculaResultadosMediosCilindro " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro::ReadInstantaneousResultsCilindro(const char *FileWAM, fpos_t &filepos) {
	try {
		int nvars = 0, var = 0;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		FResInstantCilindro.Pressure = false;
		FResInstantCilindro.PresionINS = 0.;
		FResInstantCilindro.Temperature = false;
		FResInstantCilindro.TemperaturaINS = 0.;
		FResInstantCilindro.MomentoAngularEsc = false;
		FResInstantCilindro.MomentoAngularTotalEscINS = 0.;
		FResInstantCilindro.MomentoAngularAdm = false;
		FResInstantCilindro.MomentoAngularTotalAdmINS = 0.;
		FResInstantCilindro.GastoEsc = false;
		FResInstantCilindro.GastoTotalEscINS = 0.;
		FResInstantCilindro.GastoAdm = false;
		FResInstantCilindro.GastoTotalAdmINS = 0.;
		FResInstantCilindro.MachEsc = false;
		FResInstantCilindro.MachAdm = false;
		FResInstantCilindro.SeccionEfectivaAdm = false;
		FResInstantCilindro.SeccionEfectivaTotalAdmINS = 0.;
		FResInstantCilindro.SeccionEfectivaEsc = false;
		FResInstantCilindro.SeccionEfectivaTotalEscINS = 0.;
		FResInstantCilindro.Masa = false;
		FResInstantCilindro.MasaINS = 0.;
		FResInstantCilindro.Volumen = false;
		FResInstantCilindro.VolumenINS = 0.;
		FResInstantCilindro.CoeficienteWoschni = false;
		FResInstantCilindro.CoeficienteWoschniINS = 0.;
		FResInstantCilindro.MasaCombustible = false;
		FResInstantCilindro.MasaCombustibleINS = 0.;
		FResInstantCilindro.FQL = false;
		FResInstantCilindro.FQLINS = 0.;
		FResInstantCilindro.TemperaturaCilindroInterna = false;
		FResInstantCilindro.TemperaturaCilindroInternaINS = 0.;
		FResInstantCilindro.TemperaturaCilindroMedia = false;
		FResInstantCilindro.TemperaturaCilindroMediaINS = 0.;
		FResInstantCilindro.TemperaturaCilindroExterna = false;
		FResInstantCilindro.TemperaturaCilindroExternaINS = 0.;
		FResInstantCilindro.TemperaturaPistonInterna = false;
		FResInstantCilindro.TemperaturaPistonInternaINS = 0.;
		FResInstantCilindro.TemperaturaPistonMedia = false;
		FResInstantCilindro.TemperaturaPistonMediaINS = 0.;
		FResInstantCilindro.TemperaturaPistonExterna = false;
		FResInstantCilindro.TemperaturaPistonExternaINS = 0.;
		FResInstantCilindro.TemperaturaCulataInterna = false;
		FResInstantCilindro.TemperaturaCulataInternaINS = 0.;
		FResInstantCilindro.TemperaturaCulataMedia = false;
		FResInstantCilindro.TemperaturaCulataMediaINS = 0.;
		FResInstantCilindro.TemperaturaCulataExterna = false;
		FResInstantCilindro.TemperaturaCulataExternaINS = 0.;
		FResInstantCilindro.NIT = false;
		FResInstantCilindro.GastoCortocircuito = false;
		FResInstantCilindro.GastoCortocircuitoINS = 0.;
		FResInstantCilindro.ParInstantaneo = false;
		FResInstantCilindro.ParInstantaneoINS = 0.;
		FResInstantCilindro.GastoBlowBy = false;
		FResInstantCilindro.GastoBlowByINS = 0.;
		FResInstantCilindro.FraccionMasica = false;
		FResInstantCilindro.FraccionINS = new double[FMotor->getSpeciesNumber() - FIntEGR];
		for(int i = 0; i < FMotor->getSpeciesNumber() - FIntEGR; i++) {
			FResInstantCilindro.FraccionINS[i] = 0.;
		}
		FResInstantCilindro.Gamma = false;
		FResInstantCilindro.GammaINS = 0.;

		FResInstantCilindro.HeatHead = false;
		FResInstantCilindro.HeatHeadINS = 0;
		FResInstantCilindro.HeatCyl = false;
		FResInstantCilindro.HeatCylINS = 0;
		FResInstantCilindro.HeatPis = false;
		FResInstantCilindro.HeatPisINS = 0;

		fscanf(fich, "%d ", &FNumVarIns);
		for(int i = 0; i < FNumVarIns; i++) {
			fscanf(fich, "%d ", &var);
			switch(var) {
			case 0:
				FResInstantCilindro.Pressure = true;
				break;
			case 1:
				FResInstantCilindro.Temperature = true;
				break;
			case 2:
				FResInstantCilindro.MomentoAngularEsc = true;
				break;
			case 3:
				FResInstantCilindro.MomentoAngularAdm = true;
				break;
			case 4:
				FResInstantCilindro.GastoEsc = true;
				break;
			case 5:
				FResInstantCilindro.GastoAdm = true;
				break;
			case 6:
				FResInstantCilindro.MachEsc = true;
				break;
			case 7:
				FResInstantCilindro.MachAdm = true;
				break;
			case 8:
				FResInstantCilindro.SeccionEfectivaAdm = true;
				break;
			case 9:
				FResInstantCilindro.SeccionEfectivaEsc = true;
				break;
			case 10:
				FResInstantCilindro.Masa = true;
				break;
			case 11:
				FResInstantCilindro.Volumen = true;
				break;
			case 12:
				FResInstantCilindro.MasaCombustible = true;
				break;
			case 13:
				FResInstantCilindro.FQL = true;
				break;
			case 14:
				FResInstantCilindro.CoeficienteWoschni = true;
				break;
			case 15:
				FResInstantCilindro.TemperaturaCilindroInterna = true;
				break;
			case 16:
				FResInstantCilindro.TemperaturaCilindroMedia = true;
				break;
			case 17:
				FResInstantCilindro.TemperaturaCilindroExterna = true;
				break;
			case 18:
				FResInstantCilindro.TemperaturaPistonInterna = true;
				break;
			case 19:
				FResInstantCilindro.TemperaturaPistonMedia = true;
				break;
			case 20:
				FResInstantCilindro.TemperaturaPistonExterna = true;
				break;
			case 21:
				FResInstantCilindro.TemperaturaCulataInterna = true;
				break;
			case 22:
				FResInstantCilindro.TemperaturaCulataMedia = true;
				break;
			case 23:
				FResInstantCilindro.TemperaturaCulataExterna = true;
				break;
			case 24:
				FResInstantCilindro.NIT = true;
				break;
			case 25:
				FResInstantCilindro.ParInstantaneo = true;
				break;
			case 26:
				FResInstantCilindro.GastoCortocircuito = true;
				break;
			case 27:
				FResInstantCilindro.GastoBlowBy = true;
				break;
			case 28:
				FResInstantCilindro.FraccionMasica = true;
				break;
			case 29:
				FResInstantCilindro.Gamma = true;
				break;

			case 30:
				FResInstantCilindro.HeatHead = true;
				break;

			case 31:
				FResInstantCilindro.HeatCyl = true;
				break;

			case 32:
				FResInstantCilindro.HeatPis = true;
				break;
			}
		}

		fgetpos(fich, &filepos);
		fclose(fich);

	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::ReadInstantaneousResultsCilindro en cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro::HeaderInstantaneousResultsCilindro(stringstream& insoutput, stEspecies *DatosEspecies) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");

		if(FNumVarIns > 0) {

			char *label1, *label2, *label3;
			std::string Label;
			string kkkk;
			// cadena Label7;

			if(FResInstantCilindro.Pressure) {
				Label = "\t" + PutLabel(661) + std::to_string(FNumeroCilindro) + PutLabel(908);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.Temperature) {
				Label = "\t" + PutLabel(662) + std::to_string(FNumeroCilindro) + PutLabel(910);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.MomentoAngularEsc) {
				for(int i = 0; i < FNumeroUnionesEsc; i++) {
					Label = "\t" + PutLabel(663) + std::to_string(i) + PutLabel(651) + std::to_string(FNumeroCilindro) + PutLabel(901);
					insoutput << Label.c_str();
				}
				if(FNumeroUnionesEsc > 1) {
					Label = "\t" + PutLabel(664) + std::to_string(FNumeroCilindro) + PutLabel(901);
					insoutput << Label.c_str();
				}
			}
			if(FResInstantCilindro.MomentoAngularAdm) {
				for(int i = 0; i < FNumeroUnionesAdm; i++) {
					Label = "\t" + PutLabel(665) + std::to_string(i) + PutLabel(651) + std::to_string(FNumeroCilindro) + PutLabel(901);
					insoutput << Label.c_str();
				}
				if(FNumeroUnionesAdm > 1) {
					Label = "\t" + PutLabel(666) + std::to_string(FNumeroCilindro) + PutLabel(901);
					insoutput << Label.c_str();
				}
			}
			if(FResInstantCilindro.GastoEsc) {
				for(int i = 0; i < FNumeroUnionesEsc; i++) {
					Label = "\t" + PutLabel(667) + std::to_string(i) + PutLabel(651) + std::to_string(FNumeroCilindro) + PutLabel(904);
					insoutput << Label.c_str();
				}
				if(FNumeroUnionesEsc > 1) {
					Label = "\t" + PutLabel(668) + std::to_string(FNumeroCilindro) + PutLabel(904);
					insoutput << Label.c_str();
				}
			}
			if(FResInstantCilindro.GastoAdm) {
				for(int i = 0; i < FNumeroUnionesAdm; i++) {
					Label = "\t" + PutLabel(669) + std::to_string(i) + PutLabel(651) + std::to_string(FNumeroCilindro) + PutLabel(904);
					insoutput << Label.c_str();
				}
				if(FNumeroUnionesAdm > 1) {
					Label = "\t" + PutLabel(670) + std::to_string(FNumeroCilindro) + PutLabel(901);
					insoutput << Label.c_str();
				}
			}
			if(FResInstantCilindro.MachEsc) {
				for(int i = 0; i < FNumeroUnionesEsc; i++) {
					Label = "\t" + PutLabel(671) + std::to_string(i) + PutLabel(651) + std::to_string(FNumeroCilindro) + PutLabel(901);
					insoutput << Label.c_str();
				}
			}
			if(FResInstantCilindro.MachAdm) {
				for(int i = 0; i < FNumeroUnionesAdm; i++) {
					Label = "\t" + PutLabel(672) + std::to_string(i) + PutLabel(651) + std::to_string(FNumeroCilindro) + PutLabel(901);
					insoutput << Label.c_str();
				}
			}
			if(FResInstantCilindro.SeccionEfectivaEsc) {
				for(int i = 0; i < FNumeroUnionesEsc; i++) {
					Label = "\t" + PutLabel(673) + std::to_string(i) + PutLabel(651) + std::to_string(FNumeroCilindro) + PutLabel(915);
					insoutput << Label.c_str();
				}
				if(FNumeroUnionesEsc > 1) {
					Label = "\t" + PutLabel(674) + std::to_string(FNumeroCilindro) + PutLabel(915);
					insoutput << Label.c_str();
				}
			}
			if(FResInstantCilindro.SeccionEfectivaAdm) {
				for(int i = 0; i < FNumeroUnionesAdm; i++) {
					Label = "\t" + PutLabel(675) + std::to_string(i) + PutLabel(651) + std::to_string(FNumeroCilindro) + PutLabel(915);
					insoutput << Label.c_str();
				}
				if(FNumeroUnionesAdm > 1) {
					Label = "\t" + PutLabel(676) + std::to_string(FNumeroCilindro) + PutLabel(915);
					insoutput << Label.c_str();
				}
			}
			if(FResInstantCilindro.Masa) {
				Label = "\t" + PutLabel(677) + std::to_string(FNumeroCilindro) + PutLabel(913);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.Volumen) {
				Label = "\t" + PutLabel(678) + std::to_string(FNumeroCilindro) + PutLabel(912);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.CoeficienteWoschni) {
				Label = "\t" + PutLabel(679) + std::to_string(FNumeroCilindro) + PutLabel(911);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.MasaCombustible) {
				Label = "\t" + PutLabel(680) + std::to_string(FNumeroCilindro) + PutLabel(916);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.FQL) {
				Label = "\t" + PutLabel(681) + std::to_string(FNumeroCilindro) + PutLabel(901);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.TemperaturaCilindroInterna) {
				Label = "\t" + PutLabel(641) + std::to_string(FNumeroCilindro) + PutLabel(901);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.TemperaturaCilindroMedia) {
				Label = "\t" + PutLabel(642) + std::to_string(FNumeroCilindro) + PutLabel(910);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.TemperaturaCilindroExterna) {
				Label = "\t" + PutLabel(643) + std::to_string(FNumeroCilindro) + PutLabel(910);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.TemperaturaPistonInterna) {
				Label = "\t" + PutLabel(644) + std::to_string(FNumeroCilindro) + PutLabel(910);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.TemperaturaPistonMedia) {
				Label = "\t" + PutLabel(645) + std::to_string(FNumeroCilindro) + PutLabel(910);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.TemperaturaPistonExterna) {
				Label = "\t" + PutLabel(646) + std::to_string(FNumeroCilindro) + PutLabel(910);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.TemperaturaCulataInterna) {
				Label = "\t" + PutLabel(647) + std::to_string(FNumeroCilindro) + PutLabel(910);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.TemperaturaCulataMedia) {
				Label = "\t" + PutLabel(648) + std::to_string(FNumeroCilindro) + PutLabel(910);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.TemperaturaCulataExterna) {
				Label = "\t" + PutLabel(649) + std::to_string(FNumeroCilindro) + PutLabel(910);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.NIT) {
				for(int i = 0; i < FNumeroUnionesEsc; i++) {
					Label = "\t" + PutLabel(682) + std::to_string(i) + PutLabel(651) + std::to_string(FNumeroCilindro) + PutLabel(901);
					insoutput << Label.c_str();
				}
				Label = "\t" + PutLabel(683) + std::to_string(FNumeroCilindro) + PutLabel(901);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.ParInstantaneo) {
				Label = "\t" + PutLabel(684) + std::to_string(FNumeroCilindro) + PutLabel(917);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.GastoCortocircuito) {
				Label = "\t" + PutLabel(685) + std::to_string(FNumeroCilindro) + PutLabel(904);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.GastoBlowBy) {
				Label = "\t" + PutLabel(686) + std::to_string(FNumeroCilindro) + PutLabel(904);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.FraccionMasica) {
				for(int i = 0; i < FMotor->getSpeciesNumber() - FIntEGR; i++) {
					Label = "\t" + PutLabel(687) + DatosEspecies[i].Nombre + PutLabel(651) + std::to_string(FNumeroCilindro) + PutLabel(
								901);
					insoutput << Label.c_str();
				}
			}
			if(FResInstantCilindro.Gamma) {
				Label = "\t" + PutLabel(688) + std::to_string(FNumeroCilindro) + PutLabel(901);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.HeatHead) {
				Label = "\t" + PutLabel(689) + std::to_string(FNumeroCilindro) + PutLabel(903);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.HeatCyl) {
				Label = "\t" + PutLabel(690) + std::to_string(FNumeroCilindro) + PutLabel(903);
				insoutput << Label.c_str();
			}
			if(FResInstantCilindro.HeatPis) {
				Label = "\t" + PutLabel(691) + std::to_string(FNumeroCilindro) + PutLabel(903);
				insoutput << Label.c_str();
			}
		}

		// fclose(fich);

	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::HeaderInstantaneousResultsCilindro en cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro::ImprimeResultadosInstantaneosCilindro(stringstream& insoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");

		if(FNumVarIns > 0) {

			if(FResInstantCilindro.Pressure)
				insoutput << "\t" << FResInstantCilindro.PresionINS;
			if(FResInstantCilindro.Temperature)
				insoutput << "\t" << FResInstantCilindro.TemperaturaINS;
			if(FResInstantCilindro.MomentoAngularEsc) {
				for(int i = 0; i < FNumeroUnionesEsc; i++) {
					insoutput << "\t" << FResInstantCilindro.MomentoAngularEscINS[i];
				}
				if(FNumeroUnionesEsc > 1) {
					insoutput << "\t" << FResInstantCilindro.MomentoAngularTotalEscINS;
				}
			}
			if(FResInstantCilindro.MomentoAngularAdm) {
				for(int i = 0; i < FNumeroUnionesAdm; i++) {
					insoutput << "\t" << FResInstantCilindro.MomentoAngularAdmINS[i];
				}
				if(FNumeroUnionesAdm > 1) {
					insoutput << "\t" << FResInstantCilindro.MomentoAngularTotalAdmINS;
				}
			}
			if(FResInstantCilindro.GastoEsc) {
				for(int i = 0; i < FNumeroUnionesEsc; i++) {
					insoutput << "\t" << FResInstantCilindro.GastoEscINS[i];
				}
				if(FNumeroUnionesEsc > 1) {
					insoutput << "\t" << FResInstantCilindro.GastoTotalEscINS;
				}
			}
			if(FResInstantCilindro.GastoAdm) {
				for(int i = 0; i < FNumeroUnionesAdm; i++) {
					insoutput << "\t" << FResInstantCilindro.GastoAdmINS[i];
				}
				if(FNumeroUnionesAdm > 1) {
					insoutput << "\t" << FResInstantCilindro.GastoTotalAdmINS;
				}
			}
			if(FResInstantCilindro.MachEsc) {
				for(int i = 0; i < FNumeroUnionesEsc; i++) {
					insoutput << "\t" << FResInstantCilindro.MachEscINS[i];
				}
			}
			if(FResInstantCilindro.MachAdm) {
				for(int i = 0; i < FNumeroUnionesAdm; i++) {
					insoutput << "\t" << FResInstantCilindro.MachAdmINS[i];
				}
			}
			if(FResInstantCilindro.SeccionEfectivaEsc) {
				for(int i = 0; i < FNumeroUnionesEsc; i++) {
					insoutput << "\t" << FResInstantCilindro.SeccionEfectivaEscINS[i];
				}
				if(FNumeroUnionesEsc > 1) {
					insoutput << "\t" << FResInstantCilindro.SeccionEfectivaTotalEscINS;
				}
			}
			if(FResInstantCilindro.SeccionEfectivaAdm) {
				for(int i = 0; i < FNumeroUnionesAdm; i++) {
					insoutput << "\t" << FResInstantCilindro.SeccionEfectivaAdmINS[i];
				}
				if(FNumeroUnionesAdm > 1) {
					insoutput << "\t" << FResInstantCilindro.SeccionEfectivaTotalAdmINS;
				}
			}
			if(FResInstantCilindro.Masa)
				insoutput << "\t" << FResInstantCilindro.MasaINS;
			if(FResInstantCilindro.Volumen)
				insoutput << "\t" << FResInstantCilindro.VolumenINS;
			if(FResInstantCilindro.CoeficienteWoschni)
				insoutput << "\t" << FResInstantCilindro.CoeficienteWoschniINS;
			if(FResInstantCilindro.MasaCombustible)
				insoutput << "\t" << FResInstantCilindro.MasaCombustibleINS;
			if(FResInstantCilindro.FQL)
				insoutput << "\t" << FResInstantCilindro.FQLINS;
			if(FResInstantCilindro.TemperaturaCilindroInterna)
				insoutput << "\t" << FResInstantCilindro.TemperaturaCilindroInternaINS;
			if(FResInstantCilindro.TemperaturaCilindroMedia)
				insoutput << "\t" << FResInstantCilindro.TemperaturaCilindroMediaINS;
			if(FResInstantCilindro.TemperaturaCilindroExterna)
				insoutput << "\t" << FResInstantCilindro.TemperaturaCilindroExternaINS;
			if(FResInstantCilindro.TemperaturaPistonInterna)
				insoutput << "\t" << FResInstantCilindro.TemperaturaPistonInternaINS;
			if(FResInstantCilindro.TemperaturaPistonMedia)
				insoutput << "\t" << FResInstantCilindro.TemperaturaPistonMediaINS;
			if(FResInstantCilindro.TemperaturaPistonExterna)
				insoutput << "\t" << FResInstantCilindro.TemperaturaPistonExternaINS;
			if(FResInstantCilindro.TemperaturaCulataInterna)
				insoutput << "\t" << FResInstantCilindro.TemperaturaCulataInternaINS;
			if(FResInstantCilindro.TemperaturaCulataMedia)
				insoutput << "\t" << FResInstantCilindro.TemperaturaCulataMediaINS;
			if(FResInstantCilindro.TemperaturaCulataExterna)
				insoutput << "\t" << FResInstantCilindro.TemperaturaCulataExternaINS;
			if(FResInstantCilindro.NIT) {
				for(int i = 0; i < FNumeroUnionesEsc; i++) {
					insoutput << "\t" << FResInstantCilindro.NITINS[i];
				}
				insoutput << "\t" << FResInstantCilindro.NITTotalINS;
			}
			if(FResInstantCilindro.ParInstantaneo)
				insoutput << "\t" << FResInstantCilindro.ParInstantaneoINS;
			if(FResInstantCilindro.GastoCortocircuito)
				insoutput << "\t" << FResInstantCilindro.GastoCortocircuitoINS;
			if(FResInstantCilindro.GastoBlowBy)
				insoutput << "\t" << FResInstantCilindro.GastoBlowByINS;
			if(FResInstantCilindro.FraccionMasica) {
				for(int i = 0; i < FMotor->getSpeciesNumber() - FIntEGR; i++) {
					insoutput << "\t" << FResInstantCilindro.FraccionINS[i];
				}
			}
			if(FResInstantCilindro.Gamma)
				insoutput << "\t" << FResInstantCilindro.GammaINS;

			if(FResInstantCilindro.HeatHead)
				insoutput << "\t" << FResInstantCilindro.HeatHeadINS;
			if(FResInstantCilindro.HeatCyl)
				insoutput << "\t" << FResInstantCilindro.HeatCylINS;
			if(FResInstantCilindro.HeatPis)
				insoutput << "\t" << FResInstantCilindro.HeatPisINS;

		}

		// fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::HeaderInstantaneousResultsCilindro en cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro::CalculaResultadosInstantaneosCilindro() {
	try {

		if(FNumVarIns > 0) {

			double gastoesc = 0., gastoadm = 0.;
			double secefectotaladm = 0., secefectotalesc = 0.;

			if(FResInstantCilindro.Pressure)
				FResInstantCilindro.PresionINS = FPressure;
			if(FResInstantCilindro.Temperature)
				FResInstantCilindro.TemperaturaINS = FTemperature;
			if(FResInstantCilindro.MomentoAngularEsc) {
				for(int i = 0; i < FNumeroUnionesEsc; i++) {
					FResInstantCilindro.MomentoAngularEscINS[i] = dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])->getMomento();
				}
				if(FNumeroUnionesEsc > 1) {
					FResInstantCilindro.MomentoAngularTotalEscINS = FMomentoAngularEsc;
				}
			}
			if(FResInstantCilindro.MomentoAngularAdm) {
				for(int i = 0; i < FNumeroUnionesAdm; i++) {
					FResInstantCilindro.MomentoAngularAdmINS[i] = dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])->getMomento();
				}
				if(FNumeroUnionesAdm > 1) {
					FResInstantCilindro.MomentoAngularTotalAdmINS = FMomentoAngularAdm;
				}
			}
			if(FResInstantCilindro.GastoEsc) {
				for(int i = 0; i < FNumeroUnionesEsc; i++) {
					FResInstantCilindro.GastoEscINS[i] = dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])->getMassflow();
					gastoesc += dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])->getMassflow();
				}
				if(FNumeroUnionesEsc > 1) {
					FResInstantCilindro.GastoTotalEscINS = gastoesc;
				}
			}
			if(FResInstantCilindro.GastoAdm) {
				for(int i = 0; i < FNumeroUnionesAdm; i++) {
					FResInstantCilindro.GastoAdmINS[i] = dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])->getMassflow();
					gastoadm += dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])->getMassflow();
				}
				if(FNumeroUnionesAdm > 1) {
					FResInstantCilindro.GastoTotalAdmINS = gastoadm;
				}
			}
			if(FResInstantCilindro.MachEsc) {
				for(int i = 0; i < FNumeroUnionesEsc; i++) {
					FResInstantCilindro.MachEscINS[i] = dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])->getMach();
				}
			}
			if(FResInstantCilindro.MachAdm) {
				for(int i = 0; i < FNumeroUnionesAdm; i++) {
					FResInstantCilindro.MachAdmINS[i] = dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])->getMach();
				}
			}
			if(FResInstantCilindro.SeccionEfectivaEsc) {
				for(int i = 0; i < FNumeroUnionesEsc; i++) {
					FResInstantCilindro.SeccionEfectivaEscINS[i] = dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])->getSeccionEficaz();
					secefectotalesc += dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])->getSeccionEficaz();
				}
				if(FNumeroUnionesEsc > 1) {
					FResInstantCilindro.SeccionEfectivaTotalEscINS = secefectotalesc;
				}
			}
			if(FResInstantCilindro.SeccionEfectivaAdm) {
				for(int i = 0; i < FNumeroUnionesAdm; i++) {
					FResInstantCilindro.SeccionEfectivaAdmINS[i] = dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])->getSeccionEficaz();
					secefectotaladm += dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])->getSeccionEficaz();
				}
				if(FNumeroUnionesAdm > 1) {
					FResInstantCilindro.SeccionEfectivaTotalAdmINS = secefectotaladm;
				}
			}
			if(FResInstantCilindro.Masa)
				FResInstantCilindro.MasaINS = FMasa;
			if(FResInstantCilindro.Volumen)
				FResInstantCilindro.VolumenINS = FVolumen;
			if(FResInstantCilindro.CoeficienteWoschni)
				FResInstantCilindro.CoeficienteWoschniINS = Fh;
			if(FResInstantCilindro.MasaCombustible)
				FResInstantCilindro.MasaCombustibleINS = FFuelTotal;
			if(FResInstantCilindro.FQL)
				FResInstantCilindro.FQLINS = FCalor.FQL;
			if(FResInstantCilindro.TemperaturaCilindroInterna)
				FResInstantCilindro.TemperaturaCilindroInternaINS = FTempPared[0].Cylinder;
			if(FResInstantCilindro.TemperaturaCilindroMedia)
				FResInstantCilindro.TemperaturaCilindroMediaINS = FTempPared[1].Cylinder;
			if(FResInstantCilindro.TemperaturaCilindroExterna)
				FResInstantCilindro.TemperaturaCilindroExternaINS = FTempPared[2].Cylinder;
			if(FResInstantCilindro.TemperaturaPistonInterna)
				FResInstantCilindro.TemperaturaPistonInternaINS = FTempPared[0].Piston;
			if(FResInstantCilindro.TemperaturaPistonMedia)
				FResInstantCilindro.TemperaturaPistonMediaINS = FTempPared[1].Piston;
			if(FResInstantCilindro.TemperaturaPistonExterna)
				FResInstantCilindro.TemperaturaPistonExternaINS = FTempPared[2].Piston;
			if(FResInstantCilindro.TemperaturaCulataInterna)
				FResInstantCilindro.TemperaturaCulataInternaINS = FTempPared[0].Culata;
			if(FResInstantCilindro.TemperaturaCulataMedia)
				FResInstantCilindro.TemperaturaCulataMediaINS = FTempPared[1].Culata;
			if(FResInstantCilindro.TemperaturaCulataExterna)
				FResInstantCilindro.TemperaturaCulataExternaINS = FTempPared[2].Culata;
			if(FResInstantCilindro.NIT) {
				for(int i = 0; i < FNumeroUnionesEsc; i++) {
					FResInstantCilindro.NITINS[i] = FValvEsc[i].NIT;
				}
				FResInstantCilindro.NITTotalINS = FNIT;
			}
			if(FResInstantCilindro.ParInstantaneo)
				FResInstantCilindro.ParInstantaneoINS = FParInstantaneo;
			if(FResInstantCilindro.GastoCortocircuito)
				FResInstantCilindro.GastoCortocircuitoINS = FGastoCortocircuito;
			if(FResInstantCilindro.GastoBlowBy)
				FResInstantCilindro.GastoBlowByINS = FGastoBlowBy;
			if(FResInstantCilindro.FraccionMasica) {
				for(int i = 0; i < FMotor->getSpeciesNumber() - FIntEGR; i++) {
					FResInstantCilindro.FraccionINS[i] = FFraccionMasicaEspecie[i];
				}
			}
			if(FResInstantCilindro.Gamma)
				FResInstantCilindro.GammaINS = FGamma;
			if(FResInstantCilindro.HeatHead)
				FResInstantCilindro.HeatHeadINS = FCalor.TransCulata;
			if(FResInstantCilindro.HeatCyl)
				FResInstantCilindro.HeatCylINS = FCalor.TransCilindro;
			if(FResInstantCilindro.HeatPis)
				FResInstantCilindro.HeatPisINS = FCalor.TransPiston;

		}
	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::CalculaResultadosInstantaneosCilindro en cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TCilindro::CalculaVolumen(double AnguloActual) {
	try {
		double tt = 0., ttt = 0., xxx = 0.;
		double ret_val, deltaVol, acel = 0., c;

		double a = __units::DegToRad(AnguloActual);
		double b = FMotor->getGeometria().Biela;
		double m = FMotor->getGeometria().Carrera / 2.;
		double e = FMotor->getGeometria().Excentricidad / 1000;
		double area = __geom::Circle_area(FMotor->getGeometria().Diametro);

		double Lact = m * sqrt(pow2(1. + b / m) - pow2(e / m)) - m * (cos(a) + sqrt(pow2((b / m)) - pow2(sin(a) - e / m)));

		// Lact=b+m*(1-cos(a))-sqrt(pow(b,2.)-pow(m*sin(a),2.));
		ret_val = Lact * area + FMotor->getGeometria().VCC;

		if(FMotor->getGeometria().CoefDeformaciones != 0) {
			double aux = FMotor->getGeometria().CoefDeformaciones / FMotor->getGeometria().ModuloElasticidad * pow(
							 FMotor->getGeometria().Diametro / FMotor->getGeometria().DiametroBulon,
							 2.) * (FMotor->getGeometria().AlturaCoronaPiston + b + m);

			if(FDeltaAngulo != 0) {
				double Lant = m * sqrt(pow2(1. + b / m) - pow2(e / m)) - m * (cos(a - __units::DegToRad(FDeltaAngulo)) + sqrt(pow2((
								  b / m)) - pow2(sin(a - __units::DegToRad(FDeltaAngulo)) - e / m)));

				double Lpos = m * sqrt(pow2(1. + b / m) - pow2(e / m)) - m * (cos(a + __units::DegToRad(FDeltaAngulo)) + sqrt(pow2((
								  b / m)) - pow2(sin(a + __units::DegToRad(FDeltaAngulo)) - e / m)));

				c = Lant + Lpos - Lact * 2.;
				if(c < 1e-5)
					c = 0.;
				acel = c / pow2((FDeltaAngulo / (6. * FMotor->getRegimen())));

				double Msist = 0.33 * FMotor->getGeometria().MasaBiela + FMotor->getGeometria().MasaPistonSegmentosBulon;

				deltaVol = (area * __units::BarToPa(FPressure) + Msist * acel) * aux;
			} else {
				deltaVol = area * __units::BarToPa(FPressure) * aux;
			}

			ret_val += deltaVol;
		}
		return ret_val;
	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::CalculaVolumen en cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro::IniciaVariables() {
	try {
		double TemperaturaInicial = 0.;
		double RAtmosfera, PMAtmosfera, FraccionMolarO2, FraccionMolarH2O, FraccionMolarN2;

		if(FMotor->getTheta() > FMotor->GetDesfase(FNumeroCilindro - 1)) {
			FAnguloActual = FMotor->getTheta() - FMotor->GetDesfase(FNumeroCilindro - 1);
		} else {
			FAnguloActual = FMotor->getTheta() - FMotor->GetDesfase(FNumeroCilindro - 1) + FMotor->getAngTotalCiclo();
		}
		FNumeroCiclo = 0;

		FAnguloAnterior = 0.;
		FDeltaAngulo = 0.;
		FTime1 = 0.;
		FTime0 = 0.;
		FDeltaT = 0.;
		FVolumen = CalculaVolumen(FAnguloActual);
		FVolumen0 = FVolumen;
		FVolumenCA = CalculaVolumen(FDistribucion.CA);
		FMomentoAngular = 0.;
		FAFR = 15.;

		// Transporte de Especies Quimicas
		FComposicionSaliente.resize(FMotor->getSpeciesNumber() - FIntEGR, 0.);
		FFraccionMasicaEspecie.resize(FMotor->getSpeciesNumber() - FIntEGR, 0.);
		FFraccionComienzoCicloCerrado.resize(FMotor->getSpeciesNumber() - FIntEGR, 0.);
		FMasaEspecie.resize(FMotor->getSpeciesNumber() - FIntEGR, 0.);
		FMasaEspecieCicloCerrado.resize(3, 0.);
		FComposicionCicloCerrado.resize(3, 0.);
		for(int i = 0; i < FMotor->getSpeciesNumber() - FIntEGR; i++) {
			FFraccionMasicaEspecie[i] = FMotor->GetComposicionInicial(i);
			FComposicionSaliente[i] = FMotor->GetComposicionInicial(i);
			FFraccionComienzoCicloCerrado[i] = FMotor->GetComposicionInicial(i);
		}
		if(FMotor->getSpeciesModel() == nmCalculoCompleto) {
			if(FMotor->getSpeciesNumber() == 9) {
				FComposicionCicloCerrado[1] = 0.;
				// No llega combustible de los tubos
				FComposicionCicloCerrado[2] = FFraccionMasicaEspecie[0] / FMotor->GetComposicionAtmosfera(0); // Aire fresco
				FComposicionCicloCerrado[0] = 1 - FComposicionCicloCerrado[2];
				// Gases Quemados
			} else if(FMotor->getSpeciesNumber() == 10) {
				FComposicionCicloCerrado[1] = FFraccionMasicaEspecie[7];
				// Combustible
				FComposicionCicloCerrado[2] = FFraccionMasicaEspecie[0] / FMotor->GetComposicionAtmosfera(0); // Aire fresco
				FComposicionCicloCerrado[0] = 1 - FComposicionCicloCerrado[1] - FComposicionCicloCerrado[2]; // Gases Quemados
			}
		} else if(FMotor->getSpeciesModel() == nmCalculoSimple) {
			if(FMotor->getSpeciesNumber() == 3) {
				FComposicionCicloCerrado[1] = 0.;
				// No llega combustible de los tubos
				FComposicionCicloCerrado[2] = FFraccionMasicaEspecie[1];
				// Aire fresco
				FComposicionCicloCerrado[0] = FFraccionMasicaEspecie[0];
				// Gases Quemados
			} else if(FMotor->getSpeciesNumber() == 4) {
				FComposicionCicloCerrado[1] = FFraccionMasicaEspecie[1];
				// Combustible
				FComposicionCicloCerrado[2] = FFraccionMasicaEspecie[2];
				// Aire fresco
				FComposicionCicloCerrado[0] = FFraccionMasicaEspecie[0];
				// Gases Quemados
			}
		}

		if(FMotor->getSpeciesModel() == nmCalculoCompleto) {
			RAtmosfera = FMotor->GetComposicionAtmosfera(0) * __R::O2 + FMotor->GetComposicionAtmosfera(2) * __R::H2O +
						 (FMotor->GetComposicionAtmosfera(7) - 0.01292 * (1 - FMotor->GetComposicionAtmosfera(
									 2))) * __R::N2 + FMotor->GetComposicionAtmosfera(1) * __R::CO2 + (0.01292 * (1 - FMotor->GetComposicionAtmosfera(
												 2))) * __R::Ar;
			PMAtmosfera = __R::Universal / RAtmosfera;
			FraccionMolarO2 = FMotor->GetComposicionAtmosfera(0) * PMAtmosfera / __PM::O2;
			FraccionMolarH2O = FMotor->GetComposicionAtmosfera(2) * PMAtmosfera / __PM::H2O;
			FraccionMolarN2 = 1 - FraccionMolarO2 - FraccionMolarH2O;
			// FRelacionMolarH2O_O2=FraccionMolarH2O/FraccionMolarO2;
			FRelacionMolarN2_O2 = FraccionMolarN2 / FraccionMolarO2;
		}

		if(FMotor->getSpeciesModel() == nmCalculoCompleto) {
			if(FMotor->getSpeciesNumber() == 9) {
				FFraccionMasicaEspecieFuel = 0; // No se tiene en cuenta el combustible
			} else if(FMotor->getSpeciesNumber() == 10) {
				FFraccionMasicaEspecieFuel = FFraccionMasicaEspecie[7];
			}
			FRMezcla = CalculoCompletoRMezcla(FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2],
											  FFraccionMasicaEspecieFuel, FMotor->getGammaCalculation(),
											  FMotor->getCombustible());
			TemperaturaInicial = __units::KTodegC(__units::BarToPa(FMotor->getPresionInicial()) * FVolumen /
												  FMotor->getMasaInicial() / FRMezcla);
			FCpMezcla = CalculoCompletoCpMezcla(FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2],
												FFraccionMasicaEspecieFuel, __units::degCToK(TemperaturaInicial),
												FMotor->getGammaCalculation(), FMotor->getCombustible());
			FGamma = CalculoCompletoGamma(FRMezcla, FCpMezcla, FMotor->getGammaCalculation());

		} else if(FMotor->getSpeciesModel() == nmCalculoSimple) {

			FRMezcla = CalculoSimpleRMezcla(FFraccionMasicaEspecie[0], 0, FMotor->getGammaCalculation(), FMotor->getCombustible());
			TemperaturaInicial = __units::KTodegC(__units::BarToPa(FMotor->getPresionInicial()) * FVolumen /
												  FMotor->getMasaInicial() / FRMezcla);
			FCvMezcla = CalculoSimpleCvMezcla(__units::degCToK(TemperaturaInicial), FFraccionMasicaEspecie[0], 0,
											  FMotor->getGammaCalculation(), FMotor->getCombustible());
			FGamma = CalculoSimpleGamma(FRMezcla, FCvMezcla, FMotor->getGammaCalculation());
			FHcl = -1852564 + 2195 * (30 + 273 +
									  125); //Temperatura del combustible en K +125, que es el incremento de la temperatura de combustible hasta la tobera del inyector

		}

		FPresionRCA = FMotor->getPresionInicial();

		if(FMotor->GetDesfase(FNumeroCilindro - 1) == 0) {

			FCicloCerrado = false;
			FPressure = FMotor->getPresionInicial();
			FTemperature = 60;
			FMasa = __units::BarToPa(FPressure) * FVolumen / __units::degCToK(FTemperature) / FRMezcla;
			FMasaAtrapada = __units::BarToPa(FPressure) * FVolumen / __units::degCToK(FTemperature) / FRMezcla;
			for(int j = 0; j < FMotor->getSpeciesNumber() - FIntEGR; j++) {
				FMasaEspecie[j] = FMasa * FFraccionMasicaEspecie[j];
			}
			for(int j = 0; j < 3; j++) {
				FMasaEspecieCicloCerrado[j] = FMasa * FComposicionCicloCerrado[j];
			}
			FAsonido = sqrt(FGamma * FRMezcla * __units::degCToK(FTemperature));
		} else {
			// Ciclo cerrado. Compresion Isoentropica.
			if(FAnguloActual < 180. || FAnguloActual > 540.) {
				FCicloCerrado = true;
				FPressure = FMotor->getPresionInicial() * pow((FVolumenCA / FVolumen), FGamma);
				//FMasa = FMotor->getMasaInicial();
				FMasa = __units::BarToPa(FMotor->getPresionInicial()) * FVolumenCA / __units::degCToK(60) / FRMezcla;
				FMasaAtrapada = FMasa;
				for(int j = 0; j < FMotor->getSpeciesNumber() - FIntEGR; j++) {
					FMasaEspecie[j] = FMasa * FFraccionMasicaEspecie[j];
				}
				for(int j = 0; j < 3; j++) {
					FMasaEspecieCicloCerrado[j] = FMasa * FComposicionCicloCerrado[j];
				}

				FTemperature = __units::KTodegC(__units::BarToPa(FPressure) * FVolumen / FMasa / FRMezcla);
				// Como cambia la Temperature, cambia Cp o Cv y por tanto cambia el valor de Gamma.
				if(FMotor->getSpeciesModel() == nmCalculoCompleto) {
					if(FMotor->getSpeciesNumber() == 9) {
						FFraccionMasicaEspecieFuel = 0; // No se tiene en cuenta el combustible
					} else if(FMotor->getSpeciesNumber() == 10) {
						FFraccionMasicaEspecieFuel = FFraccionMasicaEspecie[7];
					}
					FCpMezcla = CalculoCompletoCpMezcla(FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2],
														FFraccionMasicaEspecieFuel, __units::degCToK(FTemperature),
														FMotor->getGammaCalculation(), FMotor->getCombustible());
					FGamma = CalculoCompletoGamma(FRMezcla, FCpMezcla, FMotor->getGammaCalculation());

				} else if(FMotor->getSpeciesModel() == nmCalculoSimple) {

					FCvMezcla = CalculoSimpleCvMezcla(__units::degCToK(FTemperature), FFraccionMasicaEspecie[0], 0,
													  FMotor->getGammaCalculation(), FMotor->getCombustible());
					FGamma = CalculoSimpleGamma(FRMezcla, FCvMezcla, FMotor->getGammaCalculation());

				}
				FAsonido = sqrt(FGamma * FRMezcla * __units::degCToK(FTemperature));
				// Ciclo abierto. Density constante.
			} else {
				FCicloCerrado = false;
				FPressure = FMotor->getPresionInicial();
				FTemperature = 60;
				FMasa = __units::BarToPa(FPressure) * FVolumen / __units::degCToK(FTemperature) / FRMezcla;
				FMasaAtrapada = __units::BarToPa(FPressure) * FVolumen / __units::degCToK(FTemperature) / FRMezcla;
				for(int j = 0; j < FMotor->getSpeciesNumber() - FIntEGR; j++) {
					FMasaEspecie[j] = FMasa * FFraccionMasicaEspecie[j];
				}
				for(int j = 0; j < 3; j++) {
					FMasaEspecieCicloCerrado[j] = FMasa * FComposicionCicloCerrado[j];
				}
				FAsonido = sqrt(FGamma * FRMezcla * __units::degCToK(FTemperature));
			}
		}
		FMasa0 = FMasa;
		FPresion0 = FPressure;
		FTemperatura0 = FTemperature;
		FAsonido0 = FAsonido;

		if(FMotor->getCombustible() == nmMEC) {
			FMasaFuel = FMotor->getMasaFuel();
			FXComb = 10.8;
			FYComb = 18.7;
			FZComb = 0;
		} else if(FMotor->getCombustible() == nmMEP) {
			if(FMotor->getDosadoInicial() < 5) {
				FMasaFuel = FMotor->getMasaInicial() / 5.;
			} else {
				FMasaFuel = FMotor->getMasaInicial() / FMotor->getDosadoInicial();
			}
			FXComb = 7.97;
			FYComb = 14.8;
			FZComb = 0;
		} else {
			std::cout << "ERROR: Tipo de motor desconocido" << std::endl;
		}

		if(FMotor->getSpeciesModel() == nmCalculoCompleto) {
			FDosadoEstequiometrico = 1 / ((FXComb + FYComb / 4 - FZComb / 2) * __PM::O2) * (FXComb * 12.01 + FYComb * 1.01 + FZComb
									 * 16) * 0.23136;
		} else if(FMotor->getSpeciesModel() == nmCalculoSimple) {
			if(FMotor->getCombustible() == nmMEC) {
				FDosadoEstequiometrico = 0.069424352;
			} else if(FMotor->getCombustible() == nmMEP) {
				FDosadoEstequiometrico = 0.068468349;
			}
		}

		FMfint = FMasaFuel; // kg/cc
		FMaint = FMasa; // kg/cc
		FRegInt = FMotor->getRegimen();

		FMasaPorAdmision = FMasa;

		FTempPared = new stTemperaturasPared[3];
		for(int i = 0; i < 3; i++) {
			FTempPared[i].Piston = FMotor->getTempInicial().Piston;
			FTempPared[i].PistonSUMup = 0.;
			FTempPared[i].PistonSUMdown = 0.;
			FTempPared[i].Culata = FMotor->getTempInicial().Culata;
			FTempPared[i].CulataSUMup = 0.;
			FTempPared[i].CulataSUMdown = 0.;
			FTempPared[i].Cylinder = FMotor->getTempInicial().Cylinder;
			FTempPared[i].CilindroSUMup = 0.;
			FTempPared[i].CilindroSUMdown = 0.;
		}
		FTiempoCiclo = 0;

		FAngDesplazado = new double[FMotor->getLeyQuemadoBD().size()];
		FPrimeraCombustion = true;

	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::IniciaVariables en cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro::InicioFinCombustion() {
	try {
		double distReg = 0;
		double distMf = 0;
		double distMa = 0;
		double Numerador = 0;
		double Denominador = 0;
		double Ang01 = 0.;
		double dist = 0., e = 0., b = 0., DistMax = 0., DistMin = 0., Weight = 0.;
		int k = 0;

		if(FMotor->getMasaFuel() == 0.) {
			FIniComb = 0.;
			FFinComb = 0.;
		} else {

			if(FMotor->getLeyQuemadoBD().size() == 1) {
				FIniComb = FMotor->getLeyQuemadoBD()[0].Wiebes[0].Alpha0;
				FFinComb = FMotor->getLeyQuemadoBD()[0].Wiebes[FMotor->getLeyQuemadoBD()[0].Wiebes.size() - 1].Alpha0 +
						   FMotor->getLeyQuemadoBD()[0].Wiebes[FMotor->getLeyQuemadoBD()[0].Wiebes.size() - 1].IncAlpha;
			} else {

				std::vector<double> Dist;

				Dist.resize(FMotor->getLeyQuemadoBD().size());

				for(Uint i = 0; i < FMotor->getLeyQuemadoBD().size(); i++) {
					if(FMotor->getLeyQuemadoBD()[i].Wiebes.size() == 4) {
						k = 1; // Cuando hay 4 wiebes(piloto,premezcla,difusion y cola)
					} else {
						k = 0; // Cuando hay 3 wiebes(premezcla,difusion y cola)
					}
					distReg = pow2((FRegInt - FMotor->getLeyQuemadoBD()[i].n) / FMotor->getLQRegMax());
					distMf = pow2((FMfint - FMotor->getLeyQuemadoBD()[i].mf) / FMotor->getLQMfMax());
					distMa = pow2((FMaint - FMotor->getLeyQuemadoBD()[i].ma) / FMotor->getLQMaMax());
					b = distReg + distMf + distMa;

					if(b < 1e-15)
						b = 1e-15;
					Dist[i] = sqrt(b);
					if(i == 0) {
						DistMax = Dist[i];
						DistMin = Dist[i];
					} else {
						if(Dist[i] < DistMin)
							DistMin = Dist[i];
						else if(Dist[i] > DistMax)
							DistMax = Dist[i];
					}

				}
				for(Uint i = 0; i < FMotor->getLeyQuemadoBD().size(); i++) {
					Weight = pow4(((1 / Dist[i]) - (1 / DistMax)) / ((1 / DistMin) - (1 / DistMax)));
					Numerador += FMotor->getLeyQuemadoBD()[i].Wiebes[k].Alpha0 * Weight;
					Denominador += Weight;
				}

				// Media ponderada
				Ang01 = Numerador / Denominador;

				// Inicializacion de los parametros.
				FIniComb = Ang01;
				FFinComb = Ang01;

				// Busqueda del begining y el fin de la combustion
				double Finaliza = 0.;

				for(Uint i = 0; i < FMotor->getLeyQuemadoBD().size(); i++) {

					if(FMotor->getLeyQuemadoBD()[i].Wiebes.size() == 4) {
						k = 1; // Cuando hay 4 wiebes(piloto,premezcla,difusion y cola)
					} else {
						k = 0; // Cuando hay 3 wiebes(premezcla,difusion y cola)
					}
					FAngDesplazado[i] = Ang01 - FMotor->getLeyQuemadoBD()[i].Wiebes[k].Alpha0;
					/* Comentado por SHC para avanzar
					 for (Uint j = 0; j < FMotor->getLeyQuemadoBD()[i].Wiebes.size(); j++) {
					 FMotor->getLeyQuemadoBD()[i].Wiebes[j].Inicia = FMotor->getLeyQuemadoBD()[i].Wiebes[j]
					 .Alpha0 + FAngDesplazado[i];
					 }
					 */

					if(FMotor->getLeyQuemadoBD()[i].Wiebes[0].Inicia < FIniComb) {
						FIniComb = FMotor->getLeyQuemadoBD()[i].Wiebes[0].Inicia;
					}

					Finaliza = FMotor->getLeyQuemadoBD()[i].Wiebes[FMotor->getLeyQuemadoBD()[i].Wiebes.size() - 1].Inicia +
							   FMotor->getLeyQuemadoBD()[i].Wiebes[FMotor->getLeyQuemadoBD()[i].Wiebes.size() - 1].IncAlpha;

					if(Finaliza > FFinComb) {
						FFinComb = Finaliza;
					}
				}
			}
		}
	} catch(exception & N) {
		std::cout << "ERROR: AnguloInicioCombustion en el cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TCilindro::CalculaCalorLiberado(double x) {
	try {
		double Result = 0.;
		double distReg = 0;
		double distMf = 0;
		double distMa = 0;
		double Numerador = 0;
		double Denominador = 0;
		double dist = 0., e = 0., b = 0., ley = 0., DistMax = 0., DistMin = 0., Weight = 0.;

		if(FMotor->getLeyQuemadoBD().size() == 1) {
			Result = fql(x, 0);
		} else {
			std::vector<double> Dist;
			std::vector<double> LeyFQL;

			Dist.resize(FMotor->getLeyQuemadoBD().size());
			LeyFQL.resize(FMotor->getLeyQuemadoBD().size());

			for(Uint i = 0; i < FMotor->getLeyQuemadoBD().size(); i++) {
				distReg = pow2((FRegInt - FMotor->getLeyQuemadoBD()[i].n) / FMotor->getLQRegMax());
				distMf = pow2((FMfint - FMotor->getLeyQuemadoBD()[i].mf) / FMotor->getLQMfMax());
				/* : kg/cc */
				distMa = pow2((FMaint - FMotor->getLeyQuemadoBD()[i].ma) / FMotor->getLQMaMax());
				/* : kg/cc */
				// dist=pow(distReg+distMf+distMa,0.5);
				b = distReg + distMf + distMa;

				if(b < 1e-15)
					b = 1e-15;
				Dist[i] = sqrt(b);
				if(i == 0) {
					DistMax = Dist[i];
					DistMin = Dist[i];
				} else {
					if(Dist[i] < DistMin)
						DistMin = Dist[i];
					else if(Dist[i] > DistMax)
						DistMax = Dist[i];
				}
				// if(dist<1e-8)
				// dist=1e-8;
				// e=2+1/dist;
				// dist = pow( b,e);
				LeyFQL[i] = fql(x, i);
				if(LeyFQL[i] > 1.)
					printf("WARNING: Ley n. %d > 1. en %lf\n", i, x);
				// Numerador+=ley/dist;
				// Denominador+=1./dist;
			}
			for(Uint i = 0; i < FMotor->getLeyQuemadoBD().size(); i++) {
				Weight = pow4(((1 / Dist[i]) - (1 / DistMax)) / ((1 / DistMin) - (1 / DistMax)));
				Numerador += LeyFQL[i] * Weight;
				Denominador += Weight;
			}
			Result = Numerador / Denominador;
		}
		return Result;
	} catch(exception & N) {
		std::cout << "ERROR: CalculaCalorLiberado en cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TCilindro::fql(double x, int ley) {

	try {
		double ret_val = 0., wiebe = 0.;

		for(Uint k = 0; k < FMotor->getLeyQuemadoBD()[ley].Wiebes.size(); ++k) {
			wiebe = fun_wiebe(x, ley, k);
			ret_val += wiebe * FMotor->getLeyQuemadoBD()[ley].Wiebes[k].Beta;
		}

		return ret_val;

	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::fql en cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TCilindro::fun_wiebe(double x, int ley, int wiebe) {
	try {
		double ret_val = 0., xx = 0., xxx = 0.;

		if(x <= FMotor->getLeyQuemadoBD()[ley].Wiebes[wiebe].Inicia) {
			ret_val = 0.;
		} else {
			xx = (x - FMotor->getLeyQuemadoBD()[ley].Wiebes[wiebe].Inicia) / FMotor->getLeyQuemadoBD()[ley].Wiebes[wiebe].IncAlpha;
			xxx = pow(xx, FMotor->getLeyQuemadoBD()[ley].Wiebes[wiebe].m + 1);
			if((xxx * FMotor->getLeyQuemadoBD()[ley].Wiebes[wiebe].C) > 10.) {
				ret_val = 1.;
			} else {
				ret_val = 1. - 1. / exp(xxx * FMotor->getLeyQuemadoBD()[ley].Wiebes[wiebe].C);
			}
		}
		return ret_val;
	} catch(exception & N) {
		std::cout << "ERROR: TCilindro:fun_wiebe en cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

double TCilindro::FuncionGamma(double T, double X) {
	try {
		// T - Temperature en kelvin.
		// X - Tanto por uno de gases quemados.
		double a = 0., b = 0., c = 0., T00 = 0., cv = 0., Result = 0.;

		T00 = T / 100. - 2.73;
		if(fabs(X) < 1e-100 || X < 0.)
			X = 0.;
		a = 46.4 * pow(X, 0.93) + 489.6;
		b = 3.36 * pow(X, 0.8) + 7.768;
		c = 0.485 * pow075(X) + 0.0975;

		cv = (a + 2. * b * T00 - 3. * c * pow2(T00)) * 1.455;
		if(cv <= 700.) {
			cv = 700.;
		}
		Result = __R::Air / cv + 1.;
		return Result;

	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::FuncionGamma en cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro::CalculaTemperaturasPared() {
	double *TPAnt = NULL;

	try {
		double ViscGas = 0., ViscPared = 0., Vel = 0., Re = 0., Cond = 0., hExt = 0.;
		double Fo = 0., Bii = 0., Bie = 0.;
		double hi2 = 0., hi3 = 0., ho1 = 0., ho2 = 0.;
		// NUMERO DE NODOS IGUAL A 3
		TPAnt = new double[3];

		FTiempoCiclo += FDeltaT;

		// ---------------------------------------------------//
		// CILINDRO                                         //
		// ---------------------------------------------------//

		ViscGas = 3.27e-7;
		ViscPared = 1.8e-7;
		Vel = 5.64268e-7 * FMotor->getRegimen() / 60. * FMotor->getParPotMax() / FMotor->getGeometria().NCilin / pow(
				  FMotor->getGeometria().Carrera, 2.);
		Re = Vel * FMotor->getGeometria().Carrera / 2.3 / ViscGas;
		Cond = 0.67;
		hExt = 0.023 * (1 + 24.2 / pow(2.3, 0.7) / pow025(Re)) * pow(Re, 0.8) * cbrt(1.98) * pow(ViscGas / ViscPared,
				0.14) * Cond / (FMotor->getGeometria().Carrera / 2.3);

		Fo = (FMotor->getParedCilindro().Conductividad / FMotor->getParedCilindro().Density /
			  FMotor->getParedCilindro().CalorEspecifico) * FDeltaT / (pow2(FMotor->getParedCilindro().Espesor) / 4.);
		Bii = Fh * FMotor->getParedCilindro().Espesor / 2. / FMotor->getParedCilindro().Conductividad;
		Bie = hExt * FMotor->getParedCilindro().Espesor / 2. / FMotor->getParedCilindro().Conductividad;

		for(int i = 0; i < 3; i++) {
			TPAnt[i] = __units::degCToK(FTempPared[i].Cylinder);
		}

		if(Fo > 0.5 || Fo * (1 + Bii) > 0.5 || Fo * (1 + Bie) > 0.5) {
			std::cout << "WARNING: Condiciones de calculo inestables para la trans. de calor en el cilindro: " << FNumeroCilindro <<
					  std::endl;
		}

		if(FMotor->getCalculoPared() == nmConInercia && FMotor->getTheta() / 720. > FMotor->getNumCiclosSinInerciaTermica()) {
			FTempPared[2].Cylinder = __units::KTodegC(2 * Fo * (TPAnt[1] + Bie * __units::degCToK(
										 FMotor->getTempRefrigerante())) + (1 - 2 * Fo - 2 * Bie * Fo) * TPAnt[2]);
			FTempPared[1].Cylinder = __units::KTodegC(Fo * (TPAnt[0] + TPAnt[2]) + (1 - 2 * Fo) * TPAnt[1]);
			FTempPared[0].Cylinder = __units::KTodegC(2 * Fo * (TPAnt[1] + Bii * __units::degCToK(FTemperature)) +
									 (1 - 2 * Fo - 2 * Bii * Fo) * TPAnt[0]);

		} else if(FMotor->getCalculoPared() == nmSinInercia
				  || FMotor->getTheta() / 720. <= FMotor->getNumCiclosSinInerciaTermica()) {
			FTempPared[2].Cylinder = __units::KTodegC(2 * Fo * (TPAnt[1] + Bie * __units::degCToK(
										 FMotor->getTempRefrigerante())) + (1 - 2 * Fo - 2 * Bie * Fo) * TPAnt[2]);
			FTempPared[1].Cylinder = __units::KTodegC(Fo * (TPAnt[0] + TPAnt[2]) + (1 - 2 * Fo) * TPAnt[1]);
			FTempPared[0].Cylinder = __units::KTodegC(2 * Fo * (TPAnt[1] + Bii * __units::degCToK(FTemperature)) +
									 (1 - 2 * Fo - 2 * Bii * Fo) * TPAnt[0]);

			if(Fh > 0.) {
				hi2 = 1. / (1. / Fh + FMotor->getParedCilindro().Espesor / FMotor->getParedCilindro().Conductividad / 2.);
				hi3 = 1. / (1. / Fh + FMotor->getParedCilindro().Espesor / FMotor->getParedCilindro().Conductividad);
				FTempPared[0].CilindroSUMup += hi3 * __units::degCToK(FTemperature) * FDeltaT;
				FTempPared[0].CilindroSUMdown += hi3 * FDeltaT;
				FTempPared[1].CilindroSUMup += hi2 * __units::degCToK(FTemperature) * FDeltaT;
				FTempPared[1].CilindroSUMdown += hi2 * FDeltaT;
				FTempPared[2].CilindroSUMup += Fh * __units::degCToK(FTemperature) * FDeltaT;
				FTempPared[2].CilindroSUMdown += Fh * FDeltaT;
			}
			if(FAnguloActual < FAnguloAnterior && FNumeroCiclo > 1) {
				ho2 = 1 / (1 / hExt + FMotor->getParedCilindro().Espesor / FMotor->getParedCilindro().Conductividad / 2.);
				ho1 = 1 / (1 / hExt + FMotor->getParedCilindro().Espesor / FMotor->getParedCilindro().Conductividad);
				FTempPared[2].Cylinder = __units::KTodegC(
											 (FTiempoCiclo * hExt * __units::degCToK(FMotor->getTempRefrigerante()) + FTempPared[2].CilindroSUMup) /
											 (FTiempoCiclo * hExt + FTempPared[2].CilindroSUMdown));
				FTempPared[1].Cylinder = __units::KTodegC(
											 (FTiempoCiclo * ho2 * __units::degCToK(FMotor->getTempRefrigerante()) + FTempPared[1].CilindroSUMup) /
											 (FTiempoCiclo * ho2 + FTempPared[1].CilindroSUMdown));
				FTempPared[0].Cylinder = __units::KTodegC(
											 (FTiempoCiclo * ho1 * __units::degCToK(FMotor->getTempRefrigerante()) + FTempPared[0].CilindroSUMup) /
											 (FTiempoCiclo * ho1 + FTempPared[0].CilindroSUMdown));
				for(int i = 0; i < 3; i++) {
					FTempPared[i].CilindroSUMup = 0.;
					FTempPared[i].CilindroSUMdown = 0.;
				}
			}

		}

		// ---------------------------------------------------//
		// PISTON                                           //
		// ---------------------------------------------------//

		// SE HAN COMENTADO LOS CALCULOS IDENTICOS AL CILINDRO PARA AHORRAR TIEMPO DE CALCULO
		// ViscGas=3.27e-7;
		// ViscPared=1.8e-7;
		// Vel=5.64268e-7*FMotor->PutRegimen(5.64268e-7*FMotor->getRegimen()/60.*FMotor->getParPotMax()/FMotor->getGeometria().NCilin/pow(FMotor->getGeometria().Carrera,2.));
		// Re=Vel*FMotor->getGeometria().Carrera/2.3/ViscGas;
		// Cond=0.67;
		// hExt=0.023*(1+24.2/pow(2.3,0.7)/pow(Re,0.25))*pow(Re,0.8)*pow(1.98,0.33)*pow(ViscGas/ViscPared,0.14)
		// *Cond/(FMotor->getGeometria().Carrera/2.3);

		Fo = (FMotor->getParedPiston().Conductividad / FMotor->getParedPiston().Density /
			  FMotor->getParedPiston().CalorEspecifico) * FDeltaT / (pow2(FMotor->getParedPiston().Espesor) / 4.);
		Bii = Fh * FMotor->getParedPiston().Espesor / 2. / FMotor->getParedPiston().Conductividad;
		Bie = hExt * FMotor->getParedPiston().Espesor / 2. / FMotor->getParedPiston().Conductividad;

		for(int i = 0; i < 3; i++) {
			TPAnt[i] = __units::degCToK(FTempPared[i].Piston);
		}

		if(Fo > 0.5 || Fo * (1 + Bii) > 0.5 || Fo * (1 + Bie) > 0.5) {
			std::cout << "WARNING: Condiciones de calculo inestables para la trans. de calor del piston en el cilindro: " <<
					  FNumeroCilindro << std::endl;
		}

		if(FMotor->getCalculoPared() == nmConInercia && FMotor->getTheta() / 720. > FMotor->getNumCiclosSinInerciaTermica()) {
			FTempPared[2].Piston = __units::KTodegC(2 * Fo * (TPAnt[1] + Bie * __units::degCToK(FMotor->getTempRefrigerante())) +
													(1 - 2 * Fo - 2 * Bie * Fo) * TPAnt[2]);
			FTempPared[1].Piston = __units::KTodegC(Fo * (TPAnt[0] + TPAnt[2]) + (1 - 2 * Fo) * TPAnt[1]);
			FTempPared[0].Piston = __units::KTodegC(2 * Fo * (TPAnt[1] + Bii * __units::degCToK(FTemperature)) +
													(1 - 2 * Fo - 2 * Bii * Fo) * TPAnt[0]);

		} else if(FMotor->getCalculoPared() == nmSinInercia
				  || FMotor->getTheta() / 720. <= FMotor->getNumCiclosSinInerciaTermica()) {
			FTempPared[2].Piston = __units::KTodegC(2 * Fo * (TPAnt[1] + Bie * __units::degCToK(FMotor->getTempRefrigerante())) +
													(1 - 2 * Fo - 2 * Bie * Fo) * TPAnt[2]);
			FTempPared[1].Piston = __units::KTodegC(Fo * (TPAnt[0] + TPAnt[2]) + (1 - 2 * Fo) * TPAnt[1]);
			FTempPared[0].Piston = __units::KTodegC(2 * Fo * (TPAnt[1] + Bii * __units::degCToK(FTemperature)) +
													(1 - 2 * Fo - 2 * Bii * Fo) * TPAnt[0]);

			if(Fh > 0.) {
				hi2 = 1. / (1. / Fh + FMotor->getParedPiston().Espesor / FMotor->getParedPiston().Conductividad / 2.);
				hi3 = 1. / (1. / Fh + FMotor->getParedPiston().Espesor / FMotor->getParedPiston().Conductividad);
				FTempPared[0].PistonSUMup += hi3 * __units::degCToK(FTemperature) * FDeltaT;
				FTempPared[0].PistonSUMdown += hi3 * FDeltaT;
				FTempPared[1].PistonSUMup += hi2 * __units::degCToK(FTemperature) * FDeltaT;
				FTempPared[1].PistonSUMdown += hi2 * FDeltaT;
				FTempPared[2].PistonSUMup += Fh * __units::degCToK(FTemperature) * FDeltaT;
				FTempPared[2].PistonSUMdown += Fh * FDeltaT;
			}
			if(FAnguloActual < FAnguloAnterior && FNumeroCiclo > 1) {
				ho2 = 1 / (1 / hExt + FMotor->getParedPiston().Espesor / FMotor->getParedPiston().Conductividad / 2.);
				ho1 = 1 / (1 / hExt + FMotor->getParedPiston().Espesor / FMotor->getParedPiston().Conductividad);
				FTempPared[2].Piston = __units::KTodegC(
										   (FTiempoCiclo * hExt * __units::degCToK(FMotor->getTempRefrigerante()) + FTempPared[2].PistonSUMup) /
										   (FTiempoCiclo * hExt + FTempPared[2].PistonSUMdown));
				FTempPared[1].Piston = __units::KTodegC(
										   (FTiempoCiclo * ho2 * __units::degCToK(FMotor->getTempRefrigerante()) + FTempPared[1].PistonSUMup) /
										   (FTiempoCiclo * ho2 + FTempPared[1].PistonSUMdown));
				FTempPared[0].Piston = __units::KTodegC(
										   (FTiempoCiclo * ho1 * __units::degCToK(FMotor->getTempRefrigerante()) + FTempPared[0].PistonSUMup) /
										   (FTiempoCiclo * ho1 + FTempPared[0].PistonSUMdown));
				for(int i = 0; i < 3; i++) {
					FTempPared[i].PistonSUMup = 0.;
					FTempPared[i].PistonSUMdown = 0.;
				}
			}
		}

		// ---------------------------------------------------//
		// CULATA                                           //
		// ---------------------------------------------------//

		// SE HAN COMENTADO LOS CALCULOS IDENTICOS AL CILINDRO PARA AHORRAR TIEMPO DE CALCULO
		// ViscGas=3.27e-7;
		// ViscPared=1.8e-7;
		// Cond=0.67;
		Vel = 5.64268e-7 * FMotor->getRegimen() / 60. * FMotor->getParPotMax() / FMotor->getGeometria().NCilin / pow(
				  1.1 * FMotor->getGeometria().Diametro, 2.);
		Re = Vel * FMotor->getGeometria().Carrera / 2.3 / ViscGas;
		hExt = 0.023 * (1 + 24.2 / pow(2.3, 0.7) / pow025(Re)) * pow(Re, 0.8) * cbrt(1.98) * pow(ViscGas / ViscPared,
				0.14) * Cond / (1.1 * FMotor->getGeometria().Diametro / 2.3);

		Fo = (FMotor->getParedCulata().Conductividad / FMotor->getParedCulata().Density /
			  FMotor->getParedCulata().CalorEspecifico) * FDeltaT / (pow2(FMotor->getParedCulata().Espesor) / 4.);
		Bii = Fh * FMotor->getParedCulata().Espesor / 2. / FMotor->getParedCulata().Conductividad;
		Bie = hExt * FMotor->getParedCulata().Espesor / 2. / FMotor->getParedCulata().Conductividad;

		for(int i = 0; i < 3; i++) {
			TPAnt[i] = __units::degCToK(FTempPared[i].Culata);
		}

		if(Fo > 0.5 || Fo * (1 + Bii) > 0.5 || Fo * (1 + Bie) > 0.5) {
			std::cout << "WARNING: Condiciones de calculo inestables para la trans. de calor de la culata en el cilindro: " <<
					  FNumeroCilindro << std::endl;
		}

		if(FMotor->getCalculoPared() == nmConInercia && FMotor->getTheta() / 720. > FMotor->getNumCiclosSinInerciaTermica()) {
			FTempPared[2].Culata = __units::KTodegC(2 * Fo * (TPAnt[1] + Bie * __units::degCToK(FMotor->getTempRefrigerante())) +
													(1 - 2 * Fo - 2 * Bie * Fo) * TPAnt[2]);
			FTempPared[1].Culata = __units::KTodegC(Fo * (TPAnt[0] + TPAnt[2]) + (1 - 2 * Fo) * TPAnt[1]);
			FTempPared[0].Culata = __units::KTodegC(2 * Fo * (TPAnt[1] + Bii * __units::degCToK(FTemperature)) +
													(1 - 2 * Fo - 2 * Bii * Fo) * TPAnt[0]);

		} else if(FMotor->getCalculoPared() == nmSinInercia
				  || FMotor->getTheta() / 720. <= FMotor->getNumCiclosSinInerciaTermica()) {
			FTempPared[2].Culata = __units::KTodegC(2 * Fo * (TPAnt[1] + Bie * __units::degCToK(FMotor->getTempRefrigerante())) +
													(1 - 2 * Fo - 2 * Bie * Fo) * TPAnt[2]);
			FTempPared[1].Culata = __units::KTodegC(Fo * (TPAnt[0] + TPAnt[2]) + (1 - 2 * Fo) * TPAnt[1]);
			FTempPared[0].Culata = __units::KTodegC(2 * Fo * (TPAnt[1] + Bii * __units::degCToK(FTemperature)) +
													(1 - 2 * Fo - 2 * Bii * Fo) * TPAnt[0]);

			if(Fh > 0.) {
				hi2 = 1. / (1. / Fh + FMotor->getParedCulata().Espesor / FMotor->getParedCulata().Conductividad / 2.);
				hi3 = 1. / (1. / Fh + FMotor->getParedCulata().Espesor / FMotor->getParedCulata().Conductividad);
				FTempPared[0].CulataSUMup += hi3 * __units::degCToK(FTemperature) * FDeltaT;
				FTempPared[0].CulataSUMdown += hi3 * FDeltaT;
				FTempPared[1].CulataSUMup += hi2 * __units::degCToK(FTemperature) * FDeltaT;
				FTempPared[1].CulataSUMdown += hi2 * FDeltaT;
				FTempPared[2].CulataSUMup += Fh * __units::degCToK(FTemperature) * FDeltaT;
				FTempPared[2].CulataSUMdown += Fh * FDeltaT;
			}
			if(FAnguloActual < FAnguloAnterior && FNumeroCiclo > 1) {
				ho2 = 1 / (1 / hExt + FMotor->getParedCulata().Espesor / FMotor->getParedCulata().Conductividad / 2.);
				ho1 = 1 / (1 / hExt + FMotor->getParedCulata().Espesor / FMotor->getParedCulata().Conductividad);
				FTempPared[2].Culata = __units::KTodegC(
										   (FTiempoCiclo * hExt * __units::degCToK(FMotor->getTempRefrigerante()) + FTempPared[2].CulataSUMup) /
										   (FTiempoCiclo * hExt + FTempPared[2].CulataSUMdown));
				FTempPared[1].Culata = __units::KTodegC(
										   (FTiempoCiclo * ho2 * __units::degCToK(FMotor->getTempRefrigerante()) + FTempPared[1].CulataSUMup) /
										   (FTiempoCiclo * ho2 + FTempPared[1].CulataSUMdown));
				FTempPared[0].Culata = __units::KTodegC(
										   (FTiempoCiclo * ho1 * __units::degCToK(FMotor->getTempRefrigerante()) + FTempPared[0].CulataSUMup) /
										   (FTiempoCiclo * ho1 + FTempPared[0].CulataSUMdown));
				for(int i = 0; i < 3; i++) {
					FTempPared[i].CulataSUMup = 0.;
					FTempPared[i].CulataSUMdown = 0.;
				}
				FTiempoCiclo = 0.;
			}
		}

		if(TPAnt != NULL)
			delete TPAnt;

	} catch(exception & N) {
		if(TPAnt != NULL)
			delete TPAnt;
		std::cout << "ERROR: TCilindro::CalculaTemperaturasPared en cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro::CalculaFuelMEP(double MasaAire) {

	FMasaFuel = fabs(MasaAire) * FMotor->getDosadoInicial() * FDosadoEstequiometrico;
	FMotor->setMasaFuel(FMasaFuel);

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

////void TCilindro::PutMasaFuel(double valor) {
// try {
//
// FMasaFuel = valor;
//
// }
// catch(Exception & N) {
// std::cout << "ERROR: TCilindro::PutMasaFuel en el cilindro: " << FNumeroCilindro << std::endl;
// std::cout << "Tipo de error: " << N.what() << std::endl;
// throw Exception(N.what());
// }
// }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// void TCilindro::PutPresionInyeccion(double valor) {
// try {
//
// FMotor->getInjectionSys().InjectPressure = valor;
//
// }
// catch(Exception & N) {
// std::cout << "ERROR: TCilindro::PutPresionInyeccion en el cilindro: " << FNumeroCilindro << std::endl;
// std::cout << "Tipo de error: " << N.what() << std::endl;
// throw Exception(N.what());
// }
// }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// void TCilindro::PutNumeroInyecciones(int valor) {
// try {
//
// FIN = valor;
//
// }
// catch(Exception & N) {
// std::cout << "ERROR: TCilindro::PutNumeroInyecciones en el cilindro: " << FNumeroCilindro <<
// std::endl;
// std::cout << "Tipo de error: " << N.what() << std::endl;
// throw Exception(N.what());
// }
// }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// void TCilindro::PutInicioComb(double valor) {
// try {
//
// FIniComb = valor;
//
// }
// catch(Exception & N) {
// std::cout << "ERROR: TCilindro::PutInicioComb en el cilindro: " << FNumeroCilindro << std::endl;
// std::cout << "Tipo de error: " << N.what() << std::endl;
// throw Exception(N.what());
// }
// }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// void TCilindro::PutFinComb(double valor) {
// try {
//
// FFinComb = valor;
//
// }
// catch(Exception & N) {
// std::cout << "ERROR: TCilindro::PutFinComb en el cilindro: " << FNumeroCilindro << std::endl;
// std::cout << "Tipo de error: " << N.what() << std::endl;
// throw Exception(N.what());
// }
// }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// void TCilindro::PutFQL(double valor) {
// try {
//
// FCalor.FQL = valor;
//
// }
// catch(Exception & N) {
// std::cout << "ERROR: TCilindro::PutFQL en el cilindro: " << FNumeroCilindro << std::endl;
// std::cout << "Tipo de error: " << N.what() << std::endl;
// throw Exception(N.what());
// }
// }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// void TCilindro::PutModComb(bool valor) {
// try {
//
// FModComb = valor;
//
// }
// catch(Exception & N) {
// std::cout << "ERROR: TCilindro::PutModComb en el cilindro: " << FNumeroCilindro << std::endl;
// std::cout << "Tipo de error: " << N.what() << std::endl;
// throw Exception(N.what());
// }
// }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro::DefineCombustion() {
	try {
		if(FMotor->getACT()) {
			FCalcComb = nmACT;
			if(FModComb) {
				std::cout << "WARNING: Se pretende calcular la combution mediante ACT y mediante" << std::endl;
				std::cout << "         calculo externo. Se calculara unicamente con ACT." << std::endl << std::endl;
			}
		} else {
			if(FModComb) {
				FCalcComb = nmEXT;
			} else {
				FCalcComb = nmFQL;
			}
		}
	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::PutModComb en el cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

////void TCilindro::PutCalculadoPaso(bool valor) {
// try {
//
// FCalculadoPaso = valor;
//
// }
// catch(Exception & N) {
// std::cout << "ERROR: TCilindro::PutCalculadoPaso en el cilindro: " << FNumeroCilindro << std::endl;
// std::cout << "Tipo de error: " << N.what() << std::endl;
// throw Exception(N.what());
// }
// }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// void TCilindro::PutHayDLL(bool valor) {
// try {
//
// FHaydll = valor;
//
// }
// catch(Exception & N) {
// std::cout << "ERROR: TCilindro::PutHayDLL en el cilindro: " << FNumeroCilindro << std::endl;
// std::cout << "Tipo de error: " << N.what() << std::endl;
// throw Exception(N.what());
// }
// }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
double TCilindro::CalculaCTorbAdmProm() {
	try {
		double retval = 0.;
		double CTorbSum = 0.;

		for(int i = 0; i < FNumeroUnionesAdm; i++) {
			if(dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])->getValvula()->getTypeOfValve() == nmValvula4T) {
				CTorbSum += dynamic_cast<TValvula4T*>(dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])->getValvula())->getCTorbMed();
			}
		}

		retval = CTorbSum / FNumeroUnionesAdm;

		return retval;

	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::CalculaCTorbAdmProm en el cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TCilindro::CalculaCm() {
	try {
		return 2 * FMotor->getRegimen() * FMotor->getGeometria().Carrera / 60.;
	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::CalculaCm en el cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro::CalculoNIT() {
	try {
		double arriba = 0., massflow = 0.;

		// kp=1007.+(0.155+1.46/FAFR)*FTemperature;
		if(FMotor->getSpeciesModel() == nmCalculoSimple) {
			FCpMezcla = FCvMezcla + FRMezcla;
		}
		arriba = -FRMezcla / FCpMezcla;
		FNIT = FCpMezcla * __units::degCToK(FTemperature) * (1 - pow(FPressure, arriba));
		massflow = 0.;
		for(int j = 0; j < FNumeroUnionesEsc; ++j) {
			massflow += dynamic_cast<TCCCilindro*>(FCCValvulaEsc[j])->getMassflow();
			FValvEsc[j].NIT = FNIT * dynamic_cast<TCCCilindro*>(FCCValvulaEsc[j])->getMassflow();
			FValvEsc[j].NITSUM += FValvEsc[j].NIT * FDeltaT;
			// NIT medio en cada valvula
		}
		FNIT = FNIT * massflow;

	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::CalculoNIT en el cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------

void TCilindro::CalculaMomentoAngular() {
	try {
		for(int i = 0; i < FNumeroUnionesAdm; i++) {
			FMomentoAngularAdm += dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])->getMomento();
		}
		for(int i = 0; i < FNumeroUnionesEsc; i++) {
			FMomentoAngularEsc += dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])->getMomento();
		}

		FMomentoAngular = (FMomentoAngularAdm + FMomentoAngularEsc) * FDeltaT;
		/* . Se multiplica por el tiempo */

	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::CalculaMomentoAngular en el cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------

void TCilindro::SalidaGeneralCilindros() {
	try {
		if(FNumVarMed > 0) {
			printf("\n \n \n");
			std::cout << "INFO:----------------------------------------------" << std::endl;
			std::cout << "INFO: CYLINDER RESULTS" << std::endl;
			std::cout << "INFO:----------------------------------------------" << std::endl;
			std::cout << "INFO: Cylinder number " << FNumeroCilindro << " Cycle number " << FMotor->getCiclo() << std::endl;
			std::cout << "INFO:_________________________________________" << std::endl;
			if(FResMediosCilindro.RendVolumetrico)
				std::cout << "INFO: Volumetric efficiency intake manifold (refered to pipe n. " <<
						  FMotor->getTuboRendVol()->getNumeroTubo() << "): " << FResMediosCilindro.RendVolumetricoMED << " (-)" << std::endl;
			if(FResMediosCilindro.TrabajoNeto)
				std::cout << "INFO: Net work:   " << FResMediosCilindro.TrabajoNetoMED << " (J)" << std::endl;
			if(FResMediosCilindro.PresionMediaNeta)
				std::cout << "INFO: Net mean effective pressure:   " << FResMediosCilindro.PresionMediaNetaMED << " (bar)" << std::endl;
			if(FResMediosCilindro.TrabajoBombeo)
				std::cout << "INFO: Pumping work:       " << FResMediosCilindro.TrabajoBombeoMED << " (J)" << std::endl;
			if(FResMediosCilindro.PresionMediaBombeo)
				std::cout << "INFO: Pumping mean effective pressure:   " << FResMediosCilindro.PresionMediaBombeoMED << " (bar)" <<
						  std::endl;
			if(FResMediosCilindro.MasaAdmision)
				std::cout << "INFO: Intake air mass:   " << FResMediosCilindro.MasaAdmisionMED << " (kg/cc)" << std::endl;
			if(FResMediosCilindro.MasaEscape)
				std::cout << "INFO: Exhaust air mass:   " << FResMediosCilindro.MasaEscapeMED << " (kg/cc)" << std::endl;
			if(FResMediosCilindro.NITMedio)
				std::cout << "INFO: TIP in the cylinder:    " << FResMediosCilindro.NITMedioMED << " (W)" << std::endl;
			if(FResMediosCilindro.CalorCombustion)
				std::cout << "INFO: Heat released: " << FResMediosCilindro.CalorCombustionMED << " (J)" << std::endl;
			if(FResMediosCilindro.CalorCilindro)
				std::cout << "INFO: Heat transfer to cylinder liner wall: " << FResMediosCilindro.CalorCilindroMED << " (J)" <<
						  std::endl;
			if(FResMediosCilindro.CalorCulata)
				std::cout << "INFO: Heat trasnfer to cylinder head wall:   " << FResMediosCilindro.CalorCulataMED << " (J)" <<
						  std::endl;
			if(FResMediosCilindro.CalorPiston)
				std::cout << "INFO: Heat trasnfer to piston:   " << FResMediosCilindro.CalorPistonMED << " (J)" << std::endl;
			if(FResMediosCilindro.TemperaturaMedia)
				std::cout << "INFO: Mean cycle temperature:   " << FResMediosCilindro.TemperaturaMediaMED << " (C)" << std::endl;
			if(FResMediosCilindro.Swirl)
				std::cout << "INFO: Swirl at T.D.C.:      " << FResMediosCilindro.SwirlMED << " (-)" << std::endl;
			if(FResMediosCilindro.MasaAtrapada)
				std::cout << "INFO: Trapped mass:   " << FResMediosCilindro.MasaAtrapadaMED << " (kg/cc)" << std::endl;
		}

	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::SalidaGeneralCilindros en el cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro::CalculaVariablesResultados() {
	try {
		if(FResMediosCilindro.Swirl || FResInstantCilindro.MomentoAngularEsc || FResInstantCilindro.MomentoAngularEsc) {
			CalculaMomentoAngular();
		}
		if(FResInstantCilindro.NIT || FResMediosCilindro.NITMedio) {
			CalculoNIT();
		}
		if(FResMediosCilindro.Swirl) {
			CalculaSWIRL();
		}

	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::CalculaMomentoAngular en el cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro::CalculaSWIRL() {
	try {
		double wctcc = 0.;

		wctcc = __cons::Pi_4 * pow2(FMotor->getGeometria().DiametroBowl) * FMotor->getGeometria().AlturaBowl;
		wctcc = (pow2(FMotor->getGeometria().Diametro) * (FMotor->getGeometria().VCC - wctcc) + pow2(
					 FMotor->getGeometria().DiametroBowl) * wctcc) / FMotor->getGeometria().VCC / 8.;

		FWoma = FMomentoAngular / pow2(wctcc);
		FSwirl = FWoma / __units::RPMToRad_s(FMotor->getRegimen());

		FSwirlSUM += FSwirl * FDeltaT;

	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::CalculaSWIRL en el Bloque Engine. " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------

TCondicionContorno* TCilindro::GetCCValvulaAdm(int i) {
	try {
		return FCCValvulaAdm[i];
	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::GetCCValvulaAdm en el cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCondicionContorno* TCilindro::GetCCValvulaEsc(int i) {
	try {
		return FCCValvulaEsc[i];
	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::GetCCValvulaEsc en el cilindro: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TCilindro::GetAireFresco() {
	try {
		double ret_val = 0.;
		ret_val = FComposicionCicloCerrado[2];
		return ret_val;

	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::GetAireFresco en la condicion de contorno: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TCilindro::GetComposicionSaliente(int i) {
	try {
		return FComposicionSaliente[i];
	} catch(exception & N) {
		std::cout << "ERROR: TCilindro::GetComposicionSaliente en la condicion de contorno: " << FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TCilindro::PutPresionInyeccion(double valor) {

	stInjectionSys bar;
	bar = FMotor->getInjectionSys();
	bar.InjectPressure = valor;

	/////////////////////////////////// ERROR ////////////////////////////////////77
	// FMotor->getInjectionSys().InjectPressure = valor;
	/////////////////////////////////// ERROR ////////////////////////////////////77
}

void TCilindro::AsignMfController(TController **Controller) {
	if(FMfControlled)
		FMfController = Controller[FMfControllerID - 1];
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#pragma package(smart_init)
