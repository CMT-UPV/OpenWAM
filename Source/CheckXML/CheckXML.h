#ifndef CHECKXML_H
#define CHECKXML_H

#include <iostream>
#include <string>
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


/**
 * @brief Gets an attribute as a length.
 * 
 * Gets an attribute as a length.
 * 
 * @param node XML node.
 * @param name Attribute name.
 * @param unit Original unit.
 * @return Length. [m]
 */
double GetXMLLength(const xml_node& node, const std::string& name,
                    const std::string& unit);


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
 * @brief Converts a given value from some unit to meters.
 * 
 * Converts a given value from some unit to meters.
 * 
 * @param x Size to convert.
 * @param unit Original unit.
 * @return The size in meters. [m]
 */
double to_meters(const double& x, const std::string& unit);

#endif
