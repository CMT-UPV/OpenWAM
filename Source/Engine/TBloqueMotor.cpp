/* --------------------------------------------------------------------------------*\
==========================|
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
#pragma hdrstop

//#include <cmath>

#include "TBloqueMotor.h"
#include "TTubo.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TBloqueMotor::TBloqueMotor(double AmbientPressure, double AmbientTemperature, nmTipoCalculoEspecies SpeciesModel,
						   int numeroespecies, nmCalculoGamma GammaCalculation, bool ThereIsEGR) {
	FMasaFuel = 0.;
	FDosadoInicial = 0.;
	FCiclo = 0;
	FPresionAmbiente = AmbientPressure;

	FCalculoGamma = GammaCalculation;

	FNumeroEspecies = numeroespecies;
	FCalculoEspecies = SpeciesModel;
	FComposicionInicial = NULL;
	FComposicionAtmosfera = NULL;

	FDesfase = NULL;
	FCilindro = NULL;

	FPrimeravezAcumulaFuel = true;

	FHayEGR = ThereIsEGR;
	if(FHayEGR)
		FIntEGR = 0;
	else
		FIntEGR = 1;

	FTemperaturaAmbiente = __units::degCToK(AmbientTemperature);
	// FTemperaturaAmbiente=273;
	// FACT=true;

	FImponerComposicionAE = false;

	FRPMControlled = false;
	FMfControlled = false;
	FInjectionSys.InjectPCtrd = false;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TBloqueMotor::~TBloqueMotor() {

	// Liberacion memoria dinamica Cilindros.
	if(FGeom.NCilin > 0 && FCilindro != NULL) {
		for(int i = 0; i < FGeom.NCilin; i++)
			delete FCilindro[i];
		delete[] FCilindro;
	}

	if(FDesfase != NULL)
		delete[] FDesfase;
	if(FComposicionInicial != NULL)
		delete[] FComposicionInicial;
	if(FComposicionAtmosfera != NULL)
		delete[] FComposicionAtmosfera;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TBloqueMotor::LeeMotor(const char *FileWAM, fpos_t &filepos, nmTipoModelado& SimulationType,
							int CiclosSinInerciaTermica, nmTipoMotor EngineType, double *AtmosphericComposition) {
	try {
		double daux = 0.;
		double ddaux = 0.;
		int tipodesfa = 0, cil = 0, tipocombustion = 0, TipoPresionAAE = 0;
		int NumeroLeyesQuemado = 0, CalculoTempPared = 0, nwiebes = 0, ImponerComposicionAE = 0;

		FTipoMotor = EngineType;
		FTipoModelado = SimulationType;
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		// -------------------------------
		// UTILIZACION COMBUSTION ACT
		// -------------------------------

		int aux = 0;
		fscanf(fich, "%d ", &aux);
		aux == 0 ? FACT = false : FACT = true;
		if(FACT) {
			fscanf(fich, "%lf ", &FMixtureProcessCte);
			cout << FMixtureProcessCte << endl;
			if(!FHayEGR) {
				std::cout << "WARNING: If the combustion is calculated by ACT and the engine" << std::endl;
				std::cout << "         has EGR, you must select the option 'To calculate" << std::endl;
				std::cout << "         EGR transport, in the other case, the results" << std::endl;
				std::cout << "         provided by ACT won't be correct" << std::endl;
			}
		}

		// -------------------------------
		// CILINDROS Y ORDEN DE ENCENDIDO
		// -------------------------------

		fscanf(fich, "%d ", &FGeom.NCilin);
		FDesfase = new double[FGeom.NCilin];

		FCilindro = new TCilindro*[FGeom.NCilin];
		if(FTipoMotor == nm2T) {
			FAngTotalCiclo = 360.;
		} else {
			FAngTotalCiclo = 720.;
		}

		// ----------------------
		// CONDICIONES INICIALES
		// ----------------------

		fscanf(fich, "%lf ", &FRegimen); // Regimen de giro motor (inicial para transitorios)
		fscanf(fich, "%lf ", &FPresionInicialRCA); // Pressure inicial al cierre de la admision
		fscanf(fich, "%lf ", &FMasaInicial); // Masa inicial en los cilindros
		FComposicionInicial = new double[FNumeroEspecies - FIntEGR];
		FComposicionAtmosfera = new double[FNumeroEspecies - FIntEGR];
		fscanf(fich, "%d ", &ImponerComposicionAE);
		ImponerComposicionAE == 0 ? FImponerComposicionAE = false : FImponerComposicionAE = true;
		for(int i = 0; i < FNumeroEspecies - 1; i++) {
			fscanf(fich, "%lf ", &FComposicionInicial[i]);
			FComposicionAtmosfera[i] = AtmosphericComposition[i];
		}

		if(FHayEGR) {
			if(FCalculoEspecies == nmCalculoCompleto) {
				FComposicionAtmosfera[FNumeroEspecies - 1] = 0.;
				if(FComposicionInicial[0] > 0.2)
					FComposicionInicial[FNumeroEspecies - 1] = 0.;
				else
					FComposicionInicial[FNumeroEspecies - 1] = 1.;
			} else {
				FComposicionAtmosfera[FNumeroEspecies - 1] = 0.;
				if(FComposicionInicial[0] > 0.5)
					FComposicionInicial[FNumeroEspecies - 1] = 1.;
				else
					FComposicionInicial[FNumeroEspecies - 1] = 0.;
			}
		}

		fscanf(fich, "%d ", &TipoPresionAAE); // Pressure impuesta en el AAE, si es 0 se calcula
		if(TipoPresionAAE == 0) {
			FCalculoDePAAE = nmPAAECalculada;
			FPresionAAE = 0;
		} else if(TipoPresionAAE == 1) {
			FCalculoDePAAE = nmPAAEImpuesta;
			fscanf(fich, "%lf ", &FPresionAAE);
		}

		fscanf(fich, "%d ", &tipocombustion);
		if(tipocombustion == 0) {
			FCombustible = nmMEC;
		} else if(tipocombustion == 1) {
			FCombustible = nmMEP;
		} else {
			std::cout << "ERROR: Tipo de combustible mal definido " << std::endl;
		}
		if(FCombustible == nmMEC) {
			fscanf(fich, "%lf ", &FMasaFuel);
		} else {
			fscanf(fich, "%lf ", &FDosadoInicial);
		}

		fscanf(fich, "%lf ", &FRendimientoCombustion);
		fscanf(fich, "%lf ", &FPoderCalorifico);
		fscanf(fich, "%lf ", &FDensidadCombustible);

		fscanf(fich, "%d ", &FNumTuboRendVol);
		// --------------------
		// PARAMETROS TERMICOS
		// --------------------

		FNumeroCiclosSinInerciaTermica = CiclosSinInerciaTermica;
		fscanf(fich, "%lf ", &FTempInicial.Piston);
		fscanf(fich, "%lf ", &FTempInicial.Culata);
		fscanf(fich, "%lf ", &FTempInicial.Cylinder);

		fscanf(fich, "%lf ", &FGeom.AreaPiston);
		fscanf(fich, "%lf ", &FGeom.AreaCulata);

		fscanf(fich, "%lf %lf %lf %lf ", &FParedPiston.Espesor, &FParedPiston.Conductividad, &FParedPiston.Density,
			   &FParedPiston.CalorEspecifico);
		fscanf(fich, "%lf %lf %lf %lf ", &FParedCulata.Espesor, &FParedCulata.Conductividad, &FParedCulata.Density,
			   &FParedCulata.CalorEspecifico);
		fscanf(fich, "%lf %lf %lf %lf ", &FParedCilindro.Espesor, &FParedCilindro.Conductividad, &FParedCilindro.Density,
			   &FParedCilindro.CalorEspecifico);

		fscanf(fich, "%lf %lf %lf %lf", &FAjusteTranCalAdm, &FAjusteTranCalEsc, &FParPotMax, &FTempRefrigerante);

		fscanf(fich, "%d ", &CalculoTempPared);
		switch(CalculoTempPared) {
		case 0:
			FCalculoPared = nmConInercia;
			break;
		case 1:
			FCalculoPared = nmSinInercia;
			break;
		case 2:
			FCalculoPared = nmTempFija;
			break;
		}

		// ------------------------------
		// WOSCHNI.TRANSMISION DE CALOR
		// ------------------------------
		fscanf(fich, "%lf ", &FWoschni.cw1);
		fscanf(fich, "%lf ", &FWoschni.cw2);
		fscanf(fich, "%lf ", &FWoschni.xpe);

		// -----------------------
		// PARAMETROS GEOMETRICOS
		// -----------------------

		fscanf(fich, "%lf ", &FGeom.Biela);
		fscanf(fich, "%lf ", &FGeom.Carrera);
		fscanf(fich, "%lf ", &FGeom.Diametro);
		fscanf(fich, "%lf ", &FGeom.RelaCompresion);
		fscanf(fich, "%lf ", &FGeom.DiametroBowl);
		fscanf(fich, "%lf ", &FGeom.AlturaBowl);
		fscanf(fich, "%lf ", &FGeom.DistanciaValvulas);
		fscanf(fich, "%lf ", &FGeom.AreaBlowBy);
		fscanf(fich, "%lf ", &FGeom.CDBlowBy);
		fscanf(fich, "%lf ", &FGeom.Excentricidad);
		fscanf(fich, "%lf ", &FGeom.DiametroBulon);
		fscanf(fich, "%lf ", &FGeom.AlturaCoronaPiston);
		fscanf(fich, "%lf ", &FGeom.MasaBiela);
		fscanf(fich, "%lf ", &FGeom.MasaPistonSegmentosBulon);
		fscanf(fich, "%lf ", &FGeom.ModuloElasticidad);
		fscanf(fich, "%lf ", &FGeom.CoefDeformaciones);

		FGeom.VCC = __geom::Cylinder_volume(FGeom.Diametro, FGeom.Carrera) / (FGeom.RelaCompresion - 1.);
		FGeom.CilindradaUnitaria = __geom::Cylinder_volume(FGeom.Diametro, FGeom.Carrera);
		FGeom.CilindradaTotal = FGeom.CilindradaUnitaria * (double) FGeom.NCilin;

		// --------------------
		// PERDIDAS MECANICAS
		// --------------------

		fscanf(fich, "%lf %lf %lf %lf ", &FPerMec.Coef0, &FPerMec.Coef1, &FPerMec.Coef2, &FPerMec.Coef3);

		// --------------------
		// MODELO DE VEHdegCULO
		// --------------------

		if(SimulationType == nmTransitorioRegimen) {
			double mv = 0., mt = 0., mr = 0.;
			double Imc = 0., Ict = 0., Itr = 0.;

			// Lectura de las masas
			fscanf(fich, "%lf %lf %lf ", &mv, &mt, &mr);
			FMasaTotalVehiculo = mv + mt + 4 * mr;

			// Lectura de las inercias
			fscanf(fich, "%lf %lf %lf ", &Imc, &Ict, &Itr);

			// Lectura caracteristicas transmision
			fscanf(fich, "%lf %lf %lf ", &FRelCajaCambios, &FRendCajaCambios, &FRelTrasmision);

			// Lectura del radio de la rueda
			fscanf(fich, "%lf ", &FRadioRueda);

			// Lectura del CrankAngle de la carretera
			fscanf(fich, "%lf ", &FAnguloCarretera);

			FInerciaTotal = pow2(FRelCajaCambios * FRelTrasmision) * Imc + pow2(FRelTrasmision) * Ict + Itr + FMasaTotalVehiculo *
							pow2(FRadioRueda);

			FPendiente = FMasaTotalVehiculo * 9.81 * sin(FAnguloCarretera);

			FCoeficienteInercias = FRendCajaCambios * pow2(FRelCajaCambios * FRelTrasmision) / (FInerciaTotal + FMasaTotalVehiculo *
								   pow2(FRadioRueda));

			// Lectura de los coeficientes para obtener el Road Load
			fscanf(fich, "%lf %lf %lf ", &FCoefRoadLoad.A0, &FCoefRoadLoad.B0, &FCoefRoadLoad.C0);
			fscanf(fich, "%lf %lf %lf %lf ", &FCoefRoadLoad.n, &FCoefRoadLoad.cd, &FCoefRoadLoad.rho, &FCoefRoadLoad.A);
		}

		if(FACT) {
			fscanf(fich, "%d ", &FInjectionSys.HoleNumber);
			fscanf(fich, "%lf ", &FInjectionSys.HoleDiame);
			fscanf(fich, "%lf ", &FInjectionSys.CDHole);
			fscanf(fich, "%lf ", &FInjectionSys.InjectPressure);

			fscanf(fich, "%lf ", &FInjectionSys.PendOpen_A1);
			fscanf(fich, "%lf ", &FInjectionSys.PendOpen_A2);
			fscanf(fich, "%lf ", &FInjectionSys.LevMax_B1);
			fscanf(fich, "%lf ", &FInjectionSys.LevMax_B2);
			fscanf(fich, "%lf ", &FInjectionSys.PendClose_C1);
			fscanf(fich, "%lf ", &FInjectionSys.PendClose_C2);
			fscanf(fich, "%lf ", &FInjectionSys.Desfase_D1);
			fscanf(fich, "%lf ", &FInjectionSys.Desfase_D2);
			fscanf(fich, "%d ", &FInjectionSys.NumPulsos);

			// FInjecPulse=new stInjecPulse[FInjectionSys.NumPulsos];
			stInjecPulse aux2;
			for(int i = 0; i < FInjectionSys.NumPulsos; ++i) {
				fscanf(fich, "%lf ", &aux2.Angulo);
				fscanf(fich, "%lf ", &aux2.Masa);
				fscanf(fich, "%d ", &aux2.CtrAngID);
				if(aux2.CtrAngID > 0)
					aux2.CtrAngd = true;
				else
					aux2.CtrAngd = false;
				fscanf(fich, "%d ", &aux2.CtrMasID);
				if(aux2.CtrMasID > 0)
					aux2.CtrMasd = true;
				else
					aux2.CtrMasd = false;
				FInjecPulse.push_back(aux2);
			}
		} else {

			// -----------------------------
			// LEYES DE LIBERACION DE CALOR
			// -----------------------------

			FLQRegMax = 0.;
			FLQMfMax = 0.;
			FLQMaMax = 0.;

			stWiebe WiebeSimple;
			stLeyQuemadoBD LeyQuemadoSimple;
			fscanf(fich, "%d ", &FNumeroLeyesQuemado);
			for(int j = 0; j < FNumeroLeyesQuemado; ++j) {
				fscanf(fich, "%lf %lf %lf", &LeyQuemadoSimple.ma, &LeyQuemadoSimple.mf, &LeyQuemadoSimple.n);
				if(LeyQuemadoSimple.ma > FLQMaMax)
					FLQMaMax = LeyQuemadoSimple.ma;
				if(LeyQuemadoSimple.mf > FLQMfMax)
					FLQMfMax = LeyQuemadoSimple.mf;
				if(LeyQuemadoSimple.n > FLQRegMax)
					FLQRegMax = LeyQuemadoSimple.n;
				fscanf(fich, "%d ", &FNWiebes);
				for(int i = 0; i < FNWiebes; i++) {
					fscanf(fich, "%lf %lf %lf %lf %lf ", &WiebeSimple.m, &WiebeSimple.C, &WiebeSimple.Beta, &WiebeSimple.IncAlpha,
						   &WiebeSimple.Alpha0);
					WiebeSimple.Alpha0 = -WiebeSimple.Alpha0;
					WiebeSimple.Inicia = WiebeSimple.Alpha0;
					LeyQuemadoSimple.Wiebes.push_back(WiebeSimple);
				}
				FLeyQuemadoBD.push_back(LeyQuemadoSimple);
				LeyQuemadoSimple.Wiebes.clear();
			}
		}

		// ------------------------------
		// INYECCIÓN DE COMBUSTIBLE.
		// ------------------------------

		fscanf(fich, "%d ", &FTipoDatosIny);
		switch(FTipoDatosIny) {
		case 0:  // No hay datos de inyeccion
			FNumeroInyecciones = 0;
			break;
		case 1:  // Datos de Angulo y tiempo de inyecciones
			fscanf(fich, "%d ", &FNumeroInyecciones); // Numero de inyecciones
			if(FNumeroInyecciones == 0) {
				FTipoDatosIny = 0;
				break;
			}
			FAngIny.resize(FNumeroInyecciones);
			FTIny.resize(FNumeroInyecciones);
			FPercentIny.resize(FNumeroInyecciones);
			for(int i = 0; i < FNumeroInyecciones; i++) {
				fscanf(fich, "%lf %lf %lf ", &FAngIny[i], &FTIny[i],
					   &FPercentIny[i]); // Angulo de la inyeccion con pms como referencia, duracion en ms y porcentaje del total
			}
			break;
		case 2:  // Datos de tabla de tasa de inyeccion
			fscanf(fich, "%lf %lf", &FAngIniIny, &FTStepIny); // Angulo de inicio de inyeccion y paso en ms entre datos de la tabla
			fscanf(fich, "%d ", &xnum);
			FY_dat.resize(xnum);
			for(int i = 0; i < xnum; i++) {
				fscanf(fich, "%lf ", &FY_dat[i]);
			}
			FX_dat.resize(xnum);
			FX_dat[0] = FAngIniIny;
			FAStepIny = FTStepIny * FRegimen / 60. * 360. / 1000.;
			for(int i = 1; i < xnum; i++) {
				FX_dat[i] = FX_dat[i - 1] + FAStepIny;
			}
//			Se comprueba que la integral de la tasa corresponde al combustible total inyectado, si no, se reescala
			for(int i = 0; i < xnum; i++) {
				FFuelTasaInt += FY_dat[i] * FTStepIny / 1000.;
			}
			for(int i = 0; i < xnum; i++) {
				FY_dat[i] = FY_dat[i] * FMasaFuel / FFuelTasaInt;
			}
			fscanf(fich, "%d ", &TipoInterp);
			switch(TipoInterp) {
			case 0:
				fTipo = nmLineal;
				fDatosTasa = new Linear_interp(FX_dat, FY_dat);
				break;
			case 1:
				fTipo = nmHermite;
				fDatosTasa = new Hermite_interp(FX_dat, FY_dat);
				break;
			case 2:
				fTipo = nmSteps;
				fDatosTasa = new Step_interp(FX_dat, FY_dat);
				break;
			}
			break;
		}

		// ------------------------------
		// CREACION OBJETO CILINDRO.
		// ------------------------------

		if(FGeom.NCilin > 1) {
			fscanf(fich, "%d ", &tipodesfa);
			switch(tipodesfa) {
			case 0:
				FTipoDesfase = nmPersonalizado;
				break;
			case 1:
				FTipoDesfase = nmImpuesto;
				break;
			}
			if(FTipoDesfase == nmPersonalizado) {
				for(int i = 0; i < FGeom.NCilin; ++i) {
					fscanf(fich, "%lf ", &FDesfase[i]);
					if(FTipoMotor == nm2T) {
						FCilindro[i] = new TCilindro2T(this, i + 1, FHayEGR);
					} else {
						FCilindro[i] = new TCilindro4T(this, i + 1, FHayEGR);
					}
				}
			} else {
				for(int i = 0; i < FGeom.NCilin; ++i) {
					fscanf(fich, "%d ", &cil);
					FDesfase[cil - 1] = (double) i * FAngTotalCiclo / (double) FGeom.NCilin;
					if(FTipoMotor == nm2T) {
						FCilindro[cil - 1] = new TCilindro2T(this, cil, FHayEGR);
					} else {
						FCilindro[cil - 1] = new TCilindro4T(this, cil, FHayEGR);
					}
				}
			}
		} else {
			FDesfase[0] = 0.;
			if(FTipoMotor == nm2T) {
				FCilindro[0] = new TCilindro2T(this, 1, FHayEGR);
			} else {
				FCilindro[0] = new TCilindro4T(this, 1, FHayEGR);
			}
		}
		int controllers = 0;
		int param = 0;
		fscanf(fich, "%d ", &controllers);
		for(int i = 0; i < controllers; ++i) {
			fscanf(fich, "%d ", &param);
			switch(param) {
			case 0: // Engine speed controller
				fscanf(fich, "%d ", &FRPMControllerID);
				FRPMControlled = true;
				SimulationType = nmTransitorioRegimen;
				break;
			case 1:
				fscanf(fich, "%d ", &FInjectionSys.InjectPCtrID);
				FInjectionSys.InjectPCtrd = true;
				break;
			}
		}

		int MfControllerID = 0;
		for(int i = 0; i < FGeom.NCilin; ++i) {
			fscanf(fich, "%d ", &controllers);
			for(int j = 0; j < controllers; ++j) {
				fscanf(fich, "%d ", &param);
				switch(param) {
				case 0: // Mass fluel controller
					fscanf(fich, "%d ", &MfControllerID);
					FCilindro[i]->PutMfControllerID(MfControllerID);
					break;
				}
			}
		}

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TBloqueMotor::LeeMotor en el Bloque Engine. " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TBloqueMotor::AsignacionTuboRendVol(TTubo **Pipe) {
	try {

		// Asignacion del tubo al que se refiere el rendimiento volumetrico.

		FTuboRendVol = Pipe[FNumTuboRendVol - 1];

		FNodoMedio = floor((FTuboRendVol->getNin()) / 2.);

	} catch(exception & N) {
		std::cout << "ERROR: TBloqueMotor::AsignacionTuboRendVol en el Bloque Engine. " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TBloqueMotor::IniciaAnguloCalculo() {
	try {
		if(FTipoMotor == nm4T) {
			FTheta = FCilindro[0]->getDistribucion().CA;
		} else if(FTipoMotor == nm2T) {
			FTheta = 256.;
		} else {
			std::cout << "ERROR: Tipo de motor mal definido" << std::endl;
		}
	} catch(exception & N) {
		std::cout << "ERROR: TBloqueMotor::IniciaAnguloCalculo en el Bloque Engine. " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TBloqueMotor::ReadAverageResultsBloqueMotor(const char *FileWAM, fpos_t &filepos) {
	try {
		int nvars = 0, Tipovar = 0;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		FResMediosMotor.ParNeto = false;
		FResMediosMotor.ParNetoSUM = 0.;
		FResMediosMotor.ParNetoMED = 0.;
		FResMediosMotor.PMN = false;
		FResMediosMotor.PMNMED = 0.;
		FResMediosMotor.ParEfectivo = false;
		FResMediosMotor.ParEfectivoSUM = 0.;
		FResMediosMotor.ParEfectivoMED = 0.;
		FResMediosMotor.ParEfectivoCiclo = false;
		FResMediosMotor.ParEfectivoCicloMED = 0.;
		FResMediosMotor.PME = false;
		FResMediosMotor.PMEMED = 0.;
		FResMediosMotor.Potencia = false;
		FResMediosMotor.PotenciaMED = 0.;
		FResMediosMotor.PotenciaCiclo = false;
		FResMediosMotor.PotenciaCicloMED = 0.;
		FResMediosMotor.MasaAdmision = false;
		FResMediosMotor.MasaAdmisionMED = 0.;
		FResMediosMotor.MasaAdmisionSUM = 0.;
		FResMediosMotor.MasaFuel = false;
		FResMediosMotor.MasaFuelMED = 0.;
		FResMediosMotor.MasaFuelSUM = 0.;
		FResMediosMotor.RegimenGiro = false;
		FResMediosMotor.RegimenGiroSUM = 0.;
		FResMediosMotor.RegimenGiroMED = 0.;
		FResMediosMotor.RendimientoVolumetrico = false;
		FResMediosMotor.RendimientoVolumetricoMED = 0.;
		FResMediosMotor.RendimientoVolumetricoAtm = false;
		FResMediosMotor.RendimientoVolumetricoAtmMED = 0.;
		FResMediosMotor.ParPerdidasMecanicas = false;
		FResMediosMotor.ParPerdidasMecanicasSUM = 0.;
		FResMediosMotor.ParPerdidasMecanicasMED = 0.;
		FResMediosMotor.ParResistente = false;
		FResMediosMotor.ParResistenteSUM = 0.;
		FResMediosMotor.ParResistenteMED = 0.;
		FResMediosMotor.VelocidadVehiculo = false;
		FResMediosMotor.VelocidadVehiculoSUM = 0.;
		FResMediosMotor.VelocidadVehiculoMED = 0.;
		FResMediosMotor.DensidadReferenciaSUM = 0.;
		FResMediosMotor.DensidadReferenciaMED = 0.;
		FResMediosMotor.MasaTuboReferenciaSUM = 0.;
		FResMediosMotor.MasaTuboReferenciaMED = 0.;
		FResMediosMotor.GastoTuboReferenciaSUM = 0.;
		FResMediosMotor.GastoTuboReferenciaMED = 0.;
		FResMediosMotor.MasaAtrapada = false;
		FResMediosMotor.MasaAtrapadaMED = 0.;
		FResMediosMotor.TrabajoNeto = false;
		FResMediosMotor.TrabajoNetoSUM = 0.;
		FResMediosMotor.TrabajoNetoMED = 0.;
		FResMediosMotor.TrabajoBombeo = false;
		FResMediosMotor.TrabajoBombeoSUM = 0.;
		FResMediosMotor.TrabajoBombeoMED = 0.;
		FResMediosMotor.PMNCiclo = false;
		FResMediosMotor.PMNCicloMED = 0.;
		FResMediosMotor.PME = false;
		FResMediosMotor.PMECicloMED = 0.;
		FResMediosMotor.PMBCiclo = false;
		FResMediosMotor.PMBCicloMED = 0.;
		FResMediosMotor.PMICiclo = false;
		FResMediosMotor.PMICicloMED = 0.;
		FResMediosMotor.RendEfectivo = false;
		FResMediosMotor.RendEfectivoMED = 0.;
		FResMediosMotor.RendIndicado = false;
		FResMediosMotor.RendIndicadoMED = 0.;
		FResMediosMotor.ConsumoEspecifico = false;
		FResMediosMotor.ConsumoEspecificoMED = 0.;
		FResMediosMotor.Dosado = false;
		FResMediosMotor.DosadoMED = 0.;
		FResMediosMotor.AFR = false;
		FResMediosMotor.AFRMED = 0.;
		FResMediosMotor.Swirl = false;
		FResMediosMotor.SwirlMED = 0.;

		FResMediosMotor.TiempoSUM = 0.;
		FResMediosMotor.Tiempo0 = 0.;

		fscanf(fich, "%d ", &nvars);
		for(int i = 0; i < nvars; i++) {
			fscanf(fich, "%d ", &Tipovar);
			switch(Tipovar) {
			case 0:
				FResMediosMotor.ParNeto = true;
				break;
			case 1:
				FResMediosMotor.ParEfectivo = true;
				break;
			case 2:
				FResMediosMotor.ParEfectivoCiclo = true;
				break;
			case 3:
				FResMediosMotor.ParPerdidasMecanicas = true;
				break;
			case 4:
				FResMediosMotor.TrabajoNeto = true;
				break;
			case 5:
				FResMediosMotor.TrabajoBombeo = true;
				break;
			case 6:
				FResMediosMotor.PMN = true;
				break;
			case 7:
				FResMediosMotor.PME = true;
				break;
			case 8:
				FResMediosMotor.PMNCiclo = true;
				break;
			case 9:
				FResMediosMotor.PMECiclo = true;
				break;
			case 10:
				FResMediosMotor.PMICiclo = true;
				break;
			case 11:
				FResMediosMotor.PMBCiclo = true;
				break;
			case 12:
				FResMediosMotor.Potencia = true;
				break;
			case 13:
				FResMediosMotor.PotenciaCiclo = true;
				break;
			case 14:
				FResMediosMotor.MasaAdmision = true;
				break;
			case 15:
				FResMediosMotor.MasaFuel = true;
				break;
			case 16:
				FResMediosMotor.MasaAtrapada = true;
				break;
			case 17:
				FResMediosMotor.RegimenGiro = true;
				break;
			case 18:
				FResMediosMotor.RendimientoVolumetrico = true;
				break;
			case 19:
				FResMediosMotor.RendimientoVolumetricoAtm = true;
				break;
			case 20:
				FResMediosMotor.RendEfectivo = true;
				break;
			case 21:
				FResMediosMotor.RendIndicado = true;
				break;
			case 22:
				FResMediosMotor.ConsumoEspecifico = true;
				break;
			case 23:
				FResMediosMotor.ParResistente = true;
				break;
			case 24:
				FResMediosMotor.VelocidadVehiculo = true;
				break;
			case 25:
				FResMediosMotor.Dosado = true;
				break;
			case 26:
				FResMediosMotor.AFR = true;
				break;
			case 27:
				FResMediosMotor.Swirl = true;
				break;
			default:
				std::cout << "Resultados medios en el motor (" << Tipovar << ")no definido" << std::endl;
			}
		}

		fgetpos(fich, &filepos);
		fclose(fich);

	} catch(exception & N) {
		std::cout << "ERROR: TBloqueMotor::ReadAverageResults en el Bloque Engine. " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TBloqueMotor::HeaderAverageResultsBloqueMotor(stringstream& medoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");
		std::string Label;

		if(FResMediosMotor.ParNeto) {
			Label = "\t" + PutLabel(601);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.ParEfectivo) {
			Label = "\t" + PutLabel(602);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.ParEfectivoCiclo) {
			Label = "\t" + PutLabel(603);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.ParPerdidasMecanicas) {
			Label = "\t" + PutLabel(604);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.TrabajoNeto) {
			Label = "\t" + PutLabel(605);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.TrabajoBombeo) {
			Label = "\t" + PutLabel(606);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.PMN) {
			Label = "\t" + PutLabel(607);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.PME) {
			Label = "\t" + PutLabel(608);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.PMNCiclo) {
			Label = "\t" + PutLabel(609);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.PMECiclo) {
			Label = "\t" + PutLabel(610);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.PMICiclo) {
			Label = "\t" + PutLabel(611);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.PMBCiclo) {
			Label = "\t" + PutLabel(612);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.Potencia) {
			Label = "\t" + PutLabel(613);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.PotenciaCiclo) {
			Label = "\t" + PutLabel(614);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.MasaAdmision) {
			Label = "\t" + PutLabel(615);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.MasaFuel) {
			Label = "\t" + PutLabel(616);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.MasaAtrapada) {
			Label = "\t" + PutLabel(617);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.RegimenGiro) {
			Label = "\t" + PutLabel(618);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.RendimientoVolumetrico) {
			Label = "\t" + PutLabel(619);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.RendimientoVolumetricoAtm) {
			Label = "\t" + PutLabel(620);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.RendEfectivo) {
			Label = "\t" + PutLabel(621);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.RendIndicado) {
			Label = "\t" + PutLabel(622);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.ConsumoEspecifico) {
			Label = "\t" + PutLabel(623);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.ParResistente) {
			Label = "\t" + PutLabel(624);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.VelocidadVehiculo) {
			Label = "\t" + PutLabel(625);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.Dosado) {
			Label = "\t" + PutLabel(626);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.AFR) {
			Label = "\t" + PutLabel(627);
			medoutput << Label.c_str();
		}
		if(FResMediosMotor.Swirl) {
			Label = "\t" + PutLabel(628);
			medoutput << Label.c_str();
		}

		// fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TBloqueMotor::HeaderAverageResults en el Bloque Engine. " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TBloqueMotor::ImprimeResultadosMediosBloqueMotor(stringstream& medoutput) {
	try {
		// FILE *fich=fopen(FileSALIDA,"a");

		if(FResMediosMotor.ParNeto)
			medoutput << "\t" << FResMediosMotor.ParNetoMED;
		if(FResMediosMotor.ParEfectivo)
			medoutput << "\t" << FResMediosMotor.ParEfectivoMED;
		if(FResMediosMotor.ParEfectivoCiclo)
			medoutput << "\t" << FResMediosMotor.ParEfectivoCicloMED;
		if(FResMediosMotor.ParPerdidasMecanicas)
			medoutput << "\t" << FResMediosMotor.ParPerdidasMecanicasMED;
		if(FResMediosMotor.TrabajoNeto)
			medoutput << "\t" << FResMediosMotor.TrabajoNetoMED;
		if(FResMediosMotor.TrabajoBombeo)
			medoutput << "\t" << FResMediosMotor.TrabajoBombeoMED;
		if(FResMediosMotor.PMN)
			medoutput << "\t" << FResMediosMotor.PMNMED;
		if(FResMediosMotor.PME)
			medoutput << "\t" << FResMediosMotor.PMEMED;
		if(FResMediosMotor.PMNCiclo)
			medoutput << "\t" << FResMediosMotor.PMNCicloMED;
		if(FResMediosMotor.PMECiclo)
			medoutput << "\t" << FResMediosMotor.PMECicloMED;
		if(FResMediosMotor.PMICiclo)
			medoutput << "\t" << FResMediosMotor.PMICicloMED;
		if(FResMediosMotor.PMBCiclo)
			medoutput << "\t" << FResMediosMotor.PMBCicloMED;
		if(FResMediosMotor.Potencia)
			medoutput << "\t" << FResMediosMotor.PotenciaMED;
		if(FResMediosMotor.PotenciaCiclo)
			medoutput << "\t" << FResMediosMotor.PotenciaCicloMED;
		if(FResMediosMotor.MasaAdmision)
			medoutput << "\t" << FResMediosMotor.MasaAdmisionMED;
		if(FResMediosMotor.MasaFuel)
			medoutput << "\t" << FResMediosMotor.MasaFuelMED;
		if(FResMediosMotor.MasaAtrapada)
			medoutput << "\t" << FResMediosMotor.MasaAtrapadaMED;
		if(FResMediosMotor.RegimenGiro)
			medoutput << "\t" << FResMediosMotor.RegimenGiroMED;
		if(FResMediosMotor.RendimientoVolumetrico)
			medoutput << "\t" << FResMediosMotor.RendimientoVolumetricoMED;
		if(FResMediosMotor.RendimientoVolumetricoAtm)
			medoutput << "\t" << FResMediosMotor.RendimientoVolumetricoAtmMED;
		if(FResMediosMotor.RendEfectivo)
			medoutput << "\t" << FResMediosMotor.RendEfectivoMED;
		if(FResMediosMotor.RendIndicado)
			medoutput << "\t" << FResMediosMotor.RendIndicadoMED;
		if(FResMediosMotor.ConsumoEspecifico)
			medoutput << "\t" << FResMediosMotor.ConsumoEspecificoMED;
		if(FResMediosMotor.ParResistente)
			medoutput << "\t" << FResMediosMotor.ParResistenteMED;
		if(FResMediosMotor.VelocidadVehiculo)
			medoutput << "\t" << FResMediosMotor.VelocidadVehiculoMED;
		if(FResMediosMotor.Dosado)
			medoutput << "\t" << FResMediosMotor.DosadoMED;
		if(FResMediosMotor.AFR)
			medoutput << "\t" << FResMediosMotor.AFRMED;
		if(FResMediosMotor.Swirl)
			medoutput << "\t" << FResMediosMotor.SwirlMED;

		// fclose(fich);
	} catch(exception & N) {
		std::cout << "ERROR: TBloqueMotor::ImprimeResultadosMediosBloqueMotor " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TBloqueMotor::ResultadosMediosBloqueMotor() {
	try {
		double DensidadAtm = 0.;
		double MasaAtrapadaSUM = 0.;
		double FraccionAireFrescoSUM = 0., AFRSUM = 0.;
		double swirltotal = 0.;

		if(FResMediosMotor.RegimenGiro || FResMediosMotor.Potencia || FResMediosMotor.RendimientoVolumetricoAtm
		   || FResMediosMotor.RendimientoVolumetrico) {
			FResMediosMotor.RegimenGiroMED = FResMediosMotor.RegimenGiroSUM / FResMediosMotor.TiempoSUM;
			FResMediosMotor.RegimenGiroSUM = 0.;
		}
		if(FResMediosMotor.ParNeto || FResMediosMotor.PMN) {
			FResMediosMotor.ParNetoMED = FResMediosMotor.ParNetoSUM / FResMediosMotor.TiempoSUM;
			FResMediosMotor.ParNetoSUM = 0.;
		}
		if(FResMediosMotor.PMN) {
			FResMediosMotor.PMNMED = FResMediosMotor.ParNetoMED * 16. / (FGeom.NCilin * pow2(FGeom.Diametro) * FGeom.Carrera) /
									 100000.;
		}
		if(FResMediosMotor.ParEfectivo || FResMediosMotor.PME || FResMediosMotor.Potencia) {
			FResMediosMotor.ParEfectivoMED = FResMediosMotor.ParEfectivoSUM / FResMediosMotor.TiempoSUM;
			FResMediosMotor.ParEfectivoSUM = 0.;
		}
		if(FResMediosMotor.PME) {
			FResMediosMotor.PMEMED = FResMediosMotor.ParEfectivoMED * 16. / (FGeom.NCilin * pow2(
										 FGeom.Diametro) * FGeom.Carrera) / 100000.;
		}
		if(FResMediosMotor.Potencia) {
			FResMediosMotor.PotenciaMED = __units::To_kilo(FResMediosMotor.ParEfectivoMED * __cons::Pi_x_2 * __units::RPMToRPS(
											  FResMediosMotor.RegimenGiroMED));
		}
		if(FResMediosMotor.MasaAdmision) {
			for(int i = 0; i < FGeom.NCilin; i++) {
				FResMediosMotor.MasaAdmisionSUM += FCilindro[i]->getMasaPorAdmision();
			}
			FResMediosMotor.MasaAdmisionMED = FResMediosMotor.MasaAdmisionSUM / FGeom.NCilin;
			FResMediosMotor.MasaAdmisionSUM = 0.;
		}

		FResMediosMotor.MasaFuelMED = FResMediosMotor.MasaFuelSUM / FGeom.NCilin;
		FResMediosMotor.MasaFuelSUM = 0.;
		FPrimeravezAcumulaFuel = true;

		if(FResMediosMotor.MasaAtrapada || FResMediosMotor.RendimientoVolumetrico || FResMediosMotor.RendimientoVolumetricoAtm
		   || FResMediosMotor.AFR) {
			for(int i = 0; i < FGeom.NCilin; i++) {
				MasaAtrapadaSUM += FCilindro[i]->getMasaAtrapada();
				FraccionAireFrescoSUM += FCilindro[i]->getFraccionAireFresco();
			}
			FResMediosMotor.MasaAtrapadaMED = MasaAtrapadaSUM / FGeom.NCilin;
			FResMediosMotor.FraccionAireFrescoMED = FraccionAireFrescoSUM / FGeom.NCilin;
		}
		if(FResMediosMotor.RendimientoVolumetrico || FResMediosMotor.RendimientoVolumetricoAtm) {
			FResMediosMotor.GastoTuboReferenciaMED = FResMediosMotor.GastoTuboReferenciaSUM / FResMediosMotor.TiempoSUM;
			FResMediosMotor.GastoTuboReferenciaSUM = 0.;
		}
		if(FResMediosMotor.RendimientoVolumetrico) {
			FResMediosMotor.DensidadReferenciaMED = FResMediosMotor.DensidadReferenciaSUM / FResMediosMotor.TiempoSUM;
			FResMediosMotor.RendimientoVolumetricoMED = fabs(FResMediosMotor.GastoTuboReferenciaMED) /
					FResMediosMotor.DensidadReferenciaMED / FGeom.CilindradaTotal / (FResMediosMotor.RegimenGiroMED / 60. *
							(360. / FAngTotalCiclo));
			FResMediosMotor.DensidadReferenciaSUM = 0;
		}
		if(FResMediosMotor.ParPerdidasMecanicas) {
			FResMediosMotor.ParPerdidasMecanicasMED = FResMediosMotor.ParPerdidasMecanicasSUM / FResMediosMotor.TiempoSUM;
			FResMediosMotor.ParPerdidasMecanicasSUM = 0.;
		}
		if(FResMediosMotor.ParResistente) {
			FResMediosMotor.ParResistenteMED = FResMediosMotor.ParResistenteSUM / FResMediosMotor.TiempoSUM;
			FResMediosMotor.ParResistenteSUM = 0.;
		}
		if(FResMediosMotor.VelocidadVehiculo) {
			FResMediosMotor.VelocidadVehiculoMED = FResMediosMotor.VelocidadVehiculoSUM / FResMediosMotor.TiempoSUM;
			FResMediosMotor.VelocidadVehiculoSUM = 0.;
		}
		if(FResMediosMotor.RendimientoVolumetricoAtm) {
			DensidadAtm = __units::BarToPa(FPresionAmbiente) / (__R::Air * FTemperaturaAmbiente);
			FResMediosMotor.RendimientoVolumetricoAtmMED = fabs(FResMediosMotor.GastoTuboReferenciaMED) / DensidadAtm /
					FGeom.CilindradaTotal / (FResMediosMotor.RegimenGiroMED / 120.);
		}
		if(FResMediosMotor.Dosado) {
			FResMediosMotor.MasaTuboReferenciaMED = FResMediosMotor.MasaTuboReferenciaSUM / FResMediosMotor.TiempoSUM;
			FResMediosMotor.DosadoMED = FResMediosMotor.MasaFuelMED / fabs(FResMediosMotor.MasaTuboReferenciaMED);
			FResMediosMotor.MasaTuboReferenciaSUM = 0.;
		}
		if(FResMediosMotor.AFR) {
			for(int i = 0; i < FGeom.NCilin; i++) {
				AFRSUM += FCilindro[i]->getAFR();
			}
			FResMediosMotor.AFRMED = AFRSUM / FGeom.NCilin;
			AFRSUM = 0.;
			// FResMediosMotor.AFRMED=FResMediosMotor.MasaAtrapadaMED*FResMediosMotor.FraccionAireFrescoMED/FResMediosMotor.MasaFuelMED;
		}
		if(FResMediosMotor.Swirl) {
			for(int i = 0; i < FGeom.NCilin; i++) {
				swirltotal += FCilindro[i]->getSwirlSUM() / FResMediosMotor.TiempoSUM;
				/* Valor medio de Swirl para el cilindro i */
			}
			FResMediosMotor.SwirlMED = swirltotal / FGeom.NCilin;
		}

		FResMediosMotor.TrabajoNetoMED = FResMediosMotor.TrabajoNetoSUM;
		FResMediosMotor.TrabajoNetoSUM = 0.;

		/* for(int i=0;i<FGeom.NCilin;i++){
		 FResMediosMotor.TrabajoBombeoSUM+=FCilindro[i]->getTrabajoBombeo();
		 } */

		FResMediosMotor.TrabajoBombeoMED = FResMediosMotor.TrabajoBombeoSUM;
		FResMediosMotor.TrabajoBombeoSUM = 0.;

		FResMediosMotor.PMNCicloMED = __units::PaToBar(FResMediosMotor.TrabajoNetoMED / FGeom.CilindradaTotal);

		FResMediosMotor.PMBCicloMED = __units::PaToBar(FResMediosMotor.TrabajoBombeoMED / FGeom.CilindradaTotal);

		FResMediosMotor.PMICicloMED = FResMediosMotor.PMNCicloMED - FResMediosMotor.PMBCicloMED;

		FPMPMMotor = FPerMec.Coef0 + FPerMec.Coef1 * FResMediosMotor.RegimenGiroMED / 60. - FPerMec.Coef2 *
					 FResMediosMotor.RegimenGiroMED * FResMediosMotor.RegimenGiroMED / 3600 + FPerMec.Coef3 * FResMediosMotor.PMICicloMED;
		FResMediosMotor.PMECicloMED = FResMediosMotor.PMNCicloMED - FPMPMMotor;

		FResMediosMotor.ParEfectivoCicloMED = __units::BarToPa(FResMediosMotor.PMECicloMED) * FGeom.CilindradaTotal /
											  (__units::DegToRad(FAngTotalCiclo));

		FResMediosMotor.PotenciaCicloMED = __units::To_kilo(__cons::Pi_x_2 * FResMediosMotor.ParEfectivoCicloMED *
										   __units::RPMToRPS(FResMediosMotor.RegimenGiroMED));

		if(FResMediosMotor.MasaFuelMED == 0) {
			FResMediosMotor.RendIndicadoMED = 0.;
			FResMediosMotor.RendEfectivoMED = 0.;
			FResMediosMotor.ConsumoEspecificoMED = 0.;
		} else {
			FResMediosMotor.RendIndicadoMED = (FResMediosMotor.TrabajoNetoMED - FResMediosMotor.TrabajoBombeoMED) /
											  (FGeom.NCilin * FResMediosMotor.MasaFuelMED * FPoderCalorifico);
			FResMediosMotor.RendEfectivoMED = __units::BarToPa(FResMediosMotor.PMECicloMED) * FGeom.CilindradaTotal /
											  (FGeom.NCilin * FResMediosMotor.MasaFuelMED * FPoderCalorifico);
			FResMediosMotor.ConsumoEspecificoMED = 3.6e9 / (FResMediosMotor.RendEfectivoMED * FPoderCalorifico);
		}

		FResMediosMotor.TiempoSUM = 0;
	} catch(exception & N) {
		std::cout << "ERROR: TBloqueMotor::ResultadosMediosBloqueMotor " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TBloqueMotor::AcumulaResultadosMediosBloqueMotor(double TActual, int CilindroActual) {
	try {
		double partotalins = 0.;

		double DeltaT = TActual - FResMediosMotor.Tiempo0;
		// double DeltaAngulo=360.*FRegimen/60.*DeltaT;

		if(FResMediosMotor.ParNeto || FResMediosMotor.PMN || FResMediosMotor.ParEfectivo || FResMediosMotor.PME
		   || FResMediosMotor.Potencia) {
			for(int i = 0; i < FGeom.NCilin; i++) {
				partotalins += FCilindro[i]->getParInstantaneo() * DeltaT;
			}
			FResMediosMotor.ParNetoSUM += partotalins;
		}

		if(FResMediosMotor.ParEfectivo || FResMediosMotor.PME || FResMediosMotor.Potencia) {
			FResMediosMotor.ParEfectivoSUM += partotalins - FParPerdidasMecanicas * DeltaT;
		}

		if(FCilindro[0]->getAnguloActual() > FCilindro[0]->getDistribucion().CA
		   && FCilindro[0]->getAnguloActual() <= FCilindro[0]->getDistribucion().CA + (FCilindro[0]->getAnguloActual() -
				   FCilindro[0]->getAnguloAnterior())) {
			if(FPrimeravezAcumulaFuel) {
				for(int i = 0; i < FGeom.NCilin; i++) {
					FResMediosMotor.MasaFuelSUM += FCilindro[i]->getMasaFuel();
				}
				FPrimeravezAcumulaFuel = false;
			}
		}

		if(FResMediosMotor.RendimientoVolumetrico || FResMediosMotor.Dosado) {
			FResMediosMotor.DensidadReferenciaSUM += FTuboRendVol->GetDensidad(FNodoMedio) * DeltaT;
			FResMediosMotor.GastoTuboReferenciaSUM += (FTuboRendVol->GetVelocidad(FNodoMedio) * __cons::ARef *
					FTuboRendVol->GetArea(FNodoMedio) * FTuboRendVol->GetDensidad(FNodoMedio)) * DeltaT;

		}

		if(FResMediosMotor.RendimientoVolumetrico || FResMediosMotor.Potencia || FResMediosMotor.RendimientoVolumetricoAtm) {
			FResMediosMotor.RegimenGiroSUM += FRegimen * DeltaT;
		}

		if(FResMediosMotor.ParPerdidasMecanicas) {
			FResMediosMotor.ParPerdidasMecanicasSUM += FParPerdidasMecanicas * DeltaT;
		}
		if(FResMediosMotor.ParResistente) {
			FResMediosMotor.ParResistenteSUM += FParResistente * DeltaT;
		}
		if(FResMediosMotor.VelocidadVehiculo) {
			FResMediosMotor.VelocidadVehiculoSUM += FVelocidadVehiculo * DeltaT;
		}
		if(FResMediosMotor.Dosado) {
			FResMediosMotor.MasaTuboReferenciaSUM += (FTuboRendVol->GetVelocidad(FNodoMedio) * __cons::ARef * FTuboRendVol->GetArea(
						FNodoMedio) * FTuboRendVol->GetDensidad(FNodoMedio)) / FGeom.NCilin / (FRegimen / 120) * DeltaT;
		}

		/* for(int i=0;i<FGeom.NCilin;i++){
		 FResMediosMotor.TrabajoNetoSUM+=FCilindro[i]->getPreMed()*1e5*(FCilindro[i]->getVolumen()-FCilindro[i]->getVolumen0());
		 if(FCilindro[i]->getAnguloActual()>180. && FCilindro[i]->getAnguloActual()<540.){
		 FResMediosMotor.TrabajoBombeoSUM+=FCilindro[i]->getPreMed()*1e5*(FCilindro[i]->getVolumen()-FCilindro[i]->getVolumen0());
		 }
		 } */
		FResMediosMotor.TrabajoNetoSUM += __units::BarToPa(FCilindro[CilindroActual - 1]->getPreMed()) *
										  (FCilindro[CilindroActual - 1]->getVolumen() - FCilindro[CilindroActual - 1]->getVolumen0());
		if(FCilindro[CilindroActual - 1]->getAnguloActual() > 180. && FCilindro[CilindroActual - 1]->getAnguloActual() < 540.) {
			FResMediosMotor.TrabajoBombeoSUM += __units::BarToPa(FCilindro[CilindroActual - 1]->getPreMed()) *
												(FCilindro[CilindroActual - 1]->getVolumen() - FCilindro[CilindroActual - 1]->getVolumen0());
		}

		FResMediosMotor.TiempoSUM += DeltaT;
		FResMediosMotor.Tiempo0 = TActual;
	} catch(exception & N) {
		std::cout << "ERROR: TBloqueMotor::AcumulaResultadosMediosBloqueMotor " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TBloqueMotor::PrestacionesMotor() {
	try {

		printf("\n \n \n");
		std::cout << "INFO:----------------------------------------------" << std::endl;
		std::cout << "INFO: ENGINE PERFORMANCE" << std::endl;
		std::cout << "INFO:----------------------------------------------" << std::endl;
		if(FResMediosMotor.ParNeto)
			std::cout << "INFO: Net torque:                             " << FResMediosMotor.ParNetoMED << " (Nm)" << std::endl;
		if(FResMediosMotor.ParEfectivo)
			std::cout << "INFO: Effective torque:                       " << FResMediosMotor.ParEfectivoMED << " (Nm)" << std::endl;
		if(FResMediosMotor.ParEfectivoCiclo)
			std::cout << "INFO: Effective torque cycle:                 " << FResMediosMotor.ParEfectivoCicloMED << " (Nm)" <<
					  std::endl;
		if(FResMediosMotor.ParPerdidasMecanicas)
			std::cout << "INFO: Torque of mechanical losses:            " << FResMediosMotor.ParPerdidasMecanicasMED << " (Nm)" <<
					  std::endl;
		if(FResMediosMotor.TrabajoNeto)
			std::cout << "INFO: Net work:                               " << FResMediosMotor.TrabajoNetoMED << " (J)" << std::endl;
		if(FResMediosMotor.TrabajoBombeo)
			std::cout << "INFO: Pumping work:                           " << FResMediosMotor.TrabajoBombeoMED << " (J)" <<
					  std::endl;
		if(FResMediosMotor.PMN)
			std::cout << "INFO: NMEP (Mechanism):                       " << FResMediosMotor.PMNMED << " (bar)" << std::endl;
		if(FResMediosMotor.PME)
			std::cout << "INFO: BMEP (Mechanism):                       " << FResMediosMotor.PMEMED << " (bar)" << std::endl;
		if(FResMediosMotor.PMNCiclo)
			std::cout << "INFO: NMEP (Cycle):                           " << FResMediosMotor.PMNCicloMED << " (bar)" << std::endl;
		if(FResMediosMotor.PMECiclo)
			std::cout << "INFO: BMEP (Cycle):                           " << FResMediosMotor.PMECicloMED << " (bar)" << std::endl;
		if(FResMediosMotor.PMICiclo)
			std::cout << "INFO: IMEP (Cycle):                           " << FResMediosMotor.PMICicloMED << " (bar)" << std::endl;
		if(FResMediosMotor.PMBCiclo)
			std::cout << "INFO: PMEP (Cycle):                           " << FResMediosMotor.PMBCicloMED << " (bar)" << std::endl;
		if(FResMediosMotor.Potencia)
			std::cout << "INFO: Effective Power (Mechanism):            " << FResMediosMotor.PotenciaMED << " (kW)" << std::endl;
		if(FResMediosMotor.PotenciaCiclo)
			std::cout << "INFO: Effective Power (Cycle):                " << FResMediosMotor.PotenciaCicloMED << " (kW)" <<
					  std::endl;
		if(FResMediosMotor.MasaAdmision)
			std::cout << "INFO: Intake Mass:                            " << FResMediosMotor.MasaAdmisionMED << " (kg/cc)" <<
					  std::endl;
		if(FResMediosMotor.MasaFuel)
			std::cout << "INFO: Fuel Mass:                              " << FResMediosMotor.MasaFuelMED << " (kg/cc)" << std::endl;
		if(FResMediosMotor.MasaAtrapada)
			std::cout << "INFO: Trapped Mass:                           " << FResMediosMotor.MasaAtrapadaMED << " (kg/cc)" <<
					  std::endl;
		if(FResMediosMotor.RegimenGiro)
			std::cout << "INFO: Engine Speed:                           " << FResMediosMotor.RegimenGiroMED << " (rpm)" <<
					  std::endl;
		if(FResMediosMotor.RendimientoVolumetrico)
			std::cout << "INFO: Volumetric Efficiency (refered pipe n. " << FTuboRendVol->getNumeroTubo() << "): " <<
					  FResMediosMotor.RendimientoVolumetricoMED << " (-)" << std::endl;
		if(FResMediosMotor.RendimientoVolumetricoAtm)
			std::cout << "INFO: Volumetric Efficiency (Ambient Condic): " << FResMediosMotor.RendimientoVolumetricoAtmMED << " (-)"
					  << std::endl;
		if(FResMediosMotor.RendEfectivo)
			std::cout << "INFO: Effective Efficiency:                   " << FResMediosMotor.RendEfectivoMED << " (-)" << std::endl;
		if(FResMediosMotor.RendIndicado)
			std::cout << "INFO: Indicated Efficiency:                   " << FResMediosMotor.RendIndicadoMED << " (-)" << std::endl;
		if(FResMediosMotor.ConsumoEspecifico)
			std::cout << "INFO: BSFC:                                   " << FResMediosMotor.ConsumoEspecificoMED << " (g/kWh)" <<
					  std::endl;
		if(FResMediosMotor.ParResistente)
			std::cout << "INFO: Resistant Torque:                       " << FResMediosMotor.ParResistenteMED << " (Nm)" <<
					  std::endl;
		if(FResMediosMotor.VelocidadVehiculo)
			std::cout << "INFO: Vehicle Speed:                          " << FResMediosMotor.VelocidadVehiculoMED << " (Nm)" <<
					  std::endl;
		if(FResMediosMotor.Dosado)
			std::cout << "INFO: Fuel-to-air ratio:(refered pipe n. " << FTuboRendVol->getNumeroTubo() << "):     " <<
					  FResMediosMotor.DosadoMED << " (-)" << std::endl;
		if(FResMediosMotor.AFR)
			std::cout << "INFO: AFR:                                    " << FResMediosMotor.AFRMED << " (-)" << std::endl;
		if(FResMediosMotor.Swirl)
			std::cout << "INFO: Swirl at T.D.C.:                        " << FResMediosMotor.SwirlMED << " (-)" << std::endl;
		std::cout << "INFO:----------------------------------------------" << std::endl;

	} catch(exception & N) {
		std::cout << "ERROR: TBloqueMotor::PrestacionesMotor en el Bloque Engine. " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TBloqueMotor::ModeloDeVehiculo(double Time) {
	try {

		double DeltaT = Time - FTime;
		FTime = Time;
		// Calculo de par proporcionado por el motor
		double ParNeto = 0;
		double W = 0.;
		for(int i = 0; i < FGeom.NCilin; i++) {
			ParNeto += FCilindro[i]->getParInstantaneo();
		}
		FParPerdidasMecanicas = __units::BarToPa(FPMPMMotor) * FGeom.CilindradaTotal / (__units::DegToRad(FAngTotalCiclo));
		FParMotor = ParNeto - FParPerdidasMecanicas;

		if(FMfControlled) {
			FMasaFuel = FMfController->Output(FTime);
		}

		if(FRPMControlled) {
			FRegimen = FRPMController->Output(FTime);
		} else {

			if(FTipoModelado == nmTransitorioRegimen) {
				// Regimen de motor en rad/s
				W = __units::RPMToRad_s(FRegimen);

				// Resistencia de la carretera
				FRoadLoad = FCoefRoadLoad.A0 + (FCoefRoadLoad.B0 * W * FRadioRueda) / (FRelCajaCambios * FRelTrasmision) +
							FCoefRoadLoad.C0 * pow(W * FRadioRueda / (FRelCajaCambios * FRelTrasmision),
												   FCoefRoadLoad.n) + (FCoefRoadLoad.cd * FCoefRoadLoad.rho * FCoefRoadLoad.A * pow2(W * FRadioRueda) / (2 * pow2(
														   FRelCajaCambios * FRelTrasmision)));

				// Par resistente total
				FParResistente = FRadioRueda * (FRoadLoad + FPendiente) / (FRendCajaCambios * FRelCajaCambios * FRelTrasmision);
				if(FTheta < 7200) {
					FRegimen = __units::Rad_sToRPM(((FParMotor - FParResistente) * FCoeficienteInercias * DeltaT + W));
				}
				FVelocidadVehiculo = __units::m_sTokm_h((__units::RPMToRad_s(FRegimen) * FRadioRueda) /
														(FRelCajaCambios * FRelTrasmision));
			}
		}

	} catch(exception & N) {
		std::cout << "ERROR: TBloqueMotor::ModeloDeVehiculo en el Bloque Engine. " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCilindro* TBloqueMotor::GetCilindro(int i) {
	try {
		return FCilindro[i];
	} catch(exception & N) {
		std::cout << "ERROR: TBloqueMotor::GetCilindro en el EngineBlock. " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TBloqueMotor::GetDesfase(int i) {
	try {
		return FDesfase[i];
	} catch(exception & N) {
		std::cout << "ERROR: TBloqueMotor::GetCilindro en el EngineBlock. " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// void TBloqueMotor::PutTheta(double valor) {
// try {
// FTheta = valor;
// }
// catch(Exception & N) {
// std::cout << "ERROR: TBloqueMotor::PutTheta en el EngineBlock. " << std::endl;
// std::cout << "Tipo de error: " << N.what() << std::endl;
// throw Exception(N.what());
// }
// }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TBloqueMotor::PutATCAdm(double valor) {
	try {

		FAjusteTranCalAdm = valor;

	} catch(exception & N) {
		std::cout << "ERROR: TBloqueMotor::PutATCAdm en el EngineBlock. " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TBloqueMotor::PutATCEsc(double valor) {
	try {

		FAjusteTranCalEsc = valor;

	} catch(exception & N) {
		std::cout << "ERROR: TBloqueMotor::PutATCEsc en el EngineBlock. " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// void TBloqueMotor::PutRegimen(double valor) {
// try {
//
// FRegimen = valor;
//
// }
// catch(Exception & N) {
// std::cout << "ERROR: TBloqueMotor::PutRegimen en el EngineBlock. " << std::endl;
// std::cout << "Tipo de error: " << N.what() << std::endl;
// throw Exception(N.what());
// }
// }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// void TBloqueMotor::PutCiclo(int valor) {
// try {
//
// FCiclo = valor;
//
// }
// catch(Exception & N) {
// std::cout << "ERROR: TBloqueMotor::PutCiclo en el EngineBlock. " << std::endl;
// std::cout << "Tipo de error: " << N.what() << std::endl;
// throw Exception(N.what());
// }
// }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TBloqueMotor::IniciaVarCilindro() {
	try {

		for(int i = 0; i < FGeom.NCilin; i++) {
			FCilindro[i]->IniciaVariables();
		}

	} catch(exception & N) {
		std::cout << "ERROR: TBloqueMotor::IniciaVarCilindro en el EngineBlock. " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TBloqueMotor::GetComposicionInicial(int i) {
	try {

		return FComposicionInicial[i];

	} catch(exception & N) {
		std::cout << "ERROR: TBloqueMotor::GetComposicionInicial en el EngineBlock. " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TBloqueMotor::GetComposicionAtmosfera(int i) {
	try {

		return FComposicionAtmosfera[i];

	} catch(exception & N) {
		std::cout << "ERROR: TBloqueMotor::GetComposicionAtmosfera en el EngineBlock. " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TBloqueMotor::AsignRPMController(TController **Controller) {
	if(FRPMControlled)
		FRPMController = Controller[FRPMControllerID - 1];
}

void TBloqueMotor::AsignMfController(TController **Controller) {
	for(int i = 0; i < FGeom.NCilin; i++) {
		FCilindro[i]->AsignMfController(Controller);
	}
	// if (FMfControlled)
	// FMfController = Controller[FMfControllerID - 1];
	if(FACT) {
		for(int i = 0; i < FInjectionSys.NumPulsos; ++i) {
			if(FInjecPulse[i].CtrAngd)
				FInjecPulse[i].CtrAng = Controller[FInjecPulse[i].CtrAngID - 1];
			if(FInjecPulse[i].CtrMasd)
				FInjecPulse[i].CtrMas = Controller[FInjecPulse[i].CtrMasID - 1];
		}
		if(FInjectionSys.InjectPCtrd)
			FInjectionSys.InjectPCtr = Controller[FInjectionSys.InjectPCtrID - 1];
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TBloqueMotor::NewInjectionData(double Time) {
	vector<stInjecPulse>::size_type i;
	for(i = 0; i < FInjecPulse.size(); i++) {
		if(FInjecPulse[i].CtrAngd)
			FInjecPulse[i].Angulo = FInjecPulse[i].CtrAng->Output(Time);
		if(FInjecPulse[i].CtrMasd)
			FInjecPulse[i].Masa = FInjecPulse[i].CtrMas->Output(Time);
	}
	if(FInjectionSys.InjectPCtrd)
		FInjectionSys.InjectPressure = FInjectionSys.InjectPCtr->Output(Time);
}

double TBloqueMotor::TasaInyInterp(double Angle) {

	fOutput = fDatosTasa->interp(Angle);

	return fOutput;
}

#pragma package(smart_init)
