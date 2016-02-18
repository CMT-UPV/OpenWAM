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
#ifndef TDPFH
#define TDPFH
#include <cstdio>

#include "Constantes.h"
#include "Globales.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include "TCanalDPF.h"

class TBloqueMotor;
class TTubo;
class TConcentrico;
class TCondicionContorno;

class TDPF {
  private:
//---------------------------------------------------------------------------
// VARIABLES PRIVADAS
//---------------------------------------------------------------------------
	int FNumeroDPF;
	int FNumeroHacesCanales;
	int *FNumeroCanalesHaz;
	int FNumeroCanalesTotales; // Numero de canales de entrada
	int FNumeroEspecies;
	int FIntEGR;

	double FDuracionCiclo;
	int FNumCiclosSinInerciaTermica;
	int FCicloActual;
	int FNumResMedios;
	double FTiempoMedSUM;
	double FControlResMed;
	stResMediosDPF **FResMediosDPF;
	int FNumResMediosCE;
	int FNumResMediosCS;
	int FNumResInstantaneos;
	stResInstantDPF **FResInstantaneosDPF;
	int FNumResInstantaneosCE;
	int FNumResInstantaneosCS;

	bool FHayConcentrico;
	int FNumTuboExt;
	int FNumConcentrico;

	bool FDPFCatalizada;
	double FDiametroFiltroEfect;
	double FLongitudEfec;
	double FLongitudTapon;
	double FEspesorParedPorosa;
	double FPorosidadLimpia;
	double FDiametroMedioPoroso;
	double **FDiametroPoro;
	double **FSupActCatalizador;
	double **FSupActSoot;
	double **FSupEspecifica;
	double FPorosidadCapaParticulas;
	double FMasaSootInicial; // Masa de soot total en la DPF para las condiciones iniciales
	double *FMasaSootInicialHaz; // Masa de soot total en cada haz para las condiciones iniciales
	double **FMasaSootLayer; // Masa de soot sobre las paredes porosas en cada VC de cada uno de los nodos de los canales de entrada.
	double *FMasaSootLayerTotal; // Masa de soot total en la DPF presente sobre las paredes porosas de cada haz
	double FMasaSootTotal;			 // Masa de soot en el monolito
	double FDensidadSootPared;
	double FDensidadSootCapa;
	double FDiametroPrimarioSoot;
	double FDiametroAgregadoSoot;
	double FFactorPercolacion;
	double FFraccionParedSaturada;
	double FFactorForma;
	double **FEspesorSoot;
	double ***FVelocidadPared;
	double ***FVelocidadPared0;
	double FKwallLimpia;
	double **FKwall;
	double **FKsoot;
	double **FPorosidad;
	double FDiametroUnidadCelular; // Diametro de la unidad celular del modelo de filtrado
	double **FDiametroUnidadColectora; // Diametro de la unidad colectora. Se supone constante para todas las unidades colectoras en el volumen de control de cada nodo.
	double FDiametroUnidadColectoraLimpia; // Diametro de la unidad colectora cuando la trampa esta limpia.
	double **FParametroIntercepcion;
	double Ffuncion_f_Limpia; // Valor de la funcion f de Kubawara considerando la trampa limpia.
	double **FEficiencia;
	double **FCoeficienteParticion;
	double FMasaSootSaturacion;  // Masa de soot que satura una unidad colectora
	double *FMasaSootSaturacionHaz; // Masa de soot que satura las unidades colectoras de un haz
	double FMasaSootSaturacionTotal; // Masa de soot de saturacion total de la trampa.
	double FVolumenTotalDPF;          // Volumen total de pared porosa en la DPF
	double *FVolumenTotal;          // Volumen total de pared porosa en cada haz
	long long int FNumeroUnidadesCelularesFiltro; // Numero total de unidades celulares en la DPF
	long long int *FNumeroUnidadesCelularesHaz; // Numero total de unidades celulares en cada haz
	long long int
	**FNumeroUnidadesCelulares; // Numero de unidades celulares en el volumen de control de cada nodo de los canales de entrada
	double **FLongitudVC;       // Longitud del volumen de control en cada nodo.
	double **FMasaSootUC;              // Masa de soot en cada unidad colectora.

	double **FKreg1;           // Tasa de reaccion por regeneracion termica (O2)
	double **FKreg2;              // Tasa de reaccion por regeneracion con NO2
	double FIndiceCompletitud1; // Indice de completitud de la regeneracion termica (O2)
	double FIndiceCompletitud2; // Indice de completitud de la regeneracion con NO2
	double **FR1;                 // Tasa de reaccion de la combustion de CO
	double **FR2;                 // Tasa de reaccion de la combustion de HC
	double **FR3;                 // Tasa de reaccion de la oxidacion de NO
	double **FR4;                 // Tasa de reaccion de la reduccion del NO2
	double **FR5; // Tasa de reaccion de la combustion de combustible sin quemar.
	double FRatioNO2_NOx;         // Tanto por uno de NO2 en los NOx totales
	double ***FTasaFraccionMasicaEspecie;
	double ***FFraccionMasicaEspecieSalida;
	double **FFraccionMasicaNO2Salida;
	double **FFraccionMasicaNO2Entrada;
	double **FQreg;              // Calor liberado en el proceso de regeneracion
	double **FQ1;          // Calor liberado en la reaccion de combustion del CO
	double **FQ2;       // Calor liberado en la reaccion de combustion de los HC
	double FIncrH_reg1;
	double FIncrH_reg2;
	double FIncrH_R1;
	double FIncrH_R2;

	double **FAreaVCCanalEntrada;
	double **FAreaVCCanalSalida;

	TCanalDPF ***FCanal;
	int *FNodoIzq;
	int *FNodoDer;

// Variables transmision de calor
	double FCoefAjusTC;
	double FTExt;
	int FTctpt;    // Tipo calculo temperatura pared tubo.
	nmTipoCalcTempParedTubos FTipoCalcTempPared;
	int FCicloDPF;
	double ***FResistRadial;
	double ***FResistConv;
	double **FResistAxialAnt;
	double **FResistAxialPost;
	double **FResistEntreHacesAnt;
	double **FResistEntreHacesPost;
	double **FCapEntrada;
	double **FCapPared;
	double **FCapSalida;
	double *FDiametroInicialHaz;
	double *FDiametroHazInt;
	double *FDiametroHazExt;
	double ***FTPared;
	double ***FTParedAnt;
	double ****FSUMTPPromedio;
	double ***FSUMTPPromedioConc;
	double *FResistConduccionAislante;
	double *FResistConduccionAire;
	double *FResistRadiacionAire;
	double *FResistConduccionMetal;
	double *FResistConveccionExt;
	double *FResistAxialMetalExtAnt;
	double *FResistAxialMetalExtPost;
	double *FCapNodoExteriorSuperficie;
	double *FCapNodoMedioSuperficie;
	double *FCapNodoInteriorSuperficie;
	double **FTSuperficie;
	double *FNumCanalesHaz;
	double *FSuperficieTCHazAnt;
	double *FSuperficieTCHazPost;
	double *FAreaInternaHaz;

	double *FRg_int_ext;
	double *FR_int_radiacion;
	double *FR_ext_RadInt;

//double ***FVelPro;

	double FTIniPared;
	double FTIniParedExt;
	double FCoefExt;
	nmRefrigerante FTipoRefrig;    // Tipo refrigerante
	double FTRefrigerante;
	double FEmisividad;
	double FEspesorAislante;
	double FEspesorMetal;
	double FDensidadAislante;
	double FDensidadMetal;
	double FConductividadSoot;
	double FConductividadPared;
	double FConductividadParedRadial;
	double FConductividadAislante;
	double FConductividadMetal;
	double FDensidadParedPorosa;
	double FCalorEspPared;
	double FCalorEspSoot;
	double FCalorEspMetal;
	double FCalorEspAislante;
	double FAjusteResistenciaHazExt;
	double FEspesorAire;
	double FConductividadAire;
	double FEmisividadIntAire;
	double FEmisividadExtAire;

	bool FAislante;
	double FAnguloTotalCiclo;
	double *FSUMTime;

	double FTime0DPF;
	double FTime1DPF;
	double FDeltaTimeDPF;

	bool FCalculoRegeneracion;
	bool FCalculoFiltrado;

	double FFactorFrecuenciaO2;
	double FFactorFrecuenciaNO2;
	double FEnergiaActO2;
	double FEnergiaActNO2;

// Mapa para determinar proporcion de NO2
	int FNumeroDatos_FraccionesNOx;
	int FNumeroDatos_Temperaturas;
	double *FFraccionNOxTabla;
	double *FTemperaturasTabla;
	double **FMapa_ProporcionNO2;

// Resistencias con los conductos aledanos
	TTubo *FTuboEntradaDPF;
	TTubo *FTuboSalidaDPF;
	int FNodoTuboEntrada;
	int FNodoTuboSalida;

	double FAjustRAxAnt;
	double FAjustRAxPos;

//---------------------------------------------------------------------------
// FUNCIONES PRIVADAS
//---------------------------------------------------------------------------
	double Interpola(double vizq, double vder, double axid, double xif);

	void CalculoVelocidadPared();

	void SubmodeloFiltrado();

	void SubmodeloRegeneracion();

	void CalculoKsoot();

	void CalculoEspesorSoot();

	void CalculaTemperaturaPared(TBloqueMotor **Motor, double theta, int j, TTubo **Tubo, TConcentrico **Concentrico);

	void CalculaTemperaturaParedSinMotor(int j, TTubo **Tubo, TConcentrico **Concentrico);

  public:
//---------------------------------------------------------------------------
// VARIABLES PUBLICAS
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// FUNCIONES PUBLICAS
//---------------------------------------------------------------------------

	TDPF(int numeroDPF, TBloqueMotor **Motor, int NumeroEspecies);

	~TDPF();

	void LeeDatosDPF(const char *FileWAM, fpos_t &filepos, nmTipoCalculoEspecies CalculoEspecies,
					 nmCalculoGamma CalculoGamma, bool HayEGR, TBloqueMotor **Motor);

	void CalculoTransmisionCalor(TBloqueMotor **Motor, double theta, TTubo **Tubo, TConcentrico **Concentrico);

	void LeeResultadosMediosDPF(const char *FileWAM, fpos_t &filepos);

	void ImprimeResultadosMedios(std::stringstream& medoutput) const;

	void CabeceraResultadosMedios(std::stringstream& medoutput, stEspecies *DatosEspecies) const;

	void CalculaResultadosMedios(double theta);

	void LeeResultadosInstantaneosDPF(const char *FileWAM, fpos_t &filepos);

	void CabeceraResultadosInstantaneos(std::stringstream& insoutput, stEspecies *DatosEspecies) const;

	void ImprimeResultadosInstantaneos(std::stringstream& insoutput) const;

	void CalculaResultadosInstantaneos();

	void InicializaDPF(int nconcentrico, TConcentrico **Concentrico);

	void IniciaVariablesTransmisionCalor(double tamb);

	void CalculoEstabilidadDPF();

	void AjustaPaso(double TiempoFinPaso);

	void CalculoResistenciaTC(int j, TTubo **Tubo, TConcentrico **Concentrico);

	void CalculoResistenciaTC_First_Time(int j, TTubo **Tubo, TConcentrico **Concentrico);

	void ComunicacionTubos(TCondicionContorno **CC, int numCC);

	void CalculoSubmodelos();

	double getTime1DPF() {
		return FTime1DPF;
	}
	;
	void putTime1DPF(double valor) {
		FTime1DPF = valor;
	}
	;

	double getTime0DPF() {
		return FTime0DPF;
	}
	;
	void putTime0DPF(double valor) {
		FTime0DPF = valor;
	}
	;

	double getDeltaTimeDPF() {
		return FDeltaTimeDPF;
	}
	;
	void putDeltaTimeDPF(double valor) {
		FDeltaTimeDPF = valor;
	}
	;

	double getDuracionCiclo() {
		return FDuracionCiclo;
	}
	;

	int getNumeroHacesCanales() {
		return FNumeroHacesCanales;
	}
	;
	double getLongitudEfec() {
		return FLongitudEfec;
	}
	;
	double getLongitudTapon() {
		return FLongitudTapon;
	}
	;
	double getEspesorParedPorosa() {
		return FEspesorParedPorosa;
	}
	;
	double GetEspesorSoot(int j, int i) {
		return FEspesorSoot[j][i];
	}
	;
	TCanalDPF* GetCanal(int j, int i) {
		return FCanal[j][i];
	}
	;
	double GetVelocidadPared(int j, int i, int k) {
		return FVelocidadPared[j][i][k];
	}
	;
	double GetTasaFraccionMasicaEspecie(int j, int i, int k) {
		return FTasaFraccionMasicaEspecie[j][i][k];
	}
	;
	double GetFraccionMasicaSalida(int j, int i, int k) {
		return FFraccionMasicaEspecieSalida[j][i][k];
	}
	;
	double GetRreg1(int j, int i) {
		return FKreg1[j][i];
	}
	;
	double GetRreg2(int j, int i) {
		return FKreg2[j][i];
	}
	;
	double getIndCompletitud1() {
		return FIndiceCompletitud1;
	}
	;
	double getIndCompletitud2() {
		return FIndiceCompletitud2;
	}
	;
	double GetSupEspecifica(int j, int i) {
		return FSupEspecifica[j][i];
	}
	double GetEficiencia(int j, int i) {
		return FEficiencia[j][i];
	}
	;
	double GetLongitudVC(int j, int i) {
		return FLongitudVC[j][i];
	}
	;
	double GetQreg(int j, int i) {
		return FQreg[j][i];
	}
	;
	double Getq_reac1(int j, int i) {
		return FQ1[j][i];
	}
	;
	double Getq_reac2(int j, int i) {
		return FQ2[j][i];
	}
	;
	double getAnguloTotalCiclo() {
		return FAnguloTotalCiclo;
	}
	;
	double GetTPared(int j, int i, int k) {
		return FTPared[j][i][k];
	}
	;
	double getCoefAjusTC() {
		return FCoefAjusTC;
	}
	;
	double getTExt() {
		return FTExt;
	}
	;
	nmTipoCalcTempParedTubos getTipoCalcTempPared() {
		return FTipoCalcTempPared;
	}
	;
	nmRefrigerante getTipoRefrig() {
		return FTipoRefrig;
	}
	;
	double GetDiametroExtHaz(int i) {
		return FDiametroHazExt[i];
	}
	;
	double getCoefExt() {
		return FCoefExt;
	}
	;
	double GetTSuperficie(int j, int i) {
		return FTSuperficie[j][i];
	}
	;

	int getCicloDPF() {
		return FCicloDPF;
	}
	;

	double getAjustRAxAnt() {
		return FAjustRAxAnt;
	}
	;
	double getAjustRAxPos() {
		return FAjustRAxPos;
	}
	;
	double getEmisividad() {
		return FEmisividad;
	}
	;
	double getDiametroExt() {
		return FDiametroFiltroEfect + 2 * FEspesorAislante + 2 * FEspesorAire + 2 * FEspesorMetal;
	}
	;
	double getEspesorAislante() {
		return FEspesorAislante;
	}
	;
	double getEspesorAire() {
		return FEspesorAire;
	}
	;
	double getEspesorMetal() {
		return FEspesorMetal;
	}
	;
	double getConductividadMetal() {
		return FConductividadMetal;
	}
	;
	double getDiametroEfect() {
		return FDiametroFiltroEfect;
	}
	;

	void PutConcentric(int valor) {
		valor == 0 ? FHayConcentrico = false : FHayConcentrico = true;
	}
	;
};
#endif
