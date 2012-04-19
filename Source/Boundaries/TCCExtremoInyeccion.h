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

#ifndef TCCExtremoInyeccionH
#define TCCExtremoInyeccionH

#include "TCondicionContorno.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class TCCExtremoInyeccion : public TCondicionContorno {
private:

	int FNodoFin; // Nodo en el extremo del tubo que est� en la condici�n de contorno.
	int FIndiceCC; // Posici�n del vector para tomar datos del tubo para la BC (0 Nodo izquierdo; 1 Nodo derecho)
	double *FCC; // Caracter�stica conocida del tubo.
	double *FCD; // Caracter�stica desconocida del tubo.
	double FSeccion; // Di�metro del tubo en la condici�n de contorno.

	double FAngap;

	double FTheta; // �ngulo del ciclo.
	double FVelocity;
	double FSonido;
	double FPressure;

	double FTemperaturaIny;
	double FGastoIny;
	double FDuracionIny;
	double FInicioIny;

	double *FComposicion;
	double FGamma3;
	double FGamma5;

	// void PutIniIny(double valor);

	// void PutDuracionIny(double valor);

public:

	void PutIniIny(double valor) {
		FInicioIny = valor;
	};

	void PutDuracionIny(double valor) {
		FDuracionIny = valor;
	};

	TCCExtremoInyeccion(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel,
		int numeroespecies, nmCalculoGamma GammaCalculation, bool ThereIsEGR);

	~TCCExtremoInyeccion();

	void CalculaCondicionContorno(double Time);

	void ReadBoundaryData(char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe,int nDPF, TDPF **DPF);

	void ObtencionValoresInstantaneos(double Theta);

};

#endif

