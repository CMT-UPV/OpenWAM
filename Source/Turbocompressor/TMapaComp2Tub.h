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
#ifndef TMapaComp2TubH
#define TMapaComp2TubH

#include <cstdio>
#include "Constantes.h"
#include "Globales.h"
#include "TCompressorMap.h"
#include <vector>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TMapaComp2Tub: public TCompressorMap {
  private:
//---------------------------------------------------------------------------
//          VARIABLES PRIVADAS
//---------------------------------------------------------------------------

	int FNumeroCompresor;       //Numero del compresor al que pertenece el mapa.
	double FPresionRef;                //Pressure de referencia
	double FTempRef;                   //Temperature de referencia
	double FRegMin;                    //Curva de minimo regimen
	double FRegMax;                    //Curva de maximo regimen
	double FRegComp;                   //Regimen current
	double FIncReg;                    //Incremento de regimen entre curvas
	int FNumCurvasReg;                 //Numero total de curvas de regimen
	double FGastoMin;                  //Massflow minimo
	double FGastoMax;                  //Massflow maximo
	double FIncGasto;             //Incremento de massflow entre puntos del mapa
	int FNumPuntosGasto;            //Numero de puntos de massflow de las curvas
	int FNumCurvasRendMax; //Numero maximo de curvas de rendimiento maximo de todas las curvas
	int *FNumCurvasRen;                //Numero de curvas de rendimiento
	int *FNumCurvasRenAd;    //Numero de curvas de rendimiento adimensionalizado
	double *FGastoRelComp1; //Massflow de la curva para relacion de compresion = 1
	double *FGastoBombeo; //Massflow de la curva de regimen cuando corta la de bombeo
	double *FRelCompBombeo;         //Relacion de compresion del punto de bombeo
	double **FRelComp;       //Relacion de compresion para los diferentes gastos
	double **FRelCompNuevo;
	double *FGastoCompNuevo;
	double **FGastoRend;              //Massflow para cada puntod de rendimiento
	double **FRend;                    //Valores de rendimiento del compresor
	double *Fnegmas;
	double *Fnegrc;
	double *Fsolneg;
	int FNumPuntosGastoNuevo;

	stSpline2Tub *FSpl;                    //Variables auxiliares rutina Spline
	stOrtoPol2Tub *FOrtp;             //Variables auxiliares rutina PolOrtogonal

	double *FCoefSplBombeo;
	double *FCoefSplRC;
	double *FRegimenCurva;
	double **FCoefbSup;
	double **FCoefbInf;
	double **FCoefcSup;
	double **FCoefcInf;
	double **FCoefdSup;
	double **FCoefdInf;
	int FNumTerms;
	double *FCoefbX;
	double *FCoefcX;
	double *FCoefdX;

	double FRendCurvaBombeo; // Varialbes para la creacion de las curvas de rendimiento con el massflow adimensionalizado
	double FRendGastoMaximo;
	double **FGastoAdim;
	double **FRendAdim;
	double **FCoefSplRend;

	int FNumPuntos;
	double *FGastoInt;
	double *FRelCompInt;

	double FGastoBombeoX;
	double FGastoRelComp1X;
	double FRelCompBombeoX;
	int FCurvInf;

	bool FCorrect;

	double FRadioHub;
	double FRadioRodete;
	double FRadioTip;

// Datos para GT-POWER

	std::vector<std::vector<double> > FGTSpeed;
	std::vector<std::vector<double> > FGTMass;
	std::vector<std::vector<double> > FGTPres;
	std::vector<std::vector<double> > FGTEff;
	std::vector<std::vector<double> > FGTCoefCR;
	std::vector<std::vector<double> > FGTCoefEff;

//---------------------------------------------------------------------------
//          FUNCIONES PRIVADAS
//---------------------------------------------------------------------------

	void Spline(int n, double *x, double *y, double *sol);

//void Hermite(int n,double *x,double *y,double *sol);

//void Hermite(int n,std::vector<double> x,std::vector<double> y,std::vector<double> *sol);

	void SplineVector(int n, std::vector<double> x, std::vector<double> y, std::vector<double> sol);

	void PolOrtogonal(int nterms, int npoint, double *ma, double *rd, double *w, double *b, double *c, double *d);

	double EvaluaSpline(double punto, int n, double *x, double *y, double *sol);

	double EvaluaSpline(double punto, int n, std::vector<double> x, std::vector<double> y, std::vector<double> sol);

//double EvaluaHermite(double punto,int n,double *x,double *y,double *sol);

//double EvaluaHermite(double punto,int n,std::vector<double> x,std::vector<double> y,std::vector<double> sol);

	double InterpolaLineal(double punto, int n, double *x, double *y);

	void Cambio_Mapa(double radtip, double radhub, double radrodete);

	void ImprimeMapa();

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

  public:

//---------------------------------------------------------------------------
//          VARIABLES PUBLICAS
//---------------------------------------------------------------------------

	int getNumPuntos() {
		return FNumPuntos;
	}
	;

	double getTempRef() {
		return FTempRef;
	}
	;

	double getPresionRef() {
		return FPresionRef;
	}
	;

	double GetRelCompInt(int i);

	double GetGastoInt(int i);

	double getGastoRelComp1() {
		return FGastoRelComp1X;
	}
	;

	double getGastoBombeo() {
		return FGastoBombeoX;
	}
	;

	double getRelCompBombeo() {
		return FRelCompBombeoX;
	}
	;

	double getMaxCompRatio() {
		return FRelCompBombeoX;
	}
	;

	double getRegimenCorregido() {
		return FRegComp;
	}
	;

	bool getCorrect() {
		return FCorrect;
	}
	;

	double getTempMeasure() {
		return 300;
	}
	;
//---------------------------------------------------------------------------
//          FUNCIONES PUBLICAS
//---------------------------------------------------------------------------

	TMapaComp2Tub(int i);

	~TMapaComp2Tub();

	void LeeMapa(FILE *fich);

	void InterpolaMapa(double rtc, double AmbientTemperature);

	double EvaluaRendimiento(double MasaAire);

	double EvaluaRendSplines(double MasaAire);

	double EvaluaRCSplines(double MasaAire);

	double EvaluaRCHermite(double Massflow);

	double BuscaRegimen(double RC, double Massflow, double AmbientTemperature);

	void ReadGTPowerMap(FILE *fich, int correct);

	void RearrangeGTPowerMap(double rtip, double rhub, double rwheel);

	void WriteMapForWAM();

	void CalculateAdiabaticEfficiency(TTC_HTM *HTM, double TinT) {
	}
	;

	void PutRadioHub(double val) {
		FRadioHub = val;
	}
	;

	void PutRadioRodete(double val) {
		FRadioRodete = val;
	}
	;

	void PutRadioRadioTip(double val) {
		FRadioTip = val;
	}
	;

};

#endif
