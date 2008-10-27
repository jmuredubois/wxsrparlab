#pragma once

#include <fftw3.h>  // included for type fftw_complex
#include <math.h>	// included for type complex
#include <complex>	// included for type complex


#ifndef MYIMGSZ
typedef struct IMGSIZE{ 
  long cx; 
  long cy; 
} IMGSIZE; 
#define MYIMGSZ
#endif

class CImgPadder
{
public:
	CImgPadder(){};	// constructor
	~CImgPadder(){};	// destructor

	//int imgPad(double* dbIn, long dbLen, double padFact);
	//int imgPad(fftw_complex* cxIn, long cxLen, double padFact);
	//int imgPad(fftw_complex* cxIn, long cxLin, long inCx, long inCy, fftw_complex* cxOut, long cxLout, long outCx, long outCy);
	int imgPad(fftw_complex* cxIn, long cxLin, IMGSIZE szIn, fftw_complex* cxOut, long cxLout, IMGSIZE szOut);
	int imgUnPad(fftw_complex* cxIn, long cxLin, IMGSIZE szIn, fftw_complex* cxOut, long cxLout, IMGSIZE szOut);
	int imgUnPad(double* dbIn, long cxLin, IMGSIZE szIn, double* dbOut, long cxLout, IMGSIZE szOut);

private:
	int gaussRamp(double* wgtIn, long wgLen, int nRamp);
	int linGaussRamp(double* wgtIn, long wgLen, int nRamp);
	int zeroRamp(double* wgtIn, long wgLen, int nRamp);
};