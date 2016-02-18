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
#pragma hdrstop
#include "TCalculoExtern.h"

#ifdef __BORLANDC__
#include <vcl.h>
#endif

//#include "TMatlab.h"
#include "TTubo.h"
#include "TDeposito.h"
#include "TCCDeposito.h"
#include "TTGV.h"
#include "TControlFuel.h"
#include "TEGRV.h"
#include "TRegimenMotor.h"
#include "Tfql.h"
#include "TEjeTurbogrupo.h"
#include "TTurbina.h"
#include "TVenturi.h"
#include "TTipoValvula.h"
#include "TBloqueMotor.h"
#include "TRemansoMatlab.h"
#include "TCoefDescarga.h"
#include "TAjusteTransCalorCil.h"
#include "TCCUnionEntreDepositos.h"
#include "TControlInyeccion.h"
#include "TFraccionMasicaCilindro.h"
#include "TControlK.h"

//---------------------------------------------------------------------------

TCalculoExtern::TCalculoExtern() {
	FInputs = NULL;
	FOutputs = NULL;
	FEntrada = NULL;
	FSalida = NULL;

// ATENCION: Los siguiente valores determinan el numero de entradas y salidas a Matlab
// Debes modificar estos numeros segun tus requerimientos.

	FNumeroInMat = 0;
	FNumeroOutMat = 0;

	if(FNumeroInMat > 0)
		FEntrada = new double[FNumeroInMat];

	if(FNumeroOutMat > 0)
		FSalida = new double[FNumeroOutMat];

//ECU=NULL;
	FTGV = NULL;
	FFuel = NULL;
	FEGRV = NULL;
	FRegimen = NULL;
	Ffql = NULL;
	FRemansoMatlab = NULL;
	FAjusteTransCalorCil = NULL;
	FFraccionMasicaCilindro = NULL;
	FControlInyeccion = NULL;

	FSensorTubo = NULL;
	FSensorDep = NULL;
	FSensorTG = NULL;
	FSensorTurbina = NULL;
	FSensorCil = NULL;
	FSensorVent = NULL;
	FSensorMotor = NULL;

	FTiempo = 0;

	FNSensTubos = 0;
	FNSensDepositos = 0;
	FNSensTurbina = 0;
	FNSensTurbogrupo = 0;
	FNSensMotor = 0;
	FNSensVenturis = 0;
	FNSensCilindros = 0;

	FRutaTrabajo = NULL;

	FConvergencia = false;
	FCalculoGamma = nmComposicionTemperatura;
	FParametros = NULL;

	FPrimeraVez = true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

TCalculoExtern::~TCalculoExtern() {
//  if(ECU!=NULL) delete ECU;

	if(FTGV != NULL) {
		for(int i = 0; i < Fcontadortgv; i++) {
			delete FTGV[i];
		}
		delete[] FTGV;
	}

	if(FFuel != NULL)
		delete FFuel;

	if(FEGRV != NULL)
		delete FEGRV;

	if(FRegimen != NULL)
		delete FRegimen;

	if(Ffql != NULL)
		delete Ffql;

	if(FAjusteTransCalorCil != NULL)
		delete FAjusteTransCalorCil;

	if(FFraccionMasicaCilindro != NULL)
		delete FFraccionMasicaCilindro;

	if(FControlInyeccion != NULL)
		delete FControlInyeccion;

	if(FInputs != NULL)
		delete[] FInputs;

	if(FOutputs != NULL)
		delete[] FOutputs;

	if(FEntrada != NULL)
		delete[] FEntrada;

	if(FSalida != NULL)
		delete[] FSalida;

	if(FSensorTubo != NULL)
		delete[] FSensorTubo;

	if(FSensorDep != NULL)
		delete[] FSensorDep;

	if(FSensorTG != NULL)
		delete[] FSensorTG;

	if(FSensorTurbina != NULL)
		delete[] FSensorTurbina;

	if(FSensorCil != NULL)
		delete[] FSensorCil;

	if(FSensorVent != NULL)
		delete[] FSensorVent;

	if(FSensorMotor != NULL)
		delete[] FSensorMotor;

	if(FRemansoMatlab != NULL) {
		for(int i = 0; i < Fnematlab; i++) {
			delete FRemansoMatlab[i];
		}
		delete[] FRemansoMatlab;
	}

	if(FCoefDescarga != NULL) {
		for(int i = 0; i < Fnumunion; i++) {
			delete FCoefDescarga[i];
		}
		delete[] FCoefDescarga;
	}

	if(FRutaTrabajo != NULL)
		delete[] FRutaTrabajo;

	if(FParametros != NULL)
		delete[] FParametros;
}

//-------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCalculoExtern::LlamadaECU(double DeltaT, TBloqueMotor **Engine) {
	try {

		FTiempo += DeltaT;

		//ATENCION: Asignar los valores que provienen de los sensores. El orden es muy importante.

		/* FInputs[0]=FSensorTubo[0].valact;   // "Presion a la entrada de la turbina"
		 FInputs[1]=FSensorTubo[1].valact;   // "Temperature a la entrada de la turbina"
		 FInputs[2]=FSensorTubo[2].valact;   // "Massflow masico de EGR"
		 FInputs[3]=FSensorTubo[3].valact;   // "Temperature a la salida del intercooler"
		 FInputs[4]=FSensorTubo[4].valact;   // "Massflow masico antes del intercooler"
		 FInputs[5]=FSensorTubo[5].valact;   // "Presion a la salida del intercooler (antes de la ramificacion en el colector"
		 FInputs[6]=FSensorMotor[0].valact;  // "Regimen de giro del motor"
		 FInputs[7]=FSensorMotor[1].valact;  // "Instante de tiempo del motor"    */

//     if(Fhayecu){
//
//		  //ATENCION: A continuacion se deben asignar los valores de entrada de MATLAB
//		  //-----------------------------------------------------------------------------
//		  //
//		  //-----------------------------------------------------------------------------
//
//		  if(FTiempo>=Ftmuestreoecu){
//			   FTiempo=0.;
//			   for(int i=0;i<FNumeroInMat;i++)
//			   {
//					ECU->Input[i]=FEntrada[i];  /*Al igual que el numero de entradas a Matlab (FNumeroInMat), los valores de FEntrada han de ser dados por cada usuario)*/
//			   }
//			   ECU->CalculaECU();
//
//			   //ATENCION: A continuacion se deben asignar los valores de salida de MATLAB
//			   //-----------------------------------------------------------------------------
//			   for(int i=0;i<FNumeroOutMat;i++)
//			   {/* estas son las variable que vienen de Matlab*/
//					FSalida[i]=ECU->GetOutput(i);
//			   }
//			   //-----------------------------------------------------------------------------
//		  }
//	 }
		//=========================================================================
		//AQUI SE DEBEN REALIZAR LOS CALCULOS EXTERNOS
		//*****************************************
		//**** CALCULO DE LA DESCARGA A MATLAB ****//
		//FRemansoMatlab[0]->CalculaRemanso(FInputs[1]);
		/*if(FTiempo>=Ftmuestreoecu){
		 FTiempo=0.;
		 FEGRV->CalculaEGRV(FInputs[7],FInputs[2],FInputs[4],FConvergencia);
		 FTGV[0]->CalculaTurbina(FInputs[7],FInputs[5],FInputs[0],FConvergencia);
		 FAjusteTransCalorCil->CalculaTCC(FInputs[7],FInputs[4],FInputs[1],FConvergencia);
		 */
		/* CONVERGENCIA DE LA EJECUCION */
		/*  if(FInputs[7]>0.6){
		 if(fabs(FTGV[0]->ErrorP2)<0.03 & fabs(FTGV[0]->ErrorP3)<0.03 & fabs(FEGRV->ErrorEGR)<0.01 & fabs(FAjusteTransCalorCil->getErrorGastoAire())<0.03){
		 FConvergencia=true;
		 }
		 }
		 }    */

		//**** CALCULO COEFICIENTE DE DESCARGA EGR ****//
//FOutputs[0]=FEGRV->PutCDEntrante(FEGRV->CDEntrante);
		//FOutputs[1]=FEGRV->getCDSaliente();
		//FOutputs[2]=FEGRV->getCTorbellino();
		//**** CALCULO DEL COMBUSTIBLE ACTUAL ****//
		/* for(int i=0;i<Engine[0]->getGeometria().NCilin;i++){
		 if(Engine[0]->GetCilindro(i)->getAnguloActual()>Engine[0]->GetCilindro(i)->getDistribucion().CA && Engine[0]->GetCilindro(i)->getAnguloAnterior()<=Engine[0]->GetCilindro(i)->getDistribucion().CA){
		 double Fuel=FFuel->CalculaFuel();
		 FOutputs[i+3]=Fuel;
		 }
		 }   */

		//**** CALCULO DE LOS Actuators EN TGV ****//
		//FOutputs[0]=FTGV[0]->getCDStator();
		//FOutputs[1]=FTGV[0]->getCDRotor();
		//FOutputs[2]=FTGV[0]->getRendimiento();
		// Hay 3 salidas mas, por si fuese una turbina twin. Indices 10,11,12
		//**** CALCULO DE LOS COEFICIENTES DE TRANSMISION DE CALOR EN ADMISION Y ESCAPE****//
		/*FOutputs[13]=FAjusteTransCalorCil->CAdmision;
		 FOutputs[14]=FAjusteTransCalorCil->CEscape;*/

		/* Variables que vienen de MATLAB */
		//for(int i=0;i<FNumeroOutMat;i++){
		//   FOutputs[i]=FSalida[i];
		//}
		FOutputs[0] = FTiempo * 0.02;
		FOutputs[1] = FTiempo * 0.02;
		FOutputs[2] = 0;
		if(FOutputs[0] > 1)
			FOutputs[0] = 1;
		if(FOutputs[1] > 1)
			FOutputs[1] = 1;
	} catch(exception &N) {
		std::cout << "ERROR: LlamadaECU" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void TCalculoExtern::FinECU() {
	try {
//if(Fhayecu){
//	 ECU->FinalizaECU();
//}

	} catch(exception &N) {
		std::cout << "ERROR: PutInputdll" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//------------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TCalculoExtern::GetOutput_dll(int i) {
	try {
		if(i < FNOutputsdll) {
			return FOutputs[i];
		} else {
			std::cout << "WARNING: El valor de la salida de la dll se sale de rango" << std::endl;
			std::cout << "         Revisa el acceso a la dll" << std::endl;
			return 0.;
		}
	} catch(exception &N) {
		std::cout << "ERROR: GetOutputdll" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//------------------------------------------------------------------------------
//---------------------------------------------------------------------------

TRemansoMatlab* TCalculoExtern::GetTRemansoMatlab(int i) {
	try {
//   if(i<FNOutputsdll){
		return FRemansoMatlab[i];
//   }else{
//       std::cout << "WARNING: El valor de la salida de la dll se sale de rango" << std::endl;
//       std::cout << "         Revisa el acceso a la dll" << std::endl;
//       return 0.;
//   }
	} catch(exception &N) {
		std::cout << "ERROR: GetTRemansoMatlab" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//------------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCoefDescarga* TCalculoExtern::GetTCoefDescarga(int i) {
	try {
//   if(i<FNOutputsdll){
		return FCoefDescarga[i];
//   }else{
//       std::cout << "WARNING: El valor de la salida de la dll se sale de rango" << std::endl;
//       std::cout << "         Revisa el acceso a la dll" << std::endl;
//       return 0.;
//   }
	} catch(exception &N) {
		std::cout << "ERROR: GetTCoefDescarga" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//------------------------------------------------------------------------------
//---------------------------------------------------------------------------

TControlFuel* TCalculoExtern::GetFuel() {
	try {
//   if(i<FNOutputsdll){
		return FFuel;
//   }else{
//       std::cout << "WARNING: El valor de la salida de la dll se sale de rango" << std::endl;
//       std::cout << "         Revisa el acceso a la dll" << std::endl;
//       return 0.;
//   }
	} catch(exception &N) {
		std::cout << "ERROR: GetFuel" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//------------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCalculoExtern::LeeFicherosDLL(const char *FileWAM, fpos_t &filepos, int controlvalv, int nematlab, int ncilin,
									int nunmat, int CountVGT, int numespecies, int NumeroPerdidasPresion) {
	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		ObtenerRutaTrabajo(FileWAM);

		int ecu = 0, iny = 0, mfcomb = 0, comb = 0, baraba = 0, Yespecies = 0, CalculoK = 0;
		Fhayecu = false;
		Fcontroliny = false;
		Fcontrolmfcomb = false;
		Fmodcomb = false;
		Fajustbaraba = false;
		FFraccionMasicaEspeciesCil = false;
		FCalculoK = false;
		Fngrafmat = 0;
		Ftmuestreoecu = 0.02;
		fscanf(fich, " %d ", &ecu);
		ecu == 0 ? Fhayecu = false : Fhayecu = true;
		fscanf(fich, "%d %d %d %d %d %d ", &iny, &mfcomb, &comb, &baraba, &Yespecies, &CalculoK);
		iny == 0 ? Fcontroliny = false : Fcontroliny = true;
		mfcomb == 0 ? Fcontrolmfcomb = false : Fcontrolmfcomb = true;
		comb == 0 ? Fmodcomb = false : Fmodcomb = true;
		baraba == 0 ? Fajustbaraba = false : Fajustbaraba = true;
		Yespecies == 0 ? FFraccionMasicaEspeciesCil = false : FFraccionMasicaEspeciesCil = true;
		fscanf(fich, "%d ", &Fngrafmat); /* numero de magnitudes de la ECU que queremos graficar */
		if(Fhayecu) {
			//ECU=new TMatlab(FNumeroInMat,FNumeroOutMat);
			//fscanf(fich,"%lf ",&Ftmuestreoecu);
			//ECU->LeeFicherosECU(fich);
			//ECU->IniciaECU();
			std::cout << "WARNING: Matlab link must be adapted with your Matlab version" << std::endl;
		}
		if(controlvalv == 1) {
			Fntact = nunmat * 3;
			Fnumunion = nunmat;
			/*FCoefDescarga=new TCoefDescarga*[nematlab];
			 for (int i=0;i<Fnumunion;i++){
			 FCoefDescarga[i]=new TCoefDescarga();

			 } */
		}
		if(Fcontrolmfcomb) {
			Fntact += ncilin;
		}
		if(Fcontroliny) {
			Fntact += 18 * ncilin;
			FControlInyeccion = new TControlInyeccion();
		}
		if(Fajustbaraba) {
			Fntact += 2;
			FAjusteTransCalorCil = new TAjusteTransCalorCil();
		}
		if(Fmodcomb) {
			Fntact += 3 * ncilin;
			Ffql = new Tfql(ncilin);
		}
		if(FFraccionMasicaEspeciesCil) {
			Fntact += numespecies * ncilin;
			FFraccionMasicaCilindro = new TFraccionMasicaCilindro();
		}
		Fntact += nematlab * 2;
		Fntact += CountVGT * 6;
		Fcontadortgv = CountVGT;
		if(CountVGT != 0) {
			FTGV = new TTGV*[CountVGT];
			for(int i = 0; i < CountVGT; i++) {
				FTGV[i] = new TTGV();
			}
		}
		if(Fcontrolmfcomb) {
			FFuel = new TControlFuel();
		}

		FNumPerdidasPresion = 0;
		if(FCalculoK) {
			FNumPerdidasPresion = NumeroPerdidasPresion;
			FControlK = new TControlK*[FNumPerdidasPresion];
			for(int i = 0; i < FNumPerdidasPresion; i++) {
				FControlK[i] = new TControlK();
			}
			Fntact += FNumPerdidasPresion;
		}

		Fnematlab = nematlab;
		if(Fnematlab != 0) {
			FRemansoMatlab = new TRemansoMatlab*[nematlab];
			for(int i = 0; i < Fnematlab; i++) {
				FRemansoMatlab[i] = new TRemansoMatlab();
			}
		}
		/*Si no hay valvula de EGR la siguiente linea se debe comentar*/
		FEGRV = new TEGRV();
		//FRegimen=new TRegimenMotor();
		FNOutputsdll = Fntact;
		FOutputs = new double[FNOutputsdll];
		for(int i = 0; i < FNOutputsdll; i++) {
			FOutputs[i] = 0.;
		}

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: LeeFicherosDLL" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void TCalculoExtern::Lee_Sens_Tubos(const char *FileWAM, fpos_t &filepos, TTubo **Pipe,
									nmTipoCalculoEspecies SpeciesModel, bool ThereIsEGR, bool HayCombustible) {
	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		double T = 0., temp = 0., v1 = 0., den = 0.;
		int n1 = 0, t = 0;
		int tipo = 0, tubo = 0, nsensortubo = 0, ndistancias = 0;
		double dist = 0.;
		ndistancias = 0;
		fscanf(fich, "%d ", &FNSensTubos); /*numero de sensores en tubos*/
		fscanf(fich, "%d ", &ndistancias); /*numero total de distancias*/
		int cont = 0;
		FSensorTubo = new stSensorTubos[FNSensTubos];
		if(FNSensTubos != 0) {
			for(int i = 0; i <= ndistancias - 1; ++i) {
				fscanf(fich, "%d %lf %d ", &tubo, &dist, &nsensortubo);
				for(int j = 0; j <= nsensortubo - 1; ++j) {
					FSensorTubo[cont].tubo = tubo;
					FSensorTubo[cont].distancia = dist;
					fscanf(fich, "%d ", &tipo);
					switch(tipo) {
					case 0:
						FSensorTubo[cont].tipo = nmPrTubo;
						break;
					case 1:
						FSensorTubo[cont].tipo = nmVelTubo;
						break;
					case 2:
						FSensorTubo[cont].tipo = nmTempTubo;
						break;
					case 3:
						FSensorTubo[cont].tipo = nmGastoTubo;
						break;
					case 4:
						FSensorTubo[cont].tipo = nmN2Tubo;
						break;
					case 5:
						FSensorTubo[cont].tipo = nmO2Tubo;
						break;
					case 6:
						FSensorTubo[cont].tipo = nmCO2Tubo;
						break;
					case 7:
						FSensorTubo[cont].tipo = nmH2OTubo;
						break;
					case 8:
						FSensorTubo[cont].tipo = nmHCTubo;
						break;
					case 9:
						FSensorTubo[cont].tipo = nmSootTubo;
						break;
					case 10:
						FSensorTubo[cont].tipo = nmNOxTubo;
						break;
					case 11:
						FSensorTubo[cont].tipo = nmCOTubo;
						break;
					case 12:
						FSensorTubo[cont].tipo = nmAireFrTubo;
						break;
					case 13:
						FSensorTubo[cont].tipo = nmGasQuemadoTubo;
						break;
					case 14:
						FSensorTubo[cont].tipo = nmEGRTubo;
						break;
					case 15:
						FSensorTubo[cont].tipo = nmCombustibleTubo;
						break;
					default:
						printf("Error en el tipo de sensor\n");
						throw Exception("");
					} /*fin del switch*/
					fscanf(fich, "%lf ", &FSensorTubo[cont].ganancia);
					fscanf(fich, "%lf ", &FSensorTubo[cont].ctetiempo);

					FSensorTubo[cont].nodos = Pipe[FSensorTubo[cont].tubo - 1]->getNin();
					FSensorTubo[cont].deltax = Pipe[FSensorTubo[cont].tubo - 1]->getXRef();

					t = FSensorTubo[cont].tubo - 1;

					switch(FSensorTubo[cont].tipo) {
					case nmTempTubo:
						temp = Pipe[t]->GetAsonido(0) * __cons::ARef;
						FSensorTubo[cont].valreal = __units::KTodegC(temp * temp / Pipe[t]->GetGamma(0) / Pipe[t]->GetRMezcla(
														0)); // Como es el instante inicial da igual el nodo a que se hace referencia
						FSensorTubo[cont].valact = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valant = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valrealant = FSensorTubo[cont].valreal;
						break;
					case nmPrTubo:
						FSensorTubo[cont].valreal = Pipe[t]->getPresionInicial();
						FSensorTubo[cont].valact = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valant = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valrealant = FSensorTubo[cont].valreal;
						break;
					case nmVelTubo:
						FSensorTubo[cont].valreal = Pipe[t]->getVelocidadMedia();
						FSensorTubo[cont].valact = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valant = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valrealant = FSensorTubo[cont].valreal;
						break;
					case nmGastoTubo:
						temp = Pipe[t]->GetAsonido(0) * __cons::ARef; // Como es el instante inicial el nodo es arbitrario
						T = __units::KTodegC(temp * temp / Pipe[t]->GetGamma(0) / Pipe[t]->GetRMezcla(0));
						den = __units::BarToPa(Pipe[t]->getPresionInicial()) / Pipe[t]->GetRMezcla(0) / __units::degCToK(T);
						v1 = Pipe[t]->getVelocidadMedia();
						FSensorTubo[cont].valreal = __geom::Circle_area(Pipe[t]->GetDiametro(n1)) * v1 * den;
						FSensorTubo[cont].valact = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valant = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valrealant = FSensorTubo[cont].valreal;
						break;
					case nmN2Tubo:
						if(SpeciesModel == nmCalculoCompleto) {
							if(HayCombustible) {
								FIndiceTuboN2 = 8;
							} else
								FIndiceTuboN2 = 7;
						} else {
							printf("No puede haber sensor de N2 si el calculo de especies no es completo. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorTubo[cont].valreal = Pipe[t]->GetFraccionMasica(0, FIndiceTuboN2);
						FSensorTubo[cont].valact = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valant = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valrealant = FSensorTubo[cont].valreal;
						break;
					case nmO2Tubo:
						if(SpeciesModel == nmCalculoCompleto) {
							FIndiceTuboO2 = 0;
						} else {
							printf("No puede haber sensor de O2 si el calculo de especies no es completo. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorTubo[cont].valreal = Pipe[t]->GetFraccionMasica(0, FIndiceTuboO2);
						FSensorTubo[cont].valact = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valant = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valrealant = FSensorTubo[cont].valreal;
						break;
					case nmCO2Tubo:
						if(SpeciesModel == nmCalculoCompleto) {
							FIndiceTuboCO2 = 1;
						} else {
							printf("No puede haber sensor de CO2 si el calculo de especies no es completo. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorTubo[cont].valreal = Pipe[t]->GetFraccionMasica(0, FIndiceTuboCO2);
						FSensorTubo[cont].valact = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valant = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valrealant = FSensorTubo[cont].valreal;
						break;
					case nmH2OTubo:
						if(SpeciesModel == nmCalculoCompleto) {
							FIndiceTuboH2O = 2;
						} else {
							printf("No puede haber sensor de H2O si el calculo de especies no es completo. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorTubo[cont].valreal = Pipe[t]->GetFraccionMasica(0, FIndiceTuboH2O);
						FSensorTubo[cont].valact = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valant = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valrealant = FSensorTubo[cont].valreal;
						break;
					case nmHCTubo:
						if(SpeciesModel == nmCalculoCompleto) {
							FIndiceTuboHC = 3;
						} else {
							printf("No puede haber sensor de HC si el calculo de especies no es completo. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorTubo[cont].valreal = Pipe[t]->GetFraccionMasica(0, FIndiceTuboHC);
						FSensorTubo[cont].valact = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valant = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valrealant = FSensorTubo[cont].valreal;
						break;
					case nmSootTubo:
						if(SpeciesModel == nmCalculoCompleto) {
							FIndiceTuboSoot = 4;
						} else {
							printf("No puede haber sensor de Soot si el calculo de especies no es completo. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorTubo[cont].valreal = Pipe[t]->GetFraccionMasica(0, FIndiceTuboSoot);
						FSensorTubo[cont].valact = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valant = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valrealant = FSensorTubo[cont].valreal;
						break;
					case nmNOxTubo:
						if(SpeciesModel == nmCalculoCompleto) {
							FIndiceTuboNOx = 5;
						} else {
							printf("No puede haber sensor de NOx si el calculo de especies no es completo. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorTubo[cont].valreal = Pipe[t]->GetFraccionMasica(0, FIndiceTuboNOx);
						FSensorTubo[cont].valact = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valant = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valrealant = FSensorTubo[cont].valreal;
						break;
					case nmCOTubo:
						if(SpeciesModel == nmCalculoCompleto) {
							FIndiceTuboCO = 6;
						} else {
							printf("No puede haber sensor de CO si el calculo de especies no es completo. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorTubo[cont].valreal = Pipe[t]->GetFraccionMasica(0, FIndiceTuboCO);
						FSensorTubo[cont].valact = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valant = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valrealant = FSensorTubo[cont].valreal;
						break;
					case nmAireFrTubo:
						if(SpeciesModel == nmCalculoSimple) {
							if(HayCombustible) {
								FIndiceTuboAireFresco = 2;
							} else
								FIndiceTuboAireFresco = 1;
						} else {
							printf("No puede haber sensor de Aire Fresco si el calculo de especies no es simple. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorTubo[cont].valreal = Pipe[t]->GetFraccionMasica(0, FIndiceTuboAireFresco);
						FSensorTubo[cont].valact = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valant = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valrealant = FSensorTubo[cont].valreal;
						break;
					case nmGasQuemadoTubo:
						if(SpeciesModel == nmCalculoSimple) {
							FIndiceTuboGasQuemado = 0;
						} else {
							printf("No puede haber sensor de Gas Quemado si el calculo de especies no es simple. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorTubo[cont].valreal = Pipe[t]->GetFraccionMasica(0, FIndiceTuboGasQuemado);
						FSensorTubo[cont].valact = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valant = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valrealant = FSensorTubo[cont].valreal;
						break;
					case nmEGRTubo:
						if(SpeciesModel == nmCalculoCompleto) {
							if(HayCombustible) {
								if(ThereIsEGR) {
									FIndiceTuboEGR = 9;
								} else {
									printf("No puede haber sensor de EGR si no se transporta EGR. Revisa la lectura.\n");
									throw Exception("");
								}
							} else {
								if(ThereIsEGR) {
									FIndiceTuboEGR = 8;
								} else {
									printf("No puede haber sensor de EGR si no se transporta EGR. Revisa la lectura.\n");
									throw Exception("");
								}
							}
						} else {
							if(HayCombustible) {
								if(ThereIsEGR) {
									FIndiceTuboEGR = 3;
								} else {
									printf("No puede haber sensor de EGR si no se transporta EGR. Revisa la lectura.\n");
									throw Exception("");
								}
							} else {
								if(ThereIsEGR) {
									FIndiceTuboEGR = 2;
								} else {
									printf("No puede haber sensor de EGR si no se transporta EGR. Revisa la lectura.\n");
									throw Exception("");
								}
							}
						}
						FSensorTubo[cont].valreal = Pipe[t]->GetFraccionMasica(0, FIndiceTuboEGR);
						FSensorTubo[cont].valact = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valant = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valrealant = FSensorTubo[cont].valreal;
						break;
					case nmCombustibleTubo:
						if(SpeciesModel == nmCalculoCompleto) {
							if(HayCombustible) {
								FIndiceTuboComb = 7;
							} else {
								printf("No puede haber sensor de Combustible si no se transporta. Revisa la lectura.\n");
								throw Exception("");
							}
						} else {
							if(HayCombustible) {
								FIndiceTuboComb = 1;
							} else {
								printf("No puede haber sensor de Combustible si no se transporta. Revisa la lectura.\n");
								throw Exception("");
							}
						}
						FSensorTubo[cont].valreal = Pipe[t]->GetFraccionMasica(0, FIndiceTuboComb);
						FSensorTubo[cont].valact = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valant = FSensorTubo[cont].valreal;
						FSensorTubo[cont].valrealant = FSensorTubo[cont].valreal;
						break;
					}/*fin del switch*/

					cont++;
				}/*Fin for sensores para cada tubo*/
			}/*Fin for numero de distancias*/

		}

		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: Lee_Sens_Tubos" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCalculoExtern::Lee_Sens_Dep(const char *FileWAM, fpos_t &filepos, TDeposito **Plenum,
								  nmTipoCalculoEspecies SpeciesModel, bool ThereIsEGR, bool HayCombustible) {
	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		int tipo = 0, ndep = 0, deposito = 0, nsensdeposito = 0;
		ndep = 0;
		fscanf(fich, "%d ", &FNSensDepositos); /*numero de sensores en depositos*/
		fscanf(fich, "%d ", &ndep); /*numero de depositos con sensores*/
		int cont = 0;
		FSensorDep = new stSensorDep[FNSensDepositos];
		if(FNSensDepositos != 0) {
			for(int i = 0; i <= ndep - 1; ++i) {
				fscanf(fich, "%d %d ", &deposito, &nsensdeposito);
				for(int j = 0; j <= nsensdeposito - 1; ++j) {
					FSensorDep[cont].deposito = deposito;
					fscanf(fich, "%d ", &tipo);
					switch(tipo) {
					case 0:
						FSensorDep[cont].tipo = nmPrDep;
						break;
					case 1:
						FSensorDep[cont].tipo = nmTempDep;
						break;
					case 2:
						FSensorDep[cont].tipo = nmN2Dep;
						break;
					case 3:
						FSensorDep[cont].tipo = nmO2Dep;
						break;
					case 4:
						FSensorDep[cont].tipo = nmCO2Dep;
						break;
					case 5:
						FSensorDep[cont].tipo = nmH2ODep;
						break;
					case 6:
						FSensorDep[cont].tipo = nmHCDep;
						break;
					case 7:
						FSensorDep[cont].tipo = nmSootDep;
						break;
					case 8:
						FSensorDep[cont].tipo = nmNOxDep;
						break;
					case 9:
						FSensorDep[cont].tipo = nmCODep;
						break;
					case 10:
						FSensorDep[cont].tipo = nmAireFrDep;
						break;
					case 11:
						FSensorDep[cont].tipo = nmGasQuemadoDep;
						break;
					case 12:
						FSensorDep[cont].tipo = nmEGRDep;
						break;
					case 13:
						FSensorDep[cont].tipo = nmCombustibleDep;
						break;
					default:
						printf("Error en el tipo de sensor\n");
						throw Exception("");
					} /*fin del switch*/
					fscanf(fich, "%lf ", &FSensorDep[cont].ganancia);
					fscanf(fich, "%lf ", &FSensorDep[cont].ctetiempo);

					switch(FSensorDep[cont].tipo) {
					case nmPrDep:
						FSensorDep[cont].valreal = Plenum[FSensorDep[cont].deposito - 1]->getPressure();
						FSensorDep[cont].valact = FSensorDep[cont].valreal;
						FSensorDep[cont].valant = FSensorDep[cont].valreal;
						FSensorDep[cont].valrealant = FSensorDep[cont].valreal;
						break;
					case nmTempDep:
						FSensorDep[cont].valreal = Plenum[FSensorDep[cont].deposito - 1]->getTemperature();
						FSensorDep[cont].valact = FSensorDep[cont].valreal;
						FSensorDep[cont].valant = FSensorDep[cont].valreal;
						FSensorDep[cont].valrealant = FSensorDep[cont].valreal;
						break;
					case nmN2Dep:
						if(SpeciesModel == nmCalculoCompleto) {
							if(HayCombustible) {
								FIndiceDepN2 = 8;
							} else
								FIndiceDepN2 = 7;
						} else {
							printf("No puede haber sensor de N2 si el calculo de especies no es completo. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorDep[cont].valreal = Plenum[FSensorDep[cont].deposito - 1]->GetFraccionMasicaEspecie(FIndiceDepN2);
						FSensorDep[cont].valact = FSensorDep[cont].valreal;
						FSensorDep[cont].valant = FSensorDep[cont].valreal;
						FSensorDep[cont].valrealant = FSensorDep[cont].valreal;
						break;
					case nmO2Dep:
						if(SpeciesModel == nmCalculoCompleto) {
							FIndiceDepO2 = 0;
						} else {
							printf("No puede haber sensor de O2 si el calculo de especies no es completo. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorDep[cont].valreal = Plenum[FSensorDep[cont].deposito - 1]->GetFraccionMasicaEspecie(FIndiceDepO2);
						FSensorDep[cont].valact = FSensorDep[cont].valreal;
						FSensorDep[cont].valant = FSensorDep[cont].valreal;
						FSensorDep[cont].valrealant = FSensorDep[cont].valreal;
						break;
					case nmCO2Dep:
						if(SpeciesModel == nmCalculoCompleto) {
							FIndiceDepCO2 = 1;
						} else {
							printf("No puede haber sensor de CO2 si el calculo de especies no es completo. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorDep[cont].valreal = Plenum[FSensorDep[cont].deposito - 1]->GetFraccionMasicaEspecie(FIndiceDepCO2);
						FSensorDep[cont].valact = FSensorDep[cont].valreal;
						FSensorDep[cont].valant = FSensorDep[cont].valreal;
						FSensorDep[cont].valrealant = FSensorDep[cont].valreal;
						break;
					case nmH2ODep:
						if(SpeciesModel == nmCalculoCompleto) {
							FIndiceDepH2O = 2;
						} else {
							printf("No puede haber sensor de H2O si el calculo de especies no es completo. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorDep[cont].valreal = Plenum[FSensorDep[cont].deposito - 1]->GetFraccionMasicaEspecie(FIndiceDepH2O);
						FSensorDep[cont].valact = FSensorDep[cont].valreal;
						FSensorDep[cont].valant = FSensorDep[cont].valreal;
						FSensorDep[cont].valrealant = FSensorDep[cont].valreal;
						break;
					case nmHCDep:
						if(SpeciesModel == nmCalculoCompleto) {
							FIndiceDepHC = 3;
						} else {
							printf("No puede haber sensor de HC si el calculo de especies no es completo. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorDep[cont].valreal = Plenum[FSensorDep[cont].deposito - 1]->GetFraccionMasicaEspecie(FIndiceDepHC);
						FSensorDep[cont].valact = FSensorDep[cont].valreal;
						FSensorDep[cont].valant = FSensorDep[cont].valreal;
						FSensorDep[cont].valrealant = FSensorDep[cont].valreal;
						break;
					case nmSootDep:
						if(SpeciesModel == nmCalculoCompleto) {
							FIndiceDepSoot = 4;
						} else {
							printf("No puede haber sensor de Soot si el calculo de especies no es completo. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorDep[cont].valreal = Plenum[FSensorDep[cont].deposito - 1]->GetFraccionMasicaEspecie(FIndiceDepSoot);
						FSensorDep[cont].valact = FSensorDep[cont].valreal;
						FSensorDep[cont].valant = FSensorDep[cont].valreal;
						FSensorDep[cont].valrealant = FSensorDep[cont].valreal;
						break;
					case nmNOxDep:
						if(SpeciesModel == nmCalculoCompleto) {
							FIndiceDepNOx = 5;
						} else {
							printf("No puede haber sensor de NOx si el calculo de especies no es completo. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorDep[cont].valreal = Plenum[FSensorDep[cont].deposito - 1]->GetFraccionMasicaEspecie(FIndiceDepNOx);
						FSensorDep[cont].valact = FSensorDep[cont].valreal;
						FSensorDep[cont].valant = FSensorDep[cont].valreal;
						FSensorDep[cont].valrealant = FSensorDep[cont].valreal;
						break;
					case nmCODep:
						if(SpeciesModel == nmCalculoCompleto) {
							FIndiceDepCO = 6;
						} else {
							printf("No puede haber sensor de CO si el calculo de especies no es completo. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorDep[cont].valreal = Plenum[FSensorDep[cont].deposito - 1]->GetFraccionMasicaEspecie(FIndiceDepCO);
						FSensorDep[cont].valact = FSensorDep[cont].valreal;
						FSensorDep[cont].valant = FSensorDep[cont].valreal;
						FSensorDep[cont].valrealant = FSensorDep[cont].valreal;
						break;
					case nmAireFrDep:
						if(SpeciesModel == nmCalculoSimple) {
							if(HayCombustible) {
								FIndiceDepAireFresco = 2;
							} else
								FIndiceDepAireFresco = 1;
						} else {
							printf("No puede haber sensor de Aire Fresco si el calculo de especies no es simple. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorDep[cont].valreal = Plenum[FSensorDep[cont].deposito - 1]->GetFraccionMasicaEspecie(FIndiceDepAireFresco);
						FSensorDep[cont].valact = FSensorDep[cont].valreal;
						FSensorDep[cont].valant = FSensorDep[cont].valreal;
						FSensorDep[cont].valrealant = FSensorDep[cont].valreal;
						break;
					case nmGasQuemadoDep:
						if(SpeciesModel == nmCalculoSimple) {
							FIndiceDepGasQuemado = 0;
						} else {
							printf("No puede haber sensor de Gas Quemado si el calculo de especies no es simple. Revisa la lectura.\n");
							throw Exception("");
						}
						FSensorDep[cont].valreal = Plenum[FSensorDep[cont].deposito - 1]->GetFraccionMasicaEspecie(FIndiceDepGasQuemado);
						FSensorDep[cont].valact = FSensorDep[cont].valreal;
						FSensorDep[cont].valant = FSensorDep[cont].valreal;
						FSensorDep[cont].valrealant = FSensorDep[cont].valreal;
						break;
					case nmEGRDep:
						if(SpeciesModel == nmCalculoCompleto) {
							if(HayCombustible) {
								if(ThereIsEGR) {
									FIndiceDepEGR = 9;
								} else {
									printf("No puede haber sensor de EGR si no se transporta EGR. Revisa la lectura.\n");
									throw Exception("");
								}
							} else {
								if(ThereIsEGR) {
									FIndiceDepEGR = 8;
								} else {
									printf("No puede haber sensor de EGR si no se transporta EGR. Revisa la lectura.\n");
									throw Exception("");
								}
							}
						} else {
							if(HayCombustible) {
								if(ThereIsEGR) {
									FIndiceDepEGR = 3;
								} else {
									printf("No puede haber sensor de EGR si no se transporta EGR. Revisa la lectura.\n");
									throw Exception("");
								}
							} else {
								if(ThereIsEGR) {
									FIndiceDepEGR = 2;
								} else {
									printf("No puede haber sensor de EGR si no se transporta EGR. Revisa la lectura.\n");
									throw Exception("");
								}
							}
						}
						FSensorDep[cont].valreal = Plenum[FSensorDep[cont].deposito - 1]->GetFraccionMasicaEspecie(FIndiceDepEGR);
						FSensorDep[cont].valact = FSensorDep[cont].valreal;
						FSensorDep[cont].valant = FSensorDep[cont].valreal;
						FSensorDep[cont].valrealant = FSensorDep[cont].valreal;
						break;
					case nmCombustibleDep:
						if(SpeciesModel == nmCalculoCompleto) {
							if(HayCombustible) {
								FIndiceDepComb = 7;
							} else {
								printf("No puede haber sensor de Combustible si no se transporta. Revisa la lectura.\n");
								throw Exception("");
							}
						} else {
							if(HayCombustible) {
								FIndiceDepComb = 1;
							} else {
								printf("No puede haber sensor de Combustible si no se transporta. Revisa la lectura.\n");
								throw Exception("");
							}
						}
						FSensorDep[cont].valreal = Plenum[FSensorDep[cont].deposito - 1]->GetFraccionMasicaEspecie(FIndiceDepComb);
						FSensorDep[cont].valact = FSensorDep[cont].valreal;
						FSensorDep[cont].valant = FSensorDep[cont].valreal;
						FSensorDep[cont].valrealant = FSensorDep[cont].valreal;
						break;
					}/*fin del switch*/

					cont++;
				}/*Fin numero de sensores en deposito*/
			}/*Fin numero de depositos con sensor*/
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: Lee_Sens_Dep" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void TCalculoExtern::Lee_Sens_TG(const char *FileWAM, fpos_t &filepos, TEjeTurbogrupo **Axis) {
	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		int tipo = 0, ntg = 0, turbogrupo = 0, nsensturbogrupo = 0;
		ntg = 0;
		fscanf(fich, "%d ", &FNSensTurbogrupo); /*numero de sensores en TG*/
		fscanf(fich, "%d ", &ntg); /*numero de turbogrupos con sensor*/
		int cont = 0;
		FSensorTG = new stSensorTG[FNSensTurbogrupo];
		if(FNSensTurbogrupo != 0) {
			for(int i = 0; i <= ntg - 1; ++i) {
				fscanf(fich, "%d %d ", &turbogrupo, &nsensturbogrupo);
				for(int j = 0; j <= nsensturbogrupo - 1; ++j) {
					FSensorTG[cont].turbogrupo = turbogrupo;
					fscanf(fich, "%d ", &tipo);
					switch(tipo) {
					case 0:
						FSensorTG[cont].tipo = nmRTCTG;
						break;
					default:
						printf("Error en el tipo de sensor\n");
						throw Exception("");
					} /*fin del switch*/
					fscanf(fich, "%lf ", &FSensorTG[cont].ganancia);
					fscanf(fich, "%lf ", &FSensorTG[cont].ctetiempo);

					switch(FSensorTG[cont].tipo) {
					case nmRTCTG:
						FSensorTG[cont].valreal = Axis[FSensorTG[cont].turbogrupo - 1]->getRegimen();
						FSensorTG[cont].valact = FSensorTG[cont].valreal;
						FSensorTG[cont].valant = FSensorTG[cont].valreal;
						FSensorTG[cont].valrealant = FSensorTG[cont].valreal;
						break;
					}/*fin del switch*/

					cont++;
				}/*Fin numero de sensores en turbogrupo*/
			}/*Fin numero de turbogrupos con sensores*/
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: Lee_Sens_TG" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void TCalculoExtern::Lee_Sens_Turbina(const char *FileWAM, fpos_t &filepos, TTurbina **Turbine) {
	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		int tipo = 0, nturbinas = 0, turbina = 0, nsensturbina = 0;
		nturbinas = 0;
		fscanf(fich, "%d ", &FNSensTurbina); /* numero de sensores en turbina */
		fscanf(fich, "%d ", &nturbinas); /* numero de turbinas con sensor */
		int cont = 0;
		FSensorTurbina = new stSensorTurbina[FNSensTurbina];
		if(FNSensTurbina != 0) {
			for(int i = 0; i <= nturbinas - 1; ++i) {
				fscanf(fich, "%d %d ", &turbina, &nsensturbina);
				for(int j = 0; j <= nsensturbina - 1; ++j) {
					FSensorTurbina[cont].turbina = turbina;
					fscanf(fich, "%d ", &tipo);
					switch(tipo) {
					case 0:
						FSensorTurbina[cont].tipo = nmRelaci1;
						break;
					case 1:
						FSensorTurbina[cont].tipo = nmRelaci2;
						break;
					default:
						printf("Error en el tipo de sensor\n");
						throw Exception("");
					} /*fin del switch*/
					fscanf(fich, "%lf ", &FSensorTurbina[cont].ganancia);
					fscanf(fich, "%lf ", &FSensorTurbina[cont].ctetiempo);

					switch(FSensorTurbina[cont].tipo) {
					case nmRelaci1:
						FSensorTurbina[cont].valreal = Turbine[FSensorTurbina[cont].turbina - 1]->GetRelacionCinematica(0);
						FSensorTurbina[cont].valact = FSensorTurbina[cont].valreal;
						FSensorTurbina[cont].valant = FSensorTurbina[cont].valreal;
						FSensorTurbina[cont].valrealant = FSensorTurbina[cont].valreal;
						break;
					case nmRelaci2:
						FSensorTurbina[cont].valreal = Turbine[FSensorTurbina[cont].turbina - 1]->GetRelacionCinematica(1);
						FSensorTurbina[cont].valact = FSensorTurbina[cont].valreal;
						FSensorTurbina[cont].valant = FSensorTurbina[cont].valreal;
						FSensorTurbina[cont].valrealant = FSensorTurbina[cont].valreal;
						break;
					} /*fin del switch*/

					cont++;
				}/*Fin numero de sensores en turbina*/
			}/*Fin numero de turbinas con sensores*/
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: TCalculoExtern::Lee_Sens_Turbina" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void TCalculoExtern::Lee_Sens_Cil(const char *FileWAM, fpos_t &filepos, TBloqueMotor **Engine) {
	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		int tipo = 0, ncil = 0, cilindro = 0, nsenscilindro = 0;
		ncil = 0;
		fscanf(fich, "%d ", &FNSensCilindros); /*numero de sensores en cilindros*/
		fscanf(fich, "%d ", &ncil); /*numero de cilindros con sensores*/
		int cont = 0;
		FSensorCil = new stSensorCil[FNSensCilindros];
		if(FNSensCilindros != 0) {
			for(int i = 0; i <= ncil - 1; ++i) {
				fscanf(fich, "%d %d ", &cilindro, &nsenscilindro);
				for(int j = 0; j <= nsenscilindro - 1; ++j) {
					FSensorCil[cont].cilindro = cilindro;
					fscanf(fich, "%d ", &tipo);
					switch(tipo) {
					case 0:
						FSensorCil[cont].tipo = nmPrCil;
						break;
					case 1:
						FSensorCil[cont].tipo = nmTempCil;
						break;
					case 2:
						FSensorCil[cont].tipo = nmMfCil;
						break;
					case 3:
						FSensorCil[cont].tipo = nmAFRCil;
						break;
					default:
						printf("Error en el tipo de sensor\n");
						throw Exception("");
					} /*fin del switch*/
					fscanf(fich, "%lf ", &FSensorCil[cont].ganancia);
					fscanf(fich, "%lf ", &FSensorCil[cont].ctetiempo);

					switch(FSensorCil[cont].tipo) {
					case nmTempCil:
						FSensorCil[cont].valreal = Engine[0]->GetCilindro(FSensorCil[cont].cilindro - 1)->getTemperature();
						FSensorCil[cont].valact = FSensorCil[cont].valreal;
						FSensorCil[cont].valant = FSensorCil[cont].valreal;
						FSensorCil[cont].valrealant = FSensorCil[cont].valreal;
						break;
					case nmPrCil:
						FSensorCil[cont].valreal = Engine[0]->GetCilindro(FSensorCil[cont].cilindro - 1)->getPressure();
						FSensorCil[cont].valact = FSensorCil[cont].valreal;
						FSensorCil[cont].valant = FSensorCil[cont].valreal;
						FSensorCil[cont].valrealant = FSensorCil[cont].valreal;
						break;
					case nmMfCil:
						FSensorCil[cont].valreal = Engine[0]->GetCilindro(FSensorCil[cont].cilindro - 1)->getMasaFuel();
						FSensorCil[cont].valact = FSensorCil[cont].valreal;
						FSensorCil[cont].valant = FSensorCil[cont].valreal;
						FSensorCil[cont].valrealant = FSensorCil[cont].valreal;
						break;
					case nmAFRCil:
						FSensorCil[cont].valreal = 15;
						FSensorCil[cont].valact = FSensorCil[cont].valreal;
						FSensorCil[cont].valant = FSensorCil[cont].valreal;
						FSensorCil[cont].valrealant = FSensorCil[cont].valreal;
						break;

					}/*fin del switch*/

					cont++;
				}/*Fin numero de sensores en cilindro*/
			}/*Fin numero de cilindros con sensor*/
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: Lee_Sens_Cil" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void TCalculoExtern::Lee_Sens_Vent(const char *FileWAM, fpos_t &filepos, TVenturi **Venturi) {
	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		int signo = 0;
		double prent = 0., prgar = 0., velent = 0., vellat = 0., gasent = 0., gaslat = 0.;
		int tipo = 0, nventuris = 0, venturi = 0, nsensventuri = 0, v = 0;
		nventuris = 0;
		fscanf(fich, "%d ", &FNSensVenturis); /*numero de sensores en venturis*/
		fscanf(fich, "%d ", &nventuris);
		int cont = 0;
		FSensorVent = new stSensorVent[FNSensVenturis];
		if(FNSensVenturis != 0) {
			for(int i = 0; i <= nventuris - 1; ++i) {
				fscanf(fich, "%d %d ", &venturi, &nsensventuri);
				for(int j = 0; j <= nsensventuri - 1; ++j) {
					FSensorVent[cont].venturi = venturi;
					fscanf(fich, "%d ", &tipo);
					switch(tipo) {
					case 0:
						FSensorVent[cont].tipo = nmPrEntVent;
						break;
					case 1:
						FSensorVent[cont].tipo = nmPrGarVent;
						break;
					case 2:
						FSensorVent[cont].tipo = nmVelEntVent;
						break;
					case 3:
						FSensorVent[cont].tipo = nmVelLatVent;
						break;
					case 4:
						FSensorVent[cont].tipo = nmGastoEntVent;
						break;
					case 5:
						FSensorVent[cont].tipo = nmGastoLatVent;
						break;
					default:
						printf("Error en el tipo de sensor\n");
						throw Exception("");
					} /*fin del switch*/
					fscanf(fich, "%lf ", &FSensorVent[cont].ganancia);
					fscanf(fich, "%lf ", &FSensorVent[cont].ctetiempo);

					v = FSensorVent[cont].venturi - 1;
					switch(FSensorVent[FNSensVenturis].tipo) {
					case nmPrEntVent:
						prent = Venturi[v]->getPressure();
						FSensorVent[cont].valreal = prent;
						FSensorVent[cont].valact = FSensorVent[cont].valreal;
						FSensorVent[cont].valant = FSensorVent[cont].valreal;
						FSensorVent[cont].valrealant = FSensorVent[cont].valreal;
						break;
					case nmPrGarVent:
						prent = Venturi[v]->getPressure();
						prgar = prent / pow(1 + Venturi[v]->getgamma3() * pow(dynamic_cast<TCCDeposito *>
											(Venturi[v]->getCCLateral())->getValvula()->getCRecuperacion(), 2.),
											Venturi[v]->getGamma() / Venturi[v]->getgamma1());
						FSensorVent[cont].valreal = prgar;
						FSensorVent[cont].valact = FSensorVent[cont].valreal;
						FSensorVent[cont].valant = FSensorVent[cont].valreal;
						FSensorVent[cont].valrealant = FSensorVent[cont].valreal;
						break;
					case nmVelEntVent:
						if(dynamic_cast<TCCDeposito *>(Venturi[v]->getCCEntrada())->getSentidoFlujo() == nmEntrante) {
							signo = 1; /* Si el flujo es entrante signo de la velocidad positivo */
						} else if(dynamic_cast<TCCDeposito *>(Venturi[v]->getCCEntrada())->getSentidoFlujo() == nmSaliente) {
							signo = -1; /* Si el flujo es saliente signo de la velocidad negativo */
						}
						velent = dynamic_cast<TCCDeposito *>(Venturi[v]->getCCEntrada())->getVelocity() * signo * __cons::ARef;
						FSensorVent[cont].valreal = velent;
						FSensorVent[cont].valact = FSensorVent[cont].valreal;
						FSensorVent[cont].valant = FSensorVent[cont].valreal;
						FSensorVent[cont].valrealant = FSensorVent[cont].valreal;
						break;
					case nmVelLatVent:
						vellat = dynamic_cast<TCCDeposito *>(Venturi[v]->getCCLateral())->getVelocity() * __cons::ARef;
						FSensorVent[cont].valreal = vellat;
						FSensorVent[cont].valact = FSensorVent[cont].valreal;
						FSensorVent[cont].valant = FSensorVent[cont].valreal;
						FSensorVent[cont].valrealant = FSensorVent[cont].valreal;
						break;
					case nmGastoEntVent:
						gasent = -dynamic_cast<TCCDeposito *>
								 (Venturi[v]->getCCEntrada())->getMassflow(); /*Massflow entrante positivo.Saliente negativo. Pedro */
						/* gasent lleva el "-" delante porque se usa el convenio de signos de los depositos,que es el contrario al de las BC, de la que se obtiene el massflow */
						FSensorVent[cont].valreal = gasent;
						FSensorVent[cont].valact = FSensorVent[cont].valreal;
						FSensorVent[cont].valant = FSensorVent[cont].valreal;
						FSensorVent[cont].valrealant = FSensorVent[cont].valreal;
						break;
					case nmGastoLatVent:
						gaslat = dynamic_cast<TCCDeposito *>(Venturi[v]->getCCLateral())->getMassflow();
						FSensorVent[cont].valreal = gaslat;
						FSensorVent[cont].valact = FSensorVent[cont].valreal;
						FSensorVent[cont].valant = FSensorVent[cont].valreal;
						FSensorVent[cont].valrealant = FSensorVent[cont].valreal;
						break;

					}/*fin del switch*/

					cont++;
				}/*Fin numero de sensores en venturi*/
			}/*Fin numero de venturis con sensor*/

		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: Lee_Sens_Vent" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void TCalculoExtern::Lee_Sens_Motor(const char *FileWAM, fpos_t &filepos, double CrankAngle, double ene,
									double AcumulatedTime) {
	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		int tipo = 0;
		fscanf(fich, "%d ", &FNSensMotor); /*numero de sensores en motor*/
		if(ene < 0. && FNSensMotor != 0) {
			std::cout << "ERROR: No puede haber sensores en el motor,pues no hay motor.Comprobad la lectura " << std::endl;
			throw Exception("");
		}
		FSensorMotor = new stSensorMotor[FNSensMotor];
		if(FNSensMotor != 0) {
			for(int i = 0; i <= FNSensMotor - 1; ++i) {
				fscanf(fich, "%d ", &tipo);
				FSensorMotor[i].motor = 1;
				switch(tipo) {
				case 0:
					FSensorMotor[i].tipo = nmAngulo;
					break;
				case 1:
					FSensorMotor[i].tipo = nmReg;
					break;
				case 2:
					FSensorMotor[i].tipo = nmTiempo;
					break;
				default:
					printf("Error en el tipo de sensor\n");
					throw Exception("");
				} /*fin del switch*/

				fscanf(fich, "%lf ", &FSensorMotor[i].ganancia);
				fscanf(fich, "%lf ", &FSensorMotor[i].ctetiempo);

				switch(FSensorMotor[i].tipo) {
				case nmAngulo:
					FSensorMotor[i].valreal = CrankAngle;
					FSensorMotor[i].valact = FSensorMotor[i].valreal;
					FSensorMotor[i].valant = FSensorMotor[i].valreal;
					FSensorMotor[i].valrealant = FSensorMotor[i].valreal;
					break;
				case nmReg:
					FSensorMotor[i].valreal = ene;
					FSensorMotor[i].valact = FSensorMotor[i].valreal;
					FSensorMotor[i].valant = FSensorMotor[i].valreal;
					FSensorMotor[i].valrealant = FSensorMotor[i].valreal;
					break;
				case nmTiempo:
					FSensorMotor[i].valreal = AcumulatedTime;
					FSensorMotor[i].valact = FSensorMotor[i].valreal;
					FSensorMotor[i].valant = FSensorMotor[i].valreal;
					FSensorMotor[i].valrealant = FSensorMotor[i].valreal;
					break;

				}/*fin del switch*/
			}
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: Lee_Sens_Motor" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void TCalculoExtern::Lee_Sens_UED(const char *FileWAM, fpos_t &filepos, TCondicionContorno **BC) {
	try {
		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);
		int nUED = 0, CCued = 0, nsensUED = 0;
		int tipo, cont = 0;
		fscanf(fich, "%d ", &FNSensUED); /*numero de sensores en UED*/
		fscanf(fich, "%d ", &nUED); /*numero de uniones entre tubos con sensores*/
		FSensorUED = new stSensorUED[FNSensUED];
		if(FNSensUED != 0) {
			for(int i = 0; i <= nUED - 1; ++i) {
				fscanf(fich, "%d %d ", &CCued, &nsensUED);
				for(int j = 0; j <= nsensUED - 1; ++j) {
					FSensorUED[cont].CCUED = CCued;
					fscanf(fich, "%d ", &tipo);
					switch(tipo) {
					case 0:
						FSensorUED[cont].tipo = nmGasto;
						break;
					default:
						printf("Error en el tipo de sensor\n");
						throw Exception("");
					} /*fin del switch*/

					fscanf(fich, "%lf ", &FSensorUED[cont].ganancia);
					fscanf(fich, "%lf ", &FSensorUED[cont].ctetiempo);

					switch(FSensorUED[cont].tipo) {
					case nmGasto:
						FSensorUED[cont].valreal = dynamic_cast<TCCUnionEntreDepositos*>(BC[FSensorUED[cont].CCUED - 1])->getGastoImpreso();
						FSensorUED[cont].valact = FSensorUED[cont].valreal;
						FSensorUED[cont].valant = FSensorUED[cont].valreal;
						FSensorUED[cont].valrealant = FSensorUED[cont].valreal;
						break;
					}/*fin del switch*/
					cont++;
				}/*Fin numero de sensores en UED*/
			}/*Fin numero de UED con sensor*/
		}
		fgetpos(fich, &filepos);
		fclose(fich);
	} catch(exception &N) {
		std::cout << "ERROR: Lee_Sens_UED" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void TCalculoExtern::Lectura_Datos_Adicionales(const char *FileWAM, fpos_t &filepos) {
	try {

	} catch(exception &N) {
		std::cout << "ERROR: Lectura_Datos_Adicionales" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void TCalculoExtern::IniciaEntradaDLL() {
	try {
		FNInputsdll = FNSensTubos + FNSensDepositos + FNSensTurbogrupo + FNSensTurbina + FNSensMotor + FNSensVenturis +
					  FNSensCilindros + FNSensUED;
		FInputs = new double[FNInputsdll];
	} catch(exception &N) {
		std::cout << "ERROR: IniciaEntradaDLL" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void TCalculoExtern::Calculo_Sensores_Tubos(TTubo **Pipe, double deltaT) {
	try {
		double dist = 0., T = 0., temp = 0., v1 = 0., v2 = 0., den = 0., d = 0., p = 0., gto1 = 0., gto2 = 0., gamma, Rmezcla;
		int n1 = 0, j = 0, n2 = 0;
		for(int i = 0; i <= FNSensTubos - 1; i++) {
			dist = FSensorTubo[i].distancia / FSensorTubo[i].deltax;
			n1 = (int) floor(dist);
			j = FSensorTubo[i].tubo - 1;
			if(n1 == FSensorTubo[i].nodos - 1) {
				switch(FSensorTubo[i].tipo) {
				case nmTempTubo:
					temp = Pipe[j]->GetAsonido(n1) * __cons::ARef;
					FSensorTubo[i].valreal = __units::KTodegC(temp * temp / Pipe[j]->GetGamma(n1) / Pipe[j]->GetRMezcla(n1));
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmPrTubo:
					FSensorTubo[i].valreal = Pipe[j]->GetPresion(n1);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmVelTubo:
					FSensorTubo[i].valreal = Pipe[j]->GetVelocidad(n1) * __cons::ARef;
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmGastoTubo:
					temp = Pipe[j]->GetAsonido(n1) * __cons::ARef;
					T = __units::KTodegC(temp * temp / Pipe[j]->GetGamma(n1) / Pipe[j]->GetRMezcla(n1));
					den = __units::BarToPa(Pipe[j]->GetPresion(n1)) / Pipe[j]->GetRMezcla(n1) / __units::degCToK(T);
					v1 = Pipe[j]->GetVelocidad(n1) * __cons::ARef;
					FSensorTubo[i].valreal = (pow2(Pipe[j]->GetDiametro(n1)) * __cons::Pi) * v1 * den;
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmN2Tubo:
					FSensorTubo[i].valreal = Pipe[j]->GetFraccionMasica(n1, FIndiceTuboN2);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmO2Tubo:
					FSensorTubo[i].valreal = Pipe[j]->GetFraccionMasica(n1, FIndiceTuboO2);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmCO2Tubo:
					FSensorTubo[i].valreal = Pipe[j]->GetFraccionMasica(n1, FIndiceTuboCO2);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmH2OTubo:
					FSensorTubo[i].valreal = Pipe[j]->GetFraccionMasica(n1, FIndiceTuboH2O);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmHCTubo:
					FSensorTubo[i].valreal = Pipe[j]->GetFraccionMasica(n1, FIndiceTuboHC);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmSootTubo:
					FSensorTubo[i].valreal = Pipe[j]->GetFraccionMasica(n1, FIndiceTuboSoot);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmNOxTubo:
					FSensorTubo[i].valreal = Pipe[j]->GetFraccionMasica(n1, FIndiceTuboNOx);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmCOTubo:
					FSensorTubo[i].valreal = Pipe[j]->GetFraccionMasica(n1, FIndiceTuboCO);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmAireFrTubo:
					FSensorTubo[i].valreal = Pipe[j]->GetFraccionMasica(n1, FIndiceTuboAireFresco);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmGasQuemadoTubo:
					FSensorTubo[i].valreal = Pipe[j]->GetFraccionMasica(n1, FIndiceTuboGasQuemado);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmEGRTubo:
					FSensorTubo[i].valreal = Pipe[j]->GetFraccionMasica(n1, FIndiceTuboEGR);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmCombustibleTubo:
					FSensorTubo[i].valreal = Pipe[j]->GetFraccionMasica(n1, FIndiceTuboComb);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				}/*fin del switch*/
			} else {
				n2 = n1 + 1;
				d = dist - n1;
				switch(FSensorTubo[i].tipo) {
				case nmTempTubo:
					temp = xit_(Pipe[j]->GetAsonido(n1), Pipe[j]->GetAsonido(n2), 1.0, d) * __cons::ARef;
					gamma = xit_(Pipe[j]->GetGamma(n1), Pipe[j]->GetGamma(n2), 1.0, d);
					Rmezcla = xit_(Pipe[j]->GetRMezcla(n1), Pipe[j]->GetRMezcla(n2), 1.0, d);
					FSensorTubo[i].valreal = __units::KTodegC(temp * temp / gamma / Rmezcla);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmPrTubo:
					FSensorTubo[i].valreal = xit_(Pipe[j]->GetPresion(n1), Pipe[j]->GetPresion(n2), 1.0, d);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmVelTubo:
					FSensorTubo[i].valreal = xit_(Pipe[j]->GetVelocidad(n1), Pipe[j]->GetVelocidad(n2), 1.0, d) * __cons::ARef;
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmGastoTubo:
					temp = xit_(Pipe[j]->GetAsonido(n1), Pipe[j]->GetAsonido(n2), 1.0, d) * __cons::ARef;
					gamma = xit_(Pipe[j]->GetGamma(n1), Pipe[j]->GetGamma(n2), 1.0, d);
					Rmezcla = xit_(Pipe[j]->GetRMezcla(n1), Pipe[j]->GetRMezcla(n2), 1.0, d);
					T = __units::KTodegC(temp * temp / gamma / Rmezcla);
					p = xit_(Pipe[j]->GetPresion(n1), Pipe[j]->GetPresion(n2), 1.0, d);
					den = __units::BarToPa(p) / Rmezcla / __units::degCToK(T);
					v1 = Pipe[j]->GetVelocidad(n1) * __cons::ARef;
					v2 = Pipe[j]->GetVelocidad(n2) * __cons::ARef;
					gto1 = __geom::Circle_area(Pipe[j]->GetDiametro(n1));
					gto2 = __geom::Circle_area(Pipe[j]->GetDiametro(n2));
					gto1 *= v1;
					gto2 *= v2;
					FSensorTubo[i].valreal = xit_(gto1, gto2, 1.0, d) * den;
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmN2Tubo:
					FSensorTubo[i].valreal = xit_(Pipe[j]->GetFraccionMasica(n1, FIndiceTuboN2), Pipe[j]->GetFraccionMasica(n1,
												  FIndiceTuboN2), 1.0, d);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmO2Tubo:
					FSensorTubo[i].valreal = xit_(Pipe[j]->GetFraccionMasica(n1, FIndiceTuboO2), Pipe[j]->GetFraccionMasica(n1,
												  FIndiceTuboO2), 1.0, d);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmCO2Tubo:
					FSensorTubo[i].valreal = xit_(Pipe[j]->GetFraccionMasica(n1, FIndiceTuboCO2), Pipe[j]->GetFraccionMasica(n1,
												  FIndiceTuboCO2), 1.0, d);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmH2OTubo:
					FSensorTubo[i].valreal = xit_(Pipe[j]->GetFraccionMasica(n1, FIndiceTuboH2O), Pipe[j]->GetFraccionMasica(n1,
												  FIndiceTuboH2O), 1.0, d);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmHCTubo:
					FSensorTubo[i].valreal = xit_(Pipe[j]->GetFraccionMasica(n1, FIndiceTuboHC), Pipe[j]->GetFraccionMasica(n1,
												  FIndiceTuboHC), 1.0, d);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmSootTubo:
					FSensorTubo[i].valreal = xit_(Pipe[j]->GetFraccionMasica(n1, FIndiceTuboSoot), Pipe[j]->GetFraccionMasica(n1,
												  FIndiceTuboSoot), 1.0, d);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmNOxTubo:
					FSensorTubo[i].valreal = xit_(Pipe[j]->GetFraccionMasica(n1, FIndiceTuboNOx), Pipe[j]->GetFraccionMasica(n1,
												  FIndiceTuboNOx), 1.0, d);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmCOTubo:
					FSensorTubo[i].valreal = xit_(Pipe[j]->GetFraccionMasica(n1, FIndiceTuboCO), Pipe[j]->GetFraccionMasica(n1,
												  FIndiceTuboCO), 1.0, d);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmAireFrTubo:
					FSensorTubo[i].valreal = xit_(Pipe[j]->GetFraccionMasica(n1, FIndiceTuboAireFresco), Pipe[j]->GetFraccionMasica(n1,
												  FIndiceTuboAireFresco), 1.0, d);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmGasQuemadoTubo:
					FSensorTubo[i].valreal = xit_(Pipe[j]->GetFraccionMasica(n1, FIndiceTuboGasQuemado), Pipe[j]->GetFraccionMasica(n1,
												  FIndiceTuboGasQuemado), 1.0, d);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmEGRTubo:
					FSensorTubo[i].valreal = xit_(Pipe[j]->GetFraccionMasica(n1, FIndiceTuboEGR), Pipe[j]->GetFraccionMasica(n1,
												  FIndiceTuboEGR), 1.0, d);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				case nmCombustibleTubo:
					FSensorTubo[i].valreal = xit_(Pipe[j]->GetFraccionMasica(n1, FIndiceTuboComb), Pipe[j]->GetFraccionMasica(n1,
												  FIndiceTuboComb), 1.0, d);
					FSensorTubo[i].valact = ((2 * FSensorTubo[i].ctetiempo - deltaT) * FSensorTubo[i].valant + deltaT *
											 FSensorTubo[i].ganancia * (FSensorTubo[i].valreal + FSensorTubo[i].valrealant)) / (2 * FSensorTubo[i].ctetiempo +
													 deltaT);
					FSensorTubo[i].valant = FSensorTubo[i].valact;
					FSensorTubo[i].valrealant = FSensorTubo[i].valreal;
					break;
				}/*fin del switch*/
			}/*fin del else*/

		}/*fin del for*/
	} catch(exception &N) {
		std::cout << "ERROR: Calculo_Sensores_Tubos" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void TCalculoExtern::Calculo_Sensores_Deposito(TDeposito **Plenum, double deltaT) {
	try {
		for(int i = 0; i <= FNSensDepositos - 1; i++) {
			switch(FSensorDep[i].tipo) {
			case nmPrDep:
				FSensorDep[i].valreal = Plenum[FSensorDep[i].deposito - 1]->getPressure();
				FSensorDep[i].valact = ((2 * FSensorDep[i].ctetiempo - deltaT) * FSensorDep[i].valant + deltaT * FSensorDep[i].ganancia
										* (FSensorDep[i].valreal + FSensorDep[i].valrealant)) / (2 * FSensorDep[i].ctetiempo + deltaT);
				FSensorDep[i].valant = FSensorDep[i].valact;
				FSensorDep[i].valrealant = FSensorDep[i].valreal;
				break;
			case nmTempDep:
				FSensorDep[i].valreal = Plenum[FSensorDep[i].deposito - 1]->getTemperature();
				FSensorDep[i].valact = ((2 * FSensorDep[i].ctetiempo - deltaT) * FSensorDep[i].valant + deltaT * FSensorDep[i].ganancia
										* (FSensorDep[i].valreal + FSensorDep[i].valrealant)) / (2 * FSensorDep[i].ctetiempo + deltaT);
				FSensorDep[i].valant = FSensorDep[i].valact;
				FSensorDep[i].valrealant = FSensorDep[i].valreal;
				break;
			case nmN2Dep:
				FSensorDep[i].valreal = Plenum[FSensorDep[i].deposito - 1]->GetFraccionMasicaEspecie(FIndiceTuboN2);
				FSensorDep[i].valact = ((2 * FSensorDep[i].ctetiempo - deltaT) * FSensorDep[i].valant + deltaT * FSensorDep[i].ganancia
										* (FSensorDep[i].valreal + FSensorDep[i].valrealant)) / (2 * FSensorDep[i].ctetiempo + deltaT);
				FSensorDep[i].valant = FSensorDep[i].valact;
				FSensorDep[i].valrealant = FSensorDep[i].valreal;
				break;
			case nmO2Dep:
				FSensorDep[i].valreal = Plenum[FSensorDep[i].deposito - 1]->GetFraccionMasicaEspecie(FIndiceTuboO2);
				FSensorDep[i].valact = ((2 * FSensorDep[i].ctetiempo - deltaT) * FSensorDep[i].valant + deltaT * FSensorDep[i].ganancia
										* (FSensorDep[i].valreal + FSensorDep[i].valrealant)) / (2 * FSensorDep[i].ctetiempo + deltaT);
				FSensorDep[i].valant = FSensorDep[i].valact;
				FSensorDep[i].valrealant = FSensorDep[i].valreal;
				break;
			case nmCO2Dep:
				FSensorDep[i].valreal = Plenum[FSensorDep[i].deposito - 1]->GetFraccionMasicaEspecie(FIndiceTuboCO2);
				FSensorDep[i].valact = ((2 * FSensorDep[i].ctetiempo - deltaT) * FSensorDep[i].valant + deltaT * FSensorDep[i].ganancia
										* (FSensorDep[i].valreal + FSensorDep[i].valrealant)) / (2 * FSensorDep[i].ctetiempo + deltaT);
				FSensorDep[i].valant = FSensorDep[i].valact;
				FSensorDep[i].valrealant = FSensorDep[i].valreal;
				break;
			case nmH2ODep:
				FSensorDep[i].valreal = Plenum[FSensorDep[i].deposito - 1]->GetFraccionMasicaEspecie(FIndiceTuboH2O);
				FSensorDep[i].valact = ((2 * FSensorDep[i].ctetiempo - deltaT) * FSensorDep[i].valant + deltaT * FSensorDep[i].ganancia
										* (FSensorDep[i].valreal + FSensorDep[i].valrealant)) / (2 * FSensorDep[i].ctetiempo + deltaT);
				FSensorDep[i].valant = FSensorDep[i].valact;
				FSensorDep[i].valrealant = FSensorDep[i].valreal;
				break;
			case nmHCDep:
				FSensorDep[i].valreal = Plenum[FSensorDep[i].deposito - 1]->GetFraccionMasicaEspecie(FIndiceTuboHC);
				FSensorDep[i].valact = ((2 * FSensorDep[i].ctetiempo - deltaT) * FSensorDep[i].valant + deltaT * FSensorDep[i].ganancia
										* (FSensorDep[i].valreal + FSensorDep[i].valrealant)) / (2 * FSensorDep[i].ctetiempo + deltaT);
				FSensorDep[i].valant = FSensorDep[i].valact;
				FSensorDep[i].valrealant = FSensorDep[i].valreal;
				break;
			case nmSootDep:
				FSensorDep[i].valreal = Plenum[FSensorDep[i].deposito - 1]->GetFraccionMasicaEspecie(FIndiceTuboSoot);
				FSensorDep[i].valact = ((2 * FSensorDep[i].ctetiempo - deltaT) * FSensorDep[i].valant + deltaT * FSensorDep[i].ganancia
										* (FSensorDep[i].valreal + FSensorDep[i].valrealant)) / (2 * FSensorDep[i].ctetiempo + deltaT);
				FSensorDep[i].valant = FSensorDep[i].valact;
				FSensorDep[i].valrealant = FSensorDep[i].valreal;
				break;
			case nmNOxDep:
				FSensorDep[i].valreal = Plenum[FSensorDep[i].deposito - 1]->GetFraccionMasicaEspecie(FIndiceTuboNOx);
				FSensorDep[i].valact = ((2 * FSensorDep[i].ctetiempo - deltaT) * FSensorDep[i].valant + deltaT * FSensorDep[i].ganancia
										* (FSensorDep[i].valreal + FSensorDep[i].valrealant)) / (2 * FSensorDep[i].ctetiempo + deltaT);
				FSensorDep[i].valant = FSensorDep[i].valact;
				FSensorDep[i].valrealant = FSensorDep[i].valreal;
				break;
			case nmCODep:
				FSensorDep[i].valreal = Plenum[FSensorDep[i].deposito - 1]->GetFraccionMasicaEspecie(FIndiceTuboCO);
				FSensorDep[i].valact = ((2 * FSensorDep[i].ctetiempo - deltaT) * FSensorDep[i].valant + deltaT * FSensorDep[i].ganancia
										* (FSensorDep[i].valreal + FSensorDep[i].valrealant)) / (2 * FSensorDep[i].ctetiempo + deltaT);
				FSensorDep[i].valant = FSensorDep[i].valact;
				FSensorDep[i].valrealant = FSensorDep[i].valreal;
				break;
			case nmAireFrDep:
				FSensorDep[i].valreal = Plenum[FSensorDep[i].deposito - 1]->GetFraccionMasicaEspecie(FIndiceTuboAireFresco);
				FSensorDep[i].valact = ((2 * FSensorDep[i].ctetiempo - deltaT) * FSensorDep[i].valant + deltaT * FSensorDep[i].ganancia
										* (FSensorDep[i].valreal + FSensorDep[i].valrealant)) / (2 * FSensorDep[i].ctetiempo + deltaT);
				FSensorDep[i].valant = FSensorDep[i].valact;
				FSensorDep[i].valrealant = FSensorDep[i].valreal;
				break;
			case nmGasQuemadoDep:
				FSensorDep[i].valreal = Plenum[FSensorDep[i].deposito - 1]->GetFraccionMasicaEspecie(FIndiceTuboGasQuemado);
				FSensorDep[i].valact = ((2 * FSensorDep[i].ctetiempo - deltaT) * FSensorDep[i].valant + deltaT * FSensorDep[i].ganancia
										* (FSensorDep[i].valreal + FSensorDep[i].valrealant)) / (2 * FSensorDep[i].ctetiempo + deltaT);
				FSensorDep[i].valant = FSensorDep[i].valact;
				FSensorDep[i].valrealant = FSensorDep[i].valreal;
				break;
			case nmEGRDep:
				FSensorDep[i].valreal = Plenum[FSensorDep[i].deposito - 1]->GetFraccionMasicaEspecie(FIndiceTuboEGR);
				FSensorDep[i].valact = ((2 * FSensorDep[i].ctetiempo - deltaT) * FSensorDep[i].valant + deltaT * FSensorDep[i].ganancia
										* (FSensorDep[i].valreal + FSensorDep[i].valrealant)) / (2 * FSensorDep[i].ctetiempo + deltaT);
				FSensorDep[i].valant = FSensorDep[i].valact;
				FSensorDep[i].valrealant = FSensorDep[i].valreal;
				break;
			case nmCombustibleDep:
				FSensorDep[i].valreal = Plenum[FSensorDep[i].deposito - 1]->GetFraccionMasicaEspecie(FIndiceTuboComb);
				FSensorDep[i].valact = ((2 * FSensorDep[i].ctetiempo - deltaT) * FSensorDep[i].valant + deltaT * FSensorDep[i].ganancia
										* (FSensorDep[i].valreal + FSensorDep[i].valrealant)) / (2 * FSensorDep[i].ctetiempo + deltaT);
				FSensorDep[i].valant = FSensorDep[i].valact;
				FSensorDep[i].valrealant = FSensorDep[i].valreal;
				break;
			}/*fin del switch*/
		}/*fin del for*/
	} catch(exception &N) {
		std::cout << "ERROR: Calculo_Sensores_Deposito" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void TCalculoExtern::Calculo_Sensores_TG(double deltaT, TEjeTurbogrupo **Axis) {
	try {
		for(int i = 0; i <= FNSensTurbogrupo - 1; i++) {
			switch(FSensorTG[i].tipo) {
			case nmRTCTG:
				FSensorTG[i].valreal = Axis[FSensorTG[i].turbogrupo - 1]->getRegimen();
				FSensorTG[i].valact = ((2 * FSensorTG[i].ctetiempo - deltaT) * FSensorTG[i].valant + deltaT * FSensorTG[i].ganancia *
									   (FSensorTG[i].valreal + FSensorTG[i].valrealant)) / (2 * FSensorTG[i].ctetiempo + deltaT);
				FSensorTG[i].valant = FSensorTG[i].valact;
				FSensorTG[i].valrealant = FSensorTG[i].valreal;
				break;
			}/*fin del switch*/
		}/*fin del for*/
	} catch(exception &N) {
		std::cout << "ERROR: Calculo_Sensores_TG" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void TCalculoExtern::Calculo_Sensores_Turbina(double deltaT, TTurbina **Turbine) {
	try {
		for(int i = 0; i <= FNSensTurbina - 1; i++) {
			switch(FSensorTurbina[i].tipo) {
			case nmRelaci1:
				FSensorTurbina[i].valreal = Turbine[FSensorTurbina[i].turbina - 1]->GetRelacionCinematica(0);
				FSensorTurbina[i].valact = ((2 * FSensorTurbina[i].ctetiempo - deltaT) * FSensorTurbina[i].valant + deltaT *
											FSensorTurbina[i].ganancia * (FSensorTurbina[i].valreal + FSensorTurbina[i].valrealant)) /
										   (2 * FSensorTurbina[i].ctetiempo + deltaT);
				FSensorTurbina[i].valant = FSensorTurbina[i].valact;
				FSensorTurbina[i].valrealant = FSensorTurbina[i].valreal;
				break;
			case nmRelaci2:
				FSensorTurbina[i].valreal = Turbine[FSensorTurbina[i].turbina - 1]->GetRelacionCinematica(1);
				FSensorTurbina[i].valact = ((2 * FSensorTurbina[i].ctetiempo - deltaT) * FSensorTurbina[i].valant + deltaT *
											FSensorTurbina[i].ganancia * (FSensorTurbina[i].valreal + FSensorTurbina[i].valrealant)) /
										   (2 * FSensorTurbina[i].ctetiempo + deltaT);
				FSensorTurbina[i].valant = FSensorTurbina[i].valact;
				FSensorTurbina[i].valrealant = FSensorTurbina[i].valreal;
				break;
			}/*fin del switch*/
		}/*fin del for*/
	} catch(exception &N) {
		std::cout << "ERROR: TCalculoExtern::Calculo_Sensores_Turbina " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void TCalculoExtern::Calculo_Sensores_Cilindro(double deltaT, TBloqueMotor **Engine) {
	try {
		for(int i = 0; i <= FNSensCilindros - 1; i++) {
			switch(FSensorCil[i].tipo) {
			case nmTempCil:
				FSensorCil[i].valreal = Engine[0]->GetCilindro(FSensorCil[i].cilindro - 1)->getTemperature();
				FSensorCil[i].valact = ((2 * FSensorCil[i].ctetiempo - deltaT) * FSensorCil[i].valant + deltaT * FSensorCil[i].ganancia
										* (FSensorCil[i].valreal + FSensorCil[i].valrealant)) / (2 * FSensorCil[i].ctetiempo + deltaT);
				FSensorCil[i].valant = FSensorCil[i].valact;
				FSensorCil[i].valrealant = FSensorCil[i].valreal;
				break;
			case nmPrCil:
				FSensorCil[i].valreal = Engine[0]->GetCilindro(FSensorCil[i].cilindro - 1)->getPressure();
				FSensorCil[i].valact = ((2 * FSensorCil[i].ctetiempo - deltaT) * FSensorCil[i].valant + deltaT * FSensorCil[i].ganancia
										* (FSensorCil[i].valreal + FSensorCil[i].valrealant)) / (2 * FSensorCil[i].ctetiempo + deltaT);
				FSensorCil[i].valant = FSensorCil[i].valact;
				FSensorCil[i].valrealant = FSensorCil[i].valreal;
				break;
			case nmMfCil:
				FSensorCil[i].valreal = Engine[0]->GetCilindro(FSensorCil[i].cilindro - 1)->getMasaFuel();
				FSensorCil[i].valact = ((2 * FSensorCil[i].ctetiempo - deltaT) * FSensorCil[i].valant + deltaT * FSensorCil[i].ganancia
										* (FSensorCil[i].valreal + FSensorCil[i].valrealant)) / (2 * FSensorCil[i].ctetiempo + deltaT);
				FSensorCil[i].valant = FSensorCil[i].valact;
				FSensorCil[i].valrealant = FSensorCil[i].valreal;
				break;
			case nmAFRCil:
				FSensorCil[i].valreal = Engine[0]->GetCilindro(FSensorCil[i].cilindro - 1)->getAFR();
				FSensorCil[i].valact = ((2 * FSensorCil[i].ctetiempo - deltaT) * FSensorCil[i].valant + deltaT * FSensorCil[i].ganancia
										* (FSensorCil[i].valreal + FSensorCil[i].valrealant)) / (2 * FSensorCil[i].ctetiempo + deltaT);
				FSensorCil[i].valant = FSensorCil[i].valact;
				FSensorCil[i].valrealant = FSensorCil[i].valreal;
				break;

			}/*fin del switch*/
		}/*fin del for*/
	} catch(exception &N) {
		std::cout << "ERROR: Calculo_Sensores_Cilindro" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void TCalculoExtern::Calculo_Sensores_Venturi(double deltaT, TVenturi **Venturi) {
	try {
		int signo = 0;
		double prent = 0., prgar = 0., velent = 0., vellat = 0., gasent = 0., gaslat = 0.;

		for(int i = 0; i <= FNSensVenturis - 1; i++) {
			switch(FSensorVent[i].tipo) {
			case nmPrEntVent:
				prent = Venturi[FSensorVent[i].venturi - 1]->getPressure();
				FSensorVent[i].valreal = prent;
				FSensorVent[i].valact = ((2 * FSensorVent[i].ctetiempo - deltaT) * FSensorVent[i].valant + deltaT *
										 FSensorVent[i].ganancia * (FSensorVent[i].valreal + FSensorVent[i].valrealant)) / (2 * FSensorVent[i].ctetiempo +
												 deltaT);
				FSensorVent[i].valant = FSensorVent[i].valact;
				FSensorVent[i].valrealant = FSensorVent[i].valreal;
				break;
			case nmPrGarVent:
				prent = Venturi[FSensorVent[i].venturi - 1]->getPressure();
				prgar = prent / pow(
							1 + Venturi[FSensorVent[i].venturi - 1]->getgamma3() * pow(dynamic_cast<TCCDeposito *>
									(Venturi[FSensorVent[i].venturi - 1]->getCCLateral())->getValvula()->getCRecuperacion(), 2.),
							Venturi[FSensorVent[i].venturi - 1]->getGamma() / Venturi[FSensorVent[i].venturi - 1]->getgamma1());
				FSensorVent[i].valreal = prgar;
				FSensorVent[i].valact = ((2 * FSensorVent[i].ctetiempo - deltaT) * FSensorVent[i].valant + deltaT *
										 FSensorVent[i].ganancia * (FSensorVent[i].valreal + FSensorVent[i].valrealant)) / (2 * FSensorVent[i].ctetiempo +
												 deltaT);
				FSensorVent[i].valant = FSensorVent[i].valact;
				FSensorVent[i].valrealant = FSensorVent[i].valreal;
				break;
			case nmVelEntVent:
				if(dynamic_cast<TCCDeposito *>(Venturi[FSensorVent[i].venturi - 1]->getCCEntrada())->getSentidoFlujo() == nmEntrante) {
					signo = 1; /* Si el flujo es entrante signo de la velocidad positivo */
				} else if(dynamic_cast<TCCDeposito *>(Venturi[FSensorVent[i].venturi - 1]->getCCEntrada())->getSentidoFlujo() ==
						  nmSaliente) {
					signo = -1; /* Si el flujo es saliente signo de la velocidad negativo */
				}
				velent = dynamic_cast<TCCDeposito *>(Venturi[FSensorVent[i].venturi - 1]->getCCEntrada())->getVelocity() * signo *
						 __cons::ARef;
				FSensorVent[i].valreal = velent;
				FSensorVent[i].valact = ((2 * FSensorVent[i].ctetiempo - deltaT) * FSensorVent[i].valant + deltaT *
										 FSensorVent[i].ganancia * (FSensorVent[i].valreal + FSensorVent[i].valrealant)) / (2 * FSensorVent[i].ctetiempo +
												 deltaT);
				FSensorVent[i].valant = FSensorVent[i].valact;
				FSensorVent[i].valrealant = FSensorVent[i].valreal;
				break;
			case nmVelLatVent:
				vellat = dynamic_cast<TCCDeposito *>(Venturi[FSensorVent[i].venturi - 1]->getCCLateral())->getVelocity() * __cons::ARef;
				FSensorVent[i].valreal = vellat;
				FSensorVent[i].valact = ((2 * FSensorVent[i].ctetiempo - deltaT) * FSensorVent[i].valant + deltaT *
										 FSensorVent[i].ganancia * (FSensorVent[i].valreal + FSensorVent[i].valrealant)) / (2 * FSensorVent[i].ctetiempo +
												 deltaT);
				FSensorVent[i].valant = FSensorVent[i].valact;
				FSensorVent[i].valrealant = FSensorVent[i].valreal;
				break;
			case nmGastoEntVent:
				gasent = -dynamic_cast<TCCDeposito *>(Venturi[FSensorVent[i].venturi -
													  1]->getCCEntrada())->getMassflow(); /*Massflow entrante positivo.Saliente negativo. Pedro */
				/* gasent lleva el "-" delante porque se usa el convenio de signos de los depositos,que es el contrario al de las BC, de la que se obtiene el massflow */
				FSensorVent[i].valreal = gasent;
				FSensorVent[i].valact = ((2 * FSensorVent[i].ctetiempo - deltaT) * FSensorVent[i].valant + deltaT *
										 FSensorVent[i].ganancia * (FSensorVent[i].valreal + FSensorVent[i].valrealant)) / (2 * FSensorVent[i].ctetiempo +
												 deltaT);
				FSensorVent[i].valant = FSensorVent[i].valact;
				FSensorVent[i].valrealant = FSensorVent[i].valreal;
				break;
			case nmGastoLatVent:
				gaslat = dynamic_cast<TCCDeposito *>(Venturi[FSensorVent[i].venturi - 1]->getCCLateral())->getMassflow();
				FSensorVent[i].valreal = gaslat;
				FSensorVent[i].valact = ((2 * FSensorVent[i].ctetiempo - deltaT) * FSensorVent[i].valant + deltaT *
										 FSensorVent[i].ganancia * (FSensorVent[i].valreal + FSensorVent[i].valrealant)) / (2 * FSensorVent[i].ctetiempo +
												 deltaT);
				FSensorVent[i].valant = FSensorVent[i].valact;
				FSensorVent[i].valrealant = FSensorVent[i].valreal;
				break;

			}/*fin del switch*/
		}/*fin del for*/
	} catch(exception &N) {
		std::cout << "ERROR: Calculo_Sensores_Venturi" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void TCalculoExtern::Calculo_Sensores_Motor(double deltaT, TBloqueMotor **Engine, double AcumulatedTime) {
	try {
		for(int i = 0; i <= FNSensMotor - 1; i++) {
			switch(FSensorMotor[i].tipo) {
			case nmAngulo:
				FSensorMotor[i].valreal = Engine[0]->getTheta();
				FSensorMotor[i].valact = ((2 * FSensorMotor[i].ctetiempo - deltaT) * FSensorMotor[i].valant + deltaT *
										  FSensorMotor[i].ganancia * (FSensorMotor[i].valreal + FSensorMotor[i].valrealant)) /
										 (2 * FSensorMotor[i].ctetiempo + deltaT);
				FSensorMotor[i].valant = FSensorMotor[i].valact;
				FSensorMotor[i].valrealant = FSensorMotor[i].valreal;
				break;
			case nmReg:
				FSensorMotor[i].valreal = Engine[0]->getRegimen();
				FSensorMotor[i].valact = ((2 * FSensorMotor[i].ctetiempo - deltaT) * FSensorMotor[i].valant + deltaT *
										  FSensorMotor[i].ganancia * (FSensorMotor[i].valreal + FSensorMotor[i].valrealant)) /
										 (2 * FSensorMotor[i].ctetiempo + deltaT);
				FSensorMotor[i].valant = FSensorMotor[i].valact;
				FSensorMotor[i].valrealant = FSensorMotor[i].valreal;
				break;
			case nmTiempo:
				FSensorMotor[i].valreal = AcumulatedTime;
				FSensorMotor[i].valact = ((2 * FSensorMotor[i].ctetiempo - deltaT) * FSensorMotor[i].valant + deltaT *
										  FSensorMotor[i].ganancia * (FSensorMotor[i].valreal + FSensorMotor[i].valrealant)) /
										 (2 * FSensorMotor[i].ctetiempo + deltaT);
				FSensorMotor[i].valant = FSensorMotor[i].valact;
				FSensorMotor[i].valrealant = FSensorMotor[i].valreal;
				break;

			}/*fin del switch*/
		}/*fin del for*/
	} catch(exception &N) {
		std::cout << "ERROR: Calculo_Sensores_Cilindro" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void TCalculoExtern::Calculo_Sensores_UED(double deltaT, TCondicionContorno **BC) {
	try {
		for(int i = 0; i <= FNSensUED - 1; i++) {
			switch(FSensorUED[i].tipo) {
			case nmGasto:
				FSensorUED[i].valreal = dynamic_cast<TCCUnionEntreDepositos*>(BC[FSensorUED[i].CCUED - 1])->getGastoImpreso();
				FSensorUED[i].valact = ((2 * FSensorUED[i].ctetiempo - deltaT) * FSensorUED[i].valant + deltaT * FSensorUED[i].ganancia
										* (FSensorUED[i].valreal + FSensorUED[i].valrealant)) / (2 * FSensorUED[i].ctetiempo + deltaT);
				FSensorUED[i].valant = FSensorUED[i].valact;
				FSensorUED[i].valrealant = FSensorUED[i].valreal;
				break;
			}/*fin del switch*/
		}/*fin del for*/
	} catch(exception &N) {
		std::cout << "ERROR: Calculo_Sensores_UED" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

double TCalculoExtern::xit_(double vizq, double vder, double axid, double xif) {
	try {
		double xx = 0., yy = 0.;
		double ret_val = 0.;

		xx = vder - vizq;
		if(axid != 0.) {
			yy = xx / axid * xif;
			ret_val = vizq + yy;
		} else {
			printf("ERROR: valores entrada xit\n");
			throw Exception("");
		}
		return ret_val;
	} catch(exception &N) {
		std::cout << "ERROR: xit_" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TCalculoExtern::InicializaMedias() {
	try {

		//Inicializa variables en el primer instante de calculo.

		printf("Recuerda que debes inicializar las variables que quieres graficar\n");
		// VariableSUM=0.;
		PosicionTurbinaSUM = 0.;
		SensorGastoAdmisionSUM = 0.;
		SensorP2SUM = 0.;
		SensorP3SUM = 0.;
		SensorP4SUM = 0.;
		SensorRegimenTurboSUM = 0.;
	} catch(exception &N) {
		std::cout << "ERROR: InicializaMedias" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TCalculoExtern::AcumulaMedias(double DeltaT) {
	try {

	} catch(exception &N) {
		std::cout << "ERROR: AcumulaMedias: " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void TCalculoExtern::CalculaMedias() {
	try {
		// Variable1=VariableSUM/FTiempoSum;
//	 PosicionTurbina=PosicionTurbinaSUM/FTiempoSum;
//	 SensorGastoAdmision=SensorGastoAdmisionSUM/FTiempoSum;
//	 SensorP2=SensorP2SUM/FTiempoSum;
//	 SensorP3=SensorP3SUM/FTiempoSum;
//	 SensorP4=SensorP4SUM/FTiempoSum;
//	 SensorRegimenTurbo=SensorRegimenTurboSUM/FTiempoSum;
//	 SensorRelCinematica=SensorRelCinematicaSUM/FTiempoSum;
//	 PosicionTurbinaSUM=0.;
//	 SensorGastoAdmisionSUM=0.;
//	 SensorP2SUM=0.;
//	 SensorP3SUM=0.;
//	 SensorP4SUM=0.;
//	 SensorRegimenTurboSUM=0.;
//	 SensorRelCinematicaSUM=0.;
//	 FTiempoSum=0.;
		// VariableSUM=0.;

	} catch(exception &N) {
		std::cout << "ERROR: CalculaMedias: " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void TCalculoExtern::ImprimeCabeceraMedias(stringstream& medoutput) {
	try {
//	 fstream fich;
//	 fich.open(FileMed,ios::out | ios::app);
//
//	/* for(int i=0;i<Fngrafmat;i++){
//		  //fich << '\t' << "Salida_Media_EXTERN_" << i;
//
//	 } */
//	 fich << '\t' << "Posicion_Turbina_[%]";
//	 fich << '\t' << "Sensor_Gasto_Admision_[kg/s]";
//	 fich << '\t' << "Sensor_P2_[bar]";
//	 fich << '\t' << "Sensor_P3_[bar]";
//	 fich << '\t' << "Sensor_P4_[bar]";
//	 fich << '\t' << "Regimen_Turbo_[rpm]";
//	 fich << '\t' << "Sensor_Rel_Cin";
//	 fich.close();
	} catch(exception &N) {
		std::cout << "ERROR: ImprimeCabeceraMedias: " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void TCalculoExtern::ImprimeGraficosMedias(stringstream& medoutput) {
	try {
//     fstream fich;
//	 fich.open(FileMed,ios::out | ios::app);
//
//	 //fich << '\t' << Variable;
//	 fich << '\t' << PosicionTurbina;
//	 fich << '\t' << SensorGastoAdmision;
//	 fich << '\t' << SensorP2;
//	 fich << '\t' << SensorP3;
//	 fich << '\t' << SensorP4;
//	 fich << '\t' << SensorRegimenTurbo;
//	 fich << '\t' << SensorRelCinematica;
//	 fich.close();

	} catch(exception &N) {
		std::cout << "ERROR: ImprimeGraficosMedias: " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void TCalculoExtern::ImprimeCabeceraInstantaneas(stringstream& insoutput) {
	try {
		//fstream fich;
		//fich.open(FileIns,ios::out | ios::app);

		//for(int i=0;i<Fngrafmat;i++){
		//     fich << '\t' << "Salida_Instantanea_EXTERN_" << i;
		//}
		//fich.close();
	} catch(exception &N) {
		std::cout << "ERROR: ImprimeCabeceraInstantaneas: " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void TCalculoExtern::ImprimeGraficosInstantaneas(stringstream& insoutput) {
	try {
		//fstream fich;
		//fich.open(FileIns,ios::out | ios::app);

		// fich << '\t' << Variable;

		//fich.close();
	} catch(exception &N) {
		std::cout << "ERROR: ImprimeGraficosInstantaneas: " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void TCalculoExtern::ObtenerRutaTrabajo(const char *origin) {
	try {
		int contpunto = 0;
		for(int i = (int) strlen(origin) - 1; i >= 0; i--) {
			if(origin[i] == '\\') {
				contpunto = i + 1;    //ha encontrado el punto de la extension
				break;
			}
		}
		FRutaTrabajo = new char[contpunto + 1];
		for(int i = 0; i < contpunto; i++) {
			FRutaTrabajo[i] = origin[i];
		}
		FRutaTrabajo[contpunto] = '\0';

	} catch(exception &N) {
		std::cout << "ERROR: ObtenerRutaTrabajo: " << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#pragma package(smart_init)

