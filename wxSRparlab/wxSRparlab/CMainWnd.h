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
#include "libSRPLalign.h"

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
	void SetRepVtk(wxCommandEvent& event); //!< react on event changed visibility
	void SetAlpVtk(wxScrollEvent& event); //!< react on event changed color options
	void SetVisBGVtk(wxCommandEvent& event); //!< react on event changed visibility BG
	void SetColBGVtk(wxCommandEvent& event); //!< react on event changed color options BG
	void SetRepBGVtk(wxCommandEvent& event); //!< react on event changed visibility
	void SetAlpBGVtk(wxScrollEvent& event); //!< react on event changed color options
	void SetVisFGVtk(wxCommandEvent& event); //!< react on event changed visibility FG
	void SetColFGVtk(wxCommandEvent& event); //!< react on event changed color options FG
	void SetRepFGVtk(wxCommandEvent& event); //!< react on event changed visibility
	void SetAlpFGVtk(wxScrollEvent& event); //!< react on event changed color options
	void OnParaProj(wxCommandEvent& event); //! Set projection to parallel or perpective
	void OnRendTimer(wxTimerEvent& event); //! Render timer event action
	CViewSrVtk* GetVtkWin(){return _vtkWin;} ; //!< pointer to vtk window
	void OnSegmCbar(wxCommandEvent& event);  //! Display or hide segm colorbar
	void OnAmplCbar(wxCommandEvent& event);  //! Display or hide ampl colorbar
	void OnDepthCbar(wxCommandEvent& event); //! Display or hide depth colorbar
	#ifdef JMU_ICPVTK
	void OnICP(wxCommandEvent& event); //! try to use VTK's ICP
	void SetVisICP(wxCommandEvent& event); //!< react on event changed visibility
	void SetColICP(wxCommandEvent& event); //!< react on event changed color options 
	void SetRepICP(wxCommandEvent& event); //!< react on event changed visibility
	void SetAlpICP(wxScrollEvent& event);  //!< react on event changed color options
	void TextICPiter(wxCommandEvent& ); //
	//void SetICPtrlCM(wxCommandEvent& event); //
	#endif
	#ifdef JMU_KDTREEVTK
	void OnKdDist(wxCommandEvent& event); //!  point set distance based on VTK kd-tree
	void TextKdDistThr(wxCommandEvent& ); //
	#endif
#endif // JMU_USE_VTK
#ifdef JMU_ALIGNGUI
	void OnAlignPlans(wxCommandEvent& event); //!  aligment based on RANSAC plans
#endif


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
	std::vector<wxComboBox*> _repVtk;		//!< list of vtk representations options
	std::vector<wxSlider*>   _alpVtk;		//!< slider for vtk alpha option
	std::vector<wxCheckBox*> _visBGVtk;		//!< list of BACKGROUND visibility checkboxes
	std::vector<wxComboBox*> _colBGVtk;		//!< list of BACKGROUND vtk color options
	std::vector<wxComboBox*> _repBGVtk;		//!< list of vtk representations options
	std::vector<wxSlider*>   _alpBGVtk;		//!< slider for vtk alpha option
	std::vector<wxCheckBox*> _visFGVtk;		//!< list of FOREGROUND visibility checkboxes
	std::vector<wxComboBox*> _colFGVtk;		//!< list of FOREGROUND vtk color options
	std::vector<wxComboBox*> _repFGVtk;		//!< list of vtk representations options
	std::vector<wxSlider*>   _alpFGVtk;		//!< slider for vtk alpha option
	std::vector<wxCheckBox*> _visSegmVtk;		//!< list of SEGMENTED visibility checkboxes
	std::vector<wxComboBox*> _colSegmVtk;		//!< list of SEGMENTED vtk color options
	wxGridBagSizer *_sizerCamVisCol;
	void SetVisVtkIdx(std::vector<wxCheckBox*> visChkBox, std::vector<CamFrame*> camFrm, int idx); //!< react on event changed visibility
	void SetColVtkIdx(std::vector<wxComboBox*> colCmbBox, std::vector<CamFrame*> camFrm, int idx);  //!< react on event changed colormap
	void SetRepVtkIdx(std::vector<wxComboBox*> repCmbBox, std::vector<CamFrame*> camFrm, int idx);  //!< react on event changed colormap
	void SetAlpVtkIdx(std::vector<wxSlider*>   alpSliBox, std::vector<CamFrame*> camFrm, int idx);  //!< react on event changed colormap
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
	wxCheckBox* _visICP; //! Show ICP checkbox
	wxComboBox* _colICP; //! ICP color combo box
	wxComboBox* _repICP; //! ICP repr. combo box
	wxSlider*   _alpICP; //! ICP alpha slider
	wxCheckBox* _icpTrlCM; //! ICP zero center of mass flag
	wxTextCtrl *_txtICPiter; //!< control txt : ICP iterations
	int _icpIter;
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
	wxTextCtrl* _kdDistThr; //!< control txt : dist thr
	double _kdThr;
	wxCheckBox* _kdDistBlank; //! blank points far away with kdDist
#endif
#endif // JMU_USE_VTK
#ifdef JMU_ALIGNGUI
	wxButton* _buttAliPla; //! button for point set distance based on VTK kd-tree
	wxComboBox* _AliPlaSrc;
	wxComboBox* _AliPlaTgt;
	wxRadioBox* _AliPlaIdxSrc;
	wxRadioBox* _AliPlaIdxTgt;
	SRPLALI m_Align;
#endif // JMU_ALIGNGUI
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
	IDC_kdDistIdxTgt,
	IDT_kdDistThr,
	IDC_kdDistBlank,
	IDT_icpIter,
	IDC_icpTrlCM,
	IDC_visICP,
	IDC_colICP,
	IDC_repICP,
	IDC_alpICP,
	IDC_repVtk,
	IDC_alpVtk,
	IDC_visBGVtk,
	IDC_colBGVtk,
	IDC_repBGVtk,
	IDC_alpBGVtk,
	IDC_visFGVtk,
	IDC_colFGVtk,
    IDC_repFGVtk,
	IDC_alpFGVtk,
	IDB_aliPla,
	IDC_aliPlaSrc,
	IDC_aliPlaTgt,
	IDC_aliPlaIdxSrc,
	IDC_aliPlaIdxTgt

};
