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
	EVT_TEXT(IDT_ampMin, MainWnd::TextChangedAmpMin)
	EVT_TEXT(IDT_ampMax, MainWnd::TextChangedAmpMax)
	EVT_BUTTON(IDB_AcqAll, MainWnd::AcqAll)
	EVT_CHECKBOX(IDC_visVtk, MainWnd::SetVisVtk)
	EVT_RADIOBOX(IDC_colVtk, MainWnd::SetColVtk)
	EVT_CHECKBOX(IDC_ParaProj, MainWnd::OnParaProj)
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

	_txtAmpMin = NULL;
	_txtAmpMax = NULL;
	_txtAmpInit = false;
	_ampMin = 0.000;
	_ampMax = 2000.0;

	_vtkWin = NULL;

	wxString date1 = now.Format();
    //SetStatusText( _T("Welcome to wxSRparlab!") );
	wxString strW = date1 + strM + strU + strP;
	SetStatusText(strW);

	_buttAcqAll = NULL;
	_ckParaProj = NULL;
}


MainWnd::~MainWnd()
{
#ifdef JMU_USE_VTK
	if(_vtkWin){ delete(_vtkWin); _vtkWin =NULL; };
#endif
	_visVtk.clear();
	_colVtk.clear();
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

	wxFlexGridSizer *sizerZAmpScales = new wxFlexGridSizer(2,1,0,0);
		sizerZAmpScales->Add( sizerZscale, flagsExpand);
		sizerZAmpScales->Add( sizerAmpScale, flagsExpand);
		

	_buttAcqAll = new wxButton(_bgPanel, IDB_AcqAll, wxT("Acq. All") );
#ifdef JMU_USE_VTK
	_ckParaProj = new wxCheckBox(_bgPanel, IDC_ParaProj, wxT("Parallel projection"));
#endif
	wxFlexGridSizer *sizerAcqScales = new wxFlexGridSizer(1,2,0,0);
		sizerAcqScales->Add(_buttAcqAll, flagsExpand);
		sizerAcqScales->Add( sizerZAmpScales, flagsExpand);

	_sizerCamVisCol = new wxGridBagSizer();

	wxGridBagSizer *sizerPanel = new wxGridBagSizer();
		sizerPanel->Add(_sizerCamVisCol, wxGBPosition(0,0), wxGBSpan(8,4));
		sizerPanel->Add(sizerAcqScales, wxGBPosition(8,0), wxGBSpan(2,4));
#ifdef JMU_USE_VTK
		sizerPanel->Add(_ckParaProj, wxGBPosition(10,0), wxGBSpan(1,4));
#endif

	wxBoxSizer *sizerFrame = new wxBoxSizer(wxVERTICAL); // create sizer for frame
		sizerFrame->Add(_bgPanel, flagsExpand);

	_bgPanel->SetSizerAndFit(sizerPanel); // fit sizer to bg panel
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

	wxString colors[] = { wxT("Z"), wxT("Amp."), wxT("R"),
        wxT("G"), wxT("B"), wxT("W"), wxT("K") };

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

#ifdef JMU_USE_VTK
		//labT.sprintf(wxT("Cam %i"), i); // ... change checkbox text ...
		wxCheckBox *chkBox = new wxCheckBox(_bgPanel, IDC_visVtk, labT);	
		chkBox->SetValue(true);
		_sizerCamVisCol->Add(chkBox, wxGBPosition(i,0));
		_visVtk.push_back(chkBox);	// add visibility checkbox to container

		wxRadioBox *colBox = new wxRadioBox(_bgPanel, IDC_colVtk, wxT("Color"), 
			wxDefaultPosition, wxDefaultSize, 7, colors, 0, wxRA_SPECIFY_COLS );
		_sizerCamVisCol->Add(colBox, wxGBPosition(i,1));
		//sizerCamVisCol->SetItemSpan(colBox, wxGBSpan(1,7));
		_colVtk.push_back(colBox);
		camFrm->SendSizeEvent();
#endif
	} // ENDOF for loop on _numCams
	this->Layout();
	//this->SendSizeEvent();
}

/* Setting display min value*/
void MainWnd::SetZMin(double val)
{
	_zMin = val;
	_txtZMin->GetValue().Printf(wxT("%d"), val);
	_txtZMin->SetModified(true);
	SetZVtk();
}
/* Setting display min value*/
void MainWnd::SetZMax(double val)
{
	_zMax = val;
	_txtZMax->GetValue().Printf(wxT("%d"), val);
	_txtZMax->SetModified(true);
	SetZVtk();
}
/* Setting vtk display value*/
void MainWnd::SetZVtk()
{
#ifdef JMU_USE_VTK
	if(_vtkWin != NULL){
		std::vector<CamFrame*>::iterator itCam;  // get iterator on the camera frames
		for ( itCam  =m_camFrm.begin();
		      itCam !=m_camFrm.end(); 
		      itCam++)
		{
			(*itCam)->GetCamVtk()->changeDepthRange((float) _zMin, (float) _zMax);
		}
		_vtkWin->Render();
	}
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

/* Setting display min value*/
void MainWnd::SetAmpMin(double val)
{
	_ampMin = val;
	_txtAmpMin->GetValue().Printf(wxT("%d"), val);
	_txtAmpMin->SetModified(true);
	SetAmpVtk();
}
/* Setting display min value*/
void MainWnd::SetAmpMax(double val)
{
	_ampMax = val;
	_txtAmpMax->GetValue().Printf(wxT("%d"), val);
	_txtAmpMax->SetModified(true);
	SetAmpVtk();
}
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
			(*itCam)->GetCamVtk()->changeAmpRange((float) _ampMin, (float) _ampMax);
		}
		_vtkWin->Render();
	}
#endif
}

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

/* acting on "AcquireAll" value button*/
void MainWnd::AcqAll(wxCommandEvent& event)
{
	int i = 0; //debug
	std::vector<CamFrame*>::iterator it;  // get iterator on the camera frames
	for ( it=m_camFrm.begin() ; it != m_camFrm.end(); it++, i++)
	{
		(*it)->Acquire(event);
	}

}

/* acting on visVTK checkbox*/
void MainWnd::SetVisVtk(wxCommandEvent& event)
{
#ifdef JMU_USE_VTK
	int i = 0;
	std::vector<wxCheckBox*>::iterator itCtrl;  // get iterator on the controls
	std::vector<CamFrame*>::iterator itCam;  // get iterator on the camera frames
	for ( itCtrl  =_visVtk.begin(), itCam  =m_camFrm.begin();
		  itCtrl !=_visVtk.end()  , itCam !=m_camFrm.end(); 
		  itCtrl++, itCam++, i++ )
	{
		if(!_vtkWin){return;};
		(*itCam)->GetCamVtk()->hideDataAct( !((*itCtrl)->IsChecked()) );
	}
	_vtkWin->Render();
#endif
}

/* acting on colVTK radio box*/
void MainWnd::SetColVtk(wxCommandEvent& event)
{
#ifdef JMU_USE_VTK
	int i = 0;
	std::vector<wxRadioBox*>::iterator itCtrl;  // get iterator on the controls
	std::vector<CamFrame*>::iterator itCam;  // get iterator on the camera frames
	for ( itCtrl  =_colVtk.begin(), itCam  =m_camFrm.begin();
		  itCtrl !=_colVtk.end()  , itCam !=m_camFrm.end(); 
		  itCtrl++, itCam++, i++ )
	{
		if(!_vtkWin){return;};
		/* wxString colors[] = { wxT("Z"), wxT("Amp."), wxT("R"),
        wxT("G"), wxT("B"), wxT("W"), wxT("K") };*/
		wxString strCol = (*itCtrl)->GetStringSelection();
		double r, g, b; r=0.0; g=0.0; b=0.0;
		if(  strCol.IsSameAs(wxT("Z"))  )
		{
			(*itCam)->GetCamVtk()->setDataMapperColorDepth();
			SetZMin(_zMin); // trick to update data Mapper
		}
		if(  strCol.IsSameAs(wxT("Amp."))  )
		{
			(*itCam)->GetCamVtk()->setDataMapperColorGray();
			SetAmpMin(_ampMin); // trick to update data Mapper
		}
		if(  strCol.IsSameAs(wxT("R")) || strCol.IsSameAs(wxT("G")) || strCol.IsSameAs(wxT("B"))
			 || strCol.IsSameAs(wxT("W")) || strCol.IsSameAs(wxT("K")))
		{
			if(  strCol.IsSameAs(wxT("R"))  ) { (*itCam)->GetCamVtk()->setDataMapperColorR();/*/r=1.0; g=0.0; b=0.0;/**/} ;
			if(  strCol.IsSameAs(wxT("G"))  ) { (*itCam)->GetCamVtk()->setDataMapperColorG();/*/r=0.0; g=1.0; b=0.0;/**/} ;
			if(  strCol.IsSameAs(wxT("B"))  ) { (*itCam)->GetCamVtk()->setDataMapperColorB();/*/r=0.0; g=0.0; b=1.0;/**/} ;
			if(  strCol.IsSameAs(wxT("W"))  ) { (*itCam)->GetCamVtk()->setDataMapperColorW();/*/r=1.0; g=1.0; b=1.0;/**/} ;
			if(  strCol.IsSameAs(wxT("K"))  ) { (*itCam)->GetCamVtk()->setDataMapperColorK();/*/r=0.0; g=0.0; b=0.0;/**/} ;
			/*/(*itCam)->GetCamVtk()->setDataActColorRGB(r,g,b);/**/
		}
	}
	_vtkWin->Render();
#endif
}

/* remove CamFrame object from list if window is closed */
void MainWnd::PopCam(int vtkSub)
{
#ifdef JMU_USE_VTK
	int i = 0;
	std::vector<wxRadioBox*>::iterator itCtrl;  // get iterator on the controls
	std::vector<wxCheckBox*>::iterator itCtrl2;  // get iterator on the controls
	std::vector<CamFrame*>::iterator itCam;  // get iterator on the camera frames
	for ( itCtrl  =_colVtk.begin(), itCtrl2  =_visVtk.begin(), itCam  =m_camFrm.begin();
		  itCtrl !=_colVtk.end()  , itCtrl2 !=_visVtk.end()  , itCam !=m_camFrm.end(); 
		  itCtrl++, itCtrl2++, itCam++, i++ )
	{
		if( (*itCam)->GetVtkSub() == vtkSub)
		{
			m_camFrm.erase(itCam);
			(*itCtrl )->Disable();
			(*itCtrl2)->Disable();
			break;
		}
	}
#endif
}

/* acting on "Para proj" checkBox*/
void MainWnd::OnParaProj(wxCommandEvent& event)
{
#ifdef JMU_USE_VTK
	if(_ckParaProj == NULL) return;
	_vtkWin->setParallelProj( _ckParaProj->IsChecked() );
	_vtkWin->Render();
#endif
}
