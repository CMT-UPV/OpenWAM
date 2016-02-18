// ---------------------------------------------------------------------------

#ifndef TCFDConnectionH
#define TCFDConnectionH

#include "TCondicionContorno.h"

#include <ctime>

class TCFDConnection: public TCondicionContorno {
  private:

	int FNodoFin; //<! Node of the pipe connected to the boundary.
	int FIndiceCC; //<! Vector position for the pipe boundaries (0 Left node; 1 Right node)

	double *FCC; //<! Known characteristic at the boundary.
	double *FCD; //<! Unknown characteristic at the boundary.

	char* FCFDModel; //<! CFD model name.
	char* FCFDout;
	char* FCFDin;

	dVector FSpecieCFD;

	int FCFDTiemStep; //<! Time step used by the cfd model.

	bool FirstTime;

	time_t FUpdateTime;

	bool FExistFile;

  protected:

  public:
	TCFDConnection(nmTypeBC TipoCC, int numCC, nmTipoCalculoEspecies SpeciesModel, int numeroespecies,
				   nmCalculoGamma GammaCalculation, bool ThereIsEGR);

	~TCFDConnection();

	void ReadBoundaryData(const char *FileWAM, fpos_t &filepos, int NumberOfPipes, TTubo **Pipe, int nDPF, TDPF **DPF);

	void CalculaCondicionContorno(double Time);

};
// ---------------------------------------------------------------------------
#endif
