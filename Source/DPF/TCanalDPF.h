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

//---------------------------------------------------------------------------
#ifndef TCanalDPFH
#define TCanalDPFH
#include <cstdio>

#include "Constantes.h"
#include "Globales.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TDPF;
class TCondicionContorno;
class TBloqueMotor;

class TCanalDPF {
  private:
//---------------------------------------------------------------------------
//          VARIABLES PRIVADAS
//---------------------------------------------------------------------------

	int FTuboCCNodoIzq; // Guarda la posicion en la estructura de TuboExtremo del tubo para el NodoIzquierdo
	int FTuboCCNodoDer; // Guarda la posicion en la estructura de TuboExtremo del tubo para el NodoIzquierdo

	// Datos Geometricos

	int FContador;
	int FNumDPF;
	int FNumeroHaz;           // Numero de Haz (discretizacion radial de la DPF"
	int FNodoIzq;                     // Nodo Izquierdo
	int FNodoDer;                     // Nodo Derecho
	int FNin;                         // Numero de nodos
	double FXref;                     // Incremento espacial del mallado
	double FLongitudTotal;            // Longitud total del conducto
	double FMallado;                  // Mallado inicial
	int FNTramos;                     // Numero de tramos del tubo
	nmTipoMallado FTipoMallado;       // Tipo de mallado 1=Distancia 2=Angular
	double *FLTramo;                  // Longitud Tramo
	double *FDExtTramo;               // Diametro extremo tramo
	double *FDiametroTubo;            // Diametro de cada nodo de calculo
	double *FDiametroD12;             // Diametro intermedio medias diametros
	double *FDiametroS12;             // Diametro intermedio medias areas
	double *FArea;
	double *FArea12;
	double *FDerLin;
	double *FDerLin12;
	double *FDerLinArea;
	double *FDerLinArea12;
	bool FIntercooler;

	// Propiedades termicas y friccion

	//double FFriccion;                 // Friccion del conducto
	nmTipoTransCal FTipoTransCal; // Tipo de transmision de calor PIPA = 3, ESC = 2, ADM = 1
	double FCoefAjusFric;             // Coeficiente Ajuste valor de friccion
	double FTIniParedTub;             // Temperatura inicial pared tubo
	double *FCoefTurbulencia;      // Coeficiente de turbulencia tubos de escape
	double *FTPared;    // Temperatura de pared por nodo de tubo y nodo de pared
	double **FTPTubo;            // Sumatorio temperaturas de pared de los tubos
	double FTExt;
	double FCoefExt;
	double *Fhi;               // Coeficiente de pelicula interior en cada nodo.
	double *Fhe;               // Coeficiente de pelicula exterior en cada nodo.
	double *Frho;                     // Densidad del gas en cada nodo.
	double *FRe;                      // Reynolds del gas en cada nodo.
	double *FVelPro;

	// Propiedades termofluidodinamicas

	double FVelMedia;                 // Velocidad media a lo largo del conducto
	double FTini;                     // Temperatura inicial gas
	double FPini;                     // Presion inicial del gas
	double *FPresion0;                // Presion instante de calculo
	double *FAsonido0;               // Velocidad del sonido instante de calculo
	double *FVelocidad0;              // Velocidad instante de calculo
	double *FPresion1;                // Presion instante anterior
	double *FAsonido1;                // Velocidad del sonido instante anterior
	double *FVelocidad1;              // Velocidad instente anterior

	// Vectores metodo de cqlculo

	stModeloCalculoTubos FMod;
	double **FU0;                     // Vector solucion instante de calculo
	double **FU1;                     // Vector solucion instante posterior
	double **FUt; // Variacion del vector solucion con respecto al tiempo (solo CE-SE)
	double **FU12;             // Vector solucion instante de calculo intermedio
	double **FW;                      // Vector de flujo
	double **FV1;       // Termino fuente dependiente de la variacion de seccion
	double **FV2;                    // Termino fuente (Friccion - Trans. Calor)
	double FCcese;

	// Vectores del flux corrected transport

	double **FUfct0;
	double **FUfct1;
	double **FUfctd;
	double **FUfctad;
	double **Ffl;
	double **FdU;
	double **FDeltaFCTd;
	double **FflU;
	double **FaU;

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

	stTVD FTVD;
	double *sqrtRhoA;

	// Calculo Especies

	int FNumeroEspecies;               // Numero de Especies
	nmTipoCalculoEspecies FCalculoEspecies; // Calculo con 9 o 3 especies quimicas
	nmCalculoGamma FCalculoGamma;
	double *FComposicionInicial;       // Composicion inicial en el tubo
	double **FFraccionMasicaEspecie; // Fraccion Masica de cada especie por nodo.
	double **FFraccionMasicaEspecie1; // Fraccion Masica de cada especie por nodo en el instante anterior.
	double **FFraccionMasicaCC; // Fraccion Masica de cada especie en el punto de partida
	// de la linea de corriente que llega a la CC.
	bool FHayEGR;
	int FIntEGR;

	int FNumEcuaciones;

	double *FVelocidadCC;
	double *FDensidadCC;
	double *FAreaCC;
	double *FGamma;
	double *FRMezcla;
	double *FCpMezcla;     // EN PRINCIPIO NO HACE FALTA
	double *FCvMezcla;     // EN PRINCIPIO NO HACE FALTA

	double *FGamma1;
	double *FGamma3;
	double *FGamma4;
	double *FGamma5;
	double *FGamma6;

	// Tiempos

	double FTime0;                     // Tiempo para el instante de calculo
	double FTime1;                     // Tiempo para el instante posterior
	double FDeltaTime;                 // Incremento de tiempo.
	double *FCourantLocal;
	double FCourant;

	// Resultados medios e instantaneos

	stResMediosTubo *ResultadosMedios; // Estructura para resultados medios en tubos
	int FNumResMedios; // Numero de puntos para los que se piden resultados medios
	stResInstantTubo *ResultInstantaneos;
	int FNumResInstant;
	double FTiempoMedSUM;
	double FControlResMed;
	stSensoresTubo *Sensor;
	int FNumDistSensores;

	double FAnguloTotalCiclo;
	double FRegimenFicticio;

	int FCicloTubo; // Controla el calculo de la Temp de pared una vez por ciclo.

	//Variables propias de la DPF

	TDPF *FDPF;
	int FNumeroParesdeCanales;
	nmTipoCanal FTipoCanal;
	int FNumeroCanal;

	int FTipoSeccionCanal;
	nmSeccionCanal FSeccionCanal;
	double *FLadoCanalTramo; // Para canales cuadrados y triangulares (suponemos todos los lados iguales)
	double *FLadoMenorCanalTramo; // Longitud del lado menor del canal, para caso de canal rectangular
	double *FLadoMayorCanalTramo; // Longitud del lado mayor del canal, para caso de canal rectangular

	double *FLadoCanal; // Para canales cuadrados y triangulares (suponemos todos los lados iguales)
	double *FLadoCanalD12;
	double *FLadoCanalS12;
	double *FH0Pared;
	double FF; // Constante de la perdida de carga por friccion en canales cuadrados F=28.454

	double *FVelocidadPared;
	double *FViscosidadDinamica;
	double *FEspesorSoot;

	int FNodoInicialFuente;
	int FNodoFinalFuente;
	int FNodoInicialFuenteCE;
	double FDistanciaInterpolacion;

	double *Fqreg; // Calor liberado durante la regeneracion en el volumen de control del nodo i.
	double *Fq_reac1; // Calor liberado durante la combustion de CO en el volumen de control del nodo i.
	double *Fq_reac2; // Calor liberado durante la combustion de HC en el volumen de control del nodo i.
	double **FTasaFraccionMasicaEspecie; // Tasa de variacion de la Fraccion Masica de cada especie por nodo.
	double **FFraccionMasicaSalida; // Fraccion masica de cada especie a la salida de la pared porosa.
	double *FRreg1; // Tasa de regeneracion termica en el nodo i del canal de salida
	double *FRreg2; // Tasa de regeneracion catalitica en el nodo i del canal de salida
	double *FEficiencia; // Eficiencia del medio poroso en el nodo i del canal de salida
	double *FSupEspecifica;  // Superficia especifica de la trampa
	double *FLongitudVC;

	double FBetaExtremoCerrado;
	double FLandaExtremoCerrado;
	double FEntropiaExtremoCerrado;

	int FNumeroDeposito;

//---------------------------------------------------------------------------
//          FUNCIONES PRIVADAS
//---------------------------------------------------------------------------

	void CalculoPuntosMalla();

	double InterpolaTubo(double vizq, double vder, double axid, double xif);

	void Transforma1Area(double v, double a, double p, double **U, double area, double Gamma, double Gamma1,
						 double *Yespecie, int i);

	void Transforma2Area(double *v, double *a, double *p, double **U, double Area, double Gamma, double Gamma1,
						 double *Yespecie, int i);

	void Transforma3Area(double **Ufct, double **U, double Area, double Gamma, double Gamma1, double Gamma6, int i);

	void Transforma4Area(double **U1, double **Ufctd, double Area, double Gamma, double Gamma1, double Gamma3,
						 double Gamma4, double Gamma6, int i);

	double Maximo(double x, double y);

	double Minimo(double x, double y);

	void LaxWendroffArea();

	void FluxCorrectedTransport();

	void CalculaFlujo(double **U, double **W, double *Gamma, double *Gamma1, int Nodos);

	void CalculaFuente1(double **U, double **V1, double *Gamma, double *Gamma1, int Nodos);

	void CalculaFuente1Area(double **U, double **V1, double *Area, double *Gamma1, int Nodos);

	void CalculaFuente2(double **U, double **V2, double *diame, double *CoefTurbulencia, double *hi, double *rho,
						double *TempParedTubo, double *Gamma, double *Rmezcla, double *Gamma1, double *LadoCanal, double *VelocidadPared,
						double *ViscosidadDinamica, double *q_reac1, double *q_reac2, double **FraccionMasicaSalida, double *H0Pared, int Nodos,
						double *EspesorSoot, double *Eficiencia, double *Rreg1, double *Rreg2, double *SupEspecifica, double *LongitudVC,
						double **FraccionMasica);

	void CalculaFuente2Area(double **U, double **V2, double *diame, double *CoefTurbulencia, double *hi, double *rho,
							double *TempParedTubo, double *Gamma, double *Rmezcla, double *Gamma1, double *LadoCanal, double *VelocidadPared,
							double *ViscosidadDinamica, double *q_reac1, double *q_reac2, double **FraccionMasicaSalida, double *H0Pared, int Nodos,
							double *Area, double *EspesorSoot, double *Eficiencia, double *Rreg1, double *Rreg2, double *SupEspecifica,
							double *LongitudVC, double **FraccionMasica);

	double DerLinF(double d1, double d2, double xref);

	double DerLinFArea(double area1, double area2, double xref);

	void Colebrook(double rug, double dia, double *f, double Re);

	void TransmisionCalor(double tgas, double lado, double *q, double CoefTurbulencia, double hi, double rho, double Tw);

	void TransformaContorno(double *L, double *B, double *E, double *a, double *v, double *p, int modo, double Gamma1,
							double Gamma3, double Gamma4, double Gamma5);

	double CalculaNIT(double a, double v, double p, double d, double Gamma, double R);

	//void FuncionTVD();

	void Calculo_Entropia(double *entropia, double *velocidadp, int ind, double dist, int signo, double DeltaTiempo,
						  int indiceCC, nmPipeEnd TipoExtremoTubo);

	void Calculo_Caracteristica(double *caracteristica, double *velocidadp, double *asonidop, int ind, double dist,
								int signo, double entropia, double DeltaTiempo, nmPipeEnd TipoExtremoTubo);

	void CalculaB();

	void CalculaBmen();

	void CalculaBmas();

	void CalculaMatrizJacobiana();

	void TVD_Estabilidad();

	void TVD_Limitador();

	void DimensionaTVD();

	void RoeConstants();

	double Limita(double r);

	void NodoTerminoFuente();

  public:
//---------------------------------------------------------------------------
//          VARIABLES PUBLICAS
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//          FUNCIONES PUBLICAS
//---------------------------------------------------------------------------

	TCanalDPF(int NumeroEspecies, int j, nmTipoCalculoEspecies CalculoEspecies, nmCalculoGamma CalculoGamma, bool HayEGR,
			  TDPF *DPF, int numerocanal, int NumeroDPF);

	~TCanalDPF();

	void LeeDatosGeneralesCanal(const char *FileWAM, fpos_t &filepos, int NodoIzquierdo, int NodoDerecho);

	void LeeDatosGeometricosCanal(const char *FileWAM, fpos_t &filepos);

	void IniciaVariablesFundamentalesCanalDPF();

	void EstabilidadMetodoCalculo();

	void CalculaVariablesFundamentales();

	void ActualizaValoresNuevos(TCondicionContorno **CC);

	void ReduccionFlujoSubsonico();

	void ReduccionFlujoSubsonicoFCT();

	void LeeResultadosMediosCanalDPF(int NumResMedios, char *FileWAM, fpos_t &filepos);

	void CabeceraResultadosMedios(std::stringstream& medoutput, stEspecies *DatosEspecies) const;

	void ImprimeResultadosMedios(std::stringstream& medoutput) const;

	void LeeResultadosInstantaneosCanalDPF(int NumResInstantaneos, char *FileWAM, fpos_t &filepos);

	void CabeceraResultadosInstantaneos(std::stringstream& insoutput, stEspecies *DatosEspecies) const;

	void ImprimeResultadosInstantaneos(std::stringstream& insoutput) const;

	void CalculaResultadosMedios(double theta);

	void CalculaResultadosInstantaneos();

	void SalidaGeneralTubos(stEspecies *DatosEspecies) const;

	void AjustaPaso(double Intervalo);

	void CalculaCaracteristicasExtremos(TCondicionContorno **CC, double DeltaTiempo);

	double Interpola_Entropia(nmPipeEnd TipoExtremoTubo, double DeltaTiempo);

	double Interpola_Caracteristica(double entropia, int signo, int extremo, double DeltaTiempo, nmPipeEnd TipoExtremoTubo);

	void ComunicacionCanal_CC(TCondicionContorno **CC);

	void InicializaCaracteristicas(TCondicionContorno **CC);

	void ActualizaPropiedadesGas();

	void CalculaCoeficientePeliculaExterior(double pamb);

	void CalculaCoeficientePeliculaInterior();

	int GetNumeroDeposito() {
		return FNumeroDeposito;
	}
	;
	double GetDiametro(int i) {
		return FDiametroTubo[i];
	}
	;
	double GetFraccionMasicaInicial(int i) {
		return FComposicionInicial[i];
	}
	;
	double GetFraccionMasicaCC(int j, int i) {
		return FFraccionMasicaCC[j][i];
	}
	;
	double GetViscosidadDinamica(int i) {
		return FViscosidadDinamica[i];
	}
	;
	double getTime1() {
		return FTime1;
	}
	;
	double getNodoIzq() {
		return FNodoIzq;
	}
	;
	double getNodoDer() {
		return FNodoDer;
	}
	;
	void putTime1(double valor) {
		FTime1 = valor;
	}
	;
	double getTime0() {
		return FTime0;
	}
	void putTime0(double valor) {
		FTime0 = valor;
	}
	;
	double getDeltaTime() {
		return FDeltaTime;
	}
	void putDeltaTime(double valor) {
		FDeltaTime = valor;
	}
	;
	int getNin() {
		return FNin;
	}
	;
	double getXRef() {
		return FXref;
	}
	;
	double getLongitudTotal() {
		return FLongitudTotal;
	}
	;
	nmTipoTransCal getTipoTransCal() {
		return FTipoTransCal;
	}
	;
	double getCoefAjustFric() {
		return FCoefAjusFric;
	}
	;
	int getNumeroCanales() {
		return FNumeroParesdeCanales;
	}
	;
	double getMallado() {
		return FMallado;
	}
	;

	double GetDensidad(int i) {
		return Frho[i];
	}
	double GetArea(int i) {
		return FArea[i];
	}
	double GetVelocidad(int i) {
		return FVelocidad0[i];
	}
	;
	double GetAsonido(int i) {
		return FAsonido0[i];
	}
	;
	double GetPresion(int i) {
		return FPresion0[i];
	}
	;
	double GetCoefTurbulencia(int i) {
		return FCoefTurbulencia[i];
	}
	;
	double getTemperaturaInicial() {
		return FTini;
	}
	;
	double getPresionInicial() {
		return FPini;
	}
	;
	double getVelocidadMedia() {
		return FVelMedia;
	}
	;
	double GetYespecie(int j, int i) {
		return FFraccionMasicaEspecie[j][i];
	}
	;
	double GetYespecieCC(int j, int i) {
		return FFraccionMasicaCC[j][i];
	}
	;
	double GetVelocidadCC(int i) {
		return FVelocidadCC[i];
	}
	;
	double GetDensidadCC(int i) {
		return FDensidadCC[i];
	}
	;
	double GetAreaCC(int i) {
		return FAreaCC[i];
	}
	;
	double GetGamma(int i) {
		return FGamma[i];
	}
	;
	double GetRMezcla(int i) {
		return FRMezcla[i];
	}
	;
	double GetCp(int i) {
		return FCpMezcla[i];
	}
	;
	double GetCv(int i) {
		return FCvMezcla[i];
	}
	;
	int getNumeroEspecies() {
		return FNumeroEspecies;
	}
	;
	double Geth(int i) {
		return Fhi[i];
	}
	;
	double Gethext(int i) {
		return Fhe[i];
	}
	;
	double GetH0Pared(int i) {
		return FH0Pared[i];
	}
	;

	nmFormulacionLeyes getFormulacionLeyes() {
		return FMod.FormulacionLeyes;
	}
	;
	double GetU0(int i, int j) {
		return FU0[i][j];
	}
	;
	int getNodoInicialFuente() {
		return FNodoInicialFuente;
	}
	;
	int getNodoFinalFuente() {
		return FNodoFinalFuente;
	}
	;
	double getDistanciaInterpolacion() {
		return FDistanciaInterpolacion;
	}
	;
	double GetLadoCanal(int i) {
		return FLadoCanal[i];
	}
	;

};

//---------------------------------------------------------------------------
#endif
