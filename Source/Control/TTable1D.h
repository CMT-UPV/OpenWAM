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

//---------------------------------------------------------------------------
#ifndef TTable1DH
#define TTable1DH

#include "TController.h"
//#include "interp_1d.h"
//---------------------------------------------------------------------------

/*! This object is use to generate one-dimensional map to simulate ECU maps*/
class TTable1D: public TController {
  private:

	int fID;						//!< Controller ID number
	int fDimensiones;   			//!< Table dimensions (one in this case)
	dVector fX_map;					//!< Vector of the abcissas data
	dVector fY_map;     			//!< Vector of the ordinates data

	Base_interp *fDatos;			//!< Struct to interpolate within the table

	double fOutput;					//!< Map output

	double fPeriod;					//!< Time period to update the table

	nmTipoInterpolacion fTipo;		//!< Type of interpolation

  public:

	/*! One dimensional table constructor*/
	TTable1D(int i						//!< Index
			);

	/*! One dimensional table destructor*/
	~TTable1D();

	/*! Return the output of this controller*/
	double Output(double Time 				//!< Current time
				 );

	/*! Read data information of the controller*/
	void LeeController(const char *FileWAM,	//!< Filename of the input data file
					   fpos_t &filepos 	//!< Position within the input data file to read
					  );

	/*! Asign the controllers and the sensors*/
	void AsignaObjetos(TSensor **Sensor,			//!< Array with sensors
					   TController **Controller    //!< Array with controllers
					  );

	/*! Read the average results selected for the controller*/
	void LeeResultadosMedControlador(const char *FileWAM, //!< Filename of the input data file
									 fpos_t &filepos		//!< Position within the input data file to read
									);

	/*! Read the instantaneous results selected for the controller*/
	void LeeResultadosInsControlador(const char *FileWAM,		//!< Filename of the input data file
									 fpos_t &filepos		//!< Positon within the input data file to read
									);

	/*! Generate the header of the average results for the controller*/
	void CabeceraResultadosMedControlador(stringstream& medoutput		//!< StringStream where the average results are stored
										 );

	/*! Generate the header of the instantaneus results for the controller*/
	void CabeceraResultadosInsControlador(stringstream&
										  insoutput //!< StringStream where the instantaneous results are stored
										 );

	/*! Print the average results*/
	void ImprimeResultadosMedControlador(stringstream& medoutput //!< StringStream where the average results are stored
										);

	/*! Print the instantaneous results*/
	void ImprimeResultadosInsControlador(stringstream& insoutput //!< StringStream where the average results are stored
										);

	/*! Initialize average results*/
	void IniciaMedias();

	/*! Calculate average Results*/
	void ResultadosMediosController();

	/*! Acumulate the average results*/
	void AcumulaResultadosMediosController(double Actual	//!< Current time
										  );

	/*! Calculate the instantaneous results*/
	void ResultadosInstantController();

};

#endif
