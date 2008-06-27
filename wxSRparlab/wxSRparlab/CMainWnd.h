/*
 * CMainWnd.h
 * Header of the main window for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.10.19
 */

#pragma once
#include "wxSRparlab.h" //!< top-level header file
#include "wxSrApp.h"	//!< this class header file

class CViewSrVtk;

/**
 * Main window class \n
 * This class: \n
 * - contains the main wnd GUI \n
 */
class MainWnd: public wxFrame //!< main window class
{
public:
	//!Constructor
    MainWnd(const wxString& title, const wxPoint& pos, const wxSize& size); 

	//! init
	void Init();

	//! Quit(..) method
    void OnQuit(wxCommandEvent& event);
	//! About(..) method
    void OnAbout(wxCommandEvent& event);

	void SetZMin(double val);
	void SetZMax(double val);
	void TextChangedZMin(wxCommandEvent &);
	void TextChangedZMax(wxCommandEvent &);
	void SetVtkWin(CViewSrVtk *vtkWin);

    DECLARE_EVENT_TABLE()

private:
	wxTextCtrl *_txtZMin;
	wxTextCtrl *_txtZMax;
	double	_zMin;
	double	_zMax;
	bool _txtMinMaxInit;
	CViewSrVtk	*_vtkWin ; //!< pointer to vtk window
};

//! enum used by main wnd
enum MainWndEnum
{
    ID_Quit = 1,
    ID_About,
	IDT_zMin,
	IDT_zMax
};
