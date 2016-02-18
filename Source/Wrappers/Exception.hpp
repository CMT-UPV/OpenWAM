#ifndef Exception_hpp
#define Exception_hpp

/**
 * @file Exception.hpp
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
 * This file contains the declaration of a the Exception class.
 */

#include <string>
#include <exception>

using namespace std;

/**
 * @brief Custom exception class.
 *
 * A custom exception class
 */
class Exception: public exception {
  private:
	string msg;
  public:
	Exception();
	Exception(const string& __arg);
	virtual ~Exception();
	virtual const char* what();
};
#endif
