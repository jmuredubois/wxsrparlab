/*
 * CMainWnd.h
 * Header of the main window for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.11.06
 */

#pragma once
#include "wxSRparlab.h" // top-level header file
#include "CMainWnd.h"	// main window class header
#include "CamFrame.h"	// camera frame header file

/*
 * Required class declarations
 */
class SrApp;		// main application
class MainWnd;		// main window
class CamFrame;		// camera frame

/**
 * Main application class
 */
class SrApp: public wxApp	//! Main application class
{
public:
	//SrApp::SrApp();		//!< constructor
    virtual bool OnInit();	//!< method called on initialization
	virtual int  OnExit();	//!< method called on exit

private:
	MainWnd		*_mainWnd;		//!< pointer to main window
	int			_numCams;		//!< count of cameras
	CamFrame	**_camFrm;		//!< pointer to array of camFrames
};