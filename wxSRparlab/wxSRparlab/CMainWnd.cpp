/*
 * CMainWnd.cpp
 * Implementation of the main window for wxSRparlab
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
	  //#ifdef JMU_KDTREEVTK
	  //#ifdef JMU_KDSAVE
	  //   this->KdDistPNG();
	  //#endif
	  //#endif
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
	EVT_COMBOBOX(IDC_repVtk, MainWnd::SetRepVtk)
	EVT_COMMAND_SCROLL_THUMBRELEASE(IDC_alpVtk, MainWnd::SetAlpVtk)
	EVT_CHECKBOX(IDC_visBGVtk, MainWnd::SetVisBGVtk)
	EVT_COMBOBOX(IDC_colBGVtk, MainWnd::SetColBGVtk)
	EVT_COMBOBOX(IDC_repBGVtk, MainWnd::SetRepBGVtk)
	EVT_COMMAND_SCROLL_THUMBRELEASE(IDC_alpBGVtk, MainWnd::SetAlpBGVtk)
	EVT_CHECKBOX(IDC_visFGVtk, MainWnd::SetVisFGVtk)
	EVT_COMBOBOX(IDC_colFGVtk, MainWnd::SetColFGVtk)
	EVT_COMBOBOX(IDC_repFGVtk, MainWnd::SetRepFGVtk)
	EVT_COMMAND_SCROLL_THUMBRELEASE(IDC_alpFGVtk, MainWnd::SetAlpFGVtk)
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
	EVT_COMBOBOX(IDC_repICP, MainWnd::SetRepICP)
	EVT_COMMAND_SCROLL_THUMBRELEASE(IDC_alpICP, MainWnd::SetAlpICP)
	#endif
	#ifdef JMU_KDTREEVTK
	EVT_BUTTON(IDB_kdDistVtk, MainWnd::OnKdDist)
	EVT_TEXT(IDT_kdDistThr, MainWnd::TextKdDistThr)
	EVT_CHECKBOX(IDC_kdDistSave, MainWnd::OnKdDistSave)
	#endif
#endif // JMU_USE_VTK
#ifdef JMU_ALIGNGUI
	EVT_BUTTON(IDB_aliPla, MainWnd::OnAlignPlans)
#endif
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
	_ampMax = 5000.0;
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
		_repICP=NULL; _alpICP=NULL;
		_icpIter=20;
		_icpSrc = NULL; _icpTgt = NULL;
		_icpIdxSrc = NULL; _icpIdxTgt = NULL;
	#endif
	#ifdef JMU_KDTREEVTK
		_buttKdDistVtk = NULL;
		_kdDistSrc = NULL; _kdDistTgt = NULL;
		_kdDistIdxSrc = NULL; _kdDistIdxTgt = NULL;
		_kdDistThr=NULL; _kdThr=0.0;
		_kdDistBlank = NULL;
		_kdDistSave = NULL;
		_kdSavePrefix.Empty();
	#endif
#endif // JMU_USE_VTK
#ifdef JMU_ALIGNGUI
		_buttAliPla = NULL;
		_AliPlaSrc = NULL   ; _AliPlaTgt = NULL;
		_AliPlaIdxSrc = NULL; _AliPlaIdxTgt = NULL;
		m_Align = NULL;
		PLALI_Open(&m_Align);
#endif

	wxString date1 = now.Format();
	wxString strW = date1 + strM + strU + strP;
	SetStatusText(strW);

}


MainWnd::~MainWnd()
{
	if(m_pThreadReadDataSync != NULL){ m_pThreadReadDataSync->Delete(); }
#ifdef JMU_ALIGNGUI
		PLALI_Close(m_Align);
#endif
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
    _txtZMin = new wxTextCtrl( _bgPanel, IDT_zMin, wxString::Format(wxT("%d"),_zMin));
	_txtZMax = new wxTextCtrl( _bgPanel, IDT_zMax, wxString::Format(wxT("%d"),_zMax));
	_txtMinMaxInit = true; _txtZMin->SetModified(true); _txtZMax->SetModified(true);
	wxStaticText* txtZlabel = new wxStaticText(_bgPanel, wxID_ANY, wxT("   Z  [mm]  "));

	wxBoxSizer *sizerZscale = new wxBoxSizer(wxHORIZONTAL);
		//sizerZscale->Add(_buttAcqAll, flagsExpand);
	    sizerZscale->Add(_txtZMin, flagsExpand);
		//sizerZscale->AddStretchSpacer();
		sizerZscale->Add(txtZlabel);
	    sizerZscale->Add(_txtZMax, flagsExpand);

	// amplitude min and max controls
	_txtAmpMin = new wxTextCtrl( _bgPanel, IDT_ampMin, wxString::Format(wxT("%d"), _ampMin) ); 
	_txtAmpMax = new wxTextCtrl( _bgPanel, IDT_ampMax, wxString::Format(wxT("%d"), _ampMax) );
	_txtAmpInit = true; _txtAmpMin->SetModified(true); _txtAmpMax->SetModified(true);
	wxStaticText* txtAmpLabel = new wxStaticText(_bgPanel, wxID_ANY, wxT(" Amp. [a.u.]"));
	
	wxBoxSizer *sizerAmpScale = new wxBoxSizer(wxHORIZONTAL);
	    sizerAmpScale->Add(_txtAmpMin, flagsExpand);
		//sizerAmpScale->AddStretchSpacer();
		sizerAmpScale->Add(txtAmpLabel);
	    sizerAmpScale->Add(_txtAmpMax, flagsExpand);

	// segmentation min and max controls
	_txtSegMin = new wxTextCtrl( _bgPanel, IDT_segMin, wxString::Format(wxT("%d"), _segmMin) ); 
	_txtSegMax = new wxTextCtrl( _bgPanel, IDT_segMax, wxString::Format(wxT("%d"), _segmMax) );
	_txtSegInit = true; _txtSegMin->SetModified(true); _txtSegMax->SetModified(true);
	wxStaticText* txtSegmLabel = new wxStaticText(_bgPanel, wxID_ANY, wxT("Segm. [a.u.]"));
	
	wxBoxSizer *sizerSegScale = new wxBoxSizer(wxHORIZONTAL);
	    sizerSegScale->Add(_txtSegMin, flagsExpand);
		//sizerSegScale->AddStretchSpacer();
		sizerSegScale->Add(txtSegmLabel);
	    sizerSegScale->Add(_txtSegMax, flagsExpand);

	// group scale textboxes
	wxFlexGridSizer *sizerZAmpScales = new wxFlexGridSizer(2,1,0,0);
		sizerZAmpScales->Add( sizerZscale, flagsExpand);
		sizerZAmpScales->Add( sizerAmpScale, flagsExpand);
		sizerZAmpScales->Add( sizerSegScale, flagsExpand);
		
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
	wxSize sizeSli = wxSize(75,30);
	_buttICP = new wxButton(_bgPanel, IDB_icpVtk, wxT("ICP") );
	_visICP = new wxCheckBox(_bgPanel, IDC_visICP, wxT("Hide"));
	wxString colICP[] = { wxT("Depth (Z)"), wxT("Ampl."), wxT("Segm.")};
	wxString reprVtk[] = { wxT("Pts."), wxT("Surf")};
	_colICP=  new wxComboBox(_bgPanel, IDC_colICP, wxT("Ampl."),
		   wxDefaultPosition, wxDefaultSize, 3, colICP, wxCB_READONLY);
	_repICP = new wxComboBox(_bgPanel, IDC_repICP, wxT("Pts."),
			wxDefaultPosition, wxDefaultSize, 2, reprVtk, wxCB_READONLY);
	_alpICP = new wxSlider(_bgPanel, IDC_alpICP, 10, 0, 10, 
			wxDefaultPosition, sizeSli, wxSL_HORIZONTAL | wxSL_AUTOTICKS );//| wxSL_LABELS );
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
	    sizerICPopt->Add(icpIterLabel);
	    sizerICPopt->Add(_txtICPiter);
		sizerICPopt->Add(_icpTrlCM);
		sizerICPopt->Add(_visICP);
		sizerICPopt->Add(_colICP);
		sizerICPopt->Add(_repICP);
		sizerICPopt->Add(_alpICP);
	wxBoxSizer *sizerICP = new wxBoxSizer(wxHORIZONTAL); // create sizer ICP params
		sizerICP->Add(_buttICP, flagsExpand);
		sizerICP->Add(_icpIdxSrc);
		sizerICP->Add(_icpSrc , flagsExpand);
		sizerICP->Add(_icpIdxTgt );
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
	wxStaticText* kdThrLabel = new wxStaticText(_bgPanel, wxID_ANY, wxT("Inlier dist. threshold"));
	_kdDistThr=new wxTextCtrl( _bgPanel, IDT_kdDistThr, wxString::Format(wxT("%d"), _kdThr) );
	_kdDistBlank = new wxCheckBox(_bgPanel, IDC_kdDistBlank, wxT("Hide points above threshold"));
	_kdDistSave = new wxCheckBox(_bgPanel, IDC_kdDistSave, wxT("Save ALL kdDist results"));
    
	wxBoxSizer *sizerKDopt = new wxBoxSizer(wxHORIZONTAL); // create sizer ICPoptions
	    sizerKDopt->Add(kdThrLabel);
	    sizerKDopt->Add(_kdDistThr);
		sizerKDopt->Add(_kdDistBlank);
		sizerKDopt->Add(_kdDistSave);

	wxBoxSizer *sizerKdDist = new wxBoxSizer(wxHORIZONTAL); // create sizer KdDist params
		sizerKdDist->Add(_buttKdDistVtk, flagsExpand);
		sizerKdDist->Add(_kdDistIdxSrc );
		sizerKdDist->Add(_kdDistSrc    , flagsExpand);
		sizerKdDist->Add(_kdDistIdxTgt );
		sizerKdDist->Add(_kdDistTgt    , flagsExpand);

	sizerVtk0->Add(sizerKDopt, flagsNoExpand);
	sizerVtk0->Add(sizerKdDist, flagsExpand);
#endif
#ifdef JMU_ALIGNGUI
	wxString srcAliPla[] = { wxT("1st plane +p"), wxT("2nd plane +p"), wxT("3rd plane +p"), wxT("3 planes Heb")};
	_buttAliPla = new wxButton(_bgPanel, IDB_aliPla, wxT("AliPla") );
	_AliPlaSrc = new wxComboBox(_bgPanel, IDC_aliPlaSrc, wxT("1st plane +p"),
			wxDefaultPosition, wxDefaultSize, 4, srcAliPla, wxCB_READONLY);
	_AliPlaTgt = new wxComboBox(_bgPanel, IDC_aliPlaTgt, wxT("1st plane +p"),
			wxDefaultPosition, wxDefaultSize, 4, srcAliPla, wxCB_READONLY);
	_AliPlaIdxSrc = new wxRadioBox(_bgPanel, IDC_aliPlaIdxSrc, wxT("Cam"),
			wxDefaultPosition, wxDefaultSize, NUMCAMS, strCams);
	_AliPlaIdxTgt = new wxRadioBox(_bgPanel, IDC_aliPlaIdxTgt, wxT("Cam"),
			wxDefaultPosition, wxDefaultSize, NUMCAMS, strCams);

	wxBoxSizer *sizerAliPla = new wxBoxSizer(wxHORIZONTAL); // create sizer AliPla params
		sizerAliPla->Add(_buttAliPla, flagsExpand);
		sizerAliPla->Add(_AliPlaIdxSrc );
		sizerAliPla->Add(_AliPlaSrc    , flagsExpand);
		sizerAliPla->Add(_AliPlaIdxTgt );
		sizerAliPla->Add(_AliPlaTgt    , flagsExpand);

	sizerVtk0->Add(sizerAliPla, flagsExpand);
#endif // JMU_ALIGNGUI
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
		sizerPanel->Add(sizerVtk0, wxGBPosition(8,0), wxGBSpan(3,4));
		sizerPanel->Add(sizerCk, wxGBPosition(11,0), wxGBSpan(1,4));
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

	wxString colors[] = { wxT("Depth (Z)"), wxT("Ampl."), wxT("Segm."), wxT("red"),
        wxT("green"), wxT("blue"), wxT("white"), wxT("black") };

	wxString reprVtk[] = { wxT("Pts."), wxT("Surf") };
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
		wxSize sizeSli = wxSize(75,30);
		//llabT.sprintf(wxT("Cam %i"), i); // ... change cam nickname ...
		wxCheckBox *chkBox = new wxCheckBox(_bgPanel, IDC_visVtk, labT);	
		chkBox->SetValue(true);
		_sizerCamVisCol->Add(chkBox, wxGBPosition(i,0));
		_visVtk.push_back(chkBox);	// add visibility checkbox to container
		wxComboBox* colBox = new wxComboBox(_bgPanel, IDC_colVtk, wxT("Ampl."),
			wxDefaultPosition, wxDefaultSize, 8, colors, wxCB_READONLY);
		_sizerCamVisCol->Add(colBox, wxGBPosition(i,1));
		_colVtk.push_back(colBox);
		wxComboBox* repBox = new wxComboBox(_bgPanel, IDC_repVtk, wxT("Pts."),
			wxDefaultPosition, wxDefaultSize, 2, reprVtk, wxCB_READONLY);
		_sizerCamVisCol->Add(repBox, wxGBPosition(i,2));
		_repVtk.push_back(repBox);
		wxSlider* alpSli = new wxSlider(_bgPanel, IDC_alpVtk, 10, 0, 10, 
			wxDefaultPosition, sizeSli, wxSL_HORIZONTAL | wxSL_AUTOTICKS );//| wxSL_LABELS );
		_sizerCamVisCol->Add(alpSli, wxGBPosition(i,3));
		_alpVtk.push_back(alpSli);
		// labBGT.sprintf(wxT("BGcam %i"), i); // ... change cam BG nickname ...
		wxCheckBox *chkBGBox = new wxCheckBox(_bgPanel, IDC_visBGVtk, labBGT);	
		chkBGBox->SetValue(true);
		_sizerCamVisCol->Add(chkBGBox, wxGBPosition(i,4));
		_visBGVtk.push_back(chkBGBox);	// add visibility checkbox to container
		wxComboBox* colBGBox = new wxComboBox(_bgPanel, IDC_colBGVtk, wxT("Depth (Z)"),
			wxDefaultPosition, wxDefaultSize, 8, colors, wxCB_READONLY);
		_sizerCamVisCol->Add(colBGBox, wxGBPosition(i,5));
		_colBGVtk.push_back(colBGBox);
		wxComboBox* repBGBox = new wxComboBox(_bgPanel, IDC_repBGVtk, wxT("Surf"),
			wxDefaultPosition, wxDefaultSize, 2, reprVtk, wxCB_READONLY);
		_sizerCamVisCol->Add(repBGBox, wxGBPosition(i,6));
		_repBGVtk.push_back(repBGBox);
		wxSlider* alpBGSli = new wxSlider(_bgPanel, IDC_alpBGVtk, 10, 0, 10, //!< VTK 542 is SLOW for transparent data
			wxDefaultPosition, sizeSli, wxSL_HORIZONTAL | wxSL_AUTOTICKS );//| wxSL_LABELS );
		_sizerCamVisCol->Add(alpBGSli, wxGBPosition(i,7));
		_alpBGVtk.push_back(alpBGSli);
		// labFGT.sprintf(wxT("FGcam %i"), i); // ... change cam FG nickname ...
		wxCheckBox *chkFGBox = new wxCheckBox(_bgPanel, IDC_visFGVtk, labFGT);	
		chkFGBox->SetValue(true);
		_sizerCamVisCol->Add(chkFGBox, wxGBPosition(i,8));
		_visFGVtk.push_back(chkFGBox);	// add visibility checkbox to container
		wxComboBox* colFGBox = new wxComboBox(_bgPanel, IDC_colFGVtk, wxT("Ampl."),
			wxDefaultPosition, wxDefaultSize, 8, colors, wxCB_READONLY);
		_sizerCamVisCol->Add(colFGBox, wxGBPosition(i,9));
		_colFGVtk.push_back(colFGBox);
		wxComboBox* repFGBox = new wxComboBox(_bgPanel, IDC_repFGVtk, wxT("Surf"),
			wxDefaultPosition, wxDefaultSize, 2, reprVtk, wxCB_READONLY);
		_sizerCamVisCol->Add(repFGBox, wxGBPosition(i,10));
		_repFGVtk.push_back(repFGBox);
		wxSlider* alpFGSli = new wxSlider(_bgPanel, IDC_alpFGVtk, 10, 0, 10, //!< VTK 542 is SLOW for transparent data
			wxDefaultPosition, sizeSli, wxSL_HORIZONTAL | wxSL_AUTOTICKS);// | wxSL_LABELS );
		_sizerCamVisCol->Add(alpFGSli, wxGBPosition(i,11));
		_alpFGVtk.push_back(alpFGSli);
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
	  #ifdef JMU_KDTREEVTK
	  #ifdef JMU_KDSAVE
	     this->KdDistPNG();
	  #endif
	  #endif
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
		/* wxString colors[] = { wxT("Depth (Z)"), wxT("Ampl."), wxT("Segm."), wxT("red"),
        wxT("green"), wxT("blue"), wxT("white"), wxT("black") };*/
		wxString strCol = (*itCtrl)->GetValue();
		if(  strCol.IsSameAs(wxT("Depth (Z)"))  )
		{
			(*itCam)->setDataMapperColorDepth(idx);
			SetZMin(_zMin); // trick to update data Mapper
		}
		if(  strCol.IsSameAs(wxT("Ampl."))  )
		{
			(*itCam)->setDataMapperColorGray(idx);
			SetAmpMin(_ampMin); // trick to update data Mapper
		}
		if(  strCol.IsSameAs(wxT("Segm."))  )
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
#ifdef JMU_USE_VTK
/* acting on colVTK radio box*/
void MainWnd::SetRepVtk(wxCommandEvent& event)
{
	SetRepVtkIdx(_repVtk, m_camFrm,0);
	_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on colVTK radio box*/
void MainWnd::SetRepBGVtk(wxCommandEvent& event)
{
	SetRepVtkIdx(_repBGVtk, m_camFrm,1);
	_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on colVTK radio box*/
void MainWnd::SetRepFGVtk(wxCommandEvent& event)
{
	SetRepVtkIdx(_repFGVtk, m_camFrm,2);
	_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on colVTK radio box*/
void MainWnd::SetRepVtkIdx(std::vector<wxComboBox*> repCmbBox, std::vector<CamFrame*> camFrm, int idx)
{
	int i = 0;
	std::vector<wxComboBox*>::iterator itRep;  // get iterator on the controls
	std::vector<CamFrame*>::iterator itCam;  // get iterator on the camera frames
	for ( itRep  =repCmbBox.begin(), itCam  =camFrm.begin();
		  itRep !=repCmbBox.end()  , itCam !=camFrm.end(); 
		  itRep++, itCam++, i++ )
	{
		if(!_vtkWin){return;};
		/* wxString reprVtk[] = { wxT("Pts."), wxT("Surf") };*/
		wxString strRep = (*itRep)->GetValue();
		if(  strRep.IsSameAs(wxT("Pts."))  )
		{
			(*itCam)->setDataMapperRepPts(idx);
		}
		if(  strRep.IsSameAs(wxT("Surf"))  )
		{
			(*itCam)->setDataMapperRepSurf(idx);
		}
	}
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on colVTK radio box*/
void MainWnd::SetAlpVtk(wxScrollEvent& event)
{
	SetAlpVtkIdx(_alpVtk, m_camFrm,0);
	_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on colVTK radio box*/
void MainWnd::SetAlpBGVtk(wxScrollEvent& event)
{
	SetAlpVtkIdx(_alpBGVtk, m_camFrm,1);
	_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on colVTK radio box*/
void MainWnd::SetAlpFGVtk(wxScrollEvent& event)
{
	SetAlpVtkIdx(_alpFGVtk, m_camFrm,2);
	_vtkWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
}
#endif // JMU_USE_VTK
#ifdef JMU_USE_VTK
/* acting on colVTK radio box*/
void MainWnd::SetAlpVtkIdx(std::vector<wxSlider*> alpSli, std::vector<CamFrame*> camFrm, int idx)
{
	int i = 0;
	std::vector<wxSlider*>::iterator itAlp;  // get iterator on the controls
	std::vector<CamFrame*>::iterator itCam;  // get iterator on the camera frames
	for ( itAlp  =alpSli.begin(), itCam  =camFrm.begin();
		  itAlp !=alpSli.end()  , itCam !=camFrm.end(); 
		  itAlp++, itCam++, i++ )
	{
		if(!_vtkWin){return;};
		/*wxSlider* alpSli = new wxSlider(_bgPanel, IDC_alpVtk, 10, 0, 10, 
			wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);// | wxSL_AUTOTICKS | wxSL_LABELS );*/
		int alp = (*itAlp)->GetValue();
		double alpha = (double) alp / 10; // values mapped in .1 increments from 0.0 to 1.0
		(*itCam)->setDataMapperOpacity(idx, alpha);
	}
}
#endif // JMU_USE_VTK
/* remove CamFrame object from list if window is closed */
void MainWnd::PopCam(int vtkSub)
{
#ifdef JMU_USE_VTK
	int i = 0;
	std::vector<wxComboBox*>::iterator itCol;    // get iterator on the controls
	std::vector<wxCheckBox*>::iterator itVis;    // get iterator on the controls
	std::vector<wxComboBox*>::iterator itRep;    // get iterator on the controls
	std::vector<wxSlider*>::iterator   itAlp;    // get iterator on the controls
	std::vector<wxComboBox*>::iterator itBGcol;  // get iterator on the controls
	std::vector<wxCheckBox*>::iterator itBGvis;  // get iterator on the controls
	std::vector<wxComboBox*>::iterator itBGrep;  // get iterator on the controls
	std::vector<wxSlider*>::iterator   itBGalp;  // get iterator on the controls
	std::vector<wxComboBox*>::iterator itFGcol;  // get iterator on the controls
	std::vector<wxCheckBox*>::iterator itFGvis;  // get iterator on the controls
	std::vector<wxComboBox*>::iterator itFGrep;  // get iterator on the controls
	std::vector<wxSlider*>::iterator   itFGalp;  // get iterator on the controls
	//std::vector<wxComboBox*>::iterator itCtrl7;  // get iterator on the controls
	//std::vector<wxCheckBox*>::iterator itCtrl8;  // get iterator on the controls
	std::vector<CamFrame*>::iterator itCam;  // get iterator on the camera frames
	for ( itCol   =_colVtk.begin()  , itVis   =_visVtk.begin()  , itRep   =_repVtk.begin()  , itAlp   =_alpVtk.begin()  ,
		  itBGcol =_colBGVtk.begin(), itBGvis =_visBGVtk.begin(), itBGrep =_repBGVtk.begin(), itBGalp =_alpBGVtk.begin(),
		  itFGcol =_colFGVtk.begin(), itFGvis =_visFGVtk.begin(), itFGrep =_repFGVtk.begin(), itFGalp =_alpFGVtk.begin(), 
		  //itCtrl7 =_colSegmVtk.begin(), itCtrl8  =_visSegmVtk.begin(), 
		  itCam  =m_camFrm.begin();
		  itCol   !=_colVtk.end()   , itVis   !=_visVtk.end()  , itRep   !=_repVtk.end()  , itAlp   !=_alpVtk.end(),
	      itBGcol !=_colBGVtk.end() , itBGvis !=_visBGVtk.end(), itBGrep !=_repBGVtk.end(), itBGalp !=_alpBGVtk.end(),
		  itFGcol !=_colFGVtk.end() , itFGvis !=_visFGVtk.end(), itFGrep !=_repFGVtk.end(), itFGalp !=_alpFGVtk.end(),
		  //itCtrl7 !=_colSegmVtk.end() , itCtrl8 !=_visSegmVtk.end(),
		  itCam !=m_camFrm.end(); 
		  itCol++  , itVis++  , itRep++  , itAlp++  ,
		  itBGcol++, itBGvis++, itBGrep++, itBGalp++,
		  itFGcol++, itFGvis++, itFGrep++, itFGalp++,
		  /*itCtrl7++, itCtrl8++,*/ itCam++, i++ )
	{
		if( (*itCam)->GetVtkSub() == vtkSub)
		{
			m_camFrm.erase(itCam);
			(*itCol  )->Disable(); _colVtk.erase(itCol); 
			(*itVis  )->Disable(); _visVtk.erase(itVis); 
			(*itRep  )->Disable(); _repVtk.erase(itRep); 
			(*itAlp  )->Disable(); _alpVtk.erase(itAlp); 
			(*itBGcol)->Disable(); _colBGVtk.erase(itBGcol); 
			(*itBGvis)->Disable(); _visBGVtk.erase(itBGvis);
			(*itBGrep)->Disable(); _repBGVtk.erase(itBGrep); 
			(*itBGalp)->Disable(); _alpBGVtk.erase(itBGalp);
			(*itFGcol)->Disable(); _colFGVtk.erase(itFGcol);
			(*itFGvis)->Disable(); _visFGVtk.erase(itFGvis);
			(*itFGrep)->Disable(); _repFGVtk.erase(itFGrep); 
			(*itFGalp)->Disable(); _alpFGVtk.erase(itFGalp);
			//(*itCtrl7)->Disable(); _colSegmVtk.erase(itCtrl8);
			//(*itCtrl8)->Disable(); _visSegmVtk.erase(itCtrl8);
		#ifdef JMU_ICPVTK
			_icpIdxSrc->Enable(vtkSub, false);
			_icpIdxTgt->Enable(vtkSub, false);
		#endif
		#ifdef JMU_KDTREEVTK
			_kdDistIdxSrc->Enable(vtkSub, false);
			_kdDistIdxTgt->Enable(vtkSub, false);
		#endif
		#ifdef JMU_ALIGNGUI
			_AliPlaIdxSrc->Enable(vtkSub, false);
			_AliPlaIdxTgt->Enable(vtkSub, false);
		#endif
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
	/* wxString colICP[] = { wxT("Depth (Z)"), wxT("Ampl."), wxT("Segm.") };*/
		wxString strCol = _colICP->GetValue();
		if(  strCol.IsSameAs(wxT("Depth (Z)"))  )
		{
			_vtkWin->setICPColorDepth();
			SetZMin(_zMin); // trick to update data Mapper
		}
		if(  strCol.IsSameAs(wxT("Amp."))  )
		{
			_vtkWin->setICPColorGray();
			SetAmpMin(_ampMin); // trick to update data Mapper
		}
		if(  strCol.IsSameAs(wxT("Segm."))  )
		{
			_vtkWin->setICPColorSegm();
			SetSegMin(_segmMin); // trick to update data Mapper
		}
	_vtkWin->Render();
}
#endif // JMU_ICPVTK
#ifdef JMU_ICPVTK
void MainWnd::SetRepICP(wxCommandEvent& event)
{
	/* wxString reprVtk[] = { wxT("Pts."), wxT("Surf") };*/
		wxString strRep = _repICP->GetValue();
		if(  strRep.IsSameAs(wxT("Pts."))  )
		{
			_vtkWin->setICPActRepPts();
		}
		if(  strRep.IsSameAs(wxT("Surf"))  )
		{
			_vtkWin->setICPActRepSurf();
		}
	_vtkWin->Render();
}
#endif // JMU_ICPVTK
#ifdef JMU_USE_VTK
/* acting on colVTK radio box*/
void MainWnd::SetAlpICP(wxScrollEvent& event)
{
	/*wxSlider* alpSli = new wxSlider(_bgPanel, IDC_alpVtk, 10, 0, 10, 
			wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);// | wxSL_AUTOTICKS | wxSL_LABELS );*/
	int alp = _alpICP->GetValue();
	double alpha = (double) alp / 10; // values mapped in .1 increments from 0.0 to 1.0
	_vtkWin->setICPActOpacity(alpha);
	_vtkWin->Render();
}
#endif // JMU_USE_VTK
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
 */
void MainWnd::OnKdDist(wxCommandEvent& event)
{
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

	double res[4]; res[0] = -1; res[1] = -1; res[2] = -1; res[3] = -1;
	double thr = _kdThr;
	int inliers[2]; inliers[0]=0; inliers[1]=0;
	double eps = _vtkWin->kdDist(this->GetCamFrms(), idxSrc, srcField, idxTgt, tgtField, res, thr, inliers, _kdDistBlank->IsChecked());
	wxString strDist;
	strDist.Printf(wxT("kdDist with thr:%g returned: tgt:%05u - src:%05u - avg=%g - std=%g - eps=%g   - RMS=%g"), thr, inliers[0], inliers[1], res[1], res[2], res[0], res[3]);
	SetStatusText(strDist);
}
#endif //JMU_KDTREEVTK
#ifdef JMU_KDTREEVTK
/** acting to save kdDist results \n
 */
void MainWnd::KdDistPNG()
{
  if( _kdDistSave->IsChecked() )
  {
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

	double res[4]; res[0] = -1; res[1] = -1; res[2] = -1; res[3] = -1;
	double thr = _kdThr;
	int inliers[2]; inliers[0]=0; inliers[1]=0;
	double eps = 0;

	std::vector<CamFrame*>* camFrms = this->GetCamFrms();
	if( camFrms == NULL) { return ;};
	std::vector<CamFrame*>::iterator it; 
	CamFrame* srcVtk = NULL; 
	CamFrame* tgtVtk = NULL;
	for ( it=camFrms->begin() ; it != camFrms->end(); it++)
	{
		int camSub = (*it)->GetVtkSub();
		if(camSub == idxSrc) { srcVtk = (*it); };
		if(camSub == idxTgt) { tgtVtk = (*it); };
	}
	if( srcVtk == NULL){ return ;};
	if( tgtVtk == NULL){ return ;};
	wxString curPng;

	  // // FIRST: NO THR
	eps = _vtkWin->kdDist(this->GetCamFrms(), idxSrc, srcField, idxTgt, tgtField, res, 0, inliers, true);
	// // // // ALL SETS
	//srcVtk->hideDataActVtk(0, srcField);
	//tgtVtk->hideDataActVtk(0, tgtField);
	//curPng.Printf(wxT("_tgt%02ufield%02ufrm%05u_src%02ufield%02ufrm%05u_noThr_ALL.png"), idxTgt, tgtField, tgtVtk->GetCurFrame(), idxSrc, srcField, srcVtk->GetCurFrame() ); 
	//curPng = _kdSavePrefix + curPng;
	//_vtkWin->savePNGimg(curPng);
	// // // // TGT SET ONLY
	//srcVtk->hideDataActVtk(1, srcField);
	//curPng.Printf(wxT("_tgt%02ufield%02ufrm%05u_src%02ufield%02ufrm%05u_noThr_TGT.png"), idxTgt, tgtField, tgtVtk->GetCurFrame(), idxSrc, srcField, srcVtk->GetCurFrame() ); 
	//curPng = _kdSavePrefix + curPng;
	//_vtkWin->savePNGimg(curPng);
	//srcVtk->hideDataActVtk(0, srcField);
     // // // SRC SET ONLY
	tgtVtk->hideDataActVtk(1, tgtField);
	curPng.Printf(wxT("_tgt%02ufield%02ufrm%05u_src%02ufield%02ufrm%05u_noThr_SRC.png"), idxTgt, tgtField, tgtVtk->GetCurFrame(), idxSrc, srcField, srcVtk->GetCurFrame() ); 
	curPng = _kdSavePrefix + curPng;
	_vtkWin->savePNGimg(curPng);
	tgtVtk->hideDataActVtk(0, tgtField);
	 // // SECOND:  NO BLANKING
	eps = _vtkWin->kdDist(this->GetCamFrms(), idxSrc, srcField, idxTgt, tgtField, res, thr, inliers, false);
	 // // // ALL SETS
	srcVtk->hideDataActVtk(0, srcField);
	tgtVtk->hideDataActVtk(0, tgtField);
	curPng.Printf(wxT("_tgt%02ufield%02ufrm%05u_src%02ufield%02ufrm%05u_noBlank_ALL.png"), idxTgt, tgtField, tgtVtk->GetCurFrame(), idxSrc, srcField, srcVtk->GetCurFrame() ); 
	curPng = _kdSavePrefix + curPng;
	_vtkWin->savePNGimg(curPng);
	 // // // TGT SET ONLY
	srcVtk->hideDataActVtk(1, srcField);
	curPng.Printf(wxT("_tgt%02ufield%02ufrm%05u_src%02ufield%02ufrm%05u_noBlank_TGT.png"), idxTgt, tgtField, tgtVtk->GetCurFrame(), idxSrc, srcField, srcVtk->GetCurFrame() ); 
	curPng = _kdSavePrefix + curPng;
	_vtkWin->savePNGimg(curPng);
	srcVtk->hideDataActVtk(0, srcField);
 //    // // // SRC SET ONLY
	//tgtVtk->hideDataActVtk(1, tgtField);
	//curPng.Printf(wxT("_tgt%02ufield%02ufrm%05u_src%02ufield%02ufrm%05u_noBlank_SRC.png"), idxTgt, tgtField, tgtVtk->GetCurFrame(), idxSrc, srcField, srcVtk->GetCurFrame() ); 
	//curPng = _kdSavePrefix + curPng;
	//_vtkWin->savePNGimg(curPng);
	//tgtVtk->hideDataActVtk(0, tgtField);
	 // // THIRD:  BLANKING
	eps = _vtkWin->kdDist(this->GetCamFrms(), idxSrc, srcField, idxTgt, tgtField, res, thr, inliers, true);
	 // // // ALL SETS
	srcVtk->hideDataActVtk(0, srcField);
	tgtVtk->hideDataActVtk(0, tgtField);
	curPng.Printf(wxT("_tgt%02ufield%02ufrm%05u_src%02ufield%02ufrm%05u_Blank_ALL.png"), idxTgt, tgtField, tgtVtk->GetCurFrame(), idxSrc, srcField, srcVtk->GetCurFrame() ); 
	curPng = _kdSavePrefix + curPng;
	_vtkWin->savePNGimg(curPng);
	// // // // TGT SET ONLY
	//srcVtk->hideDataActVtk(1, srcField);
	//curPng.Printf(wxT("_tgt%02ufield%02ufrm%05u_src%02ufield%02ufrm%05u_Blank_TGT.png"), idxTgt, tgtField, tgtVtk->GetCurFrame(), idxSrc, srcField, srcVtk->GetCurFrame() ); 
	//curPng = _kdSavePrefix + curPng;
	//_vtkWin->savePNGimg(curPng);
	//srcVtk->hideDataActVtk(0, srcField);
     // // // SRC SET ONLY
	tgtVtk->hideDataActVtk(1, tgtField);
	curPng.Printf(wxT("_tgt%02ufield%02ufrm%05u_src%02ufield%02ufrm%05u_Blank_SRC.png"), idxTgt, tgtField, tgtVtk->GetCurFrame(), idxSrc, srcField, srcVtk->GetCurFrame() ); 
	curPng = _kdSavePrefix + curPng;
	_vtkWin->savePNGimg(curPng);
	tgtVtk->hideDataActVtk(0, tgtField);

	
	wxString strDist;
	strDist.Printf(wxT("kdDist with thr:%g returned: tgt:%05u - src:%05u - avg=%g - std=%g - eps=%g"), thr, inliers[0], inliers[1], res[1], res[2], res[0]);
	SetStatusText(strDist);
  }
}
#endif //JMU_KDTREEVTK
#ifdef JMU_KDTREEVTK
void MainWnd::TextKdDistThr(wxCommandEvent& )
{
	double val = 0;
	if( !_kdDistThr){return ;};
	wxString strVal = _kdDistThr->GetValue();
	if( strVal.ToDouble(& val) ) /* read value as double*/
	{
		_kdThr = val;
	}
	else
	{
		_kdDistThr->DiscardEdits();
		_kdDistThr->GetValue().Printf(wxT("%d"), _kdThr);
	}
}
#endif // JMU_KDTREEVTK
#ifdef JMU_KDTREEVTK
//! Open record interface for VTK data
void MainWnd::OnKdDistSave(wxCommandEvent& WXUNUSED(event))
{

 // m_settingsPane->SetText(wxT("Record VTK attempt..."));
 // wxString strR;

  if( _kdDistSave->IsChecked() )
  {
    wxTextEntryDialog* txtDlg = new wxTextEntryDialog(this, wxT("Choose a prefix for PNG files"), wxT("Please enter prefix"), wxT("toto"));
	if( (txtDlg->ShowModal()==wxID_OK) )
	{
		// save str to some string
		_kdSavePrefix = txtDlg->GetValue();
		//m_settingsPane->SetText(_vtkRecPrefix);
	}
	else
	{
		// set empty string to avoid saving
		//m_settingsPane->SetText(wxT("Not recording VTK ..."));
	}
  }
  else
  {
	//m_settingsPane->SetText(wxT("Stopped recording VTK."));
  } // 'record' checkbox is not checked

}
#endif // JMU_KDTREEVTK

#ifdef JMU_ALIGNGUI
/** acting on "kdDist" button \n
 * - bug: for now only first and last cam are used \n
 * - todo: make dataset choice configurable
 */
void MainWnd::OnAlignPlans(wxCommandEvent& event)
{
	int idxTgt = _AliPlaIdxTgt->GetSelection();
	int tgtField = 0;
	wxString strTgt = _AliPlaTgt->GetValue();
	if( strTgt.IsSameAs(wxT("1st plane +p")) ) {tgtField = 0;};
	if( strTgt.IsSameAs(wxT("2nd plane +p")) ) {tgtField = 1;};
	if( strTgt.IsSameAs(wxT("3rd plane +p")) ) {tgtField = 2;};
	if( strTgt.IsSameAs(wxT("3 planes Heb")) ) {tgtField = 3;};

	int idxSrc = _AliPlaIdxSrc->GetSelection();
	wxString strSrc = _AliPlaSrc->GetValue();
	int srcField = 0;
	if( strSrc.IsSameAs(wxT("1st plane +p")) ) { srcField = 0;};
	if( strSrc.IsSameAs(wxT("2nd plane +p")) ) { srcField = 1;};
	if( strSrc.IsSameAs(wxT("3rd plane +p")) ) { srcField = 2;};
	if( strSrc.IsSameAs(wxT("3 planes Heb")) ) { srcField = 3;};

	wxString strAli;
	strAli.Printf(wxT("AligPlans asked for params: TgtCam %i, field %i  -  SrcCam %i, field %i"),
		                 idxTgt, tgtField, idxSrc, srcField);
	SetStatusText(strAli);

	std::vector<CamFrame*>::iterator it; 
	CamFrame* srcAli = NULL; 
	CamFrame* tgtAli = NULL;
	for ( it=m_camFrm.begin() ; it != m_camFrm.end(); it++)
	{
		int camSub = (*it)->GetVtkSub();
		if(camSub == idxSrc) { srcAli = (*it); };
		if(camSub == idxTgt) { tgtAli = (*it); };
	}
	if( srcAli == NULL){ return ; };
	if( tgtAli == NULL){ return ; };

	if((tgtField==3) || (srcField==3))
	{
		strAli.Printf(wxT("Attempting to align with 3 planes: TgtCam %i:  -  SrcCam %i: "),
		                 idxTgt, idxSrc);
		SetStatusText(strAli);
		RSCPLAN plaRsc0 = tgtAli->GetRansacPlane(0);
		RSCPLAN plaRsc1 = tgtAli->GetRansacPlane(1);
		RSCPLAN plaRsc2 = tgtAli->GetRansacPlane(2);
		RSCPLAN plaRsc3 = srcAli->GetRansacPlane(0);
		RSCPLAN plaRsc4 = srcAli->GetRansacPlane(1);
		RSCPLAN plaRsc5 = srcAli->GetRansacPlane(2);
		JMUPLAN3D plansAli[6];
		for(int k=0; k<4; k++)
		{
			plansAli[0].n[k] = plaRsc0.nVec[k];
			plansAli[1].n[k] = plaRsc1.nVec[k];
			plansAli[2].n[k] = plaRsc2.nVec[k];
			plansAli[3].n[k] = plaRsc3.nVec[k];
			plansAli[4].n[k] = plaRsc4.nVec[k];
			plansAli[5].n[k] = plaRsc5.nVec[k];
		}
		double mat[16]; for(int k=0; k<16; k++) { mat[k]=0; };
		int res = PLALI_alignNplans(m_Align, mat, 3, &(plansAli[0]), &(plansAli[3]) );

		wxString savName; savName.Printf(wxT("Heb3Plans_Tgt%02i_Src%02i.xml"),
		                 idxTgt, idxSrc);
		wxString comments;
		comments.Printf(wxT("Hebert 3 planes alignment method"));
		tgtAli->WriteCamTrfMat4(savName, mat, comments);
		strAli.Printf(wxT("Wrote Heb3planes file: TgtCam %02i -  SrcCam %02i"),
		                 idxTgt, idxSrc);
		SetStatusText(strAli);
		
		return;  // return to avoid errors
	}

	int nptsTgt = tgtAli->GetAlignPtCnt();
	int nptsSrc = srcAli->GetAlignPtCnt();

	strAli.Printf(wxT("Alignment points defined: TgtCam %i: %02i pts  -  SrcCam %i: %02i pts"),
		                 idxTgt, nptsTgt, idxSrc, nptsSrc);
	SetStatusText(strAli);

	if(nptsTgt!=nptsSrc)
	{
		strAli.Printf(wxT("Align. points COUNT MISMATCH: TgtCam %i: %02i pts  -  SrcCam %i: %02i pts"),
		                 idxTgt, nptsTgt, idxSrc, nptsSrc);
		SetStatusText(strAli);
		return;  // return to avoid errors
	}
	if(nptsTgt<3)
	{
		strAli.Printf(wxT("Align. points MISSING (should be at least 3): TgtCam %i: %02i pts  -  SrcCam %i: %02i pts"),
		                 idxTgt, nptsTgt, idxSrc, nptsSrc);
		SetStatusText(strAli);
		return; // return to avoid errors
	}

	int npts = nptsTgt;
	int r, c; short x,y; unsigned short z;
	double *xyz0, *xyz1;
	xyz0 = (double*) malloc(npts*3*sizeof(double)); memset(xyz0, 0x0, npts*3*sizeof(double));
	xyz1 = (double*) malloc(npts*3*sizeof(double)); memset(xyz1, 0x0, npts*3*sizeof(double));
	for(int k=0; k<npts; k++)
	{
		tgtAli->GetAlignPoint(k, r, c, x, y, z);
		xyz0[k*3+0] = (double) x;
		xyz0[k*3+1] = (double) y;
		xyz0[k*3+2] = (double) z;
		srcAli->GetAlignPoint(k, r, c, x, y, z);
		xyz1[k*3+0] = (double) x;
		xyz1[k*3+1] = (double) y;
		xyz1[k*3+2] = (double) z;
	}
	RSCPLAN plaRsc0 = tgtAli->GetRansacPlane(tgtField);
	RSCPLAN plaRsc1 = srcAli->GetRansacPlane(srcField);
	JMUPLAN3D plansAli[2];
	for(int k=0; k<4; k++)
	{
		plansAli[0].n[k] = plaRsc0.nVec[k];
		plansAli[1].n[k] = plaRsc1.nVec[k];
	}
	double mat[16]; for(int k=0; k<16; k++) { mat[k]=0; };

	int res = PLALI_align1plan2dNpoints(m_Align, mat, &(plansAli[0]), &(plansAli[1]), npts, xyz0, xyz1);
	free(xyz0); free(xyz1); // DO NOT forget to free

	wxString savName; savName.Printf(wxT("MasterPlane_Tgt%02i_pla%i_Src%02i_pla%i.xml"),
		                 idxTgt, tgtField, idxSrc, srcField);
	wxString comments;
	comments.Printf(wxT("Masterplane alignment method"));
	tgtAli->WriteCamTrfMat4(savName, mat, comments);

	strAli.Printf(wxT("Wrote masterplane file: TgtCam %i: %02i pts  -  SrcCam %i: %02i pts"),
		                 idxTgt, nptsTgt, idxSrc, nptsSrc);
	SetStatusText(strAli);


	/*double res[3]; res[0] = -1; res[1] = -1; res[2] = -1;
	double eps = _vtkWin->kdDist(this->GetCamFrms(), idxSrc, srcField, idxTgt, tgtField, res);*/
}
#endif //JMU_ALIGNGUI
