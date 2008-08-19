/*
 * CamScattering.cpp
 * Implementation of the scattering compensation methods for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2008.08.26
 */

#include "wxSRparlab.h" //!< top-level header file
#include "CamFrame.h"	//!< main camera header file
#include "CamScattering.h" //!< camera settings panel header file


/**
 * Camera frame class constructor \n
 * Each instance must have a parent frame (usually CMainWnd) \n
 */
CamScattering::CamScattering(CamFrame* camFrm)
{
	_camFrm = camFrm;
	
}

/**
 * Camera frame class destructor \n
 */
CamScattering::~CamScattering()
{
	
}

