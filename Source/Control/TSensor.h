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
#ifndef TSensorH
#define TSensorH
#include <iostream>
#ifdef __BORLANDC__
#include <vcl.h>
#endif
#include "Globales.h"
//---------------------------------------------------------------------------

//class TTubo;
//class TDeposito;

/*! This object is used to get information from some objects and provides it to controllers */
class TSensor {
  private:

	int FNumeroSensor;						//!< ID number of the sensor

	nmObjectSensed FObjectSensed;			//!< Type of the object sensed
	nmParameterSensed FParameterSensed; 	//!< Parameter sensed

	int FObjectID;							//!< ID number of the object sensed

	bool FInicia;							//!< Indentify the fist time step

	double FDistancia;	//!< If the object sensed is a pipe. Distance from the left end where the parameter is sensed
	int FNode0;	//!< If the object sensed is a pipe. Left cell from the place where the parameter is sensed
	int FNode1;	//!< If the object sensed is a pipe. Right cell from the place where the parameter is sensed
	double FDelta;							//!< Current time step

	double FDelay;							//!< Delay. Dynamics of the sensor
	double FGain;						//!< Gain. To reproduce real sensors.

	double FTime0;		//!< Previous time where the sensor value was updated

	double FRealValue;			//!< Current real value of the sensed parameter
	double FPrevRealValue;		//!< Previous real value of the sensed parameter
	double FPrevValue;					//!< Previous value of the sensor output
	double FCurrentValue;				//!< Current value of the sensor output

	TObject *FObjectPointer;				//!< Pointer to the object sensed

	stResInstantSensor FResInstantSensor;				//!< Struct with the instantaneus results
	stResMediosSensor FResMediosSensor;		//!< Struct with the average results

	double FPeriod;	//!< Controls the time period to update the sensor measurement.

  public:
	/*! Contructor of the class TSensor*/
	TSensor(int i							//!< Index of the sensor
		   );

	/*! Destructor of the class TSensor*/
	~TSensor();

	/*! Read from the input data the sensor information*/
	void ReadSensor(const char *FileWAM,		//!< Filename of the input data
					fpos_t &filepos		//!< Specify a position within the input file.
				   );

	/*! Asign the pointer of the object sensed*/
	void AsignaObjeto(TObject *Object		//!< Pointer to the object sensed
					 );

	/*! Return the current value of the sensor output*/
	double Output();

	/*! Update the sensor output value*/
	void ActualizaMedida(double Time						//!< Current tiem
						);

	/*! Return the type of the object sensed*/
	nmObjectSensed ObjectSensed() {
		return FObjectSensed;
	}
	;

	/*! Return the ID numbero of the sensor*/
	int ObjectID() /*{return FObjectID;}*/;

	/*! Read the average results selected for the sensor*/
	void LeeResultadosMedSensor(const char *FileWAM,						//!< Filename of the input data
								fpos_t &filepos		//!< Specify a position within the input file.
							   );

	/*! Read de instantaneous results selected for the sensor*/
	void LeeResultadosInsSensor(const char *FileWAM,		//!< Filename of the input data
								fpos_t &filepos		//!< Specify a position within the input file.
							   );

	/*! Generate the header of the average results*/
	void CabeceraResultadosMedSensor(std::stringstream& medoutput		//!< StringStream where the average results are stored
									);

	/*! Generate the header of the instantaneous results*/
	void CabeceraResultadosInsSensor(std::stringstream&
									 insoutput		//!< StringStream where the instantaneous results are stored
									);

	/*! Print the average results of the sensor*/
	void ImprimeResultadosMedSensor(std::stringstream& medoutput		//!< StringStream where the average results are stored
								   );

	/*! Print the instantaneous results of the sensor*/
	void ImprimeResultadosInsSensor(std::stringstream&
									insoutput		//!< StringStream where the instantaneous results are stored
								   );

	/*! Initailize the average results*/
	void IniciaMedias();

	/*! Calculate the average results*/
	void ResultadosMediosSensor();

	/*! Acumulate the average results*/
	void AcumulaResultadosMediosSensor(double Actual		//!< Current time
									  );

	/*! Calculate the instantaneous results*/
	void ResultadosInstantSensor();

};

#endif
