/*--------------------------------------------------------------------------------*\
==========================|
 \\   /\ /\   // O pen     | OpenWAM: The Open Source 1D Gas-Dynamic Code
 \\ |  X  | //  W ave     |
 \\ \/_\/ //   A ction   | CMT-Motores Termicos / Universidad Politecnica Valencia
 \\/   \//    M odel    |
 ----------------------------------------------------------------------------------
 License

 This file is part of OpenWAM.

 OpenWAM is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 OpenWAM is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with OpenWAM.  If not, see <http://www.gnu.org/licenses/>.


 \*--------------------------------------------------------------------------------*/

/* Indices de las Variables de entrada escalares "fijas", es decir, que no cambian de ejecucion en ejecucion */

#define ACT_PISTON_D                0
#define ACT_S                       1
#define ACT_CRANK_L                 2
#define ACT_CONNECTING_ROD_L        3
#define ACT_E                       4
#define ACT_PISTON_AXIS_D           5
#define ACT_PISTON_CROWN_H          6
#define ACT_DBOWL                   7
#define ACT_VBOWL                   8
#define ACT_M_CONNECTING_ROD        9
#define ACT_M_P_R_PA                10
#define ACT_C_ESTEEL                11
#define ACT_C_MECH_DEFOR            12
#define ACT_C_MBLBY                 13
#define ACT_GCRATIO                 14
#define ACT_N_HOLES                 15
#define ACT_NOZZLE_D                16
#define ACT_DC                      17
#define ACT_CU                      18
#define ACT_WC1A                    19
#define ACT_WC1B                    20
#define ACT_C_W2                    21
#define ACT_IVC                     22
#define ACT_EVO                     23
#define ACT_K                       24

/* Indices de las Variables de entrada escalares que pueden variar en cada ejecucion */

#define ACT_CBB                     0
#define ACT_ATMOSPHERE_PRESS        1
#define ACT_TEMPERATUREIVC          2
#define ACT_SPEED                   3
#define ACT_SOI                     4
#define ACT_EOI                     5
#define ACT_MAIRIVC                 6
#define ACT_YO2IVC                  7
#define ACT_INJ_FUEL_TEMP           8
#define ACT_TCYL_HEAD               9
#define ACT_TCYL                    10
#define ACT_TPIS                    11

/* Indices de las Variables de Salida escalares */

#define ACT_IMP_HP                  0
#define ACT_PMAX                    1
#define ACT_TMAX                    2
#define ACT_P_EXIT                  3
#define ACT_T_EXIT                  4
#define ACT_DP_DA                   5

/* Indices de las Variables de Salida vectoriales */

#define ACT_P_CYL                   0
#define ACT_T_CYL                   1
#define ACT_HRF                     2
#define ACT_ROHR                    3
#define ACT_H_COOLER                4

