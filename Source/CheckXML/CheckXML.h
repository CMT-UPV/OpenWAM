#ifndef CHECKXML_H
#define CHECKXML_H

#include <iostream>
#include <string>
#include "pugixml.hpp"


/**
 * @file CheckXML.h
 * @author Francisco Jose Arnau Martinez <farnau@mot.upv.es>
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
 * The functions declared in this file are used to get the values of
 * XML attributes.
 * 
 * This file contains the interfaces to such functions.
 */


using namespace pugi;


/**
 * @brief Gets the child of a node.
 * 
 * Given a node and a name of a child, gets its child.
 * 
 * @param node Node.
 * @param name Child name.
 * @return The child of the node.
 */
xml_node GetNodeChild(xml_node node, const char* name);


/**
 * @brief Gets an attribute as a double.
 * 
 * Given a node and an attribute name, gets its value as a double.
 * 
 * @param node The node.
 * @param name Attribute name.
 * @return Attribute value, as a double.
 */
double GetAttributeAsDouble(xml_node node, const char* name);


/**
 * @brief Gets an attribute as an integer.
 * 
 * Given a node and an attribute name, gets its value as an integer.
 * 
 * @param node The node.
 * @param name Attribute name.
 * @return Attribute value, as a double.
 */
int GetAttributeAsInt(xml_node node, const char* name);


/**
 * @brief Gets an attribute as a boolean value.
 * 
 * Given a node and an attribute name, gets its value as a boolean.
 * 
 * @param node The node.
 * @param name Attribute name.
 * @return Attribute value, as a boolean.
 */
int GetAttributeAsBool(xml_node node, const char* name);


/**
 * @brief Gets an attribute as a double.
 * 
 * Given an attribute, gets its value as a double.
 * 
 * @param attrib The attribute.
 * @return Attribute value, as a double.
 */
double GetAttributeAsDouble(xml_attribute attrib);


/**
 * @brief Gets an attribute as an integer.
 * 
 * Given an attribute, gets its value as an integer.
 * 
 * @param attrib The attribute.
 * @return Attribute value, as a integer.
 */
int GetAttributeAsInt(xml_attribute attrib);


/**
 * @brief Gets an attribute as a boolean value.
 * 
 * Given an attribute, gets its value as a boolean.
 * 
 * @param attrib The attribute.
 * @return Attribute value, as a boolean.
 */
int GetAttributeAsBool(xml_attribute attrib);


/**
 * @brief Counts the number of children with a given name.
 * 
 * Given a node and a child label, counts the number of children of the
 * node that have the same label.
 * 
 * @param node The node.
 * @param label The children label.
 * @return The number of children.
 */
int CountNodes(xml_node node, const char* label);


/**
 * @brief Gets an attribute as an angle.
 * 
 * Gets an attribute as an angle.
 * 
 * @param node XML node.
 * @param name Attribute name.
 * @return Angle. [rad]
 */
double GetXMLAngle(const xml_node& node, const std::string& name);


/**
 * @brief Gets an attribute as an area.
 * 
 * Gets an attribute as an area.
 * 
 * @param node XML node.
 * @param name Attribute name.
 * @return Length. [m ** 2]
 */
double GetXMLArea(const xml_node& node, const std::string& name);


/**
 * @brief Gets an attribute as a length.
 * 
 * Gets an attribute as a length.
 * 
 * @param node XML node.
 * @param name Attribute name.
 * @return Length. [m]
 */
double GetXMLLength(const xml_node& node, const std::string& name);


/**
 * @brief Gets an attribute as a mass flow rate.
 * 
 * Gets an attribute as a mass flow rate.
 * 
 * @param node XML node.
 * @param name Attribute name.
 * @return Mass flow rate. [kg / s]
 */
double GetXMLMassFlow(const xml_node& node, const std::string& name);


/**
 * @brief Gets an attribute as a mass flow rate.
 * 
 * Gets an attribute as a mass flow rate.
 * 
 * @param node XML node.
 * @param name Attribute name.
 * @param unit Unit used in the node.
 * @return Mass flow rate. [kg / s]
 */
double GetXMLMassFlow(const xml_node& node, const std::string& name,
	const std::string& unit);


/**
 * @brief Gets an attribute as a pressure.
 * 
 * Gets an attribute as a pressure.
 * 
 * @param node XML node.
 * @param name Attribute name.
 * @return Pressure. [bar]
 */
double GetXMLPressure(const xml_node& node, const std::string& name);


/**
 * @brief Gets an attribute as a power.
 * 
 * Gets an attribute as a power.
 * 
 * @param node XML node.
 * @param name Attribute name.
 * @return Power. [W]
 */
double GetXMLPower(const xml_node& node, const std::string& name);


/**
 * @brief Gets an attribute as a rotational speed.
 * 
 * Gets an attribute as a rotational speed.
 * 
 * @param node XML node.
 * @param name Attribute name.
 * @return Rotational speed. [rpm]
 */
double GetXMLRotationalSpeed(const xml_node& node, const std::string& name);


/**
 * @brief Gets an attribute as a speed.
 * 
 * Gets an attribute as a speed.
 * 
 * @param node XML node.
 * @param name Attribute name.
 * @return Speed. [m / s]
 */
double GetXMLSpeed(const xml_node& node, const std::string& name);


/**
 * @brief Gets an attribute as a temperature.
 * 
 * Gets an attribute as a temperature.
 * 
 * @param node XML node.
 * @param name Attribute name.
 * @return Temperature. [degC]
 */
double GetXMLTemperature(const xml_node& node, const std::string& name);


/**
 * @brief Gets an attribute as a time.
 * 
 * Gets an attribute as a time.
 * 
 * @param node XML node.
 * @param name Attribute name.
 * @return Time. [s]
 */
double GetXMLTime(const xml_node& node, const std::string& name);


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
 * @brief Converts a given power from some unit to watts.
 * 
 * Converts a given power from some unit to watts.
 * 
 * @param x Power to convert.
 * @param unit Original unit.
 * @return The power in watts. [W]
 */
double to_watts(const double& x, const std::string& unit);

#endif
