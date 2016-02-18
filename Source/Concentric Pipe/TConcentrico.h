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
#ifndef TConcentricoH
#define TConcentricoH

#include "Constantes.h"
#include "Globales.h"

#include <cstdio>
#include <iostream>
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TTubo;
class TDPF;
class TCondicionContorno;
class TBloqueMotor;

class TConcentrico {
  protected:
//---------------------------------------------------------------------------
//          VARIABLES PRIVADAS
//---------------------------------------------------------------------------

	int FNumeroConcentrico;
	int FNumeroTubos;
	int FNumeroNodosTemp;
	int FCicloTubo; // Controla el c�lculo de la Temp de pared una vez por ciclo.
	int FCicloActual;
	int FDuracionCiclo;
	int FNumCiclosSinInerciaTermica;
	int FNumTuboInterno;
	int FNumTuboExterno;
	int FNumDPFInterna;

	double FEspesor_fluido;
	double FEspesorTotalInt;
	double FEspesorTotalExt;
	double FDiametroExtGap;

	double FSUMTime;

	double *FTg;
	double *FTpantant;
	double *FTpantpos;
	double *FTpant0;
	double *FTpant1;
	double *FTpant2;
	double ***FTPared;
	double ***FTParedAnt;

	double *FRg_int;          // Resistencia de convecci�n en el tubo interior
	double *FRg_int_ext; // Resistencia de convecci�n del tubo interior al gas de la c�mara de aire
	double *FRg_ext_int; // Resistencia de convecci�n del tubo exterior al gas de la c�mara de aire
	double *FR_ext;       // Resistencia del tubo exterior con el medio ambiente
	double *FR_int_radiacion; // Resistencia por radiaci�n entre las parades del tubo interior y las del tubo exterior
	double *FR_int_RadExt; // Resistencia radial del tubo interno entre el nodo medio y el exterior
	double *FR_int_AxiAnt; // Resistencia axial del tubo interno con el nodo anterior
	double *FR_int_AxiPos; // Resistencia axial del tubo interno con el nodo posterior
	double *FR_int_RadInt; // Resistencia radial del tubo interno entre el nodo medio y el interior
	double *FR_ext_RadExt; // Resistencia radial del tubo externo entre el nodo medio y el exterior
	double *FR_ext_AxiAnt; // Resistencia axial del tubo externo con el nodo anterior
	double *FR_ext_AxiPos; // Resistencia axial del tubo externo con el nodo posterior
	double *FR_ext_RadInt; // Resistencia radial del tubo externo entre el nodo medio y el interior

	double *FCapIntExt; // Capacidad t�rmica del tubo interno en el nodo externo
	double *FCapIntMed; // Capacidad t�rmica del tubo interno en el nodo medio
	double *FCapIntInt; // Capacidad t�rmica del tubo interno en el nodo interno
	double *FCapExtExt; // Capacidad t�rmica del tubo externo en el nodo externo
	double *FCapExtMed; // Capacidad t�rmica del tubo externo en el nodo medio
	double *FCapExtInt; // Capacidad t�rmica del tubo interno en el nodo interno

	double ***FSUMTPTuboIntPro;
	double ***FSUMTPTuboExtPro;

	TTubo **FTubo;
	TDPF **FDPF;
	bool FHayDPF;

//---------------------------------------------------------------------------
//          FUNCIONES PRIVADAS
//---------------------------------------------------------------------------

  public:
//---------------------------------------------------------------------------
//          VARIABLES PUBLICAS
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//          FUNCIONES PUBLICAS
//---------------------------------------------------------------------------
	TConcentrico();

	~TConcentrico();

	virtual void LeeDatosTuboConcentrico(const char *FileWAM, fpos_t &filepos, TTubo **Tubo, TDPF **DPF) = 0;

	virtual void CalculaTemperaturaPared(TBloqueMotor **Motor, double theta, TCondicionContorno **CC) = 0;

	virtual void CalculaTemperaturaParedSinMotor(TCondicionContorno **CC) = 0;

	virtual void CalculaResistenciasdePared(TCondicionContorno **CC) = 0;

	double GetTiempo(int i);

	double GetTiempoDPF();

	void PutTiempo(int i, double Tiempo);

	void PutTiempoDPF(double Tiempo);

	int GetNumTubo(int i);

	int GetNumDPF();

	int GetNumTuboExterno();

	double GetTPared(int j, int k, int i);

	double GetEspesorFluido();

	double GetRg_ext_int(int i);

	bool GetHayDPF();

};

//---------------------------------------------------------------------------
#endif
