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

#include <limits>
#define _USE_MATH_DEFINES
#include <cmath>
//---------------------------------------------------------------------------

//Gas constant
const struct{
	const double Universal = 8314.4; // [J / (mol *K)]
	const double Air = 287.;
	const double Fuel = 55.95;
	const double Burnt = 285.4;
	const double O2 = 259.825;
	const double CO2 = 188.9207;
	const double H2O = 461.398;
	const double N2 = 296.837;
	const double Ar = 208.12;
	const double Diesel = 55.95;
	const double Gasoline = 72.42;
}__R;// [J / (kg * K)]

//Molecular weight
const struct{
	const double O2 = 32.;
	const double CO2 = 44.01;
	const double H2O = 18.02;
	const double N2 = 28.01;
	const double Ar = 39.95;
	const double C = 12.01;
	const double NO2 = 46;
	const double NO = 30;
	const double CO = 28.01;
	const double UHC = 55.04;
	const double Diesel = 148.4;
	const double Gasoline = 114.8;
}__PM; // [g]

const struct{
	const double Pi = 3.14159265358979323846;
	const double Pi_2 = 1.57079632679489661923;
	const double Pi_4 = 0.785398163397448309616;
	const double Pi_x_2 = 6.283185307179586;
	const double _1_Pi = 0.318309886183791;
	const double _2_Pi = 0.636619772367581;
	const double _4_Pi = 1.273239544735163;
	const double SQR_4_Pi = 1.128379167095513;
	const double Sigma = 5.670373e-8;
	const double ARef2 = 117724.4721;
	const double ARef = 343.11;
	const double TRef = 292.99271;
	const double PRef = 1.0;
}__CTE;

const double Gamma = 1.4; ///< Air specific heat capacities ratio.
//const double ARef = 343.11; ///< Reference speed of sound. [m / s]
//const double TRef = 292.99271; ///< Reference temperature. [K]
//const double ARef2 = 117724.4721;
//const double PRef = 1.0; ///< Reference pressure. [bar]


struct{
	//GENERAL
	inline double To_kilo(double p){
		return p * 0.001;
	}
	inline double From_kilo(double p){
		return p * 1000;
	}
	//ANGLE
	inline double DegToRad(double p){
		return p * 0.017453292519943;
	}
	// PRESSURE
	inline double BarToPa(double p){
		return p * 1e5;
	}
	inline double PaToBar(double p){
		return p * 1e-5;
	}
	//ROTATIONAL SPEED
	inline double RPMToRPS(double p){
		return p * 0.016666666666667;
	}
	inline double RPMToRad_s(double p){
		return p * 0.104719755119660;
	}
	inline double Rad_sToRPM(double p){
		return p * 9.549296585513721;
	}
	//SPEED
	inline double m_sTokm_h(double p){
		return p * 3.6;
	}
	//TEMPERATURE
	inline double degCToK(double p){
		return p + 273.15;
	}
	inline double KTodegC(double p){
		return p - 273.15;
	}
}__UN;

//stUN __UN;

const double Kb = 1.38054e-23; ///< Boltzmann constant. [J / K]

const struct{
	const double CO2 = -393510; ///< Enthalpy of formation of CO2. [J / mol]
	const double CO = -110530; ///< Enthalpy of formation of CO. [J / mol]
	const double NO2 = 33100; ///< Enthalpy of formation of NO2. [J / mol]
	const double NO = 90290; ///< Enthalpy of formation of NO. [J / mol]
	const double H2O = -241830; ///< Enthalpy of formation of H2O. [J / mol]
}__HFormacion;

#endif
