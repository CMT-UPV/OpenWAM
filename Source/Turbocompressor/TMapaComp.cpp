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
#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
//#include <cmath>
#pragma hdrstop

#include "TMapaComp.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TMapaComp::TMapaComp(int i) :
	TCompressorMap() {
	FNumeroCompresor = i;

	FGastoRelComp1 = NULL;
	FGastoBombeo = NULL;
	FRelCompBombeo = NULL;
	FRelComp = NULL;
	FGastoRend = NULL;
	FRend = NULL;
	FSpl = NULL;
	FOrtp = NULL;
	FCoefSplBombeo = NULL;
	FRegimenCurva = NULL;
	FNumCurvasRen = NULL;
	FNumCurvasRenAd = NULL;
	FCoefbSup = NULL;
	FCoefbInf = NULL;
	FCoefcSup = NULL;
	FCoefcInf = NULL;
	FCoefdSup = NULL;
	FCoefdInf = NULL;
	FGastoInt = NULL;
	FRelCompInt = NULL;
	FCoefSplRC = NULL;
	FCoefbX = NULL;
	FCoefcX = NULL;
	FCoefdX = NULL;
	FGastoAdim = NULL;
	FRendAdim = NULL;
	FCoefSplRend = NULL;
// Se supone un rendimiento para la curva de bombeo igual a 0.4
	FRendCurvaBombeo = 0.4;
// Se supone un rendimiento para el punto de massflow maximo de 0.1
	FRendGastoMaximo = 0.1;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TMapaComp::~TMapaComp() {
	if(FGastoRelComp1 != NULL)
		delete[] FGastoRelComp1;
	if(FGastoBombeo != NULL)
		delete[] FGastoBombeo;
	if(FRelCompBombeo != NULL)
		delete[] FRelCompBombeo;
	if(FRelComp != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FRelComp[i];
		}
		delete[] FRelComp;
	}
	if(FGastoRend != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FGastoRend[i];
		}
		delete[] FGastoRend;
	}
	if(FRend != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FRend[i];
		}
		delete[] FRend;
	}
	if(FSpl != NULL)
		delete[] FSpl;
	if(FOrtp != NULL)
		delete[] FOrtp;
	if(FCoefSplBombeo != NULL)
		delete[] FCoefSplBombeo;
	if(FRegimenCurva != NULL)
		delete[] FRegimenCurva;
	if(FNumCurvasRen != NULL)
		delete[] FNumCurvasRen;
	if(FNumCurvasRenAd != NULL)
		delete[] FNumCurvasRenAd;
	if(FCoefbSup != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FCoefbSup[i];
		}
		delete[] FCoefbSup;
	}
	if(FCoefbInf != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FCoefbInf[i];
		}
		delete[] FCoefbInf;
	}
	if(FCoefcSup != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FCoefcSup[i];
		}
		delete[] FCoefcSup;
	}
	if(FCoefcInf != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FCoefcInf[i];
		}
		delete[] FCoefcInf;
	}
	if(FCoefdSup != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FCoefdSup[i];
		}
		delete[] FCoefdSup;
	}
	if(FCoefdInf != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FCoefdInf[i];
		}
		delete[] FCoefdInf;
	}
	delete[] FGastoInt;
	delete[] FRelCompInt;
	delete[] FCoefSplRC;
	delete[] FCoefbX;
	delete[] FCoefcX;
	delete[] FCoefdX;
	if(FGastoAdim != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FGastoAdim[i];
		}
		delete[] FGastoAdim;
	}
	if(FRendAdim != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FRendAdim[i];
		}
		delete[] FRendAdim;
	}
	if(FCoefSplRend != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FCoefSplRend[i];
		}
		delete[] FCoefSplRend;
	}
}

//---------------------------------------------------------------------------
// ---------- LeeMapa -------------------------------------------------------
// Lectura del mapa del compresor desde el fichero .wam                    //
// Incializacion de la variables del mapa                                  //
// Calculo de los coeficientes de la spline que ajusta la linea de bombeo  //
// Calculo de los coeficientes ortogonales que ajustan el rendimiento      //
//---------------------------------------------------------------------------

void TMapaComp::LeeMapa(FILE *fich) {
	double *W = NULL;
	try {
// Datos de referencia
		std::cout << "LECTURA MAPA COMPRESOR" << std::endl;
		std::cout << "______________________" << std::endl;
		fscanf(fich, "%lf %lf ", &FPresionRef, &FTempRef);
		FTempRef = __units::degCToK(FTempRef);
		;
		FPresionRef = __units::BarToPa(FPresionRef);
		fscanf(fich, "%lf %lf %lf ", &FMassMultiplier, &FCRMultiplier, &FEffMultiplier);

		std::cout << "Datos de Referencia:" << std::endl;
		std::cout << "Pressure:     " << FPresionRef << " Pa" << std::endl;
		std::cout << "Temperature: " << FTempRef << " K" << std::endl;

// Rango e incremento de regimenes de giro.
		fscanf(fich, "%lf %lf %lf ", &FRegMin, &FRegMax, &FIncReg);
		FNumCurvasReg = floor(((FRegMax - FRegMin) / FIncReg) + 0.5) + 1;

// Rango e incremento de gastos masicos.
		fscanf(fich, "%lf %lf %lf ", &FGastoMin, &FGastoMax, &FIncGasto);
		FGastoMin *= FMassMultiplier;
		FGastoMax *= FMassMultiplier;
		FIncGasto *= FMassMultiplier;
		FNumPuntosGasto = floor(((FGastoMax - FGastoMin) / FIncGasto) + 0.5) + 1;

// Numero maximo de curvas de rendimiento
		fscanf(fich, "%d ", &FNumCurvasRendMax);

// Dimensionado Vectores del mapa

		FGastoRelComp1 = new double[FNumCurvasReg];
		FGastoBombeo = new double[FNumCurvasReg + 1];
		FRelCompBombeo = new double[FNumCurvasReg + 1];
		FRegimenCurva = new double[FNumCurvasReg];
		FNumCurvasRen = new int[FNumCurvasReg];
		FNumCurvasRenAd = new int[FNumCurvasReg];
		FCoefSplBombeo = new double[FNumCurvasReg + 1];
		FSpl = new stSpline[FNumPuntosGasto + 1];
		FOrtp = new stOrtoPol[FNumCurvasRendMax + 1];
		FRelComp = new double*[FNumCurvasReg];
		for(int i = 0; i < FNumCurvasReg; i++) {
			FRelComp[i] = new double[FNumPuntosGasto];
		}
		FGastoInt = new double[FNumPuntosGasto + 1];
		FRelCompInt = new double[FNumPuntosGasto + 1];
		FCoefSplRC = new double[FNumPuntosGasto + 1];

		FGastoRend = new double*[FNumCurvasReg];
		FRend = new double*[FNumCurvasReg];
		for(int i = 0; i < FNumCurvasReg; i++) {
			FGastoRend[i] = new double[FNumCurvasRendMax];
			FRend[i] = new double[FNumCurvasRendMax];
		}

// Lectura del mapa

		for(int i = 0; i < FNumCurvasReg; i++) {
			FRegimenCurva[i] = FRegMin + FIncReg * (double) i;
			fscanf(fich, "%lf ", &FGastoRelComp1[i]);
			FGastoRelComp1[i] *= FMassMultiplier;

			fscanf(fich, "%lf %lf ", &FGastoBombeo[i], &FRelCompBombeo[i]);
			FGastoBombeo[i] = FGastoBombeo[i] * FMassMultiplier;
			FRelCompBombeo[i] = (FRelCompBombeo[i] - 1) * FCRMultiplier + 1;

			for(int j = 0; j < FNumPuntosGasto; j++) {
				fscanf(fich, "%lf ", &FRelComp[i][j]);
				FRelComp[i][j] = (FRelComp[i][j] - 1) * FCRMultiplier + 1;
			}
			FNumCurvasRen[i] = 0;
			for(int j = 0; j < FNumCurvasRendMax; j++) {
				fscanf(fich, "%lf %lf ", &FGastoRend[i][j], &FRend[i][j]);
				FGastoRend[i][j] = FGastoRend[i][j] * FMassMultiplier;
				FRend[i][j] = FRend[i][j] * FEffMultiplier;
				if(FRend[i][j] > 0.) {
					++FNumCurvasRen[i];
				}
				if(FGastoRend[i][j] > FGastoRelComp1[i]) {
					std::cout << "WARNING: Existen puntos de rendimiento en la curva de regimen: " << FRegimenCurva[i] << std::endl;
					std::cout << "         con valores de massflow mayores del massflow de relacion de compresion 1" << std::endl;
					std::cout << "         -Valor del massflow del punto de rendimiento:    " << FGastoRend[i][j] << std::endl;
					std::cout << "         -Valor del massflow de relacion de compresion 1: " << FGastoRelComp1[i] << std::endl;
					std::cout << "         Revisa el mapa. Esto puede inducir errores importantes de interpolacion" << std::endl;
					std::cout << "         en el compresor numero " << FNumeroCompresor << std::endl << std::endl;
				}
			}
		}
// Obtencion de los coeficientes de la spline que ajusta la curva de bombeo.

//Se desplan todos los valores un lugar para poder incluir el centro de coordenadas massflow=0, relacion de compresion=1

		for(int i = FNumCurvasReg; i > 0; --i) {
			FGastoBombeo[i] = FGastoBombeo[i - 1];
			FRelCompBombeo[i] = FRelCompBombeo[i - 1];
		}
		FGastoBombeo[0] = 0.;
		FRelCompBombeo[0] = 1.;

		Hermite(FNumCurvasReg + 1, FGastoBombeo, FRelCompBombeo, FCoefSplBombeo);

// Obtencion de los coeficientes polinomios ortogonales.
		/*double NumTerms = 0.;

		 W=new double[FNumCurvasRendMax];

		 FCoefbInf=new double*[FNumCurvasReg];
		 FCoefbSup=new double*[FNumCurvasReg];
		 FCoefcInf=new double*[FNumCurvasReg];
		 FCoefcSup=new double*[FNumCurvasReg];
		 FCoefdInf=new double*[FNumCurvasReg];
		 FCoefdSup=new double*[FNumCurvasReg];

		 for(int i=0;i<FNumCurvasReg;i++){
		 FCoefbInf[i]=new double[3];
		 FCoefbSup[i]=new double[3];
		 FCoefcInf[i]=new double[3];
		 FCoefcSup[i]=new double[3];
		 FCoefdInf[i]=new double[3];
		 FCoefdSup[i]=new double[3];
		 }

		 FCoefbX=new double[3];
		 FCoefcX=new double[3];
		 FCoefdX=new double[3];

		 for(int i=0;i<FNumCurvasReg-1;i++){
		 NumTerms=3;
		 if(FNumCurvasRen[i]<NumTerms){
		 NumTerms=FNumCurvasRen[i];
		 }
		 for(int j=0;j<FNumCurvasRen[i];++j){
		 W[j]=1.;
		 }

		 PolOrtogonal(NumTerms,FNumCurvasRen[i],FGastoRend[i],FRend[i],W,FCoefbInf[i],FCoefcInf[i],
		 FCoefdInf[i]);

		 if(FNumCurvasRen[i+1]<NumTerms){
		 NumTerms=FNumCurvasRen[i+1];
		 }
		 for(int j=0;j<FNumCurvasRen[i+1];++j){
		 W[j]=1.;
		 }

		 PolOrtogonal(NumTerms,FNumCurvasRen[i+1],FGastoRend[i+1],FRend[i+1],W,FCoefbSup[i+1],FCoefcSup[i+1],
		 FCoefdSup[i+1]);
		 }   */
// Obtencion de los splines de las curvas de rendimiento frente a massflow adimensionalizado
		FGastoAdim = new double*[FNumCurvasReg];
		FRendAdim = new double*[FNumCurvasReg];
		FCoefSplRend = new double*[FNumCurvasReg];
		bool RendEnBombeo = false;
		for(int i = 0; i < FNumCurvasReg; i++) {
			if(FGastoRend[i][0] < FGastoBombeo[i + 1] * 1.001 && FGastoRend[i][0] > FGastoBombeo[i + 1] * 0.999) {
				FNumCurvasRenAd[i] = FNumCurvasRen[i] + 1;
				RendEnBombeo = true;
				std::cout << "INFO: El punto de bombeo de la curva de " << FRegimenCurva[i] << " rpm tiene rendimiento" << std::endl;
			} else {
				FNumCurvasRenAd[i] = FNumCurvasRen[i] + 2;
				RendEnBombeo = false;
				std::cout << "INFO: El punto de bombeo de la curva de " << FRegimenCurva[i] << " rpm no tiene rendimiento" << std::endl;
			}
			FGastoAdim[i] = new double[FNumCurvasRenAd[i]];
			FRendAdim[i] = new double[FNumCurvasRenAd[i]];
			FCoefSplRend[i] = new double[FNumCurvasRenAd[i]];
			//Punto inicial de la curva. Bombeo.
			FGastoAdim[i][0] = 0.;
			if(RendEnBombeo) {
				FRendAdim[i][0] = FRend[i][0];
			} else {
				FRendAdim[i][0] = FRendCurvaBombeo;
			}
			//Punto final de la curva. Relacion de compresion 1.
			FGastoAdim[i][FNumCurvasRenAd[i] - 1] = 1.;
			FRendAdim[i][FNumCurvasRenAd[i] - 1] = FRendGastoMaximo;
			for(int j = 1; j < FNumCurvasRenAd[i] - 1; j++) {
				if(RendEnBombeo) {
					FGastoAdim[i][j] = (FGastoRend[i][j] - FGastoBombeo[i + 1]) / (FGastoRelComp1[i] - FGastoBombeo[i + 1]);
					FRendAdim[i][j] = FRend[i][j];
				} else {
					FGastoAdim[i][j] = (FGastoRend[i][j - 1] - FGastoBombeo[i + 1]) / (FGastoRelComp1[i] - FGastoBombeo[i + 1]);
					FRendAdim[i][j] = FRend[i][j - 1];
				}
				if(FGastoAdim[i][j] <= FGastoAdim[i][j - 1]) {
					std::cout << "WARNING: La tabla Massflow-Rendimiento en el regimen de " << FRegimenCurva[i] << " rpm esta desordenada"
							  << std::endl;
					std::cout << "         Si se continua con este mapa pueden aparecer errores en la interpolacion" << std::endl;
					std::cout << "         Ordene los datos de forma creciente en massflow" << std::endl;
				}
			}
			Hermite(FNumCurvasRenAd[i], FGastoAdim[i], FRendAdim[i], FCoefSplRend[i]);
		}
//ImprimeMapa();
		/*if(W!=NULL) delete[] W;*/
	} catch(exception &N) {
		std::cout << "ERROR: LeeMapa en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		if(W != NULL)
			delete[] W;
		throw Exception("ERROR: LeeMapa en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

//---------------------------------------------------------------------------
//--------- Spline ----------------------------------------------------------
// Obtiene los diferentes coeficientes de una curva spline que ajusta los  //
// pares de puntos (x,y) que tienen un numero n de componentes             //
//---------------------------------------------------------------------------

void TMapaComp::Spline(int n, double *x, double *y, double *sol) {
	try {
		double Espaciado = 0.;
//Calculo de diferencias
		for(int i = 1; i < n; ++i) {
			FSpl[i].h = x[i] - x[i - 1];
			FSpl[i].dif = y[i] - y[i - 1];
			if(FSpl[i].h <= 0.) {
				std::cout << "ERROR: Error al crear la spline" << std::endl;
				std::cout << "       Los valores en X no estan ordenados o existen 2 puntos situados en el mismo X" << std::endl <<
						  std::endl;
				throw Exception("ERROR: Error al crear la spline");
			}
		}

//Elementos matriz,coeficiente y terminos independientes
//Solo se almacenan elementos diagonales
		for(int i = 1; i < n - 1; ++i) {
			Espaciado = FSpl[i + 1].h / FSpl[i].h;
			if(Espaciado < 0.1 || Espaciado > 10.) {
				std::cout << "WARNING: Deberias utilizar una distribucion mas uniforme entre los valores de X" << std::endl;
				std::cout << "         utilizados para ajustar la spline y asi evitar problemas" << std::endl << std::endl;
			}
			FSpl[i].d = 2 * (FSpl[i + 1].h + FSpl[i].h);
			FSpl[i].d1 = FSpl[i + 1].h;
			FSpl[i].b = (FSpl[i + 1].dif / FSpl[i + 1].h - FSpl[i].dif / FSpl[i].h) * 6.;
		}
//Descomposicion de Cholesky
		FSpl[1].ud = sqrt(FSpl[1].d);
		for(int i = 2; i < n - 1; i++) {
			FSpl[i - 1].ud1 = FSpl[i - 1].d1 / FSpl[i - 1].ud;
			FSpl[i].ud = sqrt(FSpl[i].d - FSpl[i - 1].ud1 * FSpl[i - 1].ud1);
		}
//Sustitucion directa
		FSpl[1].yp = FSpl[1].b / FSpl[1].ud;
		for(int i = 2; i < n - 1; ++i) {
			FSpl[i].yp = (FSpl[i].b - FSpl[i - 1].ud1 * FSpl[i - 1].yp) / FSpl[i].ud;
		}
//Sustitucion inversa
		sol[0] = 0.;
		if(n >= 2) {
			sol[n - 1] = 0;
			if(n >= 3) {
				sol[n - 2] = FSpl[n - 2].yp / FSpl[n - 2].ud;
				if(n >= 4) {
					for(int i = n - 3; i >= 1; --i) {
						sol[i] = (FSpl[i].yp - FSpl[i].ud1 * sol[i + 1]) / FSpl[i].ud;
					}
				}
			}
		}

	} catch(exception &N) {
		std::cout << "ERROR: Spline en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: Spline en el compresor: "/* +std::to_string(FNumeroCompresor)+N.what()*/);
	}
}

//---------------------------------------------------------------------------
//------ EvaluaSpline -------------------------------------------------------
// A partir de una spline con coeficientes 'sol' que pasa por los n puntos //
// (x,y) se evalua su valor para x=punto                                   //
//---------------------------------------------------------------------------

double TMapaComp::EvaluaSpline(double punto, int n, double *x, double *y, double *sol) {
	try {
		int k = 0;
//Determinacion del indice para evaluacion spline
		while(x[k] < punto && k < n - 1) {
			++k;
		}
		k = k - 1;
		if(k < 0)
			k = 0;
		if(k >= n - 1)
			k = n - 2;
		double h = x[k + 1] - x[k];
		double h2 = h * h;
		double dx1 = x[k + 1] - punto;
		double dx2 = punto - x[k];
		double dx13 = pow3(dx1);
		double dx23 = pow3(dx2);

		double ret_val = (sol[k] * dx13 + sol[k + 1] * dx23 + (6 * y[k + 1] - h2 * sol[k + 1]) * dx2 +
						  (6 * y[k] - h2 * sol[k]) * dx1) / (6 * h);

		return ret_val;

	} catch(exception &N) {
		std::cout << "ERROR: EvaluaSpline en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: EvaluaSpline en el compresor: "/* +std::to_string(FNumeroCompresor)+N.what()*/);
	}
}

//---------------------------------------------------------------------------
//         ---------- EvaluaRCSplines -----------                          //
// Evalua la relacion de compresion de la curva interpolada para un        //
// determinado 'Massflow'                                                     //
//---------------------------------------------------------------------------

double TMapaComp::EvaluaRCSplines(double Massflow) {
	try {
		double ret_val = 0.;
		if(Massflow < FGastoInt[0]) {
			ret_val = FRelCompInt[0];
		} else {
			int k = 0;
			int n = FNumPuntos;
			//Determinacion del indice para evaluacion spline
			while(FGastoInt[k] < Massflow && k < n) {
				++k;
			}
			k = k - 1;
			if(k < 0)
				k = 0;
			if(k >= n)
				k = n - 1;
			double h = FGastoInt[k + 1] - FGastoInt[k];
			double h2 = h * h;
			double dx1 = FGastoInt[k + 1] - Massflow;
			double dx2 = Massflow - FGastoInt[k];
			double dx13 = pow3(dx1);
			double dx23 = pow3(dx2);

			ret_val = (FCoefSplRC[k] * dx13 + FCoefSplRC[k + 1] * dx23 + (6 * FRelCompInt[k + 1] - h2 * FCoefSplRC[k + 1]) * dx2 +
					   (6 * FRelCompInt[k] - h2 * FCoefSplRC[k]) * dx1) / (6 * h);

			if(ret_val < 1.)
				ret_val = 1.;
		}

		return ret_val;

	} catch(exception &N) {
		std::cout << "ERROR: EvaluaRCSpline en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: EvaluaRCSpline en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

double TMapaComp::EvaluaRCHermite(double Massflow) {
	try {
		double ret_val = 0., t2 = 0., t3 = 0., t = 0., h00 = 0., h10 = 0., h01 = 0., h11 = 0., h = 0.;

		if(Massflow < FGastoInt[0]) {
			ret_val = FRelCompInt[0];
		} else if(Massflow >= FGastoInt[FNumPuntos]) {
			ret_val = 1.;
		} else {
			int k = 0;
			int n = FNumPuntos;
			//Determinacion del indice para evaluacion spline
			while(FGastoInt[k] < Massflow && k < n) {
				++k;
			}
			h = (FGastoInt[k] - FGastoInt[k - 1]);
			t = (Massflow - FGastoInt[k - 1]) / h;
			t2 = t * t;
			t3 = t2 * t;
			h00 = 2 * t3 - 3 * t2 + 1;
			h10 = t3 - 2 * t2 + t;
			h01 = -2 * t3 + 3 * t2;
			h11 = t3 - t2;
			ret_val = h00 * FRelCompInt[k - 1] + h * h10 * FCoefSplRC[k - 1] + h01 * FRelCompInt[k] + h * h11 * FCoefSplRC[k];
		}
		return ret_val;

	} catch(exception &N) {
		std::cout << "ERROR: EvaluaRCSpline en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: EvaluaRCSpline en el compresor: " + std::to_string(FNumeroCompresor) + N.what());
	}
}

//---------------------------------------------------------------------------
//-------- PolOrtogonal -----------------------------------------------------
// Obtiene los coeficiente 'b', 'c' y 'd' del polinomio ortogonal utilizado//
// para ajustar el rendimiento de compresor donde 'nterms' es el numero de //
// terminos que tienen los coeficientes, 'npoint' es el numero de puntos de//
// rendimiento que tiene una curva determinada 'ma' representa el massflow    //
//'rd' el rendimiento para dicho massflow y 'w' es un vector cuyas componentes//
// valen 1  Ajuste por minimos cuadrados                                   //
//---------------------------------------------------------------------------

void TMapaComp::PolOrtogonal(int nterms, int npoint, double *ma, double *rd, double *w, double *b, double *c,
							 double *d) {
	double p = 0;
	try {
		for(int j = 0; j < nterms; ++j) {
			b[j] = 0.;
			d[j] = 0.;
			FOrtp[j].s = 0.;
		}
		c[0] = 0.;
		for(int i = 0; i < npoint; ++i) {
			d[0] += rd[i] * w[i];
			b[0] += ma[i] * w[i];
			FOrtp[0].s += w[i];
		}
		d[0] /= FOrtp[0].s;
		for(int i = 0; i < npoint; ++i) {
			FOrtp[i].error = rd[i] - d[0];
		}
		b[0] /= FOrtp[0].s;
		for(int i = 0; i < npoint; ++i) {
			FOrtp[i].pjm1 = 1.;
			FOrtp[i].pj = ma[i] - b[0];
		}
		int j = 0;
		while(j < nterms - 1) {
			++j;
			for(int i = 0; i < npoint; ++i) {
				p = FOrtp[i].pj * w[i];
				d[j] += FOrtp[i].error * p;
				p *= FOrtp[i].pj;
				b[j] += ma[i] * p;
				FOrtp[j].s += p;
			}
			d[j] /= FOrtp[j].s;
			for(int i = 0; i < npoint; ++i) {
				FOrtp[i].error -= d[j] * FOrtp[i].pj;
			}
			if(j < nterms) {
				b[j] /= FOrtp[j].s;
				c[j] = FOrtp[j].s / FOrtp[j - 1].s;
				for(int i = 0; i < npoint; ++i) {
					p = FOrtp[i].pj;
					FOrtp[i].pj = (ma[i] - b[j]) * FOrtp[i].pj - c[j] * FOrtp[i].pjm1;
					FOrtp[i].pjm1 = p;
				}
			}
		}
	} catch(exception &N) {
		std::cout << "ERROR: PolOrtogonal en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: PolOrtogonal en el compresor: "/* +std::to_string(FNumeroCompresor)+N.what()*/);
	}
}

//---------------------------------------------------------------------------
//--------InterpolaMapa -----------------------------------------------------
// Ajusta los coeficientes de una spline que representa la curva de regimen//
// 'rtc' a temperatura ambiente 'AmbientTemperature' e interpola los coeficiente del     //
// polinomio ortogonal que ajusta el rendimiento                           //
//---------------------------------------------------------------------------

void TMapaComp::InterpolaMapa(double rtc, double AmbientTemperature) {

	double DeltaN = 0.;
	int Cent = 0;

	try {

// Se adapta el regimen de giro a interpolar al mapa compresor
		FRegComp = rtc * sqrt(FTempRef / AmbientTemperature);

// Se halla el regimen de giro inferior al que se quiere interpolar

		FCurvInf = 0;
		for(int i = 0; i < FNumCurvasReg - 1; ++i) {
			if(FRegComp >= FRegimenCurva[i]) {
				FCurvInf = i;
			}
		}
		DeltaN = (FRegComp - FRegimenCurva[FCurvInf]) / FIncReg;

// Se interpola linealmente el massflow de bombeo y el de RC = 1

		if(FRegComp > FRegMax) {
			FGastoBombeoX = FGastoBombeo[FCurvInf + 1] + (FGastoBombeo[FCurvInf + 1] - FGastoBombeo[FCurvInf]) / FIncReg *
							(FRegComp - FRegMax);
		} else if(FRegComp < FRegMin) {
			FGastoBombeoX = FGastoBombeo[FCurvInf + 1] - (FGastoBombeo[FCurvInf + 2] - FGastoBombeo[FCurvInf + 1]) / FIncReg *
							(FRegMin - FRegComp);
		} else {
			FGastoBombeoX = FGastoBombeo[FCurvInf + 1] + (FGastoBombeo[FCurvInf + 2] - FGastoBombeo[FCurvInf + 1]) * DeltaN;
		}
		if(FGastoBombeoX < 0)
			FGastoBombeoX = 0.;

		FRelCompBombeoX = EvaluaHermite(FGastoBombeoX, FNumCurvasReg + 1, FGastoBombeo, FRelCompBombeo, FCoefSplBombeo);

		if(FRegComp > FRegMax) {
			FGastoRelComp1X = FGastoRelComp1[FCurvInf] + (FGastoRelComp1[FCurvInf] - FGastoRelComp1[FCurvInf - 1]) / FIncReg *
							  (FRegComp - FRegMax);
		} else if(FRegComp < FRegMin) {
			FGastoRelComp1X = FGastoRelComp1[FCurvInf] - (FGastoRelComp1[FCurvInf + 1] - FGastoRelComp1[FCurvInf]) / FIncReg *
							  (FRegMin - FRegComp);
		} else {
			FGastoRelComp1X = FGastoRelComp1[FCurvInf] + (FGastoRelComp1[FCurvInf + 1] - FGastoRelComp1[FCurvInf]) * DeltaN;
		}

// Calculo de la curva interpolada Relacion de compresion/Masa corregida

// Se haya el ultimo massflow del que se tiene informacion para el regimen de giro inferior

		if(FRegComp < FRegMin) {
			FNumPuntos = floor(FGastoRelComp1X / FIncGasto) + 1;
		} else {
			int j = 0;
			while(FRelComp[FCurvInf][j] > 0. && j < FNumPuntosGasto - 1) {
				++j;
			}
			FNumPuntos = j;
		}

// Se interpola linealmente la relacion de compresion entre el regimen de giro inferior
// y superior excepto para el ultimo punto.

		if(FRegComp < FRegMin) {
			for(int j = 0; j < FNumPuntos; ++j) {
				FGastoInt[j] = FGastoMin + FIncGasto * j;
				FRelCompInt[j] = FRelComp[FCurvInf][j] - (FRelComp[FCurvInf + 1][j] - FRelComp[FCurvInf][j]) / FIncReg *
								 (FRegMin - FRegComp);
				if(FRelCompInt[j] < 1.)
					FRelCompInt[j] = 1.;
			}
		} else {
			for(int j = 0; j < FNumPuntos; ++j) {
				FGastoInt[j] = FGastoMin + FIncGasto * j;
				FRelCompInt[j] = FRelComp[FCurvInf][j] + (FRelComp[FCurvInf + 1][j] - FRelComp[FCurvInf][j]) * DeltaN;
			}
		}
		FGastoInt[FNumPuntos] = FGastoRelComp1X;
		FRelCompInt[FNumPuntos] = 1.;

		FCurrentPresMAX = FRelCompInt[0];
		for(int j = 1; j < FNumPuntos; ++j) {
			if(FRelCompInt[j] > FCurrentPresMAX)
				FCurrentPresMAX = FRelCompInt[j];
		}

// Se obtienen los coeficientes que ajusta la spline de la curva interpolada.

		Hermite(FNumPuntos, FGastoInt, FRelCompInt, FCoefSplRC);

// Se interpolan linealmente los coeficientes de cada polinomio ortogonal para el
// regimen de giro buscado
		/*
		 FNumTerms=3;
		 Cent=2;
		 if(FNumCurvasRen[FCurvInf]<FNumTerms){
		 FNumTerms=FNumCurvasRen[FCurvInf];
		 --Cent;
		 }
		 if(FNumCurvasRen[FCurvInf+1]<FNumTerms){
		 FNumTerms=FNumCurvasRen[FCurvInf+1];
		 --Cent;
		 }
		 if(Cent>FNumTerms) FNumTerms=3;

		 for(int i=0;i<FNumTerms;++i){
		 if(FRegComp>FRegMax){
		 FCoefbX[i]=FCoefbSup[FCurvInf+1][i]+(FCoefbSup[FCurvInf+1][i]-FCoefbInf[FCurvInf][i])/FIncReg*(FRegComp-FRegMax);
		 FCoefcX[i]=FCoefcSup[FCurvInf+1][i]+(FCoefcSup[FCurvInf+1][i]-FCoefcInf[FCurvInf][i])/FIncReg*(FRegComp-FRegMax);
		 FCoefdX[i]=FCoefdSup[FCurvInf+1][i]+(FCoefdSup[FCurvInf+1][i]-FCoefdInf[FCurvInf][i])/FIncReg*(FRegComp-FRegMax);
		 }else if(FRegComp<FRegMin){
		 FCoefbX[i]=FCoefbInf[FCurvInf][i]-(FCoefbSup[FCurvInf+1][i]-FCoefbInf[FCurvInf][i])/FIncReg*(FRegMin-FRegComp);
		 FCoefcX[i]=FCoefbInf[FCurvInf][i]-(FCoefcSup[FCurvInf+1][i]-FCoefcInf[FCurvInf][i])/FIncReg*(FRegMin-FRegComp);
		 FCoefdX[i]=FCoefbInf[FCurvInf][i]-(FCoefdSup[FCurvInf+1][i]-FCoefdInf[FCurvInf][i])/FIncReg*(FRegMin-FRegComp);
		 }else{
		 FCoefbX[i]=FCoefbInf[FCurvInf][i]+(FCoefbSup[FCurvInf+1][i]-FCoefbInf[FCurvInf][i])*DeltaN;
		 FCoefcX[i]=FCoefbInf[FCurvInf][i]+(FCoefcSup[FCurvInf+1][i]-FCoefcInf[FCurvInf][i])*DeltaN;
		 FCoefdX[i]=FCoefbInf[FCurvInf][i]+(FCoefdSup[FCurvInf+1][i]-FCoefdInf[FCurvInf][i])*DeltaN;
		 }
		 } */
	} catch(exception &N) {
		std::cout << "ERROR: InterpolaMapa en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: InterpolaMapa en el compresor: "/* +std::to_string(FNumeroCompresor)+N.what()*/);
	}
}

//---------------------------------------------------------------------------
// ---------EvaluaRendimiento -----------------------------------------------
// Evalua el rendimiento a partir de los coeficientes del polinomio        //
// ortogonal de la curva de funcionamiento para un valor de                //
// masa de aire 'MasaAire'                                                 //
//---------------------------------------------------------------------------

double TMapaComp::EvaluaRendimiento(double MasaAire) {
	int k = 0;
	double prev2 = 0.;
	double prev = 0.;
	double ret_val = 0.;
	try {
		k = FNumTerms - 1;
		ret_val = FCoefdX[k];
		prev = 0.;
		--k;
		while(k >= 0) {
			prev2 = prev;
			prev = ret_val;
			ret_val = FCoefdX[k] + (MasaAire - FCoefbX[k]) * prev - FCoefcX[k + 1] * prev2;
			--k;
		}
		return ret_val;
	} catch(exception &N) {
		std::cout << "ERROR: EvaluaRendimiento en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: EvaluaRendimiento en el compresor: "/* +std::to_string(FNumeroCompresor)+N.what()*/);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TMapaComp::EvaluaRendSplines(double MasaAire) {
	double AireAdim = 0.;
	double ret_val = 0.;
	double RendInf = 0.;
	double RendSup = 0.;
	double DeltaN = 0.;
	try {
		if(MasaAire < FGastoBombeoX) {
			//En Bombeo se toma como rendimiento el del ultimo punto.
			DeltaN = (FRegComp - FRegimenCurva[FCurvInf]) / FIncReg;

			RendInf = FRendAdim[FCurvInf][0];
			RendSup = FRendAdim[FCurvInf + 1][0];

			if(FRegComp > FRegMax) {
				ret_val = RendSup + (RendSup - RendInf) / FIncReg * (FRegComp - FRegMax);
			} else if(FRegComp < FRegMin) {
				ret_val = RendInf - (RendSup - RendInf) / FIncReg * (FRegMin - FRegComp);
			} else {
				ret_val = RendInf + (RendSup - RendInf) * DeltaN;
			}
		} else if(MasaAire > FGastoRelComp1X) {
			ret_val = FRendGastoMaximo;
		} else {
			AireAdim = (MasaAire - FGastoBombeoX) / (FGastoRelComp1X - FGastoBombeoX);

			RendInf = EvaluaHermite(AireAdim, FNumCurvasRenAd[FCurvInf], FGastoAdim[FCurvInf], FRendAdim[FCurvInf],
									FCoefSplRend[FCurvInf]);

			RendSup = EvaluaHermite(AireAdim, FNumCurvasRenAd[FCurvInf + 1], FGastoAdim[FCurvInf + 1], FRendAdim[FCurvInf + 1],
									FCoefSplRend[FCurvInf + 1]);

			DeltaN = (FRegComp - FRegimenCurva[FCurvInf]) / FIncReg;

			if(FRegComp > FRegMax) {
				ret_val = RendSup + (RendSup - RendInf) / FIncReg * (FRegComp - FRegMax);
			} else if(FRegComp < FRegMin) {
				ret_val = RendInf - (RendSup - RendInf) / FIncReg * (FRegMin - FRegComp);
			} else {
				ret_val = RendInf + (RendSup - RendInf) * DeltaN;
			}
		}
		return ret_val;
	} catch(exception &N) {
		std::cout << "ERROR: EvaluaRendSplines en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: EvaluaRendSplines en el compresor: "/* +std::to_string(FNumeroCompresor)+N.what()*/);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TMapaComp::GetRelCompInt(int i) {
	try {
		return FRelCompInt[i];
	} catch(exception &N) {
		std::cout << "ERROR: GetRelCompInt en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: GetRelCompInt en el compresor: "/* +std::to_string(FNumeroCompresor)+N.what()*/);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TMapaComp::GetGastoInt(int i) {
	try {
		return FGastoInt[i];
	} catch(exception &N) {
		std::cout << "ERROR: GetGastoInt en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: GetGastoInt en el compresor: "/* +std::to_string(FNumeroCompresor)+N.what()*/);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TMapaComp::BuscaRegimen(double RC, double Massflow, double AmbientTemperature) {
	try {
		double RCSup = 0.;
		double RCInf = 0.;
		double ret_val = 0., val1 = 0., val2 = 0.;
		double reg = 0.;
		int i = 0, sup, inf;
		while(RC > RCSup && i < FNumCurvasReg) {
			reg = FRegimenCurva[i] * AmbientTemperature / FTempRef;
			InterpolaMapa(reg, AmbientTemperature);

			if(Massflow < FGastoRelComp1[i]) {
				RCSup = EvaluaRCSplines(Massflow);
			} else {
				RCSup = 1.;
			}
			sup = i;
			++i;
		}
		if(RC > RCSup) {
			std::cout << "WARNING: El Punto se sale por arriba del mapa " << std::endl;
			//sup=i-1;
		}
		if(sup > 0) {
			inf = sup - 1;
			reg = FRegimenCurva[inf] * AmbientTemperature / FTempRef;
			InterpolaMapa(reg, AmbientTemperature);

			if(Massflow < FGastoRelComp1[inf]) {
				RCInf = EvaluaRCSplines(Massflow);
			} else {
				std::cout << "WARNING: El siguiente punto puede estar mal calculado:" << std::endl;
				std::cout << "         Relacionde compresion: " << RC << std::endl;
				std::cout << "         Massflow masico:          " << Massflow << std::endl;
				std::cout << "         Deberias aumentar el numero de curvas del mapa" << std::endl << std::endl;
				RCInf = 1.;
			}
			ret_val = (RC - RCInf) / (RCSup - RCInf) * (FRegimenCurva[sup] - FRegimenCurva[inf]) + FRegimenCurva[inf];
		} else {
			inf = sup;
			sup = inf + 1;
			RCInf = RCSup;
			InterpolaMapa(FRegimenCurva[sup], __units::degCToK(AmbientTemperature));
			RCSup = EvaluaRCSplines(Massflow);
			val1 = (RC - RCInf) / (RCSup - RCInf) * (FRegimenCurva[sup] - FRegimenCurva[inf]) + FRegimenCurva[inf];

			val2 = (RC - 1) / (RCInf - 1) * (FRegimenCurva[inf]);
			ret_val = (val1 + val2) / 2;
			std::cout << "WARNING: El siguiente punto puede estar mal calculado:" << std::endl;
			std::cout << "         Relacionde compresion: " << RC << std::endl;
			std::cout << "         Massflow masico:          " << Massflow << std::endl;
			std::cout << "         Deberias incluir curvas con menor regimen de giro" << std::endl << std::endl;
		}
		return ret_val;
	} catch(exception &N) {
		std::cout << "ERROR: BuscaRegimen en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: BuscaRegimen en el compresor: "/* +std::to_string(FNumeroCompresor)+N.what()*/);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TMapaComp::ImprimeMapa() {
	std::cout << "Printing compresso map .";
	FILE *fich;
	fich = fopen("MapaSimple.txt", "w");
	double inc = 0.;
	double *massflow;
	double **rc;
	massflow = new double[200];
	rc = new double*[200];
	for(int j = 0; j < 200; j++) {
		rc[j] = new double[FNumCurvasReg];
	}
	inc = (FGastoMax - FGastoMin) / 200;
	for(int j = 0; j < 200; j++) {
		massflow[j] = FGastoMin + inc * (double) j;
	}
	for(int i = 0; i < FNumCurvasReg; i++) {
		std::cout << ".";
		InterpolaMapa(FRegMin + FIncReg * (double) i, FTempRef);
		for(int j = 0; j < 200; j++) {
			rc[j][i] = EvaluaRCSplines(massflow[j]);
		}
	}

	for(int j = 0; j < 200; j++) {
		if(j < FNumCurvasReg) {
			fprintf(fich, "%lf\t%lf\t", FGastoBombeo[j], FRelCompBombeo[j]);
		} else {
			fprintf(fich, "\t\t");
		}
		fprintf(fich, "%lf\t", massflow[j]);
		for(int i = 0; i < FNumCurvasReg; i++) {
			fprintf(fich, "%lf\t", rc[j][i]);
		}
		fprintf(fich, "\n");
	}
	delete[] massflow;
	for(int j = 0; j < 200; j++) {
		delete[] rc[j];
	}
	fclose(fich);
	std::cout << "Finished" << std::endl;
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
