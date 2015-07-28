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
 * @file BasicPipe.hpp
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
 * This file declares a basic one-dimensional pipe.
 */

#ifndef BasicPipe_hpp
#define BasicPipe_hpp

#include "Math_wam.h"

class TPipeSolver;
class TLaxWendroff;


class TBasicPipe
{
friend class TPipeSolver;
friend class TLaxWendroff;
protected:
	RowArray FU0; ///< State vector at current time.
	double FDeltaTime; ///< Time step. [s]
	double FXref; ///< Cell size. [m]
	RowVector FArea; ///< Cell area. [m ** 2]
	RowVector Fhi; ///< Interior heat transfer coefficient. [W / (m ** 2 * K)]
	RowVector Frho; ///< Density. [kg / m ** 3]
	RowVector FRe; ///< Reynolds number.
	RowVector FDerLinArea; ///< First derivative of the area. [m]
	RowArray FTWPipe; ///< Pipe wall temperature. [K]
	RowArray FGamma; ///< Specific heat capacities ratio.
	RowArray FR; ///< Gas constant. [J / (kg * K)]
	RowArray FGamma1; ///< Gamma - 1.
	RowArray FU1; ///< State vector at next time-step.
	double FCoefAjusFric; ///< Friction fitting coefficient.
	double FCoefAjusTC; ///< Heat transfer fitting coefficient.
	double FFriction; ///< Pipe friction.
	TPipeSolver * FSolver; ///< Pipe solver.
public:
    TBasicPipe();

	/**
	 * @brief Colebrook friction coefficient.
	 * 
	 * @param rug Pipe rugosity.
	 * @param d Pipe diameter.
	 * @param Re Reynolds number.
	 * @return Colebrook friction coefficient.
	 */
	double Colebrook(double rug, double d, double Re) const;
};

#endif