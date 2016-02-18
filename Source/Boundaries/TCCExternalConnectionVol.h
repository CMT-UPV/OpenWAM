//---------------------------------------------------------------------------

#ifndef TCCExternalConnectionVolH
#define TCCExternalConnectionVolH
//---------------------------------------------------------------------------

#include "TCondicionContorno.h"

class TCCExternalConnectionVol: public TCondicionContorno {
  private:

	double FUExt;
	double FTExt;
	double FPExt;

	double FDExt;
	double FAExt;

	double FDeltaX;

	double FCurrentTime;

	double FA_AExt;
	double FK_CExt;

	double FP_Boundary;
	double FT_Boundary;
	double FU_Boundary;

	double FP_BoundarySum;
	double FT_BoundarySum;
	double FU_BoundarySum;

	double *FCC; // Caracteristica conocida del tubo.
	double *FCD; // Caracteristica desconocida del tubo.

	int FNodoFin;
	int FIndiceCC;

	int FID;

	double FTime0;
	double FTimeSum;

  protected:

  public:
	TCCExternalConnectionVol(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
							 nmCalculoGamma GammaCalculation, bool ThereIsEGR);

	~TCCExternalConnectionVol();

	void UpdateCurrentExternalProperties(double U0, double T0, double P0, double t);

	void AsignGeometricalData(double D0, double deltaX);

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
