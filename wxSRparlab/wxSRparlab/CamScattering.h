/*
 * CamScattering.h
 * Header of the scattering compensation methods for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2008.08.19
 */

#pragma once

#define USE_FFTW	// using FFTW dft transform for scattering compensation calculations
#define PAD_FFTW	// image padding for FFTW processing
#define PAD_ZEROS	// pad fft image with zeros -> THIS SHOULD BE THE DEFAULT
#define LARGE_PSF // enable processing with PSF larger than SR image size ; SHOULD BE SET

#include "wxSRparlab.h" //!< top-level header file
#include "wxSrApp.h"	//!< application header file
#include "CamFrame.h"	//!< main camera header file
#include "imgPadder.h"  //
#include "CamFlagNaN.h" //
#include "myConvolveHfloat.h" // convolutions
#include <math.h>
#include <complex> // for ANSI C99 complex numbers; WATCH OUT, this seems to be C++'s  complex<T>
#include <fftw3.h>		//!< FFTW used
#pragma comment(lib, "libfftw3-3")
#include "srBuf.h"

class CamFrame;

//! class for kernels used in scattering compensation
class SCkernel
{
public:
  enum Type{KT_GAUSSIAN}; //!< enum for type of kernel
  SCkernel();	//!< constructor
  SCkernel(Type type, int numCoeffsH, float sigmaH,
	                  int numCoeffsV, float sigmaV,
					  float weight):
						_type(type),
						_numCoeffsH(numCoeffsH), _sigmaH(sigmaH),
						_numCoeffsV(numCoeffsV), _sigmaV(sigmaV),
						_weight(weight){}
  Type  _type;			//!< type of kernel (only KT_GAUSSIAN is supported)
  int   _numCoeffsH;	//!< number of horizontal coefficients to compute
  float   _sigmaH;		//!< sigma for gaussian in horizontal direction
  int   _numCoeffsV;	//!< number of horizontal coefficients to compute
  float   _sigmaV;		//!< sigma for gaussian in horizontal direction
  float _weight;		//!< weight associated with kernel
};

/**
 * Camera frame class \n
 * This class: \n
 * - computes scattering compensation (s) \n
 */
class CamScattering: public wxFrame //!< Camera frame class
{
public:
	//! constructor
    CamScattering( CamFrame* camFrm);
	//! destructor
	~CamScattering();
	//! Compensate scattering method
	int Compensate(SRBUF scatBuf, NANBUF nanBuf);
	//! Add abiltity to set target once and for all
	int SetCompensationTarget(SRBUF scatBuf, NANBUF nanBuf);
	//! Compensate on previously defined target
	int CompensateAgain();
	double*   GetPsfD();		//!< Returns the PSF image (double)
	

private:
	CamFrame	*_camFrm; // parent main wnd
	SRBUF       _savBuf;
	/*unsigned short *_amp;
	unsigned short *_pha;
	int _nCols;
	int _nRows;
	int _bufferSizeInBytes;*/
    IMGSIZE _imgSz;						//!< size of original image
    int		_numScatGauss;	//!< number of Gaussian kernels
    float		_w[8];			//!< weights for kernels in scattering compensation
    std::list<SCkernel> _kernelList;	//!< pugxml kernel list

 #ifdef IPLENG
    IplImage*			_imgIpl[6];		//!< IPL images for convolution processing
    IplConvKernelFP*	_kernIplH[8];	//!< IPL horizontal convolution kernels
    IplConvKernelFP*	_kernIplV[8];	//!< IPL vertical convolution kernels
 #else
    myConvImg*	_imgMyKrn[6];	//!< images buffers for convolution processing
    myKernel1D*	_myKernsV[8];	//!< vertical kernels for convolution processing
    myKernel1D*	_myKernsH[8];	//!< horizontal kernels for convolution processing
  #ifdef NRJ_CONS
    float _wTot;
  #endif
 #endif // IPL_ENG
//#ifdef USE_FFTW
  int			_wPsf;					//!< width  of psf
  int			_hPsf;					//!< height of psf
  fftw_plan		_fftPlanForward;		//!< FFTW plan for FORWARD , IN-PLACE dft
  fftw_plan		_fftPlanBackward;		//!< FFTW plan for BACKWARD, IN-PLACE dft
  std::complex<double> *_cxBuf;			//!< buffer for complex img in FFTW pprocessing
  std::complex<double> *_cxFilt;		//!< buffer for complex filter in FFTW pprocessing
  std::complex<double> *_cxPar;		    //!< buffer for parity         in FFTW pprocessing
  double			   *_psf;			//!< buffer for PSF (real only)
//#ifdef PAD_FFTW
  int			_wPadPsf;					//!< width  of PADDED psf
  int			_hPadPsf;					//!< height of PADDED psf
  CImgPadder* _padder;					//!< a padder instance is needed
  double _padFact;							//!< padding factor
  IMGSIZE _imgPadSz;					//!< size of padded image
  fftw_plan		_fftPadPlanForward;		//!< FFTW plan for FORWARD , IN-PLACE dft
  fftw_plan		_fftPadPlanBackward;		//!< FFTW plan for BACKWARD, IN-PLACE dft
  std::complex<double> *_cxPadBuf;			//!< buffer for complex img in FFTW pprocessing
  std::complex<double> *_cxPadFilt;			//!< buffer for complex filter in FFTW pprocessing
  std::complex<double> *_cxPadPar;		    //!< buffer for parity         in FFTW pprocessing
//#endif
//#endif
#ifdef SCATCTIMER
  CPreciseTimer _scatCTimer;	//!< timer for scattering compensation
  float			_timeScatC_s;	//!< variable to hold Scat. Comp. time in seconds
#endif

public:
  int LoadScatSettings(const char* fn);	//!< load scattering correction parameters
private:
  int InitKernels();	//!< init kernels used for scattering compensation
  int CalcBufAlloc();	//!< allocate scattering correction image buffers
  int CalcFreeBufs();	//!< free scattering correction image buffers
  int CalcScatCorr(SRBUF scatBuf, NANBUF nanBuf);	//!< scattering compensation calculation
  int CalcScatDoConv(); //!< Launch the convolution operation
  int CalcScatDoSub();	//!< Do the subtraction needed in scat compensation
  int CalcSrBuf2Conv(SRBUF scatBuf); //!< convert SR buffer 2 convolution buffer
  int CalcConv2SrBuf(SRBUF scatBuf, NANBUF nanBuf); //!< convert convolution buffer to SR buffer
  int CalcScatSimul(SRBUF scatBuf, NANBUF nanBuf);	//!< scattering SIMULATION calculation
  int CalcScatDoAdd();	//!< Do the addition needed in scat SIMULATION
  int CalcBufAllocDft();	//!< allocate scattering correction image buffers  USING DFT
  int CalcBufAllocDft(int wpsf, int hpsf);	//!< allocate scattering correction image buffers  USING DFT
  int CalcFreeBufsDft();	//!< free scattering correction image buffers      USING DFT
  int CalcScatCorrDft(SRBUF scatBuf, NANBUF nanBuf);	//!< scattering compensation calculation           USING DFT
  int LoadScatPsf4Dft(const char* fn);	//!< load scattering psf model         USING DFT
  int UpdateScatPsf4Dft();	//!< update scattering psf model				   USING DFT
  int InitScatPsfFromKernel();	//!< initialize scattering psf model from XML description USING DFT
  int InitScatPsfFromLargeKernel();	//!< initialize LARGE scattering psf model from XML description USING DFT
  int CalcUnitImpulse2Conv();   //!< initialize a unit impulse image for psf computation
  int CalcScatDoLargeConv(); //!< Launch the LARGE convolution operation
  int CalcConv2LargePsf();  //!< put convolution result into LARGE psf buffer
  int CalcConv2Psf();       //!< put convolution result into psf buffer
  int CalcSrBuf2Complex(SRBUF scatBuf);	//!< put SR data into complex buffer			    USING DFT
  int CalcComplex2SrBuf(SRBUF scatBuf, NANBUF nanBuf);	//!< put complex data into SR buffer			    USING DFT
  int CalcSrBuf2RepComplex(SRBUF scatBuf, int repW, int repH);	//!< put replicated SR data into complex buffer USING DFT
  int CalcRepComplex2SrBuf(SRBUF scatBuf, NANBUF nanBuf, int repW, int repH);	//!< put replicates complex data into SR buffer USING DFT
  
  int CalcBufAllocPadDft();	//!< allocate scattering correction image buffers  USING PADDED DFT
  int CalcBufAllocPadDft(double padFact);	//!< allocate scattering correction image buffers  USING PADDED DFT
  int CalcBufAllocPadDft(int wpadpsf, int hpadpsf);	//!< allocate scattering correction image buffers  USING PADDED DFT
  int CalcFreeBufsPadDft();	//!< free scattering correction image buffers      USING PADDED DFT
  int CalcScatCorrPadDft(SRBUF scatBuf, NANBUF nanBuf);	//!< scattering compensation calculation           USING PADDED DFT
  int CalcScatSimulPadDft(SRBUF scatBuf, NANBUF nanBuf);	//!< scattering SIMULATION calculation         USING PADDED DFT
  int CalcBufReallocDft();		//!< reallocates psf buffers if needed 		   USING PADDED DFT
  int UpdateScatPsf4PadDft();	//!< update scattering psf model			   USING PADDED DFT
  int CalcSrBuf2ComplexPad(SRBUF scatBuf);	//!< put SR data into complex buffer		   USING PADDED DFT
  int CalcComplexPad2SrBuf(SRBUF scatBuf, NANBUF nanBuf);	//!< put complex data into SR buffer		   USING PADDED DFT
};
