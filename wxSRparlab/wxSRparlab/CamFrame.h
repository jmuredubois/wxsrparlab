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
#include "srBuf.h"
#include "libSRPLscat.h"
#include "libSRPLflagNaN.h"
#pragma comment( lib, "libSRPLscat" )
#include "libSRPLavg.h"
#pragma comment( lib, "libSRPLavg" )
#include "libSRPLcoordTrf.h"
#pragma comment( lib, "libSRPLcoordTrf" )
#include "libSRPLsegm.h"
#pragma comment( lib, "libSRPLsegm" )



/** the environment variable WXWIN should point to a valid wxWidget 
 *  installation /n
 * "$(WXWIN)/include"    should be added to the include path
 */
#include "wx/image.h" //!< for wxImages

class ThreadReadData;
class MainWnd;
#ifdef JMU_USE_VTK
class CViewSrVtk;
class CamVtkView;
#endif

class CamScattering;
class CamFlagNaN;

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
    CamFrame( MainWnd* parentFrm, const wxString& title, const wxPoint& pos, const wxSize& size);
	//! destructor
	~CamFrame();
	//! Close() method
    void OnClose(wxCommandEvent& event);
	int CreateAndSetNotebook(const wxString& title); //!< creates a new Notebook
	void OnOpenDev(wxCommandEvent& WXUNUSED(event));
	void OnCloseDev(wxCommandEvent& WXUNUSED(event));
	void OnRecord(wxCommandEvent& WXUNUSED(event));
	void Acquire(wxCommandEvent& WXUNUSED(event));
	void SetFreq(wxCommandEvent& event);
	void SetReadMode(wxCommandEvent& event);
	void AcqOneFrm();
	void AcqOneFrmEvt(wxCommandEvent& WXUNUSED(event));
	void OnSetTrfMat(wxCommandEvent& WXUNUSED(event));
	#ifdef JMU_TGTFOLLOW 
		void OnTgtFile(wxCommandEvent& WXUNUSED(event)); 
	#endif
#ifdef JMU_USE_VTK
	void SetVtkWin(CViewSrVtk *vtkWin, int vtkSub);
	CamVtkView* GetCamVtk(){return _camVtk;};
	CamVtkView* GetCamBGVtk(){return _camBGVtk;};
	void RenderEvt(wxCommandEvent& WXUNUSED(event));
#endif
	int GetVtkSub(){return _vtkSub;};
	int GetNumCols(){return m_nCols;};
	int GetNumRows(){return m_nRows;};
	SRCAM GetSRcam(){return m_sr;};
	void OnSetScatParams(wxCommandEvent& WXUNUSED(event));
	bool IsScatChecked(){ return m_settingsPane->IsScatChecked(); };
	void OnClearBg(wxCommandEvent& WXUNUSED(event));
	bool IsLrnBgChecked(){ return m_settingsPane->IsLrnBgChecked(); };
	void OnClearFg(wxCommandEvent& WXUNUSED(event));
	bool IsLrnFgChecked(){ return m_settingsPane->IsLrnFgChecked(); };
	bool IsAcqContinuous(){ return (m_camReadMode==CAM_RD_CONTINU); };
	long GetAcqTime(){ return _acqTime;};
	void OnSetSegmParams(wxCommandEvent& WXUNUSED(event));

private:
	MainWnd	*_pWin; // parent main wnd
	SRCAM	m_sr;	// pointer for SR camera
	//! buffer where the SR camera will return the data
	unsigned char       *m_pSrBuf;
	wxMutex* m_mutexSrBuf;
	int m_nSrBufSz ;
	int m_nRows;
	int m_nCols;
	wxAuiNotebook* m_camNB; //!< a notebook interface
	wxFFile*		m_pFile4ReadPha; // PHASE file for read operation
	wxFFile*		m_pFile4ReadAmp; // AMPLITUDE file for read operation
	ThreadReadData*		m_pThreadReadData; // THREAD for reading data
	wxFFile*		m_pFile4WritePha; // PHASE file for WRITE operation
	wxFFile*		m_pFile4WriteAmp; // AMPLITUDE file for WRITE operation
	CamPanelSettings* m_settingsPane;	//! panel for CAMERA SETTINGS
	CamViewData* m_viewRangePane;		//! panel for RANGE data display
	CamViewData* m_viewAmpPane;			//! panel for AMPLITUDE data display
	CamViewData* m_viewZPane;			//! panel for Z data display
#ifdef DISPXYBUFFERS
	CamViewData* m_viewYPane;			//! panel for Y data display
	CamViewData* m_viewXPane;			//! panel for X data display
#endif //DISPXYBUFFERS
	CamViewData* m_viewBGRangePane;		//! panel for BACKGROUND RANGE data display
	CamViewData* m_viewBGAmpPane;		//! panel for BACKGROUND RANGE data display
	CamViewData* m_viewSegmPane;		//! panel for SEGMENTED data display
	int m_nFrmRead;		//! number of frames read
	unsigned int m_nSerialSR;		//! SR serial
	CamReadModeEnum  m_camReadMode;		//! read  mode
	bool m_bReadContinuously;	//! flag for continuous read
	ModulationFrq m_srFrq;		//! SR frequency
	float m_maxMM[4];			//! table for maximum depth value according to frequency
	//float m_fFocal; float m_fPixSzX; float m_fPixSzY; float m_fCenterX; float m_fCenterY;
	int	  m_mfrqInt;
#ifdef JMU_USE_VTK
	CViewSrVtk	*_vtkWin ; //!< pointer to vtk window
	CamVtkView *_camVtk; //!< point to vtk camera structure
	CamVtkView *_camBGVtk; //!< point to background vtk camera structure
#endif
	#ifdef JMU_TGTFOLLOW  
		wxFFile*		m_pFile4TgtCoord; 
	#endif
	int _vtkSub;
	SRPLSCAT m_scat;		// scattering compensation object
	SRPLNAN  m_NaN;			// NaN filtering object
	SRPLAVG	 m_bgAvg;		// background average object
	SRPLAVG	 m_fgAvg;		// foreground average object
	SRPLNAN  m_bgNaN;		// NaN filtering object
	SRPLNAN  m_fgNaN;		// NaN filtering object
	SRPLCTR	 m_CTrf;		// Coordinates transform object
	SRPLCTR	 m_CTrfBG;		// background coordinates transform object
	SRCTR m_ctrParam;		// camera opticla center parameters
	SRPLSEGM m_segm;		// scattering compensation object
	long _acqTime;
	wxStopWatch* m_pAcqSWatch;

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
	IDB_SetTrfMat = 9,
	IDC_AcqOne = 10,
	IDB_TgtFile = 11,
	IDC_ScatComp = 12,
	IDB_SetScatParams = 13,
	IDC_LrnBg = 14,
	IDB_ClearBg = 15,
	IDC_Render = 16,
	IDC_Record = 17,
	IDC_LrnFg = 18,
	IDB_ClearFg = 19,
	IDC_FgHidesData = 20,
	IDC_SegmBayes = 21,
	IDB_SegmSetParams = 22,
	ID_ThisIsAStop = 255
};

