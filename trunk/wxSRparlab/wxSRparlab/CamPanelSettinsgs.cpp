/*
 * CamPanelSettings.cpp
 * Implementation of the camera settings panel for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.11.08
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

	m_buttonOpenDev = new wxButton(this, IDB_OpenDev, wxT("OpenSR"));
	m_buttonCloseDev = new wxButton(this, IDB_CloseDev, wxT("CloseSR"));
	m_buttonOpenDev->SetFocus();
	this->DisableCloseSR();
	  wxBoxSizer *sizerButtons = new wxBoxSizer(wxHORIZONTAL);
	    sizerButtons->Add(m_buttonOpenDev, 1, wxEXPAND);
	    sizerButtons->Add(m_buttonCloseDev, 1, wxEXPAND);

	m_buttonSetTrfMat = new wxButton(this, IDB_SetTrfMat, wxT("Cam. trf. matrix"));
	m_buttonAcq = new wxButton(this, IDB_Acquire, wxT("Acquire"));

	#ifdef JMU_TGTFOLLOW 
		m_buttonTgtFile = new wxButton(this, IDB_TgtFile, wxT("Target file")); 
	#endif

	wxString freqs[] = { wxT("19 MHz"), wxT("20 MHz"), wxT("21 MHz"),
        wxT("30 MHz") };

    m_radioboxFrq = new wxRadioBox(this, IDR_Freq, wxT("Choose Frq"),
        wxDefaultPosition, wxDefaultSize, 4, freqs, 0, wxRA_SPECIFY_COLS);

    wxString srFilter[] = { wxT("None"), wxT("Median") };

    m_radioboxSrFilt = new wxRadioBox(this, wxID_ANY,
        wxT("Spatial filter"), wxDefaultPosition, wxDefaultSize,
        2, srFilter, 0, wxRA_SPECIFY_COLS);

	wxString strReadMode[] = { wxT("1 frame"), wxT("Continuous") };

    m_radioboxReadMode = new wxRadioBox(this, IDR_ReadMode, wxT("Read mode"),
        wxDefaultPosition, wxDefaultSize, 2, strReadMode, 2, wxRA_SPECIFY_COLS);

	m_statText = new wxStaticText( this, IDS_TEXT, wxT("0"));

	/* sizer stuff  ...*/
    wxBoxSizer *sizerPanel = new wxBoxSizer(wxVERTICAL);

	sizerPanel->Add(m_buttonAcq, 0, wxEXPAND);
	sizerPanel->Add(sizerButtons, 0, wxEXPAND);
	sizerPanel->Add(m_radioboxReadMode, 0, wxEXPAND);
    sizerPanel->Add(m_radioboxFrq, 0, wxEXPAND);
    sizerPanel->Add(m_radioboxSrFilt, 0, wxEXPAND);
	sizerPanel->Add(m_buttonSetTrfMat, 0, wxEXPAND);
	#ifdef JMU_TGTFOLLOW  
		sizerPanel->Add(m_buttonTgtFile, 0, wxEXPAND); 
	#endif

	sizerPanel->Add(m_statText, 1, wxEXPAND);


    this->SetSizerAndFit(sizerPanel);

	return res;
}

//---------------------------------------------------
/*!
	Used as a debug tool: last cmd
*/
//! Sets the text in SR setting panel
void CamPanelSettings::SetText(wxString text)
{
	m_statText->SetLabel(text);
};


//! Disables the "OpenSR" button
void CamPanelSettings::DisableOpenSR()
{
	m_buttonOpenDev->Disable();
};
//! Enables the "OpenSR" button
void CamPanelSettings::EnableOpenSR()
{
	m_buttonOpenDev->Enable();
};
//! Disables the "CloseSR" button
void CamPanelSettings::DisableCloseSR()
{
	m_buttonCloseDev->Disable();
};
//! Enables the "CloseSR" button
void CamPanelSettings::EnableCloseSR()
{
	m_buttonCloseDev->Enable();
};
//! Disables the frequency radio panel
void CamPanelSettings::DisableRadioFrq()
{
	m_radioboxFrq->Disable();
};
//! Enables the frequency radio panel
void CamPanelSettings::EnableRadioFrq()
{
	m_radioboxFrq->Enable();
};
//!< Disables the filter radio panel
void CamPanelSettings::DisableRadioFilt()
{
	m_radioboxSrFilt->Disable();
};
//!< Enables the filter radio panel
void CamPanelSettings::EnableRadioFilt()
{
	m_radioboxSrFilt->Enable();
};
//!< Disables the read mode radio panel
void CamPanelSettings::DisableRadioReadMode()
{
	m_radioboxReadMode->Disable();
};
//!< Enables the read mode radio panel
void CamPanelSettings::EnableRadioReadMode()
{
	m_radioboxReadMode->Enable();
};

/* Changing interface buttons text */
void CamPanelSettings::SetBtnTxtStop()
{
	if(!m_buttonAcq){return;};
	m_buttonAcq->SetLabel(wxT("Stop"));
	return;
};

/* Changing interface buttons text */
void CamPanelSettings::SetBtnTxtAcqu()
{
	if(!m_buttonAcq){return;};
	m_buttonAcq->SetLabel(wxT("Acquire"));
	return;
};