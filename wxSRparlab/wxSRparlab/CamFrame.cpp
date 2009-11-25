/*
 * CamFrame.cpp
 * Implementation of the camera frame for wxSRparlab
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

// ----------------------------------------------------------------------------
// GUI thread
// ----------------------------------------------------------------------------

DECLARE_EVENT_TYPE(wxEVT_JMUACQONEFRM, -1)
DEFINE_EVENT_TYPE(wxEVT_JMUACQONEFRM)
DECLARE_EVENT_TYPE(wxEVT_JMURENDER, -1)
DEFINE_EVENT_TYPE(wxEVT_JMURENDER)

class ThreadReadData : public wxThread
{
public:
    ThreadReadData(CamFrame *cFrm);
	~ThreadReadData();

    // thread execution starts here
    virtual void *Entry();

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();

private:
    CamFrame *m_cFrm;
};

ThreadReadData::ThreadReadData(CamFrame *cFrm)
: wxThread(wxTHREAD_DETACHED)//: wxThread(wxTHREAD_JOINABLE)
{
    m_cFrm = cFrm;
};

/**
 * Camera frame class destructor \n
 */
ThreadReadData::~ThreadReadData()
{

}

void ThreadReadData::OnExit()
{

}

void *ThreadReadData::Entry()
{
    wxString text;

	long acqTime = 1000L;
    for ( ; ; )
    {
        // check if we were asked to exit
        if ( TestDestroy() )
            break;
		//m_cFrm->AcqOneFrm();
		wxCommandEvent event( wxEVT_JMUACQONEFRM, IDC_AcqOne );
		acqTime = m_cFrm->GetAcqTime();
		m_cFrm->AddPendingEvent(event);
		//m_cFrm->ProcessEvent(event);
		if(acqTime <66){acqTime = 66;};
        wxThread::Sleep(acqTime);
		#ifdef LARGE_PSF
			if(m_cFrm->IsScatChecked())
			{
				wxThread::Sleep(200); // scattering compensatino is expensive
			}
        #endif
    }

    return NULL;
}

/**
 * Camera frame class constructor \n
 * Each instance must have a parent frame (usually CMainWnd) \n
 */
CamFrame::CamFrame(MainWnd* parentFrm, const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(parentFrm, -1, title, pos, size)
{
	//! - a status bar is created \n
    CreateStatusBar();
    SetStatusText( _T("cam") );

	_pWin = parentFrm;
	m_camNB = NULL;
	m_sr = NULL;
	m_pSrBuf = NULL; m_nRows = 0; m_nCols = 0; m_nSrBufSz = 0;
	m_mutexSrBuf = NULL;

	m_pFile4ReadPha = NULL;//new wxFFile();
	m_pFile4ReadAmp = NULL;
	m_pFile4WritePha = NULL;
	m_pFile4WriteAmp = NULL;
	m_pFile4WriteXYZ = NULL;
	m_pFile4WriteSeg = NULL;
	m_nFrmRead = 0; // 0 frames read when creating
	m_nSerialSR = 0;
	m_pThreadReadData = NULL;
	m_camReadMode = CAM_RD_ONESHOT;
	m_bReadContinuously = false;
	m_srFrq = MF_20MHz;
	m_maxMM[0]= 5000.0f; m_maxMM[1]= 7142.8571f;m_maxMM[2]= 7500.0f;m_maxMM[3]= 7894.7368f;
	//! @todo:  HARDCODED values for m_maxMM -> check with libusbSR driver settings
	//m_fFocal = 8.0f; m_fPixSzX = 0.04f; m_fPixSzY = 0.04f; m_fCenterX = 88.0f; m_fCenterY = 72.0f;
	m_ctrParam.f = 8.0f; m_ctrParam.pixDX = 0.04f; m_ctrParam.pixDY = 0.04f; m_ctrParam.cX = 88.0f; m_ctrParam.cY = 72.0f;
	m_ctrParam.maxMM = 7500.0f;
	m_mfrqInt = 2;
#ifdef JMU_USE_VTK
	_vtkWin=NULL;
	_camVtk = NULL;
	_camBGVtk = NULL;
	_camFGVtk = NULL;
	//_camSegmVtk = NULL;
	_vtkRecPrefix.Empty();
#endif
	#ifdef JMU_TGTFOLLOW
		m_pFile4TgtCoord = NULL;
	#endif
	_vtkSub = 0;
	m_scat = NULL;
	m_NaN = NULL;
	m_bgAvg = NULL; m_bgNaN = NULL;
	m_fgAvg = NULL; m_fgNaN = NULL;
	m_CTrf = NULL;
	m_CTrfBG = NULL;
	m_CTrfFG = NULL;
	_acqTime = 1000L;
	m_pAcqSWatch = NULL;
	m_segm = NULL;
#ifdef JMU_RANSAC
	m_ransac = NULL;
	_rscSucc = 0;
#endif
#ifdef JMU_ALIGNGUI
	for(int k=0; k<6; k++)
	{ 
		_aliPtsR[k] = 0; _aliPtsC[k] = 0;
		_aliPtsX[k] = 0; _aliPtsY[k] = 0; _aliPtsZ[k] = 0;
	}
	_aliPCnt = 0;
#endif
}

/**
 * Camera frame class destructor \n
 */
CamFrame::~CamFrame()
{
	if(_pWin != NULL){	_pWin->PopCam(_vtkSub);};
	int res = -1;
	if(m_bReadContinuously){ if(m_pThreadReadData != NULL){ m_pThreadReadData->Delete(); } } ;
#ifdef JMU_USE_VTK
	if(_camVtk != NULL) { delete(_camVtk); _camVtk = NULL; } ;
	if(_camBGVtk != NULL) { delete(_camBGVtk); _camBGVtk = NULL; } ;
	if(_camFGVtk != NULL) { delete(_camFGVtk); _camFGVtk = NULL; } ;
	//if(_camSegmVtk != NULL) { delete(_camSegmVtk); _camSegmVtk = NULL; } ;
	//if(_vtkRecPrefix != NULL) { delete(_vtkRecPrefix); _vtkRecPrefix = NULL; } ;
#endif
	if(m_sr		  != NULL) { res = SR_Close(m_sr);	m_sr = NULL; };
	if(m_mutexSrBuf != NULL) { delete(m_mutexSrBuf); m_mutexSrBuf = NULL;};
	if(m_pSrBuf   != NULL) { free((void*) m_pSrBuf  ); m_pSrBuf   = NULL; };
	if(m_pFile4ReadPha != NULL) { delete(m_pFile4ReadPha); m_pFile4ReadPha = NULL; };
	if(m_pFile4ReadAmp != NULL) { delete(m_pFile4ReadAmp); m_pFile4ReadAmp = NULL; };
	#ifdef JMU_TGTFOLLOW
		if(m_pFile4TgtCoord != NULL) { delete(m_pFile4TgtCoord); m_pFile4TgtCoord = NULL; };
	#endif
	if(m_pFile4WritePha != NULL) { delete(m_pFile4WritePha); m_pFile4WritePha = NULL; };
	if(m_pFile4WriteAmp != NULL) { delete(m_pFile4WriteAmp); m_pFile4WriteAmp = NULL; };
	if(m_pFile4WriteXYZ != NULL) { delete(m_pFile4WriteXYZ); m_pFile4WriteXYZ = NULL; };
	if(m_pFile4WriteSeg != NULL) { delete(m_pFile4WriteSeg); m_pFile4WriteSeg = NULL; };
	if(m_NaN != NULL) { PLNN_Close(m_NaN); m_NaN = NULL; };
	if(m_scat != NULL) {PLSC_Close(m_scat); m_scat = NULL; };
	if(m_bgAvg != NULL) {PLAVG_Close(m_bgAvg); m_bgAvg = NULL; };
	if(m_fgAvg != NULL) {PLAVG_Close(m_fgAvg); m_fgAvg = NULL; };
	if(m_bgNaN != NULL) { PLNN_Close(m_bgNaN); m_bgNaN = NULL; };
	if(m_fgNaN != NULL) { PLNN_Close(m_fgNaN); m_fgNaN = NULL; };
	if(m_CTrf != NULL) {PLCTR_Close(m_CTrf); m_CTrf = NULL; };
	if(m_CTrfBG != NULL) {PLCTR_Close(m_CTrfBG); m_CTrfBG = NULL; };
	if(m_CTrfFG != NULL) {PLCTR_Close(m_CTrfFG); m_CTrfFG = NULL; };
	if(m_pAcqSWatch != NULL) { delete(m_pAcqSWatch); m_pAcqSWatch = NULL;};
	if(m_segm != NULL) {PLSEGM_Close(m_segm); m_segm = NULL; };
#ifdef JMU_RANSAC
	if(m_ransac != NULL) {PLRSC_Close(m_ransac); m_ransac = NULL; };
#endif
}

BEGIN_EVENT_TABLE(CamFrame, wxFrame)
    EVT_MENU(IDM_Close, CamFrame::OnClose)
	EVT_BUTTON(IDB_OpenDev,  CamFrame::OnOpenDev)
	EVT_BUTTON(IDB_CloseDev,  CamFrame::OnCloseDev)
	EVT_BUTTON(IDB_DevSRsettings,  CamFrame::OnDevSRsettings)
	EVT_BUTTON(IDB_Acquire,  CamFrame::Acquire)
	EVT_RADIOBOX(IDR_Freq, CamFrame::SetFreq)
	EVT_RADIOBOX(IDR_ReadMode, CamFrame::SetReadMode)
	EVT_BUTTON(IDB_SetTrfMat,  CamFrame::OnSetTrfMat)
	#ifdef JMU_TGTFOLLOW
		EVT_BUTTON(IDB_TgtFile,  CamFrame::OnTgtFile)
	#endif
	EVT_COMMAND(IDC_AcqOne, wxEVT_JMUACQONEFRM, CamFrame::AcqOneFrmEvt)
	#ifdef JMU_USE_VTK
		EVT_COMMAND(IDC_Render, wxEVT_JMURENDER, CamFrame::RenderEvt)
		EVT_TEXT(IDT_BlankSegmThr, CamFrame::OnBlankSegmThr)
	#endif
	EVT_BUTTON(IDB_SetScatParams, CamFrame::OnSetScatParams)
	EVT_BUTTON(IDB_ClearBg, CamFrame::OnClearBg)
	EVT_BUTTON(IDB_ClearFg, CamFrame::OnClearFg)
	EVT_CHECKBOX(IDC_Record, CamFrame::OnRecord)
	EVT_BUTTON(IDB_SegmSetParams, CamFrame::OnSetSegmParams)
	#ifdef JMU_RANSAC
		EVT_BUTTON(IDB_RansacBG, CamFrame::RansacBG)
		EVT_BUTTON(IDB_RansacBGsucc, CamFrame::RansacBGsucc)
		EVT_BUTTON(IDB_RansacFG, CamFrame::RansacFG)
		EVT_TEXT(IDT_RansacNiterMax, CamFrame::OnSetRansacNiterMax)
		EVT_TEXT(IDT_RansacDistPla, CamFrame::OnSetRansacDistPla)
	#endif
	EVT_CHECKBOX(IDC_RecordXYZ, CamFrame::OnRecXYZ)
	EVT_CHECKBOX(IDC_RecordSeg, CamFrame::OnRecSeg)
	EVT_CHECKBOX(IDC_RecordVTK, CamFrame::OnRecVTK)
END_EVENT_TABLE()

/**
 * Camera frame close \n
 * The method: \n
 */
void CamFrame::OnClose(wxCommandEvent& WXUNUSED(event))
{
	int res;
	if(m_sr != NULL)
	{
		res = SR_Close(m_sr);
	}
	// - calls the wxWindowBase Close() method \n
    Close(TRUE);
}

/**
 * Camera frame close \n
 * The method: \n
 */
int CamFrame::CreateAndSetNotebook()
{
	int res  = 0;
	if(m_camNB != NULL)
	{
		m_camNB->Close(TRUE);
	}
	m_camNB = new wxAuiNotebook(this, -1, wxPoint(-1,-1), wxSize(-1,-1), wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS );

	// %%%%%%% Settings window
	m_settingsPane = new CamPanelSettings(m_camNB, this, wxT("Settings"), wxPoint(-1,-1), wxSize(-1,-1)); /* NBparadigm */
	m_settingsPane->InitSettings();

	// %%%%%% RANGE panel
	m_viewRangePane = new CamViewData(m_camNB, this, wxT("Range"), wxPoint(-1,-1), wxSize(-1,-1));  /* NBparadigm */
	m_viewRangePane->InitViewData();

	// %%%%%% AMPLITUDE panel
	m_viewAmpPane = new CamViewData(m_camNB, this, wxT("Amplitude"), wxPoint(-1,-1), wxSize(-1,-1));  /* NBparadigm */
	m_viewAmpPane->InitViewData();
	m_viewAmpPane->SetDispMin(0.0);
	m_viewAmpPane->SetDispMax(1000.0);

	// %%%%%% Z panel
	m_viewZPane = new CamViewData(m_camNB, this, wxT("Z [mm]"), wxPoint(-1,-1), wxSize(-1,-1));  /* NBparadigm */
	m_viewZPane->InitViewData();
	m_viewZPane->SetDispMin(3500.0);
	m_viewZPane->SetDispMax(000.0);
#ifdef DISPXYBUFFERS
	// %%%%%% Y panel
	m_viewYPane = new CamViewData(m_camNB, this, wxT("Y [mm]"), wxPoint(-1,-1), wxSize(-1,-1));  /* NBparadigm */
	m_viewYPane->InitViewData();
	m_viewYPane->SetDispMin(-1500.0);
	m_viewYPane->SetDispMax(1500.0);
	// %%%%%% x panel
	m_viewXPane = new CamViewData(m_camNB, this, wxT("X [mm]"), wxPoint(-1,-1), wxSize(-1,-1));  /* NBparadigm */
	m_viewXPane->InitViewData();
	m_viewXPane->SetDispMin(-1500.0);
	m_viewXPane->SetDispMax(1500.0);
#endif

	// %%%%% BACKGROUND RANGE panel
	m_viewBGRangePane = new CamViewData(m_camNB, this, wxT("BG pha"), wxPoint(-1,-1), wxSize(-1,-1));  /* NBparadigm */
	m_viewBGRangePane->InitViewData();
	m_viewBGRangePane->SetDispMin(45000.0);
	m_viewBGRangePane->SetDispMax(0.0);
	// %%%%% BACKGROUND AMPLITUDE panel
	m_viewBGAmpPane = new CamViewData(m_camNB, this, wxT("BG amp"), wxPoint(-1,-1), wxSize(-1,-1));  /* NBparadigm */
	m_viewBGAmpPane->InitViewData();
	m_viewBGAmpPane->SetDispMin(0.0);
	m_viewBGAmpPane->SetDispMax(1000.0);
	// %%%%% SEGMENTED panel
	m_viewSegmPane = new CamViewData(m_camNB, this, wxT("segm"), wxPoint(-1,-1), wxSize(-1,-1));  /* NBparadigm */
	m_viewSegmPane->InitViewData();
	m_viewSegmPane->SetDispMin(0);
	m_viewSegmPane->SetDispMax(255);

	/* NBparadigm */
	m_camNB->AddPage(m_settingsPane, wxT("Settings"), TRUE);//, -1);
	m_camNB->AddPage(m_viewRangePane, wxT("Range"), FALSE);//, -1);
	m_camNB->AddPage(m_viewAmpPane, wxT("Amplitude"), FALSE);//, -1);
	m_camNB->AddPage(m_viewZPane, wxT("Z [mm]"), FALSE);//, -1);
#ifdef DISPXYBUFFERS
	m_camNB->AddPage(m_viewYPane, wxT("Y [mm]"), FALSE);//, -1);
	m_camNB->AddPage(m_viewXPane, wxT("X [mm]"), FALSE);//, -1);
#endif //DISPXYBUFFERS
	m_camNB->AddPage(m_viewSegmPane, wxT("Segm"), FALSE);//, -1);
	m_camNB->AddPage(m_viewBGRangePane, wxT("BG pha"), FALSE);//, -1);
	m_camNB->AddPage(m_viewBGAmpPane, wxT("BG amp"), FALSE);//, -1);
	/* EO NBparadigm */

	return res;
}

//---------------------------------------------------
/*!
	- Opens SR device. \n
	- Opens PP object. \n
	- Loads PP settings. (Here, it is possible to change the default settings filename). \n
	- Initiates IPL buffers. \n
	- Initiates replay interface. \n
	Unfortunately, no stub SR device exists.
	\todo
	 - add stub SR device for record replay
*/
//! Opens SR device
void CamFrame::OnOpenDev(wxCommandEvent& WXUNUSED(event))
{
  int res = 0;
  m_mutexSrBuf = new wxMutex(wxMUTEX_DEFAULT);
  //logPrintf("swissrangerTester: SR_Open device");
  m_settingsPane->SetText(wxT("Open attempt"));
  m_settingsPane->DisableOpenSR();		// disable "Open" button
  m_settingsPane->EnableRadioFilt();	// ensable filter selection
  m_settingsPane->EnableRadioFrq();		// enable frequency selection
  m_settingsPane->EnableCloseSR();	// enable "Close" button
  m_settingsPane->EnableScatParams(); // enable "Scat comp params" button
  m_settingsPane->EnableSegmParams(); // enable "Segm params" button
  wxString strR;
  unsigned int serial = 0;
  if(m_sr == NULL)
  {
	  res = -1;
		res = SR_OpenDlg(& m_sr, 2, (HWND) (this->GetHandle())); // opens first found camera
		//Returns the serial number (if existing) of the camera
  }
  strR.sprintf(wxT("cam open %u"), res); // ... change title text ...
  if(m_sr != NULL)
  {
	  serial =  SR_ReadSerial(m_sr);
	  strR.sprintf(wxT("cam serial %u"), serial); // ... change title text ...
	  m_nSerialSR = serial;
  }
  m_pAcqSWatch = new wxStopWatch();
  m_pAcqSWatch->Pause();

  wxFileDialog* OpenDialogPar = new wxFileDialog(this,
	  wxT("Choose a SR parameters file to open"),	// msg
	  wxT(""), //wxT("D:\\dataSR3\\"),	// default dir
	  wxEmptyString,	// default file
	  wxT("SR parameters files (*.sr2)|*.sr2|All files (*.*)|*.*"),	// file ext
	  wxFD_OPEN|wxFD_CHANGE_DIR,
	  wxDefaultPosition);
  wxFileDialog* OpenDialogPha = new wxFileDialog(this,
	  wxT("Choose a SR PHASE file to open"),	// msg
	  wxT(""), //wxT("D:\\dataSR3\\"),	// default dir
	  wxEmptyString,	// default file
	  wxT("SR phase files (*.16b)|*.16b|All files (*.*)|*.*"),	// file ext
	  wxFD_OPEN|wxFD_CHANGE_DIR,
	  wxDefaultPosition);
  wxFileDialog* OpenDialogAmp = new wxFileDialog(this,
	  wxT("Choose a SR AMPLITUDE file to open"),	// msg
	  wxT(""), //wxT("D:\\dataSR3\\"),	// default dir
	  wxEmptyString,	// default file
	  wxT("SR amplitude files (*.16b)|*.16b|All files (*.*)|*.*"),	// file ext
	  wxFD_OPEN|wxFD_CHANGE_DIR,
	  wxDefaultPosition);
  if((OpenDialogPar->ShowModal()==wxID_OK) && (OpenDialogPha->ShowModal()==wxID_OK) && (OpenDialogAmp->ShowModal()==wxID_OK))
  {
	  wxString strPathPar = OpenDialogPar->GetPath();
	  wxString strPathPha = OpenDialogPha->GetPath();
	  wxString strPathAmp = OpenDialogAmp->GetPath();
		// Sets our current document to the file the user selected
	  wxFFile* wxFparams = new wxFFile(strPathPar, wxT("rb"));
	  if ( (wxFparams->IsOpened()) )
	  {
	    int tmp = 0; // DANGER = 1 int must be 4 bytes
		float tmpFlt = 0;
		if(sizeof(int)!=4)
		{
			res-=64;
			m_nCols = (int) 176;
			m_nRows = (int) 144;
			m_nSrBufSz = (int) (m_nCols*m_nRows*2*2); // *2 amp,pha *2:short
		}
		else // (sizeof(int)!=4)
		{
	      size_t readBytes= wxFparams->Read(&tmp, sizeof(int));
		  if(readBytes==sizeof(int)) {m_nRows = tmp;};
		  readBytes= wxFparams->Read(&tmp, sizeof(int));
		  if(readBytes==sizeof(int)) {m_nCols = tmp;};
		  readBytes= wxFparams->Read(&tmp, sizeof(int));
		  if(readBytes==sizeof(int)) {int numImg = tmp;};
		  readBytes = wxFparams->Read(&tmp, sizeof(int));
		  if(readBytes==sizeof(int)) {int bytesPerSample = tmp;}
		  readBytes= wxFparams->Read(&tmp, sizeof(int));
		  if(readBytes==sizeof(int)) {m_nSrBufSz = tmp;};
		  readBytes= wxFparams->Read(&tmp, sizeof(unsigned int));
		  if(readBytes==sizeof(unsigned int)) {m_nSerialSR = (unsigned int)tmp;};
		  readBytes = wxFparams->Read(&tmp, sizeof(int));
		  if(readBytes==sizeof(int)) {int m_frqInt = tmp; m_srFrq = (ModulationFrq) tmp;};
		  /*		float focal = 8.0f;   //mm
					float pixSzX = 0.04f; //mm
					float pixSzY = 0.04f; //mm
					float centerX = 95.1f ; float centerY = 56.3f ; // pixels*/
		  readBytes = wxFparams->Read(&tmpFlt, sizeof(float));
		  if(readBytes==sizeof(float)) { m_ctrParam.f = tmpFlt;};
		  readBytes = wxFparams->Read(&tmpFlt, sizeof(float));
		  if(readBytes==sizeof(float)) { m_ctrParam.pixDX = tmpFlt;};
		  readBytes = wxFparams->Read(&tmpFlt, sizeof(float));
		  if(readBytes==sizeof(float)) { m_ctrParam.pixDY = tmpFlt;};
		  readBytes = wxFparams->Read(&tmpFlt, sizeof(float));
		  if(readBytes==sizeof(float)) { m_ctrParam.cX = tmpFlt;};
		  readBytes = wxFparams->Read(&tmpFlt, sizeof(float));
		  if(readBytes==sizeof(float)) { m_ctrParam.cY = tmpFlt;};
		  readBytes = wxFparams->Read(&tmpFlt, sizeof(float));
		  if(readBytes==sizeof(float)) { m_ctrParam.maxMM = tmpFlt;};
		} // (sizeof(int)!=4)
		delete(wxFparams);
	    m_pFile4ReadPha = new wxFFile(strPathPha, wxT("rb"));
	    m_pFile4ReadAmp = new wxFFile(strPathAmp, wxT("rb"));
	    if ( (!(m_pFile4ReadAmp)) || (!(m_pFile4ReadAmp)) || (!(m_pFile4ReadAmp->IsOpened())) || (!(m_pFile4ReadAmp->IsOpened())))
	    {
		  res -=4;
	    }

	    strR.sprintf(wxT("cam serial %u posing as %u \n - %ix%i  - %i"), serial, m_nSerialSR, m_nRows, m_nCols, m_nSrBufSz); // ... change text ...
		wxMutexError errMutex= m_mutexSrBuf->Lock();
		if(errMutex == wxMUTEX_NO_ERROR)
		{
			m_pSrBuf = (unsigned char*) malloc(m_nSrBufSz);
			memset(m_pSrBuf, 0x77, m_nSrBufSz);
			errMutex = m_mutexSrBuf->Unlock();
		} //{errMutex == wxMUTEX_NO_ERROR)
	    res = m_viewRangePane->SetDataArray<unsigned short>((unsigned short*) &m_pSrBuf[0], m_nRows*m_nCols);
	    res = m_viewAmpPane->SetDataArray<unsigned short>((unsigned short*) &m_pSrBuf[m_nCols*m_nRows*2], m_nRows*m_nCols);
		//res = m_viewZPane->SetDataArray<unsigned short>((unsigned short*) &m_pSrZ[0], m_nRows*m_nCols);
		//#ifdef DISPXYBUFFERS
		//  res = m_viewXPane->SetDataArray<short>((short*) &m_pSrX[0], m_nRows*m_nCols);
		//  res = m_viewYPane->SetDataArray<short>((short*) &m_pSrY[0], m_nRows*m_nCols);
		//#endif //DISPXYBUFFERS
		  //res = m_viewBGRangePane->SetDataArray<unsigned short>((unsigned short*) &m_pSrBuf[0], m_nRows*m_nCols);

	    m_settingsPane->SetText(strR);
		SetStatusText( strR );
	  } // (wxFparams->IsOpened())
  } // (OpenDialogPar->ShowModal()==wxID_OK) && (OpenDialogPha->ShowModal()==wxID_OK) && (OpenDialogAmp->ShowModal()==wxID_OK)
  delete(OpenDialogPar);
  delete(OpenDialogPha);
  delete(OpenDialogAmp);
  if((m_pFile4ReadPha  != NULL) && (m_pFile4ReadAmp  != NULL) && (m_pFile4ReadPha->IsOpened()) && (m_pFile4ReadAmp->IsOpened()))
  {
	  SRBUF newbuf; newbuf.amp = NULL; newbuf.pha = NULL;
	  newbuf.nCols = m_nCols; newbuf.nRows = m_nRows;
	  newbuf.bufferSizeInBytes = m_nRows * m_nCols * 2 * sizeof(unsigned short);
	  PLSC_Open(&m_scat, newbuf);
	  PLNN_Open(&m_NaN, newbuf);
	  PLAVG_Open(&m_bgAvg, newbuf);
	  PLAVG_Open(&m_fgAvg, newbuf);
	  PLNN_Open(&m_bgNaN, newbuf);
	  PLNN_Open(&m_fgNaN, newbuf);
	  PLCTR_Open(&m_CTrf, newbuf);
	  PLCTR_Open(&m_CTrfBG, newbuf);
	  PLCTR_Open(&m_CTrfFG, newbuf);
	  PLSEGM_Open(&m_segm, newbuf);
#ifdef JMU_RANSAC
	  PLRSC_Open(&m_ransac, newbuf);
#endif
	  return;
  }


  if(m_sr != NULL)
  {
	  //m_nSrBufSz = (int) SR_GetBufferSize(m_sr);
	  m_nCols = (int) SR_GetCols(m_sr);
	  m_nRows = (int) SR_GetRows(m_sr);
	  m_nSrBufSz = m_nCols * m_nRows *sizeof(WORD)*2 ;  //2 images
	  strR.sprintf(wxT("cam serial %u - %ix%i  - %i"), serial, m_nRows, m_nCols, m_nSrBufSz); // ... change text ...
	  wxMutexError errMutex= m_mutexSrBuf->Lock();
	  if(errMutex == wxMUTEX_NO_ERROR)
	  {
		m_pSrBuf = (unsigned char*) malloc(m_nSrBufSz);
		memset(m_pSrBuf, 0x77, m_nSrBufSz);
		res = SR_SetMode(m_sr, AM_COR_FIX_PTRN ); //|| AM_COR_LED_NON_LIN );
		res = SR_Acquire(m_sr);
		memcpy( (void*) m_pSrBuf, SR_GetImage(m_sr,0), m_nRows*m_nCols*sizeof(unsigned short));
		memcpy( (void*) (&m_pSrBuf[m_nRows*m_nCols]), SR_GetImage(m_sr,1), m_nRows*m_nCols*sizeof(unsigned short));
	    errMutex = m_mutexSrBuf->Unlock();
	  } //{errMutex == wxMUTEX_NO_ERROR)

	  res = m_viewRangePane->SetDataArray<unsigned short>((unsigned short*) SR_GetImage(m_sr, 0), m_nRows*m_nCols);
	  res = m_viewAmpPane->SetDataArray<unsigned short>((unsigned short*) SR_GetImage(m_sr, 1), m_nRows*m_nCols);
	  //res = m_viewZPane->SetDataArray<unsigned short>((unsigned short*) &m_pSrZ[0], m_nRows*m_nCols);
	  //#ifdef DISPXYBUFFERS
	  //  res = m_viewXPane->SetDataArray<short>((short*) &m_pSrX[0], m_nRows*m_nCols);
	  //  res = m_viewYPane->SetDataArray<short>((short*) &m_pSrY[0], m_nRows*m_nCols);
	  //#endif //DISPXYBUFFERS
	  //res = m_viewBGRangePane->SetDataArray<unsigned short>((unsigned short*) SR_GetImage(m_sr, 0), m_nRows*m_nCols);
  }
  m_settingsPane->SetText(strR);
  SetStatusText( strR );

  SRBUF newbuf; newbuf.amp = NULL; newbuf.pha = NULL;
  newbuf.nCols = m_nCols; newbuf.nRows = m_nRows;
  newbuf.bufferSizeInBytes = m_nRows * m_nCols * 2 * sizeof(unsigned short);
  PLSC_Open(&m_scat, newbuf);
  PLNN_Open(&m_NaN, newbuf);
  PLAVG_Open (&m_bgAvg, newbuf);
  PLAVG_Open (&m_fgAvg, newbuf);
  PLNN_Open(&m_bgNaN, newbuf);
  PLNN_Open(&m_fgNaN, newbuf);
  PLCTR_Open(&m_CTrf, newbuf);
  PLCTR_Open(&m_CTrfBG, newbuf);
  PLCTR_Open(&m_CTrfFG, newbuf);
  PLSEGM_Open(&m_segm, newbuf);
#ifdef JMU_RANSAC
  PLRSC_Open(&m_ransac, newbuf);
#endif
}

//---------------------------------------------------
/*!
	- Clears IPL buffers. \n
	- Closes replay interface. \n
	- Closes record interface (if opened). \n
	- Closes SR device. \n
	- Closes PP object. \n
*/
//! Closes SR device
void CamFrame::OnCloseDev(wxCommandEvent& WXUNUSED(event))
{
  int res = 0;
  //_sr=0;//there are no valid device opened
  m_settingsPane->SetText(wxT("Close attempt"));
  if(m_bReadContinuously){ if(m_pThreadReadData != NULL){ m_pThreadReadData->Delete(); } } ;
  m_settingsPane->DisableCloseSR();		// disable "Close" button
  m_settingsPane->DisableRadioFilt();	// disable filter selection
  m_settingsPane->DisableRadioFrq();	// disable frequency selection
  m_settingsPane->DisableScatParams();  // disable "Scat Comp Params" button
  m_settingsPane->DisableSegmParams();  // disable "Segm Params" button
  if(m_sr != NULL)
  {
		res = SR_Close(m_sr);
		m_sr = NULL;
  }
  if(m_pFile4ReadPha != NULL) { delete(m_pFile4ReadPha); m_pFile4ReadPha = NULL; };
  if(m_pFile4ReadAmp != NULL) { delete(m_pFile4ReadAmp); m_pFile4ReadAmp = NULL; };
  if(m_pFile4WritePha != NULL) { delete(m_pFile4WritePha); m_pFile4WritePha = NULL; };
  if(m_pFile4WriteAmp != NULL) { delete(m_pFile4WriteAmp); m_pFile4WriteAmp = NULL; };
  if(m_pFile4WriteXYZ != NULL) { delete(m_pFile4WriteXYZ); m_pFile4WriteXYZ = NULL; };
  if(m_pFile4WriteSeg != NULL) { delete(m_pFile4WriteSeg); m_pFile4WriteSeg = NULL; };
  if(m_mutexSrBuf != NULL) { delete(m_mutexSrBuf); m_mutexSrBuf = NULL;};
  if(m_pSrBuf   != NULL) { free((void*) m_pSrBuf  ); m_pSrBuf   = NULL; };
  #ifdef JMU_TGTFOLLOW
	if(m_pFile4TgtCoord != NULL) { delete(m_pFile4TgtCoord); m_pFile4TgtCoord = NULL; };
  #endif
  m_nSrBufSz = 176*144*2*sizeof(unsigned short);
  m_nCols = 176;
  m_nRows = 144;
  m_nFrmRead = 0;
  m_nSerialSR = 0;
  if(m_NaN != NULL)   {PLNN_Close(m_NaN);    m_NaN = NULL; };
  if(m_NaN != NULL)   {PLNN_Close(m_NaN);    m_NaN = NULL; };
  if(m_scat != NULL)  {PLSC_Close(m_scat);   m_scat = NULL; };
  if(m_bgAvg != NULL) {PLAVG_Close(m_bgAvg); m_bgAvg = NULL; };
  if(m_fgAvg != NULL) {PLAVG_Close(m_fgAvg); m_fgAvg = NULL; };
  if(m_bgNaN != NULL) { PLNN_Close(m_bgNaN); m_bgNaN = NULL; };
  if(m_fgNaN != NULL) { PLNN_Close(m_fgNaN); m_fgNaN = NULL; };
  if(m_CTrf != NULL) {PLCTR_Close(m_CTrf); m_CTrf = NULL; };
  if(m_CTrfBG != NULL) {PLCTR_Close(m_CTrfBG); m_CTrfBG = NULL; };
  if(m_CTrfFG != NULL) {PLCTR_Close(m_CTrfFG); m_CTrfFG = NULL; };
  if(m_pAcqSWatch != NULL) { delete(m_pAcqSWatch); m_pAcqSWatch = NULL;};
  if(m_segm != NULL)  {PLSEGM_Close(m_segm);   m_segm = NULL; };
#ifdef JMU_RANSAC
  if(m_ransac != NULL) {PLRSC_Close(m_ransac); m_ransac = NULL; };
  _rscSucc=0;
#endif
  m_settingsPane->EnableOpenSR();	// enable "Open" button
  m_settingsPane->SetText(wxT("Close successfull"));
  SetStatusText( wxT("cam") );
}
//---------------------------------------------------
/*!
	- Checks if camera is open. \n
	- Calls SR settings. \n
	- Closes record interface (if opened). \n
	- Closes SR device. \n
	- Closes PP object. \n
*/
//! Calls SR settings dialog box (windows only)
void CamFrame::OnDevSRsettings(wxCommandEvent& WXUNUSED(event))
{
  //_sr=0;//there are no valid device opened
  m_settingsPane->SetText(wxT("Attempt to open SR settings"));
#ifndef DUMMYSR
  int res = 0;
  if(m_sr != NULL)
  {
		res+=SR_OpenSettingsDlg(m_sr, (HWND) (this->GetHandle()) ); //20080228 apparently  SR_OpenSettingsDlgModal is deprecated in driver 1.0.9
		m_settingsPane->SetText(wxT("SR settings Dlg Box opened"));
  }
  else
  {
	  m_settingsPane->SetText(wxT("No SR device open: impossible to open SR settings Dlg Box."));
  }
#else
  m_settingsPane->SetText(wxT("DUMMYSR is defined: impossible to open SR settings Dlg Box."));
#endif
  SetStatusText( wxT("cam") );
}
//---------------------------------------------------
/*!
	- Opens record files. \n
*/
//! Open record interface
void CamFrame::OnRecord(wxCommandEvent& WXUNUSED(event))
{
  unsigned int res = 0;
  m_settingsPane->SetText(wxT("Record attempt..."));
  wxString strR;

  if( m_settingsPane->IsRecordChecked() )
  {
	wxFileDialog* OpenDialogPha = new wxFileDialog(this,
	  wxT("Choose a SR PHASE file to WRITE to "),	// msg
	  wxT(""), //wxT("D:\\dataSR3\\"),	// default dir
	  wxEmptyString,	// default file
	  wxT("SR phase files (*.16b)|*.16b|All files (*.*)|*.*"),	// file ext
	  wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_OVERWRITE_PROMPT,
	  wxDefaultPosition);
	wxFileDialog* OpenDialogAmp = new wxFileDialog(this,
	  wxT("Choose a SR AMPLITUDE file to WRITE to "),	// msg
	  wxT(""), //wxT("D:\\dataSR3\\"),	// default dir
	  wxEmptyString,	// default file
	  wxT("SR amplitude files (*.16b)|*.16b|All files (*.*)|*.*"),	// file ext
	  wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_OVERWRITE_PROMPT,
	  wxDefaultPosition);
	if( (OpenDialogPha->ShowModal()==wxID_OK) && (OpenDialogAmp->ShowModal()==wxID_OK))
	{
	    wxString strPathPha = OpenDialogPha->GetPath();
	    wxString strPathAmp = OpenDialogAmp->GetPath();
	    m_pFile4WritePha = new wxFFile(strPathPha, wxT("wb"));
	    m_pFile4WriteAmp = new wxFFile(strPathAmp, wxT("wb"));
	    if ( (!(m_pFile4WriteAmp)) || (!(m_pFile4WriteAmp)) || (!(m_pFile4WriteAmp->IsOpened())) || (!(m_pFile4WriteAmp->IsOpened())))
	    {
		  res -=4;
	    }

	    strR.sprintf(wxT("Files opened.")); // ... change text ...

	    m_settingsPane->SetText(strR);
		SetStatusText( strR );
	} // (OpenDialogPha->ShowModal()==wxID_OK) && (OpenDialogAmp->ShowModal()==wxID_OK)
    delete(OpenDialogPha);
    delete(OpenDialogAmp);
  } // 'record' checkbox is checked
  else
  {
	  // Properly close record by deleting wxFile
	  if(m_pFile4WritePha != NULL) { delete(m_pFile4WritePha); m_pFile4WritePha = NULL; };
      if(m_pFile4WriteAmp != NULL) { delete(m_pFile4WriteAmp); m_pFile4WriteAmp = NULL; };
	  // save record params
	  wxFileDialog* OpenDialogPar = new wxFileDialog(this,
	  wxT("Choose a SR parameters file to WRITE TO"),	// msg
	  wxT(""), //wxT("D:\\dataSR3\\"),	// default dir
	  wxEmptyString,	// default file
	  wxT("SR parameters files (*.sr2)|*.sr2|All files (*.*)|*.*"),	// file ext
	  wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_OVERWRITE_PROMPT,
	  wxDefaultPosition);
	  if( (OpenDialogPar->ShowModal()==wxID_OK) )
	  {
	    wxString strPathPar = OpenDialogPar->GetPath();
	    wxFFile* wxFparams = new wxFFile(strPathPar, wxT("wb"));
	    if ( (wxFparams->IsOpened()) )
	    {
	      int tmp = 0; // DANGER = 1 int must be 4 bytes
		  float tmpFlt = 0;
		  if(sizeof(int)!=4)
		  {
			  res-=64;
			  m_nCols = (int) 176;
			  m_nRows = (int) 144;
			  m_nSrBufSz = (int) (m_nCols*m_nRows*2*2); // *2 amp,pha *2:short
		  }
		  else // (sizeof(int)!=4)
		  {
			tmp = m_nRows;
	        size_t wrBytes= wxFparams->Write(&tmp, sizeof(int));
			tmp = m_nCols;
		    wrBytes= wxFparams->Write(&tmp, sizeof(int));
		    tmp = 2;
		    wrBytes= wxFparams->Write(&tmp, sizeof(int));
		    tmp = (int) sizeof(unsigned short);
		    wrBytes = wxFparams->Write(&tmp, sizeof(int));
		    tmp = m_nSrBufSz;
		    wrBytes= wxFparams->Write(&tmp, sizeof(int));
		    tmp = m_nSerialSR;
		    wrBytes= wxFparams->Write(&tmp, sizeof(unsigned int));
		    tmp = m_srFrq;
		    wrBytes = wxFparams->Write(&tmp, sizeof(int));
		    /*		float focal = 8.0f;   //mm
					float pixSzX = 0.04f; //mm
					float pixSzY = 0.04f; //mm
					float centerX = 95.1f ; float centerY = 56.3f ; // pixels*/
			tmpFlt = m_ctrParam.f;
		    wrBytes = wxFparams->Write(&tmpFlt, sizeof(float));
		    tmpFlt = m_ctrParam.pixDX;
		    wrBytes = wxFparams->Write(&tmpFlt, sizeof(float));
		    tmpFlt = m_ctrParam.pixDY;
		    wrBytes = wxFparams->Write(&tmpFlt, sizeof(float));
		    tmpFlt = m_ctrParam.cX;
		    wrBytes = wxFparams->Write(&tmpFlt, sizeof(float));
		    tmpFlt = m_ctrParam.cY;
		    wrBytes = wxFparams->Write(&tmpFlt, sizeof(float));
		    tmpFlt = m_ctrParam.maxMM;
		    wrBytes = wxFparams->Write(&tmpFlt, sizeof(float));
		} // (sizeof(int)!=4)
		delete(wxFparams);
	  }
	  delete(OpenDialogPar);
  } // 'record' checkbox is not checked
}
}

//! Acquire 1 Frame
void CamFrame::Acquire(wxCommandEvent& WXUNUSED(event))
{
  if(m_camReadMode==CAM_RD_ONESHOT) // if one shot read is asked ...
  {
	AcqOneFrm();					// ... do so
  }
  else if(m_camReadMode==CAM_RD_CONTINU)	// if continuous read is asked...
  {
    if( ! m_bReadContinuously )		// ... if reading was not in progress ...
	{
		m_settingsPane->DisableRadioReadMode();  //disallow changing read mode
		m_bReadContinuously = true; // set reading flag
		m_settingsPane->SetBtnTxtStop();		// set new texts in ctrl buttons
		m_settingsPane->DisableRadioFilt();	// disable filter selection
		m_settingsPane->DisableRadioFrq();	// disable frequency selection

		m_pThreadReadData = new ThreadReadData(this);	// create reading thread...
		m_pThreadReadData->Create();
		m_pThreadReadData->Run();						// ... and run it.
	} //( ! m_bReadContinuously )
	else							// ... if reading was in progress ...
	{
		m_pThreadReadData->Delete();  // ... kill (gracefully) reading thread
		m_settingsPane->EnableRadioReadMode();  //allow changing read mode
		m_bReadContinuously = false;
		m_settingsPane->SetBtnTxtAcqu();		// set new texts in ctrl buttons
		m_settingsPane->EnableRadioFilt();	// enable filter selection
		m_settingsPane->EnableRadioFrq();	// enable frequency selection

	} //( ! m_bReadContinuously )
  }

  return;
};
//! Acquire 1 Frame
void CamFrame::AcqOneFrm()
{
  int res = 0;
  wxString strR;
  if(m_pAcqSWatch != NULL){  m_pAcqSWatch->Start();};
  // if buffers are defined
  if( (m_pSrBuf != NULL) && (m_CTrf!=NULL) )
  {
	  wxMutexError errMutex= m_mutexSrBuf->Lock();
	  if(errMutex == wxMUTEX_NO_ERROR)
	  {
		  // if reading from file ...
		  if((m_sr == NULL) &&(m_pFile4ReadPha  != NULL) && (m_pFile4ReadAmp  != NULL) && (m_pFile4ReadPha->IsOpened()) && (m_pFile4ReadAmp->IsOpened()))
		  {
			res = (int) m_pFile4ReadPha->Read(m_pSrBuf, (int)m_nCols*(int)m_nRows*sizeof(unsigned short));
			res = (int) m_pFile4ReadAmp->Read(&m_pSrBuf[(int)m_nCols*(int)m_nRows*sizeof(unsigned short)], (int)m_nCols*(int)m_nRows*sizeof(unsigned short));
			m_nFrmRead +=1;
			if( (m_pFile4ReadPha->Eof()) || (m_pFile4ReadAmp->Eof()) )
			{  // when read files are finished (EOF)
				m_pFile4ReadPha->Seek(0); // rewind phase
				m_pFile4ReadAmp->Seek(0); // rewind phase
				m_nFrmRead = 0;	// reset frame count
				// close writer files when record loops, to avoid writing indefinitely
				if((m_pFile4WritePha  != NULL) && (m_pFile4WritePha->IsOpened())) { m_pFile4WritePha->Close();};
				if((m_pFile4WriteAmp  != NULL) && (m_pFile4WriteAmp->IsOpened())) { m_pFile4WriteAmp->Close();};
				if((m_pFile4WriteXYZ  != NULL) && (m_pFile4WriteXYZ->IsOpened())) { m_pFile4WriteXYZ->Close();};
				if((m_pFile4WriteSeg  != NULL) && (m_pFile4WriteSeg->IsOpened())) { m_pFile4WriteSeg->Close();};
			}
		  }
		  else
		  { // not reading from file -> ACQUIRING
			  if((m_sr != NULL) && (m_pSrBuf != NULL) && (m_CTrf!=NULL) )
			  {
				res = SR_SetMode(m_sr, AM_COR_FIX_PTRN ); //|| AM_COR_LED_NON_LIN );
				res = SR_Acquire(m_sr);
				memcpy( (void*) m_pSrBuf, SR_GetImage(m_sr,0), m_nRows*m_nCols*sizeof(unsigned short));
				memcpy( (void*) (&m_pSrBuf[m_nRows*m_nCols*sizeof(unsigned short)]), SR_GetImage(m_sr,1), m_nRows*m_nCols*sizeof(unsigned short));
				m_nFrmRead +=1;
			  }
		  } // END OF if changing behavior if reading from file or Acquring from cam
		    SRBUF srBuf;
		    srBuf.pha = (unsigned short*) m_pSrBuf;
		    srBuf.amp = (unsigned short*) (&m_pSrBuf[m_nRows*m_nCols*sizeof(unsigned short)]);
		    srBuf.nCols = m_nCols;
		    srBuf.nRows = m_nRows;
		    srBuf.bufferSizeInBytes = m_nCols*m_nRows*2*sizeof(unsigned short);
			if( !(m_settingsPane->IsNoFlagNaNChecked()))
			{  // flag NaNs except if not desired (only for illustration purposes)
				res+=PLNN_FlagNaN(m_NaN, srBuf);
			}
		if( (m_settingsPane->IsFgHidesDataChecked()) && (m_settingsPane->IsLrnFgChecked()!=true) )
		{ // replace camera buf data by recorded foreground if GUI checkbox is checked
			SRBUF fgBuf = PLAVG_GetAvgBuf(m_fgAvg);
			memcpy( (void*) m_pSrBuf, fgBuf.pha , m_nRows*m_nCols*sizeof(unsigned short));
			memcpy( (void*) (&m_pSrBuf[m_nRows*m_nCols*sizeof(unsigned short)]), fgBuf.amp , m_nRows*m_nCols*sizeof(unsigned short));
		}
		if(m_settingsPane->IsScatChecked())
		{ // compensate for scattering if GUI checkbox is cheked
			PLSC_Compensate(m_scat, srBuf, PLNN_GetNaNbuf(m_NaN));
		}
		if(m_settingsPane->IsLrnBgChecked())
		{ //  learn background
			PLAVG_LearnBackground(m_bgAvg, srBuf);
			if( !(m_settingsPane->IsNoFlagNaNChecked()))
			{
				res+=PLNN_FlagNaN(m_bgNaN, PLAVG_GetAvgBuf(m_bgAvg));
			}
		}
		if(m_settingsPane->IsLrnFgChecked())
		{ // learn foreground
			PLAVG_LearnBackground(m_fgAvg, srBuf);
			if( !(m_settingsPane->IsNoFlagNaNChecked()))
			{
				res+=PLNN_FlagNaN(m_fgNaN, PLAVG_GetAvgBuf(m_fgAvg));
			}
		}
		PLCTR_CoordTrf(m_CTrf, srBuf, m_ctrParam);  // transform coordinates
		errMutex = m_mutexSrBuf->Unlock();
	  } //{errMutex == wxMUTEX_NO_ERROR)
	  if((m_pFile4WritePha  != NULL) && (m_pFile4WriteAmp  != NULL) && (m_pFile4WritePha->IsOpened()) && (m_pFile4WriteAmp->IsOpened()))
	  {  // write phase and amplitude data to file
		  res = (int) m_pFile4WritePha->Write(m_pSrBuf, (int)m_nCols*(int)m_nRows*sizeof(unsigned short));
		  res = (int) m_pFile4WriteAmp->Write(&m_pSrBuf[(int)m_nCols*(int)m_nRows*sizeof(unsigned short)], (int)m_nCols*(int)m_nRows*sizeof(unsigned short));
	  }
	  #ifdef JMU_TGTFOLLOW
		if(m_pFile4TgtCoord != NULL)
		{
			float frmCntFl;
			float tgt[15];
			res  =(int) m_pFile4TgtCoord->Read(&frmCntFl, sizeof(float));
			res  =(int) m_pFile4TgtCoord->Read(&tgt, 15*sizeof(float));
			if( m_pFile4TgtCoord->Eof() )
			{
			  res = m_pFile4TgtCoord->Seek(0, wxFromStart);
			  _camVtk->updateTarget();  //to reset target
			}
			if( ((int) frmCntFl) != m_nFrmRead )
			{
				res = m_pFile4TgtCoord->Seek(0, wxFromStart);
			}
			else
			{
				//_camVtk->updateTarget(&(tgt[0]), 3); // to update only line
				_camVtk->updateTarget(&(tgt[0]), 15); // to update line and triangles
			}
		}
	  #endif // JMU_TGTFOLLOW
	  #ifdef JMU_RANSAC
		_camVtk->updateRscPla(); // erase ransac plane, so that it doesn't clutter display
	  #endif

	  strR.sprintf(wxT("frm:%05u - pixFileRead %i - %ix%i  - %i"), m_nFrmRead, res, m_nRows, m_nCols, m_nSrBufSz);
	  m_viewRangePane->SetDataArray<unsigned short>((unsigned short*) &m_pSrBuf[0], m_nRows*m_nCols);
	  m_viewAmpPane->SetDataArray<unsigned short>((unsigned short*) &m_pSrBuf[m_nCols*m_nRows*2], m_nRows*m_nCols);
	  m_viewZPane->SetDataArray<unsigned short>(PLCTR_GetZ(m_CTrf), m_nRows*m_nCols);
	  #ifdef DISPXYBUFFERS
	    m_viewYPane->SetDataArray<short>(PLCTR_GetY(m_CTrf), m_nRows*m_nCols);
	    m_viewXPane->SetDataArray<short>(PLCTR_GetX(m_CTrf), m_nRows*m_nCols);
	  #endif //DISPXYBUFFERS
		SRBUF srBuf;
		    srBuf.pha = (unsigned short*) m_pSrBuf;
		    srBuf.amp = (unsigned short*) (&m_pSrBuf[m_nRows*m_nCols*sizeof(unsigned short)]);
		    srBuf.nCols = m_nCols;
		    srBuf.nRows = m_nRows;
		    srBuf.bufferSizeInBytes = m_nCols*m_nRows*2*sizeof(unsigned short);
	  // res+=PLNN_FlagNaN(m_NaN, srBuf); // already done on line 760
	  //res+=PLNN_FlagNaN(m_NaNbg, PLAVG_GetAvgBuf(m_bgAvg)); // already done on line 774
	  PLSEGM_Segment(m_segm, srBuf, PLNN_GetNaNbuf(m_NaN),
				PLAVG_GetAvgBuf(m_bgAvg), PLNN_GetNaNbuf(m_bgNaN), PLAVG_GetAvgVar(m_bgAvg) );
	  unsigned char* segmBuf = NULL;
	  if(IsSegmChecked())
	  {
		  PLSEGM_SegmentXYZ(m_segm, srBuf, PLNN_GetNaNbuf(m_NaN),
			  PLAVG_GetAvgBuf(m_bgAvg), PLNN_GetNaNbuf(m_bgNaN), PLAVG_GetAvgVar(m_bgAvg),
			  PLCTR_GetZ(m_CTrf), PLCTR_GetZ(m_CTrfBG));
		  segmBuf = (unsigned char*) (PLSEGM_GetSegmBuf(m_segm)).bg;
	  }
	  else
	  {
		  segmBuf = (unsigned char*) (PLSEGM_GetSegmBuf(m_segm)).fg;
	  }
	  m_viewSegmPane->SetDataArray<unsigned char>( segmBuf, m_nRows*m_nCols);
	  m_viewBGRangePane->SetDataArray<unsigned short>((unsigned short*) (PLAVG_GetAvgBuf(m_bgAvg)).pha, m_nRows*m_nCols);
	  m_viewBGAmpPane->SetDataArray<unsigned short>((unsigned short*) (PLAVG_GetAvgBuf(m_bgAvg)).amp, m_nRows*m_nCols);
	  #ifdef JMU_USE_VTK
		//if(m_camReadMode==CAM_RD_ONESHOT) // vtk does not support access from different threads
	    // flag for supporting blanking of vtk data with segm results
		_camVtk->setBlankSegm(m_settingsPane->IsBlankSegmVTKChecked());
		// will update TOF VTK display, but function call is different if VTK data shall be saved
		if(m_settingsPane->IsRecVTKChecked())
		{ // when wanting to save, a filename must be provided
			wxString curImg;
			curImg.Printf(wxT("%04u.vtk"), m_nFrmRead); curImg = _vtkRecPrefix + curImg;
			char charBuf[512];
			strcpy( charBuf, (const char*) curImg.char_str() );
			_camVtk->updateTOF(m_nRows, m_nCols, PLCTR_GetZ(m_CTrf), PLCTR_GetY(m_CTrf), PLCTR_GetX(m_CTrf), (unsigned short*) &m_pSrBuf[m_nCols*m_nRows*2], segmBuf, charBuf);
		}
		else
		{ // when not saving, just forget entering a filename
			_camVtk->updateTOF(m_nRows, m_nCols, PLCTR_GetZ(m_CTrf), PLCTR_GetY(m_CTrf), PLCTR_GetX(m_CTrf), (unsigned short*) &m_pSrBuf[m_nCols*m_nRows*2], segmBuf);
		}
		if(m_settingsPane->IsLrnBgChecked())
		{
			PLCTR_CoordTrf(m_CTrfBG, PLAVG_GetAvgBuf(m_bgAvg), m_ctrParam);
			_camBGVtk->updateTOF(m_nRows, m_nCols, PLCTR_GetZ(m_CTrfBG), PLCTR_GetY(m_CTrfBG), PLCTR_GetX(m_CTrfBG), PLAVG_GetAvgBuf(m_bgAvg).amp,segmBuf);
		}
		if(m_settingsPane->IsLrnFgChecked())
		{
			PLCTR_CoordTrf(m_CTrfFG, PLAVG_GetAvgBuf(m_fgAvg), m_ctrParam);
			_camFGVtk->updateTOF(m_nRows, m_nCols, PLCTR_GetZ(m_CTrfFG), PLCTR_GetY(m_CTrfFG), PLCTR_GetX(m_CTrfFG), PLAVG_GetAvgBuf(m_fgAvg).amp,segmBuf);
		}
		if(!m_bReadContinuously)
		{
			_vtkWin->Render(); // avoid rendering twice for BG and FG ; PROBLEM rendering is done once for each camera, should be done only once :-(
		}
      #endif
	  if((m_pFile4WriteXYZ  != NULL) && (m_pFile4WriteXYZ->IsOpened()))
	  {  // write XYZ data to file
		  res = (int) m_pFile4WriteXYZ->Write(PLCTR_GetX(m_CTrf), (int)m_nCols*(int)m_nRows*sizeof(short));
		  res = (int) m_pFile4WriteXYZ->Write(PLCTR_GetY(m_CTrf), (int)m_nCols*(int)m_nRows*sizeof(short));
		  res = (int) m_pFile4WriteXYZ->Write(PLCTR_GetZ(m_CTrf), (int)m_nCols*(int)m_nRows*sizeof(short));
	  }
	  if((m_pFile4WriteSeg  != NULL) && (m_pFile4WriteSeg->IsOpened()))
	  {  // write SEGMENTATION data to file
		  res = (int) m_pFile4WriteSeg->Write(segmBuf, (int)m_nCols*(int)m_nRows*sizeof(unsigned char));
	  }
	  m_settingsPane->SetText(strR);
	  m_viewRangePane->SetTxtInfo(strR);
	  m_viewAmpPane->SetTxtInfo(strR);
	  m_viewZPane->SetTxtInfo(strR);
	  #ifdef DISPXYBUFFERS
	    m_viewYPane->SetTxtInfo(strR);
	    m_viewXPane->SetTxtInfo(strR);
	  #endif //DISPXYBUFFERS
	  m_viewSegmPane->SetTxtInfo(strR);
	  wxString strBG; strBG.sprintf(wxT("Average count: %05i"), PLAVG_GetAvgCnt(m_bgAvg));
	  m_viewBGRangePane->SetTxtInfo(strBG);
	  m_viewBGAmpPane->SetTxtInfo(strBG);

  }
  //if((m_viewRangePane->IsShownOnScreen())){ m_viewRangePane->SetNewImage();};
  //if((m_viewAmpPane->IsShownOnScreen())){ m_viewAmpPane->SetNewImage();};
  if(m_pAcqSWatch != NULL)
  {
	_acqTime = m_pAcqSWatch->Time();
	m_pAcqSWatch->Pause();
  }
};


void CamFrame::AcqOneFrmEvt(wxCommandEvent& WXUNUSED(event))
{
	AcqOneFrm();
}
#ifdef JMU_USE_VTK
void CamFrame::RenderEvt(wxCommandEvent& WXUNUSED(event))
{
	_vtkWin->Render();  // avoid rendering twice for BG and FG ; PROBLEM rendering is done once for each camera, should be done only once :-(
}
#endif

//! Interface fct to set the modulation frequency
void CamFrame::SetFreq(wxCommandEvent&(event))
{
  int res = 0;
  wxString strR;
  //ModulationFrq srFrq = MF_20MHz ;

  wxString strF = event.GetString();
  if( ( strF.Find(wxT("MHz")) != wxNOT_FOUND) )
  {
	  if( ( strF.Find(wxT("19")) != wxNOT_FOUND) )
	  {
		  m_srFrq = MF_19MHz;
		  m_ctrParam.maxMM = 7894.7368f;
	  }
	  if( ( strF.Find(wxT("20")) != wxNOT_FOUND) )
	  {
		  m_srFrq = MF_20MHz;
		  m_ctrParam.maxMM = 7500.0;
	  }
	  if( ( strF.Find(wxT("21")) != wxNOT_FOUND) )
	  {
		  m_srFrq = MF_21MHz;
		  m_ctrParam.maxMM = 7142.8571f;
	  }
	  if( ( strF.Find(wxT("30")) != wxNOT_FOUND) )
	  {
		  m_srFrq = MF_30MHz;
		  m_ctrParam.maxMM = 5000.0;
	  }
  }
  strR.sprintf(wxT("Mod Frq dummy = %i "), m_srFrq);
  m_settingsPane->SetText(strR);
  if((m_sr != NULL) )
  {
	  res = SR_SetModulationFrequency(m_sr, m_srFrq);
	  ModulationFrq srFrq = SR_GetModulationFrequency(m_sr);
	  strR.sprintf(wxT("Mod Frq SR = %i "), srFrq);
	  m_settingsPane->SetText(strR);
  }
};

//! Interface fct to set the read mode
void CamFrame::SetReadMode(wxCommandEvent&(event))
{
  wxString strR;
  wxString strF = event.GetString();
  if( ( strF.Find(wxT("Continuous")) != wxNOT_FOUND) )
  {
	  m_camReadMode = CAM_RD_CONTINU;
	  strR.sprintf(wxT("Cam read mode = %s "), "Continuous");
  }
  else if( ( strF.Find(wxT("1 frame")) != wxNOT_FOUND) )
  {
	  m_camReadMode = CAM_RD_ONESHOT;
	  strR.sprintf(wxT("Cam read mode = %s "), "One frame");
  }
  m_settingsPane->SetText(strR);
};

#ifdef JMU_USE_VTK
void CamFrame::SetVtkWin(CViewSrVtk *vtkWin, int vtkSub)
{
	if( (vtkSub > -1) && (vtkSub < NUMCAMS))
	{
		_vtkSub = vtkSub;
	}
	else{return;};

	_vtkWin = vtkWin;
	if( (vtkSub > -1) && (vtkSub < NUMCAMS))
	{
		_camVtk = new CamVtkView(_vtkSub, _vtkWin->GetRenWin(), _vtkWin->GetDepthLUT());
		_camVtk->SetDepthLUT(_vtkWin->GetDepthLUT());
		_camVtk->SetGrayLUT(_vtkWin->GetGrayLUT());
		_camVtk->SetSegmLUT(_vtkWin->GetSegmLUT());
		_camVtk->SetPlainLUT(_vtkWin->GetRLUT(), _vtkWin->GetGLUT(), _vtkWin->GetBLUT(), _vtkWin->GetWLUT(), _vtkWin->GetKLUT());
		_camBGVtk = new CamVtkView(_vtkSub, _vtkWin->GetRenWin(), _vtkWin->GetDepthLUT());
		_camBGVtk->SetDepthLUT(_vtkWin->GetDepthLUT());
		_camBGVtk->SetGrayLUT(_vtkWin->GetGrayLUT());
		_camBGVtk->SetSegmLUT(_vtkWin->GetSegmLUT());
		_camBGVtk->SetPlainLUT(_vtkWin->GetRLUT(), _vtkWin->GetGLUT(), _vtkWin->GetBLUT(), _vtkWin->GetWLUT(), _vtkWin->GetKLUT());
		_camBGVtk->setDataRepBG();
		_camFGVtk = new CamVtkView(_vtkSub, _vtkWin->GetRenWin(), _vtkWin->GetDepthLUT());
		_camFGVtk->SetDepthLUT(_vtkWin->GetDepthLUT());
		_camFGVtk->SetGrayLUT(_vtkWin->GetGrayLUT());
		_camFGVtk->SetSegmLUT(_vtkWin->GetSegmLUT());
		_camFGVtk->SetPlainLUT(_vtkWin->GetRLUT(), _vtkWin->GetGLUT(), _vtkWin->GetBLUT(), _vtkWin->GetWLUT(), _vtkWin->GetKLUT());
		_camFGVtk->setDataRepSurface(); _camFGVtk->setDataOpacity(0.7);
		/*_camSegmVtk = new CamVtkView(_vtkSub, _vtkWin->GetRenWin(), _vtkWin->GetDepthLUT());
		_camSegmVtk->SetDepthLUT(_vtkWin->GetDepthLUT());
		_camSegmVtk->SetGrayLUT(_vtkWin->GetGrayLUT());
		_camSegmVtk->SetSegmLUT(_vtkWin->GetSegmLUT());
		_camSegmVtk->SetPlainLUT(_vtkWin->GetRLUT(), _vtkWin->GetGLUT(), _vtkWin->GetBLUT(), _vtkWin->GetWLUT(), _vtkWin->GetKLUT());
		_camSegmVtk->setDataRepPoints(); _camSegmVtk->setDataOpacity(0.9);*/
	}
};
#endif

void CamFrame::OnSetTrfMat(wxCommandEvent& WXUNUSED(event))
{
#ifdef JMU_USE_VTK
  //logPrintf("swissrangerTester: SR_Open device");
  m_settingsPane->SetText(wxT("Modify camera trf matrix"));
  wxString strR;

  wxFileDialog* OpenDialogMat = new wxFileDialog(this,
	  wxT("Choose a coordinates transformation matrix file to open"),	// msg
	  wxT(""), //wxT("D:\\dataSR3\\"),	// default dir
	  wxEmptyString,	// default file
	  wxT("Coordinates transformation matrix files (*.xml)|*.xml|All files (*.*)|*.*"),	// file ext
	  wxFD_OPEN|wxFD_CHANGE_DIR,
	  wxDefaultPosition);

  if((OpenDialogMat->ShowModal()==wxID_OK) )
  {
	  wxString strPathMat = OpenDialogMat->GetPath();
	  _camVtk->setTrfMat(strPathMat.char_str());
	  _camBGVtk->setTrfMat(strPathMat.char_str());
	  _camFGVtk->setTrfMat(strPathMat.char_str());
	  //_camSegmGVtk->setTrfMat(strPathMat.char_str());
  } // (OpenDialogMat->ShowModal()==wxID_OK) )
  delete(OpenDialogMat);

  m_settingsPane->SetText(wxT("Modified camera trf matrix"));
#endif
}

#ifdef JMU_TGTFOLLOW
void CamFrame::OnTgtFile(wxCommandEvent& WXUNUSED(event))
{
  //#ifdef JMU_TGTFOLLOW

  //logPrintf("swissrangerTester: SR_Open device");
  m_settingsPane->SetText(wxT("Modify camera target file"));
  wxString strR;

  wxFileDialog* OpenDialogTgt = new wxFileDialog(this,
	  wxT("Choose a target coordinates file to open"),	// msg
	  wxT(""), //wxT("D:\\dataSR3\\"),	// default dir
	  wxEmptyString,	// default file
	  wxT("Target coordinates files (*.dat)|*.dat|All files (*.*)|*.*"),	// file ext
	  wxFD_OPEN|wxFD_CHANGE_DIR,
	  wxDefaultPosition);

  if((OpenDialogTgt->ShowModal()==wxID_OK) )
  {
	  wxString strPathTgt = OpenDialogTgt->GetPath();
	  m_pFile4TgtCoord = new wxFFile(strPathTgt, wxT("rb"));
  } // (OpenDialogMat->ShowModal()==wxID_OK) )
  delete(OpenDialogTgt);

  m_settingsPane->SetText(wxT("Opened target file"));
  //#endif
}
#endif

void CamFrame::OnSetScatParams(wxCommandEvent& WXUNUSED(event))
{
  //#ifdef JMU_TGTFOLLOW

  //logPrintf("swissrangerTester: SR_Open device");
  m_settingsPane->SetText(wxT("Modify scattering compensation parameters"));
  //wxString strR;

  wxFileDialog* OpenDialogScat = new wxFileDialog(this,
	  wxT("Choose a scattering compensation parameters file to open"),	// msg
	  wxT(""), //wxT("D:\\dataSR3\\"),	// default dir
	  wxEmptyString,	// default file
	  wxT("Scattering compensation parameter files (*.xml)|*.xml|All files (*.*)|*.*"),	// file ext
	  wxFD_OPEN|wxFD_CHANGE_DIR,
	  wxDefaultPosition);

  if((OpenDialogScat->ShowModal()==wxID_OK) )
  {
	  wxString strPathScat = OpenDialogScat->GetPath();
	  PLSC_LoadScatSettings( m_scat, strPathScat.char_str() );
  } // (OpenDialogMat->ShowModal()==wxID_OK) )
  delete(OpenDialogScat);

  m_settingsPane->SetText(wxT("Opened scat. params file"));
  //#endif
}

void CamFrame::OnClearBg(wxCommandEvent& WXUNUSED(event))
{
  m_settingsPane->SetText(wxT("Clear background"));
  SRBUF srBuf;
		    srBuf.pha = (unsigned short*) m_pSrBuf;
		    srBuf.amp = (unsigned short*) (&m_pSrBuf[m_nRows*m_nCols*sizeof(unsigned short)]);
		    srBuf.nCols = m_nCols;
		    srBuf.nRows = m_nRows;
		    srBuf.bufferSizeInBytes = m_nCols*m_nRows*2*sizeof(unsigned short);
  PLAVG_LearnBackgroundInitReset( m_bgAvg, srBuf);
//#ifdef JMU_USE_VTK // not really a good fix
//  _camBGVtk->hideDataAct(true);
//#endif
  m_settingsPane->SetText(wxT("Cleared background"));
}

void CamFrame::OnClearFg(wxCommandEvent& WXUNUSED(event))
{
  m_settingsPane->SetText(wxT("Clear foreground"));
  SRBUF srBuf;
		    srBuf.pha = (unsigned short*) m_pSrBuf;
		    srBuf.amp = (unsigned short*) (&m_pSrBuf[m_nRows*m_nCols*sizeof(unsigned short)]);
		    srBuf.nCols = m_nCols;
		    srBuf.nRows = m_nRows;
		    srBuf.bufferSizeInBytes = m_nCols*m_nRows*2*sizeof(unsigned short);
  PLAVG_LearnBackgroundInitReset( m_fgAvg, srBuf);
  m_settingsPane->SetText(wxT("Cleared foreground"));
}


void CamFrame::OnSetSegmParams(wxCommandEvent& WXUNUSED(event))
{
  //#ifdef JMU_TGTFOLLOW

  //logPrintf("swissrangerTester: SR_Open device");
  m_settingsPane->SetText(wxT("Modify segmentation parameters"));
  //wxString strR;

  wxFileDialog* OpenDialogSegm = new wxFileDialog(this,
	  wxT("Choose a segmentation parameters file to open"),	// msg
	  wxT(""), //wxT("D:\\dataSR3\\"),	// default dir
	  wxEmptyString,	// default file
	  wxT("SR segmentation parameter files (*.xml)|*.xml|All files (*.*)|*.*"),	// file ext
	  wxFD_OPEN|wxFD_CHANGE_DIR,
	  wxDefaultPosition);

  if((OpenDialogSegm->ShowModal()==wxID_OK) )
  {
	  wxString strPathSegm = OpenDialogSegm->GetPath();
	  PLSEGM_LoadSegmSettings( m_segm, strPathSegm.char_str() );
  } // (OpenDialogMat->ShowModal()==wxID_OK) )
  delete(OpenDialogSegm);

  m_settingsPane->SetText(wxT("Opened scat. params file"));
  //_camVtk->changeSegmRange((float) 255.0f, (float) 0.0f);
  _pWin->SetSegmVtk();
  //#endif
}

#ifdef JMU_RANSAC
void CamFrame::RansacBG(wxCommandEvent& WXUNUSED(event))
{
    /*
	  unsigned char* segmBuf = NULL;
	  if(IsSegmChecked())
	  {
		  segmBuf = (unsigned char*) (PLSEGM_GetSegmBuf(m_segm)).bg;
	  }
	  else
	  {
		  segmBuf = (unsigned char*) (PLSEGM_GetSegmBuf(m_segm)).fg;
	  }
  //TODO for RANSAC on segmented data only
      PLRSC_SetSegmMap(m_ransac, PLAVG_GetAvgBuf(m_bgAvg), segmBuf)
  // but then careful what segmIdx to input !!!
	 */ 
  // call worker ransac function
  _rscSucc =0;
  PLRSC_ransac(m_ransac, PLAVG_GetAvgBuf(m_bgAvg), PLCTR_GetZ(m_CTrfBG), PLCTR_GetY(m_CTrfBG), PLCTR_GetX(m_CTrfBG), PLNN_GetBoolBuf(m_bgNaN), _rscSucc);
  this->RansacFollow();
  _rscSucc +=1;
  /*this->RansacBGsucc(WXUNUSED(event));*/
  return;
}
#endif // JMU_RANSAC
#ifdef JMU_RANSAC
void CamFrame::RansacBGsucc(wxCommandEvent& WXUNUSED(event))
{
  PLRSC_ransac(m_ransac, PLAVG_GetAvgBuf(m_bgAvg), PLCTR_GetZ(m_CTrfBG), PLCTR_GetY(m_CTrfBG), PLCTR_GetX(m_CTrfBG), PLNN_GetBoolBuf(m_bgNaN), _rscSucc);
  this->RansacFollow();
  _rscSucc +=1;
  return;
}
#endif // JMU_RANSAC
#ifdef JMU_RANSAC
void CamFrame::RansacFollow()
{
  RSCPLAN pla = PLRSC_GetPlaBest(m_ransac);			// Get best plane
  double distPla = PLRSC_GetDistPla(m_ransac);		// get parameter used as inlier threshold
  int iterMax = PLRSC_GetIterMax(m_ransac);			// get parameter number of iter
  double mat4[16]; for(int k=0; k<16; k++){mat4[k]=0;}; mat4[15]=1; 
	for(int k=0; k<16; k++)
	{
		mat4[k]= pla.matZ[k] ; 
	}
	// translation along z, will allow to have all RANSAC planes at same Z, fixed now at 3000
	mat4[14] = 0000 - pla.nVec[3]; // HARDCODED ALIGNMENT DEPTH 3000 mm
  if((_rscSucc<3) ) // the quickFix for GCC warning caused a failure :-/, should be ok now
  {
	  plaAli[_rscSucc]=pla;
  }
  double* nVec = &(pla.nVec[0]);
  wxString strS; wxString strSParam; wxString strSResLine; wxString strSVec; wxString strSMat;
  strSParam.Printf(wxT(" Ransac LOOPED %02i times - %05i outliers: %05i iterations - Inlier dist threshold: %04g mm. \n"), _rscSucc, pla.outliers.size(), iterMax, distPla); 
  strSResLine.Printf(wxT("Best plane at iter: %i  - %05i inliers. \n"), pla.iter, pla.inliers.size());
  strSVec.Printf(wxT("  %+06.4G x  %+06.4G y  %+06.4G z  %+06.4G  = 0 \n"), nVec[0], nVec[1], nVec[2], nVec[3]);
  /*strSMat.Printf(wxT("[%g %g %g \n  %g %g %g \n %g %g %g ] \n" ),
				  mat[0],mat[3],mat[6],mat[1],mat[4],mat[7],mat[2],mat[5],mat[8]);*/
  strSMat.Printf(wxT("[%+0.6f %+0.6f %+0.6f %+0.6f \n  %+0.6f %+0.6f %+0.6f %+0.6f \n  %+0.6f %+0.6f %+0.6f %+0.6f \n   %+0.6f %+0.6f %+0.6f %+0.6f ] \n" ),
		mat4[0], mat4[4], mat4[8], mat4[12],mat4[1], mat4[5], mat4[9], mat4[13],
		mat4[2], mat4[6], mat4[10], mat4[14], mat4[3], mat4[7], mat4[11], mat4[15]);
  strS = strSParam + strSResLine + strSVec + strSMat;
  m_settingsPane->SetText(strS);
  if(pla.iter < 0){ return;}; // return here to display of bad plane since ...
							  // ... a number of iter < 0 indicates a RANSAC error
  wxString fname; fname.Printf(wxT("rscBGCam_%02u_pla%02u.xml"), _vtkSub, _rscSucc);
  wxDateTime now = wxDateTime::Now(); wxString date1 = now.Format();
  wxString comments; 
  comments =  date1 + wxString(wxT("\n")) + strS;
  this->WriteCamTrfMat4(fname, mat4, comments);

  // OPTION 1 was to hijack the target follower. Unfortunately, this was not enough
		{
			double* nVec = &(pla.nVec[0]);
			//float frmCntFl;
			float tgt[15];
			tgt[ 0] = PLCTR_GetAvgX(m_CTrfBG);
			tgt[ 1] = PLCTR_GetAvgY(m_CTrfBG);
			tgt[ 3] = PLCTR_GetMinX(m_CTrfBG);
			tgt[ 4] = PLCTR_GetMinY(m_CTrfBG);
			tgt[ 6] = PLCTR_GetMinX(m_CTrfBG);
			tgt[ 7] = PLCTR_GetMaxY(m_CTrfBG);
			tgt[ 9] = PLCTR_GetMaxX(m_CTrfBG);
			tgt[10] = PLCTR_GetMaxY(m_CTrfBG);
			tgt[12] = PLCTR_GetMaxX(m_CTrfBG);
			tgt[13] = PLCTR_GetMinY(m_CTrfBG);
			for(int p=2; p<15; p+=3)
			{
				tgt[p] = -(nVec[0]*tgt[p-2] + nVec[1]*tgt[p-1] + nVec[3] ) / nVec[2];
			}
				//_camVtk->updateRscPla(&(tgt[0]), 3); // to update only line
				_camVtk->updateRscPla(&(tgt[0]), 15); // to update line and triangles
		}

		// DISPLAY OPTION 2: blank the points that don't belong
		std::vector<int>::iterator it;
		int pix = 0; int row=0; int col=0;
		for(it=pla.inliers.begin(); it!=pla.inliers.end(); it++)
		{ // seems like vtk is col major while my buffers are row-major
			//pix = *it;
			row = *it / m_nCols;
			col = *it % m_nCols;
			pix = col * m_nRows + row;
			_camVtk->GetStructGrid()->BlankPoint(pix);
		}
		_camVtk->GetStructGrid()->Modified();
		_camVtk->GetStructGrid()->Update();
		_vtkWin->Render();

  return;
}
#endif // JMU_RANSAC
#ifdef JMU_RANSAC
void CamFrame::RansacFG(wxCommandEvent& WXUNUSED(event))
{  // NOT IMPLEMENTED YET
   // int res = PLRSC_ransac(m_ransac, srBuf, PLCTR_GetZ(m_CTrf), PLCTR_GetY(m_CTrf), PLCTR_GetX(m_CTrf), PLNN_GetBoolBuf(m_NaN), 0);
  return;
}
#endif // JMU_RANSAC
#ifdef JMU_RANSAC
void CamFrame::OnSetRansacNiterMax(wxCommandEvent& event)
{
    int val = 0;
	val = m_settingsPane->GetRansacNiterMax();
	PLRSC_SetIterMax(m_ransac, val);
}
#endif // JMU_RANSAC
#ifdef JMU_RANSAC
void CamFrame::OnSetRansacDistPla(wxCommandEvent& event)
{
    double val = 0;
	val = m_settingsPane->GetRansacDistPla();
	PLRSC_SetDistPla(m_ransac, val);
}
#endif // JMU_RANSAC

//---------------------------------------------------
/*!
	- Opens record file for XYZ data. \n
*/
//! Open record interface
void CamFrame::OnRecXYZ(wxCommandEvent& WXUNUSED(event))
{
  unsigned int res = 0;
  m_settingsPane->SetText(wxT("Record XYZ attempt..."));
  wxString strR;

  if( m_settingsPane->IsRecXYZChecked() )
  {
	wxFileDialog* OpenDialog = new wxFileDialog(this,
	  wxT("Choose a raw XYZ file to WRITE to "),	// msg
	  wxT(""), //wxT("D:\\dataSR3\\"),	// default dir
	  wxEmptyString,	// default file
	  wxT("Raw XYZ files (ShortShortUShort) (*.raw)|*.raw|All files (*.*)|*.*"),	// file ext
	  wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_OVERWRITE_PROMPT,
	  wxDefaultPosition);
	if( (OpenDialog->ShowModal()==wxID_OK) )
	{
	    wxString strPath = OpenDialog->GetPath();
	    m_pFile4WriteXYZ = new wxFFile(strPath, wxT("wb"));
	    if ( (!(m_pFile4WriteXYZ)) || (!(m_pFile4WriteXYZ->IsOpened())) )
	    {
		  res -=4;
	    }

	    strR.sprintf(wxT("XYZ file opened.")); // ... change text ...

	    m_settingsPane->SetText(strR);
		SetStatusText( strR );
	} // (OpenDialog->ShowModal()==wxID_OK)
    delete(OpenDialog);
  } // 'recXYZ' checkbox is checked
  else
  {
	  // Properly close record by deleting file
	 if(m_pFile4WriteXYZ != NULL) { delete(m_pFile4WriteXYZ); m_pFile4WriteXYZ = NULL; };
  } // 'record' checkbox is not checked
}

//---------------------------------------------------
/*!
	- Opens record file for Seg data. \n
*/
//! Open record interface for seg data
void CamFrame::OnRecSeg(wxCommandEvent& WXUNUSED(event))
{
  unsigned int res = 0;
  m_settingsPane->SetText(wxT("Record Segmentation attempt..."));
  wxString strR;

  if( m_settingsPane->IsRecSegChecked() )
  {
	wxFileDialog* OpenDialog = new wxFileDialog(this,
	  wxT("Choose a raw Segmentation file to WRITE to "),	// msg
	  wxT(""), //wxT("D:\\dataSR3\\"),	// default dir
	  wxEmptyString,	// default file
	  wxT("Raw seg files (Uchar) (*.08b)|*.08b|All files (*.*)|*.*"),	// file ext
	  wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_OVERWRITE_PROMPT,
	  wxDefaultPosition);
	if( (OpenDialog->ShowModal()==wxID_OK) )
	{
	    wxString strPath = OpenDialog->GetPath();
	    m_pFile4WriteSeg = new wxFFile(strPath, wxT("wb"));
	    if ( (!(m_pFile4WriteSeg)) || (!(m_pFile4WriteSeg->IsOpened())) )
	    {
		  res -=4;
	    }

	    strR.sprintf(wxT("Seg file opened.")); // ... change text ...

	    m_settingsPane->SetText(strR);
		SetStatusText( strR );
	} // (OpenDialog->ShowModal()==wxID_OK)
    delete(OpenDialog);
  } // 'recSeg' checkbox is checked
  else
  {
	  // Properly close record by deleting file
	 if(m_pFile4WriteSeg != NULL) { delete(m_pFile4WriteSeg); m_pFile4WriteSeg = NULL; };
  } // 'record' checkbox is not checked
}

//---------------------------------------------------
/*!
	- Opens record file for VTK data. \n
*/
//! Open record interface for VTK data
void CamFrame::OnRecVTK(wxCommandEvent& WXUNUSED(event))
{
#ifdef JMU_USE_VTK
  m_settingsPane->SetText(wxT("Record VTK attempt..."));
  wxString strR;

  if( m_settingsPane->IsRecVTKChecked() )
  {
    wxTextEntryDialog* txtDlg = new wxTextEntryDialog(this, wxT("Choose a prefix for VTK files"), wxT("Please enter prefix"), wxT("vtk_SR3100_noScatComp"));
	if( (txtDlg->ShowModal()==wxID_OK) )
	{
		// save str to some string
		_vtkRecPrefix = txtDlg->GetValue();
		m_settingsPane->SetText(_vtkRecPrefix);
	}
	else
	{
		// set empty string to avoid saving
		m_settingsPane->SetText(wxT("Not recording VTK ..."));
	}
  }
  else
  {
	m_settingsPane->SetText(wxT("Stopped recording VTK."));
  } // 'record' checkbox is not checked
#else
	m_settingsPane->SetText(wxT("CheckBox does nothing since VTK not used..."));
#endif //#ifdef JMU_USE_VTK
}

#ifdef JMU_USE_VTK
void CamFrame::OnBlankSegmThr(wxCommandEvent& event)
{
    unsigned char val = 0;
	val = m_settingsPane->GetBlankSegmThr();
	if(_camVtk == NULL){return;};
	_camVtk->setBlankThr(val);
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
void CamFrame::changeDepthRangeVtk(float zMin, float zMax)
{
	if(_camVtk == NULL){return;};
	_camVtk->changeDepthRange(zMin,zMax);
	if(_camBGVtk == NULL){return;};
	_camBGVtk->changeDepthRange(zMin,zMax);
	if(_camFGVtk == NULL){return;};
	_camFGVtk->changeDepthRange(zMin,zMax);
	//if(_camSegmVtk == NULL){return;};
	//_camSegmVtk->changeDepthRange(zMin,zMax);
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
void CamFrame::changeAmpRangeVtk(float ampMin, float ampMax)
{
	if(_camVtk == NULL){return;};
	_camVtk->changeAmpRange(ampMin,ampMax);
	if(_camBGVtk == NULL){return;};
	_camBGVtk->changeAmpRange(ampMin,ampMax);
	if(_camFGVtk == NULL){return;};
	_camFGVtk->changeAmpRange(ampMin,ampMax);
	//if(_camSegmVtk == NULL){return;};
	//_camSegmVtk->changeAmpRange(ampMin,ampMax);
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
void CamFrame::changeSegmRangeVtk(float segmMin, float segmMax)
{
	if(_camVtk == NULL){return;};
	_camVtk->changeSegmRange(segmMin,segmMax);
	if(_camBGVtk == NULL){return;};
	_camBGVtk->changeSegmRange(segmMin,segmMax);
	if(_camFGVtk == NULL){return;};
	_camFGVtk->changeSegmRange(segmMin,segmMax);
	//if(_camSegmVtk == NULL){return;};
	//_camSegmVtk->changeSegmRange(segmMin,segmMax);
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
void CamFrame::hideDataActVtk(bool doHide, int idx)
{
	if(idx==0)
	{
		if(_camVtk == NULL){return;};
		_camVtk->hideDataAct(doHide);
	}
	if(idx==1)
	{
		if(_camBGVtk == NULL){return;};
		_camBGVtk->hideDataAct(doHide);
	}
	if(idx==2)
	{
		if(_camFGVtk == NULL){return;};
		_camFGVtk->hideDataAct(doHide);
	}
	/*if(idx==3)
	{
		if(_camSegmVtk == NULL){return;};
		_camSegmVtk->hideDataAct(doHide);
	}*/
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
void CamFrame::setDataMapperColorDepth(int idx)
{
	if(idx==0)
	{
		if(_camVtk == NULL){return;};
		_camVtk->setDataMapperColorDepth();
	}
	if(idx==1)
	{
		if(_camBGVtk == NULL){return;};
		_camBGVtk->setDataMapperColorDepth();
	}
	if(idx==2)
	{
		if(_camFGVtk == NULL){return;};
		_camFGVtk->setDataMapperColorDepth();
	}
	/*if(idx==3)
	{
		if(_camSegmVtk == NULL){return;};
		_camSegmVtk->setDataMapperColorDepth();
	}*/
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
void CamFrame::setDataMapperColorGray(int idx)
{
	if(idx==0)
	{
		if(_camVtk == NULL){return;};
		_camVtk->setDataMapperColorGray();
	}
	if(idx==1)
	{
		if(_camBGVtk == NULL){return;};
		_camBGVtk->setDataMapperColorGray();
	}
	if(idx==2)
	{
		if(_camFGVtk == NULL){return;};
		_camFGVtk->setDataMapperColorGray();
	}
	/*if(idx==3)
	{
		if(_camSegmVtk == NULL){return;};
		_camSegmVtk->setDataMapperColorGray();
	}*/
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
void CamFrame::setDataMapperColorSegm(int idx)
{
	if(idx==0)
	{
		if(_camVtk == NULL){return;};
		_camVtk->setDataMapperColorSegm();
	}
	if(idx==1)
	{
		if(_camBGVtk == NULL){return;};
		_camBGVtk->setDataMapperColorSegm();
	}
	if(idx==2)
	{
		if(_camFGVtk == NULL){return;};
		_camFGVtk->setDataMapperColorSegm();
	}
	/*if(idx==3)
	{
		if(_camSegmVtk == NULL){return;};
		_camSegmVtk->setDataMapperColorSegm();
	}*/
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
void CamFrame::setDataMapperColorR(int idx)
{
	if(idx==0)
	{
		if(_camVtk == NULL){return;};
		_camVtk->setDataMapperColorR();
	}
	if(idx==1)
	{
		if(_camBGVtk == NULL){return;};
		_camBGVtk->setDataMapperColorR();
	}
	if(idx==2)
	{
		if(_camFGVtk == NULL){return;};
		_camFGVtk->setDataMapperColorR();
	}
	/*if(idx==3)
	{
		if(_camSegmVtk == NULL){return;};
		_camSegmVtk->setDataMapperColorR();
	}*/
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
void CamFrame::setDataMapperColorG(int idx)
{
	if(idx==0)
	{
		if(_camVtk == NULL){return;};
		_camVtk->setDataMapperColorG();
	}
	if(idx==1)
	{
		if(_camBGVtk == NULL){return;};
		_camBGVtk->setDataMapperColorG();
	}
	if(idx==2)
	{
		if(_camFGVtk == NULL){return;};
		_camFGVtk->setDataMapperColorG();
	}
	/*if(idx==3)
	{
		if(_camSegmVtk == NULL){return;};
		_camSegmVtk->setDataMapperColorG();
	}*/
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
void CamFrame::setDataMapperColorB(int idx)
{
	if(idx==0)
	{
		if(_camVtk == NULL){return;};
		_camVtk->setDataMapperColorB();
	}
	if(idx==1)
	{
		if(_camBGVtk == NULL){return;};
		_camBGVtk->setDataMapperColorB();
	}
	if(idx==2)
	{
		if(_camFGVtk == NULL){return;};
		_camFGVtk->setDataMapperColorB();
	}
	/*if(idx==3)
	{
		if(_camSegmVtk == NULL){return;};
		_camSegmVtk->setDataMapperColorB();
	}*/
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
void CamFrame::setDataMapperColorW(int idx)
{
	if(idx==0)
	{
		if(_camVtk == NULL){return;};
		_camVtk->setDataMapperColorW();
	}
	if(idx==1)
	{
		if(_camBGVtk == NULL){return;};
		_camBGVtk->setDataMapperColorW();
	}
	if(idx==2)
	{
		if(_camFGVtk == NULL){return;};
		_camFGVtk->setDataMapperColorW();
	}
	/*if(idx==3)
	{
		if(_camSegmVtk == NULL){return;};
		_camSegmVtk->setDataMapperColorW();
	}*/
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
void CamFrame::setDataMapperColorK(int idx)
{
	if(idx==0)
	{
		if(_camVtk == NULL){return;};
		_camVtk->setDataMapperColorK();
	}
	if(idx==1)
	{
		if(_camBGVtk == NULL){return;};
		_camBGVtk->setDataMapperColorK();
	}
	if(idx==2)
	{
		if(_camFGVtk == NULL){return;};
		_camFGVtk->setDataMapperColorK();
	}
	/*if(idx==3)
	{
		if(_camSegmVtk == NULL){return;};
		_camSegmVtk->setDataMapperColorK();
	}*/
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
void CamFrame::setDataMapperRepPts(int idx)
{
	if(idx==0)
	{
		if(_camVtk == NULL){return;};
		_camVtk->setDataRepPoints();
	}
	if(idx==1)
	{
		if(_camBGVtk == NULL){return;};
		_camBGVtk->setDataRepPoints();
	}
	if(idx==2)
	{
		if(_camFGVtk == NULL){return;};
		_camFGVtk->setDataRepPoints();
	}
	/*if(idx==3)
	{
		if(_camSegmVtk == NULL){return;};
		_camSegmVtk->setDataRepPoints();
	}*/
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
void CamFrame::setDataMapperRepSurf(int idx)
{
	if(idx==0)
	{
		if(_camVtk == NULL){return;};
		_camVtk->setDataRepSurface();
	}
	if(idx==1)
	{
		if(_camBGVtk == NULL){return;};
		_camBGVtk->setDataRepSurface();
	}
	if(idx==2)
	{
		if(_camFGVtk == NULL){return;};
		_camFGVtk->setDataRepSurface();
	}
	/*if(idx==3)
	{
		if(_camSegmVtk == NULL){return;};
		_camSegmVtk->setDataRepSurface();
	}*/
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
void CamFrame::setDataMapperOpacity(int idx, double alpha)
{
	if(idx==0)
	{
		if(_camVtk == NULL){return;};
		_camVtk->setDataOpacity(alpha);
	}
	if(idx==1)
	{
		if(_camBGVtk == NULL){return;};
		_camBGVtk->setDataOpacity(alpha);
	}
	if(idx==2)
	{
		if(_camFGVtk == NULL){return;};
		_camFGVtk->setDataOpacity(alpha);
	}
	/*if(idx==3)
	{
		if(_camSegmVtk == NULL){return;};
		_camSegmVtk->setDataOpacity(alpha);
	}*/
}
#endif // JMU_USE_VTK
int CamFrame::WriteCamTrfMat3(wxString fn, double mat3[9], wxString comments)
{
	double mat4[16]; for(int k=0; k<16; k++){mat4[k]=0;}; mat4[15]=1;
	for(int k=0, c=0, r=0; k<9; k++)
	{
		c = k /3; r = k % 3;
		mat4[r+c*4] = mat3[k];
	}
	return this->WriteCamTrfMat4(fn, mat4, comments);
}
int CamFrame::WriteCamTrfMat4(wxString fn, double mat4[16], wxString comments)
{
	int res = 0;

	try
	{
		// http://www.grinninglizard.com/tinyxmldocs/tutorial0.html
		ticpp::Document doc( std::string(fn.mb_str()) );
		ticpp::Declaration decl("1.0","utf-8","");
		doc.InsertEndChild( decl );
		ticpp::Comment comment( std::string(comments.mb_str()) );
		doc.InsertEndChild(comment);
		ticpp::Element root("TrfMat");
		char rowStr[64]; char attrStr[64]; char valStr[64];
		for(int row = 0; row <4; row++)
		{
			sprintf(rowStr, "Row%i", row);
			ticpp::Element pRow(rowStr);
			for(int col = 0; col <4 ; col++)
			{
				sprintf(attrStr, "val%i", col);
				sprintf(valStr, "%+0.6f", mat4[row+col*4]);
				pRow.SetAttribute(attrStr, valStr);
			}
			root.InsertEndChild( pRow );
		}
		doc.InsertEndChild( root );
		doc.SaveFile();

	}
	catch( ticpp::Exception& ex )
	{
		std::cout << ex.what();
		return -1;
	}
	catch(...)
	{
		//camTranMat->Identity(); // if a problem occured, set TrfMat to Identity
		return -1;
	}
	
	return res;
}

#ifdef JMU_ALIGNGUI
void CamFrame::AddAlignPt(int row, int col)
{
	if( (row > m_nRows) || (col > m_nCols)){ 
		return;
	}
	if( (_aliPCnt<0) || (_aliPCnt>=6) ){ _aliPCnt=0;}; // only 6 points desired, loop if more
	_aliPtsR[_aliPCnt] = row;
	_aliPtsC[_aliPCnt] = col;

	unsigned short *zBGbuf = NULL; short *yBGbuf = NULL; short *xBGbuf = NULL;
	zBGbuf = PLCTR_GetZ(m_CTrfBG);
	yBGbuf = PLCTR_GetY(m_CTrfBG);
	xBGbuf = PLCTR_GetX(m_CTrfBG);
	short x, y; unsigned short z; x=0; y=0; z=0;
	if((zBGbuf!=NULL) && (yBGbuf!=NULL) && (xBGbuf!=NULL) )
	{
	  int linPix = row*m_nCols+col; //col*(m_nRows)+row; //<- col major seems bad :-/
	  z = zBGbuf[linPix];
	  y = yBGbuf[linPix];
	  x = xBGbuf[linPix];
	}
	_aliPtsZ[_aliPCnt] = z;
	_aliPtsY[_aliPCnt] = y;
	_aliPtsX[_aliPCnt] = x;

	wxString resStr;
	resStr.Printf(wxT("Alignment point %02i added: ( %03i, %03i) -> (%+04i, %+04i, %+04i)"),
		          _aliPCnt, row, col, x, y, z);
	//m_settingsPane->SetText(resStr);
	_aliPCnt++;  // increment counter of valid points
	this->SetStatusText(resStr);
}
#endif
#ifdef JMU_ALIGNGUI
void CamFrame::GetAlignPoint(int idx, int &row, int &col, short &x, short &y, unsigned short &z)
{
	if( (idx<0) || (idx >= 6) || (idx>_aliPCnt) ){ return;};
	row = _aliPtsR[idx];
	col = _aliPtsC[idx];
	z = _aliPtsZ[idx];
	y = _aliPtsY[idx];
	x = _aliPtsX[idx];
}
#endif
#ifdef JMU_ALIGNGUI
RSCPLAN CamFrame::GetRansacPlane(int idx)
{
	if( (idx <0) || (idx >= 3) ){ idx = 0;}; // DANGEROUS IF THE PLANES ARE NOT SET BEFROEHAND :-/
	return plaAli[idx];
}
#endif
