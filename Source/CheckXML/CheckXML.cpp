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


double GetXMLLength(const xml_node& node, const std::string& name,
                      const std::string& unit)
{
	double x = GetAttributeAsDouble(node, name.c_str());
	return to_meters(x, unit);
}


double GetXMLLength(const xml_node& node, const std::string& name)
{
	std::string unit = node.attribute("unit").value();
	return GetXMLLength(node, name, unit);
}


double to_meters(const double& x, const std::string& unit)
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
		std::cout << "       Assuming meters..." << std::endl;
		return x;
	}
}
