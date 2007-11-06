/*
 * CamPanelSettings.cpp
 * Implementation of the camera settings panel for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.10.24
 */

#include "wxSRparlab.h" //!< top-level header file
#include "CMainWnd.h"	//!< main window header file
#include "wxSrApp.h"	//!< application header file
#include "CamFrame.h"	//!< camera frame header file
#include "CamPanelSettings.h" //!< camera settings panel header file

/**
 * Camera settings panel class constructor \n
 * Each instance must have a parent wnd (usually a notebook) \n
 */
CamPanelSettings::CamPanelSettings(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size)
: wxPanel(parent, wxID_ANY, pos, size, wxBORDER_NONE, title)
{
	
}

/**
 * 
 */
int CamPanelSettings::InitSettings()
{
	int res = 0;

	wxButton *button0 = new wxButton(this, wxID_ANY, wxT("Button"));

	wxButton *buttonExit = new wxButton(this, IDB_CLOSE, wxT("Close"));

	wxString freqs[] = { wxT("19 MHz"), wxT("20 MHz"), wxT("21 MHz"),
        wxT("30 MHz") };

    wxRadioBox *radioboxFrq = new wxRadioBox(this, wxID_ANY, wxT("Choose Frq"),
        wxDefaultPosition, wxDefaultSize, 4, freqs, 2, wxRA_SPECIFY_ROWS);

    wxString srFilter[] = { wxT("None"), wxT("Median") };

    wxRadioBox *radioboxSrFilt = new wxRadioBox(this, wxID_ANY,
        wxT("Spatial filter"), wxDefaultPosition, wxDefaultSize,
        2, srFilter, 0, wxRA_SPECIFY_COLS);

    wxBoxSizer *sizerPanel = new wxBoxSizer(wxVERTICAL);

    sizerPanel->Add(radioboxFrq, 2, wxEXPAND);
    sizerPanel->Add(radioboxSrFilt, 1, wxEXPAND);
    sizerPanel->Add(button0, 1, wxEXPAND);
	sizerPanel->Add(buttonExit, 1, wxEXPAND);


    this->SetSizer(sizerPanel);

	return res;
}

