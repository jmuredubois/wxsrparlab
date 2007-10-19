/**
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


bool wxSrApp::OnInit()
{
    MainWnd *frame = new MainWnd( _T("Hello World"), wxPoint(100,100), wxSize(450,340) );
    frame->Show(TRUE);

	CamFrame *cam0 = new CamFrame( (wxFrame*) frame, _T("Cam0"), wxPoint(200,200), wxSize(100, 100) );
	cam0->Show(TRUE);
    SetTopWindow(frame);
    return TRUE;
} 

