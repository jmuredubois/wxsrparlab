/*
 * wxSRapp.cpp
 * Implementation of the main window for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.10.19
 */

#include "wxSRparlab.h" //!< top-level header file
#include "wxSrApp.h"	//!< this class header file
#include "CMainWnd.h"	//!< main window class header
#include "CamFrame.h"	//!< camera frame header file


/**
 * Main window construction \n
 * - the main window is derived from wxFrame \n
 * - it has no parent \n
 *
 */
SrApp::SrApp()
: wxApp()
{
    _mainWnd = NULL;
	_numCams = NUMCAMS;
	_camFrm = NULL;
}

/**
 * - this method is called on startup \n
 * - new instances of GUI classes are created here \n
 */
bool SrApp::OnInit()
{
	//! create a new main window
    _mainWnd = new MainWnd( _T("wxSRparlab"), wxPoint(100,100), wxSize(450,340) );
	if(_mainWnd != NULL){
		_mainWnd->Show(TRUE); //!< show the main window
	}
	else{
		return FALSE; // return false if creation fails
	}

	_camFrm = (CamFrame**) malloc(_numCams*sizeof(CamFrame*));
	if(_camFrm == NULL){
		wxMessageBox(_T("Allocation error"), _T("Malloc error"));
		return FALSE;
	}

	wxPoint pos = wxPoint(200,200);
	wxPoint incr = wxPoint(50,50);
	wxSize	sz = wxSize(100,100);
	wxString lab;
	
	for(int i = 0; i<_numCams; i++){
		lab.sprintf(wxT("Caméra %i"), i);
		_camFrm[i] = new CamFrame( (wxFrame*) _mainWnd, lab, pos, sz );
		_camFrm[i]->Show(TRUE);
		pos += incr;
	}


    SetTopWindow(_mainWnd); //!< declares main wnd as top wnd
    return TRUE;
} 

/**
 * - this method is called on close \n
 * - instances of GUI classes are closed here \n
 */
SrApp::~SrApp()
{
	if(_camFrm != NULL){
		delete(_camFrm);
		_camFrm = NULL;
	}
} 

