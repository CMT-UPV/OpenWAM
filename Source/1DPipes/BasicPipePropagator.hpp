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
 * @file PipePropagator.hpp
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
 * This file declares a basic pipe propagator.
 */

#ifndef BasicPipePropagator_hpp
#define BasicPipePropagator_hpp

#include "BasicPipe.hpp"
#include "Math_wam.h"

/**
 * @brief A pipe propagator prototype.
 * 
 * It is used for time-integrating the flow inside a pipe.
 */
class TBasicPipePropagator
{
protected:
	TBasicPipe * FPipe; ///< Pipe that uses the propagator.
public:
	/**
	 * @brief Default constructor.
	 * 
	 * Initialises the propagator with default values.
	 */
	TBasicPipePropagator();

	/**
	 * @brief Constructor.
	 * 
	 * Initialises the propagator and attaches it to a pipe.
	 * 
	 * @param pipe Pipe to attach to.
	 */
	TBasicPipePropagator(TBasicPipe * pipe);


	/**
	 * @brief Connects the propagator to a pipe.
	 * 
	 * @param pipe Pipe to connect to.
	 */
	virtual void Connect(TBasicPipe * pipe);

	/**
	 * @brief Integrate the flow.
	 * 
	 * Integrates the flow evolution inside the duct.
	 */
	virtual void Propagate() = 0;
};

#endif