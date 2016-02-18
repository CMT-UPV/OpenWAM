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
#ifndef TDecisorH
#define TDecisorH

#include "TController.h"
//---------------------------------------------------------------------------

/*! This object represents a decisor. The decirsor chooses the output of one controller (high)
 or other depending on the fact that the valued sensed is higher or lower than the target value */
class TDecisor: public TController {
  private:

	int fControllerLowID; //!< This parameter represents the controller ID of the lower controller
	int fControllerHighID; //!< This parameter represents the controller ID of the higher controller
	int fControllerTargetID;	//!< Controller ID of the target

	bool fTargedControlled;		//!< The target can be modified by a controller?

	TController *fControllerLow;		//!< This controller provides the lower signal
	TController *fControllerHigh;		//!< This controller provides the higher signal
	TController *fControllerTarget;	//!< This controller is used to decide the target value

	double fTarget;				//!< Target value

	double fOutput;				//!< Final Output of the target
	double fInputLow;			//!< Lower input. Output of the lower controller
	double fInputHigh;		//!< Higher input. Output of the higher controller

	double fInput;					//!< Value sensed used to take the decision

	int fID;					//!< Controller ID

  public:

	/*! Decisor contructor */
	TDecisor(int i							//!< Index
			);

	/*! Decisor destructor */
	~TDecisor();

	/*! Return the final output */
	double Output(double Time						//!< Current time
				 );

	/*! Read the data of the controller */
	void LeeController(const char *FileWAM,		//!< File name of the input data
					   fpos_t &filepos				//!< Position within the file to read
					  );

	/*! Asing the different controller and sensors */
	void AsignaObjetos(TSensor **Sensor,				//!< Array with sensors
					   TController **Controller		//!< Array with controllers
					  );

	/*! Read the average results selected */
	void LeeResultadosMedControlador(const char *FileWAM,		//!< File name of the input data
									 fpos_t &filepos				//!< Position within the file to read
									);

	/*! Read the instantenous results selected */
	void LeeResultadosInsControlador(const char *FileWAM,				//!< File name of the input data
									 fpos_t &filepos				//!< Position within the file to read
									);

	/*! Generate the average results header */
	void CabeceraResultadosMedControlador(stringstream& medoutput				//!< StringStream where the average results are stored
										 );

	/*! Generate the instantaneous results header */
	void CabeceraResultadosInsControlador(stringstream&
										  insoutput				//!< StringStream where the instantaneous results are stored
										 );

	/*! Print the average results */
	void ImprimeResultadosMedControlador(stringstream& medoutput				//!< StringStream where the averate results are stored
										);

	/*! Print the instantaneous values */
	void ImprimeResultadosInsControlador(stringstream&
										 insoutput				//!< StringStream where the instantaneous results are stored
										);

	/*! Initialize average results */
	void IniciaMedias();

	/*! Calculate average results */
	void ResultadosMediosController();

	/*! Acumulate average results */
	void AcumulaResultadosMediosController(double Actual	//!< Current time
										  );

	/*! Calculate instantaneous results */
	void ResultadosInstantController();

};

#endif
