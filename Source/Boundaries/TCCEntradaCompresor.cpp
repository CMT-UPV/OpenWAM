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

#include "TCCEntradaCompresor.h"
//#include <cmath>
#include <iostream>
#include "TCompresor.h"
#include "TTubo.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCCEntradaCompresor::TCCEntradaCompresor(nmTypeBC TipoCC,int numCC,nmTipoCalculoEspecies SpeciesModel,
                 int numeroespecies,nmCalculoGamma GammaCalculation,bool ThereIsEGR):
                 TCondicionContorno(TipoCC,numCC,SpeciesModel,numeroespecies,GammaCalculation,ThereIsEGR)
{

FTuboExtremo=NULL;
FVelocity=0;
FSonido=1;
FPressure=0;
FGasto=0;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TCCEntradaCompresor::~TCCEntradaCompresor()
{

delete[] FTuboExtremo;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCEntradaCompresor::ReadBoundaryData(char *FileWAM,fpos_t &filepos,int NumberOfPipes,TTubo **Pipe,int nDPF, TDPF **DPF)
{
try
{
int i=0;

FTuboExtremo=new stTuboExtremo[1];
FTuboExtremo[0].Pipe=NULL;

while(FNumeroTubosCC<1 && i<NumberOfPipes){
        if(Pipe[i]->getNodoIzq()==FNumeroCC){
           FTuboExtremo[FNumeroTubosCC].Pipe=Pipe[i];
           FTuboExtremo[FNumeroTubosCC].TipoExtremo=nmLeft;
           FNodoFin=0;
           FCC=&(FTuboExtremo[FNumeroTubosCC].Beta);
           FCD=&(FTuboExtremo[FNumeroTubosCC].Landa);
           FSeccionTubo=Pi*pow(Pipe[i]->GetDiametro(FNodoFin),2)/4;
           FPressure=Pipe[i]->GetPresion(FNodoFin);
           FNumeroTubosCC++;
        }
        if(Pipe[i]->getNodoDer()==FNumeroCC){
           FTuboExtremo[FNumeroTubosCC].Pipe=Pipe[i];
           FTuboExtremo[FNumeroTubosCC].TipoExtremo=nmRight;
           FNodoFin=Pipe[i]->getNin()-1;
           FCC=&(FTuboExtremo[FNumeroTubosCC].Landa);
           FCD=&(FTuboExtremo[FNumeroTubosCC].Beta);
           FSeccionTubo=Pi*pow(Pipe[i]->GetDiametro(FNodoFin),2)/4;
           FPressure=Pipe[i]->GetPresion(FNodoFin);
           FNumeroTubosCC++;
        }
        i++;
}

FILE *fich=fopen(FileWAM,"r");
fsetpos(fich, &filepos);

fscanf(fich,"%d ",&FNumeroCompresor);

fgetpos(fich, &filepos);
fclose(fich);

// Inicializaci�n del transporte de especies qu�micas.
FFraccionMasicaEspecie=new double[FNumeroEspecies-FIntEGR];
for(int i=0;i<FNumeroEspecies-FIntEGR;i++){
   FFraccionMasicaEspecie[i]=FTuboExtremo[0].Pipe->GetFraccionMasicaInicial(i);
}

}
catch(Exception &N)
{
std::cout << "ERROR: TCCEntradaCompresor::AsignaTubos en la condici�n de contorno: " << FNumeroCC <<  std::endl;
std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
throw Exception(N.Message.c_str());
}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCEntradaCompresor::AsignaCompresor(TCompresor **Compressor)
{
try
{

FCompresor=Compressor[FNumeroCompresor-1];

}
catch(Exception &N)
{
std::cout << "ERROR: TCCEntradaCompresor::AsignaCompresor en la condici�n de contorno: " << FNumeroCC <<  std::endl;
std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
throw Exception(N.Message.c_str());
}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TCCEntradaCompresor::CalculaCondicionContorno(double Time)
{
try
{
double cte1,cte2,ctea,numerador,denominador,presionact;
int contador=0;
double error=1e5;

FGamma=FTuboExtremo[0].Pipe->GetGamma(FNodoFin);
FGamma1=Gamma1(FGamma);
FGamma3=Gamma3(FGamma);
FGamma5=Gamma5(FGamma);

FGasto=FCompresor->getMassflow();
cte1=FTuboExtremo[0].Entropia;
cte2=FGamma1*pow(FTuboExtremo[0].Entropia,2)*FGasto*ARef/(2*FGamma*FSeccionTubo*1e5);
ctea=FGamma1/FGamma;

while(error>1e-5 && contador<800){
      contador++;
      numerador=cte1*pow(FPressure,FGamma5)+cte2*pow(FPressure,ctea-1)-*FCC;
      denominador=cte1*FGamma5*pow(FPressure,FGamma5-1)+cte2*(ctea-1)*pow(FPressure,ctea-2);
      presionact=FPressure-numerador/denominador;
      error=fabs(FPressure-presionact);
      FPressure=presionact;
}
if (contador > 799){
      printf("ERROR: TCCEntradaCompresor::CalculaCondicionContorno, no converge en entrada compresor en la condici�n de contorno: %d\n",FNumeroCC);
      throw Exception("ERROR: TCCEntradaCompresor::CalculaCondicionContorno no converge en entrada compresor");
}

// Calculo de velocidad,velocidad del sonido y caracter�stica reflejada.
FSonido=FTuboExtremo[0].Entropia*pow(FPressure,FGamma5);
FVelocity=(*FCC-FSonido)/FGamma3;
*FCD=FSonido-FGamma3*FVelocity;

// Seg�n este modelo de compresor,el flujo solo puede entrar en el compresor. As� que la composici�n
// ser� la determinada por la l�nea de corriente que llega del tubo. Esta ser� la composici�n del fluido
// en el compresor. Se actualiza directamente en el compresor.

}
catch(Exception &N)
{
std::cout << "ERROR: TCCEntradaCompresor::CalculaCondicionContorno en la condici�n de contorno: " << FNumeroCC <<  std::endl;
std::cout << "Tipo de error: " << N.Message.c_str() << std::endl;
throw Exception(N.Message.c_str());
}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)
