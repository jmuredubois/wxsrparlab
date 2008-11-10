/*
 * CamFrame.cpp
 * Implementation of the camera frame for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.11.08
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

    for ( ; ; )
    {
        // check if we were asked to exit
        if ( TestDestroy() )
            break;
		//m_cFrm->AcqOneFrm();
		wxCommandEvent event( wxEVT_JMUACQONEFRM, IDC_AcqOne );
		m_cFrm->AddPendingEvent(event);
		//m_cFrm->ProcessEvent(event);
        wxThread::Sleep(100);
        #ifdef __WXMAC__
            wxThread::Sleep(100); // macBook graphics card is slow
        #endif
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
	m_mutexSrBuf = new wxMutex(wxMUTEX_DEFAULT);

	m_pFile4ReadPha = NULL;//new wxFFile();
	m_pFile4ReadAmp = NULL;//new wxFFile();
	m_nFrmRead = 0; // 0 frames read when creating
	m_nSerialSR = 0;
	m_pThreadReadData = NULL;
	m_camReadMode = CAM_RD_ONESHOT;
	m_bReadContinuously = false;
	m_srFrq = MF_20MHz;
	m_maxMM[0]= 5000.0f; m_maxMM[1]= 7142.8571f;m_maxMM[2]= 7500.0f;m_maxMM[3]= 7894.7368f;
	//! @todo:  HARDCODED values for m_maxMM -> check with libusbSR driver settings
	//m_fFocal = 8.0f; m_fPixSzX = 0.04f; m_fPixSzY = 0.04f; m_fCenterX = 88.0f; m_fCenterY = 72.0f;
	//m_maxMMr = 7500.0f;
	m_ctrParam.f = 8.0f; m_ctrParam.pixDX = 0.04f; m_ctrParam.pixDY = 0.04f; m_ctrParam.cX = 88.0f; m_ctrParam.cY = 72.0f;
	m_ctrParam.maxMM = 7500.0f;
	m_mfrqInt = 2;
#ifdef JMU_USE_VTK
	_vtkWin=NULL;
	_camVtk = NULL;
	_camBGVtk = NULL;
#endif
	#ifdef JMU_TGTFOLLOW
		m_pFile4TgtCoord = NULL;
	#endif
	_vtkSub = 0;
	m_scat = NULL;
	m_NaN = NULL;
	m_bgAvg = NULL;
	m_CTrf = NULL;
	m_CTrfBG = NULL;
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
	if(_camBGVtk != NULL) { delete(_camBGVtk); _camVtk = NULL; } ;
#endif
	if(m_sr		  != NULL) { res = SR_Close(m_sr);	m_sr = NULL; };
	if(m_mutexSrBuf != NULL) { delete(m_mutexSrBuf); m_mutexSrBuf = NULL;};
	if(m_pSrBuf   != NULL) { free((void*) m_pSrBuf  ); m_pSrBuf   = NULL; };
	if(m_pFile4ReadPha != NULL) { delete(m_pFile4ReadPha); m_pFile4ReadPha = NULL; };
	if(m_pFile4ReadAmp != NULL) { delete(m_pFile4ReadAmp); m_pFile4ReadAmp = NULL; };
	#ifdef JMU_TGTFOLLOW
		if(m_pFile4TgtCoord != NULL) { delete(m_pFile4TgtCoord); m_pFile4TgtCoord = NULL; };
	#endif
	if(m_NaN != NULL) { PLNN_Close(m_NaN); m_NaN = NULL; };
	if(m_scat != NULL) {PLSC_Close(m_scat); m_scat = NULL; };
	if(m_bgAvg != NULL) {PLAVG_Close(m_bgAvg); m_bgAvg = NULL; };
	if(m_CTrf != NULL) {PLCTR_Close(m_CTrf); m_CTrf = NULL; };
	if(m_CTrfBG != NULL) {PLCTR_Close(m_CTrfBG); m_CTrfBG = NULL; };
}

BEGIN_EVENT_TABLE(CamFrame, wxFrame)
    EVT_MENU(IDM_Close, CamFrame::OnClose)
	EVT_BUTTON(IDB_OpenDev,  CamFrame::OnOpenDev)
	EVT_BUTTON(IDB_CloseDev,  CamFrame::OnCloseDev)
	EVT_BUTTON(IDB_Acquire,  CamFrame::Acquire)
	EVT_RADIOBOX(IDR_Freq, CamFrame::SetFreq)
	EVT_RADIOBOX(IDR_ReadMode, CamFrame::SetReadMode)
	EVT_BUTTON(IDB_SetTrfMat,  CamFrame::OnSetTrfMat)
	#ifdef JMU_TGTFOLLOW
		EVT_BUTTON(IDB_TgtFile,  CamFrame::OnTgtFile)
	#endif
	EVT_COMMAND(IDC_AcqOne, wxEVT_JMUACQONEFRM, CamFrame::AcqOneFrmEvt)
	EVT_BUTTON(IDB_SetScatParams, CamFrame::OnSetScatParams)
	EVT_BUTTON(IDB_ClearBg, CamFrame::OnClearBg)
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
int CamFrame::CreateAndSetNotebook(const wxString& title)
{
	int res  = 0;
	if(m_camNB != NULL)
	{
		m_camNB->Close(TRUE);
	}
	m_camNB = new wxAuiNotebook(this, -1, wxPoint(-1,-1), wxSize(-1,-1), wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS );

	// %%%%%%% Settings window
	m_settingsPane = new CamPanelSettings(m_camNB,wxT("Settings"), wxPoint(-1,-1), wxSize(-1,-1)); /* NBparadigm */
	m_settingsPane->InitSettings();

	// %%%%%% RANGE panel
	m_viewRangePane = new CamViewData(m_camNB,wxT("Range"), wxPoint(-1,-1), wxSize(-1,-1));  /* NBparadigm */
	m_viewRangePane->InitViewData();

	// %%%%%% AMPLITUDE panel
	m_viewAmpPane = new CamViewData(m_camNB,wxT("Amplitude"), wxPoint(-1,-1), wxSize(-1,-1));  /* NBparadigm */
	m_viewAmpPane->InitViewData();
	m_viewAmpPane->SetDispMin(0.0);
	m_viewAmpPane->SetDispMax(1000.0);

	// %%%%%% Z panel
	m_viewZPane = new CamViewData(m_camNB,wxT("Z [mm]"), wxPoint(-1,-1), wxSize(-1,-1));  /* NBparadigm */
	m_viewZPane->InitViewData();
	m_viewZPane->SetDispMin(3500.0);
	m_viewZPane->SetDispMax(000.0);
#ifdef DISPXYBUFFERS
	// %%%%%% Y panel
	m_viewYPane = new CamViewData(m_camNB,wxT("Y [mm]"), wxPoint(-1,-1), wxSize(-1,-1));  /* NBparadigm */
	m_viewYPane->InitViewData();
	m_viewYPane->SetDispMin(-1500.0);
	m_viewYPane->SetDispMax(1500.0);
	// %%%%%% x panel
	m_viewXPane = new CamViewData(m_camNB,wxT("X [mm]"), wxPoint(-1,-1), wxSize(-1,-1));  /* NBparadigm */
	m_viewXPane->InitViewData();
	m_viewXPane->SetDispMin(-1500.0);
	m_viewXPane->SetDispMax(1500.0);
#endif

	// %%%%% BACKGROUND RANGE panel
	m_viewBGRangePane = new CamViewData(m_camNB, wxT("BG pha"), wxPoint(-1,-1), wxSize(-1,-1));  /* NBparadigm */
	m_viewBGRangePane->InitViewData();
	m_viewBGRangePane->SetDispMin(45000.0);
	m_viewBGRangePane->SetDispMax(0.0); 
	// %%%%% BACKGROUND AMPLITUDE panel
	m_viewBGAmpPane = new CamViewData(m_camNB, wxT("BG amp"), wxPoint(-1,-1), wxSize(-1,-1));  /* NBparadigm */
	m_viewBGAmpPane->InitViewData();
	m_viewBGAmpPane->SetDispMin(0.0);
	m_viewBGAmpPane->SetDispMax(1000.0);

	/* NBparadigm */
	m_camNB->AddPage(m_settingsPane, wxT("Settings"), TRUE);//, -1);
	m_camNB->AddPage(m_viewRangePane, wxT("Range"), FALSE);//, -1);
	m_camNB->AddPage(m_viewAmpPane, wxT("Amplitude"), FALSE);//, -1);
	m_camNB->AddPage(m_viewZPane, wxT("Z [mm]"), FALSE);//, -1);
#ifdef DISPXYBUFFERS
	m_camNB->AddPage(m_viewYPane, wxT("Y [mm]"), FALSE);//, -1);
	m_camNB->AddPage(m_viewXPane, wxT("X [mm]"), FALSE);//, -1);
#endif //DISPXYBUFFERS
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
  unsigned int res = 0;
  //logPrintf("swissrangerTester: SR_Open device");
  m_settingsPane->SetText(wxT("Open attempt"));
  m_settingsPane->DisableOpenSR();		// disable "Open" button
  m_settingsPane->EnableRadioFilt();	// ensable filter selection
  m_settingsPane->EnableRadioFrq();		// enable frequency selection
  m_settingsPane->EnableCloseSR();	// enable "Close" button
  m_settingsPane->EnableScatParams(); // enable "Scat comp params" button
  wxString strR;
  unsigned int serial = 0;
  if(m_sr == NULL)
  {
	  res = -1;
		res = SR_OpenUSB(& m_sr, 0); // opens first found camera
		//Returns the serial number (if existing) of the camera
  }
  strR.sprintf(wxT("cam open %u"), res); // ... change title text ...
  if(m_sr != NULL)
  {
	  serial =  SR_ReadSerial(m_sr);
	  strR.sprintf(wxT("cam serial %u"), serial); // ... change title text ...
	  m_nSerialSR = serial;
  }

  wxFileDialog* OpenDialogPar = new wxFileDialog(this,
	  wxT("Choose a SR parameters file to open"),	// msg
	  wxT(""), //wxT("D:\\Users\\murej\\Documents\\PersPassRecords"),	// default dir
	  wxEmptyString,	// default file
	  wxT("SR parameters files (*.sr2)|*.sr2|All files (*.*)|*.*"),	// file ext
	  wxFD_OPEN|wxFD_CHANGE_DIR,
	  wxDefaultPosition);
  wxFileDialog* OpenDialogPha = new wxFileDialog(this,
	  wxT("Choose a SR PHASE file to open"),	// msg
	  wxT(""), //wxT("D:\\Users\\murej\\Documents\\PersPassRecords"),	// default dir
	  wxEmptyString,	// default file
	  wxT("SR phase files (*.16b)|*.16b|All files (*.*)|*.*"),	// file ext
	  wxFD_OPEN|wxFD_CHANGE_DIR,
	  wxDefaultPosition);
  wxFileDialog* OpenDialogAmp = new wxFileDialog(this,
	  wxT("Choose a SR AMPLITUDE file to open"),	// msg
	  wxT(""), //wxT("D:\\Users\\murej\\Documents\\PersPassRecords"),	// default dir
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
	  PLCTR_Open(&m_CTrf, newbuf);
	  PLCTR_Open(&m_CTrfBG, newbuf);
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
  PLCTR_Open(&m_CTrf, newbuf);
  PLCTR_Open(&m_CTrfBG, newbuf);
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
  if(m_sr != NULL)
  {
		res = SR_Close(m_sr);
		m_sr = NULL;
  }
  if(m_pFile4ReadPha != NULL) { delete(m_pFile4ReadPha); m_pFile4ReadPha = NULL; };
  if(m_pFile4ReadAmp != NULL) { delete(m_pFile4ReadAmp); m_pFile4ReadAmp = NULL; };
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
  if(m_scat != NULL)  {PLSC_Close(m_scat);   m_scat = NULL; };
  if(m_bgAvg != NULL) {PLAVG_Close(m_bgAvg); m_bgAvg = NULL; };
  if(m_CTrf != NULL) {PLCTR_Close(m_CTrf); m_CTrf = NULL; };
  if(m_CTrfBG != NULL) {PLCTR_Close(m_CTrfBG); m_CTrfBG = NULL; };
  m_settingsPane->EnableOpenSR();	// enable "Open" button
  m_settingsPane->SetText(wxT("Close successfull"));
  SetStatusText( wxT("cam") );
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
  if((m_sr == NULL) && (m_pSrBuf != NULL) && (m_CTrf!=NULL)&&(m_pFile4ReadPha  != NULL) && (m_pFile4ReadAmp  != NULL) && (m_pFile4ReadPha->IsOpened()) && (m_pFile4ReadAmp->IsOpened()))
  {
	  // if reading from file ...
	  wxMutexError errMutex= m_mutexSrBuf->Lock();
	  if(errMutex == wxMUTEX_NO_ERROR)
	  {
		res = (int) m_pFile4ReadPha->Read(m_pSrBuf, (int)m_nCols*(int)m_nRows*sizeof(unsigned short));
		res = (int) m_pFile4ReadAmp->Read(&m_pSrBuf[(int)m_nCols*(int)m_nRows*sizeof(unsigned short)], (int)m_nCols*(int)m_nRows*sizeof(unsigned short));
		m_nFrmRead +=1;
		    SRBUF srBuf;
		    srBuf.pha = (unsigned short*) m_pSrBuf;
		    srBuf.amp = (unsigned short*) (&m_pSrBuf[m_nRows*m_nCols*sizeof(unsigned short)]);
		    srBuf.nCols = m_nCols;
		    srBuf.nRows = m_nRows;
		    srBuf.bufferSizeInBytes = m_nCols*m_nRows*2*sizeof(unsigned short);
			NANBUF scatNaN;
			scatNaN.nCols = m_nCols;
			scatNaN.nRows = m_nRows;
			scatNaN.bufferSizeInBytes = m_nCols * m_nRows * sizeof(bool);
			scatNaN.nanBool = PLNN_FlagNaN(m_NaN, srBuf);
		if(m_settingsPane->IsScatChecked())
		{
			PLSC_Compensate(m_scat, srBuf, scatNaN);
		}
		if(m_settingsPane->IsLrnBgChecked())
		{
			PLAVG_LearnBackground(m_bgAvg, srBuf);
		}
		PLCTR_CoordTrf(m_CTrf, srBuf, m_ctrParam);
		errMutex = m_mutexSrBuf->Unlock();
	  } //{errMutex == wxMUTEX_NO_ERROR)
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
	  #endif

	  strR.sprintf(wxT("frm:%05u - pixFileRead %i - %ix%i  - %i"), m_nFrmRead, res, m_nRows, m_nCols, m_nSrBufSz);
	  m_viewRangePane->SetDataArray<unsigned short>((unsigned short*) &m_pSrBuf[0], m_nRows*m_nCols);
	  m_viewAmpPane->SetDataArray<unsigned short>((unsigned short*) &m_pSrBuf[m_nCols*m_nRows*2], m_nRows*m_nCols);
	  m_viewZPane->SetDataArray<unsigned short>(PLCTR_GetZ(m_CTrf), m_nRows*m_nCols);
	  #ifdef DISPXYBUFFERS
	    m_viewYPane->SetDataArray<short>(PLCTR_GetY(m_CTrf), m_nRows*m_nCols);
	    m_viewXPane->SetDataArray<short>(PLCTR_GetX(m_CTrf), m_nRows*m_nCols);
	  #endif //DISPXYBUFFERS
	  m_viewBGRangePane->SetDataArray<unsigned short>((unsigned short*) (PLAVG_GetAvgBuf(m_bgAvg)).pha, m_nRows*m_nCols);
	  m_viewBGAmpPane->SetDataArray<unsigned short>((unsigned short*) (PLAVG_GetAvgBuf(m_bgAvg)).amp, m_nRows*m_nCols);
	  #ifdef JMU_USE_VTK
		//if(m_camReadMode==CAM_RD_ONESHOT) // vtk does not support access from different threads
			_camVtk->updateTOF(m_nRows, m_nCols, PLCTR_GetZ(m_CTrf), PLCTR_GetY(m_CTrf), PLCTR_GetX(m_CTrf), (unsigned short*) &m_pSrBuf[m_nCols*m_nRows*2]);
		if(m_settingsPane->IsLrnBgChecked())
		{
			PLCTR_CoordTrf(m_CTrfBG, PLAVG_GetAvgBuf(m_bgAvg), m_ctrParam);
			_camBGVtk->updateTOF(m_nRows, m_nCols, PLCTR_GetZ(m_CTrfBG), PLCTR_GetY(m_CTrfBG), PLCTR_GetX(m_CTrfBG), PLAVG_GetAvgBuf(m_bgAvg).amp);
		}
		_vtkWin->Render(); // avoid rendering twice for BG and FG ; PROBLEM rendering is done once for each camera, should be done only once :-(
      #endif
	  m_settingsPane->SetText(strR);
	  m_viewRangePane->SetTxtInfo(strR);
	  m_viewAmpPane->SetTxtInfo(strR);
	  m_viewZPane->SetTxtInfo(strR);
	  #ifdef DISPXYBUFFERS
	    m_viewYPane->SetTxtInfo(strR);
	    m_viewXPane->SetTxtInfo(strR);
	  #endif //DISPXYBUFFERS
	  wxString strBG; strBG.sprintf(wxT("Average count: %05i"), PLAVG_GetAvgCnt(m_bgAvg));
	  m_viewBGRangePane->SetTxtInfo(strBG);
	  m_viewBGAmpPane->SetTxtInfo(strBG);

	  if( (m_pFile4ReadPha->Eof()) || (m_pFile4ReadAmp->Eof()) )
	  {
		  m_pFile4ReadPha->Seek(0); // rewind phase
		  m_pFile4ReadAmp->Seek(0); // rewind phase
		  m_nFrmRead = 0;	// reset frame count
	  }
  }
  if((m_sr != NULL) && (m_pSrBuf != NULL) && (m_CTrf!=NULL) )
  {
	  // if reading data live from camera ...
	  wxMutexError errMutex= m_mutexSrBuf->Lock();
	  if(errMutex == wxMUTEX_NO_ERROR)
	  {
	    res = SR_SetMode(m_sr, AM_COR_FIX_PTRN ); //|| AM_COR_LED_NON_LIN );
		res = SR_Acquire(m_sr);
		memcpy( (void*) m_pSrBuf, SR_GetImage(m_sr,0), m_nRows*m_nCols*sizeof(unsigned short));
		memcpy( (void*) (&m_pSrBuf[m_nRows*m_nCols*sizeof(unsigned short)]), SR_GetImage(m_sr,1), m_nRows*m_nCols*sizeof(unsigned short));
			SRBUF srBuf;
		    srBuf.pha = (unsigned short*) m_pSrBuf;
		    srBuf.amp = (unsigned short*) (&m_pSrBuf[m_nRows*m_nCols*sizeof(unsigned short)]);
		    srBuf.nCols = m_nCols;
		    srBuf.nRows = m_nRows;
		    srBuf.bufferSizeInBytes = m_nCols*m_nRows*2*sizeof(unsigned short);
			NANBUF scatNaN;
			scatNaN.nCols = m_nCols;
			scatNaN.nRows = m_nRows;
			scatNaN.bufferSizeInBytes = m_nCols * m_nRows * sizeof(bool);
			scatNaN.nanBool = PLNN_FlagNaN(m_NaN, srBuf);
		if(m_settingsPane->IsScatChecked())
		{
			PLSC_Compensate(m_scat, srBuf, scatNaN);
		}
		if(m_settingsPane->IsLrnBgChecked())
		{
			PLAVG_LearnBackground(m_bgAvg, srBuf);
		}
		PLCTR_CoordTrf(m_CTrf, srBuf, m_ctrParam);
	    errMutex = m_mutexSrBuf->Unlock();
	  } //{errMutex == wxMUTEX_NO_ERROR)
	  strR.sprintf(wxT("frm:%05u - pixRead %i - %ix%i  - %i"), m_nFrmRead, res, m_nRows, m_nCols, m_nSrBufSz);
	  m_viewRangePane->SetDataArray<unsigned short>((unsigned short*) SR_GetImage(m_sr, 0), m_nRows*m_nCols);
	  m_viewAmpPane->SetDataArray<unsigned short>((unsigned short*) SR_GetImage(m_sr, 1), m_nRows*m_nCols);
	  m_viewZPane->SetDataArray<unsigned short>(PLCTR_GetZ(m_CTrf), m_nRows*m_nCols);
	  #ifdef DISPXYBUFFERS
	    m_viewYPane->SetDataArray<short>(PLCTR_GetY(m_CTrf), m_nRows*m_nCols);
	    m_viewXPane->SetDataArray<short>(PLCTR_GetX(m_CTrf), m_nRows*m_nCols);
	  #endif //DISPXYBUFFERS
	  m_viewBGRangePane->SetDataArray<unsigned short>((unsigned short*) (PLAVG_GetAvgBuf(m_bgAvg)).pha, m_nRows*m_nCols);
	  m_viewBGAmpPane->SetDataArray<unsigned short>((unsigned short*) (PLAVG_GetAvgBuf(m_bgAvg)).amp, m_nRows*m_nCols);
	  #ifdef JMU_USE_VTK
		 // vtk does not support access from different threads
			_camVtk->updateTOF(m_nRows, m_nCols, PLCTR_GetZ(m_CTrf), PLCTR_GetY(m_CTrf), PLCTR_GetX(m_CTrf), (unsigned short*) &m_pSrBuf[m_nCols*m_nRows*2]);
		if(m_settingsPane->IsLrnBgChecked())
		{
			PLCTR_CoordTrf(m_CTrfBG, PLAVG_GetAvgBuf(m_bgAvg), m_ctrParam);
			_camBGVtk->updateTOF(m_nRows, m_nCols, PLCTR_GetZ(m_CTrfBG), PLCTR_GetY(m_CTrfBG), PLCTR_GetX(m_CTrfBG), PLAVG_GetAvgBuf(m_bgAvg).amp);
		}
      #endif
	  m_settingsPane->SetText(strR);
	  m_viewRangePane->SetTxtInfo(strR);
	  m_viewAmpPane->SetTxtInfo(strR);
	  m_viewZPane->SetTxtInfo(strR);
	  #ifdef DISPXYBUFFERS
	    m_viewYPane->SetTxtInfo(strR);
	    m_viewXPane->SetTxtInfo(strR);
	  #endif //DISPXYBUFFERS
	  wxString strBG; strBG.sprintf(wxT("Average count: %05i"), PLAVG_GetAvgCnt(m_bgAvg));
	  m_viewBGRangePane->SetTxtInfo(strBG);
	  m_viewBGAmpPane->SetTxtInfo(strBG);
	  m_nFrmRead +=1;
  }
  //if((m_viewRangePane->IsShownOnScreen())){ m_viewRangePane->SetNewImage();};
  //if((m_viewAmpPane->IsShownOnScreen())){ m_viewAmpPane->SetNewImage();};
};


void CamFrame::AcqOneFrmEvt(wxCommandEvent& WXUNUSED(event))
{
	AcqOneFrm();
}

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
	  }
	  if( ( strF.Find(wxT("20")) != wxNOT_FOUND) )
	  {
		  m_srFrq = MF_20MHz;
	  }
	  if( ( strF.Find(wxT("21")) != wxNOT_FOUND) )
	  {
		  m_srFrq = MF_21MHz;
	  }
	  if( ( strF.Find(wxT("30")) != wxNOT_FOUND) )
	  {
		  m_srFrq = MF_30MHz;
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
  int res = 0;
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
		_camVtk->SetPlainLUT(_vtkWin->GetRLUT(), _vtkWin->GetGLUT(), _vtkWin->GetBLUT(), _vtkWin->GetWLUT(), _vtkWin->GetKLUT());
		_camBGVtk = new CamVtkView(_vtkSub, _vtkWin->GetRenWin(), _vtkWin->GetDepthLUT());
		_camBGVtk->SetDepthLUT(_vtkWin->GetDepthLUT());
		_camBGVtk->SetGrayLUT(_vtkWin->GetGrayLUT());
		_camBGVtk->SetPlainLUT(_vtkWin->GetRLUT(), _vtkWin->GetGLUT(), _vtkWin->GetBLUT(), _vtkWin->GetWLUT(), _vtkWin->GetKLUT());
		_camBGVtk->setDataRepBG();
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
	  wxT(""), //wxT("D:\\Users\\murej\\Documents\\PersPassRecords"),	// default dir
	  wxEmptyString,	// default file
	  wxT("Coordinates transformation matrix files (*.xml)|*.xml|All files (*.*)|*.*"),	// file ext
	  wxFD_OPEN|wxFD_CHANGE_DIR,
	  wxDefaultPosition);

  if((OpenDialogMat->ShowModal()==wxID_OK) )
  {
	  wxString strPathMat = OpenDialogMat->GetPath();
	  _camVtk->setTrfMat(strPathMat.char_str());
	  _camBGVtk->setTrfMat(strPathMat.char_str());
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
	  wxT(""), //wxT("D:\\Users\\murej\\Documents\\PersPassRecords"),	// default dir
	  wxEmptyString,	// default file
	  wxT("Target coordinates files (*.dat)|*.dat|All files (*.*)|*.*"),	// file ext
	  wxFD_OPEN|wxFD_CHANGE_DIR,
	  wxDefaultPosition);

  if((OpenDialogTgt->ShowModal()==wxID_OK) )
  {
	  wxString strPathTgt = OpenDialogTgt->GetPath();
	  m_pFile4TgtCoord = new wxFFile(strPathTgt, wxT("rb"));
	  //_camVtk->setTgtFile(strPathTgt.char_str());
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
  wxString strR;

  wxFileDialog* OpenDialogScat = new wxFileDialog(this,
	  wxT("Choose a scattering compensation parameters file to open"),	// msg
	  wxT(""), //wxT("D:\\Users\\murej\\Documents\\PersPassRecords"),	// default dir
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
  m_settingsPane->SetText(wxT("Cleared background"));
}
