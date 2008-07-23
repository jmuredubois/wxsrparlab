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
#ifdef JMU_USE_VTK
#include "CamVtkView.h" //!< camera vtk view header file
#endif



/** the environment variable WXWIN should point to a valid wxWidget 
 *  installation /n
 * "$(WXWIN)/include"    should be added to the include path
 */
#include "wx/image.h" //!< for wxImages

class ThreadReadData;
#ifdef JMU_USE_VTK
class CViewSrVtk;
class CamVtkView;
#endif

//! enum used by the CamViewData class
enum CamReadModeEnum
{
	CAM_RD_CONTINU = 1 ,
	CAM_RD_ONESHOT = 2
};

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
	void SetReadMode(wxCommandEvent& event);
	void AcqOneFrm();
	void CoordTrf();
#ifdef JMU_USE_VTK
	void SetVtkWin(CViewSrVtk *vtkWin, int vtkSub);
	CamVtkView* GetCamVtk(){return _camVtk;};
#endif

private:
	SRCAM	m_sr;	// pointer for SR camera
	//! buffer where the SR camera will return the data
	unsigned char       *m_pSrBuf;
	unsigned short      *m_pSrZ;
	short      *m_pSrX;
	short      *m_pSrY;
	wxMutex* m_mutexSrBuf;
	int m_nSrBufSz ;
	int m_nRows;
	int m_nCols;
	wxNotebook* m_camNB; //!< a notebook interface
	wxFFile*		m_pFile4ReadPha; // PHASE file for read operation
	wxFFile*		m_pFile4ReadAmp; // AMPLITUDE file for read operation
	ThreadReadData*		m_pThreadReadData; // THREAD for reading data
	CamPanelSettings* m_settingsPane;	//! panel for CAMERA SETTINGS
	CamViewData* m_viewRangePane;		//! panel for RANGE data display
	CamViewData* m_viewAmpPane;			//! panel for AMPLITUDE data display
	CamViewData* m_viewZPane;			//! panel for Z data display
	CamViewData* m_viewYPane;			//! panel for Y data display
	CamViewData* m_viewXPane;			//! panel for X data display
	int m_nFrmRead;		//! number of frames read
	unsigned int m_nSerialSR;		//! SR serial
	CamReadModeEnum  m_camReadMode;		//! read  mode
	bool m_bReadContinuously;	//! flag for continuous read
	ModulationFrq m_srFrq;		//! SR frequency
	float m_maxMM[4];			//! table for maximum depth value according to frequency
	float m_fFocal; float m_fPixSzX; float m_fPixSzY; float m_fCenterX; float m_fCenterY;
	float m_maxMMr;
	int	  m_mfrqInt;
#ifdef JMU_USE_VTK
	CViewSrVtk	*_vtkWin ; //!< pointer to vtk window
	int _vtkSub;
	CamVtkView *_camVtk; //!< point to vtk camera structure
#endif

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
	IDR_ReadMode = 8,
	ID_ThisIsAStop = 255
};

