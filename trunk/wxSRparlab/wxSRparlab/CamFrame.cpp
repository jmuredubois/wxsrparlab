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
		m_cFrm->AcqOneFrm();
        wxThread::Sleep(100);
    }

    return NULL;
}

/**
 * Camera frame class constructor \n
 * Each instance must have a parent frame (usually CMainWnd) \n
 */
CamFrame::CamFrame(wxFrame* parentFrm, const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(parentFrm, -1, title, pos, size)
{
	//! - a status bar is created \n
    CreateStatusBar();
    SetStatusText( _T("cam") );

	m_camNB = NULL;
	m_sr = NULL;
	m_pSrBuf = NULL; m_nRows = 0; m_nCols = 0; m_nSrBufSz = 0;

	m_pFile4ReadPha = NULL;//new wxFFile();
	m_pFile4ReadAmp = NULL;//new wxFFile();
	m_nFrmRead = 0; // 0 frames read when creating
	m_pThreadReadData = NULL;
	m_camReadMode = CAM_RD_ONESHOT;
	m_bReadContinuously = false; 
	m_srFrq = MF_20MHz;
}

/**
 * Camera frame class destructor \n
 */
CamFrame::~CamFrame()
{
	int res = -1;
	if(m_bReadContinuously){ if(m_pThreadReadData != NULL){ m_pThreadReadData->Delete(); } } ;
	if(m_sr		  != NULL) { res = SR_Close(m_sr);	m_sr = NULL; };
	if(m_pSrBuf   != NULL) { free((void*) m_pSrBuf  ); m_pSrBuf   = NULL; };
	if(m_pFile4ReadPha != NULL) { delete(m_pFile4ReadPha); m_pFile4ReadPha = NULL; };
	if(m_pFile4ReadAmp != NULL) { delete(m_pFile4ReadAmp); m_pFile4ReadAmp = NULL; };
}

BEGIN_EVENT_TABLE(CamFrame, wxFrame)
    EVT_MENU(IDM_Close, CamFrame::OnClose)
	EVT_BUTTON(IDB_OpenDev,  CamFrame::OnOpenDev)
	EVT_BUTTON(IDB_CloseDev,  CamFrame::OnCloseDev)
	EVT_BUTTON(IDB_Acquire,  CamFrame::Acquire)
	EVT_RADIOBOX(IDR_Freq, CamFrame::SetFreq)
	EVT_RADIOBOX(IDR_ReadMode, CamFrame::SetReadMode)
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
	m_camNB = new wxNotebook(this, -1, wxPoint(-1,-1), wxSize(-1,-1), wxNB_TOP, title);

	// %%%%%%% Settings window
	m_settingsPane = new CamPanelSettings(m_camNB,wxT("Settings"), wxPoint(-1,-1), wxSize(-1,-1)); /* NBparadigm */
	m_settingsPane->InitSettings();

	// %%%%%% Camview window
	m_viewRangePane = new CamViewData(m_camNB,wxT("Range"), wxPoint(-1,-1), wxSize(-1,-1));  /* NBparadigm */
	m_viewRangePane->InitViewData();

	// %%%%%% Camview window
	m_viewAmpPane = new CamViewData(m_camNB,wxT("Amplitude"), wxPoint(-1,-1), wxSize(-1,-1));  /* NBparadigm */
	m_viewAmpPane->InitViewData();
	m_viewAmpPane->SetDispMin(0.0);
	m_viewAmpPane->SetDispMax(1000.0);

	/* NBparadigm */
	m_camNB->AddPage(m_settingsPane, wxT("Settings"), TRUE, -1);
	m_camNB->AddPage(m_viewRangePane, wxT("Range"), FALSE, -1);
	m_camNB->AddPage(m_viewAmpPane, wxT("Amplitude"), FALSE, -1);
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
  //logPrintf("swissrangerTester: SR_Open device");
  m_settingsPane->SetText(wxT("Open attempt"));
  m_settingsPane->DisableOpenSR();		// disable "Open" button
  m_settingsPane->EnableRadioFilt();	// ensable filter selection
  m_settingsPane->EnableRadioFrq();		// enable frequency selection
  m_settingsPane->EnableCloseSR();	// enable "Close" button
  wxString strR;
  int serial = 0;
  if(m_sr == NULL)
  {
	  res = -1;
		res = SR_OpenUSB(& m_sr);
		//Returns the serial number (if existing) of the camera
  }
  strR.sprintf(wxT("cam open %i"), res); // ... change title text ...
  if(m_sr != NULL)
  {
	  serial =  SR_ReadSerial(m_sr); 
	  strR.sprintf(wxT("cam serial %i"), serial); // ... change title text ...
  }

  wxFileDialog* OpenDialogPar = new wxFileDialog(this, 
	  wxT("Choose a SR parameters file to open"),	// msg
	  wxT("D:\\Users\\murej\\Documents\\PersPassRecords"),	// default dir
	  wxEmptyString,	// default file
	  wxT("SR parameters files (*.sr2)|*.sr2|All files (*.*)|*.*"),	// file ext
	  wxFD_OPEN|wxFD_CHANGE_DIR,
	  wxDefaultPosition);
  wxFileDialog* OpenDialogPha = new wxFileDialog(this, 
	  wxT("Choose a SR PHASE file to open"),	// msg
	  wxT("D:\\Users\\murej\\Documents\\PersPassRecords"),	// default dir
	  wxEmptyString,	// default file
	  wxT("SR phase files (*.16b)|*.16b|All files (*.*)|*.*"),	// file ext
	  wxFD_OPEN|wxFD_CHANGE_DIR,
	  wxDefaultPosition);
  wxFileDialog* OpenDialogAmp = new wxFileDialog(this, 
	  wxT("Choose a SR AMPLITUDE file to open"),	// msg
	  wxT("D:\\Users\\murej\\Documents\\PersPassRecords"),	// default dir
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
	  wxFFile* wxFparams = new wxFFile(strPathPar, "rb");
	  if ( (wxFparams->IsOpened()) )
	  {
	    int tmp = 0; // DANGER = 1 int must be 4 bytes
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
		  m_nRows = tmp;
		  readBytes= wxFparams->Read(&tmp, sizeof(int));
		  m_nCols = tmp;
		  readBytes= wxFparams->Read(&tmp, sizeof(int));
		  int numImg = tmp;
		  readBytes = wxFparams->Read(&tmp, sizeof(int));
		  int bytesPerSample = tmp;
		  readBytes= wxFparams->Read(&tmp, sizeof(int));
		  m_nSrBufSz = tmp;
		} // (sizeof(int)!=4)
		delete(wxFparams);
	    m_pFile4ReadPha = new wxFFile(strPathPha, "rb");
	    m_pFile4ReadAmp = new wxFFile(strPathAmp, "rb");
	    if ( (!(m_pFile4ReadAmp)) || (!(m_pFile4ReadAmp)) || (!(m_pFile4ReadAmp->IsOpened())) || (!(m_pFile4ReadAmp->IsOpened())))
	    {
		  res -=4;
	    }
 
	    strR.sprintf(wxT("cam serial %i - %ix%i  - %i"), serial, m_nRows, m_nCols, m_nSrBufSz); // ... change text ...
	    m_pSrBuf = (unsigned char*) malloc(m_nSrBufSz);
	    memset(m_pSrBuf, 0x77, m_nSrBufSz);
	  //
	    res = m_viewRangePane->SetDataArray<unsigned short>((unsigned short*) &m_pSrBuf[0], m_nRows*m_nCols);
	    res = m_viewAmpPane->SetDataArray<unsigned short>((unsigned short*) &m_pSrBuf[m_nCols*m_nRows*2], m_nRows*m_nCols);
 
	    m_settingsPane->SetText(strR);
	  } // (wxFparams->IsOpened())
  } // (OpenDialogPar->ShowModal()==wxID_OK) && (OpenDialogPha->ShowModal()==wxID_OK) && (OpenDialogAmp->ShowModal()==wxID_OK)
  delete(OpenDialogPar);
  delete(OpenDialogPha);
  delete(OpenDialogAmp);
  if((m_pFile4ReadPha  != NULL) && (m_pFile4ReadAmp  != NULL) && (m_pFile4ReadPha->IsOpened()) && (m_pFile4ReadAmp->IsOpened()))
  {
	  return;
  }
  

  if(m_sr != NULL)
  {
	  m_nSrBufSz = (int) SR_GetBufferSize(m_sr); 
	  m_nCols = (int) SR_GetCols(m_sr); 
	  m_nRows = (int) SR_GetRows(m_sr); 
	  strR.sprintf(wxT("cam serial %i - %ix%i  - %i"), serial, m_nRows, m_nCols, m_nSrBufSz); // ... change text ...
	  m_pSrBuf = (unsigned char*) malloc(m_nSrBufSz);
	  memset(m_pSrBuf, 0x77, m_nSrBufSz);
	  res = SR_SetBuffer(m_sr, (void*) m_pSrBuf , m_nSrBufSz);
	  res = SR_Acquire(m_sr, AM_COR_FIX_PTRN );//|| AM_COR_LED_NON_LIN );
	  //
	  res = m_viewRangePane->SetDataArray<unsigned short>((unsigned short*) SR_GetImage(m_sr, 0), m_nRows*m_nCols);
	  res = m_viewAmpPane->SetDataArray<unsigned short>((unsigned short*) SR_GetImage(m_sr, 1), m_nRows*m_nCols);
  }
  m_settingsPane->SetText(strR);
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
  if(m_sr != NULL)
  {
		res = SR_Close(m_sr);
		m_sr = NULL; 
  }
  if(m_pFile4ReadPha != NULL) { delete(m_pFile4ReadPha); m_pFile4ReadPha = NULL; };
  if(m_pFile4ReadAmp != NULL) { delete(m_pFile4ReadAmp); m_pFile4ReadAmp = NULL; };
  if(m_pSrBuf   != NULL) { free((void*) m_pSrBuf  ); m_pSrBuf   = NULL; };
  m_nSrBufSz = 0; 
  m_nCols = 0; 
  m_nRows = 0;
  m_nFrmRead = 0;
  m_settingsPane->EnableOpenSR();	// enable "Open" button
  m_settingsPane->SetText(wxT("Close successfull"));
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
		m_viewRangePane->SetBtnTxtStop();		// set new texts in ctrl buttons
		m_viewAmpPane->SetBtnTxtStop();			//
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
		m_viewRangePane->SetBtnTxtAcqu();
		m_viewAmpPane->SetBtnTxtAcqu();
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
  if((m_sr == NULL) && (m_pSrBuf != NULL) && (m_pFile4ReadPha  != NULL) && (m_pFile4ReadAmp  != NULL) && (m_pFile4ReadPha->IsOpened()) && (m_pFile4ReadAmp->IsOpened()))
  {
	  res = m_pFile4ReadPha->Read(m_pSrBuf, m_nCols*m_nRows*2);
	  res = m_pFile4ReadAmp->Read(&m_pSrBuf[m_nCols*m_nRows*2], m_nCols*m_nRows*2);
	  m_nFrmRead +=1;
	  // ... change text ...
	  strR.sprintf(wxT("frm:%05u - pixFileRead %i - %ix%i  - %i"), m_nFrmRead, res, m_nRows, m_nCols, m_nSrBufSz);
	  m_viewRangePane->SetDataArray<unsigned short>((unsigned short*) &m_pSrBuf[0], m_nRows*m_nCols);
	  m_viewAmpPane->SetDataArray<unsigned short>((unsigned short*) &m_pSrBuf[m_nCols*m_nRows*2], m_nRows*m_nCols);
	  m_settingsPane->SetText(strR);
	  m_viewRangePane->SetTxtInfo(strR);
	  m_viewAmpPane->SetTxtInfo(strR);

	  if( (m_pFile4ReadPha->Eof()) || (m_pFile4ReadAmp->Eof()) )
	  {
		  m_pFile4ReadPha->Seek(0); // rewind phase
		  m_pFile4ReadAmp->Seek(0); // rewind phase
		  m_nFrmRead = 0;	// reset frame count
	  }
  }
  if((m_sr != NULL) && (m_pSrBuf != NULL) )
  {
	  // ... change text ...
	  res = SR_Acquire(m_sr, AM_COR_FIX_PTRN || AM_COR_LED_NON_LIN );
	  strR.sprintf(wxT("frm:%05u - pixRead %i - %ix%i  - %i"), m_nFrmRead, res, m_nRows, m_nCols, m_nSrBufSz);
	  m_viewRangePane->SetDataArray<unsigned short>((unsigned short*) SR_GetImage(m_sr, 0), m_nRows*m_nCols);
	  m_viewAmpPane->SetDataArray<unsigned short>((unsigned short*) SR_GetImage(m_sr, 1), m_nRows*m_nCols);
	  m_settingsPane->SetText(strR);
	  m_viewRangePane->SetTxtInfo(strR);
	  m_viewAmpPane->SetTxtInfo(strR);
	  m_nFrmRead +=1;
  }
  m_viewRangePane->SetNewImage();
  m_viewAmpPane->SetNewImage();
};

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
