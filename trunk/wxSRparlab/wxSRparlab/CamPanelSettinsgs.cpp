/*
 * CamPanelSettings.cpp
 * Implementation of the camera settings panel for wxSRparlab
 *
 *    Copyright 2009 James Mure-Dubois, Heinz Hügli and Institute of Microtechnology of EPFL.
 *      http://parlab.epfl.ch/
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $Rev$:
 * $Author$:
 * $Date$:
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
CamPanelSettings::CamPanelSettings(wxWindow* parent, CamFrame* pCamFrame, const wxString& title, const wxPoint& pos, const wxSize& size)
: wxPanel(parent, wxID_ANY, pos, size, wxBORDER_NONE, title)
{
	_pCamFrame = pCamFrame;
	#ifdef JMU_RANSAC
		m_TxtRansacNiterMax = NULL;
		_ransacNiterMax = 3000;
		m_TxtRansacDistPla  = NULL;
		_ransacDistPla = 25.0;
	#endif
	#ifdef JMU_USE_VTK
		m_ckBoxBlankSegmVTK = NULL;
		m_TxtBlankSegmThr = NULL;
	#endif
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
    m_ckBoxSegBayes = new wxCheckBox( this, IDC_SegmBayes, wxT("Flat segm."),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    m_buttonSegmParams = new wxButton(this, IDB_SegmSetParams, wxT("Set segm. params"));
	this->DisableSegmParams(); // button is disabled until cam is opened

	wxBoxSizer *sizerSegm = new wxBoxSizer(wxHORIZONTAL);
		sizerSegm->Add(m_ckBoxSegBayes, 0, wxEXPAND);
		sizerSegm->Add(m_buttonSegmParams, 1, wxEXPAND);

	#ifdef JMU_USE_VTK
	// Blank segmentation results controls //
    m_ckBoxBlankSegmVTK = new wxCheckBox( this, IDC_BlankSegmVtk, wxT("Blank segm. in VTK"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    m_TxtBlankSegmThr = new wxTextCtrl( this, IDT_BlankSegmThr, wxString::Format(wxT("%i"),000) );

	wxBoxSizer *sizerBlank = new wxBoxSizer(wxHORIZONTAL);
		sizerBlank->Add(m_ckBoxBlankSegmVTK, 0, wxEXPAND);
		sizerBlank->Add(m_TxtBlankSegmThr, 1, wxEXPAND);
	#endif // JMU_USE_VTK

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

	// Record controls //
    m_ckBoxRecord = new wxCheckBox( this, IDC_Record, wxT("Record"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	m_ckBoxRecordXYZ = new wxCheckBox( this, IDC_RecordXYZ, wxT("Record XYZ"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	m_ckBoxRecordSeg = new wxCheckBox( this, IDC_RecordSeg, wxT("Record Seg"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	m_ckBoxRecordVTK = new wxCheckBox( this, IDC_RecordVTK, wxT("Record VTK"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	
	wxBoxSizer *sizerRecord = new wxBoxSizer(wxHORIZONTAL);
		sizerRecord->Add(m_ckBoxRecord, 0, wxEXPAND);
		sizerRecord->Add(m_ckBoxRecordXYZ, 0, wxEXPAND);
		sizerRecord->Add(m_ckBoxRecordSeg, 0, wxEXPAND);
		sizerRecord->Add(m_ckBoxRecordVTK, 0, wxEXPAND);

	// RANSAC controls //
	#ifdef JMU_RANSAC
		wxStaticText* ransacText = new wxStaticText(this, wxID_ANY, wxT("RANSAC"));
		m_buttonRansacBG = new wxButton(this, IDB_RansacBG, wxT("BG."));
		m_buttonRansacBGsucc = new wxButton(this, IDB_RansacBGsucc, wxT("bg N"));
		m_buttonRansacFG = new wxButton(this, IDB_RansacFG, wxT("FG."));
		m_TxtRansacNiterMax = new wxTextCtrl( this, IDT_RansacNiterMax, wxString::Format(wxT("%i"),_ransacNiterMax) ); 
		wxStaticText* rscIterLabel = new wxStaticText( this, wxID_ANY, wxT("Niter:"));
		m_TxtRansacDistPla  = new wxTextCtrl( this, IDT_RansacDistPla,  wxString::Format(wxT("%g"),_ransacDistPla) );
		wxStaticText* rscDistLabel = new wxStaticText( this, wxID_ANY, wxT("distThr:"));
		wxBoxSizer *sizerRansac = new wxBoxSizer(wxHORIZONTAL);
		sizerRansac->Add(ransacText, 0, wxEXPAND);
	    sizerRansac->Add(m_buttonRansacBG, 1, wxEXPAND);
		sizerRansac->Add(m_buttonRansacBGsucc, 1, wxEXPAND);
	    sizerRansac->Add(m_buttonRansacFG, 1, wxEXPAND);
		sizerRansac->Add(rscIterLabel, 0, wxEXPAND);
		sizerRansac->Add(m_TxtRansacNiterMax, 1, wxEXPAND);
		sizerRansac->Add(rscDistLabel, 0, wxEXPAND);
		sizerRansac->Add(m_TxtRansacDistPla, 1, wxEXPAND);
	#endif

	/* sizer stuff  ...*/
    wxBoxSizer *sizerPanel = new wxBoxSizer(wxVERTICAL);

	sizerPanel->Add(sizerButtons2, 0, wxEXPAND);
	sizerPanel->Add(sizerButtons, 0, wxEXPAND);
	sizerPanel->Add(m_radioboxReadMode, 0, wxEXPAND);
    sizerPanel->Add(m_radioboxFrq, 0, wxEXPAND);
	sizerPanel->Add(sizerLrnBg, 0, wxEXPAND);
    sizerPanel->Add(sizerScat, 0, wxEXPAND);
	sizerPanel->Add(sizerSegm, 0, wxEXPAND);
	#ifdef JMU_USE_VTK
		sizerPanel->Add(sizerBlank, 0, wxEXPAND);
	#endif
    sizerPanel->Add(m_radioboxSrFilt, 0, wxEXPAND);
	sizerPanel->Add(m_buttonSetTrfMat, 0, wxEXPAND);
	#ifdef JMU_TGTFOLLOW  
		sizerPanel->Add(m_buttonTgtFile, 0, wxEXPAND); 
	#endif
	sizerPanel->Add(sizerRecord, 0, wxEXPAND);
	sizerPanel->Add(sizerLrnFg, 0, wxEXPAND);
	sizerPanel->Add(m_ckBoxFgHidesData, 0, wxEXPAND);
	sizerPanel->Add(m_ckBoxNoFlagNaN, 0, wxEXPAND);
	#ifdef JMU_RANSAC
		sizerPanel->Add(sizerRansac, 0, wxEXPAND);
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

#ifdef JMU_RANSAC
//! Reads the value in the text control for number of RANSAC iterations
int CamPanelSettings::GetRansacNiterMax()
{
	if(!m_TxtRansacNiterMax){return -1;};
	long val = 0;
	wxString strVal = m_TxtRansacNiterMax->GetValue();
	if( strVal.ToLong(&val) ) /* read value as int*/
	{
		_ransacNiterMax = (int) val;
	}
	else
	{
		m_TxtRansacNiterMax->DiscardEdits();
		return -1;
		m_TxtRansacNiterMax->GetValue().Printf(wxT("%i"), _ransacNiterMax);
	}
	return (int)_ransacNiterMax;
};
#endif // JMU_RANSAC
#ifdef JMU_RANSAC
//! Reads the value in the text control for number of RANSAC iterations
double CamPanelSettings::GetRansacDistPla()
{
	if(!m_TxtRansacDistPla){return -1;};
	double val = 0;
	wxString strVal = m_TxtRansacDistPla->GetValue();
	if( strVal.ToDouble(&val) ) /* read value as int*/
	{
		_ransacDistPla = val;
	}
	else
	{
		m_TxtRansacDistPla->DiscardEdits();
		return -1;
		m_TxtRansacDistPla->GetValue().Printf(wxT("%g"), _ransacDistPla);
	}
	return _ransacDistPla;
};
#endif // JMU_RANSAC

#ifdef JMU_USE_VTK
//! Reads the value in the text control for number of RANSAC iterations
unsigned char CamPanelSettings::GetBlankSegmThr()
{
	if(!m_TxtBlankSegmThr){return 0;};
	long val = 0;
	wxString strVal = m_TxtBlankSegmThr->GetValue();
	if( strVal.ToLong(&val) ) /* read value as int*/
	{
		//SetAmpMax(val);
	}
	else
	{
		m_TxtBlankSegmThr->DiscardEdits();
		return 0;
		//m_TxtRansacIterMax->GetValue().Printf(wxT("%d"), _ampMax);
	}
	return (unsigned char)val;
};
#endif