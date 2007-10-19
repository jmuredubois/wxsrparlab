/*
 * CamFrame.cpp
 * Implementation of the camera frame for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.10.19
 */

#include "wxSRparlab.h" //!< top-level header file
#include "CMainWnd.h"	//!< main window header file
#include "wxSrApp.h"	//!< application header file
#include "CamFrame.h"	//!< camera frame header file

/**
 * Camera frame class constructor \n
 * Each instance: \n
 * - must have a parent frame (usually CMainWnd) \n
 */
CamFrame::CamFrame(wxFrame* parentFrm, const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(parentFrm, -1, title, pos, size)
{
    CreateStatusBar();
    SetStatusText( _T("cam") );
}

BEGIN_EVENT_TABLE(CamFrame, wxFrame)
    EVT_MENU(ID_Close, CamFrame::OnClose)
END_EVENT_TABLE()

/**
 * Camera frame close \n
 * The method: \n
 * - calls the wxWindowBase Close() method \n
 */
void CamFrame::OnClose(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}