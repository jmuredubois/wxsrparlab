/*
 * CamFrame.h
 * Header of the camera frame for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.10.24
 */

#pragma once
#include "wxSRparlab.h" //!< top-level header file
#include "wxSrApp.h"	//!< application header file
#include "CMainWnd.h"	//!< main window header file
#include "CamPanelSettings.h" //!< camera settings panel header file


/** the environment variable WXWIN should point to a valid wxWidget 
 *  installation /n
 * "$(WXWIN)/include"    should be added to the include path
 */
#include "wx/image.h" //!< for wxImages


/**
 * Camera frame class \n
 * This class: \n
 * - displays the camera image (s) \n
 * - displays GUI elements allowing to modify the display \n
 * - displays GUI elements allowing to modify camera settings \n
 */
class CamFrame: public wxFrame //!< Camera frame class
{
public:
	//! constructor
    CamFrame( wxFrame* parentFrm, const wxString& title, const wxPoint& pos, const wxSize& size);
	//! Close() method
    void OnClose(wxCommandEvent& event);
	int CreateAndSetNotebook(const wxString& title); //!< creates a new Notebook

private:
	wxNotebook* _camNB; //!< a notebook interface

public:
    DECLARE_EVENT_TABLE()
};

//! enum used by the CamFrame class
enum CamFrameEnum
{
    ID_Close = 1 ,
	IDB_CLOSE  =2 
};
