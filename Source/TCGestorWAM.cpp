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

#include "TCGestorWAM.h"

using namespace std;
//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------
TCGestorWAM::TCGestorWAM ()
{
fHayTuberia=true;
}

void TCGestorWAM::Init()
{
  BOOL fSuccess;
  LPTSTR lpszPipeName, lpszPipeNameResp;
  DWORD dwMode;

  lpszPipeName = TEXT(AnsiString("\\\\.\\pipe\\pipe" + AnsiString((float)GetCurrentProcessId())).c_str());

  //Espera hasta que se puede conectar a la tuberia

  //while (1)
  //{

     //Espero 5 segundos como maximo a que se cree la tuberia
     if (!WaitNamedPipe(lpszPipeName,5000))
     {
          //No se ha encontrado la tuberia en el tiempo designado
		 fHayTuberia=false;
          return;
     }
     else
     {
         hPipe = CreateFile(lpszPipeName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
         fHayTuberia=true;
     }

     hPipe = CreateFile(lpszPipeName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

     //if (hPipe != INVALID_HANDLE_VALUE)
     //   break;
  //}

  if (GetLastError()!=0){
	//throw "Error al abrir tuberia";
	//std::cout << "Error al abrir tuberia" << std::endl;
	fHayTuberia=false;
  }else{

	dwMode = PIPE_READMODE_MESSAGE;

	fSuccess = SetNamedPipeHandleState(hPipe,&dwMode,NULL,NULL);

	if (!fSuccess) throw "No se puede cambiar de modo la tuberia";
  }
/*
  while (1)
  {
     hPipeResp = CreateFile(lpszPipeNameResp, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

     if (hPipeResp != INVALID_HANDLE_VALUE)
        break;
  }

  if (GetLastError()!=0) throw "Error al abrir tuberia respuesta";
*/
}

void TCGestorWAM::Enviar(char *msg)
{
  DWORD cbWritten;
  BOOL  fSuccess;
  LPTSTR mensaje;

  try
  {
  mensaje = TEXT(msg);

  fSuccess = WriteFile(hPipe,mensaje,(strlen(mensaje)+1)*sizeof(TCHAR),&cbWritten,NULL);

  if (!fSuccess) throw "No se puede escribir en tuberia";

  }
  catch  (Exception &N)
  {
    printf("ERROR: Escribiendo en tuberia\n");
    throw Exception(N.Message);
  }
}

void TCGestorWAM::Enviar(float valor)
{
	if(fHayTuberia){
		DWORD cbWritten;
		BOOL  fSuccess;

		fSuccess = WriteFile(hPipe,&valor,sizeof(float),&cbWritten,NULL);

		if (!fSuccess) throw "No se puede escribir en tuberia";
    }
}
/*
void TCGestorWAM::EsperarRespuesta()
{
   DWORD cbRead;
   BOOL fSuccess;
   TCHAR chBuf[BUFSIZE];

   fSuccess = ReadFile(hPipeResp, chBuf, BUFSIZE*sizeof(TCHAR), &cbRead, NULL);
}
*/
void TCGestorWAM::NuevoMensaje(char *msg)
{
	if(fHayTuberia){
		Enviar("NUEVOMENSAJE");

		Enviar(msg);
	}

  //EsperarRespuesta();
}

void TCGestorWAM::ProcesoTranscurrido(float valor)
{
	if(fHayTuberia){
		Enviar("PROCESOTRANSCURRIDO");

		Enviar(valor);
	}

  //EsperarRespuesta();
}

void TCGestorWAM::CabeceraResInstantActualizada()
{
	if(fHayTuberia){
		Enviar("CABECERARESINSTANTACTUALIZADA");

		//EsperarRespuesta();
	}
}

void TCGestorWAM::CabeceraResMediosActualizada()
{
	if(fHayTuberia){
		Enviar("CABECERARESMEDIOSACTUALIZADA");

		//EsperarRespuesta();
    }
}

void TCGestorWAM::FichResMediosActualizado()
{
	if(fHayTuberia){
		Enviar("FICHRESMEDIOSACTUALIZADO");

		//EsperarRespuesta();
	}
}

void TCGestorWAM::FichResInstantActualizado()
{
	if(fHayTuberia){
		//Enviar("FICHRESINSTANTACTUALIZADO");

		//EsperarRespuesta();
	}
}

void TCGestorWAM::Terminar()
{
	if(fHayTuberia){
		Enviar("TERMINAR");

		//EsperarRespuesta();
    }
}


