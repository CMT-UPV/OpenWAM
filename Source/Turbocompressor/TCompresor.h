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
#ifndef TCompresorH
#define TCompresorH

#include "Globales.h"
#include "Constantes.h"
#include "TMapaComp.h"
#include "TSAEMap.h"
#include "TMapaComp2Tub.h"
#include "TTC_HTM.h"
#include "TAcousticCompressor.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TTubo;

class TDeposito;

class TCompresor {
  private:

//---------------------------------------------------------------------------
//          FUNCIONES PRIVADAS
//---------------------------------------------------------------------------

	double GetTrabajoCiclo() {
		return FMedias.Trabajo;
	}

	double GetRendimientoMedio() {
		return FMedias.Rendimiento;
	}

	double GetRelCompMedia() {
		return FMedias.RelacionCompresion;
	}

	double GetGastoMedio() {
		return FMedias.Massflow;
	}

	double GetRendimientoInstant() {
		return FInstant.Rendimiento;
	}

	double GetPotenciaInstant() {
		return FInstant.Potencia;
	}

	double GetRelCompInstant() {
		return FInstant.RelacionCompresion;
	}

	double GetGastoInstant() {
		return FInstant.Massflow;
	}

	nmCompressorModel GetModeloCompresor() {
		return FModeloCompresor;
	}

	int GetDepRotor() {
		return FDepRotor;
	}

	int GetDepStator() {
		return FDepStator;
	}

	double GetRelCompBombeo() {
		return Mapa->getRelCompBombeo();
	}

	double GetRegimen() {
		return FRegimen;
	}

	int GetEje() {
		return FEje;
	}

	double GetASonidoComp();

  protected:

//---------------------------------------------------------------------------
//          VARIABLES PROTEGIDAS
//---------------------------------------------------------------------------

	int FNumeroCompresor;
	int FEje;
	int FDepRotor;                     // Numero del deposito conectado al rotor
	int FDepStator;       // Numero del deposito conectado al estator (si tiene)
//TMapaComp *Mapa;                             // Puntero al mapa del compresor
	TCompressorMap *Mapa;
	TMapaComp2Tub *Mapa2T;
	double FRendimiento;                         // Rendimiento (-)
	double FTrabajo;                             // Trabajo del compresor (W)
	double FRelacionCompresion;                  // Relacion de compresion (-)
	double FGastoCompresor;         // Massflow que pasa por el compresor (kg/h)
	double FGastoCorregido;
	double FRegimen;
	double FRegimenCorregido;
	double FPresion10;
	double FPresion20;
	double FTemperatura10;
	double FTemperatura20;

	double FASonidoSalida;

	double FDeltaTiempo;
	double FTiempo0;

	bool FBombeo;

	TTC_HTM *FHTM;

	bool FIsAcoustic;
	TAcousticCompressor *FAcComp;

	nmCompressorModel FModeloCompresor;

	stMediasCompresor FMedias;             // Estructura para los valores medios
	stInstantaneosCompresor FInstant; // Estructura para los valores instantaneos

	double FPotenciaPaso;
	double FTrabajoPaso;
	double FDeltaTPaso;
	double FPotencia;

// Calculo de Especies
	double FGamma;
	double FRMezcla;
	double FCpMezcla;
	double *FFraccionMasicaEspecie;
	double GetFraccionMasiscaEspecie(int i);
	int FNumeroEspecies;
	bool FHayEGR;
	int FIntEGR;
	nmTipoCalculoEspecies FCalculoEspecies;
	nmCalculoGamma FCalculoGamma;
	double FFraccionMasicaEspecieFuel; // Para calculo completo de especies

  public:

//---------------------------------------------------------------------------
//          VARIABLES PUBLICAS
//---------------------------------------------------------------------------

	double getEfficiency() {
		return FRendimiento;
	}
	;

	double getMassflow() {
		return FGastoCompresor;
	}
	;

	double getTrabajo() {
		return FTrabajo;
	}
	;

	double getRendMed() {
		return GetRendimientoMedio();
	}
	;

	double getTrabCiclo() {
		return GetTrabajoCiclo();
	}
	;

	double getRCMed() {
		return GetRelCompMedia();
	}
	;

	double getGastoMed() {
		return GetGastoMedio();
	}
	;

	double getRendINS() {
		return GetRendimientoInstant();
	}
	;

	double getPotenciaINS() {
		return GetPotenciaInstant();
	}
	;

	double getGastoINS() {
		return GetGastoInstant();
	}
	;

	double getRCINS() {
		return GetRelCompInstant();
	}
	;

	nmCompressorModel getModeloCompresor() {
		return GetModeloCompresor();
	}
	;

	int getDepRotor() {
		return GetDepRotor();
	}
	;

	int getDepStator() {
		return GetDepStator();
	}
	;

	double getRCInicial() {
		return GetRelCompBombeo();
	}
	;

	double getRegimen() {
		return GetRegimen();
	}
	;

	int getAxis() {
		return GetEje();
	}
	;

	double getTiempo0() {
		return FTiempo0;
	}
	;

	double getSpeedSound() {
		return GetASonidoComp();
	}
	;

	double getPotenciaPaso() {
		return FPotenciaPaso;
	}
	;

	double getGamma() {
		return FGamma;
	}
	;

	double GetFraccionMasicaEspecie(int i);

	TCompressorMap* GetMap() {
		return Mapa;
	}
	;

//---------------------------------------------------------------------------
//          FUNCIONES PUBLICAS
//---------------------------------------------------------------------------

	TCompresor(int i, nmTipoCalculoEspecies SpeciesModel, int numeroespecies, nmCalculoGamma GammaCalculation,
			   bool ThereIsEGR);

	virtual ~TCompresor() = 0;

	virtual void CalculaGasto(double TrabajoInsTurbina, double TiempoActual) = 0;

	virtual void CondicionCompresor(double Theta, stTuboExtremo *TuboExtremo, double AcumulatedTime, int TuboCalculado) = 0;

	virtual void Initialize() = 0;

	void InterpolaValoresMapa(double rtc);

	void AcumulaMedias(double Tiempo);

	void IniciaMedias();

	void CalculaMedias();

	void CalculaInstantaneos();

	void LeeDatosGraficasMedias(const char *FileWAM, fpos_t &filepos);

	void CabeceraGraficasMedias(stringstream& medoutput);

	void ImprimeGraficasMedias(stringstream& medoutput);

	void LeeDatosGraficasInstantaneas(const char *FileWAM, fpos_t &filepos);

	void CabeceraGraficasInstantaneas(stringstream& insoutput);

	void ImprimeGraficasInstantaneas(stringstream& insoutput);

	void CalculoPotenciaPaso();

	void AsignTCHTM(TTC_HTM *HTM) {
		FHTM = HTM;
	}
	;

	void AsignAcousticElements(TTubo **Pipe, TDeposito **Volume);

	TAcousticCompressor* AcousticC() {
		return FAcComp;
	}
	;

};

#endif
