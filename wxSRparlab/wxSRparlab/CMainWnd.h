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

class CamFrame;		// camera frame
class CViewSrVtk;	// vtk window

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

	//! init
	void AddChildren();

	//! Quit(..) method
    void OnQuit(wxCommandEvent& event);
	//! About(..) method
    void OnAbout(wxCommandEvent& event);

	void SetZMin(double val);
	void SetZMax(double val);
	void TextChangedZMin(wxCommandEvent &);
	void TextChangedZMax(wxCommandEvent &);
	void AcqAll(wxCommandEvent& event);
	void SetVisVtk(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()

private:
	wxTextCtrl *_txtZMin;
	wxTextCtrl *_txtZMax;
	double	_zMin;
	double	_zMax;
	bool _txtMinMaxInit;
	int			_numCams;		//!< count of cameras
	std::list<CamFrame*> m_camFrm;		//!< list of camFrames
	CViewSrVtk	*_vtkWin ; //!< pointer to vtk window
	wxButton* _buttAcqAll; //!< Acquire image camera
	std::list<wxCheckBox*> _visVtk;		//!< list of visibility checkboxes
	//wxCheckBox* _visVtk0;		//!< visibility checkbox for cam0
	//wxCheckBox* _visVtk1;		//!< visibility checkbox for cam0
	//wxCheckBox* _visVtk2;		//!< visibility checkbox for cam0
	//wxCheckBox* _visVtk3;		//!< visibility checkbox for cam0
};

//! enum used by main wnd
enum MainWndEnum
{
    ID_Quit = 1,
    ID_About,
	IDT_zMin,
	IDT_zMax,
	IDB_AcqAll,
	IDC_visVtk
	/*IDC_visVtk0,
	IDC_visVtk1,
	IDC_visVtk2,
	IDC_visVtk3*/
};
