#include "CheckXML.h"

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


double GetXMLPressure(const xml_node& node, const std::string& name)
{
	xml_node unit_node = node.child("Units");
	std::string unit = unit_node.attribute("Pressure").value();
	double x = GetAttributeAsDouble(node, name.c_str());
	return to_bar(x, unit);
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
	else if (unit == "C"){
		return x;
	}
	else if (unit == "ºC") {
		return x;
	}
	else if (unit == "°C") {
		return x;
	}
	else if (unit == "℃") {
		return x;
	}
	else if (unit == "K") {
		return x - 273.15;
	}
	else if (unit == "F") {
		return (x - 32.) * 5. / 9.;
	}
	else if (unit == "ºF") {
		return (x - 32.) * 5. / 9.;
	}
	else if (unit == "°F") {
		return (x - 32.) * 5. / 9.;
	}
	else if (unit == "℉") {
		return (x - 32.) * 5. / 9.;
	}
	else if (unit == "R")
	{
		return (x - 491.67) * 5. / 9.;
	}
	else if (unit == "ºR")
	{
		return (x - 491.67) * 5. / 9.;
	}
	else if (unit == "°R")
	{
		return (x - 491.67) * 5. / 9.;
	}
	else {
		std::cout << "ERROR: Unit unknown" << std::endl;
		std::cout << "       UNIT: " << unit << std::endl;
		std::cout << "       Assuming ºC..." << std::endl;
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
