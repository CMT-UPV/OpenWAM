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

#ifndef TCCCilindroH
#define TCCCilindroH

#include "TCondicionContorno.h"

#include <cstdio>
#include <iostream>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TCCCilindro : public TCondicionContorno
{
private:

TTipoValvula *FValvula;
int FNumeroCilindro;

double FPref;            // Pressure de referencia
double FPresionCil;      // Presion en el cilindro
double FVelSonidoCil;    // Velocity del sonido en el cilindro
double FMomentoAngular;  // Momento angular en el cilindro
double FMasaCilindro;    // Masa dentro del cilindro

double FCDEntrada;       // Coeficiente de descarga a la entrada
double FCDSalida;        // Coeficiente de descarga a la salida
double FCTorbellino;    // Coeficiente de torbellino

double FSeccionEficaz;   // Seccion eficaz de la valvula
double FGasto;           // Massflow en el extremo del tubo.
double FMomento;
double FVelocity;         // Velocity en el extremo del tubo
double FSonido;            // Velocity del sonido en el extremo del tubo
double FVelocidadGarganta;  // Velocity en la garganta
double FMachGarganta;       // Numero de Mach en la garganta.
double FGastoGarganta;      // Massflow en la garganta.
double FRelacionPresionGarganta;
double FAd;

double FSeccionValvula;   // Seccion en la garganta
double FSeccionTubo;      // Seccion en el extremo del tubo

double FCarrera;
double Fk;          // Relacion entre la seccion del tubo en el extremo y la seccion eficaz de la valvula.
double Fcc;         //Variable auxiliar utilizada para el calculo de el caso saliente supercritico.


nmTipoValv FTipoValv;
nmCaso FCaso;

int FNodoFin;               // Nodo del tubo en el extremo del tubo.
int FSigno;              // No hara falta una vez se valide.
double *FCC;                // Caracteristica conocida del tubo.
double *FCD;                // Caracteristica desconocida del tubo.

// FUNCIONES

void FlujoEntranteCilindro();

void FlujoSalienteCilindro();

void Resolucion(double ext1,double ext2,nmCaso Caso,double *u2t,double *a2t,
     double prey,double prex);

void FESubcritico(double vel_son_supuesta,double *u2_1,double *u2_2);

void FESupercritico(double mach_supuesto,double *miembro1,double *xx2);

void FSSubcritico(double vel_son_supuesta,double *error,double *miembro2);

void FSSupercritico(double vel_supuesta,double *a2_1,double *a2_2);

public:

    double getSeccionEficaz(){return FSeccionEficaz;};
    double getMassflow(){return FGasto;};
    double getMomento(){return FMomento;};
    double getVelocity(){return FVelocity;};
    double getVelocidadSonido(){return FSonido;};
    double getVelocidadGarganta(){return FVelocidadGarganta;};
    double getMachGarganta(){return FMachGarganta;};
    double getGastoGarganta(){return FGastoGarganta;};
    double getRelacionPresionGarganta(){return FRelacionPresionGarganta;};
    int getNumeroCilindro(){return FNumeroCilindro;};
    TTipoValvula* getValvula(){return FValvula;};


  TCCCilindro(nmTypeBC TipoCC,int numCC);

  ~TCCCilindro();

  void LeeCCValvula(const char *FileWAM,fpos_t &filepos,int NumberOfPipes,TTubo **Pipe);

  void CalculaCondicionContorno();

  void AsignaTipoValvula(TTipoValvula **Origen,int Valv,int i);

  void ObtencionValoresInstantaneos();
};

#endif

