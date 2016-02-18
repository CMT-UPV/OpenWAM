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

//---------------------------------------------------------------------------
#pragma hdrstop

#include "TCGestorWAM.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------
__fastcall TCGestorWAM::TCGestorWAM() {
}

void TCGestorWAM::Init() {
	BOOL fSuccess;
	//std::string lpszPipeName;
	string lpszPipeName;
	LPTSTR lpszPipeNameResp;
	DWORD dwMode;
	stringstream process_id(stringstream::in | stringstream::out);

	process_id << (float) GetCurrentProcessId();

	//lpszPipeName = "\\\\.\\pipe\\pipe" + FloatToStr((float)GetCurrentProcessId());

	lpszPipeName = "\\\\.\\pipe\\pipe" + process_id.str();

	//lpszPipeName = TEXT("\\\\.\\pipe\\pipe");

	lpszPipeNameResp = TEXT("\\\\.\\pipe\\pipeResp");

	//Espera hasta que se puede conectar a la tuberia

	//Sleep(5000); //Espero 5 segundos

	long num_pasadas = 0;

	while(num_pasadas <= 500000) {
		hPipe = CreateFile(lpszPipeName.c_str(), GENERIC_WRITE, 0, NULL,
						   OPEN_EXISTING, 0, NULL);

		if(hPipe != INVALID_HANDLE_VALUE)
			break;

		num_pasadas++;
	}

	//Si se ha llegado al final y no se ha conectado no
	//hacer nada mas
	if(num_pasadas <= 500000) {
		if(GetLastError() != 0)
			throw "Error al abrir tuberia";

		dwMode = PIPE_READMODE_MESSAGE;

		fSuccess = SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL);

		if(!fSuccess)
			throw "No se puede cambiar de modo la tuberia";
	} else {
		//Coloco hPipe en NULL para no hacer nada al enviar datos
		hPipe = NULL;
	}

	/*while (1)
	 {
	 hPipeResp = CreateFile(lpszPipeNameResp, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

	 if (hPipeResp != INVALID_HANDLE_VALUE)
	 break;
	 }

	 if (GetLastError()!=0) throw "Error al abrir tuberia respuesta";
	 */
}

void TCGestorWAM::Enviar(char *msg) {
	DWORD cbWritten;
	BOOL fSuccess;
	LPTSTR mensaje;

	if(hPipe != NULL) {
		mensaje = TEXT(msg);

		fSuccess = WriteFile(hPipe, mensaje, (strlen(mensaje) + 1) * sizeof(TCHAR), &cbWritten, NULL);

		if(!fSuccess)
			throw "No se puede escribir en tuberia";
	}

}

void TCGestorWAM::Enviar(float valor) {
	DWORD cbWritten;
	BOOL fSuccess;

	if(hPipe != NULL) {
		fSuccess = WriteFile(hPipe, &valor, sizeof(float), &cbWritten, NULL);

		if(!fSuccess)
			throw "No se puede escribir en tuberia";
	}
}

void TCGestorWAM::EsperarRespuesta() {
	DWORD cbRead;
	BOOL fSuccess;
	TCHAR chBuf[BUFSIZE];

	fSuccess = ReadFile(hPipeResp, chBuf, BUFSIZE * sizeof(TCHAR), &cbRead,
						NULL);
}

void TCGestorWAM::NuevoMensaje(char *msg) {
	Enviar("NUEVOMENSAJE");

	Enviar(msg);

	//EsperarRespuesta();
}

void TCGestorWAM::ProcesoTranscurrido(float valor) {
	Enviar("PROCESOTRANSCURRIDO");

	Enviar(valor);

	//EsperarRespuesta();
}

void TCGestorWAM::CabeceraResInstantActualizada() {
	Enviar("CABECERARESINSTANTACTUALIZADA");
	//EsperarRespuesta();
}

void TCGestorWAM::CabeceraResMediosActualizada() {
	Enviar("CABECERARESMEDIOSACTUALIZADA");
	//EsperarRespuesta();
}

void TCGestorWAM::FichResMediosActualizado() {
	Enviar("FICHRESMEDIOSACTUALIZADO");
	//EsperarRespuesta();
}

void TCGestorWAM::FichResInstantActualizado() {
	Enviar("FICHRESINSTANTACTUALIZADO");
	//EsperarRespuesta();
}

void TCGestorWAM::Terminar() {
	Enviar("TERMINAR");
}

