/*
 * CamPanelSettings.h
 * Header of the camera settings panel for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.11.06
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
class CamPanelSettings: public wxPanel //!< Camera frame class
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

	/* Buttons */
	wxRadioBox* m_radioboxFrq;	//!< camera frq
	wxRadioBox* m_radioboxSrFilt;	//!< libusbSR filtering

	/* Text zones */
	wxStaticText* m_statText;
};
