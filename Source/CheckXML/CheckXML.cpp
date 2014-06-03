#include "CheckXML.h"

/**
 * @file CheckXML.cpp
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
 * The functions defined in this file are used to get the values of
 * XML attributes.
 * 
 * This file contains the implementations of such functions.
 */

xml_node GetNodeChild(xml_node node, const char* name) {
	if (node.child(name).empty()) {
		std::cout << "ERROR: Child node not found" << std::endl;
		std::cout << "       NODE:   " << name << std::endl;
		std::cout << "       PARENT: " << node.name() << std::endl;
	}
	return node.child(name);
}

double GetAttributeAsDouble(xml_node node, const char* name) {
	if (node.attribute(name).empty()) {
		std::cout << "ERROR: Attribute not found" << std::endl;
		std::cout << "       NODE:      " << node.name() << std::endl;
		std::cout << "       ATTRIBUTE: " << name << std::endl;
	}
	return node.attribute(name).as_double();
}

int GetAttributeAsInt(xml_node node, const char* name) {
	if (node.attribute(name).empty()) {
		std::cout << "ERROR: Attribute not found" << std::endl;
		std::cout << "       NODE:      " << node.name() << std::endl;
		std::cout << "       ATTRIBUTE: " << name << std::endl;
	}
	return node.attribute(name).as_int();
}

int GetAttributeAsBool(xml_node node, const char* name) {
	if (node.attribute(name).empty()) {
		std::cout << "ERROR: Attribute not found" << std::endl;
		std::cout << "       NODE:      " << node.name() << std::endl;
		std::cout << "       ATTRIBUTE: " << name << std::endl;
	}
	return node.attribute(name).as_bool();
}

double GetAttributeAsDouble(xml_attribute attrib) {
	if (attrib.empty()) {
		std::cout << "ERROR: Attribute not found" << std::endl;
		//std::cout << "       NODE:      " << node.name() << std::endl;
		std::cout << "       ATTRIBUTE: " << attrib.name() << std::endl;
	}
	return attrib.as_double();
}

int GetAttributeAsInt(xml_attribute attrib) {
	if (attrib.empty()) {
		std::cout << "ERROR: Attribute not found" << std::endl;
		//std::cout << "       NODE:      " << node.name() << std::endl;
		std::cout << "       ATTRIBUTE: " << attrib.name() << std::endl;
	}
	return attrib.as_int();
}

int GetAttributeAsBool(xml_attribute attrib) {
	if (attrib.empty()) {
		std::cout << "ERROR: Attribute not found" << std::endl;
		//std::cout << "       NODE:      " << node.name() << std::endl;
		std::cout << "       ATTRIBUTE: " << attrib.name() << std::endl;
	}
	return attrib.as_bool();
}

int CountNodes(xml_node node, const char* label) {

	int total = 0;
	for (xml_node node_child = GetNodeChild(node, label); node_child;
		node_child = node_child.next_sibling(label)) {
		total++;
	}
    return total;
}


double GetXMLAngle(const xml_node& node, const std::string& name)
{
	xml_node unit_node = node.child("Units");
	std::string unit = unit_node.attribute("Angle").value();
	double x = GetAttributeAsDouble(node, name.c_str());
	return to_degrees(x, unit);
}


double GetXMLArea(const xml_node& node, const std::string& name)
{
	xml_node unit_node = node.child("Units");
	std::string unit = unit_node.attribute("Area").value();
	double x = GetAttributeAsDouble(node, name.c_str());
	return to_square_metres(x, unit);
}


double GetXMLDamping(const xml_node& node, const std::string& name)
{
	xml_node unit_node = node.child("Units");
	std::string unit = unit_node.attribute("Damping").value();
	double x = GetAttributeAsDouble(node, name.c_str());
	return to_N_s_per_m(x, unit);
}


double GetXMLForce(const xml_node& node, const std::string& name)
{
	xml_node unit_node = node.child("Units");
	std::string unit = unit_node.attribute("Force").value();
	double x = GetAttributeAsDouble(node, name.c_str());
	return to_N(x, unit);
}


double GetXMLLength(const xml_node& node, const std::string& name)
{
	xml_node unit_node = node.child("Units");
	std::string unit = unit_node.attribute("Length").value();
	double x = GetAttributeAsDouble(node, name.c_str());
	return to_metres(x, unit);
}


double GetXMLMassFlow(const xml_node& node, const std::string& name)
{
	xml_node unit_node = node.child("Units");
	std::string unit = unit_node.attribute("MassFlow").value();
	return GetXMLMassFlow(node, name, unit);
}


double GetXMLMassFlow(const xml_node& node, const std::string& name,
	const std::string& unit)
{
	double x = GetAttributeAsDouble(node, name.c_str());
	return to_kg_s(x, unit);
}

double GetXMLMass(const xml_node& node, const std::string& name)
{
	xml_node unit_node = node.child("Units");
	std::string unit = unit_node.attribute("Mass").value();
	return GetXMLMassFlow(node, name, unit);
}


double GetXMLMass(const xml_node& node, const std::string& name,
	const std::string& unit)
{
	double x = GetAttributeAsDouble(node, name.c_str());
	return to_kg(x, unit);
}

double GetXMLPower(const xml_node& node, const std::string& name)
{
	xml_node unit_node = node.child("Units");
	std::string unit = unit_node.attribute("Power").value();
	double x = GetAttributeAsDouble(node, name.c_str());
	return to_watts(x, unit);
}



double GetXMLPressure(const xml_node& node, const std::string& name)
{
	xml_node unit_node = node.child("Units");
	std::string unit = unit_node.attribute("Pressure").value();
	double x = GetAttributeAsDouble(node, name.c_str());
	return to_bar(x, unit);
}


double GetXMLRotationalSpeed(const xml_node& node, const std::string& name)
{
	xml_node unit_node = node.child("Units");
	std::string unit = unit_node.attribute("RotationalSpeed").value();
	double x = GetAttributeAsDouble(node, name.c_str());
	return to_rpm(x, unit);
}


double GetXMLSpeed(const xml_node& node, const std::string& name)
{
	xml_node unit_node = node.child("Units");
	std::string unit = unit_node.attribute("Speed").value();
	double x = GetAttributeAsDouble(node, name.c_str());
	return to_m_s(x, unit);
}


double GetXMLStiffness(const xml_node& node, const std::string& name)
{
	xml_node unit_node = node.child("Units");
	std::string unit = unit_node.attribute("Stiffness").value();
	double x = GetAttributeAsDouble(node, name.c_str());
	return to_N_per_m(x, unit);
}


double GetXMLTemperature(const xml_node& node, const std::string& name)
{
	xml_node unit_node = node.child("Units");
	std::string unit = unit_node.attribute("Temperature").value();
	double x = GetAttributeAsDouble(node, name.c_str());
	return to_celsius(x, unit);
}


double GetXMLTime(const xml_node& node, const std::string& name)
{
	xml_node unit_node = node.child("Units");
	std::string unit = unit_node.attribute("Time").value();
	double x = GetAttributeAsDouble(node, name.c_str());
	return to_seconds(x, unit);
}
