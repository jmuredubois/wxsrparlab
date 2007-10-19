/**
 * CMainWnd.h
 * Header of the main window for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.10.19
 */

#pragma once
#include "wxSRparlab.h" // top-level header file
#include "wxSrApp.h"	//!< this class header file

/** the environment variable WXWIN should point to a valid wxWidget 
 *  installation /n
 * "$(WXWIN)/include"    should be added to the include path
 */
#include "wx/image.h" //!< for wxImages


class MainWnd: public wxFrame
{
public:

    MainWnd(const wxString& title, const wxPoint& pos, const wxSize& size);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

enum MainWndEnum
{
    ID_Quit = 1,
    ID_About,
};
