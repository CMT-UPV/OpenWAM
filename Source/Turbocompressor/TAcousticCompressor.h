/*--------------------------------------------------------------------------------*\
|==========================|
 |\\   /\ /\   // O pen     | OpenWAM: The Open Source 1D Gas-Dynamic Code
 | \\ |  X  | //  W ave     |
 |  \\ \/_\/ //   A ction   | CMT-Motores Termicos / Universidad Politecnica Valencia
 |   \\/   \//    M odel    |
 ----------------------------------------------------------------------------------
 | License
 |
 |	This file is part of OpenWAM.
 |
 |	OpenWAM is free software: you can redistribute it and/or modify
 |	it under the terms of the GNU General Public License as published by
 |	the Free Software Foundation, either version 3 of the License, or
 |	(at your option) any later version.
 |
 |	OpenWAM is distributed in the hope that it will be useful,
 |	but WITHOUT ANY WARRANTY; without even the implied warranty of
 |	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 |	GNU General Public License for more details.
 |
 |	You should have received a copy of the GNU General Public License
 |	along with OpenWAM.  If not, see <http://www.gnu.org/licenses/>.
 |
 \*--------------------------------------------------------------------------------*/

// ---------------------------------------------------------------------------
#ifndef TAcousticCompressorH
#define TAcousticCompressorH
// ---------------------------------------------------------------------------

#include "TTubo.h"
#include "TDeposito.h"
#include "Globales.h"

/**
 * @file TAcousticCompressor.h
 * @author Francisco Jose Arnau Martinez <farnau@mot.upv.es>
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @section DESCRIPTION
 * The TurboBearings class represent the bearing system in a turbocharger.
 *
 * It has methods to compute the power losses in the bearing system. This file
 * has the interface of TurboBearings.
 *
 */

class TAcousticCompressor {
  private:

	TTubo *FInletPipe;
	TTubo *FVolute;
	TTubo *FOutletPipe;

	TDeposito *FRotorVolume;
	TDeposito *FStatorVolume;

	int FInletPipeID;
	int FVoluteID;
	int FOutletPipeID;

	int FRotorVolumeID;
	int FStatorVolumeID;

  public:

	/**
	 * Default constructor.
	 */
	TAcousticCompressor();

	/**
	 * Default destructor.
	 */
	~TAcousticCompressor();

	TAcousticCompressor(int InletPipeID, int VoluteID, int OutletPipeID, int RotorVolumeID, int StatorVolumeID);

	/**
	 * \brief Compute the total pressure at compressor inlet.
	 *
	 * Compute the total pressure at compressor inlet.
	 *
	 * @return Total pressure at compressor inlet, in bar.
	 */
	double P10();

	double P1();

	/**
	 * \brief Compute the total pressure at compressor outlet.
	 *
	 * Compute the total pressure at compressor outlet.
	 *
	 * @return Total pressure at compressor outlet, in bar.
	 */
	double P20();

	/**
	 * \brief Compute the pressure at compressor outlet.
	 *
	 * Compute the pressure at compressor outlet.
	 *
	 * @return Pressure at compressor outlet, in bar.
	 */
	double P2();

	/**
	 * \brief Compute the total temperature at compressor inlet.
	 *
	 * Compute the total pressure at compressor outlet.
	 *
	 * @return Total temperature at compressor outlet, in K.
	 */
	double T10();

	/**
	 * \brief Compute the temperature at compressor outlet.
	 *
	 * Compute the pressure at compressor outlet.
	 *
	 * @return Temperature at compressor outlet, in K.
	 */

	double T2();

	double T20();

	double T1();

	double Din();

	double CompRatio();

	void PutHeatPower(double Power);

	void PutHeatPowerIn(double Power);

	void AsignInPipe(TTubo **Pipe) {
		FInletPipe = Pipe[FInletPipeID - 1];
	}
	;

	void AsignOutPipe(TTubo **Pipe) {
		FOutletPipe = Pipe[FOutletPipeID - 1];
	}
	;

	void AsignVolute(TTubo **Pipe) {
		FVolute = Pipe[FVoluteID - 1];
	}
	;

	void AsignRotorVol(TDeposito **Volume) {
		FRotorVolume = Volume[FRotorVolumeID - 1];
	}
	;

	void AsignStatorVol(TDeposito **Volume) {
		FStatorVolume = Volume[FStatorVolumeID - 1];
	}
	;

	void AsignElementsID(int InletPipeID, int VoluteID, int OutletPipeID, int RotorVolumeID, int StatorVolumeID);

	double CRCorrector();

	double EFCorrector(double rnew, double rorig);

	double MassFlow();

	double Efficiency();

};

#endif
