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
}

/**
 * Camera frame class destructor \n
 */
CamFrame::~CamFrame()
{
	int res = -1;
	if(m_sr		  != NULL) { res = SR_Close(m_sr);	m_sr = NULL; };
	if(m_pSrBuf   != NULL) { free((void*) m_pSrBuf  ); m_pSrBuf   = NULL; };
}

BEGIN_EVENT_TABLE(CamFrame, wxFrame)
    EVT_MENU(IDM_Close, CamFrame::OnClose)
	EVT_BUTTON(IDB_OpenDev,  CamFrame::OnOpenDev)
	EVT_BUTTON(IDB_CloseDev,  CamFrame::OnCloseDev)
	EVT_BUTTON(IDB_Acquire,  CamFrame::Acquire)
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

	//m_camNB->AddPage(new wxPanel(m_camNB), wxString("toto"), FALSE, -1);

	// %%%%%%%
	m_settingsPane = new CamPanelSettings(m_camNB,wxT("Settings"), wxPoint(-1,-1), wxSize(-1,-1));
	m_settingsPane->InitSettings();
	m_camNB->AddPage(m_settingsPane, wxT("Settings"), TRUE, -1);

	// %%%%%%
	m_viewRangePane = new CamViewData(m_camNB,wxT("Range"), wxPoint(-1,-1), wxSize(100,100));
	m_viewRangePane->InitViewData();
	m_camNB->AddPage(m_viewRangePane, wxT("Range"), TRUE, -1);
	
	//Connect(IDB_CLOSE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CamFrame::OnClose));

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
  if(m_sr != NULL)
  {
	  m_nSrBufSz = (int) SR_GetBufferSize(m_sr); 
	  m_nCols = (int) SR_GetCols(m_sr); 
	  m_nRows = (int) SR_GetRows(m_sr); 
	  strR.sprintf(wxT("cam serial %i - %ix%i  - %i"), serial, m_nRows, m_nCols, m_nSrBufSz); // ... change text ...
	  m_pSrBuf = (unsigned char*) malloc(m_nSrBufSz);
	  memset(m_pSrBuf, 0x77, m_nSrBufSz);
	  res = SR_SetBuffer(m_sr, (void*) m_pSrBuf , m_nSrBufSz);
	  res = SR_Acquire(m_sr, AM_COR_FIX_PTRN || AM_COR_LED_NON_LIN );
	  res = m_viewRangePane->SetUshortData((unsigned short*) m_pSrBuf, m_nRows*m_nCols);
  }
  m_settingsPane->SetText(strR);
  //m_settingsPane->SetText(wxT("Open successfull"));
}


//! Acquire 1 Frame
void CamFrame::Acquire(wxCommandEvent& WXUNUSED(event))
{
  int res = 0;
  wxString strR;
  if((m_sr != NULL) && (m_pSrBuf != NULL) )
  {
	  // ... change text ...
	  res = SR_Acquire(m_sr, AM_COR_FIX_PTRN || AM_COR_LED_NON_LIN );
	  /*res = */ m_viewRangePane->SetUshortData((unsigned short*) m_pSrBuf, m_nRows*m_nCols);
	  strR.sprintf(wxT("cam serial %i - %ix%i  - %i"), res, m_nRows, m_nCols, m_nSrBufSz);
	  m_settingsPane->SetText(strR);
  }
  //m_settingsPane->SetText(wxT("Open successfull"));
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
  m_settingsPane->DisableCloseSR();		// disable "Close" button
  m_settingsPane->DisableRadioFilt();	// disable filter selection
  m_settingsPane->DisableRadioFrq();	// disable frequency selection
  if(m_sr != NULL)
  {
		res = SR_Close(m_sr);
		m_sr = NULL;
		m_nSrBufSz = 0; 
		m_nCols = 0; 
		m_nRows = 0; 
  }
  m_settingsPane->EnableOpenSR();	// enable "Open" button
  m_settingsPane->SetText(wxT("Close successfull"));
}
