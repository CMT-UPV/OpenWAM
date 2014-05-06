#include "pugixml.hpp"

using namespace pugi;

xml_node GetNodeChild(xml_node node, char* name) {
	if (node.child(name).empty()) {
		cout << "ERROR: Child node not found" << endl;
		cout << "       NODE:   " << name << endl;
		cout << "       PARENT: " << node.name() << endl;
	}
	return node.child(name);
}

double GetAttributeAsDouble(xml_node node, char* name) {
	if (node.attribute(name).empty()) {
		cout << "ERROR: Attribute not found" << endl;
		cout << "       NODE:      " << node.name() << endl;
		cout << "       ATTRIBUTE: " << name << endl;
	}
	return node.attribute(name).as_double();
}

int GetAttributeAsInt(xml_node node, char* name) {
	if (node.attribute(name).empty()) {
		cout << "ERROR: Attribute not found" << endl;
		cout << "       NODE:      " << node.name() << endl;
		cout << "       ATTRIBUTE: " << name << endl;
	}
	return node.attribute(name).as_int();
}

int GetAttributeAsBool(xml_node node, char* name) {
	if (node.attribute(name).empty()) {
		cout << "ERROR: Attribute not found" << endl;
		cout << "       NODE:      " << node.name() << endl;
		cout << "       ATTRIBUTE: " << name << endl;
	}
	return node.attribute(name).as_bool();
}

double GetAttributeAsDouble(xml_attribute attrib) {
	if (attrib.empty()) {
		cout << "ERROR: Attribute not found" << endl;
		//cout << "       NODE:      " << node.name() << endl;
		cout << "       ATTRIBUTE: " << attrib.name() << endl;
	}
	return attrib.as_double();
}

int GetAttributeAsInt(xml_attribute attrib) {
	if (attrib.empty()) {
		cout << "ERROR: Attribute not found" << endl;
		//cout << "       NODE:      " << node.name() << endl;
		cout << "       ATTRIBUTE: " << attrib.name() << endl;
	}
	return attrib.as_int();
}

int GetAttributeAsBool(xml_attribute attrib) {
	if (attrib.empty()) {
		cout << "ERROR: Attribute not found" << endl;
		//cout << "       NODE:      " << node.name() << endl;
		cout << "       ATTRIBUTE: " << attrib.name() << endl;
	}
	return attrib.as_bool();
}

int CountNodes(xml_node node, char* label) {

	int total = 0;
	for (xml_node node_child = GetNodeChild(node, label); node_child;
		node_child = node_child.next_sibling(label)) {
		total++;
	}
    return total;
}
