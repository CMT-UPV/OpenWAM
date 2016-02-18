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
#ifndef BoundaryFunctionsH
#define BoundaryFunctionsH

//#include "roots.h"
// #include "Math_wam.h"
#include "Globales.h"
// ---------------------------------------------------------------------------

struct stFESub {
	double AA;
	double Ad;
	double Gam;
	double K;
	double BC;
	double Ga3;

	double U2;
	double U2_2;
	double AdAA;
	double invAdAA;

	stFESub(const double iAA, const double iAd, const double ig, const double iK, const double iCC) :
		AA(iAA), Ad(iAd), Gam(ig), K(iK), BC(iCC) {
		Ga3 = (Gam - 1) / 2;
		AdAA = Ad * AA;
		invAdAA = 1 / AdAA;
	}
	;

	double operator()(const double A2) {
		double xx = A2 * invAdAA;
		double yy = pow(xx, 2 / Ga3);
		yy = pow2(K) * yy - 1.;
		U2_2 = 0;
		if(xx > 1)
			U2_2 = AdAA * sqrt((pow2(xx) - 1.) / (yy * Ga3)); // Valor absoluto
		U2 = (BC - A2) / Ga3;
		return U2_2 - U2;
	}
};

struct stFESup {
	double Gam;
	double K;
	double Ga3;
	double Ga8;
	double Ga9;

	stFESup(const double ig, const double iK) :
		Gam(ig), K(iK) {
		Ga3 = (Gam - 1) / 2;
		Ga8 = (Gam + 1) / 2;
		Ga9 = (Gam - 1) / Ga8;
	}
	;

	double operator()(const double M) {

		double yy = Ga8 * pow(K * M, Ga9);
		double xx = Ga3 * pow2(M);
		return xx - yy + 1.;
	}
};

struct stFSSub {
	double AA;
	double Ad;
	double Gam;
	double K;
	double BC;
	double Ga3;
	double Ac;

	double U2;
	double invAdAA;
	double pow2Ac;

	stFSSub(const double iAA, const double iAd, const double ig, const double iK, const double iCC, const double iAc) :
		AA(iAA), Ad(iAd), Ac(iAc), Gam(ig), K(iK), BC(iCC) {
		Ga3 = (Gam - 1) / 2;
		invAdAA = 1 / (AA * Ad);
		pow2Ac = pow2(Ac);
	}
	;

	double operator()(const double A2) {
		U2 = 0;
		if(pow2Ac > pow2(A2)) {
			U2 = sqrt((pow2Ac - pow2(A2)) / Ga3);
		}
		double A1 = Ac * (BC + Ga3 * U2) * invAdAA;
		double U1 = K * U2 * pow2(A1 / A2);
		return pow2(A1) + Ga3 * pow2(U1) - pow2Ac;
	}
};

struct stFSSup {
	double AA;
	double Fcc;
	double Gam;
	double K;
	double BC;
	double Ga3;
	double Ac;

	double A2;
	double invAA;
	double invFcc;

	stFSSup(const double iAA, const double iFcc, const double ig, const double iK, const double iCC, const double iAc) :
		AA(iAA), Ac(iAc), Gam(ig), K(iK), BC(iCC), Fcc(iFcc) {
		Ga3 = (Gam - 1) / 2;
		invAA = 1 / AA;
		invFcc = 1 / Fcc;

	}
	;

	double operator()(const double U2) {

		double dif = pow2(Ac) - Ga3 * pow2(U2);
		if(dif > 0) {
			A2 = sqrt(dif);
		} else {
			A2 = 0;
		}
		//A2 = sqrt(pow2(Ac) - Ga3 * pow2(U2));
		double A2_2 = sqrt(U2 * pow((BC + Ga3 * U2) * invAA, Gam / Ga3) * invFcc);

		return A2 - A2_2;
	}
};

struct stRecover {

	double AA;
	double Ad;
	double Gam;
	double CR;
	double BC;
	double Ga3;

	double A2_2;
	double U2;
	double UThroat;
	double A1;
	double invAdAA;

	stRecover(const double iAA, const double iAd, const double ig, const double iCR, const double iCC) :
		AA(iAA), Ad(iAd), Gam(ig), CR(iCR), BC(iCC) {
		Ga3 = (Gam - 1) / 2;
		invAdAA = 1 / (Ad * AA);
	}
	;

	double operator()(double A2) {
		U2 = (BC - A2) / Ga3;
		double Ga3U22 = Ga3 * pow2(U2);
		double Adep1 = sqrt(pow2(A2) + Ga3U22);
		A1 = A2 * Adep1 * invAdAA;
		UThroat = U2 * CR * pow2(A1 / A2);
		double Adep2 = sqrt(pow2(A1) + Ga3 * pow2(UThroat));
		A2_2 = sqrt(pow2(Adep2) - Ga3U22);
		return A2 - A2_2;
	}
};
/* NUEVO- PROGRAMACION DE LAS EXPRESIONES NECESARIAS PARA
 EL METODO DE BRENT:
 - Union Tubos: Ensanchamiento -	Estrechamiento
 - Compressor Volumetrico
 - Perdida de presion adiabatica
 */

/* Wider in cross section */
struct stExpansion {

	/* !Definicion de las VARIABLES LOCALES */
	/*Variables locales que almacenaran la inforamcion pasada por funcion */

	double CCS; // Variable local asociada a la caracteristica conocida en 1 (salida del tubo) y que se pasa por funcion//
	double CCE; // Variable l.a. a la caracteristica conocida en 2, entrada al tubo//
	double Gam; // Variable l.a. al Gamma pasado por funcion//
	double rel_area; // Variable l.a. a la relacion de area pasada por funcion//
	double rel_entropia; // Variable l.a. a la relacion de entropia pasada por funcion//

	/* Variables locales internas, si no se quiere acceder despues a la
	 informacion que contienen, es preferible definirlas directamente dentro del
	 operador */

	double U1; // V.L para la velocidad del fluido en 1 (SALIENTE del tubo//
	double U2; // V.L para la velocidad del fluido en 2 (ENTRANTE al deposito//
	double A2; // V.L para la velocidad del sonido en 2//
	double xx3; // V.L para la velocidad del sonido, hara falta en el calculo de las caracteristicas//
	double Ga2; // Es necesario definirlo aqui, ya que se utiliza en el operador//
	double Ga3; // idem que Ga2//

	/* CONSTRUCTOR de la funcion, aqui se recibe por orden la informacion pasada por
	 funcion y se almacena en las variables locales */

	stExpansion(const double iCCS, const double iCCE, const double R_E, const double R_A, const double iGam) :
		CCS(iCCS), CCE(iCCE), rel_entropia(R_E), rel_area(R_A), Gam(iGam) {
		Ga3 = (Gam - 1) / 2;
		/* Definicion de aquellas variables internas que se usaran en el operador, pero que no se requiere su acceso, aqui o dentro del operador */
		Ga2 = Gam + 1;
	}

	double operator()(const double A1) { // A1 es la velocidad del sonido en 1 es LA VARIABLE DE LA FUNCION//

		U1 = (CCS - A1) / Ga3; // Calculo de la velocidad del fluido en 1 mediante la ecuacion 4.291//
		double xx1 = pow2(A1) + Ga3 * pow2(U1);
		double xx2 = rel_area * pow2(A1) + Gam * pow2(U1);
		if(fabs(U1 - A1) < 1e-15) {
			if(Ga2 / (Gam + rel_area) >= 1) {
				U2 = (xx2 / (Ga2 * U1));
			} else {
				U2 = (xx2 / (Ga2 * U1)) * (1 - sqrt(1 - pow2(Ga2 / (Gam + rel_area))));
			}
		} else {
			if((xx1 * 2. * Ga2 * (pow2(U1) / pow2(xx2))) >= 1) {
				U2 = xx2 / Ga2;
			} else {
				U2 = xx2 * (1 - sqrt(1. - (xx1 * 2. * Ga2 * (pow2(U1) / pow2(xx2))))) / Ga2;
			}

			if(U1 == 0) {
				U2 = 0;
			} else {
				U2 = U2 / U1;
			}
		}
		A2 = sqrt(xx1 - Ga3 * pow2(U2));
		xx3 = CCE + Ga3 * U2; // Velocity del sonido corregida, hara falta en el calculo de las caracteristicas//
		double xx = pow(rel_entropia * xx3, Gam);
		double xtx = (pow2(A2) + Gam * pow2(U2)) / (((Gam * pow2(U1)) / rel_area + pow2(A1)) * pow2(A2));
		xtx = pow(xtx, Ga3);
		double A1p = xx * xtx;

		return A1 - A1p;
	}
};
/* ! Reduction in cross section */

struct stContraction {

	/* Definicion de variables locales */
	/* Variables locales para almacenar la informacion pasada por funcion */

	double CCS; // Variable local asociada a la caracteristica incidente conocida en 1, pasada por funcion//
	double CCE; // Variable l.a a la caracteristica conocida en 2 inicial, pasada por funcion//
	double rel_entropia; // Variable l.a a la relacion de entropia pasada por funcion//
	double rel_area; // Variable l.a a la relacion de area, pasada por funcion//
	double Gam; // Variable l.a a gamma, pasado por funcion//

	/* Variables locales internas, que permitiran el acceso. Si no se requiere acceso
	 es mejor definirlas directamente dentro del operador */

	double A1; // Variable local para la velocidad del sonido en 1//
	double A2; // Variable local para la velocidad del sonido en 2//
	double U2; // Variable local para la velocidad del flujo en 2//
	double Ga1;
	double Ga3;

	/* CONSTRUCTOR: Se recibe la informacion por funcion en variables ficticias y se
	 almacena en las variables locales */

	stContraction(const double iCCS, const double iCCE, const double R_E, const double R_A, const double iGam) :
		CCS(iCCS), CCE(iCCE), rel_entropia(R_E), rel_area(R_A), Gam(iGam) {
		Ga3 = (Gam - 1) / 2;
		Ga1 = (Gam - 1);
	}

	double operator()(const double U1) { // U1 es la velocidad del fluido en 1, es la variable a iterar de la funcion//

		A1 = CCS - Ga3 * U1; // Calculo de A1 por la ecuacion 4.315//
		double xx1 = rel_entropia * 4. * CCE / Ga1; // Variable auxiliar para el calculo de A2//
		double xx2 = pow2(rel_entropia) / Ga3 + 1.; // Variable auxiliar para el calculo de A2//
		double xx3 = pow2(CCE) / Ga3 - Ga3 * pow2(U1) - pow2(A1);
		// Variable auxiliar para el calculo de A2//
		double b = pow2(xx1) - xx2 * 4. * xx3;
		if(b < 0) {  /* Adaptacion del metodo para este caso problematico */
			A2 = xx1 / (2 * xx2);
		} else {
			A2 = (xx1 + sqrt(b)) / (2. * xx2); // Calculo de A2 segun la ec. 4.316, si el //
		}
		/* Si el flujo es supersonico en la seccion 1 (diametro mayor) peta el metodo de Brent peta al calcular el valor en el extremo
		 derecho, debido a que sale una raiz negativa. Probar entonces una condicion que bloquee en este caso el calculo de la raiz
		 (pero solo para el calculo de la funcion en el extremo). Por ejemplo A2=xx1/2*xx2. YA VEREMOS QUE PASA, SI CONVERGE O SIGUE PETANDO
		 ENTONCES SERIA FLUJO SUPERSONICO */

		U2 = (A2 * rel_entropia - CCE) / Ga3; // Calculo de U2 segun ec.4.317//
		double U1p = A1 / A2; // Variable auxiliar para el calculo de U1p//
		double xx = 1. / Ga3; // Variable auxiliar para el calculo de U1p//
		U1p = rel_area * U2 / pow(U1p, xx); // Nuevo valor de la velocidad del flujo U1, segun la ec.4.318//

		return U1 - U1p;
	}
};

/* Adiabatic loss pressure */

struct stPerdPresAd {

	/* Definir las variables locales donde se almacenara la informacion que se pase
	 desde el programa y que son necesarias para el calculo de la condicion de contorno.
	 Son variables de tipo double */

	// VARIABLES ALMACENAMIENTO DE VARIABLES QUE SE PASARAN//
	double CC1; // Variable local asociada a la caracteristica incidente del extremo 1 pasada por funcion//
	double CC2; // Variable l.a. a la caracteristica incidente del extremo 2 pasada por funcion//
	double FK; // Variable l.a. al parametro K pasado por funcion//
	double Gam; // Variable l.a. a Gamma pasado por funcion//
	double FRE; // Variable l.a. a la relacion de entropia que se pasa por funcion//

	// VARIABLES INTERNAS//
	double U1; // Variable local para almacenar la velocidad del fluido en 1//
	double U2; // Variable local para almacenar la velocidad del fluido en 2//
	double A2; // Variable local para almacenar la velocidad del sonido en 2//
	double A1p; // Variable local para almacenar el valor calculado de A1 segun 4.256//
	double xx3;
	double Ga3;
	double Ga5;

	/* Constructor de la funcion. Aqui se RECIBEN las variables necesarias para el calculo
	 y que han sido enviadas desde el programa, el nombre de cabecera es ficticio, la
	 informacion se almacena por posicion. En el lado derecho se asocia a cada variable
	 local el valor de la variable pasada por funcion */

	stPerdPresAd(const double iCC1, const double iCC2, const double iFK, const double iGam, const double iFRE) :
		CC1(iCC1), CC2(iCC2), FK(iFK), Gam(iGam), FRE(iFRE) {
		Ga3 = (Gam - 1) / 2;
		Ga5 = (Gam - 1) / 2 / Gam;
	}

	double operator()(const double A1) { // A1 es la variable incognita de la funcion, es decir sobre la que se itera//

		U1 = (CC1 - A1) / Ga3; // ecuacion 4.246//
		double a = Ga3;
		double b1 = -FK * pow2(U1) / pow2(A1) + 1.;
		double b = pow2(A1) * b1;
		double c = -(pow2(A1) + Ga3 * pow2(U1)) * pow2(U1);
		double u2u1 = (sqrt(pow2(b) - 4. * a * c) - b) / (2. * a); // ec. 252//
		if(U1 == 0.) {
			U2 = 0.; // ec.253//
		} else { // ec.254//
			U2 = u2u1 / U1;
		}

		A2 = sqrt(pow2(A1) + Ga3 * (pow2(U1) - pow2(U2))); // ec.255//

		xx3 = CC2 + Ga3 * U2;
		A1p = xx3 * FRE * pow(b1, -Ga5); // ec.4.256, CUIDADO QUE HAY UN SIGNO MENOS EN EL EXPONENTE//

		return A1p - A1;
	}
};

struct stPerdPresAdL {

	double CC1; // Variable local asociada a la caracteristica incidente del extremo 1 pasada por funcion//
	double CC2; // Variable l.a. a la caracteristica incidente del extremo 2 pasada por funcion//
	double FK; // Variable l.a. al parametro K pasado por funcion//
	double Gam; // Variable l.a. a Gamma pasado por funcion//
	double FRE; // Variable l.a. a la relacion de entropia que se pasa por funcion//
	double ARef; // Variable l.a. a la velocidad del sonido de referencia //
	double ei;
	double ed;

	double U1; // Variable local para almacenar la velocidad del fluido en 1//
	double U2; // Variable local para almacenar la velocidad del fluido en 2//
	double A2; // Variable local para almacenar la velocidad del sonido en 2//
	double A1p; // Variable local para almacenar el valor calculado de A1 segun 4.256//
	double xx3;
	double Ga3;
	double Ga5;

	stPerdPresAdL(const double iCC1, const double iCC2, const double iFK, const double iGam, const double iFRE,
				  const double iARef) :
		CC1(iCC1), CC2(iCC2), FK(iFK), Gam(iGam), FRE(iFRE), ARef(iARef) {
		Ga3 = (Gam - 1) / 2;
		Ga5 = (Gam - 1) / 2 / Gam;
	}

	double operator()(const double A1) {

		U1 = (CC1 - A1) / Ga3; // Calculo de la velocidad  saliente.//
		double b1 = -FK * fabs(U1) / pow2(A1) + 1.;
		b1 += 1e-14; // Resolucion ecuacion de 2 grado y calculo de la velocidad entrante.OJO que he cambiado la formula.Pedro.//
		// Resolucion ecuacion de 2 grado y calculo de la velocidad entrante.//
		double a = Ga3;
		double b = b1 * pow2(A1);
		double c = -(pow2(A1) + Ga3 * pow2(U1)) * pow2(U1);
		double u2u1 = QuadraticEqP(a, b, c);
		if(U1 == 0.) {
			U2 = 0.;
		} else {
			U2 = u2u1 / U1;
		}
		A2 = sqrt(pow2(A1) + Ga3 * (pow2(U1) - pow2(U2)));
		// Calculo de la velocidad del sonido entrante.

		xx3 = CC2 + U2 * Ga3;

		A1p = xx3 * FRE / pow(b1, Ga5);

		return A1p - A1;
	}
};

/* ! Volumetric Compressor */

struct stComprVol {

	/* Definicion de las variables locales (pueden tomar cualquier nombre)
	 y que almacenaran la informacion de las variables pasadas a la funcion
	 desde el programa, LA INFORMACION SE ASOCIARA EN EL CONSTRUCTOR. AQUI
	 SOLO SE ESTAN DEFINIENDO */

	/* PASO POR EL CONSTRUCTOR __cons::ARef y PRef para evitar incluir la libreria globales */

	double AA; // Variable local asociada al nivel de entropia inicial pasado por la funcion//
	double BC; // Variable l.a a la caracteristica incidente inicial pasada por la funcion //
	double Gam; // Variable l.a al gamma pasado por la funcion//
	double A; // Variable l.a a la velocidad del sonido pasada por la funcion//
	double Gasto_calculado; // Variable l.a al massflow calculado por (4.113) y pasado por la funcion//
	double F; // Variable l.a a la seccion del tubo y pasado por funcion//
	double PRef; // Variable l.a a la presion de referencia y que se ha pasado por funcion y asi evitar incluir la libreria globales//
	double ARef; // Variable l.a a la velocidad del sonido de referencia con idem justificacion que PRef//

	double Ga3; // Variable local//
	double Ga4; // Variable local//
	double entropia; // Variable local para la entropia//
	// double Gasto_supuesto;    //Variable local para el massflow supueso//
	double CD; // Variable local//
	double U;

	/* CONSTRUCTOR: Aqui se RECIBEN las VARIABLES PASADAS con un nombre ficticio
	 parecido al de la definicio de las variables locales (por ejemplo precedido de i)
	 EN EL LADO IZQUIERDO (variables recibidas) : EN EL LADO DERECHO (informacion almacenada)
	 y que son necesarias para el calculo del operador (tipo de variable y nombre ficticio) y a continuacion
	 SE ASOCIAN a las variables locales que almacenaran la informacion pasada */

	stComprVol(const double iAA, const double iCC, const double iGam, const double iA, const double iGS, const double iF,
			   const double iPRef, const double iARef) :
		AA(iAA), BC(iCC), Gam(iGam), A(iA), Gasto_calculado(iGS), F(iF), PRef(iPRef), ARef(iARef) {
		Ga3 = (Gam - 1) / 2;
		Ga4 = 2 * Gam / (Gam - 1);
		CD = 1;
	}
	;

	double operator()(const double Vel) {
		double entropia = A * AA / (BC + Ga3 * Vel);
		U = Gasto_calculado * pow(entropia, Ga4) / (CD * Gam * F * __units::BarToPa(PRef) / ARef * pow(A, 1 / Ga3));

		return U - Vel;
	}

};

struct stNewCompressorConditions {
	double Mass;
	double A01;
	double AA1;
	double Section;
	double Lambda;
	double Gam;
	double Ga3;
	double Eff;

	stNewCompressorConditions(const double iMass, const double iCC, const double T01, const double P01, const double iEff,
							  const double iGam, const double R, const double Sec) :
		Mass(iMass), Lambda(iCC), Gam(iGam), Section(Sec), Eff(iEff) {
		A01 = sqrt(Gam * R * T01);
		AA1 = A01 * pow(P01, (1 - Gam) / 2 / Gam);
		Ga3 = (Gam - 1) / 2;
	}

	double operator()(const double Match) {

		double A2 = Lambda / (1 - Ga3 * Match);
		double k = pow2(A2) * (1 + Ga3 * Match * Match) / (A01 * A01) - 1;
		double AA2 = AA1 * sqrt(1 + k) / (k * Eff + 1);
		double Match2 = Mass / (Gam * pow(A2 / pow(AA2, Gam), 1 / Ga3) * A2 * Section);
		return Match - Match2;

	}
};

struct stCharOrigin {
	double W00;
	double W10;
	double W20;
	double W01;
	double W11;
	double W21;
	double G0;
	double G1;
	double dtdx;
	int signo;

	stCharOrigin(const double iW00, const double iW10, const double iW20, const double iW01, const double iW11,
				 const double iW21, const double iG0, const double iG1, const double idtdx, int isigno) :
		W00(iW00), W10(iW10), W20(iW20), W01(iW01), W11(iW11), W21(iW21), G0(iG0), G1(iG1), dtdx(idtdx), signo(isigno) {
	}

	double operator()(const double x) {
		double W0p = Interpola(W00, W01, 1., x);
		double W1p = Interpola(W10, W11, 1., x);
		double W2p = Interpola(W20, W21, 1., x);
		double Gp = Interpola(G0, G1, 1., x);
		double Up = W1p / W0p;
		double Ap = sqrt(Gp * (Gp - 1) * (W2p / W0p - pow2(Up) / 2.));
		return x + signo * (Up - signo * Ap) * dtdx;
	}
};

struct stPathOrigin {
	double W00;
	double W10;
	double W01;
	double W11;
	double dtdx;
	int signo;

	stPathOrigin(const double iW00, const double iW10, const double iW01, const double iW11, const double idtdx,
				 int isigno) :
		W00(iW00), W10(iW10), W01(iW01), W11(iW11), dtdx(idtdx), signo(isigno) {
	}

	double operator()(const double x) {
		double W0p = Interpola(W00, W01, 1., x);
		double W1p = Interpola(W10, W11, 1., x);
		double Up = W1p / W0p;
		return x + signo * Up * dtdx;
	}
};

#endif

