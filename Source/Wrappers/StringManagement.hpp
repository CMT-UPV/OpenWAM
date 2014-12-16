#ifndef StringManagement_hpp
#define StringManagement_hpp

#define RC_FILE "Labels.rc"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

/**
 * @file StringManagement.hpp
 * @author Luis Miguel Garcia-Cuevas Gonzalez <luiga12@mot.upv.es>
 * @version 0.2
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
 * StringManagement has several functions that mimic previous non-standard
 * approaches to manage strings. This file contains the interface of them.
 *
 */

/**
 * @brief Custom string class. Inherits from std::string.
 *
 * AnsiString accepts in its constructor doubles or integers, and has all the
 * methods inherited from std::string.
 */
class AnsiString: public std::string {
public:
	/**
	 * Default constructor.
	 */
	AnsiString();

	/**
	 * Versatile constructor.
	 *
	 * Initializes the object. It accepts as parameter strings, integers
	 * or doubles.
	 *
	 * @param x Variable to convert to string.
	 */
	template<class T>
	AnsiString(T x) :
			std::string() {
		std::stringstream ss;
		ss << x;
		assign(ss.str());
	}
};

/**
 * @brief Returns a string from the ressource file.
 *
 * Loads the ressource file, searchs for a string and returns it.
 *
 * @param ressource Integer that corresponds to the wanted string.
 * @return The string of class AnsiString that corresponds to the parameter
 * ressource.
 */
AnsiString LoadStr(int ressource);

/**
 * Compares if two strings are equal or not.
 *
 * @param strA First string of class AnsiString.
 * @param strB Second strinf of class AnsiString.
 * @return 0 if both strings are equal, -1 otherwise.
 */
int CompareStr(AnsiString strA, AnsiString strB);

/**
 * Converts a double to a AnsiString
 *
 * @param x The double to convert.
 * @param dummy Dummy integer for compatibility.
 * @param a Requested precission.
 * @param b Dummy integer.
 * @return A string representation of x.
 */
AnsiString FloatToStrF(double x, int dummy, int a, int b);

typedef AnsiString IntToStr;
#endif
