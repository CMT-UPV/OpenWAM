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
#ifndef TPIDControllerH
#define TPIDControllerH

#include "TController.h"

/*! This object represents a PID controller*/
class TPIDController: public TController {
  private:
	int fID;					//!< ID controller number

	double fKP_pos;				//!< Proportional gain for positive errors.
	double fKI_pos;				//!< Integral gain for positive errors.
	double fKD_pos;				//!< Derivative gain for positive errors.

	double fKP_neg;				//!< Proportional gain for negative errors.
	double fKI_neg;				//!< Integral gain for negative errors.
	double fKD_neg;				//!< Derivative gain for negative errors.

	double fpact;				//!< Proportional term of output
	double fdact;				//!< Derivative term of output
	double fiact;				//!< Integral term of output

	double fMax_out;			//!< Maximum output
	double fMin_out;			//!< Minimum output

	double fError;				//!< Current error
	double fError_ant;			//!< Previous error

	double fTime_ant;			//!< Previous time

	double fSetPoint;			//!< Set point value

	//double fVar_ant;
	double fI_ant;				//!< Previous integral term of output

	double fOutput;				//!< Current raw PID output
	double fOutput0;			//!< Constant output

	double fOutput_filt;		//!< Current filtered PID output
	double fOutput_filt_ant;	//!< Previous filtered PID output
	double fOutput_ant;			//!< Previous raw PID output
	double fTime_ant_filt;		//!< Preivous time

	double fPeriod;				//!< Time period to update the raw PID output
	double fGain;				//!< Output gain
	double fDelay;				//!< Output time delay constant

	double fDwell;				//!< Dwell time

	bool fInicio;

	bool fSetPointControlled;				//!< Is the set point controlled by another controller?
	int fSetPointControllerID;	//!< Setpoint controller ID number

	TController *fSetPointController;	//!< Pointer to the setpoint controller.

  public:

	/*! PID controller contructor*/
	TPIDController(int i					//!< Index
				  );

	/*! PID controller destructor*/
	~TPIDController();

	/*! Return the PID controller output*/
	double Output(double Time				//!< Current PID output
				 );

	/*! Read the PID data */
	void LeeController(const char *FileWAM,			//!< Input data filename
					   fpos_t &filepos			//!< Position within the input file
					  );

	/*! Asign the sensor input and the setpoint controller*/
	void AsignaObjetos(TSensor **Sensor,		//!< Array with sensor objects
					   TController **Controller	//!< Array with controller objects
					  );

	/*! Read the average results selected*/
	void LeeResultadosMedControlador(const char *FileWAM,	//!< Input data filename
									 fpos_t &filepos				//!< Position within the input file
									);

	/*! Read the instantaneous results selected*/
	void LeeResultadosInsControlador(const char *FileWAM,				//!< Input data filename
									 fpos_t &filepos				//!< Position within the input file
									);

	/*! Generate average results header */
	void CabeceraResultadosMedControlador(stringstream& medoutput				//!< StringStrems where the average results are stored
										 );

	/*! Generate instantaneous results header */
	void CabeceraResultadosInsControlador(stringstream&
										  insoutput				//!< StringStream where the instantaneous results are stored
										 );

	/*! Print average results */
	void ImprimeResultadosMedControlador(stringstream& medoutput				//!< StringStream where the average results are stored
										);

	/*! Print instantaneous results */
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

	/*! Calculate instanteneous results */
	void ResultadosInstantController();

};

//---------------------------------------------------------------------------
#endif
