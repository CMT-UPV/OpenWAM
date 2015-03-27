#ifndef UNITS_HPP
#define UNITS_HPP

/**
 * @file units.hpp
 * @author Luis Miguel Garcia-Cuevas Gonzalez <luiga12@mot.upv.es>
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
 * The functions declared in this file are used to convert to several
 * physical units.
 *
 * This file contains the interfaces to such functions.
 */

#include <string>
#include <iostream>
#include "Constantes.h"

/**
 * @brief Converts a given pressure from some unit to bar.
 *
 * Converts a given pressure from some unit to bar.
 *
 * @param x Pressure to convert.
 * @param unit Original unit.
 * @return The pressure in bar. [bar]
 */
double to_bar(const double& x, const std::string& unit);

/**
 * @brief Converts a given temperature from some unit to celsius.
 *
 * Converts a given temperature from some unit to celsius.
 *
 * @param x Temperature to convert.
 * @param unit Original unit.
 * @return The temperature. [degC]
 */
double to_celsius(const double& x, const std::string& unit);

/**
 * @brief Converts a given angle from some unit to degrees.
 *
 * Converts a given angle from some unit to degrees.
 *
 * @param x Angle to convert.
 * @param unit Original unit.
 * @return The angle in degrees. [deg]
 */
double to_degrees(const double& x, const std::string& unit);

/**
 * @brief Converts a given mass flow from some unit to kg / s.
 *
 * Converts a given mass flow rate from some unit to kg / s.
 *
 * @param x mass flow rate to convert.
 * @param unit Original unit.
 * @return The mass flow in kilograms per second. [kg / s]
 */
double to_kg_s(const double& x, const std::string& unit);

/**
 * @brief Converts a given inertia from some unit to kg * m^2.
 *
 * Converts a given inertia from some unit to kg * m^2.
 *
 * @param x inertia to convert.
 * @param unit Original unit.
 * @return The inertia in kilograms square metres. [kg * m^2]
 */
double to_kg_square_m(const double& x, const std::string& unit);

/**
 * @brief Converts a given reduced mass flow from some unit to kg / s sqrt(K)/MPa.
 *
 * Converts a given reduced mass flow rate from some unit to kg / s sqrt(K)/MPa.
 *
 * @param x reduced mass flow rate to convert.
 * @param unit Original unit.
 * @return The mass flow in kilograms per second square root of kelvin per Megapascals. [kg / s sqrt(K)/MPa]
 */
double to_kg_s_sqrt_K_MPa(const double & x, const std::string& unit);

/**
 * @brief Converts a given massfrom some unit to kg.
 *
 * Converts a given mass flow from some unit to kg.
 *
 * @param x mass flow rate to convert.
 * @param unit Original unit.
 * @return The mass in kilograms. [kg]
 */
double to_kg(const double& x, const std::string& unit);

/**
 * @brief Converts a given length from some unit to metres.
 *
 * Converts a given length from some unit to metres.
 *
 * @param x Size to convert.
 * @param unit Original unit.
 * @return The size in metres. [m]
 */
double to_metres(const double& x, const std::string& unit);

/**
 * @brief Converts a given speed from some unit to metres per second.
 *
 * Converts a given speed from some unit to metres per second.
 *
 * @param x Speed to convert.
 * @param unit Original unit.
 * @return The speed in metres per second. [m / s]
 */
double to_m_s(const double& x, const std::string& unit);

/**
 * @brief Converts a given force from some unit to newtons.
 *
 * Converts a given force from some unit to newtons.
 *
 * @param x Force to convert.
 * @param unit Original unit.
 * @return The force in newtons. [N]
 */
double to_N(const double& x, const std::string& unit);

/**
 * @brief Converts a given stiffness from some unit to newtons per metre.
 *
 * Converts a given stiffness from some unit to newtons per metre.
 *
 * @param x Stiffness to convert.
 * @param unit Original unit.
 * @return The stiffness in newtons per metre. [N / m]
 */
double to_N_per_m(const double& x, const std::string& unit);

/**
 * @brief Converts a given damping coefficient from some unit to N * s / m.
 *
 * Converts a given damping coefficient from some unit to N * s / m.
 *
 * @param x Damping coefficient to convert.
 * @param unit Original unit.
 * @return The damping coefficient. [N * s / m]
 */
double to_N_s_per_m(const double& x, const std::string& unit);

/**
 * @brief Converts a given rotational speed from some unit to rpm.
 *
 * Converts a given rotational speed from some unit to revolutions
 * per minute.
 *
 * @param x Rotational speed to convert.
 * @param unit Original unit.
 * @return The rotational speed in revolutions per minute. [rpm]
 */
double to_rpm(const double& x, const std::string& unit);

/**
 * @brief Converts a given reduced speed from some unit to rps / sqrt(K).
 *
 * Converts a given reduced speed from some unit to revolutions
 * per minute per second per square root of kelvin.
 *
 * @param x Rotational speed to convert.
 * @param unit Original unit.
 * @return The rotational speed in revolutions per second per square root of kelvin. [rps / sqrt(K)]
 */
double to_rps_sqrt_K(const double& x, const std::string& unit);

/**
 * @brief Converts a given time from some unit to seconds.
 *
 * Converts a given time from some unit to seconds.
 *
 * @param x Time to convert.
 * @param unit Original unit.
 * @return The time in seconds. [s]
 */
double to_seconds(const double& x, const std::string& unit);

/**
 * @brief Converts a given area from some unit to square metres.
 *
 * Converts a given area from some unit to square metres.
 *
 * @param x Area to convert.
 * @param unit Original unit.
 * @return The area in square metres. [m ** 2]
 */
double to_square_metres(const double& x, const std::string& unit);

/**
 * @brief Converts a given area from some unit to square metres.
 *
 * Converts a given area from some unit to square metres.
 *
 * @param x Area to convert.
 * @param unit Original unit.
 * @return The area in square metres. [m ** 3]
 */
double to_cubic_metres(const double& x, const std::string& unit);

/**
 * @brief Converts a given power from some unit to watts.
 *
 * Converts a given power from some unit to watts.
 *
 * @param x Power to convert.
 * @param unit Original unit.
 * @return The power in watts. [W]
 */
double to_watts(const double& x, const std::string& unit);

/**
 * @brief Converts a given power from some unit to watts per metre kelvin.
 *
 * Converts a given power from some unit to watts per metre kelvin.
 *
 * @param x Power to convert.
 * @param unit Original unit.
 * @return The power in watts per metre kelvin. [W/mK]
 */
double to_W_per_m_K(const double& x, const std::string& unit);

#endif
