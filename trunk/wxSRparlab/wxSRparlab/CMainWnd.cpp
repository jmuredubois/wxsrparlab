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


BEGIN_EVENT_TABLE(MainWnd, wxFrame)
    EVT_MENU(ID_Quit, MainWnd::OnQuit)
    EVT_MENU(ID_About, MainWnd::OnAbout)
	EVT_TEXT(IDT_zMin, MainWnd::TextChangedZMin)
	EVT_TEXT(IDT_zMax, MainWnd::TextChangedZMax)
	EVT_BUTTON(IDB_AcqAll, MainWnd::AcqAll)
	EVT_CHECKBOX(IDC_visVtk, MainWnd::SetVisVtk)
	EVT_RADIOBOX(IDC_colVtk, MainWnd::SetColVtk)
END_EVENT_TABLE()

IMPLEMENT_APP(SrApp)


/**
 * Main window construction \n
 * - the main window is derived from wxFrame \n
 * - it has no parent \n
 *
 */
MainWnd::MainWnd(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	_numCams = NUMCAMS;
	_vtkWin = NULL;
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

	_txtZMin = NULL;
	_txtZMax = NULL;
	_txtMinMaxInit = false;

	_zMin = 100.0;
	_zMax = 3500.0;

	_vtkWin = NULL;

	wxString date1 = now.Format();
    //SetStatusText( _T("Welcome to wxSRparlab!") );
	wxString strW = date1 + strM + strU + strP;
	SetStatusText(strW);

	_buttAcqAll = NULL;
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
	//if(_txtZMin !=NULL){_txtZMin->Close(); _txtZMin = NULL;};
	//if(_txtZMin !=NULL){_txtZMax->Close(); _txtZMax = NULL;};
    Close(TRUE);
}

/**
 * About Box \n
 * The method: \n
 * - displays an info msg using wxMessageBox \n
 */
void MainWnd::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("This is a wxWindows SR application sample"),
        _T("About wxSRparlab"), wxOK | wxICON_INFORMATION, this);
}


/**
 * Main window init \n
 * The method: \n
 *
 */
void MainWnd::Init()
{
	wxPanel *bgPanel = new wxPanel(this); // a panel to contain our controls
    _txtZMin = new wxTextCtrl( bgPanel, IDT_zMin, wxT("0.0"));
	_txtZMax = new wxTextCtrl( bgPanel, IDT_zMax, wxT("3500.0"));
	_txtMinMaxInit = true; _txtZMin->SetModified(true); _txtZMax->SetModified(true);

	_buttAcqAll = new wxButton(bgPanel, IDB_AcqAll, wxT("Acq. All") );

	wxSizerFlags flagsExpand(1);
	flagsExpand.Expand();

	wxSizerFlags flagsNoExpand(1);

	wxBoxSizer *sizerZscale = new wxBoxSizer(wxHORIZONTAL);
		sizerZscale->Add(_buttAcqAll, flagsNoExpand);
	    sizerZscale->Add(_txtZMin, flagsNoExpand);
		sizerZscale->AddStretchSpacer();
	    sizerZscale->Add(_txtZMax, flagsNoExpand);

	wxGridBagSizer *sizerCamVisCol = new wxGridBagSizer();
	wxString colors[] = { wxT("Z"), wxT("Amp."), wxT("R"),
        wxT("G"), wxT("B"), wxT("W"), wxT("K") };

	wxString labT;					// descr string for cam visibility checkbox
	//! - for the max number of cameras ...
	for(int i = 0; i<_numCams; i++){
		labT.sprintf(wxT("Cam %i"), i); // ... change checkbox text ...
		wxCheckBox *chkBox = new wxCheckBox(bgPanel, IDC_visVtk, labT);	
		chkBox->SetValue(true);
		sizerCamVisCol->Add(chkBox, wxGBPosition(i,0));
		_visVtk.push_back(chkBox);	// add visibility checkbox to container

		wxRadioBox *colBox = new wxRadioBox(bgPanel, IDC_colVtk, wxT("Color"), 
			wxDefaultPosition, wxDefaultSize, 7, colors, 0, wxRA_SPECIFY_COLS );
		sizerCamVisCol->Add(colBox, wxGBPosition(i,1));
		//sizerCamVisCol->SetItemSpan(colBox, wxGBSpan(1,7));
		_colVtk.push_back(colBox);
	} // ENDOF for loop on _numCams

	wxBoxSizer *sizerPanel = new wxBoxSizer(wxVERTICAL);
		sizerPanel->Add(sizerCamVisCol, flagsExpand);
		sizerPanel->Add(sizerZscale, flagsExpand);

	bgPanel->SetSizerAndFit(sizerPanel); // fit sizer to bg panel

	wxBoxSizer *sizerFrame = new wxBoxSizer(wxVERTICAL); // create sizer for frame
		sizerFrame->Add(bgPanel, flagsExpand);
	this->SetSizerAndFit(sizerFrame);
}

/**
 * Main window init \n
 * The method: \n
 *
 */
void MainWnd::AddChildren()
{
    // temp variables to avoid creating new camFrames on top of each other
	wxPoint pos = wxPoint(50,150);	// initial position
	wxPoint incr = wxPoint(220,00);	// increment in position
	wxSize	sz = wxSize(220,400);	// size for camFrame
	wxString lab;					// title string for camFrame
	wxString labT;					// title string for camFrame

#ifdef JMU_USE_VTK
	_vtkWin = new CViewSrVtk(NULL);
#endif

	//! - for the max number of cameras ...
	for(int i = 0; i<_numCams; i++){
		lab.sprintf(wxT("Camera %i"), i); // ... change title text ...
		labT.sprintf(wxT("Cam %i"), i); // ... change title text ...
		//! ... create and show new camFrame ... \n
		CamFrame *camFrm = new CamFrame( this, lab, pos, sz );
		camFrm->Show(TRUE);
		camFrm->CreateAndSetNotebook(labT);
#ifdef JMU_USE_VTK
		camFrm->SetVtkWin(_vtkWin, i);
#endif
		m_camFrm.push_back(camFrm);
		pos += incr; //... increment position.
	} // ENDOF for loop on _numCams
}

/* Setting display min value*/
void MainWnd::SetZMin(double val)
{
	_zMin = val;
	_txtZMin->GetValue().Printf(wxT("%d"), val);
	_txtZMin->SetModified(true);
}
/* Setting display min value*/
void MainWnd::SetZMax(double val)
{
	_zMax = val;
	_txtZMax->GetValue().Printf(wxT("%d"), val);
	_txtZMax->SetModified(true);
#ifdef JMU_USE_VTK
	if(_vtkWin != NULL){_vtkWin->changeDepthRange( (float) _zMin, (float) _zMax); };
#endif
}

/* acting on chaged text value */
void MainWnd::TextChangedZMin(wxCommandEvent &)
{
	double val = 0;
	if( !_txtMinMaxInit){return ;};
	wxString strVal = _txtZMin->GetValue();
	if( strVal.ToDouble(& val) ) /* read value as double*/
	{
		SetZMin(val);
#ifdef JMU_USE_VTK
		if(_vtkWin != NULL){_vtkWin->changeDepthRange( (float) _zMin, (float) _zMax); };
#endif
	}
	else
	{
		_txtZMin->DiscardEdits();
		_txtZMin->GetValue().Printf(wxT("%d"), _zMin);
	}
}

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

/* acting on "AcquireAll" value button*/
void MainWnd::AcqAll(wxCommandEvent& event)
{
	std::vector<CamFrame*>::iterator it;  // get iterator on the camera frames
	for ( it=m_camFrm.begin() ; it != m_camFrm.end(); it++ )
	{
		(*it)->Acquire(event);
	}

}

/* acting on visVTK checkbox*/
void MainWnd::SetVisVtk(wxCommandEvent& event)
{
#ifdef JMU_USE_VTK
	int i = 0;
	std::vector<wxCheckBox*>::iterator it;  // get iterator on the camera frames
	for ( it=_visVtk.begin() ; it != _visVtk.end(); it++, i++ )
	{
		if(!_vtkWin){return;};
		_vtkWin->hideDataAct(i, !( (*it)->IsChecked() ) );
	}
	_vtkWin->Render();
#endif
}

/* acting on colVTK radio box*/
void MainWnd::SetColVtk(wxCommandEvent& event)
{
#ifdef JMU_USE_VTK
	int i = 0;
	std::vector<wxRadioBox*>::iterator it;  // get iterator on the camera frames
	for ( it=_colVtk.begin() ; it != _colVtk.end(); it++, i++ )
	{
		if(!_vtkWin){return;};
		/* wxString colors[] = { wxT("Z"), wxT("Amp."), wxT("R"),
        wxT("G"), wxT("B"), wxT("W"), wxT("K") };*/
		wxString strCol = (*it)->GetStringSelection();
		double r, g, b; r=0.0; g=0.0; b=0.0;
		if(  strCol.IsSameAs(wxT("Z"))  )
		{
			//_vtkWin->setDataActColorZ(i);
		}
		if(  strCol.IsSameAs(wxT("Amp."))  )
		{
			//_vtkWin->setDataActColorAmp(i);
		}
		if(  strCol.IsSameAs(wxT("R")) || strCol.IsSameAs(wxT("G")) || strCol.IsSameAs(wxT("B"))
			 || strCol.IsSameAs(wxT("W")) || strCol.IsSameAs(wxT("K")))
		{
			if(  strCol.IsSameAs(wxT("R"))  ) { r=1.0; g=0.0; b=0.0;} ;
			if(  strCol.IsSameAs(wxT("G"))  ) { r=0.0; g=1.0; b=0.0;} ;
			if(  strCol.IsSameAs(wxT("B"))  ) { r=0.0; g=0.0; b=1.0;} ;
			if(  strCol.IsSameAs(wxT("W"))  ) { r=1.0; g=1.0; b=1.0;} ;
			if(  strCol.IsSameAs(wxT("K"))  ) { r=0.0; g=0.0; b=0.0;} ;
			_vtkWin->setDataActColorRGB(i,r,g,b);
		}
	}
	_vtkWin->Render();
#endif
}