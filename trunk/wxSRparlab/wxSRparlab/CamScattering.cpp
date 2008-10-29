/*
 * CamScattering.cpp
 * Implementation of the scattering compensation methods for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2008.08.26
 */

#include "wxSRparlab.h" //!< top-level header file
#include "CamFrame.h"	//!< main camera header file
#include "imgPadder.h"
#include "CamScattering.h" //!< camera settings panel header file

/**
 * Camera frame class constructor \n
 * Each instance must have a parent frame (usually CMainWnd) \n
 */
CamScattering::CamScattering(CamFrame* camFrm)
{
	_camFrm = camFrm;
	_imgSz.cx = _camFrm->GetNumCols();
	_imgSz.cy = _camFrm->GetNumRows();
	CalcBufAlloc();
	CalcBufAllocDft();
#ifdef PAD_FFTW
	_padder = NULL;		// initialize only when padding is used
#endif
	CalcBufAllocPadDft();
	//ScatOptBufAlloc();
}

/**
 * Camera frame class destructor \n
 */
CamScattering::~CamScattering()
{
    CalcFreeBufsPadDft();
	CalcFreeBufsDft();
	CalcFreeBufs();
	if(_padder){ delete(_padder); _padder = NULL;};
}

//! Add abiltity to set target once and for all
int CamScattering::SetCompensationTarget(SRBUF scatBuf, NANBUF nanBuf)
{
	int res = 0;
    if( (scatBuf.amp==NULL) || (scatBuf.pha==NULL) || (scatBuf.nCols<1) || (scatBuf.nRows<1) || (scatBuf.nCols*scatBuf.nCols*sizeof(unsigned short) != scatBuf.bufferSizeInBytes) ){return -1;};
    _savBuf.amp = scatBuf.amp;
    _savBuf.pha = scatBuf.pha;
    _savBuf.nCols = scatBuf.nCols;
    _savBuf.nRows = scatBuf.nRows;
    _savBuf.bufferSizeInBytes = scatBuf.bufferSizeInBytes;
	return res;
}

//---------------------------------------------------
/*!
	Returns the PSF used in FFTW processing. \n
*/
//! Returns the PSF used in FFTW processing.
double* CamScattering::GetPsfD()
{
#ifdef USE_FFTW
  return _psf;
#endif
  return NULL;
}

//---------------------------------------------------
/*!
	This function allocates buffers for
	scattering correction algorithms  USING FFTW. \n
*/
//! Scatttering correction image buffer allocation
int CamScattering::CalcBufAllocDft()
{
	int res = 0;
#ifdef USE_FFTW
	int wdt = _imgSz.cx;	// image width
	int hgt = _imgSz.cy;	// image height
	res += CalcBufAllocDft(wdt,hgt);
#endif	// USE_FFTW
	return res;
}
//---------------------------------------------------
/*!
	This function allocates buffers for
	scattering correction algorithms  USING FFTW. \n
*/
//! Scatttering correction image buffer allocation
int CamScattering::CalcBufAllocDft(int wpsf, int hpsf)
{
	int res = 0;
#ifdef USE_FFTW
	_wPsf = wpsf;
	_hPsf = hpsf;
	int num=_wPsf*_hPsf;	// pixel count

	_cxBuf  = (std::complex<double>*) fftw_malloc(num*sizeof(std::complex<double>));		// allocate complex image buffer
	_cxFilt = (std::complex<double>*) fftw_malloc(num*sizeof(std::complex<double>));		// allocate complex filter buffer
	_cxPar  = (std::complex<double>*) fftw_malloc(num*sizeof(std::complex<double>));		// allocate parity  filter buffer
	_psf    = (double*) malloc(num*sizeof(double));		// allocate psf buffer
	// initialize forward DFT transform plan ( IN-PLACE )
	_fftPlanForward  = fftw_plan_dft_2d(_hPsf, _wPsf, reinterpret_cast<fftw_complex*>(_cxBuf), reinterpret_cast<fftw_complex*>(_cxBuf), FFTW_FORWARD, FFTW_MEASURE);
	// initialize backward DFT transform plan ( IN-PLACE )
	_fftPlanBackward = fftw_plan_dft_2d(_hPsf, _wPsf, reinterpret_cast<fftw_complex*>(_cxBuf), reinterpret_cast<fftw_complex*>(_cxBuf), FFTW_BACKWARD, FFTW_MEASURE);

	_imgSz.cx = _wPsf;	// image width
	_imgSz.cy = _hPsf;	// image height
#endif	// USE_FFTW
	return res;
}


//---------------------------------------------------
/*!
	This function frees  buffers used for
	scattering correction USING FFTW. \n
*/
//! Free scatttering correction image buffers.
int CamScattering::CalcFreeBufsDft()
{
	int res = 0;
#ifdef USE_FFTW
	fftw_destroy_plan(_fftPlanForward);			// destroy plans
	fftw_destroy_plan(_fftPlanBackward);
	if(_cxBuf ) { fftw_free(_cxBuf ); _cxBuf  = NULL ;};	// free buffers
	if(_cxFilt) { fftw_free(_cxFilt); _cxFilt = NULL ;};
	if(_cxPar ) { fftw_free(_cxPar ); _cxPar  = NULL ;};
	if(_psf   ) { free(_psf);         _psf    = NULL ;};
#endif
	return res;
}

//---------------------------------------------------
/*!
	This function intends to compensate scattering with DFT processing. \n
  \sa PL_srAcquire(), PL_srReplay()
*/
//! Scattering compensation calculations UISNG FFTW.
int CamScattering::CalcScatCorrDft(SRBUF scatBuf, NANBUF nanBuf)
{
	int res = 0;
#ifdef SCATCTIMER
	_scatCTimer.StartTimer();
#endif
#ifdef USE_FFTW
	// Ok, this should happen in three phases
	//  1- put SR image into the complex buffer _cxBUf
	//  2- perform in-place forward dft on _cxBuf
	//  3- divide _cxBuf by _cxFilt
	//  4- perform in-place backward dft
	//  5- put result in SR image

	// dft image dimensions
	int wdt = _imgSz.cx;
	int hgt = _imgSz.cy;
	int num = wdt*hgt;
	int numScaling = wdt*hgt;

	int i = 0 ; // counter used in loops


	// 1- : convert phase and amplitude data
	// into real and imaginary parts, in double format, and
	// into an _cxBuf buffer.
	res += CalcSrBuf2Complex(scatBuf);

	// ...
	//  2- perform in-place forward dft on _cxBuf
	// ...
	fftw_execute(_fftPlanForward);

	//  3- divide _cxBuf by _cxFilt
	for(i=0;i<num;i++)
    {
#ifndef DEBUG_FFTW
		_cxBuf[i] /= num; //scaling
			#ifndef DFT_INVPSF
		_cxBuf[i] /= _cxFilt[i]; // this should perform a complex division
			#else
				_cxBuf[i] *= _cxFilt[i]; // this should perform a complex division
			#endif //DFT_INVPSF
		// problem: the quadrants seem to be mixed when this division is used - SOLVED IN FILTER LOADER
#else
		_cxBuf[i] *= _cxFilt[i]; // debug, used to see PSF
#endif // DEBUG_FFTW
    }

	//  4- perform in-place backward dft
	fftw_execute(_fftPlanBackward);

	//  5- put result in SR image
	// Last image loop : convert real and imaginary parts
	// into phase and amplitude data, in ushort format, and
	// into an SR image buffer.
	CalcComplex2SrBuf(scatBuf, nanBuf);

#endif // USE_FFTW
#ifdef SCATCTIMER
	__int64 tic = _scatCTimer.GetTime();
	// float timeScatC_s = (float)tic/1e6f;
	_timeScatC_s = (float)tic/1e6f;
#endif
	return res;
}

//---------------------------------------------------
/*!
	This function reads a scattering psf model for DFT proessing. \n
  \sa PL_srAcquire(), PL_srReplay()
*/
//! Scattering compensation calculations UISNG FFTW.
int CamScattering::LoadScatPsf4Dft(const char* fn)
{
	int res = 0;
#ifdef USE_FFTW
	// define local variables for image dimensions
	int wdt = _camFrm->GetNumCols();	// image width
	int hgt = _camFrm->GetNumRows();	// image height
	int num=wdt*hgt;	// pixel count

	// file open operation
	FILE* pModel = fopen(fn, "rb");
	if(!pModel) return -1;
	if(!_cxBuf) return -2;
	double val;
	double max=0.0; int indMax = 0; // DEBUG, for parity filter
	for(int k=0; k<num ; k++) // loop on all coefficients
	{
		fread(&val, sizeof(double), 1, pModel);
		_cxBuf[k] = std::complex<double>(val, 0.0);
		_psf[k] = val;
		//_cxBuf[k] = std::complex<double>(0.0, 0.0);// DEBUG DEBUG DEBUG
		if(k==0)// DEBUG, for parity filter
		{
			max = val;
		}
		if(val>max)// DEBUG, for parity filter
		{
			max = val;
			indMax = k;
		}
		_cxPar[k] = std::complex<double>(0.0, 0.0);// DEBUG, for parity filter
	}
	//_cxBuf[(int)floor(num/2.0)-0] = std::complex<double>(1.0, 0.0);// DEBUG DEBUG DEBUG
	_cxPar[indMax] = std::complex<double>(1.0,0.0); //DEBUG, for parity unity filter

	res+=fclose(pModel); //close file

	res+= UpdateScatPsf4PadDft(); // _cxBuf, _cxFilt and _cxPar will NOT be changed
	res+= UpdateScatPsf4Dft();	  // _cxBuf, _cxFilt and _cxPar will be changed

#endif
	return res;
}

//---------------------------------------------------
/*!
	This function reads a scattering psf model for DFT proessing. \n
  \sa PL_srAcquire(), PL_srReplay()
*/
//! Scattering compensation calculations UISNG FFTW.
int CamScattering::UpdateScatPsf4Dft()
{
	int res = 0;
#ifdef USE_FFTW
	// dft image dimensions
	int wdt = _imgSz.cx;
	int hgt = _imgSz.cy;
	int num = wdt*hgt;

	// ...
	// Dft psf to obtain inverse of filter
	// ...
	fftw_execute(_fftPlanForward); // _cxBuf should now contain the filter values

	// transfer filter values to _cxFilt
	  //memcpy((void*) _cxFilt, (const void*) _cxBuf, num*sizeof(std::complex<double>)); //pb: does not allow scaling
	for(int k=0 ; k<num ; k++)
	{
		// // //_cxBuf[k] /= num; //scaling
		_cxFilt[k] = _cxBuf[k]; //copy
#ifdef DEBUG_FFTW
		// DEBUG DEBUG DEBUG   NOOP filtering operation
		//_cxFilt[k] = std::complex<double>(1.0, 0.0);
#endif
	}

	// transfer parity values to _cxBuf
	memcpy((void*) _cxBuf, (const void*) _cxPar, num*sizeof(std::complex<double>)); //pb: does not allow scaling
	// Dft for parity
	fftw_execute(_fftPlanForward); // _cxBuf should now contain the filter values
	// transfer parity filter values to _cxPar
	memcpy((void*) _cxPar, (const void*) _cxBuf, num*sizeof(std::complex<double>)); //pb: does not allow scaling

	//  multiply _cxFilt by _cxPar (to avoid an unecessary multiplication in acquire loop)
	for(int i=0;i<num;i++)
    {
		_cxFilt[i] *= std::complex<double>(_cxPar[i].real(), -_cxPar[i].imag()); // this should take care of parity
    }

#endif
	return res;
}

//---------------------------------------------------
/*!
	This function puts the image in the SR buffer
	into the DFT complex buffer. \n
*/
//! Puts SR data into complex buffer
int CamScattering::CalcSrBuf2Complex(SRBUF scatBuf)
{
	int res = 0;
#ifdef USE_FFTW
	double pi = 3.1415926535;	//pi needed for Euler transforms
	int wdt = scatBuf.nCols;	// image width
	int hgt = scatBuf.nRows;	// image height
	int num = wdt*hgt;	// pixel count

	WORD* amp = scatBuf.amp;//(WORD*)SR_GetImage(_srCam, 1);	// read current amplitude image
	WORD* pha = scatBuf.pha;//(WORD*)SR_GetImage(_srCam, 0);	// read current phase image

	int i; // counter used in loops

	double cosdb = 0 ; double re = 0; // variables to store current pixel information
	double sindb = 0 ; double im = 0; // variables to store current pixel information

	  // 1- : convert phase and amplitude data
	  // into real and imaginary parts, in double format, and
	  // into an _cxBuf buffer.
	for(i=0;i<num;i++)
    {
		  // Euler :  amp*e^(j*pha) = amp*( cos(pha) + j*sin(pha))
		cosdb = cos( (( (double)pha[i] ) / 65535.0 * 2.0 * pi)) ;
		re = ((double)amp[i] ) * cosdb;
		sindb = sin( (( (double)pha[i] ) / 65535.0 * 2.0 * pi)) ;
		im = ((double)amp[i] ) * sindb;

		_cxBuf[i]=std::complex<double>( re, im );
#ifdef DEBUG_FFTW
		_cxBuf[i]=std::complex<double>( 1.0, 0.0); // debug, used to see PSF
#endif // DEBUG_FFTW
    }
#ifdef LARGE_PSF
	// dft image dimensions //sould be an integer multiple of the SR image size
	int wdtL = _imgSz.cx;
	int hgtL = _imgSz.cy;
	int numL = wdtL*hgtL;
	IMGSIZE imSzSr; imSzSr.cx = wdt; imSzSr.cy = hgt;
	if((wdtL!=wdt) || (hgtL!=hgt))
	{
		res += _padder->imgPad(reinterpret_cast<fftw_complex*>(_cxBuf),wdt*hgt*sizeof(fftw_complex), imSzSr, reinterpret_cast<fftw_complex*>(_cxPadBuf), _imgSz.cx*_imgSz.cy*sizeof(fftw_complex), _imgSz);
		memcpy( (void*) _cxBuf, (const void*) _cxPadBuf, numL*sizeof(std::complex<double>) );
	}
#endif // LARGE_PSF
#endif // USE_FFTW
	return res;
}

//---------------------------------------------------
/*!
	This function puts the image in the DFT complex buffer
	into the SR buffer. \n
*/
//! Puts complex data into SR buffer
int CamScattering::CalcComplex2SrBuf(SRBUF scatBuf, NANBUF nanBuf)
{
	int res = 0;
#ifdef USE_FFTW
	double pi = 3.1415926535;	//pi needed for Euler transforms
	int wdt = scatBuf.nCols;	// image width
	int hgt = scatBuf.nRows;	// image height
	int num = wdt*hgt;	// pixel count

#ifdef LARGE_PSF
	// dft image dimensions //sould be an integer multiple of the SR image size
	int wdtL = _imgSz.cx;
	int hgtL = _imgSz.cy;
	int numL = wdt*hgt;
	IMGSIZE imSzSr; imSzSr.cx = wdt; imSzSr.cy = hgt;
	if((wdtL!=wdt) || (hgtL!=hgt))
	{
		res += _padder->imgUnPad(reinterpret_cast<fftw_complex*>(_cxBuf), _imgSz.cx*_imgSz.cy*sizeof(fftw_complex), _imgSz, reinterpret_cast<fftw_complex*>(_cxPadBuf),wdt*hgt*sizeof(fftw_complex), imSzSr);
		memcpy( (void*) _cxBuf, (const void*) _cxPadBuf, num*sizeof(std::complex<double>) );
	}
#endif // LARGE_PSF

	WORD* amp = scatBuf.amp;//(WORD*)SR_GetImage(_srCam, 1);	// read current amplitude image
	WORD* pha = scatBuf.pha;//(WORD*)SR_GetImage(_srCam, 0);	// read current phase image

	int i; // counter used in loops

	//  5- put result in SR image
	// Last image loop : convert real and imaginary parts
	// into phase and amplitude data, in ushort format, and
	// into an SR image buffer.
	double re = 0; double im = 0;
	double atan = 0 ; double ampdb = 0;	// current pixel variables
	WORD ampW = 0 ; WORD phaW = 0;		// current pixel variables
	WORD ampNaN = 0 ; WORD phaNaN = 65535;  // values PUT INTO nils
										    // amplitude 0
											// phase 65535 (7.50m)
	for(i=0;i<num;i++)	// for each pixel
    {
		if(nanBuf.nanBool[i])	// if NIL, set NIL amplitude and phase
		{
		  pha[i] = phaNaN;	// set NIL amplitude
		  amp[i] = ampNaN;	// set NIL phase
		}
		else			// when not NIL
		{
		  re = _cxBuf[i].real();	// get real value
		  im = _cxBuf[i].imag();	// get imag value

			atan = atan2( im, re) ;	// compute phase (through atan2f)
			ampdb = sqrt(re*re + im*im); // compute amplitude
		  //atan = arg(_cxBuf[i]) ;  // slower than my atan2 solution (really!)
		  //ampdb = abs(_cxBuf[i]);
		  if ((atan<0)) atan += 2*pi;	// unwrap phase (only positive values)
		  ampW = (WORD) ampdb;		// cast float value into SR WORD
		  phaW = (WORD) (65535* atan / (2*pi)); // cast float value into SR WORD
		  pha[i] = phaW;	// overwrite SR phase with modified value
		  amp[i] = ampW;	// overwrite SR amplitude with modified value
		}
    }
#endif // USE_FFTW
	return res;
}
//---------------------------------------------------
/*!
	This function puts the image in the SR buffer
	into the DFT complex buffer, with replication of
	pixels to match DFT image size. \n
*/
//! Puts SR data into complex buffer
int CamScattering::CalcSrBuf2RepComplex(SRBUF scatBuf, int repW, int repH)
{
	int res = 0;
#ifdef USE_FFTW
#ifdef LARGE_PSF
	double pi = 3.1415926535;	//pi needed for Euler transforms
	int wdt = scatBuf.nCols;	// image width
	int hgt = scatBuf.nRows;	// image height
	int num = wdt*hgt;	// pixel count

	// dft image dimensions //sould be an integer multiple of the SR image size
	int wdtD = _imgSz.cx;
	int hgtD = _imgSz.cy;
	int numD = wdt*hgt;
	int ratio = wdtD / wdt;

	WORD* amp= scatBuf.amp;//(WORD*)SR_GetImage(_srCam, 1);	// read current amplitude image
	WORD* pha= scatBuf.pha;//(WORD*)SR_GetImage(_srCam, 0);	// read current phase image

	double cosdb = 0 ; double re = 0; // variables to store current pixel information
	double sindb = 0 ; double im = 0; // variables to store current pixel information

	  // 1- : convert phase and amplitude data
	  // into real and imaginary parts, in double format, and
	  // into an _cxBuf buffer.

	int i, row, col, rw, rh; // counter used in loops
	i= 0;
	for(col=0;col<wdt;col++)
    {
		for(row=0;row<hgt;row++)
		{
			  // Euler :  amp*e^(j*pha) = amp*( cos(pha) + j*sin(pha))
			cosdb = cos( (( (double)pha[i] ) / 65535.0 * 2.0 * pi)) ;
			re = ((double)amp[i] ) * cosdb;
			sindb = sin( (( (double)pha[i] ) / 65535.0 * 2.0 * pi)) ;
			im = ((double)amp[i] ) * sindb;

			for(rh =0; rh< repH; rh++)
			{
				for(rw =0; rw< repW; rw++)
				{
					//_cxBuf[row*repH*wdtD + col*repW + rw + rh*wdtD]=std::complex<double>( re, im ); //TST_03
					//_cxBuf[col*repW*hgtD + row*repH + rh + rw*hgtD]=std::complex<double>( re, im ); //TST_04
					//_cxBuf[(col*repW+rw)*hgtD + (row)*repH + rh]=std::complex<double>( re, im ); //TST_05
					  _cxBuf[(row*repH+rh)*wdtD + (col)*repW + rw]=std::complex<double>( re, im ); //TST_06
				}
			}
			i++;
		}
    }
#endif //LARGE_PSF
#endif // USE_FFTW
	return res;
}

//---------------------------------------------------
/*!
	This function puts the image in the DFT complex buffer
	into the SR buffer, with dropping of pixels for larger
	DFT image size. \n
*/
//! Puts complex data into SR buffer
int CamScattering::CalcRepComplex2SrBuf(SRBUF scatBuf, NANBUF nanBuf, int repW, int repH)
{
	int res = 0;
#ifdef USE_FFTW
#ifdef LARGE_PSF
	double pi = 3.1415926535;	//pi needed for Euler transforms
	int wdt = scatBuf.nCols;	// image width
	int hgt = scatBuf.nRows;	// image height
	int num = wdt*hgt;	// pixel count

	WORD* amp= scatBuf.amp;//(WORD*)SR_GetImage(_srCam, 1);	// read current amplitude image
	WORD* pha= scatBuf.pha;//(WORD*)SR_GetImage(_srCam, 0);	// read current phase image

	int i, iRep; // counter used in loops

//  5- put result in SR image
	// Last image loop : convert real and imaginary parts
	// into phase and amplitude data, in ushort format, and
	// into an SR image buffer.
	double re = 0; double im = 0;
	double atan = 0 ; double ampdb = 0;	// current pixel variables
	WORD ampW = 0 ; WORD phaW = 0;		// current pixel variables
	WORD ampNaN = 0 ; WORD phaNaN = 65535;  // values PUT INTO nils
										    // amplitude 0
											// phase 65535 (7.50m)
	for(i=0;i<num;i++)	// for each pixel
    {
		if(nanBuf.nanBool[i])	// if NIL, set NIL amplitude and phase
		{
		  pha[i] = phaNaN;	// set NIL amplitude
		  amp[i] = ampNaN;	// set NIL phase
		}
		else			// when not NIL
		{
		  iRep = i*(repW*repH);
		  re = _cxBuf[iRep].real();	// get real value
		  im = _cxBuf[iRep].imag();	// get imag value

			atan = atan2( im, re) ;	// compute phase (through atan2f)
			ampdb = sqrt(re*re + im*im); // compute amplitude
		  //atan = arg(_cxBuf[i]) ;  // slower than my atan2 solution (really!)
		  //ampdb = abs(_cxBuf[i]);
		  if ((atan<0)) atan += 2*pi;	// unwrap phase (only positive values)
		  ampW = (WORD) ampdb;		// cast float value into SR WORD
		  phaW = (WORD) (65535* atan / (2*pi)); // cast float value into SR WORD
		  pha[i] = phaW;	// overwrite SR phase with modified value
		  amp[i] = ampW;	// overwrite SR amplitude with modified value
		}
    }
#endif // LARGE_PSF
#endif // USE_FFTW
	return res;
}
//---------------------------------------------------
/*!
	This function allocates buffers for
	scattering correction algorithms  USING FFTW AND PADDING. \n
*/
//! Scatttering correction image buffer allocation
int CamScattering::CalcBufAllocPadDft()
{
	int res = 0;
#ifdef USE_FFTW
#ifdef PAD_FFTW // if padding is used
	res += CalcBufAllocPadDft(2.0); //!< HARDCODED padding factor
#endif  // PAD_FFTW
#endif	// USE_FFTW
	return res;
}
//---------------------------------------------------
/*!
	This function allocates buffers for
	scattering correction algorithms  USING FFTW AND PADDING. \n
*/
//! Scatttering correction image buffer allocation
int CamScattering::CalcBufAllocPadDft(double padFact)
{
	int res = 0;
#ifdef USE_FFTW
#ifdef PAD_FFTW // if padding is used
	_padFact = padFact;
	// compute padded image size
	int wdt = (int)(_padFact*_wPsf);	// image width
	int hgt = (int)(_padFact*_hPsf);	// image height
	res += CalcBufAllocPadDft(wdt,hgt); // go to worker allocator
#endif  // PAD_FFTW
#endif	// USE_FFTW
	return res;
}
//---------------------------------------------------
/*!
	This function allocates buffers for
	scattering correction algorithms  USING FFTW AND PADDING. \n
*/
//! Scatttering correction image buffer allocation
int CamScattering::CalcBufAllocPadDft(int wpadpsf, int hpadpsf)
{
	int res = 0;
#ifdef USE_FFTW
#ifdef PAD_FFTW // if padding is used
	if(_padder){ delete(_padder); _padder = NULL;};
	_padder = new CImgPadder();

	_wPadPsf = wpadpsf;
	_hPadPsf = hpadpsf;

	_padFact =  (((double)_wPadPsf)/((double)_wPsf) + ((double)_hPadPsf)/((double)_hPsf) ) /2.0;

	// compute padded image size
	int wdt = (int)(_wPadPsf);	// image width
	int hgt = (int)(_hPadPsf);	// image height
	int num=(int)(wdt * hgt);	// new pixel count

	// allocate padded buffers
	_cxPadBuf  = (std::complex<double>*) fftw_malloc(num*sizeof(std::complex<double>));		// allocate complex image buffer
	_cxPadFilt = (std::complex<double>*) fftw_malloc(num*sizeof(std::complex<double>));		// allocate complex filter buffer
	_cxPadPar  = (std::complex<double>*) fftw_malloc(num*sizeof(std::complex<double>));		// allocate parity  filter buffer
	memset(_cxPadBuf , 0x0, num*sizeof(std::complex<double>));		// initialize complex image buffer
	memset(_cxPadFilt, 0x0, num*sizeof(std::complex<double>));		// initialize complex image buffer
	memset(_cxPadPar , 0x0, num*sizeof(std::complex<double>));		// initialize complex image buffer
	// initialize forward DFT transform plan ( IN-PLACE )
	_fftPadPlanForward  = fftw_plan_dft_2d(hgt, wdt, reinterpret_cast<fftw_complex*>(_cxPadBuf), reinterpret_cast<fftw_complex*>(_cxPadBuf), FFTW_FORWARD, FFTW_MEASURE);
	// initialize backward DFT transform plan ( IN-PLACE )
	_fftPadPlanBackward = fftw_plan_dft_2d(hgt, wdt, reinterpret_cast<fftw_complex*>(_cxPadBuf), reinterpret_cast<fftw_complex*>(_cxPadBuf), FFTW_BACKWARD, FFTW_MEASURE);

	// fill IMGSIZE containers (required by padder)
	_imgPadSz.cx = wdt;	// padded image width
	_imgPadSz.cy = hgt;	// padded image height

#endif  // PAD_FFTW
#endif	// USE_FFTW
	return res;
}

//---------------------------------------------------
/*!
	This function frees  buffers used for
	scattering correction USING FFTW AND PADDING. \n
*/
//! Free scatttering correction image buffers.
int CamScattering::CalcFreeBufsPadDft()
{
	int res = 0;
#ifdef USE_FFTW
#ifdef PAD_FFTW
	fftw_destroy_plan(_fftPadPlanForward);			// destroy plans
	fftw_destroy_plan(_fftPadPlanBackward);
	if(_padder){ delete(_padder); _padder = NULL;};
	if(_cxPadBuf ) { fftw_free(_cxPadBuf ); _cxPadBuf  = NULL ;};	// free buffers
	if(_cxPadFilt) { fftw_free(_cxPadFilt); _cxPadFilt = NULL ;};
	if(_cxPadPar ) { fftw_free(_cxPadPar ); _cxPadPar  = NULL ;};
#endif
#endif
	return res;
}

//---------------------------------------------------
/*!
	This function intends to compensate scattering with DFT processing USING PADDING. \n
  \sa PL_srAcquire(), PL_srReplay()
*/
//! Scattering compensation calculations UISNG FFTW.
int CamScattering::CalcScatCorrPadDft(SRBUF scatBuf, NANBUF nanBuf)
{
	int res = 0;
#ifdef SCATCTIMER
	_scatCTimer.StartTimer();
#endif
#ifdef USE_FFTW
#ifdef PAD_FFTW
	// Ok, this should happen in three phases
	//  1- put SR image into the complex buffer _cxBUf
	//  2- perform in-place forward dft on _cxBuf
	//  3- divide _cxBuf by _cxFilt
	//  4- perform in-place backward dft
	//  5- put result in SR image
	int numP = _imgPadSz.cx*_imgPadSz.cy;	// pixel count
	int num = _imgSz.cx*_imgSz.cy;	// pixel count

	int i = 0 ; // counter used in loops

	// 1- : convert phase and amplitude data
	// into real and imaginary parts, in double format, and
	// into an _cxBuf buffer.
	res += CalcSrBuf2ComplexPad(scatBuf);

	// ...
	//  2- perform in-place forward dft on _cxBuf
	// ...
	fftw_execute(_fftPadPlanForward);

	//  3- divide _cxBuf by _cxFilt
	for(i=0;i<numP;i++)
    {
#ifndef DEBUG_FFTW
		_cxPadBuf[i] /= numP; //scaling
		_cxPadBuf[i] /= _cxPadFilt[i]; // this should perform a complex division
#else
		_cxPadBuf[i] *= _cxPadFilt[i]; // debug, used to see PSF
#endif
    }

	//  4- perform in-place backward dft
	fftw_execute(_fftPadPlanBackward);


	//  5- put result in SR image
	// Last image loop : convert real and imaginary parts
	// into phase and amplitude data, in ushort format, and
	// into an SR image buffer.
	res += CalcComplexPad2SrBuf(scatBuf, nanBuf);
#endif // PAD_FFTW
#endif
#ifdef SCATCTIMER
	int64 tic = _scatCTimer.GetTime();
	_timeScatC_s = (float)tic/1e6f;
#endif
	return res;
}
//---------------------------------------------------
/*!
	This function simulates scattering with DFT processing USING PADDING. \n
  \sa PL_srAcquire(), PL_srReplay()
*/
//! Scattering compensation calculations UISNG FFTW.
int CamScattering::CalcScatSimulPadDft(SRBUF scatBuf , NANBUF nanBuf)
{
	int res = 0;
#ifdef SCATCTIMER
	_scatCTimer.StartTimer();
#endif
#ifdef USE_FFTW
#ifdef PAD_FFTW
	// Ok, this should happen in three phases
	//  1- put SR image into the complex buffer _cxBUf
	//  2- perform in-place forward dft on _cxBuf
	//  3- divide _cxBuf by _cxFilt
	//  4- perform in-place backward dft
	//  5- put result in SR image
	int numP = _imgPadSz.cx*_imgPadSz.cy;	// pixel count
	int num = _imgSz.cx*_imgSz.cy;	// pixel count

	int i = 0 ; // counter used in loops

	// 1- : convert phase and amplitude data
	// into real and imaginary parts, in double format, and
	// into an _cxBuf buffer.
	res += CalcSrBuf2ComplexPad(scatBuf);

	// ...
	//  2- perform in-place forward dft on _cxBuf
	// ...
	fftw_execute(_fftPadPlanForward);

	//  3- MULTIPLY _cxBuf by _cxFilt to silulate scattering //divide _cxBuf by _cxFilt
	for(i=0;i<numP;i++)
    {
#ifndef DEBUG_FFTW
		_cxPadBuf[i] /= numP; //scaling
		_cxPadBuf[i] *= _cxPadFilt[i]; // this should perform a complex multiplication
#else
		_cxPadBuf[i] *= _cxPadFilt[i]; // debug, used to see PSF
#endif
    }

	//  4- perform in-place backward dft
	fftw_execute(_fftPadPlanBackward);


	//  5- put result in SR image
	// Last image loop : convert real and imaginary parts
	// into phase and amplitude data, in ushort format, and
	// into an SR image buffer.
	res += CalcComplexPad2SrBuf(scatBuf, nanBuf);
#endif // PAD_FFTW
#endif
#ifdef SCATCTIMER
	int64 tic = _scatCTimer.GetTime();
	// float timeScatC_s = (float)tic/1e6f;
	_timeScatC_s = (float)tic/1e6f;
#endif
	return res;
}

//---------------------------------------------------
/*!
	This function updates scattering psf model for DFT proessing USING PADDING. \n
  \sa PL_srAcquire(), PL_srReplay()
*/
//! Scattering compensation calculations UISNG FFTW.
int CamScattering::UpdateScatPsf4PadDft()
{
	int res = 0;
#ifdef USE_FFTW
#ifdef PAD_FFTW  // when using padding, the padding function must be called prior to plan execution
	//int wdt = (int)SR_GetCols(_srCam);	// image width
	//int hgt = (int)SR_GetRows(_srCam);	// image height
	//int num = (int)SR_GetRows(_srCam)*(int)SR_GetCols(_srCam);	// pixel count
	// dft image dimensions
	int wdt = _imgSz.cx;
	int hgt = _imgSz.cy;
	int num = wdt*hgt;

	// padded image dimensions
	int wdtP = _imgPadSz.cx;
	int hgtP = _imgPadSz.cy;
	int numP = wdtP*hgtP;

	// create the padded filter
	_padder->imgPad(reinterpret_cast<fftw_complex*>(_cxBuf),wdt*hgt*sizeof(fftw_complex), _imgSz, reinterpret_cast<fftw_complex*>(_cxPadBuf), wdtP*hgtP*sizeof(fftw_complex), _imgPadSz);

	// ...
	// Dft psf to obtain inverse of filter
	// ...
	fftw_execute(_fftPadPlanForward); // _cxPadBuf should now contain the filter values

	// transfer filter values to _cxPadFilt
	  //memcpy((void*) _cxFilt, (const void*) _cxBuf, num*sizeof(std::complex<double>)); //pb: does not allow scaling
	for(int k=0 ; k<numP ; k++)
	{
		//_cxPadBuf[k] /= numP; //scaling //debug
		_cxPadFilt[k] = _cxPadBuf[k]; //copy
#ifdef DEBUG_FFTWPAD
		// DEBUG DEBUG DEBUG   NOOP filtering operation
		//_cxPadFilt[k] = std::complex<double>(1.0, 0.0);
#endif
	}

	// transfer parity values to _cxBuf
	_padder->imgPad(reinterpret_cast<fftw_complex*>(_cxPar),wdt*hgt*sizeof(fftw_complex), _imgSz, reinterpret_cast<fftw_complex*>(_cxPadBuf), wdtP*hgtP*sizeof(fftw_complex), _imgPadSz);
	// Dft for parity
	fftw_execute(_fftPadPlanForward); // _cxBuf should now contain the filter values
	// transfer parity filter values to _cxPar
	memcpy((void*) _cxPadPar, (const void*) _cxPadBuf, numP*sizeof(std::complex<double>)); //pb: does not allow scaling

	//  multiply _cxFilt by _cxPar (to aovid an unecessary multiplication in acquire loop)
	for(int i=0;i<numP;i++)
    {
		_cxPadFilt[i] *= std::complex<double>(_cxPadPar[i].real(), -_cxPadPar[i].imag()); // this should take care of parity
    }
#endif //PAD_FFTW
#endif
	return res;
}

//---------------------------------------------------
/*!
	This function puts the image in the SR buffer
	into the DFT complex buffer. \n
*/
//! Puts SR data into complex buffer
int CamScattering::CalcSrBuf2ComplexPad(SRBUF scatBuf)
{
	int res = 0;
#ifdef USE_FFTW
#ifdef PAD_FFTW
	res += CalcSrBuf2Complex(scatBuf); // put image into _cxBuf
	// create the padded filter
	_padder->imgPad(reinterpret_cast<fftw_complex*>(_cxBuf),_imgSz.cx*_imgSz.cy*sizeof(fftw_complex), _imgSz, reinterpret_cast<fftw_complex*>(_cxPadBuf), _imgPadSz.cx*_imgPadSz.cy*sizeof(fftw_complex), _imgPadSz);
#endif // PAD_FFTW
#endif // USE_FFTW
	return res;
}

//---------------------------------------------------
/*!
	This function puts the image in the DFT complex buffer
	into the SR buffer. \n
*/
//! Puts complex data into SR buffer
int CamScattering::CalcComplexPad2SrBuf(SRBUF scatBuf, NANBUF nanBuf)
{
	int res = 0;
#ifdef USE_FFTW
#ifdef PAD_FFTW
	// put image into _cxBuf
	_padder->imgUnPad(reinterpret_cast<fftw_complex*>(_cxPadBuf), _imgPadSz.cx*_imgPadSz.cy*sizeof(fftw_complex), _imgPadSz, reinterpret_cast<fftw_complex*>(_cxBuf), _imgSz.cx*_imgSz.cy*sizeof(fftw_complex), _imgSz);
	// call non-pad function
	res += CalcComplex2SrBuf(scatBuf, nanBuf);
#endif // PAD_FFTW
#endif // USE_FFTW
	return res;
}

//---------------------------------------------------
/*!
	This function performs the LARGE convolution process with
	a scattering kernel. The scattering kernel is
	modelled by a series of gaussian kernels.
	The computation is done by separation
	of the gaussian kernels. \n
	Code contains IFDEF's to select between
	IPL or custom convolution implementation.\n
*/
//! Scattering compensation convolution.
int CamScattering::CalcScatDoLargeConv()
{
	int res = 0;
#ifdef USE_FFTW
#ifdef LARGE_PSF
	int wdt = _imgSz.cx;	// image width
	int hgt = _imgSz.cy;	// image height
	int num= wdt*hgt;		// pixel count

	float re = 0; // variables to store current pixel information

#ifdef IPLENG
	IplImage* imgIpl[3];
#else
	myConvImg* imgMyKrn[3];
#endif //IPLENG
	for(int k=0; k<3; k++) // 3 buffers are needed for PSF creation
	{
#ifdef IPLENG
		  // create IPL image headers
		imgIpl[k] = iplCreateImageHeader(
			1, // number of channels
			0, // no alpha channel
			IPL_DEPTH_32F, // data of byte type
			"GRAY", // color model
			"G", // color order
			IPL_DATA_ORDER_PIXEL, // channel arrangement
			IPL_ORIGIN_TL, // top left orientation
			IPL_ALIGN_DWORD, // bytes align
			wdt, // image width
			hgt, // image height
			NULL, // no ROI
			NULL, // no mask ROI
			NULL, // no image ID
			NULL); // not tiled
		if( NULL == imgIpl[k] ) res-=1; // decrement res if a problem occurs
		iplAllocateImageFP( imgIpl[k], 1, 0.0f );			// allocate IPL image data
		if( NULL == imgIpl[k]->imageData ) res-=1 ;		// check result
#else
		imgMyKrn[k] = AllocMyConvImg(num,wdt); // allocate image buffer
		  // initialize buffer to 0
		memset( (void*) imgMyKrn[k]->_data, 0x00,num*sizeof(float));
#endif //IPLENG
	}
#ifdef IPLENG
	iplSetBorderMode(imgIpl[0], IPL_BORDER_CONSTANT, IPL_SIDE_ALL, 0 ); // 20060821 JMu border with IPL
#endif //IPLENG

// compute center pixel position
	int indMax = 0;
	indMax = wdt* hgt/2 + wdt/2 ; // compute the index of the center pixel
#ifndef IPLENG
	int i; // counter used in loops
	// put unit impulse in center position
	imgMyKrn[0]->_data[indMax] = 1.0f; //unit impulse
 #ifdef NRJ_CONS
	_wTot = 0.0f; // used for energy conserved convolution
 #endif //NRJCONS
#else  //IPLENG
	re = 1.0f;
	// put unit impulse in center position
	memcpy( (LPVOID) &imgIpl[0]->imageData[sizeof(float)*indMax],
			    (LPCVOID) &re, sizeof(float) );
#endif //IPLENG

//! CONVOLUTION LOOP
	for(int k=(_numScatGauss-1);k>-1;k--)// largest convolution first
										 // (most noise sensitive)
	{
		if( _w[k] != 0)		// avoid to convolve if weight is 0
		{
#ifdef IPLENG
			  // _imgIpl[1] : temporary real image buffer
			iplConvolveSep2DFP(imgIpl[0], imgIpl[1], _kernIplH[k], _kernIplV[k]);
			  // multiply convolution result by appropriate weight
			iplMultiplySFP(imgIpl[1], imgIpl[1], _w[k]);
			  // accumulate result in imgIpl[2]
			iplAdd(imgIpl[1], imgIpl[2], imgIpl[2]);
#else //IPLENG
			  // _imgMyKrn[2] : temporary real image buffer
#ifndef CONVFULL2D
			myConvolveHVfloat(imgMyKrn[0], _myKernsH[k], _myKernsV[k], imgMyKrn[1]);
#else //CONVFULL2D
			myConvolve2Dfloat(imgMyKrn[0], _myKernsH[k], _myKernsV[k], imgMyKrn[1]);
#endif //CONVFULL2D
			  // multiply convolution result by appropriate weight
			  // and accumulate result
			for(i=0;i<num;i++)
			{
				imgMyKrn[2]->_data[i] += _w[k] * imgMyKrn[1]->_data[i];
			}
			#ifdef NRJ_CONS
			 _wTot += _w[k];
			#endif
#endif //IPLENG
		}
	}	// END OF CONVOLUTION LOOP


// addtion at end of convolution
#ifdef IPLENG
			#ifndef DFT_INVPSF
	  // add Dirac
	iplAdd(imgIpl[0], imgIpl[2], imgIpl[0]);
			#else
				// subtract Dirac
				//iplAdd(imgIpl[0], imgIpl[2], imgIpl[0]);
				iplSubtract(imgIpl[0], imgIpl[2], imgIpl[0]);
			#endif // DFT_INVPSF
#else
			#ifndef DFT_INVPSF
	  // add Dirac
	for(i=0;i<num;i++)
	{
#ifndef NRJ_CONS
		  // IPL conformity
		imgMyKrn[0]->_data[i] += imgMyKrn[2]->_data[i];
#else
		  // energy conserved convolution
		imgMyKrn[0]->_data[i] = (1-_wTot)*imgMyKrn[0]->_data[i] + imgMyKrn[2]->_data[i];
#endif //NRJ_CONS
	}
			#else
				// subtract Dirac
					for(i=0;i<num;i++)
					{
				#ifndef NRJ_CONS
						  // IPL conformity
						imgMyKrn[0]->_data[i] -= imgMyKrn[2]->_data[i];
				#else
						  // energy conserved convolution
						imgMyKrn[0]->_data[i] = (1-_wTot)*imgMyKrn[0]->_data[i] - imgMyKrn[2]->_data[i];
				#endif //NRJ_CONS
					}
			#endif // DFT_INVPSF
#endif // IPL_ENG
	// END OF addition




	//remaining to do: put LARGE kernel into PSF buffers, and call update buffers method.
	re = 0; // variables to store current pixel information

	if(!_cxBuf) return -2;
	double val;
	double max=0.0;
	indMax = 0; // DEBUG, for parity filter
	for(int k=0; k<num ; k++) // loop on all coefficients
	{
		#ifdef IPLENG
			// awkward memcpy since IPL imageData is a char*
		  memcpy( (LPVOID) &re,			// get real value
			      (LPCVOID) &imgIpl[0]->imageData[sizeof(float)*k],
				   sizeof(float) );
		#else
			re = imgMyKrn[0]->_data[k];	// get real value
		#endif
		val = (double) re;
		_cxBuf[k] = std::complex<double>(val, 0.0);
		_psf[k] = val;
		//_cxBuf[k] = std::complex<double>(0.0, 0.0);// DEBUG DEBUG DEBUG
		if(k==0)// DEBUG, for parity filter
		{
			max = val;
		}
		if(val>max)// DEBUG, for parity filter
		{
			max = val;
			indMax = k;
		}
		_cxPar[k] = std::complex<double>(0.0, 0.0);// DEBUG, for parity filter
	}
	//_cxBuf[(int)floor(num/2.0)-0] = std::complex<double>(1.0, 0.0);// DEBUG DEBUG DEBUG
	_cxPar[indMax] = std::complex<double>(1.0,0.0); //DEBUG, for parity unity filter


	// now call the psf updates !!!
	// DONE IN InitScatPsfFromKernel() FUNCTION
	//res+= UpdateScatPsf4PadDft(); // _cxBuf, _cxFilt and _cxPar will NOT be changed
	//res+= UpdateScatPsf4Dft();	  // _cxBuf, _cxFilt and _cxPar will be changed



	// dft image dimensions //sould be an integer multiple of the SR image size
	int wdtSR = wdt ;//SR_GetCols(_srCam);
	int hgtSR = hgt ; //SR_GetRows(_srCam);
	int numSR = wdtSR*hgtSR;
	IMGSIZE imSzSr; imSzSr.cx = wdtSR; imSzSr.cy = hgtSR;
	// temporary double PSF buffer
	double* psfLoc = (double*) malloc(numSR*sizeof(double));
	memset(psfLoc, 0x0, numSR*sizeof(double));
	//
	res += _padder->imgUnPad(_psf,wdt*hgt*sizeof(double), _imgSz, psfLoc, wdtSR*hgtSR*sizeof(double), imSzSr);
	memcpy( (void*) _psf, (const void*) psfLoc, numSR*sizeof(double) );
	free(psfLoc); psfLoc = NULL;


		// This loop deletes intermediary images for scat calculation
	for(int k=0; k<3; k++)
	{
#ifdef IPLENG
		iplDeallocate( imgIpl[k], IPL_IMAGE_DATA ); // deallocate image data
			res = res - iplGetErrStatus();
		iplDeallocate( imgIpl[k], IPL_IMAGE_HEADER ); // deallocate image header
#else
		DeallocateMyConvImg(imgMyKrn[k]);	// deallocate image data
		SAFE_FREE(imgMyKrn[k]);
#endif
	}
#endif //LARGE_PSF
#endif // USE_FFTW
	return res;
}


//---------------------------------------------------
/*!
	This function allocates kernels for
	scattering correction algorithms. \n
	Code contains an 'ifdef' to select between
	IPL or custom convolution implementation.\n
	\sa PL_srAcquire(), PL_LoadScatKernels(), SwissrangerParlab::CalcScatCorr()
*/
//! Scattering correction kernels allocations
int CamScattering::InitKernels()
{
  int res = 0;
  int k = 0;		// will be set to the number of kernels
  float pi = 3.1415926535f;
  int wdt = _imgSz.cx; int hgt = _imgSz.cy;
  int num = _imgSz.cx * _imgSz.cy ;
  int numCoeffsH, numCoeffsV, centerH, centerV;
  float sigmaH, sigmaV;
  float w;
  float aH, aV;

  std::list<SCkernel>::iterator begin=_kernelList.begin(),end=_kernelList.end(),itr;

  for(itr=begin;itr!=end;itr++)
  {
    SCkernel& kernel=*itr;
    switch(kernel._type)
	{
		case SCkernel::KT_GAUSSIAN:
			{
				numCoeffsH = kernel._numCoeffsH;
				sigmaH     = kernel._sigmaH;
				numCoeffsV = kernel._numCoeffsV;
				sigmaV     = kernel._sigmaV;
				w		   = kernel._weight;
				centerH    = (int)(numCoeffsH/2);
				centerV    = (int)(numCoeffsV/2);
				float *kh = (float*)malloc(numCoeffsH*sizeof(float));
				float *kv = (float*)malloc(numCoeffsV*sizeof(float));
				int j = 0;
				aH = 1 / ( sqrt(2*pi) * sigmaH); // gaussian scaling amplitude
				for(j=0; j<numCoeffsH ; j++) // compute kernel coeffcients
				{
					kh[j] = (float)(aH * exp(-(float)
						  ( (float)((j -centerH)/sigmaH)
						  * (float)((j -centerH)/sigmaH) /2)));
				}
				aV = 1 / ( sqrt(2*pi) * sigmaV); // gaussian scaling amplitude
				for(j=0; j<numCoeffsV ; j++) // compute kernel coeffcients
				{
					kv[j] = (float)(aV * exp(-(float)
						  ( (float)((j -centerV)/sigmaV)
						  * (float)((j -centerV)/sigmaV) /2)));
				}
#ifdef IPLENG
				  // create IPL kernel
				_kernIplH[k] = iplCreateConvKernelFP( numCoeffsH,   1,
					                                  centerH,   0, &kh[0]);
				  // create IPL kernel
				_kernIplV[k] = iplCreateConvKernelFP( 1, numCoeffsV,
					                                  0, centerV, &kv[0]);
#else
				_myKernsH[k] = CreateMyKernel1D( numCoeffsH, centerH, &kh[0]);
				_myKernsV[k] = CreateMyKernel1D( numCoeffsV, centerV, &kv[0]);
#endif
				  // save weight value for this kernel
				memcpy( (void*) &_w[k], (void*) &w, sizeof(float) );

				SAFE_FREE(kh); // cleanup
				SAFE_FREE(kv); // cleanup
				k+=1;
				break;
			}
	}
	for(int j =k; j<8;j++) // zero pad the kernels list up to 8
	{
		w = 0;
		memcpy( (void*) &_w[j], (void*) &w, sizeof(float) );
		DeallocateMyKernel1D(_myKernsH[j]);DeallocateMyKernel1D(_myKernsV[j]);
		SAFE_FREE(_myKernsH[j]); SAFE_FREE(_myKernsV[j]);
	}
  }
  _numScatGauss = k ;
  return res;
}
//---------------------------------------------------
/*!
	This function emulates scattering as a series
	convolution process with a scattering kernel.
	The scattering kernel is modelled by a series of
	gaussian kernels. The computation is done by separation
	of the gaussian kernels. \n
	Code contains IFDEF's to select between
	IPL or custom convolution implementation.\n
  \sa CalcScatCorr()
*/
//! Scattering compensation calculations.
int CamScattering::InitScatPsfFromKernel()
{
	int res = 0;

	res += CalcUnitImpulse2Conv();	// put unit impulse into convolution buffers
	res += CalcScatDoConv();	// Do the convolution operation
	res += CalcScatDoAdd();		// ADD the original image to the convolved image (the list of kernels does not contain the Dirac)
	res += CalcConv2Psf();		// put convolved data into PSF buffer;

	res+= UpdateScatPsf4PadDft(); // _cxBuf, _cxFilt and _cxPar will NOT be changed
	res+= UpdateScatPsf4Dft();	  // _cxBuf, _cxFilt and _cxPar will be changed

	return res;
}

//---------------------------------------------------
/*!
	This function emulates scattering as a
	convolution process with a scattering kernel.
	The scattering kernel is modelled by a sum of
	gaussian kernels. The computation is done by DFT
	processing. \n
  \sa CalcScatCorrPadDft()
*/
//! Scattering compensation calculations.
int CamScattering::InitScatPsfFromLargeKernel()
{
	int res = 0;

	res += CalcBufReallocDft();	// put unit impulse into convolution buffers
	// WATCH OUT : CalcBufReallocDft() can reAlloc the DFT buffers
	res += CalcScatDoLargeConv();	// Do the convolution operation

	res+= UpdateScatPsf4PadDft(); // _cxBuf, _cxFilt and _cxPar will NOT be changed
	res+= UpdateScatPsf4Dft();	  // _cxBuf, _cxFilt and _cxPar will be changed

	return res;
}

//---------------------------------------------------
/*!
	This function puts a unit impulse signal into
	ad-hoc convolution buffers. /n
	Code contains IFDEF's to select between
	IPL or custom convolution implementation.\n
  \sa InitScatPsfFromKernel()
*/
//! Bufffer conversions.
int CamScattering::CalcBufReallocDft()
{
	int res = 0;

	int wdt = _imgSz.cx;	// read width (wdt) from current camera
	int hgt = _imgSz.cy;	// read height (hgt) from current camera
	int num=wdt*hgt;	// pixel count

	//! loop on convolution kernels for convolution image dimensions
	std::list<SCkernel>::iterator begin=_kernelList.begin(),end=_kernelList.end(),itr;

	int numCoeffsH, numCoeffsV;
	int wdtMax = wdt;
	int hgtMax = hgt;

	for(itr=begin;itr!=end;itr++)
	{
		SCkernel& kernel=*itr;
		switch(kernel._type)
		{
			case SCkernel::KT_GAUSSIAN:
			{
				numCoeffsH = kernel._numCoeffsH;
				numCoeffsV = kernel._numCoeffsV;
				if(numCoeffsH > wdtMax)
				{
					wdtMax = numCoeffsH;
				}
				if(numCoeffsV > hgtMax)
				{
					hgtMax = numCoeffsV;
				}
			}
		} // end of switch on kernel type
	}	// END OF loop on conv kernels

	double ratioW, ratioH;
	ratioW =0;ratioH =0;
	if(wdtMax != wdt)
	{
		ratioW = (double)wdtMax /(double) wdt;
	}
	if(hgtMax != hgt)
	{
		ratioH = (double)hgtMax /(double) hgt;
	}
	//int repFact = (int) ceil((double) max(ratioW,ratioH));
	int repFact = 0;
	if(ceil(ratioW)>0){repFact = (int)  ceil(ratioW);};
	if( (ceil(ratioH)>0) && (ceil(ratioH) > ceil(ratioW)) ){repFact = (int)  ceil(ratioH);};
	if(repFact != 0)		// if the image must be enlarged...
	{
		wdt *=repFact;		// ...define new image dimensions
		hgt *=repFact;
	}
	else
	{
		return res;			// ... return without reallocating
	}

	// initialize DFT buffers a new with updated width and height
	res += CalcFreeBufsDft();
	res += CalcBufAllocDft(wdt, hgt);
	//if((wdt>_imgPadSz.cx) || (hgt>_imgPadSz.cy) )
	//{
		res += CalcFreeBufsPadDft();
		//res += CalcBufAllocPadDft(wdt+20, hgt+20);
		res += CalcBufAllocPadDft();
	//}

	return res;
}

//---------------------------------------------------
/*!
	This function puts the data of the ad-hoc convolution buffers
	into the PSF buffer. /n
	Code contains IFDEF's to select between
	IPL or custom convolution implementation.\n
  \sa InitScatPsfFromKernel()
*/
//! Bufffer conversions.
int CamScattering::CalcConv2Psf()
{
	int res = 0;

	int wdt = _imgSz.cx;	// read width (wdt) from current camera
	int hgt = _imgSz.cy;	// read height (hgt) from current camera
	int num = wdt * hgt;	// pixel count

	float re = 0; // variables to store current pixel information

	if(!_cxBuf) return -2;
	double val;
	double max=0.0; int indMax = 0; // DEBUG, for parity filter
	for(int k=0; k<num ; k++) // loop on all coefficients
	{
		#ifdef IPLENG
			// awkward memcpy since IPL imageData is a char*
		  memcpy( (LPVOID) &re,			// get real value
			      (LPCVOID) &_imgIpl[0]->imageData[sizeof(float)*k],
				   sizeof(float) );
		#else
			re = _imgMyKrn[0]->_data[k];	// get real value
		#endif
		val = (double) re;
		_cxBuf[k] = std::complex<double>(val, 0.0);
		_psf[k] = val;
		//_cxBuf[k] = std::complex<double>(0.0, 0.0);// DEBUG DEBUG DEBUG
		if(k==0)// DEBUG, for parity filter
		{
			max = val;
		}
		if(val>max)// DEBUG, for parity filter
		{
			max = val;
			indMax = k;
		}
		_cxPar[k] = std::complex<double>(0.0, 0.0);// DEBUG, for parity filter
	}
	//_cxBuf[(int)floor(num/2.0)-0] = std::complex<double>(1.0, 0.0);// DEBUG DEBUG DEBUG
	_cxPar[indMax] = std::complex<double>(1.0,0.0); //DEBUG, for parity unity filter
	return res;
}

//---------------------------------------------------
/*!
	This function allocates buffers for
	scattering correction algorithms. \n
	Code contains an 'ifdef' to select between
	IPL or custom convolution implementation.\n
*/
//! Scatttering correction image buffer allocation
int CamScattering::CalcBufAlloc()
{
	int res = 0;
	int wdt = _imgSz.cx;	// image width
	int hgt = _imgSz.cy;	// image height
	int num=wdt*hgt;	// pixel count

	_numScatGauss = 7; // initialize the _numScatGauss variable
					   // loading another XML file can change this value

	for(int k=0; k<6; k++) // 6 buffers are needed for scattering compensation
	{
#ifdef IPLENG
		memset( (void*) &_kernIplH[0], 0x00,8*sizeof(IplConvKernelFP*)); // initialize 1Dkrn pointers to 0;
		memset( (void*) &_kernIplV[0], 0x00,8*sizeof(IplConvKernelFP*)); // initialize 1Dkrn pointers to 0;
		  // create IPL image headers
		_imgIpl[k] = iplCreateImageHeader(
			1, // number of channels
			0, // no alpha channel
			IPL_DEPTH_32F, // data of byte type
			"GRAY", // color model
			"G", // color order
			IPL_DATA_ORDER_PIXEL, // channel arrangement
			IPL_ORIGIN_TL, // top left orientation
			IPL_ALIGN_DWORD, // bytes align
			wdt, // image width
			hgt, // image height
			NULL, // no ROI
			NULL, // no mask ROI
			NULL, // no image ID
			NULL); // not tiled
		if( NULL == _imgIpl[k] ) res=res -1; // decrement res if a problem occurs
		iplAllocateImageFP( _imgIpl[k], 1, 0.0f );			// allocate IPL image data
		if( NULL == _imgIpl[k]->imageData ) res=res-1 ;		// check result
#else
		memset( (void*) &_myKernsH[0], 0x00,8*sizeof(myKernel1D*)); // initialize 1Dkrn pointers to 0;
		memset( (void*) &_myKernsV[0], 0x00,8*sizeof(myKernel1D*)); // initialize 1Dkrn pointers to 0;
		_imgMyKrn[k] = AllocMyConvImg(num,wdt); // allocate image buffer
		  // initialize buffer to 0
		memset( (void*) _imgMyKrn[k]->_data, 0x00,num*sizeof(float));
#endif
	}
#ifdef IPLENG
	iplSetBorderMode(_imgIpl[0], IPL_BORDER_CONSTANT, IPL_SIDE_ALL, 0 ); // 20060821 JMu border with IPL
	iplSetBorderMode(_imgIpl[1], IPL_BORDER_CONSTANT, IPL_SIDE_ALL, 0 ); // 20060821 JMu border with IPL
#endif
	  // define initial weights
	static float w[8] = { 0.0005f, 0.0005f, 0.010f, 0.020f,
		                  0.040f, 0.08f, 0.08f, 0.0f };
	  // copy them in _w member
	memcpy( (void*) &_w[0], (void*) &w, 7*sizeof(float) );
	return res;
}

//---------------------------------------------------
/*!
	This function frees  buffers used for
	scattering correction . \n
	Code contains an 'ifdef' to select between
	IPL or custom convolution implementation.\n
*/
//! Free scatttering correction image buffers.
int CamScattering::CalcFreeBufs()
{
	int res = 0;
	int k=0;
	// This loop deletes convolution kernels.
	for(k=0;k<8;k++)
	{
#ifdef IPLENG
		if (_kernIplH[k]!=NULL) iplDeleteConvKernelFP( _kernIplH[k]);
		if (_kernIplV[k]!=NULL) iplDeleteConvKernelFP( _kernIplV[k]);
#else
		DeallocateMyKernel1D(_myKernsH[k]);DeallocateMyKernel1D(_myKernsV[k]);
		SAFE_FREE(_myKernsH[k]); SAFE_FREE(_myKernsV[k]);
#endif
	}
	// This loop deletes intermediary images for scat calculation
	for(k=0; k<6; k++)
	{
#ifdef IPLENG
		iplDeallocate( _imgIpl[k], IPL_IMAGE_DATA ); // deallocate image data
			res = res - iplGetErrStatus();
		iplDeallocate( _imgIpl[k], IPL_IMAGE_HEADER ); // deallocate image header
#else
		DeallocateMyConvImg(_imgMyKrn[k]);	// deallocate image data
		SAFE_FREE(_imgMyKrn[k]);
#endif
	}

	return res;
}

//---------------------------------------------------
/*!
	This function intends to model scattering as a series
	convolution process with a scattering kernel.
	The scattering kernel is modelled by a series of
	gaussian kernels. The computation is done by separation
	of the gaussian kernels. \n
	Code contains IFDEF's to select between
	IPL or custom convolution implementation.\n
  \sa PL_srAcquire(), PL_srReplay()
*/
//! Scattering compensation calculations.
int CamScattering::CalcScatCorr(SRBUF scatBuf, NANBUF nanBuf)
{
	int res = 0;

#ifdef SCATCTIMER
	_scatCTimer.StartTimer();
#endif

	res += CalcSrBuf2Conv(scatBuf);	// put SR data into convolution buffers

	res += CalcScatDoConv();	// Do the convolution operation

	res += CalcScatDoSub();		// Subtract the original image to the convolved image

	res += CalcConv2SrBuf(scatBuf, nanBuf); // put corrected data back into SR buffer;

#ifdef SCATCTIMER
	__int64 tic = _scatCTimer.GetTime();
	// float timeScatC_s = (float)tic/1e6f;
	_timeScatC_s = (float)tic/1e6f;
#endif
	return res;
}

//---------------------------------------------------
/*!
	This function emulates scattering as a series
	convolution process with a scattering kernel.
	The scattering kernel is modelled by a series of
	gaussian kernels. The computation is done by separation
	of the gaussian kernels. \n
	Code contains IFDEF's to select between
	IPL or custom convolution implementation.\n
  \sa CalcScatCorr()
*/
//! Scattering compensation calculations.
int CamScattering::CalcScatSimul(SRBUF scatBuf, NANBUF nanBuf)
{
	int res = 0;

#ifdef SIMUL_SCAT_DFT
	res += CalcScatSimulPadDft(scatBuf, nanBuf);
	return res;
#endif

#ifdef SCATCTIMER
	_scatCTimer.StartTimer();
#endif

	res += CalcSrBuf2Conv(scatBuf);	// put SR data into convolution buffers

	res += CalcScatDoConv();	// Do the convolution operation

	//res += CalcScatDoSub();		// Subtract the original image to the convolved image
	res += CalcScatDoAdd();		// ADD the original image to the convolved image (the list of kernels does not contain the Dirac)

	res += CalcConv2SrBuf(scatBuf, nanBuf); // put corrected data back into SR buffer;

#ifdef SCATCTIMER
	__int64 tic = _scatCTimer.GetTime();
	// float timeScatC_s = (float)tic/1e6f;
	_timeScatC_s = (float)tic/1e6f;
#endif
	return res;
}

//---------------------------------------------------
/*!
	This function performs the addition of input image
	needed in scattering SIMULATION method using
	convolution. \n
	Code contains IFDEF's to select between
	IPL or custom convolution implementation.\n
  \sa CalcScatCorr()
*/
//! Scattering compensation subtraction.
int CamScattering::CalcScatDoAdd()
{
	int res = 0;

#ifdef IPLENG
	  // substract scattering estimate from measured signal
	iplAdd(_imgIpl[0], _imgIpl[4], _imgIpl[0]);
	iplAdd(_imgIpl[1], _imgIpl[5], _imgIpl[1]);
#else
	int num= _imgSz.cx * _imgSz.cy;	// pixel count
	int i; // counter used in loops

	for(i=0;i<num;i++)
	{
#ifndef NRJ_CONS
		  // IPL conformity
		_imgMyKrn[0]->_data[i] += _imgMyKrn[4]->_data[i];
		_imgMyKrn[1]->_data[i] += _imgMyKrn[5]->_data[i];/**/
#else
		  // energy conserved convolution
		_imgMyKrn[0]->_data[i] = (1-_wTot)*_imgMyKrn[0]->_data[i] + _imgMyKrn[4]->_data[i];
		_imgMyKrn[1]->_data[i] = (1-_wTot)*_imgMyKrn[1]->_data[i] + _imgMyKrn[5]->_data[i];/**/
#endif //NRJ_CONS
	}
#endif // IPL_ENG

	return res;
}

//---------------------------------------------------
/*!
	This function performs the convolution process with
	a scattering kernel. The scattering kernel is
	modelled by a series of gaussian kernels.
	The computation is done by separation
	of the gaussian kernels. \n
	Code contains IFDEF's to select between
	IPL or custom convolution implementation.\n
  \sa CalcScatCorr()
*/
//! Scattering compensation convolution.
int CamScattering::CalcScatDoConv()
{
	int res = 0;

#ifndef IPLENG
	int num=_imgSz.cx *_imgSz.cy;	// pixel count
	int i; // counter used in loops
 #ifdef NRJ_CONS
	_wTot = 0.0f; // used for energy conserved convolution
 #endif
#endif

//! CONVOLUTION LOOP
	for(int k=(_numScatGauss-1);k>-1;k--)// largest convolution first
										 // (most noise sensitive)
	{
		if( _w[k] != 0)		// avoid to convolve if weight is 0
		{
#ifdef IPLENG
			  // _imgIpl[2] : temporary real image buffer
			  // _imgIpl[3] : temporary imag image buffer
			iplConvolveSep2DFP(_imgIpl[0], _imgIpl[2], _kernIplH[k], _kernIplV[k]);
			iplConvolveSep2DFP(_imgIpl[1], _imgIpl[3], _kernIplH[k], _kernIplV[k]);
			  // multiply convolution result by appropriate weight
			iplMultiplySFP(_imgIpl[2], _imgIpl[2], _w[k]);
			iplMultiplySFP(_imgIpl[3], _imgIpl[3], _w[k]);
			  // accumulate result in _imgIpl[4], _imgIpl[5]
			iplAdd(_imgIpl[2], _imgIpl[4], _imgIpl[4]);
			iplAdd(_imgIpl[3], _imgIpl[5], _imgIpl[5]);
#else
			  // _imgMyKrn[2] : temporary real image buffer
			  // _imgMyKrn[3] : temporary imag image buffer
#ifndef CONVFULL2D
			myConvolveHVfloat(_imgMyKrn[0], _myKernsH[k], _myKernsV[k], _imgMyKrn[2]);
			myConvolveHVfloat(_imgMyKrn[1], _myKernsH[k], _myKernsV[k], _imgMyKrn[3]);
#else
			myConvolve2Dfloat(_imgMyKrn[0], _myKernsH[k], _myKernsV[k], _imgMyKrn[2]);
			myConvolve2Dfloat(_imgMyKrn[1], _myKernsH[k], _myKernsV[k], _imgMyKrn[3]);
#endif
			  // multiply convolution result by appropriate weight
			  // and accumulate result
			for(i=0;i<num;i++)
			{
				_imgMyKrn[4]->_data[i] += _w[k] * _imgMyKrn[2]->_data[i];
				_imgMyKrn[5]->_data[i] += _w[k] * _imgMyKrn[3]->_data[i];
			}
			#ifdef NRJ_CONS
			 _wTot += _w[k];
			#endif
#endif
		}
	}	// END OF CONVOLUTION LOOP

	return res;
}

//---------------------------------------------------
/*!
	This function performs the subtraction of input image
	needed in scattering compensation method using
	convolution. \n
	Code contains IFDEF's to select between
	IPL or custom convolution implementation.\n
  \sa CalcScatCorr()
*/
//! Scattering compensation subtraction.
int CamScattering::CalcScatDoSub()
{
	int res = 0;

#ifdef IPLENG
	  // substract scattering estimate from measured signal
	iplSubtract(_imgIpl[0], _imgIpl[4], _imgIpl[0]);
	iplSubtract(_imgIpl[1], _imgIpl[5], _imgIpl[1]);
#else
	int num = _imgSz.cx * _imgSz.cy;	// pixel count
	int i; // counter used in loops

	for(i=0;i<num;i++)
	{
#ifndef NRJ_CONS
		  // IPL conformity
		_imgMyKrn[0]->_data[i] -= _imgMyKrn[4]->_data[i];
		_imgMyKrn[1]->_data[i] -= _imgMyKrn[5]->_data[i];/**/
#else
		  // energy conserved convolution
		_imgMyKrn[0]->_data[i] = (1+_wTot)*_imgMyKrn[0]->_data[i] - _imgMyKrn[4]->_data[i];
		_imgMyKrn[1]->_data[i] = (1+_wTot)*_imgMyKrn[1]->_data[i] - _imgMyKrn[5]->_data[i];/**/
#endif //NRJ_CONS
	}
#endif // IPL_ENG

	return res;
}


//---------------------------------------------------
/*!
	This function puts the data of the SR buffer into
	ad-hoc convolution buffers. /n
	Code contains IFDEF's to select between
	IPL or custom convolution implementation.\n
  \sa CalcScatCorr()
*/
//! Bufffer conversions.
int CamScattering::CalcSrBuf2Conv(SRBUF scatBuf)
{
	int res = 0;

	float pi = 3.1415926535f;			// pi needed for complex numbers handling
	int wdt = scatBuf.nCols;	// read width (wdt) from current camera
	int hgt = scatBuf.nRows;	// read height (hgt) from current camera
	int num = wdt*hgt;	// pixel count

	WORD* amp = scatBuf.amp;	// read current amplitude image
	WORD* pha = scatBuf.pha ;	// read current phase image

	int i; // counter used in loops

	float cos = 0 ; float re = 0; // variables to store current pixel information
	float sin = 0 ; float im = 0; // variables to store current pixel information
	  // First image loop : convert phase and amplitude data
	  // into real and imaginary parts, in float format, and
	  // into an IPL image buffer.
	for(i=0;i<num;i++)
    {
		  // Euler :  amp*e^(j*pha) = amp*( cos(pha) + j*sin(pha))
		cos = cosf( (( (float)pha[i] ) / 65535 * 2 * pi)) ;
		re = ((float)amp[i] ) * cos;
		sin = sinf( (( (float)pha[i] ) / 65535 * 2 * pi)) ;
		im = ((float)amp[i] ) * sin;
#ifdef IPLENG
		  // _imgIpl[0] stores the real part of data read from srBuffer
		  // awkward memcpy since IPL imageData is a char*
		memcpy( (LPVOID) &_imgIpl[0]->imageData[sizeof(float)*i],
			    (LPCVOID) &re, sizeof(float) );
		  // _imgIpl[1] stores the imag part of data read from srBuffer
		  // awkward memcpy since IPL imageData is a char*
		memcpy( (LPVOID) &_imgIpl[1]->imageData[sizeof(float)*i],
			    (LPCVOID) &im, sizeof(float) );
#else
		  // _imgMyKrn[1] stores the imag part of data read from srBuffer
		_imgMyKrn[0]->_data[i] = re ;
		  // _imgMyKrn[1] stores the imag part of data read from srBuffer
		_imgMyKrn[1]->_data[i] = im ;
#endif
    }
	// Two buffers must be reset. They will hold convolution results
#ifdef IPLENG
	iplSetFP(_imgIpl[4], 0.0f); // _imgIpl[4] : real part of scat estimate image
	iplSetFP(_imgIpl[5], 0.0f); // _imgIpl[5] : imag part of scat estimate image
#else
	  // _imgMyKrn[4] : real part of scat estimate image
	memset(_imgMyKrn[4]->_data ,0x00,sizeof(float)*(num));
	  // _imgMyKrn[5] : imag part of scat estimate image
	memset(_imgMyKrn[5]->_data ,0x00,sizeof(float)*(num));
#endif
	return res;
}

//---------------------------------------------------
/*!
	This function puts the data of the ad-hoc convolution buffers
	back into the SR buffer. /n
	Code contains IFDEF's to select between
	IPL or custom convolution implementation.\n
  \sa CalcScatCorr()
*/
//! Bufffer conversions.
int CamScattering::CalcConv2SrBuf(SRBUF scatBuf, NANBUF nanBuf)
{
	int res = 0;

	float pi = 3.1415926535f;			// pi needed for complex numbers handling
	int wdt = scatBuf.nCols;	// read width (wdt) from current camera
	int hgt = scatBuf.nRows;	// read height (hgt) from current camera
	int num = wdt*hgt;	// pixel count

	WORD* amp = scatBuf.amp;	// read current amplitude image
	WORD* pha = scatBuf.pha;	// read current phase image

	int i; // counter used in loops

	float cos = 0 ; float re = 0; // variables to store current pixel information
	float sin = 0 ; float im = 0; // variables to store current pixel information

	// Last image loop : convert real and imaginary parts
	// into phase and amplitude data, in ushort format, and
	// into an SR image buffer.
	float atan2 = 0 ; float ampf = 0;	// current pixel variables
	WORD ampW = 0 ; WORD phaW = 0;		// current pixel variables
	WORD ampNaN = 0 ; WORD phaNaN = 65535;  // values PUT INTO nils
										    // amplitude 0
											// phase 65535 (7.50m)
	for(i=0;i<num;i++)	// for each pixel
    {
		if(nanBuf.nanBool[i])	// if NIL, set NIL amplitude and phase
		{
		  pha[i] = phaNaN;	// set NIL amplitude
		  amp[i] = ampNaN;	// set NIL phase
		}
		else			// when not NIL
		{
#ifdef IPLENG
			// awkward memcpy since IPL imageData is a char*
		  memcpy( (LPVOID) &re,			// get real value
			      (LPCVOID) &_imgIpl[0]->imageData[sizeof(float)*i],
				   sizeof(float) );
		    // awkward memcpy since IPL imageData is a char*
		  memcpy( (LPVOID) &im,			// get imag value
			      (LPCVOID) &_imgIpl[1]->imageData[sizeof(float)*i],
				  sizeof(float) );
#else
		re = _imgMyKrn[0]->_data[i];	// get real value
		im = _imgMyKrn[1]->_data[i];	// get imag value
#endif
		  atan2 = atan2f( im, re) ;	// compute phase (through atan2f)
		  if ((atan2<0)) atan2 += 2*pi;	// unwrap phase (only positive values)
		  ampf = sqrt(re*re + im*im); // compute amplitude
		  ampW = (WORD) ampf;		// cast float value into SR WORD
		  phaW = (WORD) (65535* atan2 / (2*pi)); // cast float value into SR WORD
		  pha[i] = phaW;	// overwrite SR phase with modified value
		  amp[i] = ampW;	// overwrite SR amplitude with modified value
		}
    }
	return res;
}

//---------------------------------------------------
/*!
	This function puts a unit impulse signal into
	ad-hoc convolution buffers. /n
	Code contains IFDEF's to select between
	IPL or custom convolution implementation.\n
  \sa InitScatPsfFromKernel()
*/
//! Bufffer conversions.
int CamScattering::CalcUnitImpulse2Conv()
{
	int res = 0;

	float pi = 3.1415926535f;			// pi needed for complex numbers handling
	int wdt = _imgSz.cx;	// read width (wdt) from current camera
	int hgt = _imgSz.cy;	// read height (hgt) from current camera
	int num = wdt*hgt;	// pixel count

	float re = 1.0f; // variables to store current pixel information

	int i = 0;
	i = wdt* hgt/2 + wdt/2 ;
#ifdef IPLENG
	iplSetFP(_imgIpl[0], 0.0f); // _imgIpl[4] : real part of input image
	iplSetFP(_imgIpl[1], 0.0f); // _imgIpl[5] : imag part of input image
	      // _imgIpl[0] stores the real part of data read from srBuffer
		  // awkward memcpy since IPL imageData is a char*
		memcpy( (LPVOID) &_imgIpl[0]->imageData[sizeof(float)*i],
			    (LPCVOID) &re, sizeof(float) );
	// Two buffers must be reset. They will hold convolution results
	iplSetFP(_imgIpl[4], 0.0f); // _imgIpl[4] : real part of scat estimate image
	iplSetFP(_imgIpl[5], 0.0f); // _imgIpl[5] : imag part of scat estimate image
#else
	  // _imgMyKrn[4] : real part of input image
	memset(_imgMyKrn[0]->_data ,0x00,sizeof(float)*(num));
	  // _imgMyKrn[5] : imag part of input image
	memset(_imgMyKrn[1]->_data ,0x00,sizeof(float)*(num));
		  // _imgMyKrn[1] stores the imag part of data read from srBuffer
		_imgMyKrn[0]->_data[i] = re ;
	// Two buffers must be reset. They will hold convolution results
	  // _imgMyKrn[4] : real part of scat estimate image
	memset(_imgMyKrn[4]->_data ,0x00,sizeof(float)*(num));
	  // _imgMyKrn[5] : imag part of scat estimate image
	memset(_imgMyKrn[5]->_data ,0x00,sizeof(float)*(num));
#endif
	return res;
}

//---------------------------------------------------
/*!
	Reads scattering compensation settings from XML file. \n
	\sa PL_ScatCompLoadParams()
*/
int CamScattering::LoadScatSettings(const char* fn)
{
	int res = 0;
	try
	{
		ticpp::Document doc( fn );
		doc.LoadFile();

		_kernelList.erase(_kernelList.begin(),_kernelList.end());
	
		ticpp::Element* pKrn = doc.FirstChildElement("PersPass")->FirstChildElement("ScatComp")->FirstChildElement("kernel");
		while(pKrn != NULL)
		{
			int numCoeffsH=0;
			float sigmaH=0;
			int numCoeffsV=0;
			float sigmaV=0;
			float weight=0.f;
			
			pKrn->GetAttribute("numCoeffsV", &numCoeffsV);
			pKrn->GetAttribute("sigmaV", &sigmaV);
			pKrn->GetAttribute("numCoeffsH", &numCoeffsH);
			pKrn->GetAttribute("sigmaH", &sigmaH);
			pKrn->GetAttribute("weight", &weight);
			_kernelList.push_back(SCkernel(SCkernel::KT_GAUSSIAN, numCoeffsH, sigmaH, numCoeffsV, sigmaV, weight));
			pKrn = pKrn->NextSiblingElement("kernel", false);
		}

	}
	catch( ticpp::Exception& ex )
	{
		std::cout << ex.what();
		return -1;
	}
	catch(...)
	{
		_kernelList.push_back(SCkernel(SCkernel::KT_GAUSSIAN, 20, 3, 30, 2, 1.0)); // dummy values
		_numScatGauss = 1;
		return -1;
	}
	res += InitKernels();

#ifndef LARGE_PSF
  res += InitScatPsfFromKernel();// initialize the scattering PSF from its the kernel description
#else // LARGE_PSF
  res += InitScatPsfFromLargeKernel();//initialize a large scattering PSF from its the kernel description
#endif // LARGE_PSF
  return res;
}

//! Compensate scattering method
int CamScattering::Compensate(SRBUF scatBuf, NANBUF nanBuf)
{
	int res = 0;
	res += CalcScatCorrPadDft(scatBuf, nanBuf);
	return res;
}