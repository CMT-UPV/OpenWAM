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
 * @file PipeMethod.hpp
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
 * This file declares a pipe computation method.
 */

#ifndef PipeMethod_hpp
#define PipeMethod_hpp

#include "BasicPipeMethod.hpp"
#include "BasicPipe.hpp"


/**
 * @brief A pipe computation method prototype.
 * 
 * It is used for time-integrating the flow inside a pipe, and for computing
 * some of the flow properties.
 */
class TPipeMethod: public TBasicPipeMethod
{
protected:
	double FMaxCourant; ///< Maximum Courant number.
	double FMaxTimeStep; ///< Maximum time-step. [s]
public:
	/**
	 * @brief Connects the method to a pipe.
	 * 
	 * @param pipe Pipe to connect to.
	 */
	virtual void Connect(TBasicPipe * pipe);

	/**
	 * @brief Returns the maximum allowable time-step.
	 *
	 * Returns the maximum allowable time-step due to stability criteria,
	 * probably due to the Courant-Friedrichs-Lewy condition.
	 *
	 * @return Maximum allowable time-step. [s]
	 */
	virtual double getMaxTimeStep();

	/**
	 * @brief Integrate the flow.
	 * 
	 * Integrates the flow evolution inside the duct.
	 */
	virtual void Solve() = 0;
};

#endif