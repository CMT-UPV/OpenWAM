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

/**
 * @file BasicPipeMethod.hpp
 * @author Francisco Jose Arnau <farnau@mot.upv.es>
 * @author Luis Miguel Garcia-Cuevas Gonzalez <luismiguelgcg@mot.upv.es>
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
 * This file declares a pure virtual pipe computation method.
 */

#ifndef BasicPipeMethod_hpp
#define BasicPipeMethod_hpp

#include "Math_wam.h"


class TBasicPipe;

/**
 * @brief A pure virtual pipe computation method prototype.
 * 
 * It is used for time-integrating the flow inside a pipe, and for computing
 * some of the flow properties.
 */
class TBasicPipeMethod
{
protected:
	TBasicPipe * FPipe; ///< Pipe that uses the method.
public:
	/**
	 * @brief Connects the method to a pipe.
	 * 
	 * @param pipe Pipe to connect to.
	 */
	virtual void Connect(TBasicPipe * pipe) = 0;

	/**
	 * @brief Integrate the flow.
	 * 
	 * Integrates the flow evolution inside the duct.
	 */
	virtual void Solve() = 0;

	/**
	 * @brief Sets the state vector.
	 * 
	 * Sets the state vector with a given pressure, temperature and flow speed.
	 * 
	 * @param p Pressure. [Pa]
	 * @param T Temperature. [K]
	 * @param u Flow speed. [m / s]
	 */
	virtual void setPTU(double p, double T, double u) = 0;

	/**
	 * @brief Sets the state vector.
	 * 
	 * Sets the state vector with a given pressure, temperature and flow speed,
	 * one set of values for each node/cell.
	 * 
	 * @param p Pressure. [Pa]
	 * @param T Temperature. [K]
	 * @param u Flow speed. [m / s]
	 */
	virtual void setPTU(const RowVector& p, const RowVector& T,
		const RowVector& u) = 0;

	/**
	 * @brief Updates the flow variables with the current state vector values.
	 */
	virtual void UpdateFlowVariables() = 0;

	/**
	 * @brief Updates R, gamma and company.
	 */
	virtual void UpdateGasProperties() = 0;
};

#endif