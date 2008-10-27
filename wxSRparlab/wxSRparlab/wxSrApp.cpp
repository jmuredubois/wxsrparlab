/*
 * wxSRapp.cpp
 * Implementation of the main window for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.10.24
 */

#include "wxSRparlab.h" //!< top-level header file
#include "wxSrApp.h"	//!< this class header file
#include "CMainWnd.h"	//!< main window class header
#include "CamFrame.h"	//!< camera frame header file


/**
 * Main application construction \n
 * - the main app is derived from wxApp \n
 * - a pointer to the main wnd is initialized as null \n
 * - a pointer^2 to camFrames is initialized as null \n
 * - the _numCams variable is initialized to the global value NUMCAMS \n
 *
 */
//SrApp::SrApp()
//: wxApp()
//{
//    _mainWnd = NULL;
//	_numCams = NUMCAMS;
//}

/**
 * This method is called on startup \n
 * - new instances of GUI classes are created here \n \n
 */
bool SrApp::OnInit()
{
	//! - inits private vars to safe defaults \n
	_mainWnd = NULL;

	//! - creates a new main window \n
    _mainWnd = new MainWnd( _T("wxSRparlab"), wxPoint(050,050), wxSize(300,150) );
	if(_mainWnd != NULL){
		_mainWnd->Init();
		_mainWnd->Show(TRUE); // show the main window
	}
	else{
		return FALSE; // return false if creation fails
	} // ENDOF if(_mainWnd != NULL)

	_mainWnd->AddChildren();
	_mainWnd->Layout();
	_mainWnd->Fit();
	_mainWnd->Refresh();
	_mainWnd->Update();
	_mainWnd->Show(TRUE);
    SetTopWindow(_mainWnd); //! - declares main wnd as top wnd \n
	// success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
	_mainWnd->SendSizeEvent();
    return TRUE;
} 


/**
 * This method is called on exit \n
 * - instances of GUI classes are closed by wxWidget heritance \n
 */
int SrApp::OnExit()
{
	int res = 0;
	return res;
}