/*
 * CamPanelSettings.h
 * Header of the camera settings panel for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.11.13
 */

#pragma once
#include "wxSRparlab.h" //!< top-level header file
#include "wxSrApp.h"	//!< application header file
#include "CMainWnd.h"	//!< main window header file
#include "CamFrame.h"	//!< cam  window header file



/**
 * Camera settings panel class \n
 * This class: \n
 * - displays GUI elements allowing to modify camera settings \n
 */
class CamPanelSettings: public wxPanel //!< Camera settings panel
{
public:
	//! constructor
    CamPanelSettings( wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size);
	//! init settings
	int InitSettings();
	void SetText(wxString text); //!< Sets the text in SR setting panel

	/* Opening and closing */
	void DisableOpenSR(); //!< Disables the "OpenSR" button
	void EnableOpenSR();  //!< Enables the "OpenSR" button
	void DisableCloseSR(); //!< Disables the "CloseSR" button
	void EnableCloseSR();  //!< Enables the "CloseSR" button

	/* Disabling and enabling other controls */
	void CamPanelSettings::DisableRadioFrq(); //!< Disables the frequency radio panel
	void CamPanelSettings::EnableRadioFrq();  //!< Enables the frequency radio panel
	void CamPanelSettings::DisableRadioFilt(); //!< Disables the filter radio panel
	void CamPanelSettings::EnableRadioFilt();  //!< Enables the fliter radio panel


private:

	/* Buttons */
	wxButton* m_buttonOpenDev; //!< Open camera
	wxButton* m_buttonCloseDev; //!< Close camera

	/* Radio boxes */
	wxRadioBox* m_radioboxFrq;	//!< camera frq
	wxRadioBox* m_radioboxSrFilt;	//!< libusbSR filtering
	
	/* Check boxes */
	wxCheckBox* m_ckBoxFixPtrn;
	wxCheckBox* m_ckBoxLedNonLin;
	
	/* TextControls */
	wxTextCtrl* m_TxtDelayStat;
	wxTextCtrl* m_TxtDelayDin;
	wxTextCtrl* m_TxtScatSzX;
	wxTextCtrl* m_TxtScatSzy;
	wxTextCtrl* m_TxtRegAddr;
	wxTextCtrl* m_TxtRegValue;
	wxTextCtrl* m_TxtAmpThr;
	wxTextCtrl* m_TxtSatThr;
	wxTextCtrl* m_TxtIntegrTime;

	/* Text zones */
	wxStaticText* m_statText;
	wxStaticText* m_stTxtVersion;
	wxStaticText* m_stTxtbufSize;
	wxStaticText* m_stTxtRows;
	wxStaticText* m_stTxtCols;
	wxStaticText* m_stTxtNumImg;
	wxStaticText* m_stTxtBytePerPix;
	wxStaticText* m_stTxtDevId;
	wxStaticText* m_stTxtSerial;
	
	/*Auto illumination*/
	wxCheckBox* m_ckBoxAutoIllum;
	wxTextCtrl* m_TxtIntTimeMin;
	wxTextCtrl* m_TxtIntTimeMax;
	wxTextCtrl* m_TxtPercentOver;
	wxTextCtrl* m_TxtDesiredPos;
	
};
