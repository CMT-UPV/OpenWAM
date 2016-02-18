/**
 * @file labels.cpp
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
 * The function defined here is used to get labels using an index.
 */

#include "labels.hpp"

namespace OpenWAM_labels {
	std::map<int, std::string> labels;
}

using namespace OpenWAM_labels;

void init_labels() {
	labels[10000] = "EN";

	labels[1011] = "Inflow_Discharge_Coef_Node_";
	labels[1012] = "Outflow_Discharge_Coef_Node_";
	labels[1013] = "Lift_Wate-Gate_";
	labels[1014] = "Lift_Sheet_";

	labels[1021] = "Power_comp_";
	labels[1022] = "Efficiency_comp_";
	labels[1023] = "Comp_Ratio_comp_";
	labels[1024] = "Mass_Flow_comp_";
	labels[1025] = "Corrected_Mass_Flow_comp_";
	labels[1026] = "Corrected_Speed_comp_";
	labels[1027] = "Specific_Heats_Ratio_comp_";
	labels[1028] = "Work_comp_";

	labels[1301] = "P_duct_";
	labels[1302] = "V_duct_";
	labels[1303] = "T_duct_";
	labels[1304] = "F_duct_";
	labels[1305] = "V_Right_duct_";
	labels[1306] = "V_Left_duct_";
	labels[1307] = "P_to_Rigth_duct_";
	labels[1308] = "P_to_Left_duct_";
	labels[1309] = "NIT_duct_";
	labels[1310] = "WT0_duct_";
	labels[1311] = "WT1_duct_";
	labels[1312] = "WT2_duct_";
	labels[1313] = "Internal_Heat_Trans_Coef_duct_";
	labels[1314] = "Y_";
	labels[1315] = "Specific_Heats_Ratio_";
	labels[1316] = "_at_";
	labels[1317] = "_m";
	labels[1318] = "_duct_";

	labels[1401] = "Power_Roots_Comp_(BC)_";
	labels[1402] = "Mass_Flow_Roots_Comp_(BC)_";
	labels[1403] = "Output_Pressure_Roots_Comp_(BC)_";

	labels[1411] = "Mass_Flow_Volumes_Connection_(BC)_";

	labels[1501] = "Pressure_plenum_";
	labels[1502] = "Temperature_plenum_";
	labels[1503] = "Volume_plenum_";
	labels[1504] = "Mass_plenum_";
	labels[1505] = "Y_";
	labels[1506] = "_plenum_";
	labels[1507] = "Specific_Heat_ratio_plenum_";

	labels[1511] = "Work_turb_";
	labels[1512] = "Efficiency_turb_";
	labels[1513] = "Blade_speed_ratio_";
	labels[1514] = "_turb_";
	labels[1515] = "Global_blade_speed_ratio_turb_";
	labels[1516] = "Corrected_air_mass_flow_";
	labels[1517] = "Corrected_speed_";
	labels[1518] = "Expansion_ratio_";
	labels[1519] = "Power_turb_";

	labels[1521] = "Pre_entry_vent_";
	labels[1522] = "Pre_throat_vent_";
	labels[1523] = "Mach_entry_vent_";
	labels[1524] = "Mach_throat_vent_";
	labels[1525] = "Vel_entry_vent_";
	labels[1526] = "Vel_throat_vent_";
	labels[1527] = "Gasto_ent_vent_";
	labels[1528] = "Gasto_lat_vent_";

	labels[1601] = "Net_Torque(Nm)";
	labels[1602] = "Effective_Torque(Nm)";
	labels[1603] = "Cicle_Effective_Torque(Nm)";
	labels[1604] = "Torque_of_mechanical_losses(Nm)";
	labels[1605] = "Net_Work(J)";
	labels[1606] = "Pumping_Work(J)";
	labels[1607] = "NMEP(bar)";
	labels[1608] = "BMEP(bar)";
	labels[1609] = "NMEP_Cycle(bar)";
	labels[1610] = "BMEP_Cycle(bar)";
	labels[1611] = "IMEP_Cycle(bar)";
	labels[1612] = "PMEP_Cycle(bar)";
	labels[1613] = "Power(kW)";
	labels[1614] = "Power_Cycle(kW)";
	labels[1615] = "Intake_Mass(kg/cc)";
	labels[1616] = "Fuel_Mass(kg/cc)";
	labels[1617] = "Trapped_Mass(kg/cc)";
	labels[1618] = "Engine_Speed(rpm)";
	labels[1619] = "Volumetric_Efficiency(-)";
	labels[1620] = "Volumetric_Efficiency_Atm(-)";
	labels[1621] = "Effective_Efficiency(-)";
	labels[1622] = "Indicated_Efficiency(-)";
	labels[1623] = "Break_Specific_Fuel_Consumption(g/kWh)";
	labels[1624] = "Resistant_Torque(Nm)";
	labels[1625] = "Vehicle_Speed(km/h)";
	labels[1626] = "Fuel_To_Air_Ratio(-)";
	labels[1627] = "AFR(-)";
	labels[1628] = "Swirl(-)";

	labels[1631] = "Net_Work_Cyl_";
	labels[1632] = "NMEP_Cyl_";
	labels[1633] = "Pumping_Work_Cyl_";
	labels[1634] = "PMEP_Cyl_";
	labels[1635] = "Combustion_Heat_Cyl_";
	labels[1636] = "Heat_Through_Liner_Cyl_";
	labels[1637] = "Heat_Through_Cylinder_Head_Cyl_";
	labels[1638] = "Heat_Through_Piston_Cyl_";
	labels[1639] = "IMEP_Cyl_";
	labels[1640] = "Trapped_Mass_Cyl_";
	labels[1641] = "Temp_Liner_Int_Cyl_";
	labels[1642] = "Temp_Liner_Med_Cyl_";
	labels[1643] = "Temp_Liner_Ext_Cyl_";
	labels[1644] = "Temp_Piston_Int_Cyl_";
	labels[1645] = "Temp_Piston_Med_Cyl_";
	labels[1646] = "Temp_Piston_Ext_Cyl_";
	labels[1647] = "Temp_CylHead_Int_Cyl_";
	labels[1648] = "Temp_CylHead_Med_Cyl_";
	labels[1649] = "Temp_CylHead_Ext_Cyl_";
	labels[1650] = "Mean_NIT_ValvExh_";
	labels[1651] = "_Cyl_";
	labels[1652] = "Mean_NIT_Cyl_";
	labels[1653] = "Mean_AFR_Cyl_";
	labels[1654] = "BlowBy_Mass_Cyl_";
	labels[1655] = "Intake_Mass_Cyl_";
	labels[1656] = "Exhaust_Mass_Cyl_";
	labels[1657] = "Short_Circuit_Mass_Cyl_";
	labels[1658] = "Average_Temperature_Cyl_";
	labels[1659] = "Average_Swirl_Cyl_";
	labels[1660] = "Volumetric_Efficiency_Cyl_";

	labels[1661] = "Pressure_Cyl_";
	labels[1662] = "Temperature_Cyl_";
	labels[1663] = "Angular_Momentum_Valv_Exh_";
	labels[1664] = "Angular_Momentum_Exh_Total_Cyl_";
	labels[1665] = "Angular_Momentum_Valv_Int_";
	labels[1666] = "Angular_Momentum_Int_Total_Cyl_";
	labels[1667] = "Mass_Flow_Exh_Valve_";
	labels[1668] = "Total_Exh_Mass_Flow_Cyl_";
	labels[1669] = "Mass_Flow_Int_Valve_";
	labels[1670] = "Total_Int_Mass_Flow_Cyl_";
	labels[1671] = "Mach_Valv_Exh_";
	labels[1672] = "Mach_Valv_Int_";
	labels[1673] = "Effective_Section_Valv_Exh_";
	labels[1674] = "Effective_Section_Valv_Exh_Total_Cyl_";
	labels[1675] = "Effective_Section_Valv_Int_";
	labels[1676] = "Effective_Section_Valv_Int_Total_Cyl_";
	labels[1677] = "Mass_Cyl_";
	labels[1678] = "Volume_Cyl_";
	labels[1679] = "Woshni_Coef_Cyl_";
	labels[1680] = "Fuel_Mass_Cyl_";
	labels[1681] = "HRL_Cyl_";
	labels[1682] = "NIT_Valv_Exh_";
	labels[1683] = "NIT_Total_Cyl_";
	labels[1684] = "Torque_Cyl_";
	labels[1685] = "Short_Circuit_Flow_Cyl_";
	labels[1686] = "BlowBy_Cyl_";
	labels[1687] = "Y_";
	labels[1688] = "Gamma_Cyl_";
	labels[1689] = "Heat_Cyl_Head_Cyl_";
	labels[1690] = "Heat_Cylinder_Cyl_";
	labels[1691] = "Heat_Piston_Cyl_";

	labels[1701] = "Time(s)";
	labels[1702] = "Angle(deg)";
	labels[1703] = "Speed_Turbocharger_Shaft_";
	labels[1704] = "Cycle(-)";
	labels[1705] = "Output_Controller_";
	labels[1706] = "Error_PID_";
	labels[1707] = "Output_Sensor_";
	labels[1708] = "Input_Sensor_";
	labels[1709] = "P_Action_PID_";
	labels[1710] = "I_Action_PID_";
	labels[1711] = "D_Action_PID_";
	labels[1712] = "Filt_Output_Controller_";
	labels[1713] = "Mechanical_power_turbo_";
	labels[1714] = "Mechanical_efficiency_turbo_";

	labels[2011] = "Coef_Descarga_Entrada_Nodo_";
	labels[2012] = "Coef_Descarga_Salida_Nodo_";
	labels[2013] = "Levantamiento_Waste-Gate_";
	labels[2014] = "Levantamiento_Lamina_";

	labels[2021] = "Potencia_comp_";
	labels[2022] = "Rendimiento_comp_";
	labels[2023] = "RelComp_comp_";
	labels[2024] = "Gasto_comp_";
	labels[2025] = "Gasto_corr_comp_";
	labels[2026] = "Regimen_corr_comp_";
	labels[2027] = "Gamma_comp_";
	labels[2028] = "Trabajo_comp_";

	labels[2301] = "P_tubo_";
	labels[2302] = "V_tubo_";
	labels[2303] = "T_tubo_";
	labels[2304] = "G_tubo_";
	labels[2305] = "VDer_tubo_";
	labels[2306] = "VIzq_tubo_";
	labels[2307] = "PhaciaDer_tubo_";
	labels[2308] = "PhaciaIzq_tubo_";
	labels[2309] = "NIT_tubo_";
	labels[2310] = "TP0_tubo_";
	labels[2311] = "TP1_tubo_";
	labels[2312] = "TP2_tubo_";
	labels[2313] = "CoefPelInterior_tubo_";
	labels[2314] = "Y_";
	labels[2315] = "Gamma_tubo_";
	labels[2316] = "_a_";
	labels[2317] = "_m";
	labels[2318] = "_tubo_";

	labels[2401] = "Potencia_CV(CC)_";
	labels[2402] = "Gasto_CV(CC)_";
	labels[2403] = "Presion_Salida_CV(CC)_";

	labels[2411] = "Gasto_UnionEntreDep(CC)_";

	labels[2501] = "Presion_dep_";
	labels[2502] = "Temperatura_dep_";
	labels[2503] = "Volumen_dep_";
	labels[2504] = "Masa_dep_";
	labels[2505] = "Y_";
	labels[2506] = "_dep_";
	labels[2507] = "Gamma_dep_";

	labels[2511] = "Trabajo_turb_";
	labels[2512] = "Rendimiento_turb_";
	labels[2513] = "Relacion_cinematica_";
	labels[2514] = "_turb_";
	labels[2515] = "Relacion_cinematica_global_turb_";
	labels[2516] = "Gasto_corregido_";
	labels[2517] = "Regimen_corregido_";
	labels[2518] = "Relacion_expansion_";
	labels[2519] = "Potencia_turb_";

	labels[2521] = "Pre_entrada_vent_";
	labels[2522] = "Pre_garganta_vent_";
	labels[2523] = "Mach_entrada_vent_";
	labels[2524] = "Mach_garganta_vent_";
	labels[2525] = "Vel_entrada_vent_";
	labels[2526] = "Vel_garganta_vent_";
	labels[2527] = "Gasto_ent_vent_";
	labels[2528] = "Gasto_lat_vent_";

	labels[2601] = "Par_Neto(Nm)";
	labels[2602] = "Par_Efectivo(Nm)";
	labels[2603] = "Par_Efectivo_Ciclo(Nm)";
	labels[2604] = "Par_Perdidas_Mecanicas(Nm)";
	labels[2605] = "Trabajo_Neto(J)";
	labels[2606] = "Trabajo_Bombeo(J)";
	labels[2607] = "PMN(bar)";
	labels[2608] = "PME(bar)";
	labels[2609] = "PMN_Ciclo(bar)";
	labels[2610] = "PME_Ciclo(bar)";
	labels[2611] = "PMI_Ciclo(bar)";
	labels[2612] = "PMB_Ciclo(bar)";
	labels[2613] = "Potencia(kW)";
	labels[2614] = "Potencia_Ciclo(kW)";
	labels[2615] = "Masa_Admision(kg/cc)";
	labels[2616] = "Masa_Fuel(kg/cc)";
	labels[2617] = "Masa_Atrapada(kg/cc)";
	labels[2618] = "Regime_Giro(rpm)";
	labels[2619] = "Rendimiento_Volumetrico(-)";
	labels[2620] = "Rendimiento_Volumetrico_Atm(-)";
	labels[2621] = "Rendimiento_Efectivo(-)";
	labels[2622] = "Rendimiento_Indicado_Atm(-)";
	labels[2623] = "Consumo_Especifico(g/kWh)";
	labels[2624] = "Par_Resistente(Nm)";
	labels[2625] = "Velocidad_Vehiculo(km/h)";
	labels[2626] = "Dosado(-)";
	labels[2627] = "AFR(-)";
	labels[2628] = "Swirl(-)";

	labels[2631] = "Trabajo_Neto_Cil_";
	labels[2632] = "Presion_Media_Neta_Cil_";
	labels[2633] = "Trabajo_Bombeo_Cil_";
	labels[2634] = "Presion_Media_Bombeo_Cil_";
	labels[2635] = "Calor_Combustion_Cil_";
	labels[2636] = "Calor_Cilindro_Cil_";
	labels[2637] = "Calor_Culata_Cil_";
	labels[2638] = "Calor_Piston_Cil_";
	labels[2639] = "Presion_Media_Indicada_Cil_";
	labels[2640] = "Masa_Atrapada_Cil_";
	labels[2641] = "Temp_Cil_Int_Cil_";
	labels[2642] = "Temp_Cil_Med_Cil_";
	labels[2643] = "Temp_Cil_Ext_Cil_";
	labels[2644] = "Temp_Pis_Int_Cil_";
	labels[2645] = "Temp_Pis_Med_Cil_";
	labels[2646] = "Temp_Pis_Ext_Cil_";
	labels[2647] = "Temp_Cul_Int_Cil_";
	labels[2648] = "Temp_Cul_Med_Cil_";
	labels[2649] = "Temp_Cul_Ext_Cil_";
	labels[2650] = "NIT_Medio_ValvEsc_";
	labels[2651] = "_cil_";
	labels[2652] = "NIT_Medio_Cil_";
	labels[2653] = "AFR_Medio_Cil_";
	labels[2654] = "GastoBlowBy_Medio_Cil_";
	labels[2655] = "MasaAdmision_Medio_Cil_";
	labels[2656] = "MasaEscape_Medio_Cil_";
	labels[2657] = "MasaCortocircuito_Medio_Cil_";
	labels[2658] = "TemperaturaMediaCiclo_Cil_";
	labels[2659] = "Swirl_Medio_Cil_";
	labels[2660] = "RendVolumetrico_Medio_Cil_";

	labels[2661] = "Presion_cil_";
	labels[2662] = "Temperatura_cil_";
	labels[2663] = "Momento_Angular_Valv_Esc_";
	labels[2664] = "Momento_Angular_Esc_Total_cil_";
	labels[2665] = "Momento_Angular_Valv_Adm_";
	labels[2666] = "Momento_Angular_Adm_Total_cil_";
	labels[2667] = "Gasto_Valv_Esc_";
	labels[2668] = "Gasto_Valv_Esc_Total_cil_";
	labels[2669] = "Gasto_Valv_Adm_";
	labels[2670] = "Gasto_Valv_Adm_Total_cil_";
	labels[2671] = "Mach_Valv_Esc_";
	labels[2672] = "Mach_Valv_Adm_";
	labels[2673] = "Seccion_Efectiva_Valv_Esc_";
	labels[2674] = "Seccion_Efectiva_Valv_Esc_Total_cil_";
	labels[2675] = "Seccion_Efectiva_Valv_Adm_";
	labels[2676] = "Seccion_Efectiva_Valv_Adm_Total_cil_";
	labels[2677] = "Masa_cil_";
	labels[2678] = "Volumen_cil_";
	labels[2679] = "Coeficiente_Woshni_cil_";
	labels[2680] = "Masa_Fuel_cil_";
	labels[2681] = "FQL_cil_";
	labels[2682] = "NIT_Valv_Esc_";
	labels[2683] = "NIT_Total_cil_";
	labels[2684] = "Par_Instantaneo_cil_";
	labels[2685] = "Gasto_Cortocircuito_cil_";
	labels[2686] = "Gasto_BlowBy_cil_";
	labels[2687] = "Y_";
	labels[2688] = "Gamma_cil_";

	labels[2701] = "Tiempo(s)";
	labels[2702] = "Angulo(deg)";
	labels[2703] = "Regimen_Eje_";
	labels[2704] = "Ciclo(-)";
	labels[2705] = "Salida_Controlador_";
	labels[2706] = "Error_PID_";
	labels[2707] = "Salida_Sensor_";
	labels[2708] = "Entrada_Sensor_";
	labels[2709] = "Accion_P_PID_";
	labels[2710] = "Accion_I_PID_";
	labels[2711] = "Accion_D_PID_";
	labels[2712] = "Salida_filt_Controlador_";

	labels[901] = "(-)";
	labels[902] = "(m)";
	labels[903] = "(W)";
	labels[904] = "(kg/s)";
	labels[905] = "(kg/s*T^0.5/MPa)";
	labels[906] = "(rpm/K^0.5)";
	labels[907] = "(J)";
	labels[908] = "(bar)";
	labels[909] = "(m/s)";
	labels[910] = "(degC)";
	labels[911] = "(W/m^2/K)";
	labels[912] = "(m^3)";
	labels[913] = "(kg)";
	labels[914] = "(kg/cc)";
	labels[915] = "(m^2)";
	labels[916] = "(mg)";
	labels[917] = "(Nm)";
	labels[918] = "(rpm)";
	labels[919] = "(g)";
	labels[920] = "(mm)";
	labels[921] = "(1/m2)";
	labels[922] = "(1/s)";

	labels[1800] = "DPF_";
	labels[1801] = "_beam_";
	labels[1802] = "VP_IC_";
	labels[1803] = "VP_OC_";
	labels[1804] = "Soot_mass";
	labels[1805] = "Soot_thickness_";
	labels[1806] = "k_wall_";
	labels[1807] = "k_soot_";
	labels[1808] = "Filtration_efficiency_";
	labels[1809] = "Porosity_";
	labels[1810] = "Partition_coef_";
	labels[1811] = "Diameter_UC_";
	labels[1812] = "Kreg_thermal_";
	labels[1813] = "Kreg_cat_";
	labels[1814] = "Qreg_";
	labels[1815] = "T_wall_IC_";
	labels[1816] = "T_wall_middle_";
	labels[1817] = "T_wall_OC_";
	labels[1818] = "T_sup_ext_";
	labels[1819] = "T_sup_middle_";
	labels[1820] = "T_sup_int_";
	labels[1821] = "Y_";
	labels[1822] = "_wall_outlet_";
	labels[1823] = "T_";
	labels[1824] = "P_";
	labels[1825] = "V_";
	labels[1826] = "G_";
	labels[1827] = "Internal_Heat_Transfer_coef_";
	labels[1828] = "Gamma_";
	labels[1829] = "V_to_right_";
	labels[1830] = "V_to_left_";
	labels[1831] = "P_to_right_";
	labels[1832] = "P_to_left_";

	labels[2800] = "DPF_";
	labels[2801] = "_Haz_";
	labels[2802] = "VP_CE_";
	labels[2803] = "VP_CS_";
	labels[2804] = "Masa_soot_";
	labels[2805] = "Espesor_soot_";
	labels[2806] = "k_wall_";
	labels[2807] = "k_soot_";
	labels[2808] = "Eficiencia_filtrado_";
	labels[2809] = "Porosidad_";
	labels[2810] = "Coef_particion_";
	labels[2811] = "Diametro_UC_";
	labels[2812] = "Kreg_termica_";
	labels[2813] = "Kreg_cat_";
	labels[2814] = "Qreg_";
	labels[2815] = "T_pared_CE_";
	labels[2816] = "T_pared_intermedia_";
	labels[2817] = "T_pared_CS_";
	labels[2818] = "T_sup_ext_";
	labels[2819] = "T_sup_med_";
	labels[2820] = "T_sup_int_";
	labels[2821] = "Y_";
	labels[2822] = "_salida_pared_";
	labels[2823] = "T_";
	labels[2824] = "P_";
	labels[2825] = "V_";
	labels[2826] = "G_";
	labels[2827] = "Coef_Pel_Interior_";
	labels[2828] = "Gamma_";
	labels[2829] = "V_hacia_der_";
	labels[2830] = "V_hacia_izq_";
	labels[2831] = "P_hacia_der_";
	labels[2832] = "P_hacia_izq_";
}

#include <iostream>

using namespace std;

std::string PutLabel(int idx) {
	if(idx < 900) {
		if(labels[10000].compare("ES") == 0) {
			idx += 2000;
		} else if(labels[10000].compare("EN") == 0) {
			idx += 1000;
		}
	}
	return labels[idx];
}
