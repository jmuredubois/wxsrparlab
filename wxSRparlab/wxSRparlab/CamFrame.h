/*
 * CamFrame.h
 * Header of the camera frame for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.11.08
 */

#pragma once
#include "wxSRparlab.h" //!< top-level header file
#include "wxSrApp.h"	//!< application header file
#include "CMainWnd.h"	//!< main window header file
#include "CamPanelSettings.h" //!< camera settings panel header file
#include "CamViewData.h" //!< camera view panel header file



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
	//! destructor
	~CamFrame();
	//! Close() method
    void OnClose(wxCommandEvent& event);
	int CreateAndSetNotebook(const wxString& title); //!< creates a new Notebook
	void OnOpenDev(wxCommandEvent& WXUNUSED(event));
	void OnCloseDev(wxCommandEvent& WXUNUSED(event));
	void Acquire(wxCommandEvent& WXUNUSED(event));
	void SetFreq(wxCommandEvent& event);

private:
	SRCAM	m_sr;	// pointer for SR camera
	//! buffer where the SR camera will return the data
	unsigned char       *m_pSrBuf;
	int m_nSrBufSz ;
	int m_nRows;
	int m_nCols;
	wxNotebook* m_camNB; //!< a notebook interface
	CamPanelSettings* m_settingsPane;
	CamViewData* m_viewRangePane;
	CamViewData* m_viewAmpPane;

public:
    DECLARE_EVENT_TABLE()
};

//! enum used by the CamFrame class
enum CamFrameEnum
{
    IDM_Close = 1 ,
	IDB_CLOSE = 2 ,
	IDS_TEXT  = 3 ,
	IDB_OpenDev = 4,
	IDB_CloseDev = 5,
	IDB_Acquire = 6, 
	IDR_Freq = 7,
	ID_ThisIsAStop = 255
};
