/**
 * CamFrame.h
 * Header of the camera frame for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.10.19
 */

#pragma once
#include "wxSRparlab.h" //!< top-level header file
#include "wxSrApp.h"	//!< application header file
#include "CMainWnd.h"	//!< main window header file

/** the environment variable WXWIN should point to a valid wxWidget 
 *  installation /n
 * "$(WXWIN)/include"    should be added to the include path
 */
#include "wx/image.h" //!< for wxImages


class CamFrame: public wxFrame
{
public:
    CamFrame( wxFrame* parentFrm, const wxString& title, const wxPoint& pos, const wxSize& size);
    void OnClose(wxCommandEvent& event);

public:

    DECLARE_EVENT_TABLE()
};

enum CamFrameEnum
{
    ID_Close = 1
};
