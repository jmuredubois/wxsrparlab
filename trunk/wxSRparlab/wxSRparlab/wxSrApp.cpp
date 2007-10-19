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
 * - this method is called on startup \n
 * - new instances of GUI classes are created here \n
 */
bool wxSrApp::OnInit()
{
	//! create a new main window
    MainWnd *mainWnd = new MainWnd( _T("Hello World"), wxPoint(100,100), wxSize(450,340) );
    mainWnd->Show(TRUE); //!< show the main window

	//! create a new camera frame
	CamFrame *cam0 = new CamFrame( (wxFrame*) mainWnd, _T("Cam0"), wxPoint(200,200), wxSize(100, 100) );
	cam0->Show(TRUE); //!< show the camera frame


    SetTopWindow(mainWnd); //!< declares main wnd as top wnd
    return TRUE;
} 

