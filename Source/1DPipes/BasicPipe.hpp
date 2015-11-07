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

#include "TCondicionContorno.h"
#include "Math_wam.h"
#include "BasicPipeMethod.hpp"

class TLaxWendroff;


class TBasicPipe
{
friend class TBasicPipeMethod;
friend class TPipeMethod;
friend class TLaxWendroff;
protected:
	RowArray FU0; ///< State vector at current time.
	double FCurrentTime; ///< Current time for the pipe. [s]
	double FDeltaTime; ///< Time step. [s]
	double FXref; ///< Cell size. [m]
	RowVector Fa; ///< Speed of sound. [m / s]
	RowVector FArea; ///< Node or cell interface area. [m ** 2]
	RowVector FA_A; ///< Non-dimensional entropy level. [-]
	RowVector Fbeta; ///< Left-travelling non-dimensional characteristic. [-]
	RowVector Fhi; ///< Interior heat transfer coefficient. [W / (m ** 2 * K)]
	RowVector Frho; ///< Density. [kg / m ** 3]
	RowVector FRe; ///< Reynolds number.
	RowVector FDerLinArea; ///< First derivative of the area. [m]
	RowArray FTWPipe; ///< Pipe wall temperature. [K]
	RowVector FGamma; ///< Specific heat capacities ratio.
	RowVector Flambda; ///< Right-travelling non-dimensional characteristic. [-]
	RowVector FR; ///< Gas constant. [J / (kg * K)]
	RowVector Fcv; ///< Specific heat capacity at constant volume. [J / (kg * K)]
	RowVector Fcp; ///< Specific heat capacity at constant pressure. [J / (kg * K)]
	RowVector Fdx; ///< Distance between nodes or cell length. [m]
	RowVector FGamma1; ///< Gamma - 1.
	RowVector FMArea; ///< Cell center or in-between node area. [m ** 2]
	RowVector FMx; ///< Cell center or in-between node position. [m]
	RowArray FU1; ///< State vector at next time-step.
	RowVector Fpressure; ///< Pressure vector. [Pa]
	RowVector Ftemperature; ///< Temperature vector. [K]
	RowVector Fspeed; ///< Flow speed vector. [m / s]
	RowVector FVolume; ///< Cell volume. [m ** 2]
	RowVector Fx; ///< Node or cell interface position. [m]
	double FCoefAjusFric; ///< Friction fitting coefficient.
	double FCoefAjusTC; ///< Heat transfer fitting coefficient.
	double FFriction; ///< Pipe friction.
	bool FIsIntegrated; ///< Whether or not the pipe is already integrated.
	TCondicionContorno * FLeftBC; ///< Left boundary condition.
	TCondicionContorno * FRightBC; ///< Right boundary condition.
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
	void Solve();

	/**
	 * @brief Sets the boundary conditions of the pipe.
	 * 
	 * @param leftBC Left end boundary condition.
	 * @param rightBC Right end boundary condition.
	 */
	void setBCs(TCondicionContorno * leftBC,
				TCondicionContorno * rightBC);

	/**
	 * @brief Sets the time-step for the integration of this pipe.
	 * 
	 * @param dt Time-step. [s]
	 */
	void setDeltaTime(double dt);

	/**
	 * @brief Sets the pipe geometry.
	 * 
	 * Sets the pipe geometry, using several sections with linear variations
	 * of area. Tries to keep the objective distance between nodes, or cell length
	 * (when using a finite-volume method), keeping a minimum of 3 nodes (or 2
	 * cells). The final node distance will be rounded to keep the pipe length.
	 * 
	 * @param x Known node (or cell interface) positions. [m]
	 * @param dx Node distance (or cell length) objective. [m]
	 * @param A Known node (or cell interface) areas. [m ** 2]
	 */
	void setGeometry(const RowVector & x, double dx, const RowVector & A);

	/**
	 * @brief Sets the state vector.
	 * 
	 * Sets the state vector with a given pressure, temperature and flow speed.
	 * 
	 * @param p Pressure. [Pa]
	 * @param T Temperature. [K]
	 * @param u Flow speed. [m / s]
	 */
	void setPTU(double p, double T, double u);

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
	void setPTU(const RowVector& p, const RowVector& T, const RowVector& u);

	/**
	 * @brief Gets the pipe entropy level vector.
	 *
	 * Gets the entropy level inside the pipe:
	 *
	 * @f[
	 * A_A = \cfrac{a}{a_0} / \left( \cfrac{p}{p_0} \right)
	 * ^ \frac{\gamma - 1}{2 \cdot \gamma}
	 * @f]
	 *
	 * where @f$ A_A @f$ is the entropy level, * @f$ a @f$ is the speed of
	 * sound, @f$ a_0 @f$ is the reference speed of sound, @f$ p @f$ is the
	 * flow pressure, @f$ p_0 @f$ is the reference pressure and
	 * @f$ \gamma @f$ is the specific heat capacities ratio.
	 *
	 * Its value is returned for each node/cell of the pipe.
	 *
	 * @return The pipe entropy level vector. [-]
	 */
	RowVector getA_A() const;

	/**
	 * @brief Gets the pipe entropy level at a given cell.
	 *
	 * Gets the entropy level inside the pipe:
	 *
	 * @f[
	 * A_A = \cfrac{a}{a_0} / \left( \cfrac{p}{p_0} \right)
	 * ^ \frac{\gamma - 1}{2 \cdot \gamma}
	 * @f]
	 *
	 * where @f$ A_A @f$ is the entropy level, * @f$ a @f$ is the speed of
	 * sound, @f$ a_0 @f$ is the reference speed of sound, @f$ p @f$ is the
	 * flow pressure, @f$ p_0 @f$ is the reference pressure and
	 * @f$ \gamma @f$ is the specific heat capacities ratio.
	 *
	 * Its value is returned for ith node/cell of the pipe.
	 *
	 * @param i Cell number.
	 * @return The pipe entropy level at a given cell. [-]
	 */
	double getA_A(unsigned int i) const;

	/**
	 * @brief Gets the pipe entropy level at a given distance from the inlet.
	 *
	 * Gets the entropy level inside the pipe:
	 *
	 * @f[
	 * A_A = \cfrac{a}{a_0} / \left( \cfrac{p}{p_0} \right)
	 * ^ \frac{\gamma - 1}{2 \cdot \gamma}
	 * @f]
	 *
	 * where @f$ A_A @f$ is the entropy level, * @f$ a @f$ is the speed of
	 * sound, @f$ a_0 @f$ is the reference speed of sound, @f$ p @f$ is the
	 * flow pressure, @f$ p_0 @f$ is the reference pressure and
	 * @f$ \gamma @f$ is the specific heat capacities ratio.
	 *
	 * Its value is returned for a node/cell that is at a distance @f$ x @f$
	 * of the pipe inlet.
	 *
	 * @param x Distance from the inlet. [m]
	 * @return The pipe entropy level at a given point. [-]
	 */
	double getA_A(double x) const;

	/**
	 * @brief Gets the pipe beta vector.
	 *
	 * Gets the left-travelling non-dimensional characteristic:
	 *
	 * @f[
	 * \beta = \cfrac{a}{a_0} - \cfrac{\gamma - 1}{2} \cdot \cfrac{u}{a_0}
	 * @f]
	 *
	 * where @f$ \beta @f$ is the non-dimensional characteristic,
	 * @f$ a @f$ is the speed of sound, @f$ a_0 @f$ is the reference speed of
	 * sound, @f$ \gamma @f$ is the specific heat capacities ratio and
	 * @f$ u @f$ is the flow speed.
	 *
	 * Its value is returned for each node/cell of the pipe.
	 *
	 * @return The pipe beta vector. [-]
	 */
	RowVector getBeta() const;

	/**
	 * @brief Gets the pipe beta at a given cell.
	 *
	 * Gets the left-travelling non-dimensional characteristic:
	 *
	 * @f[
	 * \beta = \cfrac{a}{a_0} - \cfrac{\gamma - 1}{2} \cdot \cfrac{u}{a_0}
	 * @f]
	 *
	 * where @f$ \beta @f$ is the non-dimensional characteristic,
	 * @f$ a @f$ is the speed of sound, @f$ a_0 @f$ is the reference speed of
	 * sound, @f$ \gamma @f$ is the specific heat capacities ratio and
	 * @f$ u @f$ is the flow speed.
	 *
	 * Its value is returned for ith node/cell of the pipe.
	 *
	 * @param i Cell number.
	 * @return The pipe beta at a given cell. [-]
	 */
	double getBeta(unsigned int i) const;

	/**
	 * @brief Gets the pipe beta at a given distance from the inlet.
	 *
	 * Gets the left-travelling non-dimensional characteristic:
	 *
	 * @f[
	 * \beta = \cfrac{a}{a_0} - \cfrac{\gamma - 1}{2} \cdot \cfrac{u}{a_0}
	 * @f]
	 *
	 * where @f$ \beta @f$ is the non-dimensional characteristic,
	 * @f$ a @f$ is the speed of sound, @f$ a_0 @f$ is the reference speed of
	 * sound, @f$ \gamma @f$ is the specific heat capacities ratio and
	 * @f$ u @f$ is the flow speed.
	 *
	 * Its value is returned for a node/cell that is at a distance @f$ x @f$
	 * of the pipe inlet.
	 *
	 * @param x Distance from the inlet. [m]
	 * @return The pipe beta at a given point. [-]
	 */
	double getBeta(double x) const;

	/**
	 * @brief Returns the current time for this pipe.
	 * 
	 * @returns The current time for this pipe. [s]
	 */
	double getCurrentTime() const;

	/**
	 * @brief Gets the pipe lambda vector.
	 *
	 * Gets the right-travelling non-dimensional characteristic:
	 *
	 * @f[
	 * \lambda = \cfrac{a}{a_0} + \cfrac{\gamma - 1}{2} \cdot \cfrac{u}{a_0}
	 * @f]
	 *
	 * where @f$ \lambda @f$ is the non-dimensional characteristic,
	 * @f$ a @f$ is the speed of sound, @f$ a_0 @f$ is the reference speed of
	 * sound, @f$ \gamma @f$ is the specific heat capacities ratio and
	 * @f$ u @f$ is the flow speed.
	 *
	 * Its value is returned for each node/cell of the pipe.
	 *
	 * @return The pipe lambda vector. [-]
	 */
	RowVector getLambda() const;

	/**
	 * @brief Gets the pipe lambda at a given cell.
	 *
	 * Gets the right-travelling non-dimensional characteristic:
	 *
	 * @f[
	 * \lambda = \cfrac{a}{a_0} + \cfrac{\gamma - 1}{2} \cdot \cfrac{u}{a_0}
	 * @f]
	 *
	 * where @f$ \lambda @f$ is the non-dimensional characteristic,
	 * @f$ a @f$ is the speed of sound, @f$ a_0 @f$ is the reference speed of
	 * sound, @f$ \gamma @f$ is the specific heat capacities ratio and
	 * @f$ u @f$ is the flow speed.
	 *
	 * Its value is returned for ith node/cell of the pipe.
	 *
	 * @param i Cell number.
	 * @return The pipe lambda at a given cell. [-]
	 */
	double getLambda(unsigned int i) const;

	/**
	 * @brief Gets the pipe lambda at a given distance from the inlet.
	 *
	 * Gets the right-travelling non-dimensional characteristic:
	 *
	 * @f[
	 * \lambda = \cfrac{a}{a_0} + \cfrac{\gamma - 1}{2} \cdot \cfrac{u}{a_0}
	 * @f]
	 *
	 * where @f$ \lambda @f$ is the non-dimensional characteristic,
	 * @f$ a @f$ is the speed of sound, @f$ a_0 @f$ is the reference speed of
	 * sound, @f$ \gamma @f$ is the specific heat capacities ratio and
	 * @f$ u @f$ is the flow speed.
	 *
	 * Its value is returned for a node/cell that is at a distance @f$ x @f$
	 * of the pipe inlet.
	 *
	 * @param x Distance from the inlet. [m]
	 * @return The pipe lambda at a given point. [-]
	 */
	double getLambda(double x) const;

	/**
	 * @brief Returns the maximum allowable time-step.
	 *
	 * Returns the maximum allowable time-step due to stability criteria.
	 *
	 * @return Maximum allowable time-step. [s]
	 */
	double getMaxTimeStep();

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

	/**
	 * @brief Gets the pipe speed vector.
	 * 
	 * @return The pipe speed vector. [m / s]
	 */
	RowVector getSpeed() const;

	/**
	 * @brief Gets the pipe speed at a given cell.
	 * 
	 * @param i Cell number.
	 * @return The pipe speed at a given cell. [m / s]
	 */
	double getSpeed(unsigned int i) const;

	/**
	 * @brief Gets the pipe speed at a given distance from the inlet.
	 * 
	 * @param x Distance from the inlet. [m]
	 * @return The pipe speed at a given point. [m / s]
	 */
	double getSpeed(double x) const;

	/**
	 * @brief Gets the pipe speed of sound vector.
	 * 
	 * @return The pipe speed vector. [m / s]
	 */
	RowVector getSpeedOfSound() const;

	/**
	 * @brief Gets the pipe speed of sound at a given cell.
	 * 
	 * @param i Cell number.
	 * @return The pipe speed of sound at a given cell. [m / s]
	 */
	double getSpeedOfSound(unsigned int i) const;

	/**
	 * @brief Gets the pipe speed of sound at a given distance from the inlet.
	 * 
	 * @param x Distance from the inlet. [m]
	 * @return The pipe speed of sound at a given point. [m / s]
	 */
	double getSpeedOfSound(double x) const;

	/**
	 * @brief Gets the pipe temperature vector.
	 * 
	 * @return The pipe temperature vector. [K]
	 */
	RowVector getTemperature() const;

	/**
	 * @brief Gets the pipe temperature at a given cell.
	 * 
	 * @param i Cell number.
	 * @return The pipe temperature at a given cell. [K]
	 */
	double getTemperature(unsigned int i) const;

	/**
	 * @brief Gets the pipe temperature at a given distance from the inlet.
	 * 
	 * @param x Distance from the inlet. [m]
	 * @return The pipe temperature at a given point. [K]
	 */
	double getTemperature(double x) const;

	TBasicPipeMethod * FMethod; ///< Pipe computation method.
};


#endif