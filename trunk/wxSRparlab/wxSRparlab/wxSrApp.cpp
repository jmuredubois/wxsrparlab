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
//	_camFrm = NULL;
//}

/**
 * This method is called on startup \n
 * - new instances of GUI classes are created here \n \n
 */
bool SrApp::OnInit()
{
	//! - inits private vars to safe defaults \n
	_mainWnd = NULL;
	_numCams = NUMCAMS;
	_camFrm = NULL;

	//! - creates a new main window \n
    _mainWnd = new MainWnd( _T("wxSRparlab"), wxPoint(100,100), wxSize(800,600) );
	if(_mainWnd != NULL){
		_mainWnd->Show(TRUE); // show the main window
	}
	else{
		return FALSE; // return false if creation fails
	} // ENDOF if(_mainWnd != NULL)


	//! - allocates a 1D table of camFrames \n
	_camFrm = (CamFrame**) malloc(_numCams*sizeof(CamFrame*));
	if(_camFrm == NULL){ // crude allocation error checkc
		wxMessageBox(_T("Allocation error"), _T("Malloc error"));
		return FALSE;
	} // ENDOF if(_camFrm == NULL)
	memset( (void*) _camFrm, 0x0, _numCams*sizeof(CamFrame*) );


	// temp variables to avoid creating new camFrames on top of each other
	wxPoint pos = wxPoint(100,100);	// initial position
	wxPoint incr = wxPoint(50,50);	// increment in position
	wxSize	sz = wxSize(400,300);	// size for camFrame
	wxString lab;					// title string for camFrame
	wxString labT;					// title string for camFrame
	
	//! - for the max number of cameras ...
	for(int i = 0; i<_numCams; i++){
		lab.sprintf(wxT("Caméra %i"), i); // ... change title text ...
		labT.sprintf(wxT("Cam %i"), i); // ... change title text ...
		//! ... create and show new camFrame ... \n
		_camFrm[i] = new CamFrame( _mainWnd, lab, pos, sz );
		_camFrm[i]->Show(TRUE);
		_camFrm[i]->CreateAndSetNotebook(labT);
		pos += incr; //... increment position.
	} // ENDOF for loop on _numCams


	_mainWnd->Show(TRUE);
    SetTopWindow(_mainWnd); //! - declares main wnd as top wnd \n
	// success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.

    return TRUE;
} 


/**
 * This method is called on exit \n
 * - instances of GUI classes are closed by wxWidget heritance \n
 */
int SrApp::OnExit()
{
	int res = 0;
	//! - table _camFrm is deallocated here \n
	if(_camFrm != NULL){
		delete(_camFrm);
		_camFrm = NULL;
	}
	return res;
}