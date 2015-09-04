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
#include "BasicPipeMethod.hpp"

class TLaxWendroff;


class TBasicPipe
{
friend class TBasicPipeMethod;
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
	RowArray Fcv; ///< Specific heat capacity at constant volume. [J / (kg * K)]
	RowArray Fcp; ///< Specific heat capacity at constant pressure. [J / (kg * K)]
	RowArray FGamma1; ///< Gamma - 1.
	RowArray FU1; ///< State vector at next time-step.
	RowVector Fpressure; ///< Pressure vector. [Pa]
	RowVector Ftemperature; ///< Temperature vector. [K]
	RowVector Fspeed; ///< Flow speed vector. [m / s]
	double FCoefAjusFric; ///< Friction fitting coefficient.
	double FCoefAjusTC; ///< Heat transfer fitting coefficient.
	double FFriction; ///< Pipe friction.
public:
    TBasicPipe();
	
	TBasicPipe(const RowVector & Area, const RowVector & rho,
		const RowVector & p, const RowVector & u);

	/**
	 * @brief Colebrook friction coefficient.
	 * 
	 * @param rug Pipe rugosity.
	 * @param d Pipe diameter.
	 * @param Re Reynolds number.
	 * @return Colebrook friction coefficient.
	 */
	double Colebrook(double rug, double d, double Re) const;

	/**
	 * @brief Integrate the flow.
	 * 
	 * Integrates the flow evolution inside the duct.
	 */
	virtual void Solve();

	/**
	 * @brief Sets the state vector.
	 * 
	 * Sets the state vector with a given pressure, temperature and flow speed.
	 * 
	 * @param p Pressure. [Pa]
	 * @param T Temperature. [K]
	 * @param u Flow speed. [m / s]
	 */
	virtual void set_pTu(double p, double T, double u);

	/**
	 * @brief Gets the pipe pressure vector.
	 * 
	 * @return The pipe pressure vector. [Pa]
	 */
	RowVector getPressure() const;

	/**
	 * @brief Gets the pipe pressure at a given cell.
	 * 
	 * @param i Cell number.
	 * @return The pipe pressure at a given cell. [Pa]
	 */
	double getPressure(unsigned int i) const;

	/**
	 * @brief Gets the pipe pressure at a given distance from the inlet.
	 * 
	 * @param x Distance from the inlet. [m]
	 * @return The pipe pressure at a given point. [Pa]
	 */
	double getPressure(double x) const;

	TBasicPipeMethod * FMethod; ///< Pipe computation method.
};


#endif