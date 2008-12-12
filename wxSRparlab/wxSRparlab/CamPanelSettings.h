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
	void CamPanelSettings::DisableRadioReadMode(); //!< Disables the read mode radio panel
	void CamPanelSettings::EnableRadioReadMode();  //!< Enables the read mode radio panel

	void CamPanelSettings::SetBtnTxtStop();	//!< sets button text to 'Stop'
	void CamPanelSettings::SetBtnTxtAcqu();	//!< sets button text to 'Acquire'
	bool IsScatChecked(){return m_ckBoxScatComp->IsChecked();}; //
	bool IsRecordChecked(){return m_ckBoxRecord->IsChecked();}; //

	void CamPanelSettings::DisableScatParams(); //!< Disables the "Scat Comp Params" button
	void CamPanelSettings::EnableScatParams();  //!< Enables the "Scat Comp Params" button
	bool IsLrnBgChecked(){return m_ckBoxLrnBg->IsChecked();}; //


private:

	/* Buttons */
	wxButton* m_buttonOpenDev; //!< Open camera
	wxButton* m_buttonCloseDev; //!< Close camera
	wxButton* m_buttonSetTrfMat; //!< Set camera trf Mat
	wxButton* m_buttonAcq; //!< Acquire button
	#ifdef JMU_TGTFOLLOW  
	  wxButton* m_buttonTgtFile; 
	#endif
	wxButton* m_buttonScatParams;
	wxButton* m_buttonClearBg;

	/* Radio boxes */
	wxRadioBox* m_radioboxFrq;	//!< camera frq
	wxRadioBox* m_radioboxSrFilt;	//!< libusbSR filtering
	wxRadioBox* m_radioboxReadMode; //!< Read mode
	
	/* Check boxes */
	wxCheckBox* m_ckBoxFixPtrn;
	wxCheckBox* m_ckBoxLedNonLin;
	
	wxCheckBox* m_ckBoxScatComp;
	wxCheckBox* m_ckBoxLrnBg;
	wxCheckBox* m_ckBoxRecord;
	
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
