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
 * @return Temperature. [ºC]
 */
double GetXMLTemperature(const xml_node& node, const std::string& name);


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
 * @return The temperature. [ºC]
 */
double to_celsius(const double& x, const std::string& unit);


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
 * @brief Converts a given area from some unit to square metres.
 * 
 * Converts a given area from some unit to square metres.
 * 
 * @param x Area to convert.
 * @param unit Original unit.
 * @return The area in square metres. [m ** 2]
 */
double to_square_metres(const double& x, const std::string& unit);

#endif
