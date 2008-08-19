/*
 * CamScattering.h
 * Header of the scattering compensation methods for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2008.08.19
 */

#pragma once
#include "wxSRparlab.h" //!< top-level header file
#include "wxSrApp.h"	//!< application header file
#include "CamFrame.h"	//!< main camera header file
#include <complex> // for ANSI C99 complex numbers; WATCH OUT, this seems to be C++'s  complex<T>
#include <fftw3.h>		//!< FFTW used
#pragma comment(lib, "libfftw3-3")

class CamFrame;

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
	int Compensate(unsigned short* amp, unsigned short* pha, int nCols, int nRows, int bufferSizeInBytes);
	//! Add abiltity to set target once and for all
	int SetCompensationTarget(unsigned short* amp, unsigned short* pha, int nCols, int nRows, int bufferSizeInBytes);
	//! Compensate on previously defined target
	int CompensateAgain();

private:
	CamFrame	*_camFrm; // parent main wnd
	unsigned short *_amp;
	unsigned short *_pha;
	int *_nCols;
	int *_nRows;
	int *_bufferSizeInBytes;


	int			_wPsf;					//!< width  of psf
	int			_hPsf;					//!< height of psf
	fftw_plan		_fftPlanForward;		//!< FFTW plan for FORWARD , IN-PLACE dft
	fftw_plan		_fftPlanBackward;		//!< FFTW plan for BACKWARD, IN-PLACE dft
	std::complex<double> *_cxBuf;			//!< buffer for complex img in FFTW pprocessing
	std::complex<double> *_cxFilt;		//!< buffer for complex filter in FFTW pprocessing
	std::complex<double> *_cxPar;		    //!< buffer for parity         in FFTW pprocessing
	double			   *_psf;			//!< buffer for PSF (real only)
	
};
