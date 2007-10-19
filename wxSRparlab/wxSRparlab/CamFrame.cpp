/**
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


CamFrame::CamFrame(wxFrame* parentFrm, const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(parentFrm, -1, title, pos, size)
{
    CreateStatusBar();
    SetStatusText( _T("cam") );
}

BEGIN_EVENT_TABLE(CamFrame, wxFrame)
    EVT_MENU(ID_Close, CamFrame::OnClose)
END_EVENT_TABLE()

void CamFrame::OnClose(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}