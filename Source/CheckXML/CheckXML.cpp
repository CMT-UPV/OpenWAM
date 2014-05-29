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
	std::string unit = unit_node.attribute("Time").value();
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
	std::string unit = unit_node.attribute("Rotational_Speed").value();
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


double to_bar(const double& x, const std::string& unit)
{
	if (unit == ""){
		return x;
	}
	else if (unit == "bar"){
		return x;
	}
	else if (unit == "atm") {
		return x / 1.01325;
	}
	else if (unit == "mbar"){
		return x / 1000.;
	}
	else if (unit == "Pa"){
		return x / 1e5;
	}
	else if (unit == "hPa") {
		return x / 1000.;
	}
	else if (unit == "kPa") {
		return x / 100.;
	}
	else if (unit == "MPa") {
		return x * 10.;
	}
	else if (unit == "PSI") {
		return x * 0.0689476;
	}
	else {
		std::cout << "ERROR: Unit unknown" << std::endl;
		std::cout << "       UNIT: " << unit << std::endl;
		std::cout << "       Assuming bar..." << std::endl;
		return x;
	}
}


double to_celsius(const double& x, const std::string& unit)
{
	if (unit == ""){
		return x;
	}
	else if (unit == "degC") {
		return x;
	}
	else if (unit == "K") {
		return x - 273.15;
	}
	else if (unit == "F") {
		return (x - 32.) * 5. / 9.;
	}
	else if (unit == "degF") {
		return (x - 32.) * 5. / 9.;
	}
	else if (unit == "R")
	{
		return (x - 491.67) * 5. / 9.;
	}
	else if (unit == "degR")
	{
		return (x - 491.67) * 5. / 9.;
	}
	else {
		std::cout << "ERROR: Unit unknown" << std::endl;
		std::cout << "       UNIT: " << unit << std::endl;
		std::cout << "       Assuming degC..." << std::endl;
		return x;
	}
}


double to_degrees(const double& x, const std::string& unit)
{
	if (unit == "") {
		return x;
	}
	else if (unit == "grad") {
		return x;
	}
	else if (unit == "deg") {
		return x;
	}
	else if (unit == "degree") {
		return x;
	}
	else if (unit == "rad") {
		return x / 3.14159265;
	}
	else if (unit == "radian") {
		return x / 3.14159265;
	}
	else {
		std::cout << "ERROR: Unit unknown" << std::endl;
		std::cout << "       UNIT: " << unit << std::endl;
		std::cout << "       Assuming degrees..." << std::endl;
		return x;
	}
}


double to_kg_s(const double& x, const std::string& unit)
{
	if (unit == "") {
		return x;
	}
	else if (unit == "kg / s") {
		return x;
	}
	else if (unit == "kg/s") {
		return x;
	}
	else if (unit == "kg / min") {
		return x / 60.;
	}
	else if (unit == "kg/min") {
		return x / 60.;
	}
	else if (unit == "kg / h") {
		return x / 3600.;
	}
	else if (unit == "kg/h") {
		return x / 3600.;
	}
	else if (unit == "kg / hr") {
		return x / 3600.;
	}
	else if (unit == "kg/hr") {
		return x / 3600.;
	}
	else if (unit == "g / s") {
		return x / 1000.;
	}
	else if (unit == "g/s") {
		return x / 1000.;
	}
	else if (unit == "g / min") {
		return x / 1000. / 60.;
	}
	else if (unit == "g/min") {
		return x / 1000. / 60.;
	}
	else if (unit == "g / h") {
		return x / 1000. / 3600.;
	}
	else if (unit == "g/h") {
		return x / 1000. / 3600.;
	}
	else if (unit == "g / hr") {
		return x / 1000. / 3600.;
	}
	else if (unit == "g/hr") {
		return x / 1000. / 3600.;
	}
	else if (unit == "lb / s") {
		return x * 0.45359237;
	}
	else if (unit == "lb/s") {
		return x * 0.45359237;
	}
	else if (unit == "lb / min") {
		return x * 0.45359237 / 60.;
	}
	else if (unit == "lb/min") {
		return x * 0.45359237 / 60.;
	}
	else if (unit == "lb / h") {
		return x * 0.45359237 / 3600.;
	}
	else if (unit == "lb/h") {
		return x * 0.45359237 / 3600.;
	}
	else if (unit == "lb / hr") {
		return x * 0.45359237 / 3600.;
	}
	else if (unit == "lb/hr") {
		return x * 0.45359237 / 3600.;
	}
	else {
		std::cout << "ERROR: Unit unknown" << std::endl;
		std::cout << "       UNIT: " << unit << std::endl;
		std::cout << "       Assuming kg / s..." << std::endl;
		return x;
	}
}


double to_metres(const double& x, const std::string& unit)
{
	if (unit == ""){
		return x;
	}
	else if (unit == "m"){
		return x;
	}
	else if (unit == "mm") {
		return x / 1000.;
	}
	else if (unit == "cm"){
		return x / 100.;
	}
	else if (unit == "dm"){
		return x / 10.;
	}
	else if (unit == "inch") {
		return x * 2.54 / 100.;
	}
	else {
		std::cout << "ERROR: Unit unknown" << std::endl;
		std::cout << "       UNIT: " << unit << std::endl;
		std::cout << "       Assuming m..." << std::endl;
		return x;
	}
}


double to_m_s(const double& x, const std::string& unit)
{
	if (unit == ""){
		return x;
	}
	else if (unit == "m / s"){
		return x;
	}
	else if (unit == "m/s"){
		return x;
	}
	else if (unit == "km / hr") {
		return x / 3.6;
	}
	else if (unit == "km/hr") {
		return x / 3.6;
	}
	else if (unit == "km / h") {
		return x / 36.;
	}
	else if (unit == "km/h") {
		return x / 36.;
	}
	else if (unit == "mph"){
		return x * 1609.34 / 3600.;
	}
	else if (unit == "MPH"){
		return x * 1609.34 / 3600.;
	}
	else if (unit == "kn"){
		return x * 0.514444;
	}
	else if (unit == "kt") {
		return x * 0.514444;
	}
	else if (unit == "NMPH") {
		return x * 0.514444;
	}
	else if (unit == "nmph") {
		return x * 0.514444;
	}
	else {
		std::cout << "ERROR: Unit unknown" << std::endl;
		std::cout << "       UNIT: " << unit << std::endl;
		std::cout << "       Assuming m / s..." << std::endl;
		return x;
	}
}


double to_rpm(const double& x, const std::string& unit)
{
	if (unit == ""){
		return x;
	}
	else if (unit == "rpm"){
		return x;
	}
	else if (unit == "rad / s"){
		return x / (2. * 3.14159265) * 60.;
	}
	else if (unit == "rad/s") {
		return x / (2. * 3.14159265) * 60.;
	}
	else if (unit == "Hz") {
		return x * 60.;
	}
	else {
		std::cout << "ERROR: Unit unknown" << std::endl;
		std::cout << "       UNIT: " << unit << std::endl;
		std::cout << "       Assuming rpm..." << std::endl;
		return x;
	}
}


double to_seconds(const double& x, const std::string& unit)
{
	if (unit == ""){
		return x;
	}
	else if (unit == "s"){
		return x;
	}
	else if (unit == "ms"){
		return x / 1000.;
	}
	else if (unit == "m") {
		return x *  60.;
	}
	else if (unit == "min") {
		return x * 60.;
	}
	else if (unit == "h") {
		return x * 3600.;
	}
	else {
		std::cout << "ERROR: Unit unknown" << std::endl;
		std::cout << "       UNIT: " << unit << std::endl;
		std::cout << "       Assuming s..." << std::endl;
		return x;
	}
}


double to_square_metres(const double& x, const std::string& unit)
{
	if (unit == ""){
		return x;
	}
	else if (unit == "m ** 2"){
		return x;
	}
	else if (unit == "m**2") {
		return x;
	}
	else if (unit == "m ^ 2") {
		return x;
	}
	else if (unit == "m^2") {
		return x;
	}
	else if (unit == "mm ** 2") {
		return x * 1E-6;
	}
	else if (unit == "mm**2") {
		return x * 1E-6;
	}
	else if (unit == "mm ^ 2") {
		return x * 1E-6;
	}
	else if (unit == "mm^2") {
		return x * 1E-6;
	}
	else if (unit == "cm ** 2") {
		return x * 1E-4;
	}
	else if (unit == "cm**2") {
		return x * 1E-4;
	}
	else if (unit == "cm ^ 2") {
		return x * 1E-4;
	}
	else if (unit == "cm^2") {
		return x * 1E-4;
	}
	else if (unit == "dm ** 2") {
		return x * 1E-2;
	}
	else if (unit == "dm**2") {
		return x * 1E-2;
	}
	else if (unit == "dm ^ 2") {
		return x * 1E-2;
	}
	else if (unit == "dm^2") {
		return x * 1E-2;
	}
	else if (unit == "inch ** 2") {
		return x * 0.00064516;
	}
	else if (unit == "inch**2") {
		return x * 0.00064516;
	}
	else if (unit == "inch ^ 2") {
		return x * 0.00064516;
	}
	else if (unit == "inch^2") {
		return x * 0.00064516;
	}
	else if (unit == "in ** 2") {
		return x * 0.00064516;
	}
	else if (unit == "in**2") {
		return x * 0.00064516;
	}
	else if (unit == "in ^ 2") {
		return x * 0.00064516;
	}
	else if (unit == "in^2") {
		return x * 0.00064516;
	}
	else if (unit == "square in") {
		return x * 0.00064516;
	}
	else if (unit == "sq inches") {
		return x * 0.00064516;
	}
	else if (unit == "sq inch") {
		return x * 0.00064516;
	}
	else if (unit == "sq in") {
		return x * 0.00064516;
	}
	else {
		std::cout << "ERROR: Unit unknown" << std::endl;
		std::cout << "       UNIT: " << unit << std::endl;
		std::cout << "       Assuming m / s..." << std::endl;
		return x;
	}
}


double to_watts(const double& x, const std::string& unit)
{
	if (unit == ""){
		return x;
	}
	else if (unit == "W"){
		return x;
	}
	else if (unit == "kW"){
		return x / 1000.;
	}
	else if (unit == "CV") {
		return x *  735.498750;
	}
	else if (unit == "HP") {
		return x * 745.69987158227022;
	}
	else {
		std::cout << "ERROR: Unit unknown" << std::endl;
		std::cout << "       UNIT: " << unit << std::endl;
		std::cout << "       Assuming W..." << std::endl;
		return x;
	}
}
