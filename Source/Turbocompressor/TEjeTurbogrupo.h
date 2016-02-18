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
#ifndef TEjeTurbogrupoH
#define TEjeTurbogrupoH

#include "Globales.h"

#include "TController.h"
#include "TTC_HTM.h"
//#include "TTC_MechLosses.h"

#include <cstdio>
#include <iostream>

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class TTurbina;
class TCompresor;

class TEjeTurbogrupo {

  private:

  protected:

	int FNumeroEje; // Numero de Axis (empieza en 1).
	int FNumCilindros;

	double FRegimenEje;
	nmAxisSpeedCalculation FVariacionRegimen;
	double FAngle0;

	double FMomentoInercia; // Momento de Inercia del Axis.

	int FNumCompresoresAcoplados; // Numero de Compresores acoplados al eje.
	int FNumTurbinasAcopladas; // Numero de Turbinas acoplados al eje.
	int *FNumeroCompresor; // Vector con los numeros de compresor acoplados al eje.
	int *FNumeroTurbina; // Vector con los numeros de turbina acopladas al eje.
	TCompresor **FCompresor; // Vector de objetos Compressor.
	TTurbina **FTurbina; // Vector de objetos Turbine.

	double FSumTrabajoCompresores;
	double FSumTrabajoTurbinas;
	double FDeltaReg;

	stResMediosEje FResMediosEje;
	stResInstantEje FResInstantEje;
	int FNumCiclo;

	TController *FController;
	int FControllerID;
	bool FRPMControlled;

	double FTime;

	double FDShaft;
	double FHD;
	double FDoil;
	double FDwater;
	double FJournalBLengh;
	double FTthrustBRmin;
	double FTthrustBRmax;

	double FJournalB_K;
	double FC_p3;
	double Fk_m;
	double F_f;

	double FCWArea;
	double FTWArea;

	double FCAC;
	double FCAT;

	double FMoil;
	double FToil;
	double FPoil;

	double FTwater;
	double FMwater;

	double FTamb;

	bool FThereIsHTM;
	TTC_HTM *FHTM;

	stHTMoil *FOil;
	stHTMwater *FWater;

	TurboBearings *FMechLosses;
	double FMechPower;
	double FMechEff;

  public:

	double getRegimen() {
		return FRegimenEje;
	}
	;

	int getNumeroEje() {
		return FNumeroEje;
	}
	;

	int GetNumeroCompresor(int i);

	int getNumeroCompresoresAcoplados() {
		return FNumCompresoresAcoplados;
	}
	;

	TEjeTurbogrupo(int i, int ncilin);

	~TEjeTurbogrupo();

	void ReadTurbochargerAxis(const char *FileWAM, fpos_t &filepos, TCompresor **Compressor, TTurbina **Turbine);

	void CalculaEjesTurbogrupo(double Theta, nmTipoModelado SimulationType, double Time, double CrankAngle);

	void ReadAverageResultsEje(const char *FileWAM, fpos_t &filepos);

	void CabeceraResultadosMedEje(stringstream& medoutput);

	void ImprimeResultadosMedEje(stringstream& medoutput);

	void IniciaMedias();

	void ResultadosMediosEje();

	void AcumulaResultadosMediosEje(double Actual);

	void ReadInstantaneousResultsEje(const char *FileWAM, fpos_t &filepos);

	void HeaderInstantaneousResultsEje(stringstream& insoutput);

	void ImprimeResultadosInstantaneosEje(stringstream& insoutput);

	void ResultadosInstantEje();

	void InterpolaValoresMapa();

	void AsignaRPMController(TController **Controller);

	void InitizlizeHTM(double Tamb);

};

#endif
