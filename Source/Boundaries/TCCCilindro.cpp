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
// ---------------------------------------------------------------------------
// En la Tesis de Jose Miguel Corberan, paginas 104-139
// Resumido en el articulo "Solucion a la condicion de contorno de la union
// cilindro-conducto de los MCIA" (Esta en el COMETT PROGRAMME 1995)
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
#pragma hdrstop

#include "TCCCilindro.h"
//#include <cmath>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include "TTubo.h"

#include "TValvula4T.h"
#include "TLamina.h"
#include "TLumbrera.h"

#include "TBloqueMotor.h"
#include "TCilindro4T.h"
#include "Globales.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCCCilindro::TCCCilindro(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
						 nmCalculoGamma GammaCalculation, bool ThereIsEGR) :
	TCondicionContorno(TipoCC, numCC, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {

	if(TipoCC == nmIntakeValve)
		FTipoValv = nmValvAdmision;
	else if(TipoCC == nmExhaustValve)
		FTipoValv = nmValvEscape;
	else
		printf("ERROR en tipo de valvula TCCCilindro en la condicion de contorno: %d\n", FNumeroCC);

	FTuboExtremo = NULL;
	FValvula = NULL;

	FGasto = 0.;
	FVelocity = 0.;
	FSonido = 1.;
	FMomento = 0.;
	FVelocidadGarganta = 0.;
	FMachGarganta = 1.;
	FGastoGarganta = 0.;
	FRelacionPresionGarganta = 1.;

	FTime0 = 0.;
	FTime1 = 0.;
	FAnguloAnterior = 0.;
	FAnguloActual = 0.;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TCCCilindro::~TCCCilindro() {

	delete[] FTuboExtremo;

	if(FValvula != NULL)
		delete FValvula;

	FValvula = NULL;

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCCilindro::AsignaTipoValvula(TTipoValvula **Origen, int Valv, int i) {
	try {

		switch(Origen[Valv - 1]->getTypeOfValve()) {
		case nmValvula4T:
			FValvula = new TValvula4T(dynamic_cast<TValvula4T*>(Origen[Valv - 1]), i);
			break;
		case nmLamina:
			FValvula = new TLamina(dynamic_cast<TLamina*>(Origen[Valv - 1]), i);
			break;
		case nmLumbrera2T:
			FValvula = new TLumbrera(dynamic_cast<TLumbrera*>(Origen[Valv - 1]), i);
			break;
		}

		// FValvula->putDiametroTubo(FTuboExtremo[0].Pipe->GetDiametro(FNodoFin));

		FValvula->PutPipe(FTuboExtremo[0].Pipe, FNodoFin);

		FSeccionValvula = __geom::Circle_area(FValvula->getDiametro());
		FSeccionTubo = __geom::Circle_area(FTuboExtremo[0].Pipe->GetDiametro(FNodoFin));

	} catch(exception & N) {
		std::cout << "ERROR: TCCCilindro::AsignaTipoValvula en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCCilindro::ReadBoundaryData(const char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe, int nDPF,
								   TDPF **DPF) {
	try {
		int i = 0;
		int numid = 0; // Variable necesaria para WAMer.

		FTuboExtremo = new stTuboExtremo[1];
		FTuboExtremo[0].Pipe = NULL;

		FPref = 1;

		while(FNumeroTubosCC < 1 && i < NumberOfPipes) {
			if(Pipe[i]->getNodoIzq() == FNumeroCC) {
				FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
				FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmLeft;
				FNodoFin = 0;
				FIndiceCC = 0;
				FCC = &(FTuboExtremo[FNumeroTubosCC].Beta);
				FCD = &(FTuboExtremo[FNumeroTubosCC].Landa);
				FNumeroTubosCC++;
			}
			if(Pipe[i]->getNodoDer() == FNumeroCC) {
				FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
				FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmRight;
				FNodoFin = Pipe[i]->getNin() - 1;
				FIndiceCC = 1;
				FCC = &(FTuboExtremo[FNumeroTubosCC].Landa);
				FCD = &(FTuboExtremo[FNumeroTubosCC].Beta);
				FNumeroTubosCC++;
			}
			i++;
		}
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ",
			   &numid); // Esto es un dato que necesita el WAMer. Los usuarios de WAM hacemos la vista gorda hasta que se arregle.
		fscanf(fich, "%d ", &FNumeroCilindro);

		fgetpos(fich, &filepos);
		fclose(fich);

		// Inicializacion del transporte de especies quimicas
		FFraccionMasicaEspecie = new double[FNumeroEspecies - FIntEGR];
		for(int i = 0; i < FNumeroEspecies - FIntEGR; i++) {
			FFraccionMasicaEspecie[i] = FTuboExtremo[0].Pipe->GetFraccionMasicaInicial(i);
		}

	}

	catch(exception & N) {
		std::cout << "ERROR: TCCCilindro::LeeCCCilindro en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCCilindro::AsignaCilindro(TBloqueMotor *EngineBlock) {
	try {

		FMotor = EngineBlock;
		FCilindro = EngineBlock->GetCilindro(FNumeroCilindro - 1);
		FAnguloActual = FCilindro->getAnguloActual();
		FValvula->PutCylider(FCilindro);

	} catch(exception & N) {
		std::cout << "ERROR: TCCCilindro::AsignaCilindro en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCCilindro::CalculaCondicionContorno(double Time) {
	try {
		double rel_CCon_Entropia, coef, FraccionMasicaAcum = 0.;

		FTime0 = FTime1;
		FTime1 = Time;

		FGamma = FTuboExtremo[0].Pipe->GetGamma(FNodoFin);
		FGamma1 = __Gamma::G1(FGamma);
		FGamma2 = __Gamma::G2(FGamma);
		FGamma3 = __Gamma::G3(FGamma);
		FGamma4 = __Gamma::G4(FGamma);
		FGamma5 = __Gamma::G5(FGamma);
		FGamma6 = __Gamma::G6(FGamma);

		// FSeccionValvula = Pi * pow(FValvula->getDiametro(),2.)/ 4.;
		// FSeccionTubo=Pi*pow(FTuboExtremo[0].Pipe->GetDiametro(FNodoFin),2.)/4.;

		FAd = pow(FCilindro->getPressure() / FPref, 1. / FGamma4);
		rel_CCon_Entropia = *FCC / FTuboExtremo[0].Entropia;
		if(rel_CCon_Entropia / FAd > 1.000005) {   // Flujo entrante al cilindro
			FSentidoFlujo = nmEntrante;
			FValvula->GetCDin(FTime1);
			FCDEntrada = FValvula->getCDTubVol();
			if(FCDEntrada > 0.0001) {  /* Abierto */
				FSeccionEficaz = FCDEntrada * FSeccionValvula;
				FlujoEntranteCilindro();
				/* CALCULO DEL MOMENTO ANGULAR ENTRANTE L */
				if(FGasto < -1e-5) {
					FCTorbellino = FValvula->getCTorb();
					if(FTipoValv == nmValvEscape) {
						coef = FCTorbellino / 4.;
					} else {
						coef = FCTorbellino;
					}
					FMomento = coef * pow2(__cons::ARef * FGasto) / (200000 * FMotor->getGeometria().Carrera * FGamma *
							   FCilindro->getPressure()) * (pow2(*FCC + *FCD) / 4. + FGamma3 * pow2(
									   ((*FCD - *FCC) / FGamma1)));
				}
				// Transporte de especies quimicas.
				for(int j = 0; j < FNumeroEspecies - 2; j++) {
					FFraccionMasicaEspecie[j] = FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC, j);
					FraccionMasicaAcum += FFraccionMasicaEspecie[j];
				}
				FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
				if(FHayEGR)
					FFraccionMasicaEspecie[FNumeroEspecies - 1] = FTuboExtremo[0].Pipe->GetFraccionMasicaCC(FIndiceCC, FNumeroEspecies - 1);
			} else { /* Cerrado */
				FMomento = 0.;
				FGasto = 0.;
				*FCD = *FCC;
				FVelocity = 0.;
				FSonido = *FCD;
				FVelocidadGarganta = 0.;
				FMachGarganta = 0.;
				FGastoGarganta = 0.;
				FRelacionPresionGarganta = 0.;
				FSeccionEficaz = 0.;
				// La composicion se mantiene, al estar el cilindro cerrado.
			}
		} else if(rel_CCon_Entropia / FAd < .999995) {   // Flujo saliente del cilindro
			FSentidoFlujo = nmSaliente;
			FValvula->GetCDout(FTime1);
			FCDSalida = FValvula->getCDVolTub();
			if(FCDSalida > 0.0001) {  /* Abierto */
				FSeccionEficaz = FCDSalida * FSeccionValvula;
				FlujoSalienteCilindro();
				/* CALCULO DEL MOMENTO ANGULAR SALIENTE */
				if(FGasto > 1e-5) {
					FMomento = -FCilindro->getMomentoAngular() * FGasto / FCilindro->getMasa();
				}

				// Transporte de especies quimicas.
				for(int j = 0; j < FNumeroEspecies - 2; j++) {
					FFraccionMasicaEspecie[j] = FCilindro->GetComposicionSaliente(j);
					FraccionMasicaAcum += FFraccionMasicaEspecie[j];
				}
				FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
				if(FHayEGR)
					FFraccionMasicaEspecie[FNumeroEspecies - 1] = FCilindro->GetComposicionSaliente(FNumeroEspecies - 1);
			} else { /* Cerrado */
				FMomento = 0.;
				FGasto = 0.;
				*FCD = *FCC;
				FVelocity = 0.;
				FSonido = *FCD;
				FVelocidadGarganta = 0.;
				FMachGarganta = 0.;
				FGastoGarganta = 0.;
				FRelacionPresionGarganta = 0.;
				FSeccionEficaz = 0.;
				// La composicion se mantiene, al estar el cilindro cerrado.
			}
		} else { // Flujo Parado
			FSentidoFlujo = nmParado;
			*FCD = *FCC;
			FGasto = 0.;
			FVelocity = 0.;
			FSonido = *FCD;
			FVelocidadGarganta = 0.;
			FMachGarganta = 0.;
			FGastoGarganta = 0.;
			FRelacionPresionGarganta = 0.;
			FSeccionEficaz = 0.;
			// La composicion se mantiene, al estar el flujo parado.
		}
		FValvula->AcumulaCDMedio(Time);
	} catch(exception & N) {
		std::cout << "ERROR: TCCCilindro::CalculaCondicionContorno en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCCilindro::FlujoEntranteCilindro() {
	try {
		double vel_son_garganta = 0., velocidad_garganta = 0., Mach = 0., xaa2 = 0., ycal = 0., d1 = 0.;

		Fk = FSeccionTubo / FSeccionEficaz;
		if(Fk < 1)
			Fk = 1;
		vel_son_garganta = FTuboExtremo[0].Entropia * FAd; // Velocity del sonido en la garganta. Adimensional.

		// Calculo de la velocidad en la garganta.Caso de salto subcritico.
		FCaso = nmFlujoEntranteSaltoSubcritico;
		if(Fk == 1) {
			FSonido = FTuboExtremo[0].Entropia * FAd;
			FVelocity = (*FCC - FSonido) / FGamma3;
		} else
			Resolucion(vel_son_garganta, *FCC, FCaso, &FVelocity, &FSonido);

		// Ecuacion de la energia
		velocidad_garganta = sqrt(2. * FGamma6 * (pow2(FSonido) + FGamma3 * pow2(FVelocity) - pow2(vel_son_garganta)));
		// Se ha calculado la velocidad en la garganta en valor absoluto.

		// Calculo de la velocidad en la garganta en el caso de salto supercritico
		if(velocidad_garganta > vel_son_garganta) {
			FCaso = nmFlujoEntranteSaltoSupercritico;
			Resolucion(0.0, 1.0, FCaso, &ycal, &Mach);
			FVelocity = *FCC / (1 / Mach + FGamma3);
			FSonido = FVelocity / Mach;

			d1 = Fk * FVelocity * pow(FSonido, 1. / FGamma3);
			vel_son_garganta = pow(d1, FGamma1 / FGamma2);
			velocidad_garganta = vel_son_garganta;
		}
		double Ga3U = FGamma3 * FVelocity;
		// Fin caso de salto supercritico

		xaa2 = pow(FTuboExtremo[0].Entropia, FGamma4);
		FGasto = __units::BarToPa(-FGamma * FSeccionTubo * pow(FSonido,
								  2 * FGamma6) * FVelocity) / (__cons::ARef * xaa2); // Massflow entrante al cilindro negativo
		*FCD = FSonido - Ga3U;
		*FCC = FSonido + Ga3U;
		FRelacionPresionGarganta = pow(FSonido / (FTuboExtremo[0].Entropia * FAd), FGamma4);
		FGastoGarganta = FGasto / (FCDEntrada * FSeccionValvula);
		FMachGarganta = -velocidad_garganta / vel_son_garganta; // Negativo por ser flujo entrante
		FVelocidadGarganta = velocidad_garganta;
	}

	catch(exception & N) {
		std::cout << "ERROR: TCCCilindro::FlujoEntranteCilindro en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCCilindro::FlujoSalienteCilindro() {
	try {

		double a1 = 0., xx = 0., yy = 0., d1 = 0., Ga3U = 0.;
		double a2cr = 0., val1 = 0., val2 = 0., u2cr = 0., ycal = 0., error = 0., valde = 0., miembro2 = 0.;
		// Variables para resolver la onda de choque.
		double relacion_velocidades_son, Mach_tras_ondachoque, Mach, temp_antes_ondachoque, temp_tras_ondachoque;
		double root_a = 0.;

		Fk = FSeccionTubo / FSeccionEficaz;
		if(Fk < 1)
			Fk = 1.0;

		double sqrtGa2 = sqrt(2. / FGamma2);

		/* Calculo del valor de la velocidad del sonido en el extremo del tubo para
		 el cual el salto es critico. */
		u2cr = FCilindro->getSpeedsound() / __cons::ARef * sqrtGa2 * (sqrt(pow2(Fk) + FGamma1 * FGamma2) - Fk) / FGamma1;
		a2cr = sqrt(pow2(FCilindro->getSpeedsound() / __cons::ARef) - FGamma3 * pow2(u2cr));
		// Ecuacion de la energia. Garganta-Cylinder.

		/* A partir  de a2cr se determina el error en el calculo de A2 al suponer salto
		 subcritico. Si es negativo, el salto es supercritico. Si es positivo, el salto
		 es subcritico. */
		// FSSubcritico(a2cr,&error,&miembro2);
		stFSSub FSA2(FTuboExtremo[0].Entropia, FAd, FGamma, Fk, *FCC, FCilindro->getSpeedsound() / __cons::ARef);

		error = FSA2(a2cr);

		if(error < 0.) {   // Salto de presiones supercritico.

			/* Determinacion del intervalo de iteracion. Para ello se supone que
			 en el extremo del tubo se dan las condiciones criticas. Explicado en
			 los apuntes de Pedro. */
			a1 = sqrtGa2 * FCilindro->getSpeedsound() / __cons::ARef;
			FVelocidadGarganta = a1;
			xx = pow(FAd / (FCilindro->getSpeedsound() / __cons::ARef), FGamma4);
			yy = pow(a1, 2. / FGamma1);
			Fcc = FVelocidadGarganta * yy * xx / Fk;
			// FSSupercritico(FVelocidadGarganta,&val1,&val2);

			stFSSup FU2(FTuboExtremo[0].Entropia, Fcc, FGamma, Fk, *FCC, FCilindro->getSpeedsound() / __cons::ARef);
			val1 = FU2(FVelocidadGarganta);

			if(val1 < 0.)
				valde = FVelocidadGarganta;
			else
				valde = (FCilindro->getSpeedsound() / __cons::ARef) / sqrt(FGamma3);

			/* Una vez conocido el intervalo de iteracion, se pasa a la resolucion
			 del caso flujo saliente salto supercritico. */
			FCaso = nmFlujoSalienteSaltoSupercritico;
			Resolucion(0.0, valde, FCaso, &FVelocity, &FSonido);
			Ga3U = FVelocity * FGamma3;
			// Calcula del massflow. Como es saliente del cilindro, siempre es positivo.
			xx = pow(sqrtGa2, (FGamma2 / FGamma1));
			yy = pow(FAd, FGamma4);
			FGasto = __units::BarToPa(FCDSalida * FSeccionValvula * FGamma * xx * yy) / (FCilindro->getSpeedsound());

			/* Reduccion a flujo subsonico mediante onda de choque plana en el caso
			 de que se hayan obtenido condiciones supersonicas en el extremo del
			 tubo. Explicado en la tesis Corberan (pagina de la 47 a la 52
			 (punto 2.5) y de la 122 a la 129 (lo importante a partir de la 127) */
			Mach = FVelocity / FSonido;
			xx = *FCC + Ga3U;
			FTuboExtremo[0].Entropia = FTuboExtremo[0].Entropia * FSonido / xx;
			// Ecuacion de la caracteristica incidente.
			if(Mach > 1.) {

				/* Las ecuaciones siguientes corresponden a la resolucion de la onda
				 de choque plana. Se pueden encontrar en el punto 2.5 de la tesis
				 de Corberan. */
				xx = FGamma4 * pow2(Mach) - 1.;
				Mach_tras_ondachoque = sqrt((pow2(Mach) + 2. / FGamma1) / xx);
				temp_tras_ondachoque = FGamma3 * pow2(Mach) + 1.;
				temp_antes_ondachoque = FGamma3 * pow2(Mach_tras_ondachoque) + 1.;
				relacion_velocidades_son = sqrt(temp_tras_ondachoque / temp_antes_ondachoque);
				FSonido = FSonido * relacion_velocidades_son;
				FVelocity = FSonido * Mach_tras_ondachoque;
				Ga3U = FVelocity * FGamma3;
				d1 = xx * FGamma1 / FGamma2;
				FTuboExtremo[0].Entropia = FTuboExtremo[0].Entropia * relacion_velocidades_son / pow(d1, FGamma5);
			}
		} else { // Salto de presiones subcritico.

			// Resolucion del caso de flujo saliente salto subcritico.
			FCaso = nmFlujoSalienteSaltoSubcritico;
			Resolucion(a2cr, FCilindro->getSpeedsound() / __cons::ARef, FCaso, &ycal, &FSonido);
			// Aplicando la Ecuacion de la Energia entre el cilindro y la garganta:
			root_a = pow2(FCilindro->getSpeedsound() / __cons::ARef) - pow2(FSonido);
			if(root_a > 0) {
				FVelocity = sqrt((pow2(FCilindro->getSpeedsound() / __cons::ARef) - pow2(FSonido)) / FGamma3);
			} else if(root_a > -1e12) {
				FVelocity = 0;
			} else {
				FVelocity = 0.;
				printf("ERROR: Calculating outflow in boundary %d", FNumeroCC);
			}
			Ga3U = FVelocity * FGamma3;
			// Calculo del massflow. Como es saliente del cilindro, siempre es positivo.
			xx = *FCC + Ga3U;
			a1 = FCilindro->getSpeedsound() / __cons::ARef * xx / (FTuboExtremo[0].Entropia * FAd);
			FVelocidadGarganta = Fk * pow2(a1) * FVelocity / pow2(FSonido);
			FGasto = __units::BarToPa(FCDSalida * FSeccionValvula * FGamma * pow(FAd / (FCilindro->getSpeedsound() / __cons::ARef),
									  FGamma4) * FVelocidadGarganta * pow(a1, 2. / FGamma1)) / __cons::ARef;

			FTuboExtremo[0].Entropia = FTuboExtremo[0].Entropia * FSonido / xx;
			// Ecuacion de la caracteristica incidente.
		}
		*FCD = FSonido + Ga3U;
		*FCC = FSonido - Ga3U;
		d1 = FSonido / (FTuboExtremo[0].Entropia * FAd);
		FRelacionPresionGarganta = pow(d1, FGamma4);
		FMachGarganta = FVelocidadGarganta / a1;
		// FMachGarganta = FVelocidadGarganta/a1; // Positivo por ser flujo saliente
		FGastoGarganta = FGasto / (FCDSalida * FSeccionValvula);
	}

	catch(exception & N) {
		std::cout << "ERROR: TCCCilindro::FlujoSalienteCilindro en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCCilindro::Resolucion(double ext1, double ext2, nmCaso Caso, double *u2t, double *a2t) {
	try {
		if(Caso == nmFlujoEntranteSaltoSubcritico) {
			stFESub FEA2(FTuboExtremo[0].Entropia, FAd, FGamma, Fk, *FCC);
			*a2t = FindRoot(FEA2, ext1, ext2);
			*u2t = FEA2.U2;
		} else if(Caso == nmFlujoEntranteSaltoSupercritico) {
			stFESup FMatch(FGamma, Fk);
			*a2t = FindRoot(FMatch, ext1, ext2);
			*u2t = 0.;
		} else if(Caso == nmFlujoSalienteSaltoSubcritico) {
			stFSSub FSA2(FTuboExtremo[0].Entropia, FAd, FGamma, Fk, *FCC, FCilindro->getSpeedsound() / __cons::ARef);
			*a2t = FindRoot(FSA2, ext1, ext2);
			*u2t = FSA2.U2;
		} else if(Caso == nmFlujoSalienteSaltoSupercritico) {
			stFSSup FU2(FTuboExtremo[0].Entropia, Fcc, FGamma, Fk, *FCC, FCilindro->getSpeedsound() / __cons::ARef);
			*u2t = FindRoot(FU2, ext1, ext2);
			*a2t = FU2.A2;
		} else {
			printf("Error en la definicion del flujo TCCDeposito::Resolucion en la condicion de contorno: %d\n", FNumeroCC);
			throw Exception("");
		}
	}

	catch(exception & N) {
		std::cout << "ERROR: TCCCilindro::Resolucion en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// void TCCCilindro::FESubcritico(double vel_son_supuesta,double *u2_1,double *u2_2)
// {
// try
// {
//
// double xx,yy,u2;
//
///* Resolucion de la ecuacion (20) del articulo "Solucion a la condicion de
// contorno de la union cilindro-conducto de los MCIA". Ecuacion 4.30 en la
// tesis de Corberan */
//
// xx=vel_son_supuesta/(FTuboExtremo[0].Entropia*FAd);
// yy=pow(xx,4.*FGamma6);
// yy=pow(Fk,2.)*yy-1.;
// *u2_2=FTuboExtremo[0].Entropia*FAd*sqrt(2.*FGamma6*(pow(xx,2.)-1.)/yy); // Valor absoluto
//
///* Resolucion de la ecuacion de la caracteristica incidente. */
//
// *u2_1=(*FCC-vel_son_supuesta)/FGamma3;  // En valor absoluto
//
// }
// catch(Exception &N)
// {
// std::cout << "ERROR: TCCCilindro::FESubcritico en la condicion de contorno: " << FNumeroCC << std::endl;
// std::cout << "Tipo de error: " << N.what() << std::endl;
// throw Exception(N.what());
// }
// }
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//
// void TCCCilindro::FESupercritico(double mach_supuesto,double *miembro1,
// double *miembro2)
// {
// try
// {
//
// double xx,yy;
//
///* Resolucion de la ecuacion (21) del articulo "Solucion a la condicion de
// contorno de la union cilindro-conducto de los MCIA". Ecuacion (4.31) de
// la tesis de Corberan */
//
// yy=(FGamma2/2.)*pow(Fk*mach_supuesto,2.*FGamma1/FGamma2);
// xx=FGamma3*pow(mach_supuesto,2);
// *miembro1=xx-yy+1.;     // Miembro 1 de la ecuacion (21)
// *miembro2=0;            // Miembro 2 de la ecuacion (21)
//
// }
// catch(Exception &N)
// {
// std::cout << "ERROR: TCCCilindro::FESupercritico en la condicion de contorno: " << FNumeroCC << std::endl;
// std::cout << "Tipo de error: " << N.what() << std::endl;
// throw Exception(N.what());
// }
// }
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//
// void TCCCilindro::FSSubcritico(double vel_son_supuesta,double *error,double *miembro2)
// {
// try
// {
// double a1,u1,u2;
//
// *miembro2=0;
//
///* Resolucion del algoritmo de calculo propuesto en la pagina 113 de la tesis
// de Corberan. */
//
// u2 = sqrt((pow(FCilindro->getSpeedsound()/__cons::ARef,2)-pow(vel_son_supuesta,2))/FGamma3);
// a1 = FCilindro->getSpeedsound()/__cons::ARef*(*FCC+FGamma3*u2)/(FTuboExtremo[0].Entropia*FAd);
// u1 = Fk*u2*pow(a1,2)/pow(vel_son_supuesta,2);
// *error=pow(a1,2)+FGamma3*pow(u1,2)-pow(FCilindro->getSpeedsound()/__cons::ARef,2);
//
// }
// catch(Exception &N)
// {
// std::cout << "ERROR: TCCCilindro::FSSubcritico en la condicion de contorno: " << FNumeroCC << std::endl;
// std::cout << "Tipo de error: " << N.what() << std::endl;
// throw Exception(N.what());
// }
// }
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//
// void TCCCilindro::FSSupercritico(double vel_supuesta,double *a2_1,double *a2_2)
// {
// try
// {
//
//// Resolucion de la ecuacion de la energia entre el cilindro y el extremo del tubo.
// *a2_1 = sqrt(pow(FCilindro->getSpeedsound()/__cons::ARef,2)-FGamma3*pow(vel_supuesta,2));
//
//// Resolucion de la ecuacion 4.20 de la tesis de Corberan.
// *a2_2 = sqrt(vel_supuesta*pow((*FCC+FGamma3*vel_supuesta)/
// FTuboExtremo[0].Entropia,FGamma4)/Fcc);
//
// }
// catch(Exception &N)
// {
// std::cout << "ERROR: TCCCilindro::FSSupercritico en la condicion de contorno: " << FNumeroCC << std::endl;
// std::cout << "Tipo de error: " << N.what() << std::endl;
// throw Exception(N.what());
// }
// }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TCCCilindro::ActualizaAnguloValvula(double TiempoActual, double Regimen) {
	try {

		FTime0 = FTime1;
		FTime1 = TiempoActual;
		FDeltaT = FTime1 - FTime0;
		FDeltaAngulo = 360. * Regimen / 60. * FDeltaT;
		FAnguloAnterior = FAnguloActual;
		FAnguloActual = FAnguloAnterior + FDeltaAngulo;
		if(FAnguloActual > 720.) {
			FAnguloActual -= 720.;
		}

	} catch(exception & N) {
		std::cout << "ERROR: TCCCilindro::ActualizaAnguloValvula en la condicion de contorno: " << FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

#pragma package(smart_init)
