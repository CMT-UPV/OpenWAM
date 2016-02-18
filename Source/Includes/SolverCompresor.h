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
#ifndef SolverCompresorH
#define SolverCompresorH
//---------------------------------------------------------------------------

//#include "roots.h"
#include "Constantes.h"

struct stCompSolverVOut {

	Doub Massflow;
	Doub SecOut;
	Doub TempTotOut;
	Doub PresTotOut;
	Doub CpMezcla;
	Doub RMezcla;
	Doub Gam;

	Doub RhoOut;
	Doub TempOut;
	Doub PresOut;
	Doub VOut;

	stCompSolverVOut(Doub gas, Doub sout, Doub ttout, Doub ptout, Doub cpm, Doub rm, Doub g) {
		Massflow = gas;
		SecOut = sout;
		TempTotOut = ttout;
		PresTotOut = ptout;
		CpMezcla = cpm;
		RMezcla = rm;
		Gam = g;
	}
	Doub operator()(const Doub AOut) {
//		RhoOut=abs(Massflow/SecOut * VOut);
//		TempOut=TempTotOut-pow(VOut,2)/2./CpMezcla;
//		PresOut=RhoOut*TempOut*RMezcla;
//		return PresTotOut-PresOut*pow(TempTotOut/TempOut,Gam/(Gam-1));

		TempOut = AOut * AOut / Gam / RMezcla;
		VOut = sqrt((TempTotOut - TempOut) * 2 * CpMezcla);
		PresOut = PresTotOut / pow(TempTotOut / TempOut, Gam / (Gam - 1));
		return Massflow - PresOut / RMezcla / TempOut * VOut * SecOut;
	}
};

struct stCompSolverVIn {

	Doub CarIn;
	Doub CarOut;
	Doub AaIn;
	Doub Gam;
	Doub RMezcla;
	Doub CpMezcla;
	Doub SecIn;
	Doub SecOut;
	Doub RelComp;
	Doub CoefPres;
	Doub VOut;
	Int_ Sig;
	Int_ SentFlow;
	Bool Direct;

	Doub Gam1;
	Doub Gam4;
	Doub PresIn;
	Doub TempIn;
	Doub TempOut;
	Doub RhoIn;
	Doub VIn;
	Doub AOut;
	Doub TempTotIn;
	Doub TempTotInAnt;
	Doub TempTotOut;
	Doub PresTotIn;
	Doub PresTotOut;
	Doub Massflow;

	stCompSolverVIn(Doub cari, Doub caro, Doub aai, Doub g, Doub rm, Doub cpm, Doub sin, Doub sout, Doub rc, Doub cpre,
					Int_ s, Int_ sf, Doub ttinant, Doub ttoutant, Bool dir, Bool firt) {
		CarIn = cari;
		CarOut = caro;
		AaIn = aai;
		Sig = s;
		Gam = g;
		Gam1 = Gam - 1;
		Gam4 = 2 * Gam / Gam1;
		RMezcla = rm;
		CpMezcla = cpm;
		SentFlow = sf;
		RelComp = rc;
		CoefPres = cpre;
		Direct = dir;
		TempTotInAnt = ttinant;
		SecIn = sin;
		SecOut = sout;
	}
	Doub operator()(const Doub AIn) {

//		AIn=__cons::ARef*CarIn-(double)Sig*0.5*Gam1*VIn;
//		PresIn=1e5*pow(AaIn/AIn,-Gam4);
//		TempIn=pow(AIn,2.)/Gam/RMezcla;
//		RhoIn=PresIn/(RMezcla*TempIn);
//		TempTotIn=TempIn+pow(VIn,2.)/2./CpMezcla;
//		PresTotIn=PresIn*pow(TempTotIn/TempIn,Gam/Gam1);
//
//		Massflow=SentFlow*SecIn*RhoIn*VIn;
//
//		PresTotOut=PresTotIn*pow(RelComp,SentFlow);
//
//		if(Direct){
//			TempTotOut=TempTotIn*(1+CoefPres);
//		}else{
//			TempTotOut=TempTotInAnt;
//		}
//		Doub VOutMin=0.;
//		Doub VOutMax=(double)Sig*2*__cons::ARef*CarOut/(3-Gam);
//
//		Doub VMax1=-sqrt(2*CpMezcla*TempTotOut);
//		if(VOutMax < VMax1){
//			VOutMax=VMax1;
//		}
//
//		VMax1=sqrt(2*RMezcla*Gam*TempTotOut/(Gam+1));
//		if(VOutMax < VMax1){
//			VOutMax=VMax1;
//		}

		PresIn = __units::BarToPa(pow(AaIn / AIn, -Gam4));
		TempIn = pow2(AIn) / Gam / RMezcla;
		VIn = 2 * (__cons::ARef * CarIn - AIn) / Gam1;
		RhoIn = PresIn / (RMezcla * TempIn);
		TempTotIn = TempIn + pow2(VIn) / 2. / CpMezcla;
		PresTotIn = PresIn * pow(TempTotIn / TempIn, Gam / Gam1);

		Massflow = SentFlow * SecIn * RhoIn * VIn;

		PresTotOut = PresTotIn * pow(RelComp, SentFlow);

		if(Direct) {
			TempTotOut = TempTotIn * (1 + CoefPres);
		} else {
			TempTotOut = TempTotInAnt;
		}

		Doub AOutMin = CarOut * __cons::ARef;
		Doub AOutMax = 2 * CarOut * __cons::ARef / (3 - Gam);

		Doub lim = sqrt(Gam * RMezcla * TempTotOut);
		if(AOutMax > lim)
			AOutMax = lim;
		if(AOutMin < lim) {

			stCompSolverVOut FunAout(Massflow, SecOut, TempTotOut, PresTotOut, CpMezcla, RMezcla, Gam);

			AOut = rtbis(FunAout, AOutMin, AOutMax, 1e-10);

			VOut = FunAout.VOut;
		} else {
			return 1.;
		}

		//AOut=sqrt(Gam * RMezcla*TempOut);
		return CarOut * __cons::ARef - (AOut - (double) Sig * VOut * Gam1 / 2) / __cons::ARef;

	}
};

#endif
