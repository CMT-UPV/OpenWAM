#ifndef CHECKXML_H
#define CHECKXML_H

#include <iostream>
#include "pugixml.hpp"

using namespace pugi;

xml_node GetNodeChild(xml_node node, const char* name);

double GetAttributeAsDouble(xml_node node, const char* name);

int GetAttributeAsInt(xml_node node, const char* name);

int GetAttributeAsBool(xml_node node, const char* name);

double GetAttributeAsDouble(xml_attribute attrib);

int GetAttributeAsInt(xml_attribute attrib);

int GetAttributeAsBool(xml_attribute attrib);

int CountNodes(xml_node node, const char* label);

#endif
