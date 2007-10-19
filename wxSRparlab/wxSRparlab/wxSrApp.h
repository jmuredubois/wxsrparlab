/*
 * CMainWnd.h
 * Header of the main window for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.10.19
 */

#pragma once
#include "wxSRparlab.h" //!< top-level header file
#include "CMainWnd.h"	//!< main window class header
#include "CamFrame.h"	//!< camera frame header file

/*
 * Derived class
 */
class SrApp;
class MainWnd;
class CamFrame;

/**
 * Main application class
 */
class SrApp: public wxApp	//! Main application class
{
public:
	SrApp::SrApp();		//constructor
	~SrApp();	//destructor
	//! method called on initialization
    virtual bool OnInit();
	//virtual bool OnQuit();

private:
	MainWnd		*_mainWnd;
	int			_numCams;		// count of cameras
	CamFrame	**_camFrm;
};