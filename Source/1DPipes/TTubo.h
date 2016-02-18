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


 \*--------------------------------------------------------------------------------*/

/**
 * @file TTubo.h
 * @author Francisco Jose Arnau <farnau@mot.upv.es>
 *
 * @section LICENSE
 *
 * This file is part of OpenWAM.
 *
 * OpenWAM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenWAM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenWAM.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @section DESCRIPTION
 * This file declares a finite differences pipe.
 */

//---------------------------------------------------------------------------
#ifndef TTuboH
#define TTuboH
#include <cstdio>
#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
//#include <cmath>
#include "TCondicionContorno.h"
#include "TDeposito.h"
#include "TBloqueMotor.h"
#include "TCCUnionEntreTubos.h"
#include "TCCCilindro.h"
#include "TCCDeposito.h"
#include "TDPF.h"
#include "Constantes.h"
#include "Globales.h"
//#include "roots.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TCondicionContorno;
class TBloqueMotor;
class TDeposito;

struct stFunDisCar {
	double A_0;
	double A_1;
	double V_0;
	double V_1;
	double D_t;
	double Sign;

	stFunDisCar(double a0, double a1, double v0, double v1, double dt, double sg) {
		A_0 = a0;
		A_1 = a1;
		V_0 = v0;
		V_1 = v1;
		D_t = dt;
		Sign = sg;
	}
	double operator()(double x) {
		double xx = 1 - x;
		if(Sign > 0) {
			double a = A_0 * xx + A_1 * x;
			double v = V_0 * xx + V_1 * x;
			return x - (a - v) * D_t;
		} else {
			double a = A_0 * x + A_1 * xx;
			double v = V_0 * x + V_1 * xx;
			return xx - (a + v) * D_t;
		}
	}
};

/**
 * @brief a Finite differences pipe.
 *
 * This class contains the methods, variables and attributes related with
 * finite differences pipes.
 */
class TTubo {
  private:
//---------------------------------------------------------------------------
//          VARIABLES PRIVADAS
//---------------------------------------------------------------------------

	int FTuboCCNodoIzq; //!< Save the position in the struct TuboExtremo of the pipe connected to the left end
	int FTuboCCNodoDer; //!< Save the position in the struct TuboExtremo of the pipe connected to the right end

	// Datos Geometricos

	int FNumeroTubo;          	//!< Number of the pipe
	int FNodoIzq;               //!< Number of the connection at the left end
	int FNodoDer;       		//!< Number of the connection at the rignt end
	int FNin;              		//!< Number of calculation cells
	int FJClase;           		//!< Tipe of the pipe
	double FXref;                 	//!< Cell size
	double FLongitudTotal;   		//!< Total length of the pipe
	double FMallado;           	//!< Initial cell size
	int FNTramos;               //!< Number of parts of the pipe
	nmTipoMallado FTipoMallado;       	//!< Meshing mode 1=Distance 2=Angle
	double *FLTramo;         //!< Array with the length of the parts of the pipe
	double *FDExtTramo; //!< Array with the diameter at the end of the parts of the pipe
	double *FDiametroTubo;         //!< Diametro de cada nodo de calculo
	double *FDiametroD12; //!< Diameter interpolated between two cell using the diameters average
	double *FDiametroS12; //!< Diameter interpolated between two cell using the section average
	double *FArea;				    //!< Array with the cell sections.
	double *FArea12;			//!< Array with the sections between two cells
	double *FDerLin;
	double *FDerLin12;
	double *FDerLinArea;
	double *FDerLinArea12;
	bool FIntercooler;           //!< The pipe is repited to form a set of pipes
	bool FConcentrico;        //!< Does the pipe form part of a concentric duct?
	int FNumeroConductos;       //!< Number of similar pipes that form the set

	// Propiedades termicas y friccion

	double FFriccion;              //!< Friction of the pipe
	nmTipoTransCal FTipoTransCal;     		//!< Type of heat transfer
	double FCoefAjusFric;          //!< Friction multiplier
	double FCoefAjusTC;            //!< Heat transfer multiplier
	double FEspesorIntPrin; //!< Inner thickness of the main layer of the pipe wall
	double FEspesorExtPrin; //!< Outer thickness of the main layer of the pipe wall
	double FEspesorPrin;        //!< Thicknes of the main layer of the pipe wall
	double FDensidadPrin; //!< Density of the material of the main layer of the pipe wall
	double FCalEspPrin; //!< Specific heat of the material of the main layer of the pipe wall
	double FConductPrin; //!< Conductivity of the material of the main layer of the pipe wall
	double FTRefrigerante;         //!< Coolant temperature
	nmRefrigerante FTipRefrig;        		//!< Type of coolant
	int FTctpt;                 //!< Type of wall temperature calculation
	nmTipoCalcTempParedTubos FTipoCalcTempPared; //!< Type of wall temperature calculation
	double FTIniParedTub;          //!< Initial wall temperatura
	double *FVelPro; //!< Array with the temporal average of the gas velocity at each cell
	double *FCoefTurbulencia;      //!< Turbulence coefficient in exhaust pipes
	double ***FSUMTPTuboPro;       //!< Summation of wall temperature
	double FSUMTime;				//!< Summation of the time steps
	double **FTPTubo;            //!< Wall temperature in the current time steps
	double **FTParedAnt;        //!< Wall temperature in the previous time steps
	double FTExt;	                //!< External temperature
	//double FAlpha;
	double FCoefExt;	//!< Heat transfer coeficient multiplier at outer pipe wall
	double *Fhi;                   //!< Inner heat trasnfer coeficient
	double *Fhe;                   //!< Outer heat transfer coeficient
	double *Frho;                  //!< Gas density at each cell
	double *FRe;                   //!< Reynolds number at each cell
	stCapa *FCapa;                 //!< Array with the layers of the pipe wall
	int FNumCapas;              //!< Number of layers
	double FEmisividad;            //!< Emisivity of the pipe wall
	double *FResistRadInt; //!< Inner radial thermal resistance of the pipe wall
	double *FResistRadExt; //!< Outer radial thermal resistance of the pipe wall
	double *FResistAxiAnt; //!< Previus axial thermal resistance of the pipe wall
	double *FResistAxiPos;   //!< Next axial thermal resistance of the pipe wall
	double *FCapInt;               //!< Inner thermal capacity
	double *FCapMed;               //!< Thermal capacity at the middle
	double *FCapExt;               //!< Outer thermal capacity

	// Propiedades termofluidodinamicas

	double FVelMedia;             //!< Average gas velocity along the entry pipe
	double FTini;                  //!< Initial gas temperature
	double FPini;                  //!< Initial pressure
	double *FPresion0;             //!< Pressure at the current time step
	double *FAsonido0; //!< Adimensionalized speed of sound at the current time step
	double *FVelocidad0; //!< Adimensionalized gas velocity at the current time step
	double *FPresion1;             //!< Pressure at the new time step
	double *FAsonido1; //!< Adimensionalized speed of sound at the new time step
	double *FVelocidad1; //!< Adimensionalized gas velocity at the new time step
	double *FVelocidadDim;			//!< Gas velocity at the nodes
	double *FAsonidoDim;			//!< Speed of sound at the nodes
	double *FTemperature;			//!< Gas temperature at the nodes
	double *FFlowMass;				//!< Air mass flow at the nodes

	// Vectores metodo de calculo

	stModeloCalculoTubos FMod;				//!< Struct with the options of the numerical scheme used to solve the duct
	double **FU0;					//!< State vector at the current time
	double **FU0Sum;				//!< Sumatory of the state vector (not used)
	double **FU0Medio;				//!< Average of the state vector (not used)
	double *FVelocidadMedia;		//!< Average gas velocity (not used)
	double *FAsonidoMedia;			//!< Average speed of sound (not used)
	double *FPresionMedia;			//!< Average presseure (not used)
	double **FU1;					//!< State vector at the following time step
	double **FUt;					//!< Variation of the state vector versus time (Used with CE-SE)
	double **FU12;				//!< State vector at the intermediate time step
	double **FW;					//!< Flux vector
	double **FV1;	//!< Source vector (related with cross section variation)
	double **FV2;	//!< Source vector (related with friction and heat transfer)
	double FCcese;					//!< CE-SE constant

	// Vectores del flux corrected transport

	double **FUfct0;			//!< State vector input for the FCT techniques
	double **FUfct1;			//!< State vector output for the FCT techniques
	double **FUfctd;				//!< Difused state vector
	double **FUfctad;				//!< Antidifused state vector
	double **Ffl;					//!< FCT intermediate parameter
	double **FdU;					//!< FCT intermediate parameter
	double **FDeltaFCTd;			//!< FCT intermediate parameter
	double **FflU;					//!< FCT intermediate parameter
	double **FaU;					//!< FCT intermediate parameter

	// Vectores de TVD Davis

	//double **FTVD;
	//double **FTVDdU;
	//double *FTVDpp;
	//double *FTVDpn;
	//double *FTVDphp;
	//double *FTVDphn;
	//double *FTVDrp;
	//double *FTVDrn;
	//double *FTVDGn;
	//double *FTVDGp;

	// Variables del TVD

	stTVD FTVD;		//!< Struct with the intermediate parameters of TVD scheme
	double *sqrtRhoA;

	// Calculo Especies

	int FNumeroEspecies;		//!< Number of chemical species considered
	nmTipoCalculoEspecies FCalculoEspecies; 	//!< Species calculation mode
	nmCalculoGamma FCalculoGamma;	//!< Specific heat ratio calculation mode
	double *FComposicionInicial; 	//!< Initial composition
	double **FFraccionMasicaEspecie; //!< Mass fraction of each chemical specie at every cell
	double **FFraccionMasicaCC;	//!< Mass fraction of each chemical specie at boundaries
	bool FHayEGR;				//!< The EGR specie is considered?
	int FIntEGR;	//!< Integer equal to 1 if EGR is considered else equal to 0

	int FNumEcuaciones;	//!< Total number of conservation equation (including species mass conservation)

	double *FVelocidadCC;			//!< Gas velocity at boundaries
	double *FDensidadCC;			//!< Gas density at boundaries
	double *FAreaCC;				//!< Section at boundaries
	double *FGamma;			//!< Specific heat ratio at every calculation cell.
	double *FRMezcla;		//!< Perfect gas constant at every calculation cell.
	double *FCpMezcla;		//!< Specific heat at constant pressure at every calculation cell.
	double *FCvMezcla; //!< Specific heat at constant volume at every calculation cell.
	double FGammaN;

	double *FGamma1;				//!< Value dependent of specific heat ratio
	double *FGamma3;				//!< Value dependent of specific heat ratio
	double *FGamma4;				//!< Value dependent of specific heat ratio
	double *FGamma5;				//!< Value dependent of specific heat ratio
	double *FGamma6;				//!< Value dependent of specific heat ratio

	// Tiempos

//        double 						FTime0;					//!< Time at current time step
//        double 						FTime1; 				//!< Time at following time step
//        double 						FDeltaTime; 			//!< Time step
	double *FCourantLocal;	//!< Local Courant number at every calculation cell
	double FCourant;				//!< Courant number

	// Resultados medios e instantaneos

	stResMediosTubo *ResultadosMedios; 		//!< Struct with the average results
	int FNumResMedios;			//!< Number of average results selected
	stResInstantTubo *ResultInstantaneos;			//!< Struct with the instantaneous results
	int FNumResInstant;			//!< Number of instantaneous results selected
	double FTiempoMedSUM;	//!< Time acumulation to obtain the average results
	double FControlResMed;	//!< Parameter to control when the average is calculated
	stSensoresTubo *Sensor;				//!< (not used)
	int FNumDistSensores;		//!< (not used)

	double FAnguloTotalCiclo;		//!< Total angle of the engine cycle
	double FRegimenFicticio;	//!< Ficticios engine speed to use when there is not an engine in the model calculated.

	int FCicloTubo; 			//!< Number of the current cycle
	int FCicloActual; // !< Number of the current cycle when there is not engine in the configuration
	double FDuracionCiclo; // !< Cycle duration (in time) to calculate wall temperature convergence when there is not engine in the configuration
	int FNumCiclosSinInerciaTermica; // !< Number of cycles without thermal inertia consideration when there is not engine in the configuration

	TDPF *FDPFSalidaTubo;  		//  DPF connected to the outlet of the duct
	TDPF *FDPFEntradaTubo;       //  DPF connected to the inlet of the duct
	bool FHayDPFNodoIzq; //  bool to identify DPF connected at the inlet of the duct
	bool FHayDPFNodoDer; //  bool to identify DPF connected at the outlet of the duct
	int FNodoDPFEntrada;  //  Node of the DPF connected to the inlet of the duct
	int FNodoDPFSalida;  //  Node of the DPF connected to the outlet of the duct
	int *FTipoCanal; //  bool to identify DPF connected at the outlet of the duct

//---------------------------------------------------------------------------
//          FUNCIONES PRIVADAS
//---------------------------------------------------------------------------

	/*! Generate the mesh in the pipes */
	void CalculoPuntosMalla(double ene						//!< Engine speed
						   );

	//double InterpolaTubo(double  vizq,double  vder,double  axid,double  xif);

	/*! Transfor from the gas velocity, speed of sound and pressure to state vector */
	void Transforma1(const double& v,						//!< Gas velocity
					 const double& a,								//!< Speed of sound
					 const double& p,								//!< Pressure
					 double** U,								//!< State vector
					 const double& Gamma,						//!< Specific heat ratio
					 const double& Gamma1,					//!< f(Specific heat ratio)
					 double* Yespecie,		//!< Array with the specie mass fractions
					 const int& i								//!< Cell index
					);

	/*! Transfor from the gas velocity, speed of sound and pressure to state vector (including the cross section in the state vector) */
	void Transforma1Area(const double& v,					//!< Gas velocity
						 const double& a,								//!< Speed of sound
						 const double& p,								//!< Pressure
						 double** U,								//!< State vector
						 const double& area,							//!< Cross section
						 const double& Gamma,						//!< Specific heat ratio
						 const double& Gamma1,					//!< f(Specific heat ratio)
						 double* Yespecie,		//!< Array with the specie mass fractions
						 const int& i								//!< Cell index
						);

	/*! Transfor from the state vector to gas  velocity, speed of sound and pressure */
	void Transforma2(double& v,								//!< Gas velocity
					 double& a,								//!< Speed of sound
					 double& p,								//!< Pressure
					 double** U,								//!< State vector
					 const double& Gamma,						//!< Specific heat ratio
					 const double& Gamma1,					//!< f(Specific heat ratio)
					 double* Yespecie,			//!< Array with the specie mass fraction
					 const int& i								//!< Cell index
					);

	/*! Transfor from the state vector to gas  velocity, speed of sound and pressure */
	void Transforma2Area(double& v,							//!< Gas velocity
						 double& a,								//!< Comment
						 double& p,								//!< Comment
						 double** U,								//!< Comment
						 const double& area,							//!< Comment
						 const double& Gamma,							//!< Comment
						 const double& Gamma1,							//!< Comment
						 double* Yespecie,						//!< Comment
						 const int& i								//!< Comment
						);

	/*! Comment */
	void Transforma3Area(double** Ufct,								//!< Comment
						 double** U,									//!< Comment
						 double Area,								//!< Comment
						 double Gamma,								//!< Comment
						 double Gamma1,								//!< Comment
						 double Gamma6,								//!< Comment
						 int i									//!< Comment
						);

	/*! Comment */
	void Transforma4Area(double** U1,		//!< Comment
						 double** Ufctd,	//!< Comment
						 double Area,	//!< Comment
						 double Gamma,	//!< Comment
						 double Gamma1,	//!< Comment
						 double Gamma3,	//!< Comment
						 double Gamma4,	//!< Comment
						 double Gamma6,	//!< Comment
						 int i		//!< Comment
						);

	/*! Comment */
	double Maximo(double x,	//!< Comment
				  double y	//!< Comment
				 );

	/*! Comment */
	double Minimo(double x,	//!< Comment
				  double y	//!< Comment
				 );

	/*! Comment */
	void LaxWendroff();

	/*! Comment */
	void LaxWendroffArea();

	/*! Comment */
	void FluxCorrectedTransport();

	/*! Comment */
	void CalculaFlujo(double** U,		//!< Comment
					  double** W,		//!< Comment
					  double* Gamma,	//!< Comment
					  double* Gamma1,	//!< Comment
					  int Nodos	//!< Comment
					 );

	/*! Comment */
	void CalculaFuente1(double** U,		//!< Comment
						double** V1,		//!< Comment
						double* Gamma,	//!< Comment
						double* Gamma1,	//!< Comment
						int Nodos	//!< Comment
					   );

	/*! Comment */
	void CalculaFuente1Area(double** U,		//!< Comment
							double** V1,		//!< Comment
							double* Area,	//!< Comment
							double* Gamma1,	//!< Comment
							int Nodos	//!< Comment
						   );

	/*! Comment */
	void CalculaFuente2(double** U,					//!< Comment
						double** V2,					//!< Comment
						double* Area,				//!< Comment
						double* hi,					//!< Comment
						double* rho,				//!< Comment
						double* Re,					//!< Comment
						double* TempParedTubo,		//!< Comment
						double* Gamma,				//!< Comment
						double* Rmezcla,			//!< Comment
						double* Gamma1,				//!< Comment
						int Nodos				//!< Comment
					   );

	/*! Comment */
	void CalculaFuente2Area(double** U,					//!< Comment
							double** V2,					//!< Comment
							double* Area,				//!< Comment
							double* hi,					//!< Comment
							double* rho,				//!< Comment
							double* Re,					//!< Comment
							double* TempParedTubo,		//!< Comment
							double* Gamma,				//!< Comment
							double* Rmezcla,			//!< Comment
							double* Gamma1,				//!< Comment
							int Nodos				//!< Comment
						   );

	/*! Comment */
	double DerLinF(double d1,	//!< Comment
				   double d2,	//!< Comment
				   double xref	//!< Comment
				  );

	/*! Comment */
	double DerLinFArea(double area1,	//!< Comment
					   double area2,	//!< Comment
					   double xref		//!< Comment
					  );

	/*! Comment */
	void Colebrook(double rug,	//!< Comment
				   double dia,	//!< Comment
				   double& f,		//!< Comment
				   double Re		//!< Comment
				  );

	/*! Comment */
	void TransmisionCalor(double tgas,				//!< Comment
						  double diametro,			//!< Comment
						  double& q,					//!< Comment
						  double hi,					//!< Comment
						  double rho,				//!< Comment
						  double Tw					//!< Comment
						 );

	/*! Comment */
	void TransformaContorno(double& L,		//!< Comment
							double& B,		//!< Comment
							double& E,		//!< Comment
							double& a,		//!< Comment
							double& v,		//!< Comment
							double& p,		//!< Comment
							const int& modo,	//!< Comment
							const double& Gamma1,	//!< Comment
							const double& Gamma3,	//!< Comment
							const double& Gamma4,	//!< Comment
							const double& Gamma5	//!< Comment
						   );

	/*! Comment */
	double CalculaNIT(double a,		//!< Comment
					  double v,		//!< Comment
					  double p,		//!< Comment
					  double d,		//!< Comment
					  double Gamma, double Rmezcla);

	//void FuncionTVD();

	/*! Comment */
	void Calculo_Entropia(double& entropia,		//!< Comment
						  double& velocidadp,		//!< Comment
						  int ind,			//!< Comment
						  double dist,			//!< Comment
						  int signo,			//!< Comment
						  double DeltaTiempo,	//!< Comment
						  int indiceCC		//!< Comment
						 );

	/*! Comment */
	void Calculo_Caracteristica(double& caracteristica,	//!< Comment
								double& velocidadp,		//!< Comment
								double& asonidop,		//!< Comment
								int ind,			//!< Comment
								double dist,			//!< Comment
								int signo,			//!< Comment
								double entropia,		//!< Comment
								double DeltaTiempo		//!< Comment
							   );

	/*! Comment */
	void CalculaB();

	/*! Comment */
	void CalculaBmen();

	/*! Comment */
	void CalculaBmas();

	/*! Comment */
	void CalculaMatrizJacobiana();

	/*! Comment */
	void TVD_Estabilidad();

	/*! Comment */
	void TVD_Limitador();

	void RoeConstants();

	/*! Comment */
	void DimensionaTVD();

	/*! Comment */
	double Limita(double r	//!< Comment
				 );

  public:
//---------------------------------------------------------------------------
//          VARIABLES PUBLICAS
//---------------------------------------------------------------------------

	/**
	 * @brief Gets the pipe id.
	 *
	 * Gets the current pipe id.
	 *
	 * @return Pipe id.
	 */
	int getNumeroTubo() const;

	/**
	 * @brief Gets the left-hand side node.
	 *
	 * Gets the current pipe left-hand side node.
	 *
	 * @return Left-hand side node.
	 */
	int getNodoIzq() const;

	/**
	 * @brief Gets the right-hand side node.
	 *
	 * Gets the current pipe right-hand side node.
	 *
	 * @return Right-hand side node.
	 */
	int getNodoDer() const;

	int getNin() {
		return FNin;
	}
	;
	int getNumeroConductos() {
		return FNumeroConductos;
	}
	;
	double getXRef() {
		return FXref;
	}
	;

	/**
	 * @brief Gets the total length of the pipe.
	 *
	 * Gets the total length of the pipe.
	 *
	 * @return Total length of the pipe.
	 */
	double getLongitudTotal() const;

	nmTipoTransCal getTipoTransCal() {
		return FTipoTransCal;
	}
	;
	double getCoefAjustTC() {
		return FCoefAjusTC;
	}
	;
	double getCoefAjustFric() {
		return FCoefAjusFric;
	}
	;
	double getFriccion() {
		return FFriccion;
	}
	;
	double getEmisividad() {
		return FEmisividad;
	}
	;
	bool getConcentrico() {
		return FConcentrico;
	}
	;
	double getDuracionCiclo() {
		return FDuracionCiclo;
	}
	;
	double getNumCiclosSinInerciaTermica() {
		return FNumCiclosSinInerciaTermica;
	}
	;
	nmTipoCalcTempParedTubos getTipoCalcTempPared() {
		return FTipoCalcTempPared;
	}
	;
	double getAnguloTotalCiclo() {
		return FAnguloTotalCiclo;
	}
	;
	double getTExt() {
		return FTExt;
	}
	;
	double getEspesorIntPrin() {
		return FEspesorIntPrin;
	}
	;
	double getConductPrin() {
		return FConductPrin;
	}
	;
	double getEspesorPrin() {
		return FEspesorPrin;
	}
	;
	double getDensidadPrin() {
		return FDensidadPrin;
	}
	;
	double getCalEspPrin() {
		return FCalEspPrin;
	}
	;
	bool getHayDPFNodoDer() {
		return FHayDPFNodoDer;
	}
	;
	bool getHayDPFNodoIzq() {
		return FHayDPFNodoIzq;
	}
	;
	int getNumCapas() {
		return FNumCapas;
	}
	; /*! Return the number of wall layer */
	TDPF* getDPFEntrada() {
		return FDPFEntradaTubo;
	}
	;
	TDPF* getDPFSalida() {
		return FDPFSalidaTubo;
	}
	;
	int getNodoDPFEntrada() {
		return FNodoDPFEntrada;
	}
	;
	int getNodoDPFSalida() {
		return FNodoDPFSalida;
	}
	;

	double FTime0; //!< Time at current time step

	/**
	 * @brief Gets the current time.
	 *
	 * Gets the current time.
	 *
	 * @return Current time. [s]
	 */
	double getTime0() const;

	/**
	 * @brief Sets the current time.
	 *
	 * Sets the current time.
	 *
	 * @param valor Current time. [s]
	 */
	void PutTime0(double valor);

	double FTime1; //!< Time at following time step

	/**
	 * @brief Gets the time at the following time-step.
	 *
	 * Gets the time at the following time-step.
	 *
	 * @return Time after the following time-step. [s]
	 */
	double getTime1() const;

	/**
	 * @brief Sets the time after the following time-step.
	 *
	 * Sets the time after the following time-step.
	 *
	 * @param valor Time after the following time-step. [s]
	 */
	void PutTime1(double valor);

	double FDeltaTime; //!< Time step

	/**
	 * @brief Gets the time step.
	 *
	 * Gets the current time step.
	 *
	 * @return Time step. [s]
	 */
	double getDeltaTime() const;

	/**
	 * @brief Sets the time step.
	 *
	 * Sets the current time step.
	 *
	 * @param valor Time-step. [s]
	 */
	void PutDeltaTime(double valor);

	/**
	 * @brief Gets the mesh size.
	 *
	 * Gets the mesh size.
	 *
	 * @return Mesh size. [m]
	 */
	double getMallado() const;

	/**
	 * @brief Gets the density.
	 *
	 * Gets the density at a given cell.
	 *
	 * @param i Cell.
	 * @return Density. [kg / (m ** 3)]
	 */
	double GetDensidad(int i) const;

	/**
	 * @brief Gets the cross section at a given cell.
	 *
	 * Gets the cross section at a given cell.
	 *
	 * @param i Cell.
	 * @return Cross section. [m ** 2]
	 */
	double GetArea(int i) const;

	/**
	 * @brief Gets the cell diameter.
	 *
	 * Gets the diameter at a given cell.
	 *
	 * @param i Cell.
	 * @return Diameter. [m]
	 */
	double GetDiametro(int i) const;

	/**
	 * @brief Gets the fluid speed.
	 *
	 * Gets the fluid speed at a given cell.
	 *
	 * @param i Cell.
	 * @return Speed. [m / s]
	 */
	double GetVelocidad(int i) const;

	/**
	 * @brief Gets the speed of sound.
	 *
	 * Gets the speed of sound at a given cell.
	 *
	 * @param i Cell.
	 * @return Speed of sound. [m / s]
	 */
	double GetAsonido(int i) const;

	/**
	 * @brief Gets the fluid pressure.
	 *
	 * Gets the fluid pressure at a given cell.
	 *
	 * @param i Cell.
	 * @return Fluid pressure. [m / s]
	 */
	double GetPresion(int i) const;

	/**
	 * @brief Gets the wall temperature at a given cell and node.
	 *
	 * Gets the wall temperature at a given cell and node.
	 *
	 * @param j Node.
	 * @param i Cell.
	 * @return Wall temperature.
	 */
	double GetTPTubo(int j, int i) const;

	/**
	 * @brief Sets the wall temperature at a given cell and node.
	 *
	 * Sets the wall temperature at a given cell and node.
	 *
	 * @param k Node.
	 * @param i Cell.
	 * @param valor Wall temperature.
	 */
	void PutTPTubo(int k, int i, double valor);

	/**
	 * @brief Gets the previous wall temperature at a given cell and node.
	 *
	 * Gets the previous wall temperature at a given cell and node.
	 *
	 * @param j Node.
	 * @param i Cell.
	 * @return Wall temperature.
	 */
	double GetTPTuboAnt(int j, int) const;

	/**
	 * @brief Gets the integrated gas velocity.
	 *
	 * Gets the integrated gas velocity at a given cell.
	 *
	 * @param i Cell.
	 * @return Integrated gas velocity. [m / s]
	 */
	double GetVelPro(int i) const;

	/**
	 * @brief Sets the integrated gas velocity.
	 *
	 * Sets the integrated gas velocity at a given cell.
	 *
	 * @param i Cell.
	 * @param valor Integrated gas velocity. [m / s]
	 */
	void PutVelPro(int i, double valor);

	/**
	 * @brief Gets the turbulence coefficient.
	 *
	 * Gets the turbulence coefficient at a given cell.
	 *
	 * @param i Cell.
	 * @return Turbulence coefficient.
	 */
	double GetCoefTurbulencia(int i) const;

	/**
	 * @brief Gets the initial temperature.
	 *
	 * Gets the initial temperature.
	 *
	 * @return Initial temperature.
	 */
	double getTemperaturaInicial() const;

	/**
	 * @brief Gets the initial temperature of the wall.
	 *
	 * Gets the initial temperature of the wall.
	 *
	 * @return Initial temperature of the wall.
	 */
	double getTempWallIni() const;

	/**
	 * @brief Gets the initial pressure.
	 *
	 * Gets the initial pressure.
	 *
	 * @return Initial pressure.
	 */
	double getPresionInicial() const;

	/**
	 * @brief Gets the mean speed.
	 *
	 * Gets the mean speed.
	 *
	 * @return Mean speed. [m / s]
	 */
	double getVelocidadMedia() const;

	/**
	 * @brief Gets the initial mass fraction of species i.
	 *
	 * Gets the initial mass fraction of species i.
	 *
	 * @param i Species id.
	 * @return Mass fraction.
	 */
	double GetFraccionMasicaInicial(int i) const;

	/*! Return the specie i mass fraction at boundary j */
	double GetFraccionMasicaCC(int j,						//!< Boundary index
							   int i											//!< Specie index
							  ) {
		return FFraccionMasicaCC[j][i];
	}
	;

	/*! Return the gas velocity at boundary i */
	double GetVelocidadCC(int i								//!< Boundary index
						 ) {
		return FVelocidadCC[i];
	}
	;

	/*! Return the gas density at boudary i */
	double GetDensidadCC(int i								//!< Boundary index
						) {
		return FDensidadCC[i];
	}
	;

	/*! Return the cross section at boundary i  */
	double GetAreaCC(int i									//!< Boundary index
					) {
		return FAreaCC[i];
	}
	;

	/**
	 * @brief Gets the specific heat capacities ratio at a given cell.
	 *
	 * @param i Cell.
	 * @return Specific heat capacities ratio.
	 */
	double GetGamma(int i) const;

	/**
	 * @brief Gets the gas constant of the mixture at a given cell.
	 *
	 * @param i Cell.
	 * @return Gas constant. [J / (kg * K)]
	 */
	double GetRMezcla(int i) const;

	/**
	 * @brief Gets the specific heat capacity at constant pressure at a given
	 * cell.
	 *
	 * @param i Cell.
	 * @return Specific heat capacity at constant pressure. [J / (kg * K)]
	 */
	double GetCpMezcla(int i) const;

	/**
	 * @brief Gets the specific heat capacity at constant volume at a given
	 * cell.
	 *
	 * @param i Cell.
	 * @return Specific heat capacity at constant volume. [J / (kg * K)]
	 */
	double GetCvMezcla(int i) const;

	nmFormulacionLeyes getFormulacionLeyes() {
		return FMod.FormulacionLeyes;
	}
	;
	/*! Return the valuve of the component i of the state vector at the cell j */
	double GetU0(int i,										//!< Equation index
				 int j											//!< Cell index
				) {
		return FU0[i][j];
	}
	;

	/*! Return the heat transfer coefficient at cell i */

	double Gethi(int i											//!< Cell index
				) {
		return Fhi[i];
	}
	;

	/*! Return the external heat transfer coefficient at cell i */

	double Gethe(int i											//!< Cell index
				) {
		return Fhe[i];
	}
	;

	/*! Return the thermal proporties in layer i */

	stCapa GetCapa(int i) {
		return FCapa[i];
	}
	;

	/*! Return the type of monolith channel connected to the duct end */

	int GetTipoCanal(int i) {
		return FTipoCanal[i];
	}
	;

	void PutConcentric(double valor) {
		valor == 0 ? FConcentrico = false : FConcentrico = true;
	}
	;

//---------------------------------------------------------------------------
//          FUNCIONES PUBLICAS
//---------------------------------------------------------------------------

	/*! Contructor of the pipe class */
	TTubo(int SpeciesNumber, //!< Number of different species considered in the simulation.
		  int j,										//!< Index of the pipe
		  double SimulationDuration,								//!< Duration of the simulation. If there is not engine.
		  TBloqueMotor **Engine,			//!< Pointer to the engine object
		  nmTipoCalculoEspecies SpeciesModel,	//!< Type of species calculation
		  nmCalculoGamma GammaCalculation,	//!< Type of specific heat calculation
		  bool ThereIsEGR						//!< EGR specie is considered?
		 );

	/*! Destructor of the pipe class*/
	~TTubo();

	/*!Comment  */
	void LeeDatosGeneralesTubo(const char*FileWAM, 					//!< Comment
							   fpos_t&filepos 						//!< Comment
							  );

	/*!Comment  */
	void LeeDatosGeometricosTubo(const char* FileWAM,          //!< Comment
								 fpos_t& filepos,          //!< Comment
								 double ene,              //!< Comment
								 int tipomallado,      //!< Comment
								 TBloqueMotor ** Engine            //!< Comment
								);

	/*!Comment  */
	void IniciaVariablesFundamentalesTubo();

	/*!Comment  */
	void IniciaVariablesTransmisionCalor(TCondicionContorno ** BC,  //!< Comment
										 TBloqueMotor ** Engine,     //!< Comment
										 double AmbientTemperature        //!< Comment
										);

	/*!Comment  */
	void EstabilidadMetodoCalculo();

	/*!Comment  */
	void CalculaVariablesFundamentales();

	/*!Comment  */
	void ActualizaValoresNuevos(TCondicionContorno ** BC         //!< Comment
							   );

	/*!Comment  */
	void ReduccionFlujoSubsonico();

	/*!Comment  */
	void ReduccionFlujoSubsonicoFCT();

	/*!Comment  */
	void ReadAverageResultsTubo(const char* FileWAM,                //!< Comment
								fpos_t& filepos,                //!< Comment
								bool HayMotor                //!< Comment
							   );

	/*!Comment  */
	void HeaderAverageResults(std::stringstream& medoutput,        //!< Comment
							  stEspecies* DatosEspecies     //!< Comment
							 ) const;

	/*!Comment  */
	void ImprimeResultadosMedios(std::stringstream& medoutput       //!< Comment
								) const;

	/*!Comment  */
	void ReadInstantaneousResultsTubo(const char* FileWAM,          //!< Comment
									  fpos_t& filepos,                //!< Comment
									  bool HayMotor                //!< Comment
									 );

	/*!Comment  */
	void HeaderInstantaneousResults(std::stringstream& insoutput,   //!< Comment
									stEspecies* DatosEspecies     //!< Comment
								   ) const;

	/*!Comment  */
	void ImprimeResultadosInstantaneos(std::stringstream& insoutput //!< Comment
									  ) const;

	/*!Comment  */
	void CalculaResultadosMedios(double Theta            //!< Comment
								);

	/*!Comment  */
	void CalculaResultadosInstantaneos();

	/*!Comment  */
	void SalidaGeneralTubos(stEspecies* DatosEspecies      //!< Comment
						   ) const;

	/*!Comment  */
	void CalculaTemperaturaPared(TBloqueMotor ** Engine,     //!< Comment
								 double Theta,      //!< Comment
								 double CrankAngle,     //!< Comment
								 TCondicionContorno ** BC         //!< Comment
								);

	void CalculaTemperaturaParedSinMotor(TCondicionContorno **BC);

	/*!Comment  */
	void AjustaPaso(double Intervalo                //!< Comment
				   );

	/*!Comment  */
	void CalculaCaracteristicasExtremos(TCondicionContorno ** BC,   //!< Comment
										double DeltaTiempo //!< Comment
									   );

	/*!Comment  */
	double Interpola_Entropia(nmPipeEnd TipoExtremoTubo,    //!< Comment
							  double DeltaTiempo         //!< Comment
							 );

	/*!Comment  */
	double Interpola_Caracteristica(double entropia,                //!< Comment
									int signo,                    //!< Comment
									int extremo,                  //!< Comment
									double DeltaTiempo               //!< Comment
								   );

	/*!Comment  */
	void ComunicacionTubo_CC(TCondicionContorno ** BC         //!< Comment
							);

	/*!Comment  */
	void ComunicacionDPF(TCondicionContorno **CC,  //!< Comment
						 TDeposito **Deposito                        //!< Comment
						);

	/*!Comment  */
	void InicializaCaracteristicas(TCondicionContorno ** BC         //!< Comment
								  );

	/*!Comment  */
	void CalculaCoeficientePeliculaExterior(TBloqueMotor ** Engine, //!< Comment
											double AmbientPressure,             //!< Comment
											double AmbientTemperature              //!< Comment
										   );

	/*!Comment  */
	void CalculaResistenciasdePared(TCondicionContorno ** BC        //!< Comment
								   );

	/*!Comment  */
	void CalculaCoeficientePeliculaInterior(TCondicionContorno ** BC //!< Comment
										   );

	/*!Comment  */
	void ActualizaPropiedadesGas();

	/*! Return the value of the of the specie i mass fraction at cell j */
	double GetFraccionMasica(int j,								//!< Cell index
							 int i											//!< Specie index
							) {
		return FFraccionMasicaEspecie[j][i];
	}
	;
};

//---------------------------------------------------------------------------
#endif
