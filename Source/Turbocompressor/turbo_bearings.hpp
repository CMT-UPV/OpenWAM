#ifndef TURBO_BEARINGS_HPP
#define TURBO_BEARINGS_HPP

//#include <cmath>

#include "THTM_Fluids.h"

/**
 * @file turbo_bearings.hpp
 * @author Luis Miguel Garcia-Cuevas Gonzalez <luiga12@mot.upv.es>
 * @version 0.3.4
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
 * The TurboBearings class represent the bearing system in a turbocharger.
 *
 * It has methods to compute the power losses in the bearing system. This file
 * has the interface of TurboBearings.
 *
 */

/**
 * @brief The TurboBearings class represent the bearing system in a turbocharger.
 *
 * The TurboBearing class has methods to compute the power losses in the bearing
 * system.
 */
class TurboBearings {
  private:

	stHTMoil *_Oil;

	double _R_jb;
	double _L_jb;
	double _h_jb;
	double _k_jb;
	double _k_tb;
	double _T1;
	double _n;
	double _p1;
	double _p2;
	double _p3;
	double _p4;
	double _k_m;
	double _m;
	double _c;
	double _rho;
	double _k_A_c;
	double _k_A_t;
	double _A_c;
	double _A_t;
	double _R_tb_min;
	double _R_tb_max;

  public:

	/**
	 * Default constructor.
	 */

	TurboBearings();

	/**
	 * Constructor that sets the different parameters.
	 *
	 * Initializes the class with its different fixed parameters.
	 *
	 * @param Oil Object for the oil methods and properties.
	 * @param L_jb Length of the journal bearing in m.
	 * @param R_jb Radius of the shaft at the journal bearing in m.
	 * @param h_jb Film thickness at the journal bearing in m.
	 * @param k_jb Correction factor for the oil tangential speed gradient at
	 * the journal bearing.
	 * @param k_A_c Form factor of the pressure distribution at the compressor wheel.
	 * @param k_A_t Form factor of the pressure distribution at the turbine wheel.
	 * @param A_c Compressor disc area in m^2.
	 * @param A_t Turbine disc area in m^2.
	 * @param k_m Fraction of oil mass flow that goes through the thrust bearing.
	 * @param R_tb_min Minimum radius of the thrust bearing, in m.
	 * @param R_tb_max Maximum radius of the thrust bearing, in m.
	 * @param k_tb Correction factor for the oil tangential speed gradient at
	 * the thrust bearing.
	 */

	TurboBearings(stHTMoil *Oil, double L_jb, double R_jb, double h_jb, double k_jb, double k_A_c, double k_A_t, double A_c,
				  double A_t, double k_m, double R_tb_min, double R_tb_max, double k_tb);

	/**
	 * \brief Compute the thrust bearing oil film thickness.
	 *
	 * A value proportional to the thrust bearing oil film thickness is computed
	 * as:
	 * @f[
	 * h_{tb} = \left| \frac{k_m \dot{m} 12 g) \mu}{\rho
	 * (k_{A_c} A_c (p_2 - p_1) / 4 + k_{A_t} A_t (p_3 - p_4) / 2)}\right|
	 * ^{\frac{1}{3}}
	 * @f]
	 *
	 * @param T The journal bearing oil temperature, in K.
	 * @return A value proportional to the oil film thickness.
	 */

	double h_tb(double T);

	/**
	 * \brief Compute the journal bearing power lossses.
	 *
	 * The journal bearing power losses are computed as:
	 * @f[
	 * P_{jb} = 2 \pi R_{jb}^3 k_jb \frac{L_jb}{h_{jb}} \mu n^2
	 * @f]
	 *
	 * @param T The journal bearing oil temperature, in K.
	 * @return The journal bearing power losses, in W.
	 */
	double P_jb(double T);

	/**
	 * \brief Compute the thrust bearing power losses.
	 *
	 * The thrust bearing power losses are computed as:
	 * @f[
	 * P_{tb} = \pi \left( R_{tb,max} ^ 2 - R_{tb,min} ^ 2 \right)
	 * \bar{R}_{tb}^2 k_{tb} \mu n^2 / h_{tb}
	 * @f]
	 *
	 * @param T The thrust bearing oil temperature, in K.
	 * @return The thrust bearing power losses, in W.
	 */
	double P_tb(double T);

	/**
	 * \brief Compute the mean temperature of the oil.
	 *
	 * The mean temperature of the oil is computed assuming that it is equal to
	 * the arithmetic mean between the inlet temperature and the outlet
	 * temperature, so in adiabatic conditions:
	 *
	 * @f[
	 * \dot{m} c \left( T_m - T_1 \right)
	 * = 0.75 \left( P_{jb} + P_{tb} \right)
	 * @f]
	 *
	 * @f[
	 * T_m = T_1 + 0.75 \left(P_{jb} + P_{tb} \right) / \left(\dot{m} c \right)
	 * @f]
	 *
	 * @return The mean temperature of the oil, in W.
	 */
	double get_T_oil_m();

	/**
	 * \brief Compute the power losses in the bearings.
	 *
	 * To compute the power losses first the mean temperature of the oil is
	 * computed.  Then, the power of both the journal and the thrust bearing
	 * are estimated.
	 *
	 * @param T1 The oil inlet temperature.
	 * @param n The turbocharger rotating speed in radians per second.
	 * @param p1 The pressure at the inlet of the compressor in Pa.
	 * @param p2 The pressure at the outlet of the compressor in Pa.
	 * @param p3 The pressure at the inlet of the turbine wheel in Pa.
	 * @param p4 The pressure at the outlet of the turbine in Pa.
	 * @param m The oil mass flow rate in kg / s.
	 * @return The power losses in the bearing system, in W.
	 */
	double P_oil(double T1, double n, double p1, double p2, double p3, double p4, double m);
};

#endif
