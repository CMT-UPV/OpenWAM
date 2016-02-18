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

#include "TMapaComp2Tub.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TMapaComp2Tub::TMapaComp2Tub(int i) :
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
	FCoefSplRC = NULL;
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
	FCoefbX = NULL;
	FCoefcX = NULL;
	FCoefdX = NULL;
	FGastoAdim = NULL;
	FRendAdim = NULL;
	FCoefSplRend = NULL;
// Se supone un rendimiento para la curva de bombeo igual a 0.4
	FRendCurvaBombeo = 0.4;
// Se supone un rendimiento para el punto de massflow maximo de 0.1
	FRendGastoMaximo = 0.4;
	Fnegmas = NULL;
	Fnegrc = NULL;
	Fsolneg = NULL;
	FRelCompNuevo = NULL;
	FGastoCompNuevo = NULL;

	FCorrect = true;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TMapaComp2Tub::~TMapaComp2Tub() {
	if(FGastoRelComp1 != NULL)
		delete[] FGastoRelComp1;
	FGastoRelComp1 = NULL;
	if(FGastoBombeo != NULL)
		delete[] FGastoBombeo;
	FGastoBombeo = NULL;
	if(FRelCompBombeo != NULL)
		delete[] FRelCompBombeo;
	FRelCompBombeo = NULL;
	if(FRelComp != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FRelComp[i];
		}
		delete[] FRelComp;
	}
	FRelComp = NULL;
	if(FGastoRend != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FGastoRend[i];
		}
		delete[] FGastoRend;
	}
	FGastoRend = NULL;
	if(FRend != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FRend[i];
		}
		delete[] FRend;
	}
	FRend = NULL;
	if(FSpl != NULL)
		delete[] FSpl;
	FSpl = NULL;
	if(FOrtp != NULL)
		delete[] FOrtp;
	FOrtp = NULL;
	if(FCoefSplBombeo != NULL)
		delete[] FCoefSplBombeo;
	FCoefSplBombeo = NULL;
	if(FCoefSplRC != NULL)
		delete[] FCoefSplRC;
	FCoefSplRC = NULL;
	if(FRegimenCurva != NULL)
		delete[] FRegimenCurva;
	FRegimenCurva = NULL;
	if(FNumCurvasRen != NULL)
		delete[] FNumCurvasRen;
	FNumCurvasRen = NULL;
	if(FNumCurvasRenAd != NULL)
		delete[] FNumCurvasRenAd;
	FNumCurvasRenAd = NULL;
	if(FCoefbSup != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FCoefbSup[i];
		}
		delete[] FCoefbSup;
	}
	FCoefbSup = NULL;
	if(FCoefbInf != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FCoefbInf[i];
		}
		delete[] FCoefbInf;
	}
	FCoefbInf = NULL;
	if(FCoefcSup != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FCoefcSup[i];
		}
		delete[] FCoefcSup;
	}
	FCoefcSup = NULL;
	if(FCoefcInf != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FCoefcInf[i];
		}
		delete[] FCoefcInf;
	}
	FCoefcInf = NULL;
	if(FCoefdSup != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FCoefdSup[i];
		}
		delete[] FCoefdSup;
	}
	FCoefdSup = NULL;
	if(FCoefdInf != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FCoefdInf[i];
		}
		delete[] FCoefdInf;
	}
	FCoefdInf = NULL;
	delete[] FGastoInt;
	FGastoInt = NULL;
	delete[] FRelCompInt;
	FRelCompInt = NULL;
	delete[] FCoefbX;
	FCoefbX = NULL;
	delete[] FCoefcX;
	FCoefcX = NULL;
	delete[] FCoefdX;
	FCoefdX = NULL;
	if(FGastoAdim != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FGastoAdim[i];
		}
		delete[] FGastoAdim;
	}
	FGastoAdim = NULL;
	if(FRendAdim != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FRendAdim[i];
		}
		delete[] FRendAdim;
	}
	FRendAdim = NULL;
	if(FCoefSplRend != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FCoefSplRend[i];
		}
		delete[] FCoefSplRend;
	}
	FCoefSplRend = NULL;
	if(Fnegmas != NULL)
		delete[] Fnegmas;
	Fnegmas = NULL;
	if(Fnegrc != NULL)
		delete[] Fnegrc;
	Fnegrc = NULL;
	if(Fsolneg != NULL)
		delete[] Fsolneg;
	Fsolneg = NULL;
	if(FRelCompNuevo != NULL) {
		for(int i = 0; i < FNumCurvasReg; i++) {
			delete[] FRelCompNuevo[i];
		}
		delete[] FRelCompNuevo;
	}
	FRelCompNuevo = NULL;
	if(FGastoCompNuevo != NULL)
		delete[] FGastoCompNuevo;
	FGastoCompNuevo = NULL;
}

//---------------------------------------------------------------------------
// ---------- LeeMapa -------------------------------------------------------
// Lectura del mapa del compresor desde el fichero .wam                    //
// Incializacion de la variables del mapa                                  //
// Calculo de los coeficientes de la spline que ajusta la linea de bombeo  //
// Calculo de los coeficientes ortogonales que ajustan el rendimiento      //
//---------------------------------------------------------------------------

void TMapaComp2Tub::LeeMapa(FILE *fich) {
	double *W = NULL;
	try {
// Datos de referencia
		std::cout << "LECTURA MAPA COMPRESOR" << std::endl;
		std::cout << "______________________" << std::endl;
		fscanf(fich, "%lf %lf ", &FPresionRef, &FTempRef);
		FTempRef = __units::degCToK(FTempRef);
		FPresionRef = __units::BarToPa(FPresionRef);

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
		FOrtp = new stOrtoPol2Tub[FNumCurvasRendMax + 1];
		FRelComp = new double*[FNumCurvasReg];
		for(int i = 0; i < FNumCurvasReg; i++) {
			FRelComp[i] = new double[FNumPuntosGasto];
		}

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
			FGastoBombeo[i] *= FMassMultiplier;
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

		Cambio_Mapa(FRadioTip, FRadioHub, FRadioRodete);

// Obtencion de los coeficientes de la spline que ajusta la curva de bombeo.

//Se desplan todos los valores un lugar para poder incluir el centro de coordenadas massflow=0, relacion de compresion=1

		for(int i = FNumCurvasReg; i > 0; --i) {
			FGastoBombeo[i] = FGastoBombeo[i - 1];
			FRelCompBombeo[i] = FRelCompBombeo[i - 1];
		}
		FGastoBombeo[0] = 0.;
		FRelCompBombeo[0] = 1.;

		Hermite(FNumCurvasReg + 1, FGastoBombeo, FRelCompBombeo, FCoefSplBombeo);

// Obtencion de los splines de las curvas de rendimiento frente a massflow adimensionalizado
		FGastoAdim = new double*[FNumCurvasReg];
		FRendAdim = new double*[FNumCurvasReg];
		FCoefSplRend = new double*[FNumCurvasReg];
		bool RendEnBombeo = false;
		for(int i = 0; i < FNumCurvasReg; i++) {
			if(FGastoRend[i][0] < FGastoBombeo[i + 1] * 1.001 && FGastoRend[i][0] > FGastoBombeo[i + 1] * 0.999) {
				FNumCurvasRenAd[i] = FNumCurvasRen[i] + 1;
				RendEnBombeo = true;
				std::cout << "INFO: Surge limit for " << FRegimenCurva[i] << " rpm iso-speed line has an efficiency value deffined" <<
						  std::endl;
			} else {
				FNumCurvasRenAd[i] = FNumCurvasRen[i] + 2;
				RendEnBombeo = false;
				std::cout << "INFO: Surge limit for " << FRegimenCurva[i] <<
						  " rpm iso-speed line does not have an efficiency value deffined" << std::endl;
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
		ImprimeMapa();
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

void TMapaComp2Tub::Spline(int n, double *x, double *y, double *sol) {
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
				//sol[n-2]=0;
				if(n >= 4) {
					for(int i = n - 3; i >= 1; --i) {
						sol[i] = (FSpl[i].yp - FSpl[i].ud1 * sol[i + 1]) / FSpl[i].ud;
						//sol[i]=0;
					}
				}
			}
		}
		/*for(int i=0;i<n;++i){
		 printf("%d %lf %lf %lf\n",i,x[i],y[i],sol[i]);
		 } */
	} catch(exception &N) {
		std::cout << "ERROR: Spline en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: Spline en el compresor: "/* +std::to_string(FNumeroCompresor)+N.what()*/);
	}
}

//void TMapaComp2Tub::Hermite(int n,double *x,double *y,double *sol)
//{
//try
//{
//
//double DeltaK,AlphaK,BetaK,TauK;
//
//for(int i=1;i<n-1;++i){
//	sol[i]=(y[i]-y[i-1])/2./(x[i]-x[i-1])+(y[i+1]-y[i])/2./(x[i+1]-x[i]);
//}
//sol[0]=(y[1]-y[0])/(x[1]-x[0]);
//sol[n-1]=(y[n-1]-y[n-2])/(x[n-1]-x[n-2]);
//
//for(int i=0;i<n-1;i++){
//	DeltaK=(y[i+1]-y[i])/(x[i+1]-x[i]);
//	if(DeltaK==0){
//		sol[i]=0;
//		sol[i+1]=0;
//	}else{
//		AlphaK=sol[i]/DeltaK;
//		BetaK=sol[i+1]/DeltaK;
//		if(BetaK*BetaK+AlphaK*AlphaK>9){
//			TauK=3/sqrt(BetaK*BetaK+AlphaK*AlphaK);
//			sol[i]=TauK*AlphaK*DeltaK;
//			sol[i+1]=TauK*BetaK*DeltaK;
//		}
//	}
//}
//}
//catch(Exception &N)
//{
//std::cout << "ERROR: Hermite en el compresor: " << FNumeroCompresor << std::endl;
//std::cout << "Tipo de error: " << N.what() << std::endl;
//throw Exception("ERROR: Spline en el compresor: " +std::to_string(FNumeroCompresor)+N.what());
//}
//}
//
//void TMapaComp2Tub::Hermite(int n,std::vector<double> x,std::vector<double> y,std::vector<double> *sol)
//{
//try
//{
//
//double DeltaK,AlphaK,BetaK,TauK;
//
//for(int i=1;i<n-1;++i){
//	(*sol)[i]=(y[i]-y[i-1])/2./(x[i]-x[i-1])+(y[i+1]-y[i])/2./(x[i+1]-x[i]);
//}
//(*sol)[0]=(y[1]-y[0])/(x[1]-x[0]);
//(*sol)[n-1]=(y[n-1]-y[n-2])/(x[n-1]-x[n-2]);
//
//for(int i=0;i<n-1;i++){
//	DeltaK=(y[i+1]-y[i])/(x[i+1]-x[i]);
//	if(DeltaK==0){
//		(*sol)[i]=0;
//		(*sol)[i+1]=0;
//	}else{
//		AlphaK=(*sol)[i]/DeltaK;
//		BetaK=(*sol)[i+1]/DeltaK;
//		if(BetaK*BetaK+AlphaK*AlphaK>9){
//			TauK=3/sqrt(BetaK*BetaK+AlphaK*AlphaK);
//			(*sol)[i]=TauK*AlphaK*DeltaK;
//			(*sol)[i+1]=TauK*BetaK*DeltaK;
//		}
//	}
//}
//}
//catch(Exception &N)
//{
//std::cout << "ERROR: Hermite en el compresor: " << FNumeroCompresor << std::endl;
//std::cout << "Tipo de error: " << N.what() << std::endl;
//throw Exception("ERROR: Spline en el compresor: " +std::to_string(FNumeroCompresor)+N.what());
//}
//}

void TMapaComp2Tub::SplineVector(int n, std::vector<double> x, std::vector<double> y, std::vector<double> sol) {
	try {
		double Espaciado = 0.;

		std::vector<stSpline2Tub> Spl;

		Spl.resize(n);

//Calculo de diferencias
		for(int i = 1; i < n; ++i) {
			Spl[i].h = x[i] - x[i - 1];
			Spl[i].dif = y[i] - y[i - 1];
			if(Spl[i].h <= 0.) {
				std::cout << "ERROR: Error al crear la spline" << std::endl;
				std::cout << "       Los valores en X no estan ordenados o existen 2 puntos situados en el mismo X" << std::endl <<
						  std::endl;
				throw Exception("ERROR: Error al crear la spline");
			}
		}

//Elementos matriz,coeficiente y terminos independientes
//Solo se almacenan elementos diagonales
		for(int i = 1; i < n - 1; ++i) {
			Espaciado = Spl[i + 1].h / Spl[i].h;
			if(Espaciado < 0.1 || Espaciado > 10.) {
				std::cout << "WARNING: Deberias utilizar una distribucion mas uniforme entre los valores de X" << std::endl;
				std::cout << "         utilizados para ajustar la spline y asi evitar problemas" << std::endl << std::endl;
			}
			Spl[i].d = 2 * (Spl[i + 1].h + Spl[i].h);
			Spl[i].d1 = Spl[i + 1].h;
			Spl[i].b = (Spl[i + 1].dif / Spl[i + 1].h - Spl[i].dif / Spl[i].h) * 6.;
		}
//Descomposicion de Cholesky
		Spl[1].ud = sqrt(Spl[1].d);
		for(int i = 2; i < n - 1; i++) {
			Spl[i - 1].ud1 = Spl[i - 1].d1 / Spl[i - 1].ud;
			Spl[i].ud = sqrt(Spl[i].d - Spl[i - 1].ud1 * Spl[i - 1].ud1);
		}
//Sustitucion directa
		Spl[1].yp = Spl[1].b / Spl[1].ud;
		for(int i = 2; i < n - 1; ++i) {
			Spl[i].yp = (Spl[i].b - Spl[i - 1].ud1 * Spl[i - 1].yp) / Spl[i].ud;
		}
//Sustitucion inversa
		sol[0] = 0.;
		if(n >= 2) {
			sol[n - 1] = 0;
			if(n >= 3) {
				sol[n - 2] = Spl[n - 2].yp / Spl[n - 2].ud;
				if(n >= 4) {
					for(int i = n - 3; i >= 1; --i) {
						sol[i] = (Spl[i].yp - Spl[i].ud1 * sol[i + 1]) / Spl[i].ud;
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

double TMapaComp2Tub::EvaluaSpline(double punto, int n, double *x, double *y, double *sol) {
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

//double TMapaComp2Tub::EvaluaHermite(double punto,int n,double *x,double *y,double *sol)
//{
//try
//{
//double ret_val,h00,h10,h01,h11,t2,t3,t,h;
//int k=0;
//
//if(punto<=x[0]){
//	ret_val=y[0];
//}else if(punto>=x[n-1]){
//	ret_val=y[n-1];
//}else{
//	while(x[k]<punto && k<n-1){
//		++k;
//	}
//	h=(x[k]-x[k-1]);
//	t=(punto-x[k-1])/h;
//	t2=t*t;
//	t3=t2*t;
//	h00=2*t3-3*t2+1;
//	h10=t3-2*t2+t;
//	h01=-2*t3+3*t2;
//	h11=t3-t2;
//	ret_val=h00*y[k-1]+h*h10*sol[k-1]+h01*y[k]+h*h11*sol[k];
//}
//
//return ret_val;
//
//}
//catch(Exception &N)
//{
//std::cout << "ERROR: EvaluaHermite en el compresor: " << FNumeroCompresor << std::endl;
//std::cout << "Tipo de error: " << N.what() << std::endl;
//throw Exception("ERROR: EvaluaHermite en el compresor: " +std::to_string(FNumeroCompresor)+N.what());
//}
//}
//
//double TMapaComp2Tub::EvaluaHermite(double punto,int n,std::vector<double> x,std::vector<double> y,std::vector<double> sol)
//{
//try
//{
//double ret_val,h00,h10,h01,h11,t2,t3,t,h;
//int k=0;
//
//if(punto<=x[0]){
//	ret_val=y[0];
//}else if(punto>=x[n-1]){
//	ret_val=y[n-1];
//}else{
//	while(x[k]<punto && k<n-1){
//		++k;
//	}
//	h=(x[k]-x[k-1]);
//	t=(punto-x[k-1])/h;
//	t2=t*t;
//	t3=t2*t;
//	h00=2*t3-3*t2+1;
//	h10=t3-2*t2+t;
//	h01=-2*t3+3*t2;
//	h11=t3-t2;
//	ret_val=h00*y[k-1]+h*h10*sol[k-1]+h01*y[k]+h*h11*sol[k];
//}
//
//return ret_val;
//
//}
//catch(Exception &N)
//{
//std::cout << "ERROR: EvaluaHermite en el compresor: " << FNumeroCompresor << std::endl;
//std::cout << "Tipo de error: " << N.what() << std::endl;
//throw Exception("ERROR: EvaluaHermite en el compresor: " +std::to_string(FNumeroCompresor)+N.what());
//}
//}

double TMapaComp2Tub::InterpolaLineal(double punto, int n, double *x, double *y) {
	try {
		int k = 0;
		double delta = 0.;
		double ret_val = 0.;
//Determinacion del indice para evaluacion spline

		if(punto < x[0]) {
			ret_val = y[0];
		} else if(punto > x[n - 1]) {
			ret_val = y[n - 1];
		} else {
			while(x[k] < punto && k < n - 1) {
				++k;
			}
			delta = (punto - x[k - 1]) / (x[k] - x[k - 1]);
			ret_val = y[k] * delta + y[k - 1] * (1 - delta);

		}

		return ret_val;

	} catch(exception &N) {
		std::cout << "ERROR: EvaluaSpline en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: EvaluaSpline en el compresor: "/* +std::to_string(FNumeroCompresor)+N.what()*/);
	}
}

double TMapaComp2Tub::EvaluaSpline(double punto, int n, std::vector<double> x, std::vector<double> y,
								   std::vector<double> sol) {
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

double TMapaComp2Tub::EvaluaRCSplines(double Massflow) {
	try {
		double ret_val = 0.;
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
		}
		return ret_val;

	} catch(exception &N) {
		std::cout << "ERROR: EvaluaRCSpline en el compresor: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception("ERROR: EvaluaRCSpline en el compresor: "/* +std::to_string(FNumeroCompresor)+N.what()*/);
	}
}

double TMapaComp2Tub::EvaluaRCHermite(double Massflow) {
	try {
		double ret_val = 0., t2 = 0., t3 = 0., t = 0., h00 = 0., h10 = 0., h01 = 0., h11 = 0., h = 0.;

		if(Massflow <= FGastoInt[0]) {
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
		throw Exception("ERROR: EvaluaRCSpline en el compresor: "/* +std::to_string(FNumeroCompresor)+N.what()*/);
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

void TMapaComp2Tub::PolOrtogonal(int nterms, int npoint, double *ma, double *rd, double *w, double *b, double *c,
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

void TMapaComp2Tub::InterpolaMapa(double rtc, double AmbientTemperature) {

	double DeltaN = 0.;
	int Cent = 0;
	int index = 0;

	try {

// Se adapta el regimen de giro a interpolar al mapa compresor
		if(FCorrect)
			FRegComp = rtc * sqrt(FTempRef / AmbientTemperature);
		else
			FRegComp = rtc;

// Se halla el regimen de giro inferior al que se quiere interpolar

		FCurvInf = 0;
		for(int i = 0; i < FNumCurvasReg; ++i) {
			if(FRegComp >= FRegimenCurva[i]) {
				FCurvInf = i;
			}
		}
		DeltaN = (FRegComp - FRegimenCurva[FCurvInf]) / FIncReg;

// Se interpola linealmente el massflow de bombeo y el de RC = 1

		if(FRegComp >= FRegMax) {
			FGastoBombeoX = FGastoBombeo[FCurvInf + 1] + (FGastoBombeo[FCurvInf + 1] - FGastoBombeo[FCurvInf]) / FIncReg *
							(FRegComp - FRegMax);

			FRelCompBombeoX = FRelCompBombeo[FCurvInf + 1] + (FRelCompBombeo[FCurvInf + 1] - FRelCompBombeo[FCurvInf]) / FIncReg *
							  (FRegComp - FRegMax);

			FGastoRelComp1X = FGastoRelComp1[FCurvInf] + (FGastoRelComp1[FCurvInf] - FGastoRelComp1[FCurvInf - 1]) / FIncReg *
							  (FRegComp - FRegMax);

		} else if(FRegComp < FRegMin) {
			FGastoBombeoX = FGastoBombeo[FCurvInf + 1] - (FGastoBombeo[FCurvInf + 2] - FGastoBombeo[FCurvInf + 1]) / FIncReg *
							(FRegMin - FRegComp);

			FRelCompBombeoX = EvaluaHermite(FGastoBombeoX, FNumCurvasReg + 1, FGastoBombeo, FRelCompBombeo, FCoefSplBombeo);

			FGastoRelComp1X = FGastoRelComp1[FCurvInf] - (FGastoRelComp1[FCurvInf + 1] - FGastoRelComp1[FCurvInf]) / FIncReg *
							  (FRegMin - FRegComp);
		} else {
			FGastoBombeoX = FGastoBombeo[FCurvInf + 1] + (FGastoBombeo[FCurvInf + 2] - FGastoBombeo[FCurvInf + 1]) * DeltaN;

			FRelCompBombeoX = EvaluaHermite(FGastoBombeoX, FNumCurvasReg + 1, FGastoBombeo, FRelCompBombeo, FCoefSplBombeo);

			FGastoRelComp1X = FGastoRelComp1[FCurvInf] + (FGastoRelComp1[FCurvInf + 1] - FGastoRelComp1[FCurvInf]) * DeltaN;
		}
		if(FGastoBombeoX < 0)
			FGastoBombeoX = 0.;
		if(FGastoRelComp1X < FGastoBombeoX)
			FGastoRelComp1X = FGastoBombeoX + 1e-6;

// Calculo de la curva interpolada Relacion de compresion/Masa corregida

// Se haya el ultimo massflow del que se tiene informacion para el regimen de giro inferior

		if(FRegComp < FRegMin) {
			FNumPuntos = floor((FGastoRelComp1X - FGastoMin) / FIncGasto) + 1;
		} else {
			int j = 0;
			/*while(FRelCompNuevo[FCurvInf][j]>0. && j<FNumPuntosGastoNuevo-1){
			 ++j;
			 }
			 FNumPuntos=j;*/
			if(FRegComp >= FRegMax) {
				index = FCurvInf - 1;
			} else {
				index = FCurvInf;
			}
			while((FGastoInt[j] = FGastoMin + FIncGasto * j) < FGastoRelComp1[index]) {
				++j;
			}
			FNumPuntos = j;
		}

// Se interpola linealmente la relacion de compresion entre el regimen de giro inferior
// y superior excepto para el ultimo punto.

		if(FRegComp < FRegMin) {
			for(int j = 0; j < FNumPuntos; ++j) {
				FGastoInt[j] = FGastoMin + FIncGasto * j;
				FRelCompInt[j] = FRelCompNuevo[FCurvInf][j] - (FRelCompNuevo[FCurvInf + 1][j] - FRelCompNuevo[FCurvInf][j]) / FIncReg
								 * (FRegMin - FRegComp);
				if(FRelCompInt[j] < 1.)
					FRelCompInt[j] = 1.;
			}
		} else {
			for(int j = 0; j < FNumPuntos; ++j) {
				//FGastoInt[j]=FGastoMin+FIncGasto*j;
				if(FRegComp >= FRegMax) {
					FRelCompInt[j] = FRelCompNuevo[FCurvInf][j] + (FRelCompNuevo[FCurvInf][j] - FRelCompNuevo[FCurvInf - 1][j]) * DeltaN;
				} else {
					FRelCompInt[j] = FRelCompNuevo[FCurvInf][j] + (FRelCompNuevo[FCurvInf + 1][j] - FRelCompNuevo[FCurvInf][j]) * DeltaN;
				}
			}
		}
		FGastoInt[FNumPuntos] = FGastoRelComp1X;
		FRelCompInt[FNumPuntos] = 1.;

// Se obtienen los coeficientes que ajusta la spline de la curva interpolada.

		Hermite(FNumPuntos + 1, FGastoInt, FRelCompInt, FCoefSplRC);

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

double TMapaComp2Tub::EvaluaRendimiento(double MasaAire) {
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

double TMapaComp2Tub::EvaluaRendSplines(double MasaAire) {
	double AireAdim = 0.;
	double ret_val = 0.;
	double RendInf = 0.;
	double RendSup = 0.;
	double DeltaN = 0.;
	int index = 0;
	try {
		if(FRegComp >= FRegMax) {
			index = FCurvInf - 1;
		} else {
			index = FCurvInf;
		}

		if(MasaAire < FGastoBombeoX) {
			//En Bombeo se toma como rendimiento el del ultimo punto.
			DeltaN = (FRegComp - FRegimenCurva[FCurvInf]) / FIncReg;

			RendInf = FRendAdim[index][0];
			RendSup = FRendAdim[index + 1][0];

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

			RendInf = EvaluaHermite(AireAdim, FNumCurvasRenAd[index], FGastoAdim[index], FRendAdim[FCurvInf],
									FCoefSplRend[FCurvInf]);

			//RendInf=InterpolaLineal(AireAdim,FNumCurvasRenAd[index],FGastoAdim[index],
			//	  FRendAdim[FCurvInf]);

			RendSup = EvaluaHermite(AireAdim, FNumCurvasRenAd[index + 1], FGastoAdim[index + 1], FRendAdim[index + 1],
									FCoefSplRend[index + 1]);

			//RendSup=InterpolaLineal(AireAdim,FNumCurvasRenAd[index+1],FGastoAdim[index+1],
			//	  FRendAdim[index+1]);

			DeltaN = (FRegComp - FRegimenCurva[FCurvInf]) / FIncReg;

			if(FRegComp >= FRegMax) {
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

double TMapaComp2Tub::GetRelCompInt(int i) {
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

double TMapaComp2Tub::GetGastoInt(int i) {
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

double TMapaComp2Tub::BuscaRegimen(double RC, double Massflow, double AmbientTemperature) {
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
				RCSup = EvaluaRCHermite(Massflow);
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
				RCInf = EvaluaRCHermite(Massflow);
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
			RCSup = EvaluaRCHermite(Massflow);
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

void TMapaComp2Tub::Cambio_Mapa(double radtip, double radhub, double radrodete) {
	try {
//FILE *fplot;
		int cont = 0;
		double r1 = 0.;
		int n = 0;

//los valores de ra,rb y r2, deberan ser medidos directamente en el compresor y ser leidos desde lee_compresor

		Fnegmas = new double[5];
		Fnegrc = new double[5];
		Fsolneg = new double[5];
		for(int i = 0; i < 5; i++) {
			Fsolneg[i] = 0.;
		}
		n = 0.;
		while((double) n * FIncGasto < 0.07) {
			++n;
		}
		FGastoMin = -n * FIncGasto;

		r1 = sqrt((pow2(radhub) + pow2(radtip)) / 2);
//FGastoMin=-0.07;
		FNumPuntosGastoNuevo = floor(((FGastoMax - FGastoMin) / FIncGasto) + 0.5) + 1;

		FSpl = new stSpline2Tub[FNumPuntosGastoNuevo + 1];
		FGastoInt = new double[FNumPuntosGastoNuevo + 1];
		FRelCompInt = new double[FNumPuntosGastoNuevo + 1];
		FCoefSplRC = new double[FNumPuntosGastoNuevo + 1];

		FRelCompNuevo = new double*[FNumCurvasReg];
		for(int i = 0; i < FNumCurvasReg; i++) {
			FRelCompNuevo[i] = new double[FNumPuntosGastoNuevo];
		}

		FGastoCompNuevo = new double[FNumPuntosGastoNuevo];
		for(int j = 0; j < FNumPuntosGastoNuevo; ++j) {
			FGastoCompNuevo[j] = FGastoMin + j * FIncGasto;
		}

		for(int i = 0; i < FNumCurvasReg; ++i) {
			Fnegmas[0] = FGastoMin;
			Fnegmas[1] = -0.0001;
			Fnegmas[2] = 0;
			Fnegmas[3] = FGastoBombeo[i] - 0.0001;
			Fnegmas[4] = FGastoBombeo[i];
			Fnegrc[0] = FRelCompBombeo[i];
			Fnegrc[1] = pow(1 + 1 / (__Gamma::Cp_x2 * (FTempRef)) * pow2(__units::RPMToRad_s(FRegMin + i * FIncReg)) * (pow2(
								radrodete) - pow2(r1)), __Gamma::G_9);
			Fnegrc[2] = pow(1 + 1 / (__Gamma::Cp_x2 * (FTempRef)) * pow2(__units::RPMToRad_s(FRegMin + i * FIncReg)) * (pow2(
								radrodete) - pow2(r1)), __Gamma::G_9);
			Fnegrc[3] = FRelCompBombeo[i];
			Fnegrc[4] = FRelCompBombeo[i];
			Hermite(5, Fnegmas, Fnegrc, Fsolneg);
			cont = 1;
			for(int j = 0; j < FNumPuntosGastoNuevo; ++j) {
				if(FGastoMin + j * FIncGasto < FGastoBombeo[i]) {
					FRelCompNuevo[i][j] = EvaluaHermite(FGastoMin + j * FIncGasto, 5, Fnegmas, Fnegrc, Fsolneg);
					if(FGastoMin + j * FIncGasto > FIncGasto / 10)
						++cont;
				} else {
					FRelCompNuevo[i][j] = FRelComp[i][cont];
					++cont;
				}
				//printf("%d %lf %lf\n",i,FGastoMin+j*FIncGasto,FRelCompNuevo[i][j]);
			}
		}

		/*fplot=fopen ("datos.m","w");
		 fprintf (fplot, "function [a, NumPuntosGasto, IncGasto, GastoMin, GastoRelComp1]=datos \na=[");
		 for (int i=0; i<FNumCurvasReg; ++i) {
		 for(int j=0; j<FNumPuntosGastoNuevo;++j){
		 if(i!=FNumCurvasReg || j!=FNumPunyosGastoNuevo) fprintf (fplot,"%lf;\n ",Faux[i][j]);
		 else fprintf (fplot,"%lf ];\n",rcom[i]);

		 }
		 fprintf (fplot,"NumPuntosGasto= %d; \nIncGasto= %lf; \nGastoMin= %lf; \nmarc1= [",FNumPuntosGastoNuevo,FIncGasto,FGastoMin);
		 for (i=0; i<FNumCurvasReg; i++) fprintf (fplot,"%lf;\n",FGastoRelComp1[i]);
		 fprintf (fplot,"];");
		 fclose (fplot); */
	} catch(exception &N) {
		std::cout << "ERROR: Cambio_Mapa: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TMapaComp2Tub::ImprimeMapa() {
	std::cout << "Printing compressor map .";
	FILE *fich;
	FILE *fichrd;
	fich = fopen("MapaInterp.txt", "w");
	fichrd = fopen("MapaRend.txt", "w");
	double inc = 0.;
	double *massflow;
	double **rc;
	double **rend;
	double *rcbomb;
	double *gasbomb;

	massflow = new double[200];

	rcbomb = new double[FNumCurvasReg + 3];

	gasbomb = new double[FNumCurvasReg + 3];

	rc = new double*[200];
	rend = new double*[200];
	for(int j = 0; j < 200; j++) {
		rc[j] = new double[FNumCurvasReg + 2];
		rend[j] = new double[FNumCurvasReg + 2];
	}
	inc = (FGastoMax - FGastoMin) / 199;
	for(int j = 0; j < 200; j++) {
		massflow[j] = FGastoMin + inc * (double) j;
	}
	gasbomb[0] = 0.;
	rcbomb[0] = 1.;

	for(int i = 0; i < FNumCurvasReg + 1; i++) {
		std::cout << ".";
		InterpolaMapa(FRegMin + FIncReg * (double) i, FTempRef);
		for(int j = 0; j < 200; j++) {
			if(massflow[j] > FGastoRelComp1X) {
				rc[j][i] = 1.;
				rend[j][i] = 0.4;
			} else {
				rc[j][i] = EvaluaRCHermite(massflow[j]);
				if(massflow[j] <= FGastoBombeoX) {
					rend[j][i] = EvaluaRendSplines(FGastoBombeoX);
				} else {
					rend[j][i] = EvaluaRendSplines(massflow[j]);
				}
			}
		}
		gasbomb[i + 1] = FGastoBombeoX;
		rcbomb[i + 1] = FRelCompBombeoX;

	}

	for(int j = 0; j < 200; j++) {
		if(j < FNumCurvasReg + 1 + 2) {
			fprintf(fich, "%lf\t%lf\t", gasbomb[j], rcbomb[j]);
		} else {
			fprintf(fich, "\t\t");
		}
		fprintf(fich, "%lf\t", massflow[j]);
		fprintf(fichrd, "%lf\t", massflow[j]);
		for(int i = 0; i < FNumCurvasReg + 1; i++) {
			fprintf(fich, "%lf\t", rc[j][i]);
			fprintf(fichrd, "%lf\t", rend[j][i]);

		}
		fprintf(fich, "\n");
		fprintf(fichrd, "\n");
	}

	for(int j = 0; j < 200; j++) {
		delete[] rc[j];
		delete[] rend[j];
	}
	delete[] massflow;
	delete[] rcbomb;
	delete[] gasbomb;
	delete[] rend;
	delete[] rc;

	fclose(fich);
	fclose(fichrd);

	FILE *fich2;
	fich2 = fopen("MapaReal.txt", "w");

	for(int j = 0; j < FNumPuntosGastoNuevo; ++j) {
		fprintf(fich2, "%lf\t", FGastoMin + j * FIncGasto);
		for(int i = 0; i < FNumCurvasReg; ++i) {
			fprintf(fich2, "%lf\t", FRelCompNuevo[i][j]);
		}
		fprintf(fich2, "\n");
	}

	fclose(fich2);

	std::cout << "Finished." << std::endl;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TMapaComp2Tub::ReadGTPowerMap(FILE *fich, int correct) {
	try {
		double speed = 0., mass = 0., pres = 0., eff = 0.;
		int i = 0; //Curva de isoregimen
		int j = 0; //Puntos de la curva
		if(correct == 1)
			FCorrect = true;
		else
			FCorrect = false;
		fscanf(fich, "%lf %lf", &FPresionRef, &FTempRef);
		FPresionRef = __units::BarToPa(FPresionRef);
		FGTSpeed.resize(i + 1);
		FGTMass.resize(i + 1);
		FGTPres.resize(i + 1);
		FGTEff.resize(i + 1);
		while(!feof(fich)) {
			fscanf(fich, "%lf %lf %lf %lf", &speed, &mass, &pres, &eff);
			if(!feof(fich)) {
				if(j > 0) {
					if(speed != FGTSpeed[i][j - 1]) {
						i++;
						j = 0;
						FGTSpeed.resize(i + 1);
						FGTMass.resize(i + 1);
						FGTPres.resize(i + 1);
						FGTEff.resize(i + 1);
					}
				}
				FGTSpeed[i].push_back(speed);
				FGTMass[i].push_back(mass);
				FGTPres[i].push_back(pres);
				FGTEff[i].push_back(eff);
				j++;
			}
		}

	} catch(exception &N) {
		std::cout << "ERROR: Cambio_Mapa: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TMapaComp2Tub::RearrangeGTPowerMap(double rtip, double rhub, double rwheel) {
	try {
		double FlowMass = 0.;

		//FTempRef=303;
		//FPresionRef=0.96;
		//for(int i=0;i<FGTSpeed.size();i++){
		//	for(int j=0;j<FGTSpeed[i].size();j++){
		//		printf("%lf %lf %lf %lf\n",FGTSpeed[i][j],FGTMass[i][j],FGTPres[i][j],FGTEff[i][j]);
		//	}
		//}

		// Regimen maximo y minimo del mapa.
		FRegMin = FGTSpeed[0][0];
		FRegMax = FGTSpeed[FGTSpeed.size() - 1][0];
		FIncReg = (FRegMax - FRegMin) / (FGTSpeed.size() - 1);

		FNumCurvasReg = FGTSpeed.size();
		// Massflow maximo y minimo del mapa.
		FGastoMin = 0;
		FGastoMax = FGTMass[FGTMass.size() - 1][FGTMass[FGTMass.size() - 1].size() - 1];
		FIncGasto = (FGastoMax - FGastoMin) / 20;

		FNumPuntosGasto = 21;

		FGTCoefCR.resize(FNumCurvasReg);

		FNumCurvasRendMax = 0;

		for(int i = 0; i < FNumCurvasReg; ++i) {
			FGTCoefCR[i].resize(FGTSpeed[i].size());
			//int k=FGTSpeed[i].size();
			Hermite(FGTSpeed[i].size(), FGTMass[i], FGTPres[i], &FGTCoefCR[i]);
			/*for(int j=0;j<FGTSpeed[i].size();j++){
			 printf("%lf\n",FGTCoefCR[i][j]);
			 }
			 printf("\n"); */
			//SplineVector(k,FGTMass[i],FGTPres[i],FGTMass[i]);
			if(FGTEff[i].size() > (dMatrix::size_type) FNumCurvasRendMax) {
				FNumCurvasRendMax = FGTEff[i].size();
			}
		}

		FGastoRelComp1 = new double[FNumCurvasReg];
		FGastoBombeo = new double[FNumCurvasReg + 1];
		FRelCompBombeo = new double[FNumCurvasReg + 1];
		FRegimenCurva = new double[FNumCurvasReg];
		FNumCurvasRen = new int[FNumCurvasReg];
		FNumCurvasRenAd = new int[FNumCurvasReg];
		FCoefSplBombeo = new double[FNumCurvasReg + 1];
		FOrtp = new stOrtoPol2Tub[FNumCurvasRendMax + 1];
		FRelComp = new double*[FNumCurvasReg];
		for(int i = 0; i < FNumCurvasReg; i++) {
			FRelComp[i] = new double[FNumPuntosGasto];
		}
		FGastoRend = new double*[FNumCurvasReg];
		FRend = new double*[FNumCurvasReg];
		for(int i = 0; i < FNumCurvasReg; i++) {
			FGastoRend[i] = new double[FNumCurvasRendMax];
			FRend[i] = new double[FNumCurvasRendMax];
		}

		for(int i = 0; i < FNumCurvasReg; i++) {
			FRegimenCurva[i] = FRegMin + FIncReg * (double) i;
			FGastoRelComp1[i] = FGTMass[i][FGTMass[i].size() - 1];
			FGastoBombeo[i] = FGTMass[i][0];
			FRelCompBombeo[i] = FGTPres[i][0];
			for(int j = 0; j < 21; j++) {
				FlowMass = FGastoMin + (double) j * FIncGasto;
				if(FlowMass > FGastoBombeo[i]) {
					if(FlowMass < FGastoRelComp1[i]) {
						FRelComp[i][j] = EvaluaHermite(FlowMass, FGTMass[i].size(), FGTMass[i], FGTPres[i], FGTCoefCR[i]);
					} else {
						FRelComp[i][j] = 1.;
					}

				} else {
					FRelComp[i][j] = FRelCompBombeo[i];
				}
			}
			FNumCurvasRen[i] = FGTEff[i].size() - 1;
			for(Uint j = 0; j < FGTEff[i].size() - 1; j++) {
				FGastoRend[i][j] = FGTMass[i][j];
				FRend[i][j] = FGTEff[i][j];
			}
		}

		Cambio_Mapa(rtip, rhub, rwheel);

		// Interpolacion curva de bombeo.

		for(int i = FNumCurvasReg; i > 0; --i) {
			FGastoBombeo[i] = FGastoBombeo[i - 1];
			FRelCompBombeo[i] = FRelCompBombeo[i - 1];
		}
		FGastoBombeo[0] = 0.;
		FRelCompBombeo[0] = 1.;

		Hermite(FNumCurvasReg + 1, FGastoBombeo, FRelCompBombeo, FCoefSplBombeo);

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

		ImprimeMapa();

	} catch(exception &N) {
		std::cout << "ERROR: RearrangeGTPowerMap: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TMapaComp2Tub::WriteMapForWAM() {
	try {
		double gast = 0.;

		FILE *fich2;
		fich2 = fopen("MapaWAM.cmp", "w");

		fprintf(fich2, "%lf %lf ", FPresionRef, FTempRef);
		fprintf(fich2, "%lf %lf %lf ", FRegMin, FRegMax, FIncReg);
		fprintf(fich2, "%lf %lf %lf ", FGastoMin, FGastoMax, FIncGasto);
		fprintf(fich2, "%d", FNumCurvasRendMax);
		fprintf(fich2, "\n");
		for(int i = 0; i < FNumCurvasReg; i++) {
			fprintf(fich2, "%lf\n", FGastoRelComp1[i]);
			fprintf(fich2, "%lf %lf\n", FGastoBombeo[i + 1], FRelCompBombeo[i + 1]);
			for(int j = 0; j < FNumPuntosGasto; j++) {
				gast = (double) j * FIncGasto;
				if(gast > FGastoRelComp1[i]) {
					fprintf(fich2, "0.0\n");
				} else {
					fprintf(fich2, "%lf\n", FRelComp[i][j]);
				}
			}
			for(int j = 0; j < FNumCurvasRendMax; j++) {
				if(j < FNumCurvasRen[i]) {
					fprintf(fich2, "%lf %lf\n", FGastoRend[i][j], FRend[i][j]);
				} else {
					fprintf(fich2, "0.0 0.0\n");
				}
			}

		}

		fclose(fich2);

	} catch(exception &N) {
		std::cout << "ERROR: WriteMapForWAM: " << FNumeroCompresor << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

#pragma package(smart_init)
