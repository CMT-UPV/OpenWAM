#include "StringManagement.hpp"

/**
 * @file StringManagement.cpp
 * @author Luis Miguel García-Cuevas González <luiga12@mot.upv.es>
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
 * approaches to manage strings. This file contains the implementation of them.
 * 
 */

using namespace std;


AnsiString::AnsiString(): std::string() {}

AnsiString LoadStr(int ressource)
{
    /* Loads a string from a ressource file.
       Returns the string numbered as ressource. */
    std::string line;
    std::string file_name;
    stringstream number;
    char * home;
	size_t found;
	size_t first_pos;
	size_t last_pos;
    home = getenv("HOME");
    ifstream file(RC_FILE);
    if (!file.is_open())
    {
        file_name = AnsiString(home) + AnsiString("/.OpenWAM/") + AnsiString(RC_FILE);
        file.open(file_name.c_str());
    }
    if (!file.is_open())
    {
        file_name = AnsiString("/etc/OpenWAM/") + AnsiString(RC_FILE);
        file.open(file_name.c_str());
    }
    number << ressource;
    if (file.is_open())
    {
        while (file.good())
        {
            getline(file, line);
            found = line.find(number.str())
            if (found == 0)
            {
                first_pos = line.find("\"");
                second_pos = line.find("\"", first_pos + 1);
                return AnsiString(line.substr(first_pos + 1,
                    second_pos - first_pos - 1));
            }
        }
        file.close();
    }
    else std::cout << "Unable to open file \"" << RC_FILE << "\"" << std::endl ;
    return "";
}


int CompareStr(AnsiString strA, AnsiString strB)
{
    /* Compares two strings.
       Returns 0 if strA == strB. */
    if(strA.compare(strB) == 0)
        return 0;
    return -1;
}

AnsiString FloatToStrF(double x, int dummy, int a, int b)
{
    /* Converts a double to a std::string.
       Returns the string version of x. */
    std::ostringstream ss;
    ss.precision(a);
    ss << x;
    return ss.str();
}
