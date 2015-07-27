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
 * @file LaxWendroff.hpp
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
 * This file declares a Lax Wendroff finite-differences integrator for
 * one-dimensional pipes.
 */

#ifndef LaxWendroff_hpp
#define LaxWendroff_hpp

#include "BasicPipe.hpp"
#include "PipeSolver.hpp"
#include "Math_wam.h"

/**
 * @brief A Lax Wendroff integrator.
 * 
 * A two-steps Lax Wendroff integrator.  It is second order-accurate in both
 * time and space.
 */
class TLaxWendroff: public TPipeSolver
{
protected:
	RowVector Fhi12; ///< Internal heat transfer coefficient at half time-step.
	RowVector Frho12; ///< Density at half time-step. [kg / m ** 3]
	RowVector FRe12; ///< Reynolds number at half time-step.
	RowVector FTWPipe12; ///< Wall temperature at half time-step. [K]
	RowVector FGamma12; ///< Heat capacities ratio at half time-step.
	RowVector FR12; ///< Gas constant at half time-step. [J / (kg * K)]
	RowVector FGamma1_12; ///< Gamma - 1.
	RowVector FDerLinArea_12; ///< Area derivative at half time-step. [m]
	RowVector FArea_12; ///< Area at half cell. [m ** 2]
	RowArray Fx1;
	RowArray Fx2;
	RowArray Fx3;
	RowArray Fx4;
	RowArray Fx1_12;
	RowArray Fx2_12;
	RowArray Fx3_12;
	RowArray Fx4_12;
	RowArray FW; ///< Flux vector;
	RowArray FV1; ///< Source terms due to area.
	RowArray FV2; ///< Source terms due to friction and heat.
	RowArray FU_12; ///< State vector at half time-step.
	RowArray FW_12; ///< Flux vector at half time-step.
	RowArray FV1_12; ///< Source terms due to area at half time-step.
	RowArray FV2_12; ///< Source terms due to friction and heat at half time-step.
public:
	/**
	 * @brief Default constructor.
	 * 
	 * Initialises the propagator with default values.
	 */
	TLaxWendroff();

	/**
	 * @brief Constructor.
	 * 
	 * Initialises the propagator and attaches it to a pipe.
	 * 
	 * @param pipe Pipe to attach to.
	 */
	TLaxWendroff(TBasicPipe * pipe);

	/**
	 * @brief Computes the flux vector.
	 * 
	 * @param U State vector.
	 * @param W Flux vector.
	 * @param Gamma Specific heat capacities ratio.
	 * @param Gamma1 Specific heat capacities ratio minus 1.
	 */
	void ComputeFlux(const RowArray & U, RowArray & W,
		const RowArray & Gamma, const RowArray & Gamma1);

	/**
	 * @brief Computes the source terms due to area change.
	 * 
	 * @param U State vector.
	 * @param V Source terms.
	 * @param A Area. [m ** 2]
	 * @param Gamma1 Gamma - 1.
	 */
	void ComputeSource1(const RowArray & U, RowArray & V,
		const RowArray & A, const RowArray & Gamma1);

	/**
	 * @brief Connects the Lax Wendroff integrator to a pipe.
	 * 
	 * @param pipe Pipe to connect to.
	 */
	virtual void Connect(TBasicPipe * pipe);

	/**
	 * @brief Integrate the flow.
	 * 
	 * Integrates the flow evolution inside the duct.
	 */
	void Solve();
};

#endif