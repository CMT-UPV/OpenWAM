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

/**
 * @file Constantes.h
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
 * This file defines several constants used in OpenWAM.
 */


//---------------------------------------------------------------------------

#ifndef ConstantesH
#define ConstantesH
//#include "TBloqueMotor.h"
#ifdef __BORLANDC__
    #include <vcl.h>
#endif
//---------------------------------------------------------------------------
const double Runiversal=8314.4;
const double R=287.;
const double RBurnt=285.4;
const double RAir=287.;
const double RO2=259.825;
const double RCO2=188.9207;
const double RH2O=461.398;
const double RN2=296.837;
const double RArgon=208.12;
const double PMO2=32;
const double PMCO2=44.01;
const double PMH2O=18.02;
const double PMN2=28.01;
const double PMArgon=39.95;
const double PMCarbono=12.01;
const double PMNO2=46;
const double PMNO=30;
const double PMCO=28.01;
const double PMDiesel=148.4; // Diesel C10.8H18.7 , los HC mantienen la misma relacion C/H y le damos
const double PMHC=55.04;     // la composici�n C4H6.93
const double RDiesel=55.95;
const double PMGasoline=114.8;    // Gasoline molar mass
const double RGasoline=72.42;



const double Pi=3.1415927;
const double Gamma=1.4;
const double ARef=343.11;
const double TRef=292.99271; ///< Reference temperature. [K]
const double ARef2=117724.4721;
const double PRef=1.0; ///< Reference pressure. [bar]
const double StephanBoltzmann = 5.67e-8; // W/m2 K4

const double unBarToPa = 1e5;
const double unPaToBar = 1e-5;
const double unKToC = -273.15;
const double unCToK = 273.15;

const double Kb=1.38054e-23; ///< Boltzmann constant. [J / K]

const double HFormacion_CO2=-393510; ///< Enthalpy of formation of CO2. [J / mol]
const double HFormacion_CO=-110530; ///< Enthalpy of formation of CO. [J / mol]
const double HFormacion_NO2=33100; ///< Enthalpy of formation of NO2. [J / mol]
const double HFormacion_NO=90290; ///< Enthalpy of formation of NO. [J / mol]
const double HFormacion_H2O=-241830; ///< Enthalpy of formation of H2O. [J / mol]

#endif
