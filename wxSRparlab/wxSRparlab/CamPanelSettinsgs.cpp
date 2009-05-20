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
	m_buttonDevSRsettings = new wxButton(this, IDB_DevSRsettings, wxT("SR settings"));
#ifdef DUMMYSR
	m_buttonDevSRsettings->Disable();
#endif
	m_buttonOpenDev->SetFocus();
	this->DisableCloseSR();
	  wxBoxSizer *sizerButtons = new wxBoxSizer(wxHORIZONTAL);
	    sizerButtons->Add(m_buttonOpenDev, 1, wxEXPAND);
	    sizerButtons->Add(m_buttonCloseDev, 1, wxEXPAND);

	m_buttonSetTrfMat = new wxButton(this, IDB_SetTrfMat, wxT("Cam. trf. matrix"));
	m_buttonAcq = new wxButton(this, IDB_Acquire, wxT("Acquire"));

	wxBoxSizer *sizerButtons2 = new wxBoxSizer(wxHORIZONTAL);
	    sizerButtons2->Add(m_buttonAcq, 2, wxEXPAND);
	    sizerButtons2->Add(m_buttonDevSRsettings, 1, wxEXPAND);

	#ifdef JMU_TGTFOLLOW 
		m_buttonTgtFile = new wxButton(this, IDB_TgtFile, wxT("Target file")); 
	#endif

	wxString freqs[] = { wxT("19 MHz"), wxT("20 MHz"), wxT("21 MHz"),
        wxT("30 MHz") };

    m_radioboxFrq = new wxRadioBox(this, IDR_Freq, wxT("Choose Frq"),
        wxDefaultPosition, wxDefaultSize, 4, freqs, 0, wxRA_SPECIFY_COLS);

    wxString srFilter[] = { wxT("None"), wxT("Median") };
    
	// Scat comp controls //
    m_ckBoxScatComp = new wxCheckBox( this, IDC_ScatComp, wxT("Scattering compensation"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
        
    m_buttonScatParams = new wxButton(this, IDB_SetScatParams, wxT("Set scat. comp. params"));
	this->DisableScatParams(); // button is disabled until cam is opened

	wxBoxSizer *sizerScat = new wxBoxSizer(wxHORIZONTAL);
		sizerScat->Add(m_ckBoxScatComp, 0, wxEXPAND);
		sizerScat->Add(m_buttonScatParams, 1, wxEXPAND);

	// Segmentation controls //
    m_ckBoxSegBayes = new wxCheckBox( this, IDC_SegmBayes, wxT("Bayesian segmentation"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
        
    m_buttonSegmParams = new wxButton(this, IDB_SegmSetParams, wxT("Set segm. params"));
	this->DisableSegmParams(); // button is disabled until cam is opened

	wxBoxSizer *sizerSegm = new wxBoxSizer(wxHORIZONTAL);
		sizerSegm->Add(m_ckBoxSegBayes, 0, wxEXPAND);
		sizerSegm->Add(m_buttonSegmParams, 1, wxEXPAND);

	// Learn background controls //
    m_ckBoxLrnBg = new wxCheckBox( this, IDC_LrnBg, wxT("Learn background"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	m_buttonClearBg = new wxButton(this, IDB_ClearBg, wxT("Clear BG."));

	wxBoxSizer *sizerLrnBg = new wxBoxSizer(wxHORIZONTAL);
		sizerLrnBg->Add(m_ckBoxLrnBg, 0, wxEXPAND);
		sizerLrnBg->Add(m_buttonClearBg, 1, wxEXPAND);

	// Learn foreground controls //
    m_ckBoxLrnFg = new wxCheckBox( this, IDC_LrnFg, wxT("Learn foreground"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	m_buttonClearFg = new wxButton(this, IDB_ClearBg, wxT("Clear FG."));
	m_ckBoxFgHidesData = new wxCheckBox( this, IDC_FgHidesData, wxT("Foreground replaces data"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);

	wxBoxSizer *sizerLrnFg = new wxBoxSizer(wxHORIZONTAL);
		sizerLrnFg->Add(m_ckBoxLrnFg, 0, wxEXPAND);
		sizerLrnFg->Add(m_buttonClearFg, 1, wxEXPAND);

	// DisableFlagNaN controls
	m_ckBoxNoFlagNaN = new wxCheckBox( this, IDC_NoFlagNaN, wxT(" Disable NaN filtering"),
		wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);


	// Frequency controls
    m_radioboxSrFilt = new wxRadioBox(this, wxID_ANY,
        wxT("Spatial filter"), wxDefaultPosition, wxDefaultSize,
        2, srFilter, 0, wxRA_SPECIFY_COLS);

	wxString strReadMode[] = { wxT("1 frame"), wxT("Continuous") };

    m_radioboxReadMode = new wxRadioBox(this, IDR_ReadMode, wxT("Read mode"),
        wxDefaultPosition, wxDefaultSize, 2, strReadMode, 2, wxRA_SPECIFY_COLS);

	m_statText = new wxStaticText( this, IDS_TEXT, wxT("0"));

	// Scat comp controls //
    m_ckBoxRecord = new wxCheckBox( this, IDC_Record, wxT("Record"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);

	/* sizer stuff  ...*/
    wxBoxSizer *sizerPanel = new wxBoxSizer(wxVERTICAL);

	sizerPanel->Add(sizerButtons2, 0, wxEXPAND);
	sizerPanel->Add(sizerButtons, 0, wxEXPAND);
	sizerPanel->Add(m_radioboxReadMode, 0, wxEXPAND);
    sizerPanel->Add(m_radioboxFrq, 0, wxEXPAND);
	sizerPanel->Add(sizerLrnBg, 0, wxEXPAND);
    sizerPanel->Add(sizerScat, 0, wxEXPAND);
	sizerPanel->Add(sizerSegm, 0, wxEXPAND);
    sizerPanel->Add(m_radioboxSrFilt, 0, wxEXPAND);
	sizerPanel->Add(m_buttonSetTrfMat, 0, wxEXPAND);
	#ifdef JMU_TGTFOLLOW  
		sizerPanel->Add(m_buttonTgtFile, 0, wxEXPAND); 
	#endif
	sizerPanel->Add(m_ckBoxRecord, 0, wxEXPAND);
	sizerPanel->Add(sizerLrnFg, 0, wxEXPAND);
	sizerPanel->Add(m_ckBoxFgHidesData, 0, wxEXPAND);
	sizerPanel->Add(m_ckBoxNoFlagNaN, 0, wxEXPAND);
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

//! Disables the "Scat Comp Params" button
void CamPanelSettings::DisableScatParams()
{
	m_buttonScatParams->Disable();
};
//! Enables the "Scat Comp Params" button
void CamPanelSettings::EnableScatParams()
{
	m_buttonScatParams->Enable();
};

//! Disables the "Segm Params" button
void CamPanelSettings::DisableSegmParams()
{
	m_buttonSegmParams->Disable();
};
//! Enables the "Segm Params" button
void CamPanelSettings::EnableSegmParams()
{
	m_buttonSegmParams->Enable();
};