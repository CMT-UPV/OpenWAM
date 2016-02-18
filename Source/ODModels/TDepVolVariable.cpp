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

#include "TDepVolVariable.h"

#include "TCCDeposito.h"
#include "TCCUnionEntreDepositos.h"
#include "TTubo.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TDepVolVariable::TDepVolVariable(int i, int ncv, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
								 nmCalculoGamma GammaCalculation, bool ThereIsEGR) :
	TDeposito(i, nmDepVolVble, SpeciesModel, numeroespecies, GammaCalculation, ThereIsEGR) {

	FNumeroCompresorVol = ncv + 1;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TDepVolVariable::~TDepVolVariable() {
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TDepVolVariable::LeeDatosDepVolVariable(const char *FileWAM, fpos_t &filepos, bool HayMotor) {
	try {

		int ControlRegimen = 0;

		FILE *fich = fopen(FileWAM, "r");
		fsetpos(fich, &filepos);

		fscanf(fich, "%lf %lf %lf ", &FLBiela, &FCarrera, &FDiametro);
		fscanf(fich, "%lf %lf %lf ", &FRelCompre, &FDesfase, &FDescentramiento);

		fscanf(fich, "%d ", &ControlRegimen);

		switch(ControlRegimen) {
		case 0:
			FControlRegimen = nmPropio;
			break;
		case 1:
			FControlRegimen = nmMotor;
			break;
		}
		if(FControlRegimen == nmPropio) {
			fscanf(fich, "%lf ", &FRegimen);
			FRelacionVelocidades = 1.;
		} else if(FControlRegimen == nmMotor && HayMotor) {
			fscanf(fich, "%lf ", &FRelacionVelocidades);
		} else {
			std::cout << "ERROR: TDepVolVariable::LeeDatosIniciales Lectura del Control del Regimen erronea " << std::endl;
			throw Exception(" ");
		}

		fgetpos(fich, &filepos);
		fclose(fich);

		FVolumenMuerto = __geom::Circle_area(FDiametro) * FCarrera / (FRelCompre - 1.);

	} catch(exception &N) {
		std::cout << "ERROR: TDepVolVariable::LeeDatosDepVolVariable en el compresor volumetrico: " << FNumeroCompresor <<
				  std::endl;
//std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TDepVolVariable::ActualizaPropiedades(double TimeCalculo) {

	double H = 0.; //Entalpia de entrada
	double Energia = 0.;
	double MasaEntrante, FraccionMasicaAcum = 0.;
	double DeltaT = 0., DeltaAngulo = 0.;
	double g = 0., v = 0., a = 0., m = 0.;
	int SignoFlujo = 1, SignoFlujoED = 1;

	try {
		FMasa0 = FMasa;
		MasaEntrante = 0.;
		H = 0.;
		DeltaT = TimeCalculo - FTime;
		DeltaAngulo = 360 * FRegimen / 60. * DeltaT;
		FAngulo += DeltaAngulo;
		FVolumen0 = FVolumen;
		FVolumen = CalculaVolumen(FAngulo, FCarrera, FLBiela, FDiametro, FVolumenMuerto);

		if(FCalculoEspecies == nmCalculoCompleto) {

			FRMezcla = CalculoCompletoRMezcla(FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2], 0,
											  FCalculoGamma, nmMEP);
			FCpMezcla = CalculoCompletoCpMezcla(FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FFraccionMasicaEspecie[2], 0,
												__units::degCToK(FTemperature), FCalculoGamma, nmMEP);
			FGamma = CalculoCompletoGamma(FRMezcla, FCpMezcla, FCalculoGamma);

		} else if(FCalculoEspecies == nmCalculoSimple) {

			FRMezcla = CalculoSimpleRMezcla(FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1], FCalculoGamma, nmMEP);
			FCvMezcla = CalculoSimpleCvMezcla(__units::degCToK(FTemperature), FFraccionMasicaEspecie[0], FFraccionMasicaEspecie[1],
											  FCalculoGamma, nmMEP);
			FGamma = CalculoSimpleGamma(FRMezcla, FCvMezcla, FCalculoGamma);

		}

		for(int i = 0; i < FNumeroUniones; i++) {
			if(dynamic_cast<TCCDeposito *>(FCCDeposito[i])->getSentidoFlujo() == nmEntrante) {
				SignoFlujo = 1;
			} else if(dynamic_cast<TCCDeposito *>(FCCDeposito[i])->getSentidoFlujo() == nmSaliente) {
				SignoFlujo = -1;
			}
			g = (double) - dynamic_cast<TCCDeposito *>(FCCDeposito[i])->getMassflow();
			m = g * DeltaT * FCCDeposito[i]->GetTuboExtremo(0).Pipe->getNumeroConductos();
			v = (double) SignoFlujo * dynamic_cast<TCCDeposito *>(FCCDeposito[i])->getVelocity();
			a = dynamic_cast<TCCDeposito *>(FCCDeposito[i])->getSpeedSound();
			MasaEntrante += m;
			if(v > 0) {
				H += EntalpiaEntrada(a, v, m, FAsonido, FMasa0, FCCDeposito[i]->getGamma());
			}
			for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
				FMasaEspecie[j] += FCCDeposito[i]->GetFraccionMasicaEspecie(j) * m;
			}
		}
		for(int i = 0; i < FNumeroUnionesED; i++) {

			if(FNumeroDeposito == dynamic_cast<TCCUnionEntreDepositos *>(FCCUnionEntreDep[i])->getNumeroDeposito1()) {
				SignoFlujoED = dynamic_cast<TCCUnionEntreDepositos *>(FCCUnionEntreDep[i])->getSentidoFlujoED1();
			} else if(FNumeroDeposito == dynamic_cast<TCCUnionEntreDepositos *>(FCCUnionEntreDep[i])->getNumeroDeposito2()) {
				SignoFlujoED = dynamic_cast<TCCUnionEntreDepositos *>(FCCUnionEntreDep[i])->getSentidoFlujoED2();
			} else
				printf("ERROR:TDepVolVariable::ActualizaPropiedades en el deposito %d, union entre depositos %d\n", FNumeroDeposito, i);

			g = (double) SignoFlujoED * dynamic_cast<TCCUnionEntreDepositos *>(FCCUnionEntreDep[i])->getMassflow();
			m = g * DeltaT;
			a = (double) dynamic_cast<TCCUnionEntreDepositos *>(FCCUnionEntreDep[i])->getSpeedSound();
			MasaEntrante += m;
			if(g > 0) {
				H += EntalpiaEntrada(a, 0, m, FAsonido, FMasa0, FCCUnionEntreDep[i]->getGamma());
			}
			for(int j = 0; j < FNumeroEspecies - FIntEGR; j++) {
				FMasaEspecie[j] += FCCUnionEntreDep[i]->GetFraccionMasicaEspecie(j) * m;
			}
		}
		FMasa = FMasa0 + MasaEntrante;
		for(int j = 0; j < FNumeroEspecies - 2; j++) {
			FFraccionMasicaEspecie[j] = FMasaEspecie[j] / FMasa;
			FraccionMasicaAcum += FFraccionMasicaEspecie[j];
		}
		FFraccionMasicaEspecie[FNumeroEspecies - 2] = 1. - FraccionMasicaAcum;
		if(FHayEGR)
			FFraccionMasicaEspecie[FNumeroEspecies - 1] = FMasaEspecie[FNumeroEspecies - 1] / FMasa;

		Energia = pow(FVolumen0 * FMasa / FMasa0 / FVolumen * exp(H), __Gamma::G1(FGamma));
		FAsonido *= sqrt(Energia);
		FPressure = __units::PaToBar(pow2(__cons::ARef * FAsonido) * FMasa / (FGamma * FVolumen));
		FPresionIsen = pow(FPressure / FPresRef, __Gamma::G5(FGamma));
		FTemperature = __units::KTodegC(pow2(FAsonido * __cons::ARef) / (FGamma * FRMezcla));
		FTime = TimeCalculo;
		if(FAngulo > 360.) {
			FAngulo -= 360.;
		}

	} catch(exception &N) {
		std::cout << "ERROR: TDepVolVariable::ActualizaPropiedades en el compresor volumetrico: " << FNumeroCompresorVol <<
				  std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double TDepVolVariable::CalculaVolumen(double CrankAngle, double carrera, double lbiela, double diametro,
									   double vol_muerto) {
	double c = 0., tt = 0., ttt = 0.;
	double ret_val = 0.;

	try {

		c = __units::DegToRad(CrankAngle);
		tt = pow2(lbiela);
		tt -= pow2(carrera * sin(c) / 2.);
		tt = sqrt(tt);
		ttt = lbiela + carrera * (1. - cos(c)) / 2. - tt;
		ret_val = ttt * __geom::Circle_area(diametro);
		ret_val += vol_muerto;
		return ret_val;

	} catch(exception &N) {
		std::cout << "ERROR: TDepVolVariable::CalculaVolumen en el compresor volumetrico: " << FNumeroCompresorVol << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TDepVolVariable::IniciaVolumen(double Theta) {
	try {
		FAngulo = Theta - FDesfase;
		FVolumen = CalculaVolumen(FAngulo, FCarrera, FLBiela, FDiametro, FVolumenMuerto);
		FMasa = FVolumen * FGamma * __units::BarToPa(FPressure) / pow2(FAsonido * __cons::ARef);
		FVolumen0 = FVolumen;
	} catch(exception &N) {
		std::cout << "ERROR: TDepVolVariable::IniciaVolumen en el compresor volumetrico: " << FNumeroCompresorVol << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

void TDepVolVariable::UpdateProperties0DModel(double TimeCalculo) {

	ActualizaPropiedades(TimeCalculo);

	AcumulaResultadosMedios(TimeCalculo);

}

void TDepVolVariable::UpdateSpeed(double NewSpeed) {
	FRegimen = NewSpeed * FRelacionVelocidades;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)

