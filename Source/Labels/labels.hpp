#ifndef LABELS_HPP
#define LABELS_HPP

/**
 * @file labels.hpp
 * @author Luis Miguel Garcia-Cuevas Gonzalez <luiga12@mot.upv.es>
 * @version 0.1
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
 * The function declared here is used to get labels using an index.
 */

#include <map>
#include <string>

namespace OpenWAM_labels {
	extern std::map<int, std::string> labels;
}

/**
 * @brief Initialises the labels.
 */
void init_labels();

/**
 * @brief Returns an integer.
 *
 * Given an integer, it returns a label.
 *
 * @param idx Label index.
 * @return Label.
 */
std::string PutLabel(int idx);

#endif
