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
	bool IsRecXYZChecked(){return m_ckBoxRecordXYZ->IsChecked();}; //
	bool IsRecSegChecked(){return m_ckBoxRecordSeg->IsChecked();}; //
	bool IsRecVTKChecked(){return m_ckBoxRecordVTK->IsChecked();}; //
	#ifdef JMU_USE_VTK
		bool IsBlankSegmVTKChecked(){return m_ckBoxBlankSegmVTK->IsChecked();}; //!< helper fct for "Blank segmentation" checkbox
		unsigned char GetBlankSegmThr(); //!< Reads the value in the text control for number of RANSAC iterations
	#endif // JMU_USE_VTK

	void CamPanelSettings::DisableScatParams(); //!< Disables the "Scat Comp Params" button
	void CamPanelSettings::EnableScatParams();  //!< Enables the "Scat Comp Params" button
	bool IsLrnBgChecked(){return m_ckBoxLrnBg->IsChecked();}; //
	bool IsLrnFgChecked(){return m_ckBoxLrnFg->IsChecked();}; //
	bool IsFgHidesDataChecked(){return m_ckBoxFgHidesData->IsChecked();}; //
	bool IsNoFlagNaNChecked(){return m_ckBoxNoFlagNaN->IsChecked();}; //

	void CamPanelSettings::DisableSegmParams(); //!< Disables the "Segm Params" button
	void CamPanelSettings::EnableSegmParams();  //!< Enables the "Segm Params" button
	bool IsSegmChecked(){return m_ckBoxSegBayes->IsChecked();}; //
	#ifdef JMU_RANSAC
		int GetRansacNiterMax();
		double GetRansacDistPla();
	#endif


private:

	/* Buttons */
	wxButton* m_buttonOpenDev; //!< Open camera
	wxButton* m_buttonCloseDev; //!< Close camera
	wxButton* m_buttonDevSRsettings; //!< button to call Swissranger settings dialog box (windows only)
	wxButton* m_buttonSetTrfMat; //!< Set camera trf Mat
	wxButton* m_buttonAcq; //!< Acquire button
	#ifdef JMU_TGTFOLLOW  
	  wxButton* m_buttonTgtFile; 
	#endif
	wxButton* m_buttonScatParams;
	wxButton* m_buttonClearBg; // 'clear background' button
	wxButton* m_buttonClearFg; // 'clear foreground' button
	wxButton* m_buttonSegmParams; // segmentation parameters button
	#ifdef JMU_RANSAC
	  wxButton* m_buttonRansacBG; // call RANSAC on bg data
	  wxButton* m_buttonRansacBGsucc; // call successive RANSAC on bg data
	  wxButton* m_buttonRansacFG; // call RANSAC of fg data
	  wxTextCtrl* m_TxtRansacNiterMax; // RANSAC max number of iterations
	  int _ransacNiterMax; // RANSAC max number of iterations
	  wxTextCtrl* m_TxtRansacDistPla; // RANSAC max number of iterations
	  double _ransacDistPla; // RANSAC max number of iterations
	  //bool _TxtRansacNiterMaxInit;
	#endif

	/* Radio boxes */
	wxRadioBox* m_radioboxFrq;	//!< camera frq
	wxRadioBox* m_radioboxSrFilt;	//!< libusbSR filtering
	wxRadioBox* m_radioboxReadMode; //!< Read mode
	
	/* Check boxes */
	wxCheckBox* m_ckBoxFixPtrn;  // check box for fix pattern noise filter -> NOT USED
	wxCheckBox* m_ckBoxLedNonLin; // check box for nonLinLED noise filter -> NOT USED
	
	wxCheckBox* m_ckBoxScatComp; //!< check box for activating scattering compensation
	wxCheckBox* m_ckBoxLrnBg;	 //!< check box for learning background
	wxCheckBox* m_ckBoxLrnFg;	 //!< check box for learning foreground
	wxCheckBox* m_ckBoxRecord;	 //!< check box to record current image buffer (after scat comp)
	wxCheckBox* m_ckBoxFgHidesData;	 //!< check box for learning foreground
	wxCheckBox* m_ckBoxSegBayes; //!< check box for activating bayesian segmentation
	wxCheckBox* m_ckBoxNoFlagNaN; //!< check box for DEactivating FlagNaN
	wxCheckBox* m_ckBoxRecordXYZ; //!< check box to record current XYZ buffers (after scat comp)
	wxCheckBox* m_ckBoxRecordSeg; //!< check box to record current XYZ buffers (after scat comp)
	wxCheckBox* m_ckBoxRecordVTK; //!< check box to record current VTK data (after scat comp)
	#ifdef JMU_USE_VTK
		wxCheckBox* m_ckBoxBlankSegmVTK; //!< check box to blank points with segm below thr in VTK display
		wxTextCtrl* m_TxtBlankSegmThr; // RANSAC max number of iterations
	#endif // JMU_USE_VTK
	
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
