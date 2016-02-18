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
#ifndef TCCUnionEntreTubosH
#define TCCUnionEntreTubosH

#include "TCondicionContorno.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class TCCUnionEntreTubos: public TCondicionContorno {
  private:

	int *FNodoFin;               // Nodo del tubo en el extremo del tubo.
	int *FIndiceCC; // Posicion del vector para tomar datos del tubo para la BC (0 Nodo izquierdo; 1 Nodo derecho)
	double **FCC;                // Caracteristica conocida del tubo.
	double **FCD;                // Caracteristica desconocida del tubo.
	int *FNumeroTubo;
	int *FTubo;       // Almacena el numero de tubo en la condicion de contorno.
	int FTuboActual;

	double FGamma3; // Son expresiones con Gamma. Se usan estas variables para no calcularlas tantas veces por instante de tiempo en la misma funcion.
	double FGamma2;
	double FGamma1;

	double FEspesor;
	double FConductividad;

	void Estrechamiento(double CCS, double CCE, double rel_entropia, double rel_area, double smag, double *xa1, double *xa2,
						double *xu2, double *xu1);

	void Ensanchamiento(double CCS, double CCE, double rel_entropia, double rel_area, double smag, double *xu1, double *xu2,
						double *xa2, double *xa1, double *xx3);

  public:

	double getEspesor() {
		return FEspesor;
	}
	;
	double getConductividad() {
		return FConductividad;
	}
	;

	TCCUnionEntreTubos(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
					   nmCalculoGamma GammaCalculation, bool ThereIsEGR);

	~TCCUnionEntreTubos();

	void ReadBoundaryData(const char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe, int nDPF, TDPF **DPF);

	void CalculaCondicionContorno(double Time);

	void TuboCalculandose(int TuboActual);

//  void CalculaCaracteristicas(double Time);

};

#endif

