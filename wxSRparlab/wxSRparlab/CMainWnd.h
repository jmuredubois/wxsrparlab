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

#define JMU_ICPVTK // testing ICP from VTK
#define JMU_KDTREEVTK // testing KDTREE from VTK

class ThreadReadDataSync; // synchronized read data
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
	~MainWnd();

	//! init
	void Init();	//!< initializes many controls

	//! init
	void AddChildren();	//!< add children: camera frames

	//! Quit(..) method
    void OnQuit(wxCommandEvent& event);	//!< cleanup
	//! About(..) method
    void OnAbout(wxCommandEvent& event); //!< about box
	void AcqAll(wxCommandEvent& event);	//!< send Acquire command to all cams
	void PopCam(int vtkSub); //!< remove camFrame object from list if window is closed
	std::vector<CamFrame*>* GetCamFrms(){return &m_camFrm;}; //! Get cam frames list
#ifdef JMU_USE_VTK
	void SetZMin(double val);	//!< set min Z for vtk display
	void SetZMax(double val);	//!< set max Z for vtk display
	void SetZVtk();
	void TextChangedZMin(wxCommandEvent &);	//!< react on event changed min Z
	void TextChangedZMax(wxCommandEvent &); //!< react on event changed max Z
	void SetAmpMin(double val);	//!< set min amp for vtk display
	void SetAmpMax(double val);	//!< set max amp for vtk display
	void SetAmpVtk();
	void TextChangedAmpMin(wxCommandEvent &); //!< react on event changed min amp
	void TextChangedAmpMax(wxCommandEvent &); //!< react on event changed max amp
	void SetSegMin(double val);	//!< set min seg for vtk display
	void SetSegMax(double val);	//!< set max seg for vtk display
	void SetSegmVtk();
	void TextChangedSegMin(wxCommandEvent &); //!< react on event changed min seg
	void TextChangedSegMax(wxCommandEvent &); //!< react on event changed max seg
	void SetVisVtk(wxCommandEvent& event); //!< react on event changed visibility
	void SetColVtk(wxCommandEvent& event); //!< react on event changed color options
	void OnParaProj(wxCommandEvent& event); //! Set projection to parallel or perpective
	void OnRendTimer(wxTimerEvent& event); //! Render timer event action
	CViewSrVtk* GetVtkWin(){return _vtkWin;} ; //!< pointer to vtk window
	void OnSegmCbar(wxCommandEvent& event);  //! Display or hide segm colorbar
	void OnAmplCbar(wxCommandEvent& event);  //! Display or hide ampl colorbar
	void OnDepthCbar(wxCommandEvent& event); //! Display or hide depth colorbar
	void OnICP(wxCommandEvent& event); //! try to use VTK's ICP
	void OnKdDist(wxCommandEvent& event); //!  point set distance based on VTK kd-tree
#endif // JMU_USE_VTK


    DECLARE_EVENT_TABLE()

private:
	wxPanel *_bgPanel; //!< background panel for main wnd
	int			_numCams;		//!< count of cameras
	std::vector<CamFrame*> m_camFrm;		//!< list of camFrames
	wxButton* _buttAcqAll; //!< Acquire image camera
#ifdef JMU_USE_VTK
	wxTextCtrl *_txtZMin;		//!< control text : min Z
	wxTextCtrl *_txtZMax;		//!< control text : max Z
	double	_zMin;				//!< stored min Z
	double	_zMax;				//!< stored max Z
	bool _txtMinMaxInit;		//!< helper var: ensure Z ctrl txt are initialized
	wxTextCtrl *_txtAmpMin;		//!< control txt : min amp
	wxTextCtrl *_txtAmpMax;		//!< control txt : max amp
	double _ampMin;				//!< stored min amp
	double _ampMax;				//!< stored max amp
	bool _txtAmpInit;			//!< helper var: ensure amp ctrl txt are initialized
	wxTextCtrl *_txtSegMin;		//!< control txt : min seg
	wxTextCtrl *_txtSegMax;		//!< control txt : max seg
	double _segmMin;				//!< stored min segm
	double _segmMax;				//!< stored max segm
	bool _txtSegInit;			//!< helper var: ensure seg ctrl txt are initialized
	CViewSrVtk	*_vtkWin ; //!< pointer to vtk window
	std::vector<wxCheckBox*> _visVtk;		//!< list of visibility checkboxes
	std::vector<wxComboBox*> _colVtk;		//!< list of vtk color options
	std::vector<wxCheckBox*> _visBGVtk;		//!< list of BACKGROUND visibility checkboxes
	std::vector<wxComboBox*> _colBGVtk;		//!< list of BACKGROUND vtk color options
	wxGridBagSizer *_sizerCamVisCol;
	wxCheckBox* _ckParaProj; //! Parallel projection button
	wxTimer	_renderTimer; //! timer for rendering events
	int _rendTgt; //! target rendering time in ms
	double _rendCapms; //!rendering capacity in ms
	ThreadReadDataSync*		m_pThreadReadDataSync; // THREAD for reading data
	wxCheckBox* _ckSegmCbar;  //! Show segmentation colorbar checkbox
	wxCheckBox* _ckAmplCbar;  //! Show amplitude    colorbar checkbox
	wxCheckBox* _ckDepthCbar; //! Show depth        colorbar checkbox
#ifdef JMU_ICPVTK
	wxButton* _buttICP; //! button for ICP
	wxComboBox* _icpSrc;
	wxComboBox* _icpTgt;
	wxRadioBox* _icpIdxSrc;
	wxRadioBox* _icpIdxTgt;
#endif
#ifdef JMU_KDTREEVTK
	wxButton* _buttKdDistVtk; //! button for point set distance based on VTK kd-tree
	wxComboBox* _kdDistSrc;
	wxComboBox* _kdDistTgt;
	wxRadioBox* _kdDistIdxSrc;
	wxRadioBox* _kdDistIdxTgt;
#endif
#endif // JMU_USE_VTK
};

//! enum used by main wnd
enum MainWndEnum
{
    ID_Quit = 1,
    ID_About,
	IDT_zMin,
	IDT_zMax,
	IDT_ampMin,
	IDT_ampMax,
	IDT_segMin,
	IDT_segMax,
	IDB_AcqAll,
	IDC_visVtk,
	IDC_colVtk,
	IDE_RendTimer,
	IDC_ParaProj,
	IDC_SegmCbar,
	IDC_AmplCbar,
	IDC_DepthCbar,
	IDB_icpVtk,
	IDC_icpSrc,
	IDC_icpTgt,
	IDC_icpIdxSrc,
	IDC_icpIdxTgt,
	IDB_kdDistVtk,
	IDC_kdDistSrc,
	IDC_kdDistTgt,
	IDC_kdDistIdxSrc,
	IDC_kdDistIdxTgt
};
