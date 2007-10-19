/*
 * CMainWnd.h
 * Header of the main window for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.10.19
 */

#pragma once
#include "wxSRparlab.h" //!< top-level header file
#include "wxSrApp.h"	//!< this class header file


/**
 * Main window class \n
 * This class: \n
 * - contains the main wnd GUI \n
 */
class MainWnd: public wxFrame //!< main window class
{
public:
	//!Constructor
    MainWnd(const wxString& title, const wxPoint& pos, const wxSize& size); 

	//! Quit(..) method
    void OnQuit(wxCommandEvent& event);
	//! About(..) method
    void OnAbout(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

//! enum used by main wnd
enum MainWndEnum
{
    ID_Quit = 1,
    ID_About,
};
