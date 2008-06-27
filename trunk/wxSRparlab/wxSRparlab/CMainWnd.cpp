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

	_zMin = 3500.0;
	_zMax = 100.0;

	_vtkWin = NULL;

	wxString date1 = now.Format();
    //SetStatusText( _T("Welcome to wxSRparlab!") );
	wxString strW = date1 + strM + strU + strP;
	SetStatusText(strW);
}

/**
 * Main window close \n
 * The method: \n
 * - calls the wxWindowBase Close() method \n
 */
void MainWnd::OnQuit(wxCommandEvent& WXUNUSED(event))
{
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
    _txtZMin = new wxTextCtrl( this, IDT_zMin, wxT("3500.0"));
	_txtZMax = new wxTextCtrl( this, IDT_zMax, wxT("0.0"));
	_txtMinMaxInit = true; _txtZMin->SetModified(true); _txtZMax->SetModified(true);

	wxBoxSizer *sizerZscale = new wxBoxSizer(wxHORIZONTAL);
	    sizerZscale->Add(_txtZMin, 0, wxEXPAND);
		sizerZscale->AddStretchSpacer();
	    sizerZscale->Add(_txtZMax, 0, wxEXPAND);

	this->SetSizerAndFit(sizerZscale);
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
	if(_vtkWin != NULL){_vtkWin->changeDepthRange( (float) _zMin, (float) _zMax); };
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
		if(_vtkWin != NULL){_vtkWin->changeDepthRange( (float) _zMin, (float) _zMax); };
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

void MainWnd::SetVtkWin(CViewSrVtk *vtkWin)
{
	_vtkWin = vtkWin;
};