/*--------------------------------------------------------------------------------*\
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


 \*--------------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// En la Tesis de Jose Miguel Corberan, paginas 104-139
// Resumido en el articulo "Solucion a la condicion de contorno de la union
// cilindro-conducto de los MCIA" (Esta en el COMETT PROGRAMME 1995)
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#pragma hdrstop

#include "TCCCilindro.h"
//#include <cmath>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include "TValvula4T.h"
#include "TTubo.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCCCilindro::TCCCilindro(nmTypeBC TipoCC, int numCC) :
		TCondicionContorno(TipoCC, numCC) {

	if (TipoCC == nmIntakeValve)
		FTipoValv = nmValvAdmision;
	else if (TipoCC == nmExhaustValve)
		FTipoValv = nmValvEscape;
	else
		printf(
				"ERROR en tipo de valvula TCCCilindro en la condicion de contorno: %d\n",
				FNumeroCC);

	FTuboExtremo = NULL;
	FValvula = NULL;

	FGasto = 0;
	FVelocity = 0;
	FSonido = 1;
	FMomento = 0;
	FPresionCil = 1;
	FVelSonidoCil = 1;
	FMomentoAngular = 0;
	FMasaCilindro = 0;
	FVelocidadGarganta = 0;
	FMachGarganta = 1;
	FGastoGarganta = 0;
	FRelacionPresionGarganta = 1;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCCCilindro::~TCCCilindro() {

	delete[] FTuboExtremo;

	if (FValvula != NULL)
		delete FValvula;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCilindro::AsignaTipoValvula(TTipoValvula **Origen, int Valv, int i) {
	try {

		FValvula = new TValvula4T(dynamic_cast<TValvula4T *>(Origen[Valv - 1]),
				i);

		FValvula->putDiametroTubo(FTuboExtremo[0].Pipe->GetDiametro(FNodoFin));

	} catch (Exception &N) {
		std::cout
				<< "ERROR: TCCCilindro::AsignaTipoValvula en la condicion de contorno: "
				<< FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCilindro::LeeCCValvula(const char *FileWAM, fpos_t &filepos,
		int NumberOfPipes, TTubo **Pipe) {
	try {
		int i = 0;

		FTuboExtremo = new stTuboExtremo[1];
		FTuboExtremo[0].Pipe = NULL;

		FPref = 1;

		while (FNumeroTubosCC < 1 && i < NumberOfPipes) {
			if (Pipe[i]->getNodoIzq() == FNumeroCC) {
				FNumeroTubosCC++;
				FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
				FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmLeft;
				FNodoFin = 0;
				FCC = &(FTuboExtremo[FNumeroTubosCC].Beta);
				FCD = &(FTuboExtremo[FNumeroTubosCC].Landa);
				FSigno = 1;   // No hara falta en el futuro.
			}
			if (Pipe[i]->getNodoDer() == FNumeroCC) {
				FNumeroTubosCC++;
				FTuboExtremo[FNumeroTubosCC].Pipe = Pipe[i];
				FTuboExtremo[FNumeroTubosCC].TipoExtremo = nmRight;
				FNodoFin = Pipe[i]->getNin() - 1;
				FCC = &(FTuboExtremo[FNumeroTubosCC].Landa);
				FCD = &(FTuboExtremo[FNumeroTubosCC].Beta);
				FSigno = -1;  // No hara falta en el futuro.
			}
			i++;
		}
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%d ", &FNumeroCilindro);

		fgetpos(fich, &filepos);
		fclose(fich);

	}

	catch (Exception &N) {
		std::cout
				<< "ERROR: TCCCilindro::AsignaTubos en la condicion de contorno: "
				<< FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCilindro::ObtencionValoresInstantaneos() {
	try {
		/* LO QUE SE NECESITA SE OBTENDRA A COMUNICANDO CON EL OBJETO CILINDRO
		 FPresionCil=presion_cil;
		 FVelSonidoCil=vel_sonido_cil;
		 FMomentoAngular=momento_angular;
		 FMasaCilindro=masa_cilindro;

		 dynamic_cast<TValvula4T *>(FValvula)->CalculaCD(Angulo);

		 FCDEntrada=FValvula->getCDTubVol();
		 FCDSalida=FValvula->getCDVolTub();
		 FCTorbellino=FValvula->getCTorb();

		 if (FCDEntrada > 2.0 || FCDEntrada < 0.0) {
		 printf("ERROR: TCCDeposito::ObtencionValoresInstantaneos, en calculo coeficiente descarga entrante: %lf, en %lf grados, en la condicion de contorno: %d\n",FCDEntrada,Angulo,FNumeroCC);
		 throw Exception("ERROR: en calculo coeficiente descarga entrante: "+AnsiString(FCDEntrada)+", en "+AnsiString(Angulo)+" grados");
		 }
		 if (FCDSalida > 1.0 || FCDSalida < 0.0) {
		 printf("ERROR: TCCDeposito::ObtencionValoresInstantaneos, en calculo coeficiente descarga saliente: %lf, en %lf grados, en la condicion de contorno: %d\n",FCDSalida,Angulo,FNumeroCC);
		 throw Exception("ERROR: en calculo coeficiente descarga saliente: "+AnsiString(FCDSalida)+", en "+AnsiString(Angulo)+" grados ");
		 }
		 */
	} catch (Exception &N) {
		std::cout
				<< "ERROR: TCCCilindro::ObtencionValoresInstantaneos en la condicion de contorno: "
				<< FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCilindro::CalculaCondicionContorno() {
	try {
		double rel_CCon_Entropia, coef;

		FSeccionValvula = Pi * pow2(FValvula->getDiametro()) / 4.;
		FSeccionTubo = Pi * pow2(FTuboExtremo[0].Pipe->GetDiametro(FNodoFin))
				/ 4.;

		FAd = pow(FPresionCil / FPref, 1. / Gamma4);
		rel_CCon_Entropia = *FCC / FTuboExtremo[0].Entropia;
		if (rel_CCon_Entropia / FAd > 1.000005) {  // Flujo entrante al cilindro
			/* ________ */
			/* caso >1  */
			/* ________ */
			if (FCDEntrada > 0.0001) { /* Abierto */
				FSeccionEficaz = FCDEntrada * FSeccionValvula;
				FlujoEntranteCilindro();
				/*       CALCULO DEL MOMENTO ANGULAR ENTRANTE L */
				if (FGasto < -1e-5) {
					if (FTipoValv == nmExhaustValve) {
						coef = FCTorbellino / 4.;
					} else {
						coef = FCTorbellino;
					}
					FMomento = coef * pow2(ARef * FGasto)
							/ (200000 * FCarrera * Gamma * FPresionCil)
							* (pow2(*FCC + *FCD) / 4.
									+ Gamma3 * pow2((*FCD - *FCC) / Gamma1));
				}
				/*if (i == 0) {
				 *g1lf = FCDEentrada * sval;// Se refiere a salida de resultados del cilindro.A revisar.
				 } */
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
				/*if (i == 0) {
				 *g1lf = 0.;
				 } */
			}
		} else if (rel_CCon_Entropia / FAd < .999995) { // Flujo saliente del cilindro
			/* ________ */
			/* caso <1 */
			/* ________ */
			if (FCDSalida > 0.0001) { /* Abierto */
				FSeccionEficaz = FCDSalida * FSeccionValvula;
				FlujoSalienteCilindro();
				/* CALCULO DEL MOMENTO ANGULAR SALIENTE */
				if (FGasto > 1e-5) {
					FMomento = -FMomentoAngular * FGasto / FMasaCilindro;
				}
				/*if (i == 0) {
				 *g1lf = cdsal * sval;
				 } */
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
				/*if (i == 0) {
				 *g1lf = 0.;
				 }    */
			}
		} else {   // Flujo Parado
			/* ________ */
			/* caso =1  */
			/* ________ */
			*FCD = *FCC;
			FGasto = 0.;
			FVelocity = 0.;
			FSonido = *FCD;
			FVelocidadGarganta = 0.;
			FMachGarganta = 0.;
			FGastoGarganta = 0.;
			FRelacionPresionGarganta = 0.;
			FSeccionEficaz = 0.;
		}
	} catch (Exception &N) {
		std::cout
				<< "ERROR: TCCCilindro::CalculaCondicionContorno en la condicion de contorno: "
				<< FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCilindro::FlujoEntranteCilindro() {
	try {
		double vel_son_garganta, velocidad_garganta, Mach, xaa2, ycal, d1;

		Fk = FSeccionTubo / FSeccionEficaz;
		vel_son_garganta = FTuboExtremo[0].Entropia * FAd; //Velocity del sonido en la garganta. Adimensional.

//Calculo de la velocidad en la garganta.Caso de salto subcritico.
		FCaso = nmFlujoEntranteSaltoSubcritico;
		Resolucion(vel_son_garganta, *FCC, FCaso, &FVelocity, &FSonido, 1e-5,
				2e-6);

//Ecuacion de la energia
		velocidad_garganta = sqrt(
				2. * Gamma6
						* (pow2(FSonido) + Gamma3 * pow2(FVelocity)
								- pow2(vel_son_garganta)));
//Se ha calculado la velocidad en la garganta en valor absoluto.

//Calculo de la velocidad en la garganta en el caso de salto supercritico
		if (velocidad_garganta > vel_son_garganta) {
			FCaso = nmFlujoEntranteSaltoSupercritico;
			Resolucion(0.0, 1.0, FCaso, &ycal, &Mach, 1e-5, 2e-6);
			FVelocity = -*FCC * FSigno / (1 / Mach + Gamma3);
			FSonido = -FVelocity * FSigno / Mach;

			d1 = Fk * fabs(FVelocity) * pow(FSonido, 1. / Gamma3);
			vel_son_garganta = pow(d1, Gamma1 / Gamma2);
			velocidad_garganta = vel_son_garganta;  // En valor absoluto.
		}
//Fin caso de salto supercritico

		xaa2 = pow(FTuboExtremo[0].Entropia, Gamma4);
		FGasto = -fabs(
				Gamma * FSeccionTubo * pow(FSonido, 2 * Gamma6) * FVelocity
						* 1e5 / (ARef * xaa2)); // Massflow entrante al cilindro negativo
		*FCD = FSonido + Gamma3 * FVelocity * FSigno;
		*FCC = FSonido - Gamma3 * FVelocity * FSigno;
		FRelacionPresionGarganta = pow(
				FSonido / (FTuboExtremo[0].Entropia * FAd), Gamma4);
		FGastoGarganta = FGasto / (FCDEntrada * FSeccionValvula);
		FMachGarganta = fabs(velocidad_garganta / vel_son_garganta); // En valor absoluto.
		FVelocidadGarganta = -FSigno * velocidad_garganta;
	}

	catch (Exception &N) {
		std::cout
				<< "ERROR: TCCCilindro::FlujoEntranteCilindro en la condicion de contorno: "
				<< FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCilindro::FlujoSalienteCilindro() {
	try {

		double a1 = 0., xx = 0., yy = 0., d1 = 0.;
		double a2cr = 0., val1 = 0., val2 = 0., u2cr = 0., ycal = 0., error = 0., valde = 0., miembro2 = 0.;
//Variables para resolver la onda de choque.
		double relacion_velocidades_son, Mach_tras_ondachoque, Mach,
				temp_antes_ondachoque, temp_tras_ondachoque;

		Fk = FSeccionTubo / FSeccionEficaz;

		/* Calculo del valor de la velocidad del sonido en el extremo del tubo para
		 el cual el salto es critico. */
		u2cr = FVelSonidoCil * sqrt(2. / Gamma2)
				* (sqrt(pow2(Fk) + Gamma1 * Gamma2) - Fk) * FSigno / Gamma1;
		a2cr = sqrt(pow2(FVelSonidoCil) - Gamma3 * pow2(u2cr)); //Ecuacion de la energia. Garganta-Cylinder.

		/* A partir  de a2cr se determina el error en el calculo de A2 al suponer salto
		 subcritico. Si es negativo, el salto es supercritico. Si es positivo, el salto
		 es subcritico. */
		FSSubcritico(a2cr, &error, &miembro2);

		if (error < 0.) { // Salto de presiones supercritico.

			/* Determinacion del intervalo de iteracion. Para ello se supone que
			 en el extremo del tubo se dan las condiciones criticas. Explicado en
			 los apuntes de Pedro. */
			a1 = sqrt(2. / Gamma2) * FVelSonidoCil;
			FVelocidadGarganta = FSigno * a1;
			xx = pow(FAd / FVelSonidoCil, Gamma4);
			yy = pow(a1, 2. / Gamma1);
			Fcc = FVelocidadGarganta * yy * xx / Fk;
			FSSupercritico(FVelocidadGarganta, &val1, &val2);
			if (val1 - val2 < 0.)
				valde = FVelocidadGarganta;
			if (val1 - val2 >= 0.)
				valde = FSigno * FVelSonidoCil / sqrt(Gamma3);

			/* Una vez conocido el intervalo de iteracion, se pasa a la resolucion
			 del caso flujo saliente salto supercritico. */
			FCaso = nmFlujoSalienteSaltoSupercritico;
			Resolucion(0.0, valde, FCaso, &FVelocity, &FSonido, 1e-5, 2e-6);

			// Calcula del massflow. Como es saliente del cilindro, siempre es positivo.
			xx = pow(sqrt(2. / Gamma2), (Gamma2 / Gamma1));
			yy = pow(FAd, Gamma4);
			FGasto = FCDSalida * FSeccionValvula * Gamma * xx * yy * 1e5
					/ (FVelSonidoCil * ARef);

			/* Reduccion a flujo subsonico mediante onda de choque plana en el caso
			 de que se hayan obtenido condiciones supersonicas en el extremo del
			 tubo. Explicado en la tesis Corberan (pagina de la 47 a la 52
			 (punto 2.5) y de la 122 a la 129 (lo importante a partir de la 127) */
			Mach = fabs(FVelocity / FSonido);
			xx = *FCC + Gamma3 * FVelocity * FSigno;
			FTuboExtremo[0].Entropia = FTuboExtremo[0].Entropia * FSonido / xx; // Ecuacion de la caracteristica incidente.
			if (Mach > 1.) {

				/* Las ecuaciones siguientes corresponden a la resolucion de la onda
				 de choque plana. Se pueden encontrar en el punto 2.5 de la tesis
				 de Corberan.  */
				xx = Gamma4 * pow2(Mach) - 1.;
				Mach_tras_ondachoque = sqrt((pow2(Mach) + 2. / Gamma1) / xx);
				temp_tras_ondachoque = Gamma3 * pow2(Mach) + 1.;
				temp_antes_ondachoque = Gamma3 * pow2(Mach_tras_ondachoque)
						+ 1.;
				relacion_velocidades_son = sqrt(
						temp_tras_ondachoque / temp_antes_ondachoque);
				FSonido = FSonido * relacion_velocidades_son;
				FVelocity = FSonido * Mach_tras_ondachoque * FSigno;
				d1 = xx * Gamma1 / Gamma2;
				FTuboExtremo[0].Entropia = FTuboExtremo[0].Entropia
						* relacion_velocidades_son / pow(d1, Gamma5);
			}

		} else { // Salto de presiones subcritico.

			// Resolucion del caso de flujo saliente salto subcritico.
			FCaso = nmFlujoSalienteSaltoSubcritico;
			Resolucion(a2cr, FVelSonidoCil, FCaso, &ycal, &FSonido, 1e-5, 2e-6);
			// Aplicando la Ecuacion de la Energia entre el cilindro y la garganta:
			FVelocity = sqrt((pow2(FVelSonidoCil) - pow2(FSonido)) / Gamma3)
					* FSigno;

			// Calculo del massflow. Como es saliente del cilindro, siempre es positivo.
			a1 = FVelSonidoCil * (*FCC + Gamma3 * FVelocity * FSigno)
					/ (FTuboExtremo[0].Entropia * FAd);
			FVelocidadGarganta = Fk * pow2(a1) * FVelocity / pow2(FSonido);
			FGasto = fabs(
					FCDSalida * FSeccionValvula * Gamma
							* pow(FAd / FVelSonidoCil, Gamma4)
							* FVelocidadGarganta * pow(a1, 2. / Gamma1) * 1e5
							/ ARef);
			xx = *FCC + Gamma3 * FVelocity * FSigno;
			FTuboExtremo[0].Entropia = FTuboExtremo[0].Entropia * FSonido / xx; // Ecuacion de la caracteristica incidente.
		}
		*FCD = FSonido + Gamma3 * FVelocity * FSigno;
		*FCC = FSonido - Gamma3 * FVelocity * FSigno;
		d1 = FSonido / (FTuboExtremo[0].Entropia / FAd);
		FRelacionPresionGarganta = pow(d1, Gamma4);
		FMachGarganta = fabs(FVelocidadGarganta / a1); // En valor absoluto.
		FGastoGarganta = FGasto / (FCDSalida * FSeccionValvula);
	}

	catch (Exception &N) {
		std::cout
				<< "ERROR: TCCCilindro::FlujoSalienteCilindro en la condicion de contorno: "
				<< FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCilindro::Resolucion(double ext1, double ext2, nmCaso Caso, double *u2t,
		double *a2t, double prey, double prex) {
	try {
		int contador = 0;
		double xx, xx1 = 0., xx2 = 0., ed, ei, medio = 0.;
		bool salida;

		contador = 1;
		ei = ext1;
		ed = ext2;

		salida = false;
		while (contador <= 800 && !salida) {
			medio = (ei + ed) / 2.;
			switch (Caso) {
			case nmFlujoEntranteSaltoSubcritico:
				FESubcritico(medio, &xx1, &xx2);
				break;
			case nmFlujoEntranteSaltoSupercritico:
				FESupercritico(medio, &xx1, &xx2);
				break;
			case nmFlujoSalienteSaltoSubcritico:
				FSSubcritico(medio, &xx1, &xx2);
				break;
			case nmFlujoSalienteSaltoSupercritico:
				FSSupercritico(medio, &xx1, &xx2);
				break;
			default:
				printf(
						"Error en la definicion del flujo TCCCilindro::Resolucion en la condicion de contorno: %d\n",
						FNumeroCC);
				throw Exception("");
			} /*fin del switch*/

			xx = xx1 - xx2;
			if (xx1 + xx2 != 0.)
				xx = xx * 2. / fabs(xx1 + xx2);
			if (fabs(ei - ed) > prex) {
				if (xx > prey || xx <= -prey) {
					if (xx > prey) {
						ei = medio;
					}
					if (xx <= -prey) {
						ed = medio;
					}
				} else {
					salida = true;
				}
			} else {
				salida = true;
			}
			++contador;
		}
		if (salida) {
			if (ei == 0.)
				ei = medio; /*evita posibles errores*/

			switch (Caso) {
			case nmFlujoEntranteSaltoSubcritico:
				*u2t = xx1;
				*a2t = ed;
				break;
			case nmFlujoEntranteSaltoSupercritico:
				*u2t = xx2;
				*a2t = ei;
				break;
			case nmFlujoSalienteSaltoSubcritico:
				*a2t = ei;
				*u2t = xx2;
				break;
			case nmFlujoSalienteSaltoSupercritico:
				*a2t = xx2;
				*u2t = ei;
				break;
			default:
				printf(
						"Error en la definicion del flujo TCCCilindro::Resolucion en la condicion de contorno: %d\n",
						FNumeroCC);
				throw Exception("");
			} /*fin del switch*/
		}
		if (contador > 800) {
			printf(
					"ERROR: TCCCilindro::Resolucion. No se cumplen las condiciones de convergencia en la condicion de contorno: %d\n",
					FNumeroCC);
		}
	}

	catch (Exception &N) {
		std::cout
				<< "ERROR: TCCCilindro::Resolucion en la condicion de contorno: "
				<< FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCilindro::FESubcritico(double vel_son_supuesta, double *u2_1,
		double *u2_2) {
	try {

		double xx = 0., yy = 0., u2 = 0.;

		/* Resolucion de la ecuacion (20) del articulo "Solucion a la condicion de
		 contorno de la union cilindro-conducto de los MCIA". Ecuacion 4.30 en la
		 tesis de Corberan */

		xx = vel_son_supuesta / (FTuboExtremo[0].Entropia * FAd);
		yy = pow(xx, 4. * Gamma6);
		yy = pow2(Fk) * yy - 1.;
		u2 = -FTuboExtremo[0].Entropia * FAd
				* sqrt(2. * Gamma6 * (pow2(xx) - 1.) / yy) * FSigno;

// Se pone el siguiente if para que en Resolucion la eleccion del intervalo
// de calculo sea la adecuada. En la pagina 59 de Apuntes de Pedro esta explicado.
		if (u2 >= 0) {
			*u2_2 = u2;
			/* Resolucion de la ecuacion de la caracteristica incidente. */

			*u2_1 = (vel_son_supuesta - *FCC) * FSigno / Gamma3;
		} else {
			*u2_1 = u2;
			/* Resolucion de la ecuacion de la caracteristica incidente. */

			*u2_2 = (vel_son_supuesta - *FCC) * FSigno / Gamma3;
		}
	} catch (Exception &N) {
		std::cout
				<< "ERROR: TCCCilindro::FESubcritico en la condicion de contorno: "
				<< FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCilindro::FESupercritico(double mach_supuesto, double *miembro1,
		double *miembro2) {
	try {

		double xx = 0., yy = 0.;

		/* Resolucion de la ecuacion (21) del articulo "Solucion a la condicion de
		 contorno de la union cilindro-conducto de los MCIA". Ecuacion (4.31) de
		 la tesis de Corberan */

		yy = (Gamma2 / 2.) * pow(Fk * mach_supuesto, 2. * Gamma1 / Gamma2);
		xx = Gamma3 * pow2(mach_supuesto);
		*miembro1 = xx - yy + 1.;     // Miembro 1 de la ecuacion (21)
		*miembro2 = 0;            // Miembro 2 de la ecuacion (21)

	} catch (Exception &N) {
		std::cout
				<< "ERROR: TCCCilindro::FESupercritico en la condicion de contorno: "
				<< FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCilindro::FSSubcritico(double vel_son_supuesta, double *error,
		double *miembro2) {
	try {
		double a1 = 0., u1 = 0., u2 = 0.;

		*miembro2 = 0;

		/* Resolucion del algoritmo de calculo propuesto en la pagina 113 de la tesis
		 de Corberan. */

		u2 = sqrt((pow2(FVelSonidoCil) - pow2(vel_son_supuesta)) / Gamma3)
				* FSigno;
		a1 = FVelSonidoCil * (*FCC + Gamma3 * u2 * FSigno)
				/ (FTuboExtremo[0].Entropia * FAd);
		u1 = Fk * u2 * pow2(a1) / pow2(vel_son_supuesta);
		*error = pow2(a1) + Gamma3 * pow2(u1) - pow2(FVelSonidoCil);

	} catch (Exception &N) {
		std::cout
				<< "ERROR: TCCCilindro::FSSubcritico en la condicion de contorno: "
				<< FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCCilindro::FSSupercritico(double vel_supuesta, double *a2_1,
		double *a2_2) {
	try {

// Resolucion de la ecuacion de la energia entre el cilindro y el extremo del tubo.
		*a2_1 = sqrt(pow2(FVelSonidoCil) - Gamma3 * pow2(vel_supuesta));

// Resolucion de la ecuacion 4.20 de la tesis de Corberan.
		*a2_2 = sqrt(
				vel_supuesta
						* pow(
								(*FCC + Gamma3 * vel_supuesta * FSigno)
										/ FTuboExtremo[0].Entropia, Gamma4)
						/ Fcc); // Si vel_supuesta < 0, Fcc tambien lo es.

	} catch (Exception &N) {
		std::cout
				<< "ERROR: TCCCilindro::FSSupercritico en la condicion de contorno: "
				<< FNumeroCC << std::endl;
		std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
		throw Exception(N.Message);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)

