#ifndef CHECKXML_H
#define CHECKXML_H

#include <iostream>
#include <string>
#include "Constantes.h"
#include "pugixml.hpp"
#include "units.hpp"

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
 * @brief Gets an attribute as a damping coefficient.
 *
 * Gets an attribute as a damping coefficient.
 *
 * @param node XML node.
 * @param name Attribute name.
 * @return Damping coefficient. [N * s / m]
 */
double GetXMLDamping(const xml_node& node, const std::string& name);

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
 * @brief Gets an attribute as a length.
 *
 * Gets an attribute as a length.
 *
 * @param node XML node.
 * @param name Attribute name.
 * @param unit Unit used in the node.
 * @return Length. [m]
 */
double GetXMLLength(const xml_node& node, const std::string& name,
		const std::string& unit);

/**
 * @brief Gets an attribute as a force.
 *
 * Gets an attribute as a force.
 *
 * @param node XML node.
 * @param name Attribute name.
 * @return Force. [N]
 */
double GetXMLForce(const xml_node& node, const std::string& name);

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

double GetXMLReducedMassFlow(const xml_node& node, const std::string& name);

double GetXMLReducedMassFlow(const xml_node& node, const std::string& name,
		const std::string& unit);

/**
 * @brief Gets an attribute as a reduced speed.
 *
 * Gets an attribute as a reduced speed.
 *
 * @param node XML node.
 * @param name Attribute name.
 * @return Reduced speed. [rps / sqrt(K)]
 */
double GetXMLReducedSpeed(const xml_node& node, const std::string& name);

/**
 * @brief Gets an attribute as a reduced speed.
 *
 * Gets an attribute as a reduced speed.
 *
 * @param node XML node.
 * @param name Attribute name.
 * @param unit Unit used in the node
 * @return Reduced speed. [rps / sqrt(K)]
 */
double GetXMLReducedSpeed(const xml_node& node, const std::string& name,
		const std::string& unit);

/**
 * @brief Gets an attribute as a mass.
 *
 * Gets an attribute as a mass.
 *
 * @param node XML node.
 * @param name Attribute name.
 * @return Mass. [kg]
 */
double GetXMLMass(const xml_node& node, const std::string& name);

/**
 * @brief Gets an attribute as a mass.
 *
 * Gets an attribute as a mass.
 *
 * @param node XML node.
 * @param name Attribute name.
 * @param unit Unit used in the node.
 * @return Mass. [kg]
 */
double GetXMLMass(const xml_node& node, const std::string& name,
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
 * @brief Gets an attribute as a rotational speed.
 *
 * Gets an attribute as a rotational speed.
 *
 * @param node XML node.
 * @param name Attribute name.
 * @param unit Unit used in the node
 * @return Rotational speed. [rpm]
 */
double GetXMLRotationalSpeed(const xml_node& node, const std::string& name,
		const std::string& unit);

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
 * @brief Gets an attribute as a stiffness.
 *
 * Gets an attribute as a stiffness.
 *
 * @param node XML node.
 * @param name Attribute name.
 * @return Stiffness. [N / m]
 */
double GetXMLStiffness(const xml_node& node, const std::string& name);

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
 * @brief Gets an attribute as a Volume
 *
 * Gets an attribute as a volume
 *
 * @param node XML node.
 * @param name Attribute name
 * @return Volume [m3]
 */
double GetXMLVolume(const xml_node& node, const std::string& name);

/**
 * @brief Gets an attribute as a Volume
 *
 * Gets an attribute as a volume
 *
 * @param node XML node.
 * @param name Attribute name
 * @param unit Unit used in the node
 * @return Volume [m3]
 */
double GetXMLVolume(const xml_node& node, const std::string& name,
		const std::string& unit);

#endif
