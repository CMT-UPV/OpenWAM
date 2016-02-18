// ---------------------------------------------------------------------------

#ifndef TCCExternalConnectionH
#define TCCExternalConnectionH
// ---------------------------------------------------------------------------

#include "TCondicionContorno.h"

class TCCExternalConnection: public TCondicionContorno {
  private:

	dVector FUExt;
	dVector FTExt;
	dVector FPExt;

	dVector FDExt;
	dVector FAExt;

	double FDeltaX;

	double FCurrentTime;

	double FA_AExt;
	double FK_CExt;

	double FP_Boundary;
	double FT_Boundary;
	double FU_Boundary;

	double *FCC; // Caracteristica conocida del tubo.
	double *FCD; // Caracteristica desconocida del tubo.

	int FNodoFin;
	int FIndiceCC;

	int FID;

  protected:

  public:
	TCCExternalConnection(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
						  nmCalculoGamma GammaCalculation, bool ThereIsEGR);

	~TCCExternalConnection();

	void UpdateCurrentExternalProperties(double U0, double U1, double T0, double T1, double P0, double P1, double t);

	void AsignGeometricalData(double D0, double D1, double deltaX);

	void ExternalCharacteristics(double Time);

	void CalculaCondicionContorno(double Time);

	void ReadBoundaryData(const char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe, int nDPF, TDPF **DPF);

	int GetID() {
		return FID;
	}
	;

	void LoadNewData(double* p, double* T, double* u);

};
#endif
