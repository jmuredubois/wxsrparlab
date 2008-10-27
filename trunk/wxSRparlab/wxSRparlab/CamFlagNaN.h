/*
 * CamFlagNaN.h
 * Header of the NaN flagging methods for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2008.10.23
 */

#pragma once
#include "wxSRparlab.h" //!< top-level header file
#include "wxSrApp.h"	//!< application header file
#include "CamFrame.h"	//!< main camera header file
#include "srBuf.h"


typedef struct nanVar {
    bool* nanBool;
    int nCols;
    int nRows;
    int bufferSizeInBytes;
} NANBUF;

class CamFrame;

/**
 * Camera frame class \n
 * This class: \n
 * - computes scattering compensation (s) \n
 */
class CamFlagNaN: public wxFrame //!< Camera frame class
{
public:
	//! constructor
    CamFlagNaN( CamFrame* camFrm);
	//! destructor
	~CamFlagNaN();
	//! FlagNaN method
	bool* FlagNaN(SRBUF nanBuf);
	bool* GetNaNs();		//!< Returns the PSF image (double)
	

private:
	CamFrame	*_camFrm; // parent main wnd
	//bool*	_imgNaN; // NaN image
	SRBUF savBuf;
	NANBUF _imgNaN;
	
#ifdef FLAGNTIMER
  CPreciseTimer _flagNTimer;	//!< timer for FlagNaN
  float			_timeFlagN_s;	//!< variable to hold FlagNaN time in seconds
#endif

};

