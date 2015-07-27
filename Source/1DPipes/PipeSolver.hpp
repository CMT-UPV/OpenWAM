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
 * @file PipeSolver.hpp
 * @author Francisco Jose Arnau <farnau@mot.upv.es>
 * @author Luis Miguel Garcia-Cuevas Gonzalez <farnau@mot.upv.es>
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
 * This file declares a basic pipe solver.
 */

#ifndef PipeSolver_hpp
#define PipeSolver_hpp

#include "BasicPipe.hpp"
#include "Math_wam.h"

/**
 * @brief A pipe solver prototype.
 * 
 * It is used for time-integrating the flow inside a pipe.
 */
class TPipeSolver
{
protected:
	TBasicPipe * FPipe; ///< Pipe that uses the propagator.
public:
	/**
	 * @brief Default constructor.
	 * 
	 * Initialises the solver with default values.
	 */
	TPipeSolver();

	/**
	 * @brief Constructor.
	 * 
	 * Initialises the solver and attaches it to a pipe.
	 * 
	 * @param pipe Pipe to attach to.
	 */
	TPipeSolver(TBasicPipe * pipe);


	/**
	 * @brief Connects the solver to a pipe.
	 * 
	 * @param pipe Pipe to connect to.
	 */
	virtual void Connect(TBasicPipe * pipe);

	/**
	 * @brief Integrate the flow.
	 * 
	 * Integrates the flow evolution inside the duct.
	 */
	virtual void Solve() = 0;
};

#endif