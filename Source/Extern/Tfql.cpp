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

#include "Tfql.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

Tfql::Tfql(int ncilin) {

	Fncilin = ncilin;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

Tfql::~Tfql() {
	if(Fmax != NULL)
		delete[] Fmax;

	if(Fpar_dist != NULL) {
		for(int i = 0; i < Fnparametros; ++i) {
			delete[] Fpar_dist[i];
		}
		delete[] Fpar_dist;
	}

	if(Flm != NULL) {
		for(int i = 0; i < Fnwiebe; ++i) {
			delete[] Flm[i];
		}
		delete[] Flm;
	}

	if(Flc != NULL) {
		for(int i = 0; i < Fnwiebe; ++i) {
			delete[] Flc[i];
		}
		delete[] Flc;
	}

	if(Flb != NULL) {
		for(int i = 0; i < Fnwiebe; ++i) {
			delete[] Flb[i];
		}
		delete[] Flb;
	}

	if(Fla != NULL) {
		for(int i = 0; i < Fnwiebe; ++i) {
			delete[] Fla[i];
		}
		delete[] Fla;
	}

	if(Fli != NULL) {
		for(int i = 0; i < Fnwiebe; ++i) {
			delete[] Fli[i];
		}
		delete[] Fli;
	}

	if(Flab != NULL) {
		for(int i = 0; i < Fnwiebe; ++i) {
			for(int j = 0; j < Fnley; ++j) {
				delete[] Flab[i][j];
			}
			delete[] Flab[i];
		}
		delete[] Flab;
	}

	if(Fcombustion != NULL)
		delete[] Fcombustion;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void Tfql::lee_leylib(char *Ruta, FILE *fich) {
	char *Filefql;
	char *Datosfql;

	try {

		for(int i = 0; i <= (int) strlen(Ruta); i++) {
			Datosfql[i] = Ruta[i];
		}

		fscanf(fich, "%s ", &Filefql);
		strcat(Datosfql, Filefql);

		Fichfql = fopen(Datosfql, "r");
		if((Fichfql = fopen(Datosfql, "r")) == NULL) {
			std::cout << "ERROR: Fichero de leyes de liberacion de calor no cargado";
		} else {
			fscanf(Fichfql, "%d ", &Fnley);
			fscanf(Fichfql, "%d ", &Fnwiebe);
			fscanf(Fichfql, "%d ", &Fnparametros);
			Fpar_dist = new double *[Fnparametros];
			for(int i = 0; i <= Fnparametros - 1; ++i) {
				Fpar_dist[i] = new double[Fnley];
			}
			Fmax = new double[Fnparametros];
			for(int k = 0; k <= Fnparametros - 1; k++) {
				Fmax[k] = 0;
			}
			Flm = new double*[Fnwiebe];
			for(int i = 0; i <= Fnwiebe - 1; ++i) {
				Flm[i] = new double[Fnley];
			}
			Flc = new double*[Fnwiebe];
			for(int i = 0; i <= Fnwiebe - 1; ++i) {
				Flc[i] = new double[Fnley];
			}
			Flb = new double*[Fnwiebe];
			for(int i = 0; i <= Fnwiebe - 1; ++i) {
				Flb[i] = new double[Fnley];
			}
			Fli = new double*[Fnwiebe];
			for(int i = 0; i <= Fnwiebe - 1; ++i) {
				Fli[i] = new double[Fnley];
			}
			Fla = new double*[Fnwiebe];
			for(int i = 0; i <= Fnwiebe - 1; ++i) {
				Fla[i] = new double[Fnley];
			}

			Flab = new double **[Fnwiebe];
			for(int i = 0; i <= Fnwiebe - 1; ++i) {
				Flab[i] = new double *[Fnley];
				for(int j = 0; j <= Fnley - 1; ++j) {
					Flab[i][j] = new double[Fncilin];
				}
			}

			Fcombustion = new bool[Fncilin];
			for(int i = 0; i <= Fncilin - 1; ++i) {
				Fcombustion[i] = false;
			}

			printf("INFO: N. de leyes: %d\n", Fnley);
			printf("INFO: N. de Wiebes: %d\n", Fnwiebe);

			for(int i = 0; i < Fnley; ++i) {
				for(int j = 0; j <= Fnparametros - 1; ++j) {
					fscanf(Fichfql, "%lf ", &Fpar_dist[j][i]);
					if(Fmax[j] < fabs(Fpar_dist[j][i])) {
						Fmax[j] = fabs(Fpar_dist[j][i]);
					}
				}
				for(int j = 0; j <= Fnwiebe - 1; ++j) {
					fscanf(Fichfql, "%lf %lf %lf %lf %lf ", &Flm[j][i], &Flc[j][i], &Flb[j][i], &Fli[j][i], &Fla[j][i]);
					Fla[j][i] = 720. + Fla[j][i];
				}

			}
		}
		fclose(Fichfql);
	} catch(exception &N) {
		std::cout << "ERROR: lee_leylib (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double Tfql::fql(double x, int j, int i) {

	try {

		double ret_val, wiebe = 0., c;

		ret_val = 0.;

		if(x > 720) {
			c = x / 720;
			x = x - 720 * floor(c);
		}

		/* Depende del orden de encendido */
		/* Para un cuatro cilindros dW10b*/
		//if(i==0)  x=x;
		if(i == 1)
			x = x - 540;
		if(i == 2)
			x = x - 180;
		if(i == 3)
			x = x - 360;

		/*     Para un seis cilindros VOLVO de Vicente
		 if(i==0)  x=x;
		 if(i==1)  x=x-480;
		 if(i==2)  x=x-240;
		 if(i==3)  x=x-600;
		 if(i==4)  x=x-120;
		 if(i==5)  x=x-360;
		 */
		while(x < 180)
			x = x + 720;
		while(x > 900)
			x = x - 720;

		if(x > Fang0 + 719.75 && x < Fang0 + 720.25)
			Fcombustion[i] = true;

		if(Fcombustion[i]) {
			for(int k = 0; k <= Fnwiebe - 1; ++k) {
				wiebe = fun_wiebe(x, Flm[k][j], Flc[k][j], Fli[k][j], Flab[k][j][i]);
				ret_val += wiebe * Flb[k][j];
			}
		} else {
			ret_val = 0.;
		}
		return ret_val;

	}

	catch(exception &N) {
		std::cout << "ERROR: fql (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double Tfql::fun_wiebe(double x, double m, double c, double ia, double a0)

{

	try {

		double ret_val = 0., xx = 0., xxx = 0.;

		if(x <= a0) {
			ret_val = 0;
		} else {

			xx = (x - a0) / ia;
			xxx = pow(xx, m + 1);
			if((xxx * c) > 10.) {  /*  Josevi */
				ret_val = 1.;
			} else {
				ret_val = 1. - 1. / exp(xxx * c);
			}

		}
		return ret_val;
	}

	catch(exception &N) {
		std::cout << "ERROR: fun_wiebe (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void Tfql::calcula_angulos_combustion(double *parametros, int i) {

	try {
		int k = 0;
		double up, down, ang01 = 0., tras = 0., a = 0., b = 0., c = 0., r = 0., s = 0., d = 0., densidad = 0., egr = 0., e;
		double dist = 0.;

		up = 0.;
		down = 0.;

		if(Fnwiebe == 4) {
			k = 1;
		} else {
			k = 0;
		}

		for(int j = 0; j < Fnley; ++j) {
			b = 0;
			/* for(int i=0;i<=Fnparametros-1;++i){
			 a=pow((parametros[i]-Fpar_dist[i][j])/Fmax[i],2.);
			 b+=a;
			 }  */

			//c=pow((parametros[1]-Fpar_dist[1][j])/Fmax[1],2.);
			r = pow2((parametros[2] - Fpar_dist[2][j]) / Fmax[2]); // Regimen
			s = pow2((parametros[5] - Fpar_dist[5][j]) / Fmax[5]); // SOI
			d = pow2((parametros[6] - Fpar_dist[6][j]) / Fmax[6]); // Dosado absoluto
			egr = pow2((parametros[7] - Fpar_dist[7][j]) / Fmax[7]); // Massflow de EGR
			densidad = pow2((parametros[8] - Fpar_dist[8][j]) / Fmax[8]); // Density en 2
			b = c + r + s + d + egr + densidad;

			dist = sqrt(b);
			if(dist < 1e-15)
				dist = 1e-15;
			e = 2 + 1 / dist;
			if(b < 0.008) {
				dist = 1e-15;
			} else {
				dist = pow(b, e);
			}
			if(dist < 1e-15)
				dist = 1e-15;
			up += (Fla[k][j] / dist);
			down += (1 / dist);
		}
		ang01 = up / down;

		Fang0 = ang01 - 720.;
		Ffinc = ang01 - 720.;

		for(int j = 0; j < Fnley; ++j) {
			tras = ang01 - Fla[k][j];
			for(int n = 0; n <= Fnwiebe - 1; ++n) {
				Flab[n][j][i] = Fla[n][j] + tras;
			}
			if(Flab[0][j][i] - 720. < Fang0) {
				Fang0 = Flab[0][j][i] - 720.;   // ??????????  anado -720
			}
			if((Flab[Fnwiebe - 1][j][i] + Fli[Fnwiebe - 1][j] - 720.) > Ffinc) {
				Ffinc = Flab[Fnwiebe - 1][j][i] + Fli[Fnwiebe - 1][j] - 720.;
			}
		}
	}

	catch(exception &N) {
		std::cout << "ERROR: fql (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double Tfql::calcula_fql(double *parametros, double x, int i) {

	try {
		double up, down, dist = 0., ret_val = 0., a = 0., b = 0., ley = 0., e;
		int j = 0;

		up = 0.;
		down = 0.;

		for(j = 0; j < Fnley; ++j) {
			b = 0;
			for(int k = 0; k < Fnparametros - 4; ++k) {
				a = pow2((parametros[k] - Fpar_dist[k][j]) / Fmax[k]);
				b += a;
			}

			dist = sqrt(b);
			e = 2 + 1 / dist;
			dist = pow(b, e);
			ley = fql(x, j, i);
			if(ley > 1.)
				printf("WARNING: Ley n. %d > 1. en %lf\n", j, x);
			up += ley / dist;
			down += 1. / dist;

		}

		ret_val = up / down;

		return ret_val;

	}

	catch(exception &N) {
		std::cout << "ERROR: calcula_fql (DLL)" << std::endl;
		std::cout << "Tipo de error: " << N.what() << std::endl;
		throw Exception(N.what());
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void Tfql::lee_leylib2(FILE *BaseDatos) {

	fscanf(BaseDatos, "%d ", &Fnley);
	fscanf(BaseDatos, "%d ", &Fnwiebe);
	fscanf(BaseDatos, "%d ", &Fnparametros);
	Fpar_dist = new double *[Fnparametros];
	for(int i = 0; i <= Fnparametros - 1; ++i) {
		Fpar_dist[i] = new double[Fnley];
	}
	Fmax = new double[Fnparametros];
	for(int k = 0; k <= Fnparametros - 1; k++) {
		Fmax[k] = 0;
	}
	Flm = new double*[Fnwiebe];
	for(int i = 0; i <= Fnwiebe - 1; ++i) {
		Flm[i] = new double[Fnley];
	}
	Flc = new double*[Fnwiebe];
	for(int i = 0; i <= Fnwiebe - 1; ++i) {
		Flc[i] = new double[Fnley];
	}
	Flb = new double*[Fnwiebe];
	for(int i = 0; i <= Fnwiebe - 1; ++i) {
		Flb[i] = new double[Fnley];
	}
	Fli = new double*[Fnwiebe];
	for(int i = 0; i <= Fnwiebe - 1; ++i) {
		Fli[i] = new double[Fnley];
	}
	Fla = new double*[Fnwiebe];
	for(int i = 0; i <= Fnwiebe - 1; ++i) {
		Fla[i] = new double[Fnley];
	}
	Flab = new double **[Fnwiebe];
	for(int i = 0; i <= Fnwiebe - 1; ++i) {
		Flab[i] = new double *[Fnley];
		for(int j = 0; j <= Fnley - 1; ++j) {
			Flab[i][j] = new double[Fncilin];
		}
	}

	printf("INFO: N. de leyes: %d\n", Fnley);
	printf("INFO: N. de Wiebes: %d\n", Fnwiebe);

	for(int i = 0; i < Fnley; ++i) {
		for(int j = 0; j <= Fnparametros - 1; ++j) {
			fscanf(BaseDatos, "%lf ", &Fpar_dist[j][i]);
			if(Fmax[j] < fabs(Fpar_dist[j][i])) {
				Fmax[j] = fabs(Fpar_dist[j][i]);
			}
		}
		for(int j = 0; j <= Fnwiebe - 1; ++j) {
			fscanf(BaseDatos, "%lf %lf %lf %lf %lf ", &Flm[j][i], &Flc[j][i], &Flb[j][i], &Fli[j][i], &Fla[j][i]);
			Fla[j][i] = 720. + Fla[j][i];
		}

	}

}

#pragma package(smart_init)

