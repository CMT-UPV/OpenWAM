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
#ifndef TControllerH
#define TControllerH

#include "Globales.h"
#include "TSensor.h"
//---------------------------------------------------------------------------

/*! This oject is used to control parameter in the engine (rack positions, fuel injected ...) */
class TController {
  protected:

	std::vector<TSensor *> FSensor;	//!< Array with the pointers of the sensor inputs

	int FNumSensors;						//!< Number of sensor input
	iVector FSensorID;				//!< Array with the ID of the sensor inputs

	int FControllerID;						//!< ID of this controller

	nmControlMethod FControl;				//!< Type of controller

	stResMediosCtrl FResMediosCtrl;	//!< Struct with the average results of the controllers
	stResInstantCtrl FResInstantCtrl;	//!< Struct with the average results of the controllers

  public:
	/*! Constructor of the controller class*/
	TController(nmControlMethod meth,			//!< Type of controller
				int i							//!< Index
			   );

	/*! Destructor of the controller class*/
	~TController();

	/*! Read the information of the controller*/
	virtual void LeeController(const char *FileWAM,		//!< Input data filename
							   fpos_t &filepos			//!< Position to read within the input file
							  ) = 0;

	/*! Return the output value of the controller*/
	virtual double Output(double Time						//!< Current time
						 ) = 0;

	/*! Asign the input sensor and controllers*/
	virtual void AsignaObjetos(TSensor **Sensor, //!< Array with the sensor objects
							   TController **Controller	//!< Array with the controller objects
							  ) = 0;

	/*! Read the average results selected*/
	virtual void LeeResultadosMedControlador(const char *FileWAM,	//!< Input data filename
			fpos_t &filepos			//!< Position to read within the input file
											) = 0;

	/*! Read the instantanous results selected*/
	virtual void LeeResultadosInsControlador(const char *FileWAM,			//!< Input data filename
			fpos_t &filepos			//!< Position to read within the input file
											) = 0;

	/*! Generate the header of the average results*/
	virtual void CabeceraResultadosMedControlador(std::stringstream&
			medoutput //!< StringStream where the average results are stored
												 ) = 0;

	/*! Generate the header of the instantaneous results*/
	virtual void CabeceraResultadosInsControlador(std::stringstream&
			insoutput //!< StringStream where the instantaneous results are stored
												 ) = 0;

	/*! Print the average results of the controller*/
	virtual void ImprimeResultadosMedControlador(std::stringstream&
			medoutput //!< StringStream where the average results are stored
												) = 0;

	/*! Print the instantaneous results of the controller*/
	virtual void ImprimeResultadosInsControlador(std::stringstream& insoutput) = 0;

	/*! Initialize average results*/
	virtual void IniciaMedias() = 0;

	/*! Calculate average results*/
	virtual void ResultadosMediosController() = 0;

	/*! Acumulate average results*/
	virtual void AcumulaResultadosMediosController(double Actual //!< Current tiem
												  ) = 0;

	/*! Calculate instantaneus results*/
	virtual void ResultadosInstantController() = 0;
};

struct stGainInput {

	TSensor *InSensor;
	TController *InController;

	stGainInput() {
	}
	;

	virtual double Output(double t) = 0;
};

struct stGainInputSensor: stGainInput {

	stGainInputSensor(TSensor *insens) :
		stGainInput() {
		InSensor = insens;
	}
	;

	double Output(double t) {
		return InSensor->Output();
	}
	;

};

struct stGainInputController: stGainInput {

	stGainInputController(TController *inctrl) :
		stGainInput() {
		InController = inctrl;
	}
	;

	double Output(double t) {
		return InController->Output(t);
	}
	;

};
#endif
