/* --------------------------------------------------------------------------------*\
|==========================|
|\\   /\ /\   // O pen     | OpenWAM: The Open Source 1D Gas-Dynamic Code
| \\ |  X  | //  W ave     |
|  \\ \/_\/ //   A ction   | CMT-Motores Termicos / Universidad Politecnica Valencia
|   \\/   \//    M odel    |
|----------------------------------------------------------------------------------
|License
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
|
\*-------------------------------------------------------------------------------- */

// ---------------------------------------------------------------------------

#pragma hdrstop

#include "TCilindro2T.h"
#include "TBloqueMotor.h"
#include "TCCCilindro.h"
#include "TTubo.h"

// #include <cmath>
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCilindro2T::TCilindro2T(TBloqueMotor *Engine, int nc, bool ThereIsEGR)
: TCilindro(Engine, ThereIsEGR) {

	FNumeroCilindro = nc;
	FAcumMasaPorAdm = 0;
	FAcumMasaPorEsc = 0;
	FAcumMasaEGR = 0;

	FCalor.Liberado = 0;

	FAnguloRetrasoCombustion = 2.;
	// FAngle4S2S = 360.;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCilindro2T::~TCilindro2T() {

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro2T::ActualizaPropiedades(double TiempoActual) {
	try {
		double masavalesc, MasaAdmInstante = 0., MasaEscInstante = 0.;
		double FraccionCC = 0., MasaCortocircuitoAdm = 0.,
		MasaCortocircuitoEsc = 0.;
		int NumeroUnionesEntrante = 0;
		double FraccionMasicaAcum = 0., mfquefin = 0.;
		double z;
		double ctorbadmp;

		if (FCicloCerrado && FPrimerInstanteCicloCerrado) {

			FPrimerInstanteCicloCerrado = false;

			// C�lculo Inicio y Fin de la Combusti�n.
			FMfint = FMasaFuel; // kg/cc
			FMaint = FMasaPorAdmision; // kg/cc
			FRegInt = FMotor->getRegimen();

			if (FMotor->getACT()) {

				VariableInicialesCicloACT();

				CALCULUS_OF_INJECTION_RATE(FIN, FSOP, FMFI, FSOI, FEOI,
					Ftest_variables[0], FCAI, Ftest_variables[5], FA_TASA,
					FB_TASA, FC_TASA, FD_TASA, Finjection_rate,
					FCAD_injection_rate);

				ACT(Fengine_parameters, Fengine_model_constants,
					Ftest_variables, Finjection_rate,
					FCAD_injection_rate, Fsize_inlet_inj, FIN, FSOI, FEOI,
					FCAI, FCAD_exit, FHRF_exit, FROHR_exit, Fp_cyl_exit,
					Fdp_da_cyl_exit, FT_cyl_exit, FH_cooler_exit,
					Fmean_var_exit, Fheat_transfer,
					Finjection_rate_exit, Faccum_injection_rate_exit, dataIN,
					&dataOUT);

				// Normalize total mass fraction.
				double ACT_Correction = 0;
				for (int i = 0; i < 8; i++) {
					ACT_Correction += dataOUT.species_EVO[i];
				}
				for (int i = 0; i < 8; i++) {
					dataOUT.species_EVO[i] = dataOUT.species_EVO[i]
					/ ACT_Correction;
				}

				FPresionMedAdm = 0.;
				FPresionMedEsc = 0.;
			}

			if (FCalcComb == nmFQL) {
				InicioFinCombustion();
				FAnguloInjeccion = FIniComb - FAnguloRetrasoCombustion;
			}
			else if (FCalcComb == nmACT) {
				FAnguloInjeccion = FMotor->getInjecPulse(0).Angulo;
				FIniComb = FMotor->getInjecPulse(0).Angulo;
				FFinComb = FDistribucion.AE;
			}
			// Queda una �ltima opci�n, que se realice el c�lculo con la dll. En ese caso se el valor a
			// FIniComb y FFinComb ya se le habr� dado a partir de la DLL con una property write.

			std::cout << "INFO: Fuel mass:           " << FMasaFuel * 1e6 <<
			" (mg)" << std::endl;
			std::cout <<
			"____________________________________________________" << std::endl;
		}

		if (FMotor->getGammaCalculation() == nmGammaConstante) {
			if (FMotor->getSpeciesModel() == nmCalculoCompleto) {
				if (!FCicloCerrado) {
					FGamma = FuncionGamma(FTemperature + 273., 0.1);
				}
				else if (FCicloCerrado) {
					FGamma = FuncionGamma(FTemperature + 273.,
						FComposicionCicloCerrado[0]);
				}
			}
			else if (FMotor->getSpeciesModel() == nmCalculoSimple) {
				FGamma = FuncionGamma(FTemperature + 273.,
					FFraccionMasicaEspecie[0]);
			}
			FRMezcla = R;
		}
		else if (FMotor->getGammaCalculation()
			== nmComposicion || FMotor->getGammaCalculation()
			== nmComposicionTemperatura) {
			if (FMotor->getSpeciesModel() == nmCalculoCompleto) {

				FRMezcla = CalculoCompletoRMezcla(FFraccionMasicaEspecie[0],
					FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2],
					FMotor->getGammaCalculation());
				FCpMezcla = CalculoCompletoCpMezcla(FFraccionMasicaEspecie[0],
					FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2],
					FTemperature + 273., FMotor->getGammaCalculation());
				FGamma = CalculoCompletoGamma(FRMezcla, FCpMezcla,
					FMotor->getGammaCalculation());

			}
			else if (FMotor->getSpeciesModel() == nmCalculoSimple) {

				FRMezcla = CalculoSimpleRMezcla(FFraccionMasicaEspecie[0],
					FMotor->getGammaCalculation());
				FCvMezcla = CalculoSimpleCvMezcla(FTemperature + 273.,
					FFraccionMasicaEspecie[0], FMotor->getGammaCalculation());
				FGamma = CalculoSimpleGamma(FRMezcla, FCvMezcla,
					FMotor->getGammaCalculation());

			}
		}
		FGamma1 = Gamma1(FGamma);
		FGamma2 = Gamma2(FGamma);
		FGamma4 = Gamma4(FGamma);
		FGamma6 = Gamma6(FGamma);

		// ACTUALIZAMOS LOS VALORES DE TIEMPO Y ANGULO AL TIEMPO ACTUAL
		FTime0 = FTime1;
		FTime1 = TiempoActual;
		FDeltaT = FTime1 - FTime0;
		FDeltaAngulo = 360. * FMotor->getRegimen() / 60. * FDeltaT;
		FAnguloAnterior = FAnguloActual;
		FAnguloActual = FAnguloAnterior + FDeltaAngulo;
		if (FAnguloActual > 360.) { // Hector 2T
			FAnguloActual -= 360.; // Hector 2T
			FNumeroCiclo++;
			// std::cout << "INFO: El cilindro " << FNumeroCilindro << " comienza el ciclo " << FNumeroCiclo << std::endl;
		}
		// SE CENTRA LA COMBUSTION EN 0
		if (FAnguloActual > 180.) { // Hector 2T
			FAnguloComb0 = FAnguloActual - 360 - FDeltaAngulo; // Hector 2T
			FAnguloComb = FAnguloActual - 360.; // Hector 2T
		}
		else {
			FAnguloComb0 = FAnguloComb;
			FAnguloComb = FAnguloActual;
		}

		if (FAnguloActual >= FDistribucion.AE && FAnguloAnterior <
			FDistribucion.AE) {
			FCicloCerrado = false;
			FCalor.Liberado = 0.;
			FCalor.LiberadoTotal = 0.;
			FCalor.FQL = 0.;
			FCalor.FQL0 = 0.;
			FFuelTotal = 0;
		}

		// CALCULO DE LA PRESION MEDIA DE ADMISION Y DE ESCAPE PARA ACT
		if (FMotor->getACT()) {
			for (int i = 0; i < FNumeroUnionesAdm; i++) {
				FPresionMedAdm += FCCValvulaAdm[i]->GetTuboExtremo(0)
				.Pipe->GetPresion(0) * FDeltaT / FNumeroUnionesAdm;
			}
			for (int i = 0; i < FNumeroUnionesEsc; i++) {
				FPresionMedEsc += FCCValvulaEsc[i]->GetTuboExtremo(0)
				.Pipe->GetPresion(0) * FDeltaT / FNumeroUnionesEsc;
			}
			FTimeAcumAct += FDeltaT;
		}

		/* =============================== */
		/* BALANCE DE MASA EN EL CILINDRO */
		/* =============================== */
		FMasa0 = FMasa;

		if (!FCicloCerrado) {
			// GASTO POR ADMISION
			for (int i = 0; i < FNumeroUnionesAdm; i++) {
				FMasaValvAdm = -dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])
				->getMassflow() * FDeltaT; // signo - xq el massflow entrante se calcula negativo en la BC. As� en el cilindro ser� positivo si es entrante y viceversa.
				if (FAnguloActual - FDeltaAngulo < FDistribucion.CA &&
					FAnguloActual > FDistribucion.CA) { // FDistribucion.CA es el CrankAngle de cierre de la admision.
					FMasaValvAdm = FMasaValvAdm *
					(FDistribucion.CA - FAnguloActual + FDeltaAngulo)
					/ FDeltaAngulo;
				}
				MasaAdmInstante += FMasaValvAdm;
				FMasa += FMasaValvAdm;
				/* SUMATORIO DE LA MASA QUE ATRAVIESA CADA UNA DE LAS VALVULAS DE ADMISION */
				FAcumMasaPorAdm += FMasaValvAdm;

				/* Transporte de especies qu�micas */
				for (int j = 0; j < FMotor->getSpeciesNumber() - FIntEGR; j++) {
					FMasaEspecie[j] += FCCValvulaAdm[i]
					->GetFraccionMasicaEspecie(j) * FMasaValvAdm;
				}
				if (FHayEGR)
					FAcumMasaEGR += FCCValvulaAdm[i]->GetFraccionMasicaEspecie
						(FMotor->getSpeciesNumber() - 1) * FMasaValvAdm;
			}
			// GASTO POR ESCAPE
			for (int i = 0; i < FNumeroUnionesEsc; i++) {
				masavalesc = -dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])
					->getMassflow() * FDeltaT; // signo - xq el massflow entrante se calcula negativo en la BC. As� en el cilindro ser� positivo si es entrante y viceversa.
				if (FAnguloActual - FDeltaAngulo < FDistribucion.CE &&
					FAnguloActual > FDistribucion.CE) {
					masavalesc = masavalesc *
						(FDistribucion.CE - FAnguloActual +
						FDeltaAngulo) / FDeltaAngulo; // FDistribucion.CE es el CrankAngle de cierre del escape.
				}
				MasaEscInstante += masavalesc;
				FMasa += masavalesc;
				FAcumMasaPorEsc += masavalesc;

				/* Transporte de especies qu�micas */
				for (int j = 0; j < FMotor->getSpeciesNumber() - FIntEGR; j++) {
					FMasaEspecie[j] += FCCValvulaEsc[i]
						->GetFraccionMasicaEspecie(j) * masavalesc;
				}
			}
		}

		/* ================================= */
		/* MASA DE BLOW-BY */
		/* ================================= */
		if (FPressure > FMotor->getPresionAmb() && FMotor->getGeometria()
			.CDBlowBy > 0.) {
			if (FMotor->getPresionAmb() / FPressure < pow(2. / FGamma2,
					FGamma6)) {
				FPresionCarter = FPressure * pow(2. / FGamma2, FGamma4 / 2.);

			}
			else {
				FPresionCarter = FMotor->getPresionAmb();
			}
			z = FGamma4 * (pow(FPresionCarter / FPressure, 2 / FGamma) - pow
				(FPresionCarter / FPressure, FGamma2 / FGamma));
			FGastoBlowBy = FMotor->getGeometria()
				.CDBlowBy * 3.5e-5 * FMotor->getGeometria()
				.Diametro * FPressure * 1e5 * sqrt
				(z / FRMezcla / (FTemperature + 273.));
		}
		else {
			FGastoBlowBy = 0.;
		}
		FMasaBlowBy = FGastoBlowBy * FDeltaT;

		/* ================================= */
		/* INYECCI�N DE COMBUSTIBLE (MEC) */
		/* ================================= */

		if (FAnguloComb > FAnguloInjeccion && FAnguloComb0 <
			FAnguloInjeccion && FMotor->getCombustible() == nmMEC) {
			// En el �ngulo de begining de la combusti�n se empieza a introducir el combustible
			// Se pasa a estado de injecci�n verdadero.
			FInyeccion = true;
			FFuelTotal = 0.;
			FTasaFuel = 0.;
		}
		if (FInyeccion) {

			if (FMotor->getACT()) {
				FTasaFuel = Interp1(FAnguloComb, FCAD_injection_rate,
					Finjection_rate, FCAI) / 1000.;
				FFuelInstant = FTasaFuel * FDeltaT;

				if (FAnguloComb > FCAD_injection_rate[FCAI - 1]) {
					FInyeccion = false;
				}
			}
			else {
				// Se reparte la inyecci�n de combustible durante 1.3 ms
				// FFuelInstant representa la cantidad de masa de fuel introducida en un instante de calculo.
				FTasaFuel = FMasaFuel / 1.3e-3;
				FFuelInstant = FTasaFuel * FDeltaT;
				// Se va acumulando la masa de fuel para comprobar que no super el valor de combustible fijado por cilindro y ciclo.
			}
			if ((FFuelTotal + FFuelInstant) > FMasaFuel) {
				// La inyecci�n se corta cuando se alcanza el valor de combustible total.
				FFuelInstant = FMasaFuel - FFuelTotal;
				FInyeccion = false;
				FFuelTotal = FMasaFuel;
			}
			else {
				FFuelTotal += FFuelInstant;
			}

		}
		else {
			FFuelInstant = 0.;
		}

		FMasa = FMasa + FFuelInstant - FMasaBlowBy;

		// AL CIERRE DE LA VALVULA DE ADMISION SE OBTIENE LA MASA TOTAL POR ADMISION Y ESCAPE,
		// LA MASA ATRAPADA EN EL CILINDRO, EL COMBUSTIBLE (PARA LOS MEP EN FUNCION DE
		// LA MASA POR ADMISION) LA PRESION Y LOS ANGULOS DE INICIO Y FIN DE LA COMBUSTION
		if (FAnguloActual > FDistribucion.CA && FAnguloAnterior <=
			FDistribucion.CA && !FCicloCerrado) {

			FCicloCerrado = true;
			FPrimerInstanteCicloCerrado = true;
			FMasaPorAdmision = FAcumMasaPorAdm;
			FAcumMasaPorAdm = 0;
			FMasaPorEscape = FAcumMasaPorEsc;
			FAcumMasaPorEsc = 0;
			FMasaEGR = FAcumMasaEGR;
			FAcumMasaEGR = 0;
			FMasaAtrapada = FMasa;
			if (FMotor->getCombustible() == nmMEP) {
				CalculaFuelMEP(FMasaPorAdmision);
			}
			FPresionRCA = FPressure;

			if (FMotor->getACT()) {
				if (FMotor->getSpeciesNumber() == 9) {
					FSpecies_IVC[0] = FFraccionMasicaEspecie[7]; // N2
				}
				else if (FMotor->getSpeciesNumber() == 10) {
					FSpecies_IVC[0] = FFraccionMasicaEspecie[8]; // N2
				}
				FSpecies_IVC[1] = FFraccionMasicaEspecie[0]; // O2
				FSpecies_IVC[2] = FFraccionMasicaEspecie[1]; // CO2
				FSpecies_IVC[3] = FFraccionMasicaEspecie[2]; // H2O
				FSpecies_IVC[4] = FFraccionMasicaEspecie[5]; // NOx
				FSpecies_IVC[5] = FFraccionMasicaEspecie[6]; // CO
				FSpecies_IVC[6] = FFraccionMasicaEspecie[4]; // SOOT
				FSpecies_IVC[7] = FFraccionMasicaEspecie[3]; // HC
			}

			// Transporte de Especies Qu�micas. Se inician las masas de cada especie en el ciclo cerrado.
			if (FMotor->getSpeciesModel() == nmCalculoCompleto) {
				if (FMotor->getSpeciesNumber() == 9) {
					for (int i = 0; i < FMotor->getSpeciesNumber() - FIntEGR;
						i++) {
						FFraccionComienzoCicloCerrado[i] =
							FFraccionMasicaEspecie[i];
					}
					FComposicionCicloCerrado[1] = 0.;
					// No llega combustible de los tubos
					// FComposicionCicloCerrado[2]=FFraccionMasicaEspecie[0]+FFraccionMasicaEspecie[1]+FFraccionMasicaEspecie[2]+FFraccionMasicaEspecie[7]; // Aire fresco
					FComposicionCicloCerrado[2] = FFraccionMasicaEspecie[0]
						/ FMotor->GetComposicionAtmosfera(0);
					FComposicionCicloCerrado[0] = 1 - FComposicionCicloCerrado
						[2]; // Gases Quemados

				}
				else if (FMotor->getSpeciesNumber() == 10) {
					for (int i = 0; i < FMotor->getSpeciesNumber() - FIntEGR;
						i++) {
						FFraccionComienzoCicloCerrado[i] =
							FFraccionMasicaEspecie[i];
					}
					FComposicionCicloCerrado[1] = FFraccionMasicaEspecie[7];
					// Combustible
					// FComposicionCicloCerrado[2]=FFraccionMasicaEspecie[0]+FFraccionMasicaEspecie[1]+FFraccionMasicaEspecie[2]+FFraccionMasicaEspecie[8]; // Aire fresco
					FComposicionCicloCerrado[2] = FFraccionMasicaEspecie[0]
						/ FMotor->GetComposicionAtmosfera(0);
					FComposicionCicloCerrado[0] = 1 - FComposicionCicloCerrado
						[1] - FComposicionCicloCerrado[2]; // Gases Quemados
				}
			}
			else if (FMotor->getSpeciesModel() == nmCalculoSimple) {
				if (FMotor->getSpeciesNumber() == 3) {
					FComposicionCicloCerrado[1] = 0.;
					// No llega combustible de los tubos
					FComposicionCicloCerrado[2] = FFraccionMasicaEspecie[1];
					// Aire fresco
					FComposicionCicloCerrado[0] = FFraccionMasicaEspecie[0];
					// Gases Quemados

				}
				else if (FMotor->getSpeciesNumber() == 4) {
					FComposicionCicloCerrado[1] = FFraccionMasicaEspecie[1];
					// Combustible
					FComposicionCicloCerrado[2] = FFraccionMasicaEspecie[2];
					// Aire fresco
					FComposicionCicloCerrado[0] = FFraccionMasicaEspecie[0];
					// Gases Quemados
				}
			}
			for (int j = 0; j < 3; j++) {
				FMasaEspecieCicloCerrado[j] = FMasa * FComposicionCicloCerrado
					[j];
			}

			if (FHayEGR)
				FFraccionMasicaEspecie[FMotor->getSpeciesNumber() - 1] = 1.;

			std::cout << "INFO: End of Gas-exchange process in cylinder " <<
				FNumeroCilindro << std::endl;
			std::cout <<
				"____________________________________________________" << std::endl;
			std::cout << "INFO: Intake mass:         " << FMasaPorAdmision *
				1e3 << " (g)" << std::endl;
			std::cout << "INFO: Exhaust mass:        " << FMasaPorEscape *
				1e3 << " (g)" << std::endl;
			std::cout << "INFO: Trapped mass:        " << FMasaAtrapada *
				1e3 << " (g)" << std::endl;
			// std::cout << "INFO: Masa de combustible: " << FMasaFuel*1e6 << " (mg)" << std::endl;
			std::cout << "INFO: Pressure at I.C.:    " << FPresionRCA <<
				" (bar)" << std::endl;
			// std::cout << "____________________________________________________" << std::endl;
		}

		/* ================================= */
		/* BALANCE DE ENERGIA EN EL CILINDRO */
		/* ================================= */

		FVolumen0 = FVolumen;
		FVolumen = CalculaVolumen(FAnguloActual);
		FCm = CalculaCm();

		// HEAT TRANSFER CALCULATION
		// When
		if (FMotor->getCombustible() == nmMEP || FMotor->getCombustible()
			== nmMEC) {
			// Spark Ignition Engine:
			// Design and Simulation of 2S engines - G.P.Blair, page 306
			// (not exactly the same; radiation is neglected)
			double tcil = FTemperature + 273.;
			double visc = 7.457e-6 + 4.1547e-8 * tcil - 7.4793e-12 * pow2(tcil);
			double dens = FPressure * 1e5 / FRMezcla / tcil;
			double reyn = dens * FCm * FMotor->getGeometria().Diametro / visc;
			double nuss = 0.26 * pow(reyn, 0.7);
			double condk = 6.1944e-3 + 7.3814e-5 * tcil - 1.2491e-8 * pow2
				(tcil);
			Fh = condk * nuss / FMotor->getGeometria().Diametro;
		}
		else {
			// Compresion Ignition Engine:
			// Woschni equation
			// Periods adapted from 4S to 2S (needs to be validated and/or modified)
			if (FAnguloActual >= 0. && FAnguloActual <= 90.) {
				Fequis = Fratioctm + 1 / (pow(cosh(FAnguloActual / 100.),
						40.) + Fratioctm / (1 - Fratioctm));
			}
			else if (FAnguloActual >= 270. && FAnguloActual <= 360.) {
				Fequis = Fratioctm + 1 /
					(pow(cosh((360. - FAnguloActual) / 100.),
						40.) + Fratioctm / (1 - Fratioctm));
			}
			else {
				Fequis = 0.;
			}
			FCu = (2. * Pi * FMotor->getRegimen() / 60)
				* Fequis * Fctorbadmp * FKctm * pow2
				(FMotor->getGeometria().Diametro) /
				(2. * FMotor->getGeometria().DiametroBowl);

			// MOTOR SIN COMBUSTION O EN ARRASTRE
			if (FAnguloComb < FIniComb || FAnguloComb > FFinComb ||
				FMasaFuel == 0.) {
				Fc2 = 0;
				// MOTOR EN COMBUSTION
			}
			else {
				Fc2 = 0.001; // antes 3.24e-3  (ahora 0.001)MOTOR DE CAMARA ABIERTA ;  6.22e-3  MOTOR DE C�MARA DIVIDIDA
			}
			// COEFICIENTE DE PELICULA DE WOSCHNI
			double deltaP = (FPressure - FPresionRCA * pow
				(FVolumenCA / FVolumen, 1.36)) * 1e5;
			if (deltaP < 0.)
				deltaP = 0.;
			if (FAnguloActual >= 90. && FAnguloActual <= 180.) {
				// CARRERA DE ESCAPE
				Fh = 1.2e-2 * pow(FPressure * 1e5, 0.8) * pow
					(FTemperature + 273., -0.53) * pow
					(FMotor->getGeometria().Diametro, -0.2) * pow
					((6.18 * FCm + 0.417 * FCu) * FMotor->getAjusteTranCalEsc
					() + Fc2 * deltaP * FMotor->getGeometria()
					.CilindradaUnitaria / FMasaAtrapada / FRMezcla, 0.8);
			}
			else if (FAnguloActual >= 180. && FAnguloActual <= 270.) {
				// CARRERA DE ADMISION
				Fh = 1.2e-2 * pow(FPressure * 1e5, 0.8) * pow
					(FTemperature + 273., -0.53) * pow
					(FMotor->getGeometria().Diametro, -0.2) * pow
					((6.18 * FCm + 0.417 * FCu) * FMotor->getAjusteTranCalAdm
					() + Fc2 * deltaP * FMotor->getGeometria()
					.CilindradaUnitaria / FMasaAtrapada / FRMezcla, 0.8);
			}
			else { // CARRERAS DE COMPRESION Y EXPANSION
				Fh = 1.2e-2 * pow(FPressure * 1e5, 0.8) * pow
					(FTemperature + 273., -0.53) * pow
					(FMotor->getGeometria().Diametro, -0.2) * pow
					((FMotor->getWoschni().cw1 * FCm + FMotor->getWoschni()
						.cw2 * FCu) + Fc2 * deltaP * FMotor->getGeometria()
					.CilindradaUnitaria / FMasaAtrapada / FRMezcla, 0.8);
			}
		}

		if (FMotor->getCalculoPared() != nmTempFija)
			CalculaTemperaturasPared();

		// CALOR TRANSMITIDO A LAS PAREDES
		FCalor.TransCilindro = Fh * (FTempPared[0].Cylinder - FTemperature)
			* 4 * (FVolumen / 2. + FVolumen0 / 2. - FMotor->getGeometria()
			.VCC) / FMotor->getGeometria().Diametro;
		FCalor.TransPiston = Fh * (FTempPared[0].Piston - FTemperature)
			* FMotor->getGeometria().AreaPiston;
		FCalor.TransCulata = Fh * (FTempPared[0].Culata - FTemperature)
			* FMotor->getGeometria().AreaCulata;
		FCalor.TransTotal = (FCalor.TransCilindro + FCalor.TransPiston +
			FCalor.TransCulata) * FDeltaT;

		// CALCULO DEL CALOR LIBERADO
		if (FCicloCerrado) {
			if (FAnguloComb < FIniComb || FAnguloComb > FFinComb ||
				FMasaFuel == 0.) {
				FCalor.Liberado = 0.;
				FCalor.LiberadoTotal = 0.;
				FCalor.FQL = 0.;
				FCalor.FQL0 = 0.;

				FMasaEspecieCicloCerrado[0] = FMasaEspecieCicloCerrado[0]
					- FComposicionCicloCerrado[0] * FMasaBlowBy;
				// Gases Quemados
				FMasaEspecieCicloCerrado[1] = FMasaEspecieCicloCerrado[1]
					+ FFuelInstant; // Combustible
				FMasaEspecieCicloCerrado[2] = FMasaEspecieCicloCerrado[2]
					- FComposicionCicloCerrado[2] * FMasaBlowBy; // Aire Fresco

				FComposicionCicloCerrado[0] = FMasaEspecieCicloCerrado[0]
					/ FMasa;
				FComposicionCicloCerrado[1] = FMasaEspecieCicloCerrado[1]
					/ FMasa;
				FComposicionCicloCerrado[2] = FMasaEspecieCicloCerrado[2]
					/ FMasa;

			}
			else {

				if (FPrimeraCombustion) {
					if (FCalcComb == nmFQL) {
						FCalor.FQL0 = CalculaCalorLiberado
							(FAnguloComb - FDeltaAngulo);
					}
					else {
						FCalor.FQL0 = FCalor.FQL;
					}
					FPrimeraCombustion = false;
				}
				if (FCalcComb == nmFQL) {
					FCalor.FQL = CalculaCalorLiberado(FAnguloComb);
				}
				else if (FCalcComb == nmACT) {
					FCalor.FQL = Interp1(FAnguloComb, FCAD_exit, FHRF_exit,
						FCAI);
				}
				FCalor.Liberado = (FCalor.FQL - FCalor.FQL0)
					* FMfint * FMotor->getPoderCalorifico()
					* FMotor->getRendimientoCombustion();
				FCalor.LiberadoTotal += FCalor.Liberado;
				if (FCalor.Liberado < 0)
					FCalor.Liberado = 0.;

				// Transporte de Especies Quimicas
				FMfquem = FCalor.Liberado / FMotor->getPoderCalorifico();
				FMairequem = FMfquem / FDosadoEstequiometrico;
				if (FMairequem > (FMasaEspecieCicloCerrado[2])) {
					// Se necesita mas aire del que tenemos para completar la combustion.
					mfquefin = FMasaEspecieCicloCerrado[2] *
						FDosadoEstequiometrico;
					if (!FSaturado) {
						printf(
							"WARNING: Cylinder %d does not have enough air at CrankAngle %lf\n", FNumeroCilindro, FAnguloActual);
						FCalor.Liberado = (FCalor.FQL - FCalor.FQL0)
							* mfquefin * FMotor->getPoderCalorifico()
							* FMotor->getRendimientoCombustion();

						FSaturado = true;

						FMasaEspecieCicloCerrado[0] = FMasaEspecieCicloCerrado
							[0] + FMasaEspecieCicloCerrado[2] + mfquefin -
							FComposicionCicloCerrado[0] * FMasaBlowBy;
						// Gases Quemados
						FMasaEspecieCicloCerrado[1] = FMasaEspecieCicloCerrado
							[1] - mfquefin - FComposicionCicloCerrado[1]
							* FMasaBlowBy + FFuelInstant; // Combustible
						FMasaEspecieCicloCerrado[2] = 0.; // Aire Fresco

					}
					else {
						FCalor.Liberado = 0.;

						FMasaEspecieCicloCerrado[0] = FMasaEspecieCicloCerrado
							[0] - FComposicionCicloCerrado[0] * FMasaBlowBy;
						// Gases Quemados
						FMasaEspecieCicloCerrado[1] = FMasaEspecieCicloCerrado
							[1] - FComposicionCicloCerrado[1]
							* FMasaBlowBy + FFuelInstant; // Combustible
					}

				}
				else {
					FSaturado = false;

					FMasaEspecieCicloCerrado[0] = FMasaEspecieCicloCerrado[0]
						+ FMairequem + FMfquem - FComposicionCicloCerrado[0]
						* FMasaBlowBy; // Gases Quemados
					FMasaEspecieCicloCerrado[1] = FMasaEspecieCicloCerrado[1]
						- FMfquem - FComposicionCicloCerrado[1]
						* FMasaBlowBy + FFuelInstant; // Combustible
					FMasaEspecieCicloCerrado[2] = FMasaEspecieCicloCerrado[2]
						- FMairequem - FComposicionCicloCerrado[2]
						* FMasaBlowBy; // Aire Fresco
				}

				FComposicionCicloCerrado[0] = FMasaEspecieCicloCerrado[0]
					/ FMasa;
				FComposicionCicloCerrado[1] = FMasaEspecieCicloCerrado[1]
					/ FMasa;
				FComposicionCicloCerrado[2] = FMasaEspecieCicloCerrado[2]
					/ FMasa;

				FCalor.FQL0 = FCalor.FQL;
			}
		}

		/* Transporte de Especies Qu�micas */
		if (FCicloCerrado) {
			if (FMotor->getSpeciesModel() == nmCalculoCompleto) {
				if (FMotor->getACT()) {
					double Yfuel = (FFuelTotal - FMfint * FCalor.FQL) / FMasa;
					if (Yfuel < 0)
						Yfuel = 0;

					FFraccionMasicaEspecie[0] =
						(FSpecies_IVC[1] * (1 - FCalor.FQL)
						+ dataOUT.species_EVO[1] * FCalor.FQL) * (1 - Yfuel);
					FFraccionMasicaEspecie[1] =
						(FSpecies_IVC[2] * (1 - FCalor.FQL)
						+ dataOUT.species_EVO[2] * FCalor.FQL) * (1 - Yfuel);
					FFraccionMasicaEspecie[2] =
						(FSpecies_IVC[3] * (1 - FCalor.FQL)
						+ dataOUT.species_EVO[3] * FCalor.FQL) * (1 - Yfuel);
					FFraccionMasicaEspecie[3] =
						(FSpecies_IVC[7] * (1 - FCalor.FQL)
						+ dataOUT.species_EVO[7] * FCalor.FQL) * (1 - Yfuel);
					FFraccionMasicaEspecie[4] =
						(FSpecies_IVC[6] * (1 - FCalor.FQL)
						+ dataOUT.species_EVO[6] * FCalor.FQL) * (1 - Yfuel);
					FFraccionMasicaEspecie[5] =
						(FSpecies_IVC[4] * (1 - FCalor.FQL)
						+ dataOUT.species_EVO[4] * FCalor.FQL) * (1 - Yfuel);
					FFraccionMasicaEspecie[6] =
						(FSpecies_IVC[5] * (1 - FCalor.FQL)
						+ dataOUT.species_EVO[5] * FCalor.FQL) * (1 - Yfuel);

					if (FMotor->getSpeciesNumber() == 9) {
						FFraccionMasicaEspecie[3] += Yfuel;
						FFraccionMasicaEspecie[7] =
							(FSpecies_IVC[0] * (1 - FCalor.FQL)
							+ dataOUT.species_EVO[0] * FCalor.FQL) *
							(1 - Yfuel);

					}
					else if (FMotor->getSpeciesNumber() == 10) {
						FFraccionMasicaEspecie[7] = Yfuel;
						FFraccionMasicaEspecie[8] =
							(FSpecies_IVC[0] * (1 - FCalor.FQL)
							+ dataOUT.species_EVO[0] * FCalor.FQL) *
							(1 - Yfuel);
					}
					for (int j = 0; j < FMotor->getSpeciesNumber() - FIntEGR;
						j++) {
						FMasaEspecie[j] = FFraccionMasicaEspecie[j] * FMasa;
					}
				}
				else {
					if (FMotor->getSpeciesNumber() == 9) {
						if (!FSaturado) {
							FMolesCombQuemado = FMfquem /
								(FXComb * 12.01 + FYComb * 1.01 + FZComb *
								16); // Moles de combustible quemado en el incremento temporal calculado
						}
						else {
							FMolesCombQuemado = mfquefin /
								(FXComb * 12.01 + FYComb * 1.01 + FZComb *
								16); // Moles de combustible quemado en el incremento temporal calculado
						}
						FMasaO2Reactivos = FMolesCombQuemado *
							(FYComb / 4 + FXComb - FZComb / 2) * PMO2;
						FMasaN2Reactivos = FMolesCombQuemado *
							(FYComb / 4 + FXComb - FZComb / 2)
							* FRelacionMolarN2_O2 * PMN2;
						// FMasaH2OReactivos=FMolesCombQuemado*(FYComb/4+FXComb-FZComb/2)*FRelacionMolarH2O_O2*PMH2O;
						FMasaCO2Productos = FMolesCombQuemado * FXComb * PMCO2;
						FMasaH2OProductos = FMolesCombQuemado * (FYComb / 2
							/* +(FYComb/4+FXComb-FZComb/2)*FRelacionMolarH2O_O2 */
							) * PMH2O;
						FMasaN2Productos = FMolesCombQuemado *
							(FYComb / 4 + FXComb - FZComb / 2)
							* FRelacionMolarN2_O2 * PMN2;

						FFraccionMasicaEspecie[0] =
							(FMasaEspecie[0] - FMasaO2Reactivos -
							FFraccionMasicaEspecie[0] * FMasaBlowBy) / FMasa;
						// Y O2
						FFraccionMasicaEspecie[1] =
							(FMasaEspecie[1] + FMasaCO2Productos -
							FFraccionMasicaEspecie[1] * FMasaBlowBy) / FMasa;
						// Y CO2
						FFraccionMasicaEspecie[2] =
							(FMasaEspecie[2] + FMasaH2OProductos -
							FFraccionMasicaEspecie[2]
							* FMasaBlowBy /* -FMasaH2OReactivos */ ) / FMasa;
						// Y H2O
						FFraccionMasicaEspecie[7] =
							(FMasaEspecie[7] - FFraccionMasicaEspecie[7]
							* FMasaBlowBy) / FMasa; // Y N2
						FFraccionMasicaEspecie[3] = FComposicionCicloCerrado
							[1]; // Combustible que queda sin quemar
						FFraccionMasicaEspecie[4] = 0;
						FFraccionMasicaEspecie[5] = 0;
						FFraccionMasicaEspecie[6] = 0;
						for (int j = 0;
							j < FMotor->getSpeciesNumber() - FIntEGR;
							j++) {
							FMasaEspecie[j] = FFraccionMasicaEspecie[j] * FMasa;
						}

					}
					else if (FMotor->getSpeciesNumber() == 10) {
						if (!FSaturado) {
							FMolesCombQuemado = FMfquem /
								(FXComb * 12.01 + FYComb * 1.01 + FZComb *
								16); // Moles de combustible quemado en el incremento temporal calculado
						}
						else {
							FMolesCombQuemado = mfquefin /
								(FXComb * 12.01 + FYComb * 1.01 + FZComb *
								16); // Moles de combustible quemado en el incremento temporal calculado
						}
						FMasaO2Reactivos = FMolesCombQuemado *
							(FYComb / 4 + FXComb - FZComb / 2) * PMO2;
						FMasaN2Reactivos = FMolesCombQuemado *
							(FYComb / 4 + FXComb - FZComb / 2)
							* FRelacionMolarN2_O2 * PMN2;
						// FMasaH2OReactivos=FMolesCombQuemado*(FYComb/4+FXComb-FZComb/2)*FRelacionMolarH2O_O2*PMH2O;
						FMasaCO2Productos = FMolesCombQuemado * FXComb * PMCO2;
						FMasaH2OProductos = FMolesCombQuemado * (FYComb / 2
							/* +(FYComb/4+FXComb-FZComb/2)*FRelacionMolarH2O_O2 */
							) * PMH2O;
						FMasaN2Productos = FMolesCombQuemado *
							(FYComb / 4 + FXComb - FZComb / 2)
							* FRelacionMolarN2_O2 * PMN2;

						FFraccionMasicaEspecie[0] =
							(FMasaEspecie[0] - FMasaO2Reactivos -
							FFraccionMasicaEspecie[0] * FMasaBlowBy) / FMasa;
						// Y O2
						FFraccionMasicaEspecie[1] =
							(FMasaEspecie[1] + FMasaCO2Productos -
							FFraccionMasicaEspecie[1] * FMasaBlowBy) / FMasa;
						// Y CO2
						FFraccionMasicaEspecie[2] =
							(FMasaEspecie[2] + FMasaH2OProductos -
							FFraccionMasicaEspecie[2]
							* FMasaBlowBy /* -FMasaH2OReactivos */ ) / FMasa;
						// Y H2O
						FFraccionMasicaEspecie[7] =
							(FMasaEspecie[7] - FFraccionMasicaEspecie[7]
							* FMasaBlowBy) / FMasa; // Y N2
						FFraccionMasicaEspecie[3] = 0;
						FFraccionMasicaEspecie[4] = 0;
						FFraccionMasicaEspecie[5] = 0;
						FFraccionMasicaEspecie[6] = 0;
						FFraccionMasicaEspecie[8] = FComposicionCicloCerrado[1];
						for (int j = 0;
							j < FMotor->getSpeciesNumber() - FIntEGR;
							j++) {
							FMasaEspecie[j] = FFraccionMasicaEspecie[j] * FMasa;
						}

					}
				}
			}
			else if (FMotor->getSpeciesModel() == nmCalculoSimple) {
				if (FMotor->getSpeciesNumber() == 3) {
					FFraccionMasicaEspecie[0] = FComposicionCicloCerrado[0]
						+ FComposicionCicloCerrado[1];
					FFraccionMasicaEspecie[1] = FComposicionCicloCerrado[2];

					for (int j = 0; j < FMotor->getSpeciesNumber() - FIntEGR;
						j++) {
						FMasaEspecie[j] = FFraccionMasicaEspecie[j] * FMasa;
					}
				}
				else if (FMotor->getSpeciesNumber() == 4) {
					FFraccionMasicaEspecie[0] = FComposicionCicloCerrado[0];
					FFraccionMasicaEspecie[1] = FComposicionCicloCerrado[1];
					FFraccionMasicaEspecie[2] = FComposicionCicloCerrado[2];

					for (int j = 0; j < FMotor->getSpeciesNumber() - FIntEGR;
						j++) {
						FMasaEspecie[j] = FFraccionMasicaEspecie[j] * FMasa;
					}
				}
			}
		}
		else if (!FCicloCerrado) {
			for (int j = 0; j < FMotor->getSpeciesNumber() - 2; j++) {
				FFraccionMasicaEspecie[j] =
					(FMasaEspecie[j] - FFraccionMasicaEspecie[j]
					* FMasaBlowBy) / FMasa;
				FraccionMasicaAcum += FFraccionMasicaEspecie[j];
				FMasaEspecie[j] = FFraccionMasicaEspecie[j] * FMasa;
			}
			FFraccionMasicaEspecie[FMotor->getSpeciesNumber() - 2]
				= 1. - FraccionMasicaAcum;
			FMasaEspecie[FMotor->getSpeciesNumber() - 2] =
				FFraccionMasicaEspecie[FMotor->getSpeciesNumber() - 2] * FMasa;
			if (FHayEGR) {
				FFraccionMasicaEspecie[FMotor->getSpeciesNumber() - 1] =
					(FMasaEspecie[FMotor->getSpeciesNumber() - 1] -
					FFraccionMasicaEspecie[FMotor->getSpeciesNumber() - 1]
					* FMasaBlowBy) / FMasa;
				FMasaEspecie[FMotor->getSpeciesNumber() - 1] =
					FFraccionMasicaEspecie[FMotor->getSpeciesNumber() - 1]
					* FMasa;
			}
		}
		/* FIN Transporte de Especies Qu�micas */

		if (FAnguloActual > FDistribucion.CA && FAnguloAnterior <=
			FDistribucion.CA) {
			if (FMasaFuel == 0.) {
				FAFR = 0.;
			}
			else {
				FAFR = FMasaEspecieCicloCerrado[2] / FMasaFuel;
				// Masa Aire Fresco Inicial al cierre/Masa Fuel
			}
		}

		if (FMotor->getGammaCalculation() == nmGammaConstante) {
			if (FMotor->getSpeciesModel() == nmCalculoCompleto) {
				if (!FCicloCerrado) {
					FGamma = FuncionGamma(FTemperature + 273., 0.1);
				}
				else if (FCicloCerrado) {
					FGamma = FuncionGamma(FTemperature + 273.,
						FComposicionCicloCerrado[0]);
				}
			}
			else if (FMotor->getSpeciesModel() == nmCalculoSimple) {
				FGamma = FuncionGamma(FTemperature + 273.,
					FFraccionMasicaEspecie[0]);
			}
			FGamma1 = Gamma1(FGamma);
			FGamma2 = Gamma2(FGamma);
			FGamma4 = Gamma4(FGamma);
			FGamma6 = Gamma6(FGamma);
		}
		bool PrimerPaso = true;
		double ASon0 = FAsonido;
		double ASon1 = FAsonido;
		double MasTemp0 = 1 / (FTemperature + 273.) / FMasa0;
		double MasTemp1 = 1 / (FTemperature + 273.) / FMasa0;
		double MasTempMed;
		bool CotaError = false;
		double H1 = 0.;
		double H0 = 0.;
		double Energia;
		double Error;
		double Diff;

		// ITERACION PARA BUSCAR EL ESTADO TERMODINAMICO FINAL
		while (!CotaError) {
			// ENTALPIA POR ADMISION;
			H1 = 0.;
			for (int i = 0; i < FNumeroUnionesAdm; i++) {
				if (dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])->getMassflow()
					!= 0 && dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])
					->getSentidoFlujo() == nmEntrante) {
					if (PrimerPaso) {
						H1 += EntalpiaEntrada
							(dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])
							->getSpeedsound(),
							dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])
							->getVelocity(),
							-dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])
							->getMassflow() * FDeltaT, ASon1 / ARef, FMasa0);
					}
					else {
						H1 += EntalpiaEntrada
							(dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])
							->getSpeedsound(),
							dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])
							->getVelocity(),
							-dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])
							->getMassflow() * FDeltaT, ASon1 / ARef, FMasa);
					}
				}
			}
			// ENTALPIA POR ESCAPE;
			for (int i = 0; i < FNumeroUnionesEsc; i++) {
				if (dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])->getMassflow()
					!= 0 && dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])
					->getSentidoFlujo() == nmEntrante) {
					if (PrimerPaso) {
						H1 += EntalpiaEntrada
							(dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])
							->getSpeedsound(),
							dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])
							->getVelocity(),
							-dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])
							->getMassflow() * FDeltaT, ASon1 / ARef, FMasa0);
					}
					else {
						H1 += EntalpiaEntrada
							(dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])
							->getSpeedsound(),
							dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])
							->getVelocity(),
							-dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])
							->getMassflow() * FDeltaT, ASon1 / ARef, FMasa);
					}
				}
			}
			if (PrimerPaso) {
				H0 = H1;
				PrimerPaso = false;
			}
			MasTempMed = (MasTemp0 + MasTemp1) / 2;
			Energia = FVolumen0 * FMasa / FVolumen / FMasa0 * exp
				((H1 + H0) / 2. + (FCalor.TransTotal + FCalor.Liberado) *
				(MasTempMed / FRMezcla));
			ASon1 = FAsonido * sqrt(pow(Energia, FGamma1));
			Error = (Diff = ASon1 - ASon0, fabs(Diff)) / ASon1;
			if (Error > 1e-6) {
				MasTemp1 = 1. / (pow2(ASon1) * FMasa) * FGamma * FRMezcla;
				ASon0 = ASon1;
			}
			else {
				CotaError = true;
			}
		}
		// ACTUALIZA NUEVAS PROPIEDADES CILINDRO

		FAsonido0 = FAsonido;
		FAsonido = ASon1;

		FTemperatura0 = FTemperature;
		FTemperature = pow2(FAsonido) / FGamma / FRMezcla - 273.;
		FPresion0 = FPressure;
		FPressure = pow2(FAsonido) * FMasa / FGamma / FVolumen * 1e-5;
		if (FAnguloActual > FDistribucion.AE && FAnguloAnterior <=
			FDistribucion.AE) {
			std::cout << "INFO: Begin gas-exchange process in cylinder " <<
				FNumeroCilindro << std::endl;
			std::cout <<
				"____________________________________________________" << std::endl << std::endl;
			std::cout << "INFO: Calculated pressure at E.O.:    " <<
				FPressure << " (bar)" << std::endl;
			std::cout << "INFO: Calculated temperature at E.O.: " <<
				FTemperature << " (\260C)" << std::endl;
			if (FMotor->getCalculoDePAAE() == nmPAAEImpuesta) {
				FPressure = FMotor->getPresionAAE();
				FTemperature = FPressure * 1e5 * FVolumen / FMasa / FRMezcla -
					273.;
				FAsonido = sqrt(FRMezcla * FGamma * (FTemperature + 273.));
				std::cout << "INFO: Imposed pressure at E.O.:     " <<
					FPressure << " (bar)" << std::endl;
				std::cout << "INFO: Imposed temperature at E.O.:  " <<
					FTemperature << " (\260C)" << std::endl;
			}
			if (FMotor->getImponerComposicionAE()) {
				for (int i = 0; i < FMotor->getSpeciesNumber() - 1; i++) {
					FFraccionMasicaEspecie[i] = FMotor->GetComposicionInicial
						(i);
				}
			}
			std::cout <<
				"____________________________________________________" << std::endl << std::endl;
		}
		/* if(FNumeroCilindro==3){
		printf(" %lf %lf %lf \n ", FGamma,FRMezcla,FFraccionMasicaEspecie[0]);
		} */

		// CALCULA EL PAR INSTANT�NEO PRODUCIDO POR EL CILINDRO

		double a = FAnguloActual * Pi / 180.;
		double L = FMotor->getGeometria().Biela;
		double R = FMotor->getGeometria().Carrera / 2.;
		double e = FMotor->getGeometria().Excentricidad / 1000;
		double area = Pi * pow2(FMotor->getGeometria().Diametro) / 4.;

		double b = asin((e - R * sin(a)) / L);

		// CALCULO TRABAJO Y PAR
		FParInstantaneo = (FPressure - FMotor->getPresionAmb())
			* 1e5 * area * R * sin(a - b) / cos(b);

		FPreMed = (FPressure + FPresion0) / 2.;

		FTrabajoNetoACUM += FPreMed * (FVolumen - FVolumen0) * 1e5;
		if (FAnguloActual > 180 && FAnguloActual <= 540) {
			FTrabajoBombeoACUM += FPreMed * (FVolumen - FVolumen0) * 1e5;
		}
		if (FAnguloActual < FAnguloAnterior) {
			FTrabajoNeto = FTrabajoNetoACUM;
			FTrabajoNetoACUM = 0.;
			FPMN = FTrabajoNeto * 1e-5 / FMotor->getGeometria()
				.CilindradaUnitaria;
			FTrabajoBombeo = FTrabajoBombeoACUM;
			FTrabajoBombeoACUM = 0.;
			FPMB = FTrabajoBombeo * 1e-5 / FMotor->getGeometria()
				.CilindradaUnitaria;
			FPMI = FPMN - FPMB;
		}

		FDensidadReferencia = FMotor->getTuboRendVol()->GetGamma
			(FMotor->getNodoMedio()) * FMotor->getTuboRendVol()->GetPresion
			(FMotor->getNodoMedio()) * 1e5 / pow2
			(FMotor->getTuboRendVol()->GetAsonido(FMotor->getNodoMedio())
			* ARef);

		/* ================================= */
		/* MODELO DE CORTOCIRCUITO */
		/* ================================= */

		if (FAnguloActual > FDistribucion.AA && FAnguloActual <
			FDistribucion.CE) {
			if (MasaAdmInstante > 0. && MasaEscInstante < 0.) {
				// Cortocircuito con sentido Admisi�n hacia Escape.    (massflow positivo)
				MasaCortocircuitoAdm = MasaAdmInstante * FAlphaEscape / Pi;
				MasaCortocircuitoEsc = MasaEscInstante *
					(Pi / 2. - FAlphaEscape) / Pi;
				if (fabs(MasaCortocircuitoAdm) < fabs(MasaCortocircuitoEsc)) {
					FMasaCortocircuito = fabs(MasaCortocircuitoAdm);
				}
				else {
					FMasaCortocircuito = fabs(MasaCortocircuitoEsc);
				}
				FGastoCortocircuito = FMasaCortocircuito / FDeltaT;
				for (int j = 0; j < FMotor->getSpeciesNumber() - 2; j++) {
					for (int i = 0; i < FNumeroUnionesAdm; i++) {
						if (dynamic_cast<TCCCilindro*>(FCCValvulaAdm[i])
							->getSentidoFlujo() == nmEntrante) {
							FraccionCC += FCCValvulaAdm[i]
								->GetFraccionMasicaEspecie(j);
							NumeroUnionesEntrante++;
						}
					}
					FraccionCC = FraccionCC / NumeroUnionesEntrante;
					// MasaEscInstante tiene signo - cuando el flujo sale por el escape.
					FComposicionSaliente[j] = FFraccionMasicaEspecie[j] *
						(MasaEscInstante + FMasaCortocircuito)
						/ MasaEscInstante - FraccionCC * FMasaCortocircuito /
						MasaEscInstante;
					FraccionCC = 0.;
					NumeroUnionesEntrante = 0;
				}
				if (FHayEGR)
					FComposicionSaliente[FMotor->getSpeciesNumber() - 1]
						= FFraccionMasicaEspecie[FMotor->getSpeciesNumber()
				- 1];
			}
			else if (MasaAdmInstante < 0. && MasaEscInstante > 0.) {
				// Cortocircuito con sentido Escape hacia Admisi�n.   (massflow negativo)
				MasaCortocircuitoAdm = MasaAdmInstante *
					(Pi / 2. - FAlphaAdmision) / Pi;
				MasaCortocircuitoEsc = MasaEscInstante * FAlphaAdmision / Pi;
				if (fabs(MasaCortocircuitoAdm) < fabs(MasaCortocircuitoEsc)) {
					FMasaCortocircuito = -fabs(MasaCortocircuitoAdm);
				}
				else {
					FMasaCortocircuito = -fabs(MasaCortocircuitoEsc);
				}
				FGastoCortocircuito = FMasaCortocircuito / FDeltaT;
				for (int j = 0; j < FMotor->getSpeciesNumber() - 2; j++) {
					for (int i = 0; i < FNumeroUnionesEsc; i++) {
						if (dynamic_cast<TCCCilindro*>(FCCValvulaEsc[i])
							->getSentidoFlujo() == nmEntrante) {
							FraccionCC += FCCValvulaEsc[i]
								->GetFraccionMasicaEspecie(j);
							NumeroUnionesEntrante++;
						}
					}
					FraccionCC = FraccionCC / NumeroUnionesEntrante;
					// La masa por la v�lvula de admisi�n ser� negativa, por ser saliente del cilindro.
					// La masa de cortocircuito de escape a admisi�n es negativa.
					FComposicionSaliente[j] = FFraccionMasicaEspecie[j] *
						(MasaAdmInstante - FMasaCortocircuito)
						/ MasaAdmInstante + FraccionCC * FMasaCortocircuito /
						MasaAdmInstante;
					FraccionCC = 0.;
					NumeroUnionesEntrante = 0;
				}
				if (FHayEGR)
					FComposicionSaliente[FMotor->getSpeciesNumber() - 1]
						= FFraccionMasicaEspecie[FMotor->getSpeciesNumber()
				- 1];
			}
			else {
				// No hay cortocircuito, pues en este instante entra o sale massflow por todas las v�lvulas.
				FMasaCortocircuito = 0.;
				FGastoCortocircuito = 0.;
				for (int j = 0; j < FMotor->getSpeciesNumber() - FIntEGR; j++) {
					FComposicionSaliente[j] = FFraccionMasicaEspecie[j];
				}
			}
		}
		else {
			// No hay cortocircuito, pues alguna de las v�lvulas no est� abierta.
			FMasaCortocircuito = 0.;
			FGastoCortocircuito = 0.;
			for (int j = 0; j < FMotor->getSpeciesNumber() - FIntEGR; j++) {
				FComposicionSaliente[j] = FFraccionMasicaEspecie[j];
			}
		}

	}
	catch(Exception & N) {
		std::cout <<
			"ERROR: TCilindro2T::ActualizaPropiedades en cilindro: "
			<< FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double TCilindro2T::EntalpiaEntrada(double ASonEnt, double VelEnt,
	double MasEnt, double ASonCil, double MasCil) {
	try {
		double xx, yy, ret_val;

		if (fabs(MasEnt) != 0.) {
			xx = (ASonEnt * ASonEnt / ASonCil / ASonCil - 1.) / FGamma1;
			yy = VelEnt * VelEnt / ASonCil / ASonCil / 2.;
			ret_val = FGamma * MasEnt * (xx + yy) / MasCil;
		}
		else {
			ret_val = 0.;
		}
		return ret_val;
	}
	catch(Exception & N) {
		std::cout << "ERROR: TCilindro2T::EntalpiaEntrada en cilindro: " <<
			FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCilindro2T::VariableInicialesCicloACT() {
	try {
		FPresionMedAdm /= FTimeAcumAct;
		FPresionMedEsc /= FTimeAcumAct;
		FTimeAcumAct = 0.;

		Ftest_variables[0] = FRegInt; // Speed
		if (FMasaPorAdmision != 0)
			Ftest_variables[1] = (FMasaPorAdmision - FMasaEGR) * 1000;
		// Measured air mass
		Ftest_variables[2] = FMasaAtrapada * 1000; // In-cylinder air mass at inlet valve closing
		Ftest_variables[3] = FTemperature + 273; // In-cylinder temperature at inlet valve closing
		Ftest_variables[4] = FMasaFuel * 1e6; // Fuel injected mass
		Ftest_variables[5] = FMotor->getInjectionSys().InjectPressure;
		// Injection pressure
		Ftest_variables[6] = FPresionMedAdm * 1e5; // Inlet pressure
		Ftest_variables[7] = FPresionMedEsc * 1e5; // Exhaust pressure
		// Ftest_variables[8]=FMotor->getGeometria().CDBlowBy;          //Blow-by coefficient
		// Ftest_variables[9]=FMotor->getPresionAmb()*1e5;              //Atmosphere pressure
		Ftest_variables[10] = 318.15; // Fuel injection temperature
		Ftest_variables[11] = FTempPared[0].Culata + 273;
		// Cylinder head temperature
		Ftest_variables[12] = FTempPared[0].Cylinder + 273;
		// Cylinder temperature
		Ftest_variables[13] = FTempPared[0].Piston + 273; // Piston temperature
		if (FMotor->getSpeciesModel() == nmCalculoCompleto) {
			Ftest_variables[14] = FFraccionMasicaEspecie[5] * 1e6 * 1.587;
			// NOx concentration at IVC
			Ftest_variables[15] = FFraccionMasicaEspecie[4] * 1e6;
			// SOOT concentration at IVC
			Ftest_variables[16] = FFraccionMasicaEspecie[6] * 1e6;
			// CO concentration at IVC
			Ftest_variables[17] = FFraccionMasicaEspecie[3] * 1e6;
			// HC concentration at IVC
		}

		std::cout << "____________________________________________________" <<
			std::endl;
		std::cout << "INFO: Engine speed:        " << Ftest_variables[0] <<
			" (rpm)" << std::endl;
		std::cout << "INFO: Intake mass:         " << Ftest_variables[1] <<
			" (kg/s)" << std::endl;
		std::cout << "INFO: Trapped mass:        " << Ftest_variables[2] <<
			" (g)" << std::endl;
		std::cout << "INFO: Temperature at IVC:  " << Ftest_variables[3] <<
			" (K)" << std::endl;
		std::cout << "INFO: Fuel mass:           " << Ftest_variables[4] <<
			" (mg)" << std::endl;
		std::cout << "INFO: Injection pressure:  " << Ftest_variables[5] <<
			" (bar)" << std::endl;
		std::cout << "INFO: Intake pressure:     " << Ftest_variables[6] <<
			" (Pa)" << std::endl;
		std::cout << "INFO: Exhaust pressure:    " << Ftest_variables[7] <<
			" (Pa)" << std::endl;
		std::cout << "____________________________________________________" <<
			std::endl;
	}
	catch(Exception & N) {
		std::cout <<
			"ERROR: TCilindro2T::VariableInicialesCicloACT en cilindro: " <<
			FNumeroCilindro << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message.c_str());
	}
}

#pragma package(smart_init)
