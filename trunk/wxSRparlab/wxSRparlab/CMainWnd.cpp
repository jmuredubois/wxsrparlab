/*
 * CMainWnd.cpp
 * Implementation of the main window for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.10.19
 */

#include "wxSRparlab.h" //!< top-level header file
#include "CMainWnd.h"	//!< main window header file
#include "wxSrApp.h"	//!< application header file
#include "CamFrame.h"	//!< camera frame header file

class ThreadReadDataSync : public wxThread
{
public:
    ThreadReadDataSync(MainWnd *cWnd);
	~ThreadReadDataSync();

    // thread execution starts here
    virtual void *Entry();

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();

private:
    MainWnd *m_Wnd;
};

ThreadReadDataSync::ThreadReadDataSync(MainWnd *cWnd)
: wxThread(wxTHREAD_DETACHED)//: wxThread(wxTHREAD_JOINABLE)
{
    m_Wnd = cWnd;
};

/**
 * Camera frame class destructor \n
 */
ThreadReadDataSync::~ThreadReadDataSync()
{

}

void ThreadReadDataSync::OnExit()
{

}


DECLARE_EVENT_TYPE(wxEVT_JMUACQONEFRM, -1)
DECLARE_EVENT_TYPE(wxEVT_JMURENDER, -1)

void *ThreadReadDataSync::Entry()
{
    wxString text;
	int i = 0; //debug
	long acqTime = 1000L;
	std::vector<CamFrame*>::iterator it;  // get iterator on the camera frames
	float fps = 1.0; wxString strFps;

    for ( ; ; )
    {
		// check if we were asked to exit
        if ( TestDestroy() )
            break;
		acqTime = 10L;
		for ( it=(m_Wnd->GetCamFrms())->begin() ; it != (m_Wnd->GetCamFrms())->end(); it++, i++)
		{
			wxCommandEvent event( wxEVT_JMUACQONEFRM, IDC_AcqOne );
			acqTime += (*it)->GetAcqTime();
			(*it)->AddPendingEvent(event);
		}
		if(acqTime <66){acqTime = 66;};
		fps = 1000.0f/(float)acqTime;
		strFps.Printf(wxT("Data update: %04.1f fps"), fps);
		#ifdef JMU_USE_VTK
			m_Wnd->GetVtkWin()->setReadFpsTxt(strFps.char_str());
		#endif
		m_Wnd->Refresh();
		m_Wnd->SetStatusText(strFps);
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



BEGIN_EVENT_TABLE(MainWnd, wxFrame)
    EVT_MENU(ID_Quit, MainWnd::OnQuit)
    EVT_MENU(ID_About, MainWnd::OnAbout)
	EVT_BUTTON(IDB_AcqAll, MainWnd::AcqAll)
#ifdef JMU_USE_VTK
	EVT_TEXT(IDT_zMin, MainWnd::TextChangedZMin)
	EVT_TEXT(IDT_zMax, MainWnd::TextChangedZMax)
	EVT_TEXT(IDT_ampMin, MainWnd::TextChangedAmpMin)
	EVT_TEXT(IDT_ampMax, MainWnd::TextChangedAmpMax)
	EVT_TEXT(IDT_segMin, MainWnd::TextChangedSegMin)
	EVT_TEXT(IDT_segMax, MainWnd::TextChangedSegMax)
	EVT_CHECKBOX(IDC_visVtk, MainWnd::SetVisVtk)
	EVT_COMBOBOX(IDC_colVtk, MainWnd::SetColVtk)
	EVT_CHECKBOX(IDC_visBGVtk, MainWnd::SetVisBGVtk)
	EVT_COMBOBOX(IDC_colBGVtk, MainWnd::SetColBGVtk)
	EVT_CHECKBOX(IDC_visFGVtk, MainWnd::SetVisFGVtk)
	EVT_COMBOBOX(IDC_colFGVtk, MainWnd::SetColFGVtk)
	EVT_CHECKBOX(IDC_ParaProj, MainWnd::OnParaProj)
	EVT_CHECKBOX(IDC_SegmCbar, MainWnd::OnSegmCbar)
	EVT_CHECKBOX(IDC_AmplCbar, MainWnd::OnAmplCbar)
	EVT_CHECKBOX(IDC_DepthCbar, MainWnd::OnDepthCbar)
	EVT_TIMER(IDE_RendTimer, MainWnd::OnRendTimer)
	#ifdef JMU_ICPVTK
	EVT_BUTTON(IDB_icpVtk, MainWnd::OnICP)
	EVT_TEXT(IDT_icpIter, MainWnd::TextICPiter)
	EVT_CHECKBOX(IDC_visICP, MainWnd::SetVisICP)
	EVT_COMBOBOX(IDC_colICP, MainWnd::SetColICP)
	#endif
	#ifdef JMU_KDTREEVTK
	EVT_BUTTON(IDB_kdDistVtk, MainWnd::OnKdDist)
	#endif
#endif // JMU_USE_VTK
END_EVENT_TABLE()

IMPLEMENT_APP(SrApp)


/**
 * Main window construction \n
 * - the main window is derived from wxFrame \n
 * - it has no parent \n
 *
 */
MainWnd::MainWnd(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size),_renderTimer(this, IDE_RendTimer)
{
	_numCams = NUMCAMS;
    wxMenu *menuFile = new wxMenu;


    menuFile->Append( ID_About, _T("&About...") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_Quit, _T("E&xit") );

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, _T("&File") );

    SetMenuBar( menuBar );

    CreateStatusBar();
	wxString strU = wxGetUserName();
	wxString strM = wxT(" - Welcome ");
	wxString strP = wxT(" !");

	wxDateTime now = wxDateTime::Now();

	_buttAcqAll = NULL;
#ifdef JMU_USE_VTK
	_vtkWin = NULL;

	_txtZMin = NULL;
	_txtZMax = NULL;
	_txtMinMaxInit = false;
	_zMin = 100.0;
	_zMax = 3500.0;

	_txtAmpMin = NULL;
	_txtAmpMax = NULL;
	_txtAmpInit = false;
	_ampMin = 0.000;
	_ampMax = 2000.0;
	_txtSegMin = NULL;
	_txtSegMax = NULL;
	_txtSegInit = false;
	_segmMin = 0.0;
	_segmMax = 255.0;

	_ckParaProj = NULL;
	_ckSegmCbar = NULL; _ckAmplCbar = NULL; _ckDepthCbar = NULL;
	m_pThreadReadDataSync = NULL;
	#ifdef JMU_ICPVTK
		_buttICP = NULL;
		_visICP = NULL; _colICP= NULL; _icpTrlCM= NULL; _txtICPiter=NULL; 
		_icpIter=20;
		_icpSrc = NULL; _icpTgt = NULL;
		_icpIdxSrc = NULL; _icpIdxTgt = NULL;
	#endif
	#ifdef JMU_KDTREEVTK
		_buttKdDistVtk = NULL;
		_kdDistSrc = NULL; _kdDistTgt = NULL;
		_kdDistIdxSrc = NULL; _kdDistIdxTgt = NULL;
	#endif
#endif // JMU_USE_VTK

	wxString date1 = now.Format();
	wxString strW = date1 + strM + strU + strP;
	SetStatusText(strW);

}


MainWnd::~MainWnd()
{
	if(m_pThreadReadDataSync != NULL){ m_pThreadReadDataSync->Delete(); }
#ifdef JMU_USE_VTK
	if(_vtkWin){ delete(_vtkWin); _vtkWin =NULL; };
	_visVtk.clear();
	_colVtk.clear();
	_repVtk.clear();
	_alpVtk.clear();
	_visBGVtk.clear();
	_colBGVtk.clear();
	_repBGVtk.clear();
	_alpBGVtk.clear();
	_visFGVtk.clear();
	_colFGVtk.clear();
	_repFGVtk.clear();
	_alpFGVtk.clear();
	//_visSegmVtk.clear();
	//_colSegmVtk.clear();
#endif // JMU_USE_VTK
}

/**
 * Main window close \n
 * The method: \n
 * - calls the wxWindowBase Close() method \n
 */
void MainWnd::OnQuit(wxCommandEvent& WXUNUSED(event))
{
#ifdef JMU_USE_VTK
	if(_vtkWin){ delete(_vtkWin); _vtkWin =NULL; };
#endif
    Close(TRUE);
}

/**
 * About Box \n
 * The method: \n
 * - displays an info msg using wxMessageBox \n
 */
void MainWnd::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxMessageBox(_T("This is a wxWindows SR application sample. \n Author: J. Mure-Dubois - Copyright 2008."),
        _T("About wxSRparlab"), wxOK | wxICON_INFORMATION, this);
}


/**
 * Main window init \n
 * The method: \n
 *
 */
void MainWnd::Init()
{
	_bgPanel = new wxPanel(this); // a panel to contain our controls
	wxSizerFlags flagsExpand(1); flagsExpand.Expand(); // sizer flags for expanding sizer
	wxSizerFlags flagsNoExpand(1);	// sizer flags for non-expanding sizer
	_buttAcqAll = new wxButton(_bgPanel, IDB_AcqAll, wxT("Acq. all") );
#ifdef JMU_USE_VTK
	// depth min and max text controls
    _txtZMin = new wxTextCtrl( _bgPanel, IDT_zMin, wxT("0.0"));
	_txtZMax = new wxTextCtrl( _bgPanel, IDT_zMax, wxT("3500.0"));
	_txtMinMaxInit = true; _txtZMin->SetModified(true); _txtZMax->SetModified(true);

	wxBoxSizer *sizerZscale = new wxBoxSizer(wxHORIZONTAL);
		//sizerZscale->Add(_buttAcqAll, flagsExpand);
	    sizerZscale->Add(_txtZMin, flagsExpand);
		sizerZscale->AddStretchSpacer();
	    sizerZscale->Add(_txtZMax, flagsExpand);

	// amplitude min and max controls
	_txtAmpMin = new wxTextCtrl( _bgPanel, IDT_ampMin, wxString::Format(wxT("%d"), _ampMin) ); 
	_txtAmpMax = new wxTextCtrl( _bgPanel, IDT_ampMax, wxString::Format(wxT("%d"), _ampMax) );
	_txtAmpInit = true; _txtAmpMin->SetModified(true); _txtAmpMax->SetModified(true);
	
	wxBoxSizer *sizerAmpScale = new wxBoxSizer(wxHORIZONTAL);
	    sizerAmpScale->Add(_txtAmpMin, flagsExpand);
		sizerAmpScale->AddStretchSpacer();
	    sizerAmpScale->Add(_txtAmpMax, flagsExpand);

	// segmentation min and max controls
	_txtSegMin = new wxTextCtrl( _bgPanel, IDT_segMin, wxString::Format(wxT("%d"), _ampMin) ); 
	_txtSegMax = new wxTextCtrl( _bgPanel, IDT_segMax, wxString::Format(wxT("%d"), _ampMax) );
	_txtSegInit = true; _txtSegMin->SetModified(true); _txtSegMax->SetModified(true);
	
	wxBoxSizer *sizerSegScale = new wxBoxSizer(wxHORIZONTAL);
	    sizerSegScale->Add(_txtSegMin, flagsExpand);
		sizerSegScale->AddStretchSpacer();
	    sizerSegScale->Add(_txtSegMax, flagsExpand);

	// group scale textboxes
	wxFlexGridSizer *sizerZAmpScales = new wxFlexGridSizer(2,1,0,0);
		sizerZAmpScales->Add( sizerZscale, flagsExpand);
		sizerZAmpScales->Add( sizerAmpScale, flagsExpand);
		sizerZAmpScales->Add( sizerSegScale, flagsExpand);
		
	#ifdef JMU_KDTREEVTK
	
	#endif
	_ckParaProj = new wxCheckBox(_bgPanel, IDC_ParaProj, wxT("Parallel projection"));
	wxBoxSizer *sizerVtk0 = new wxBoxSizer(wxVERTICAL); // create sizer for frame
		sizerVtk0->Add(_ckParaProj, flagsExpand);

	wxString srcTgt[] = { wxT("Current"), wxT("Background"), wxT("Foreground"), wxT("Segmentation")};
	wxString strCams[NUMCAMS]; 
	for(int i = 0; i<NUMCAMS; i++){
		strCams[i].sprintf(wxT("%i"),i); 
		//labT.sprintf(wxT("Cam %i"), i); // ... change cam nickname ...
	}
#ifdef JMU_ICPVTK
	_buttICP = new wxButton(_bgPanel, IDB_icpVtk, wxT("ICP") );
	_visICP = new wxCheckBox(_bgPanel, IDC_visICP, wxT("Hide"));
	wxString colICP[] = { wxT("Depth (Z)"), wxT("Amplitude"), wxT("Segmentation")};
	_colICP=  new wxComboBox(_bgPanel, IDC_colICP, wxT("Amplitude"),
		   wxDefaultPosition, wxDefaultSize, 3, colICP, wxCB_READONLY);
	_icpTrlCM= new wxCheckBox(_bgPanel, IDC_icpTrlCM, wxT("Match center of mass"));
	wxStaticText* icpIterLabel = new wxStaticText(_bgPanel, wxID_ANY, wxT("ICP iter.")); 
	_txtICPiter=new wxTextCtrl( _bgPanel, IDT_icpIter, wxString::Format(wxT("%i"), _icpIter) );
	_icpSrc = new wxComboBox(_bgPanel, IDC_icpSrc, wxT("Current"),
			wxDefaultPosition, wxDefaultSize, 4, srcTgt, wxCB_READONLY);
	_icpTgt = new wxComboBox(_bgPanel, IDC_icpTgt, wxT("Current"),
			wxDefaultPosition, wxDefaultSize, 4, srcTgt, wxCB_READONLY);
	_icpIdxSrc = new wxRadioBox(_bgPanel, IDC_icpIdxSrc, wxT("Cam"),
			wxDefaultPosition, wxDefaultSize, NUMCAMS, strCams);
	_icpIdxTgt = new wxRadioBox(_bgPanel, IDC_icpIdxTgt, wxT("Cam"),
			wxDefaultPosition, wxDefaultSize, NUMCAMS, strCams);

	wxBoxSizer *sizerICPopt = new wxBoxSizer(wxHORIZONTAL); // create sizer ICPoptions
	    sizerICPopt->Add(icpIterLabel,flagsNoExpand);
	    sizerICPopt->Add(_txtICPiter,flagsNoExpand);
		sizerICPopt->Add(_icpTrlCM,flagsNoExpand);
		sizerICPopt->Add(_visICP,flagsNoExpand);
		sizerICPopt->Add(_colICP,flagsNoExpand);
	wxBoxSizer *sizerICP = new wxBoxSizer(wxHORIZONTAL); // create sizer ICP params
		sizerICP->Add(_buttICP, flagsNoExpand);
		sizerICP->Add(_icpIdxSrc , flagsExpand);
		sizerICP->Add(_icpSrc , flagsExpand);
		sizerICP->Add(_icpIdxTgt , flagsExpand);
		sizerICP->Add(_icpTgt , flagsExpand);

	sizerVtk0->Add(sizerICPopt, flagsNoExpand);
	sizerVtk0->Add(sizerICP, flagsExpand);
#endif
#ifdef JMU_KDTREEVTK
	_buttKdDistVtk = new wxButton(_bgPanel, IDB_kdDistVtk, wxT("kdDist") );
	_kdDistSrc = new wxComboBox(_bgPanel, IDC_kdDistSrc, wxT("Current"),
			wxDefaultPosition, wxDefaultSize, 4, srcTgt, wxCB_READONLY);
	_kdDistTgt = new wxComboBox(_bgPanel, IDC_kdDistTgt, wxT("Current"),
			wxDefaultPosition, wxDefaultSize, 4, srcTgt, wxCB_READONLY);
	_kdDistIdxSrc = new wxRadioBox(_bgPanel, IDC_kdDistIdxSrc, wxT("Cam"),
			wxDefaultPosition, wxDefaultSize, NUMCAMS, strCams);
	_kdDistIdxTgt = new wxRadioBox(_bgPanel, IDC_kdDistIdxTgt, wxT("Cam"),
			wxDefaultPosition, wxDefaultSize, NUMCAMS, strCams);

	wxBoxSizer *sizerKdDist = new wxBoxSizer(wxHORIZONTAL); // create sizer KdDist params
		sizerKdDist->Add(_buttKdDistVtk, flagsExpand);
		sizerKdDist->Add(_kdDistIdxSrc , flagsExpand);
		sizerKdDist->Add(_kdDistSrc    , flagsExpand);
		sizerKdDist->Add(_kdDistIdxTgt , flagsExpand);
		sizerKdDist->Add(_kdDistTgt    , flagsExpand);

	sizerVtk0->Add(sizerKdDist, flagsExpand);
#endif
	_ckSegmCbar  = new wxCheckBox(_bgPanel, IDC_SegmCbar, wxT("Hide Segm. scale"));
	_ckAmplCbar  = new wxCheckBox(_bgPanel, IDC_AmplCbar, wxT("Hide Ampl. scale"));
	_ckDepthCbar = new wxCheckBox(_bgPanel, IDC_DepthCbar, wxT("Hide Depth. scale"));
	wxBoxSizer *sizerCk = new wxBoxSizer(wxHORIZONTAL); // create sizer for frame
		//sizerCk->Add(sizerVtk0, flagsExpand);
		sizerCk->Add(_ckSegmCbar, flagsExpand);
		sizerCk->Add(_ckAmplCbar, flagsExpand);
		sizerCk->Add(_ckDepthCbar, flagsExpand);
#endif // JMU_USE_VTK

	wxFlexGridSizer *sizerAcqScales = new wxFlexGridSizer(1,2,0,0);
		sizerAcqScales->Add(_buttAcqAll, flagsExpand);
#ifdef JMU_USE_VTK
		sizerAcqScales->Add( sizerZAmpScales, flagsExpand);

	_sizerCamVisCol = new wxGridBagSizer(); // used later, in AddChildren
#endif // JMU_USE_VTK

	wxGridBagSizer *sizerPanel = new wxGridBagSizer();
#ifdef JMU_USE_VTK
		sizerPanel->Add(_sizerCamVisCol, wxGBPosition(0,0), wxGBSpan(4,4));
#endif // JMU_USE_VTK
		sizerPanel->Add(sizerAcqScales, wxGBPosition(6,0), wxGBSpan(2,4));
#ifdef JMU_USE_VTK
		//sizerPanel->Add(_ckParaProj, wxGBPosition(10,0), wxGBSpan(1,4));
		sizerPanel->Add(sizerVtk0, wxGBPosition(8,0), wxGBSpan(1,4));
		sizerPanel->Add(sizerCk, wxGBPosition(9,0), wxGBSpan(1,4));
#endif // JMU_USE_VTK

	wxBoxSizer *sizerFrame = new wxBoxSizer(wxVERTICAL); // create sizer for frame
		sizerFrame->Add(_bgPanel, flagsExpand);

	_bgPanel->SetSizerAndFit(sizerPanel); // fit sizer to bg panel
	this->SetSizerAndFit(sizerFrame);

	_rendTgt = 200; // target rendering time
	_rendCapms = 2.0*double(_rendTgt);
	_renderTimer.Start(_rendTgt); //rendering every ... ms
}

/**
 * Main window init \n
 * The method: \n
 *
 */
void MainWnd::AddChildren()
{
    // temp variables to avoid creating new camFrames on top of each other
	wxPoint pos = wxPoint(50,500);	// initial position
	wxPoint incr = wxPoint(100,50);	// increment in position
	wxSize	sz = wxSize(500,490);	// size for camFrame
	wxString lab;					// title string for camFrame
	wxString labT;					// string for cam nickname
	wxString labBGT;					// string for cam BGnickname
	wxString labFGT;					// string for cam FGnickname
	wxString labSegmT;					// string for cam Segmnickname

#ifdef JMU_USE_VTK
	_vtkWin = new CViewSrVtk(NULL, 440,50,1024,768);

	wxString colors[] = { wxT("Depth (Z)"), wxT("Amplitude"), wxT("Segmentation"), wxT("red"),
        wxT("green"), wxT("blue"), wxT("white"), wxT("black") };
#endif

	//! - for the max number of cameras ...
	for(int i = 0; i<_numCams; i++){
		lab.sprintf(wxT("Camera %i"), i); // ... change title text ...
		labT.sprintf(wxT("Cam %i"), i); // ... change cam nickname ...
		labBGT.sprintf(wxT("BGcam %i"), i); // ... change cam BG nickname ...
		labFGT.sprintf(wxT("FGcam %i"), i); // ... change cam FG nickname ...
		labSegmT.sprintf(wxT("Segm. %i"), i); // ... change cam Segm nickname ...
		//! ... create and show new camFrame ... \n
		CamFrame *camFrm = new CamFrame( this, lab, pos, sz );
		camFrm->Show(TRUE);
		camFrm->CreateAndSetNotebook();
#ifdef JMU_USE_VTK
		camFrm->SetVtkWin(_vtkWin, i);
#endif
		m_camFrm.push_back(camFrm);
		pos += incr; //... increment position.

#ifdef JMU_USE_VTK
		//llabT.sprintf(wxT("Cam %i"), i); // ... change cam nickname ...
		wxCheckBox *chkBox = new wxCheckBox(_bgPanel, IDC_visVtk, labT);	
		chkBox->SetValue(true);
		_sizerCamVisCol->Add(chkBox, wxGBPosition(i,0));
		_visVtk.push_back(chkBox);	// add visibility checkbox to container
		wxComboBox* colBox = new wxComboBox(_bgPanel, IDC_colVtk, wxT("Amplitude"),
			wxDefaultPosition, wxDefaultSize, 8, colors, wxCB_READONLY);
		_sizerCamVisCol->Add(colBox, wxGBPosition(i,1));
		_colVtk.push_back(colBox);
		// labBGT.sprintf(wxT("BGcam %i"), i); // ... change cam BG nickname ...
		wxCheckBox *chkBGBox = new wxCheckBox(_bgPanel, IDC_visBGVtk, labBGT);	
		chkBGBox->SetValue(true);
		_sizerCamVisCol->Add(chkBGBox, wxGBPosition(i,2));
		_visBGVtk.push_back(chkBGBox);	// add visibility checkbox to container
		wxComboBox* colBGBox = new wxComboBox(_bgPanel, IDC_colBGVtk, wxT("Depth (Z)"),
			wxDefaultPosition, wxDefaultSize, 8, colors, wxCB_READONLY);
		_sizerCamVisCol->Add(colBGBox, wxGBPosition(i,3));
		_colBGVtk.push_back(colBGBox);
		// labFGT.sprintf(wxT("FGcam %i"), i); // ... change cam FG nickname ...
		wxCheckBox *chkFGBox = new wxCheckBox(_bgPanel, IDC_visFGVtk, labFGT);	
		chkFGBox->SetValue(true);
		_sizerCamVisCol->Add(chkFGBox, wxGBPosition(i,4));
		_visFGVtk.push_back(chkFGBox);	// add visibility checkbox to container
		wxComboBox* colFGBox = new wxComboBox(_bgPanel, IDC_colFGVtk, wxT("Amplitude"),
			wxDefaultPosition, wxDefaultSize, 8, colors, wxCB_READONLY);
		_sizerCamVisCol->Add(colFGBox, wxGBPosition(i,5));
		_colFGVtk.push_back(colFGBox);
		// labSegmT.sprintf(wxT("Segm. %i"), i); // ... change cam Segm nickname ...
		/*wxCheckBox *chkSegmBox = new wxCheckBox(_bgPanel, IDC_visVtk, labSegmT);	
		chkSegmBox->SetValue(true);
		_sizerCamVisCol->Add(chkSegmBox, wxGBPosition(i,6));
		_visSegmVtk.push_back(chkSegmBox);	// add visibility checkbox to container
		wxComboBox* colSegmBox = new wxComboBox(_bgPanel, IDC_colVtk, wxT("Depth (Z)"),
			wxDefaultPosition, wxDefaultSize, 8, colors, wxCB_READONLY);
		_sizerCamVisCol->Add(colSegmBox, wxGBPosition(i,7));
		_colSegmVtk.push_back(colSegmBox);*/


		camFrm->SendSizeEvent();
		this->SetSegmVtk();
		this->SetAmpVtk();
		this->SetZVtk();
#endif
	} // ENDOF for loop on _numCams
	this->Layout();
}

#ifdef JMU_USE_VTK
/* Setting display min value*/
void MainWnd::SetZMin(double val)
{
	_zMin = val;
	_txtZMin->GetValue().Printf(wxT("%d"), val);
	_txtZMin->SetModified(true);
	SetZVtk();
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* Setting display min value*/
void MainWnd::SetZMax(double val)
{
	_zMax = val;
	_txtZMax->GetValue().Printf(wxT("%d"), val);
	_txtZMax->SetModified(true);
	SetZVtk();
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* Setting vtk display value*/
void MainWnd::SetZVtk()
{
	if(_vtkWin != NULL){
		std::vector<CamFrame*>::iterator itCam;  // get iterator on the camera frames
		for ( itCam  =m_camFrm.begin();
		      itCam !=m_camFrm.end(); 
		      itCam++)
		{
			(*itCam)->changeDepthRangeVtk((float) _zMin, (float) _zMax);
		}
		_vtkWin->changeDepthRange((float) _zMin, (float) _zMax);
		_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
	}
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on chaged text value */
void MainWnd::TextChangedZMin(wxCommandEvent &)
{
	double val = 0;
	if( !_txtMinMaxInit){return ;};
	wxString strVal = _txtZMin->GetValue();
	if( strVal.ToDouble(& val) ) /* read value as double*/
	{
		SetZMin(val);
	}
	else
	{
		_txtZMin->DiscardEdits();
		_txtZMin->GetValue().Printf(wxT("%d"), _zMin);
	}
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on chaged text value */
void MainWnd::TextChangedZMax(wxCommandEvent &)
{
	double val = 0;
	if( !_txtMinMaxInit){return ;};
	wxString strVal = _txtZMax->GetValue();
	if( strVal.ToDouble(& val) ) /* read value as double*/
	{
		SetZMax(val);
	}
	else
	{
		_txtZMax->DiscardEdits();
		_txtZMax->GetValue().Printf(wxT("%d"), _zMax);
	}
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* Setting display min value*/
void MainWnd::SetAmpMin(double val)
{
	_ampMin = val;
	_txtAmpMin->GetValue().Printf(wxT("%d"), val);
	_txtAmpMin->SetModified(true);
	SetAmpVtk();
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* Setting display min value*/
void MainWnd::SetAmpMax(double val)
{
	_ampMax = val;
	_txtAmpMax->GetValue().Printf(wxT("%d"), val);
	_txtAmpMax->SetModified(true);
	SetAmpVtk();
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* Setting vtk display value*/
void MainWnd::SetAmpVtk()
{
#ifdef JMU_USE_VTK
	if(_vtkWin != NULL){ 
		std::vector<CamFrame*>::iterator itCam;  // get iterator on the camera frames
		for ( itCam  =m_camFrm.begin();
		      itCam !=m_camFrm.end(); 
		      itCam++)
		{
			(*itCam)->changeAmpRangeVtk((float) _ampMin, (float) _ampMax);
		}
		_vtkWin->changeAmpRange((float) _ampMin, (float) _ampMax);
		_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
	}
#endif
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* Setting display min value*/
void MainWnd::SetSegMin(double val)
{
	_segmMin = val;
	_txtSegMin->GetValue().Printf(wxT("%d"), val);
	_txtSegMin->SetModified(true);
	SetSegmVtk();
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* Setting display min value*/
void MainWnd::SetSegMax(double val)
{
	_segmMax = val;
	_txtSegMax->GetValue().Printf(wxT("%d"), val);
	_txtSegMax->SetModified(true);
	SetSegmVtk();
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* Setting vtk display value*/
void MainWnd::SetSegmVtk()
{
#ifdef JMU_USE_VTK
	if(_vtkWin != NULL){ 
		std::vector<CamFrame*>::iterator itCam;  // get iterator on the camera frames
		for ( itCam  =m_camFrm.begin();
		      itCam !=m_camFrm.end(); 
		      itCam++)
		{
			(*itCam)->changeSegmRangeVtk((float) _segmMin, (float) _segmMax);
		}
		_vtkWin->changeSegmRange((float) _segmMin, (float) _segmMax);
		_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
	}
#endif
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on changed text value */
void MainWnd::TextChangedAmpMin(wxCommandEvent &)
{
	double val = 0;
	if( !_txtAmpInit){return ;};
	wxString strVal = _txtAmpMin->GetValue();
	if( strVal.ToDouble(& val) ) /* read value as double*/
	{
		SetAmpMin(val);
	}
	else
	{
		_txtAmpMin->DiscardEdits();
		_txtAmpMin->GetValue().Printf(wxT("%d"), _ampMin);
	}
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on changed text value */
void MainWnd::TextChangedAmpMax(wxCommandEvent &)
{
	double val = 0;
	if( !_txtAmpInit){return ;};
	wxString strVal = _txtAmpMax->GetValue();
	if( strVal.ToDouble(& val) ) /* read value as double*/
	{
		SetAmpMax(val);
	}
	else
	{
		_txtAmpMax->DiscardEdits();
		_txtAmpMax->GetValue().Printf(wxT("%d"), _ampMax);
	}
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on changed text value */
void MainWnd::TextChangedSegMin(wxCommandEvent &)
{
	double val = 0;
	if( !_txtSegInit){return ;};
	wxString strVal = _txtSegMin->GetValue();
	if( strVal.ToDouble(& val) ) /* read value as double*/
	{
		SetSegMin(val);
	}
	else
	{
		_txtSegMin->DiscardEdits();
		_txtSegMin->GetValue().Printf(wxT("%d"), _segmMin);
	}
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on changed text value */
void MainWnd::TextChangedSegMax(wxCommandEvent &)
{
	double val = 0;
	if( !_txtSegInit){return ;};
	wxString strVal = _txtSegMax->GetValue();
	if( strVal.ToDouble(& val) ) /* read value as double*/
	{
		SetSegMax(val);
	}
	else
	{
		_txtSegMax->DiscardEdits();
		_txtSegMax->GetValue().Printf(wxT("%d"), _segmMax);
	}
}
#endif // JMU_USE_VTK

/* acting on "AcquireAll" value button*/
void MainWnd::AcqAll(wxCommandEvent& event)
{
	int i = 0; //debug
	std::vector<CamFrame*>::iterator it;  // get iterator on the camera frames
	bool contin = true;
	for ( it=m_camFrm.begin() ; it != m_camFrm.end(); it++, i++)
	{
		//(*it)->Acquire(event);
		contin = contin && (*it)->IsAcqContinuous();
	}
	if(contin)
	{
		if (m_pThreadReadDataSync == NULL)
		{
			m_pThreadReadDataSync = new ThreadReadDataSync(this);	// create reading thread...
			m_pThreadReadDataSync->Create();
			m_pThreadReadDataSync->Run();						// ... and run it.
		}
		else if ((m_pThreadReadDataSync != NULL) && (m_pThreadReadDataSync->IsRunning()))
		{
			m_pThreadReadDataSync->Delete();
			m_pThreadReadDataSync = NULL;
		}
	}
	else
	{
		for ( it=m_camFrm.begin() ; it != m_camFrm.end(); it++, i++)
		{
			wxCommandEvent event( wxEVT_JMUACQONEFRM, IDC_AcqOne );
			(*it)->ProcessEvent(event);
		}
	}

}

#ifdef JMU_USE_VTK
/* acting on visVTK checkbox*/
void MainWnd::SetVisVtk(wxCommandEvent& event)
{
	SetVisVtkIdx(_visVtk, m_camFrm,0);
	_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on visVTK checkbox*/
void MainWnd::SetVisBGVtk(wxCommandEvent& event)
{
	SetVisVtkIdx(_visBGVtk, m_camFrm,1);
	_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on visVTK checkbox*/
void MainWnd::SetVisFGVtk(wxCommandEvent& event)
{
	SetVisVtkIdx(_visFGVtk, m_camFrm,2);
	_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on visVTK checkbox*/
void MainWnd::SetVisVtkIdx(std::vector<wxCheckBox*> visChkBox, std::vector<CamFrame*> camFrm, int idx)
{
	int i = 0;
	std::vector<wxCheckBox*>::iterator itCtrl;  // get iterator on the controls
	std::vector<CamFrame*>::iterator itCam;  // get iterator on the camera frames
	for ( itCtrl  = visChkBox.begin(), itCam  = camFrm.begin();
		  itCtrl != visChkBox.end()  , itCam != camFrm.end(); 
		  itCtrl++, itCam++, i++ )
	{
		if(!_vtkWin){return;};
		(*itCam)->hideDataActVtk( !((*itCtrl)->IsChecked()) , idx);
	}
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on colVTK radio box*/
void MainWnd::SetColVtk(wxCommandEvent& event)
{
	SetColVtkIdx(_colVtk, m_camFrm,0);
	_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on colVTK radio box*/
void MainWnd::SetColBGVtk(wxCommandEvent& event)
{
	SetColVtkIdx(_colBGVtk, m_camFrm,1);
	_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on colVTK radio box*/
void MainWnd::SetColFGVtk(wxCommandEvent& event)
{
	SetColVtkIdx(_colFGVtk, m_camFrm,2);
	_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on colVTK radio box*/
void MainWnd::SetColVtkIdx(std::vector<wxComboBox*> colCmbBox, std::vector<CamFrame*> camFrm, int idx)
{
	int i = 0;
	std::vector<wxComboBox*>::iterator itCtrl;  // get iterator on the controls
	std::vector<CamFrame*>::iterator itCam;  // get iterator on the camera frames
	for ( itCtrl  =colCmbBox.begin(), itCam  =camFrm.begin();
		  itCtrl !=colCmbBox.end()  , itCam !=camFrm.end(); 
		  itCtrl++, itCam++, i++ )
	{
		if(!_vtkWin){return;};
		/* wxString colors[] = { wxT("Depth (Z)"), wxT("Amplitude"), wxT("Segmentation"), wxT("red"),
        wxT("green"), wxT("blue"), wxT("white"), wxT("black") };*/
		wxString strCol = (*itCtrl)->GetValue();
		double r, g, b; r=0.0; g=0.0; b=0.0;
		if(  strCol.IsSameAs(wxT("Depth (Z)"))  )
		{
			(*itCam)->setDataMapperColorDepth(idx);
			SetZMin(_zMin); // trick to update data Mapper
		}
		if(  strCol.IsSameAs(wxT("Amplitude"))  )
		{
			(*itCam)->setDataMapperColorGray(idx);
			SetAmpMin(_ampMin); // trick to update data Mapper
		}
		if(  strCol.IsSameAs(wxT("Segmentation"))  )
		{
			(*itCam)->setDataMapperColorSegm(idx);
			SetSegMin(_segmMin); // trick to update data Mapper
		}
		if(  strCol.IsSameAs(wxT("red")) || strCol.IsSameAs(wxT("green")) || strCol.IsSameAs(wxT("blue"))
			 || strCol.IsSameAs(wxT("white")) || strCol.IsSameAs(wxT("black")))
		{
			if(  strCol.IsSameAs(wxT("red"  ))  ) { (*itCam)->setDataMapperColorR(idx);} ;
			if(  strCol.IsSameAs(wxT("green"))  ) { (*itCam)->setDataMapperColorG(idx);} ;
			if(  strCol.IsSameAs(wxT("blue" ))  ) { (*itCam)->setDataMapperColorB(idx);} ;
			if(  strCol.IsSameAs(wxT("white"))  ) { (*itCam)->setDataMapperColorW(idx);} ;
			if(  strCol.IsSameAs(wxT("black"))  ) { (*itCam)->setDataMapperColorK(idx);} ;
		}
	}
}
#endif // JMU_USE_VTK
/* remove CamFrame object from list if window is closed */
void MainWnd::PopCam(int vtkSub)
{
#ifdef JMU_USE_VTK
	int i = 0;
	std::vector<wxComboBox*>::iterator itCtrl;  // get iterator on the controls
	std::vector<wxCheckBox*>::iterator itCtrl2;  // get iterator on the controls
	std::vector<wxComboBox*>::iterator itCtrl3;  // get iterator on the controls
	std::vector<wxCheckBox*>::iterator itCtrl4;  // get iterator on the controls
	std::vector<wxComboBox*>::iterator itCtrl5;  // get iterator on the controls
	std::vector<wxCheckBox*>::iterator itCtrl6;  // get iterator on the controls
	//std::vector<wxComboBox*>::iterator itCtrl7;  // get iterator on the controls
	//std::vector<wxCheckBox*>::iterator itCtrl8;  // get iterator on the controls
	std::vector<CamFrame*>::iterator itCam;  // get iterator on the camera frames
	for ( itCtrl  =_colVtk.begin()  , itCtrl2  =_visVtk.begin()  , 
		  itCtrl3 =_colBGVtk.begin(), itCtrl4  =_visBGVtk.begin(), 
		  itCtrl5 =_colFGVtk.begin(), itCtrl6  =_visFGVtk.begin(), 
		  //itCtrl7 =_colSegmVtk.begin(), itCtrl8  =_visSegmVtk.begin(), 
		  itCam  =m_camFrm.begin();
		  itCtrl  !=_colVtk.end()   , itCtrl2 !=_visVtk.end()  ,
	      itCtrl3 !=_colBGVtk.end() , itCtrl4 !=_visBGVtk.end(),
		  itCtrl5 !=_colFGVtk.end() , itCtrl6 !=_visFGVtk.end(),
		  //itCtrl7 !=_colSegmVtk.end() , itCtrl8 !=_visSegmVtk.end(),
		  itCam !=m_camFrm.end(); 
		  itCtrl++, itCtrl2++, itCtrl3++, itCtrl4++,itCtrl5++, itCtrl6++,/*itCtrl7++, itCtrl8++,*/ itCam++, i++ )
	{
		if( (*itCam)->GetVtkSub() == vtkSub)
		{
			m_camFrm.erase(itCam);
			(*itCtrl )->Disable(); _colVtk.erase(itCtrl ); 
			(*itCtrl2)->Disable(); _visVtk.erase(itCtrl2); 
			(*itCtrl3)->Disable(); _colBGVtk.erase(itCtrl3); 
			(*itCtrl4)->Disable(); _visBGVtk.erase(itCtrl4); 
			(*itCtrl5)->Disable(); _colFGVtk.erase(itCtrl5);
			(*itCtrl6)->Disable(); _visFGVtk.erase(itCtrl6);
			//(*itCtrl7)->Disable(); _colSegmVtk.erase(itCtrl8);
			//(*itCtrl8)->Disable(); _visSegmVtk.erase(itCtrl8);
			_icpIdxSrc->Enable(vtkSub, false);
			_icpIdxTgt->Enable(vtkSub, false);
			_kdDistIdxSrc->Enable(vtkSub, false);
			_kdDistIdxTgt->Enable(vtkSub, false);
			break;
		}
	}
#endif
}

#ifdef JMU_USE_VTK
/* acting on "Para proj" checkBox*/
void MainWnd::OnParaProj(wxCommandEvent& event)
{
	if(_ckParaProj == NULL) return;
	_vtkWin->setParallelProj( _ckParaProj->IsChecked() );
	_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
void MainWnd::OnRendTimer(wxTimerEvent& event) //! Render timer event action
{
	if(_vtkWin == NULL) return;
	float fps = 1.0; wxString strFps;
	fps = 1000.0f/(float)_rendTgt;
	strFps.Printf(wxT("Rendering: %04.1f fps"), fps);
	_vtkWin->setFpsTxt(strFps.char_str());
	_vtkWin->Render();//JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
	_rendCapms = 0.8*_rendCapms + (_vtkWin->timeRender()*1000);
	double rendDiff = _rendCapms - (double)_rendTgt;
	if( rendDiff > 0) // if rendering is slower than target
	{
		_rendTgt = (int)(1.2*_rendCapms); // allow more rendering time
		_renderTimer.Start(_rendTgt);
	}
	if( (rendDiff < 0) && (-rendDiff > 0.2*_rendCapms) && ((int)(0.9*_rendCapms) > 66) ) // if rendering is faster than target
	{
		_rendTgt = (int)(0.9*_rendCapms); // shorten allowed rendering time
		_renderTimer.Start(_rendTgt);
	}
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on "Hide Segm. colorbar" checkBox*/
void MainWnd::OnSegmCbar(wxCommandEvent& event)
{
	if(_ckSegmCbar == NULL) return;
	_vtkWin->hideSegmCbar( _ckSegmCbar->IsChecked() );
	_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on "Hide Ampl. colorbar" checkBox*/
void MainWnd::OnAmplCbar(wxCommandEvent& event)
{
	if(_ckAmplCbar == NULL) return;
	_vtkWin->hideAmplCbar( _ckAmplCbar->IsChecked() );
	_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on "Hide Depth colorbar" checkBox*/
void MainWnd::OnDepthCbar(wxCommandEvent& event)
{
	if(_ckSegmCbar == NULL) return;
	_vtkWin->hideDepthCbar( _ckDepthCbar->IsChecked() );
	_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
}
#endif // JMU_USE_VTK
#ifdef JMU_ICPVTK
void MainWnd::SetVisICP(wxCommandEvent& event)
{
	_vtkWin->hideICPact(_visICP->IsChecked());
	_vtkWin->Render();
}
#endif // JMU_ICPVTK
#ifdef JMU_ICPVTK
void MainWnd::SetColICP(wxCommandEvent& event)
{
	/* wxString colICP[] = { wxT("Depth (Z)"), wxT("Amplitude"), wxT("Segmentation") };*/
		wxString strCol = _colICP->GetValue();
		if(  strCol.IsSameAs(wxT("Depth (Z)"))  )
		{
			_vtkWin->setICPColorDepth();
			SetZMin(_zMin); // trick to update data Mapper
		}
		if(  strCol.IsSameAs(wxT("Amplitude"))  )
		{
			_vtkWin->setICPColorGray();
			SetAmpMin(_ampMin); // trick to update data Mapper
		}
		if(  strCol.IsSameAs(wxT("Segmentation"))  )
		{
			_vtkWin->setICPColorSegm();
			SetSegMin(_segmMin); // trick to update data Mapper
		}
}
#endif // JMU_ICPVTK
#ifdef JMU_ICPVTK
void MainWnd::TextICPiter(wxCommandEvent& )
{
	unsigned long val = 0;
	if( !_txtICPiter){return ;};
	wxString strVal = _txtICPiter->GetValue();
	if( strVal.ToULong(& val) ) /* read value as double*/
	{
		_icpIter = (int) val;
	}
	else
	{
		_txtICPiter->DiscardEdits();
		_txtICPiter->GetValue().Printf(wxT("%u"), _icpIter);
	}
}
#endif // JMU_ICPVTK
//#ifdef JMU_ICPVTK
//void MainWnd::SetICPtrlCM(wxCommandEvent& event)
//{
//}
//#endif // JMU_ICPVTK
#ifdef JMU_ICPVTK
/** acting on "ICP" button \n
 * - bug: for now only first and last cam are used \n
 * - todo: make dataset choice configurable
 */
void MainWnd::OnICP(wxCommandEvent& event)
{
	#ifdef JMU_ICPVTK
	int idxTgt = _icpIdxTgt->GetSelection();
	int tgtField = 0;
	wxString strTgt = _icpTgt->GetValue();
	if( strTgt.IsSameAs(wxT("Current"))      ) {tgtField = 0;};
	if( strTgt.IsSameAs(wxT("Background"))   ) {tgtField = 1;};
	if( strTgt.IsSameAs(wxT("Foreground"))   ) {tgtField = 2;};
	if( strTgt.IsSameAs(wxT("Segmentation")) ) {tgtField = 3;};

	int idxSrc = _icpIdxSrc->GetSelection();
	wxString strSrc = _icpSrc->GetValue();
	int srcField = 0;
	if( strSrc.IsSameAs(wxT("Current"))      ) { srcField = 0;};
	if( strSrc.IsSameAs(wxT("Background"))   ) { srcField = 1;};
	if( strSrc.IsSameAs(wxT("Foreground"))   ) { srcField = 2;};
	if( strSrc.IsSameAs(wxT("Segmentation")) ) { srcField = 3;};

	double mat[16]; for(int k =0; k<16; k++ ) { mat[k] = 0.0;}; mat[0]=1.0; mat[5]=1.0; mat[10]=1.0; mat[15]=1.0;
	//vtkStructuredGrid* icpSource = _vtkWin->icpFct(this->GetCamFrms(), idxSrc, srcField, idxTgt, tgtField, mat);
	//(m_camFrm.back() )->GetCamVtk()->ShowStructGrid(icpSource);
	int trlCM = 0;
	if(_icpTrlCM->IsChecked()) {trlCM =1;};
	wxString strRes = _vtkWin->icpFct(this->GetCamFrms(), idxSrc, srcField, idxTgt, tgtField, _icpIter, trlCM, mat);
	wxString fname; fname.Printf(wxT("icpRes_tgt%02u_field%02u_src%02u_field%02u.xml"), idxTgt, tgtField,idxSrc, srcField);
	wxDateTime now = wxDateTime::Now(); wxString date1 = now.Format();
	wxString comments; 
	comments =  date1 + wxString(wxT("\n")) + strRes;
	m_camFrm[0]->WriteCamTrfMat4(fname, mat, comments);
	_vtkWin->Render();
	#endif //JMU_ICPVTK
}
#endif // JMU_USE_VTK
#ifdef JMU_KDTREEVTK
/** acting on "kdDist" button \n
 * - bug: for now only first and last cam are used \n
 * - todo: make dataset choice configurable
 */
void MainWnd::OnKdDist(wxCommandEvent& event)
{
	#ifdef JMU_KDTREEVTK
	int idxTgt = _kdDistIdxTgt->GetSelection();
	int tgtField = 0;
	wxString strTgt = _kdDistTgt->GetValue();
	if( strTgt.IsSameAs(wxT("Current"))      ) {tgtField = 0;};
	if( strTgt.IsSameAs(wxT("Background"))   ) {tgtField = 1;};
	if( strTgt.IsSameAs(wxT("Foreground"))   ) {tgtField = 2;};
	if( strTgt.IsSameAs(wxT("Segmentation")) ) {tgtField = 3;};

	int idxSrc = _kdDistIdxSrc->GetSelection();
	wxString strSrc = _kdDistSrc->GetValue();
	int srcField = 0;
	if( strSrc.IsSameAs(wxT("Current"))      ) { srcField = 0;};
	if( strSrc.IsSameAs(wxT("Background"))   ) { srcField = 1;};
	if( strSrc.IsSameAs(wxT("Foreground"))   ) { srcField = 2;};
	if( strSrc.IsSameAs(wxT("Segmentation")) ) { srcField = 3;};

	double res[3]; res[0] = -1; res[1] = -1; res[2] = -1;
	double eps = _vtkWin->kdDist(this->GetCamFrms(), idxSrc, srcField, idxTgt, tgtField, res);
	wxString strDist;
	strDist.sprintf(wxT("kdDist returned: %g - avg=%g - std=%g - eps=%g"), eps, res[1], res[2], res[0]);
	SetStatusText(strDist);
	#endif //JMU_KDTREEVTK
}
#endif // JMU_USE_VTK

