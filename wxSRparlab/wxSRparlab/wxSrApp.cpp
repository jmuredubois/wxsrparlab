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
	_numCams = NUMCAMS;
	_vtkWin = NULL;

	//! - creates a new main window \n
    _mainWnd = new MainWnd( _T("wxSRparlab"), wxPoint(050,050), wxSize(250,100) );
	if(_mainWnd != NULL){
		_mainWnd->Init();
		_mainWnd->Show(TRUE); // show the main window
	}
	else{
		return FALSE; // return false if creation fails
	} // ENDOF if(_mainWnd != NULL)


	// temp variables to avoid creating new camFrames on top of each other
	wxPoint pos = wxPoint(50,150);	// initial position
	wxPoint incr = wxPoint(220,00);	// increment in position
	wxSize	sz = wxSize(220,400);	// size for camFrame
	wxString lab;					// title string for camFrame
	wxString labT;					// title string for camFrame

  #ifdef JMU_USE_VTK
	_vtkWin = new CViewSrVtk(NULL);
	_mainWnd->SetVtkWin(_vtkWin);
  #endif

	//! - for the max number of cameras ...
	for(int i = 0; i<_numCams; i++){
		lab.sprintf(wxT("Camera %i"), i); // ... change title text ...
		labT.sprintf(wxT("Cam %i"), i); // ... change title text ...
		//! ... create and show new camFrame ... \n
		CamFrame *camFrm = new CamFrame( _mainWnd, lab, pos, sz );
		camFrm->Show(TRUE);
		camFrm->CreateAndSetNotebook(labT);
#ifdef JMU_USE_VTK
		camFrm->SetVtkWin(_vtkWin, i);
#endif
		m_camFrm.push_back(camFrm);
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
	#ifdef JMU_USE_VTK
	if(_vtkWin){ delete(_vtkWin); _vtkWin =NULL; };
	#endif
	return res;
}