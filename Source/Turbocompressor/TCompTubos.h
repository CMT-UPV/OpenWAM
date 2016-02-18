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
#ifndef TCompTubosH
#define TCompTubosH

#include "TCompresor.h"
#include "Constantes.h"
// #include "SolverCompresor.h"
#include "NewCompSolver.h"
// #include "roots.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class TCondicionContorno;
class TTubo;

struct stCompOut {
	double A1in;
	double U1in;
	double A1out;
	double U1out;
	double Rc;
	double Cpre;
	double Gam;
	double Ga1;
	double Ga3;
	double Ga8;
	double E1;
	double M1;
	double A1outA;

	stCompOut(double iA1in, double iU1in, double iRc, double iCpre, double iGam) :
		A1in(iA1in), U1in(iU1in), Rc(iRc), Cpre(iCpre), Gam(iGam) {

		Ga1 = Gam - 1;
		Ga3 = Ga1 / 2;
		Ga8 = Gam / Ga1;
		E1 = (pow2(A1in) + Ga3 * pow2(U1in)) * (1 + Cpre);
		M1 = pow(A1in, 1 / Ga3) * U1in / Rc * pow(1 + Cpre, Ga8);
	}
	;
	double operator()(const double U1out) {

		A1outA = pow(E1 - Ga3 * pow2(U1out), 1 / Ga1);
		double A1outB = M1 / U1out;
		return A1outA - A1outB;
	}
	;

};

class TCompTubos: public TCompresor {
  private:

	TTubo *FTuboRot;
	TTubo *FTuboEst;

	int FPosicionRotor; // Posicion en el vector estructura TuboExtremo del tubo del rotor
	int FPosicionEstator; // Posicion en el vector estructura TuboExtremo del tubo del estator
	int FIndiceCCRotor; // Posicion del vector para tomar datos del tubo para la BC (0 Nodo izquierdo; 1 Nodo derecho)
	int FIndiceCCStator; // Posicion del vector para tomar datos del tubo para la BC (0 Nodo izquierdo; 1 Nodo derecho)

	double FPresionIn; // Pressure a la entrada del compresor (p_in)
	double FAsonidoIn; // Velocity del sonido a la entrada del compresor (a_in)
	double FDensidadIn; // Density a la entrada del compresor (den_in)
	double FTempIn; // Temperature a la entrada del compresor (t_in)
	double FVelInMax; // Velocity a la entrada maxima (x_max)
	double FVelInMin; // Velocity a la entrada minima (x_min)
	double FErrorVelIn; // Error de la velocidad a la entrada (error_x)
	double FVelocidadIn; // Velocity a la entrada (x)
	double FErrorCarIn; // Error limite de la caracteristica a la entrada (err_in)
	int FTuboIn; // Numero del tubo de entrada
	nmPipeEnd FExtremoIn; // PipeEnd del tubo de entrada
	int FCuentaVelIn; // (contx)

	double FPresionOut; // Pressure a la salida del compresor (p_out)
	double FAsonidoOut; // Velocity del sonido a la salida del compresor (a_out)
	double FDensidadOut; // Density a la salida del compresor (den_out)
	double FTempOut; // Temperature a la salida del compresor (t_out)
	double FVelOutMax; // Velocity a la salida maxima (y_max)
	double FVelOutMin; // Velocity a la salida minima (y_min)
	double FErrorVelOut; // Error de la velocidad a la salida (error_y)
	double FVelocidadOut; // Velocity a la salida del compresor (y)
	double FErrorCarOut; // Error limite de la caracteristica a la salida (err_out)
	int FTuboOut; // Numero del tubo de salida
	nmPipeEnd FExtremoOut; // PipeEnd del tubo de salida
	int FCuentaVelOut; // (conty)

	double FTempTotalInAnt; // Temperature total a la entrada anterior (tt_in_ant)
	double FTempTotalOutAnt; // Temperature total a la salida anterior (tt_out_ant)
	double FTempTotalIn;
	double FTempTotalOut;
	double FPreTotalIn;
	double FPreTotalOut;

	bool FFlujoDirecto; // Indicador del sentido del flujo
	bool FCambioSentido; // Indicador del cambido de sentido (svc_inverter)
	int FSentidoFlujo; // Indicador del sentido del flujo (svc)
	bool FPrimerCiclo;
	bool FCheckSentido;
	int FContadorCheckSentido; // Cuenta el numero de cambios de sentido en el mismo instante.
	// Si vale 2 se pasa a calcular las caracteristicas como extremo cerrado.

	double *FCarCIn;
	double *FCarDIn;
	double *FAaIn;

	double *FCarCOut;
	double *FCarDOut;
	double *FAaOut;

	int FSignoRotor;
	int FSignoStator;

	nmPipeEnd FExtremoRotor;
	nmPipeEnd FExtremoStator;

	// int FIndExtRotor;
	// int FIndExtStator;

	int FTuboRotor;
	int FTuboStator;

	double FAreaIn; // Area del conducto de entrada (areain)
	double FAreaOut; // Area del conducto de salida (areaout)

	double FAreaRotor;
	double FAreaStator;

	double FGasto1; // Masa en el compresor (mc)
	double FGastoOut;

	double FAout;

	double FLongitudCaract; // Longitud caracteristica del compresor

	double FCoefPresiones;

	double FRadioHub;
	double FRadioRodete;
	double FRadioTip;

	int FNodoFinRotor;
	int FNodoFinStator;

	double FGamma1;
	double FGamma2;
	double FGamma3;
	double FGamma4;
	double FGamma5;

	double FVolumen;
	double FPresionDep;
	double FAsonidoDep;
	double FTempDep;
	double FMasaDep;

	double FA1in;
	double FA1out;
	double FU1in;
	double FU1out;
	double FA2;
	double FU2;

	double FFlowIn;
	double FFlowOut;

	void Biseccion(double *VelIn, double *VelOut, double *AIn, double *AOut, double CarIn, double AaIn, double CarOut,
				   int sig);

	void MetodoNewton2D(double *a1, double *a2, double *u1, double *u2, double aa1, double aa2, double cc1, double cc2,
						double s1, double s2, double k, int sig);

	void Solver(double *a1, double *a2, double *u1, double *u2, double aa1, double aa2, double cc1, double cc2, double s1,
				double s2, double k, int sig);

	void ExtremoCerrado();

	double EntalpiaEntrada(double ASonidoE, double VelocidadE, double MasaE, double ASonidoD, double MasaD, double Gamma);

  protected:

  public:

	TCompTubos(int i, nmTipoCalculoEspecies SpeciesModel, int numeroespecies, nmCalculoGamma GammaCalculation,
			   bool ThereIsEGR);

	~TCompTubos();

	void CondicionCompresor(double Theta, stTuboExtremo *TuboExtremo, double TiempoActual, int TuboCalculado);

	void LeeCompresor(const char *FileWAM, fpos_t &filepos);

	void RelacionTubos(TCondicionContorno **BC, int NumeroCC);

	// NO SE UTILIZA EN ESTE TIPO DE COMPRESOR
	void CalculaGasto(double TrabajoInsTurbina, double TiempoActual) {
	}

	void Solver();

	void SolveInletBoundary(double &A, double &U, double &Ao, double &Uo);

	void SolveOutletBoundary(double &A, double &U);

	void NewPropertiesInTheVolume();

	void InFlow(double Ad, double &A, double &U);

	void OutFlow(double Ad, double &A, double &U);

	void AsignPipes(TCondicionContorno **BC, int NumeroCC);

	void Initialize();

};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#endif

