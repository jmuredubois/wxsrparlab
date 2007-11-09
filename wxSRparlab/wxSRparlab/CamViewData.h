/*
 * CamViewData.h
 * Header of the camera view panel for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.11.09
 */

#pragma once
#include "wxSRparlab.h" //!< top-level header file
#include "wxSrApp.h"	//!< application header file
#include "CMainWnd.h"	//!< main window header file
#include "CamFrame.h"	//!< cam  window header file

#define LUTLEN 256	// color LUT length
#define NCOMP 3		// number of components (RGB)

/**
 * Camera settings panel class \n
 * This class: \n
 * - displays GUI elements allowing to modify camera settings \n
 */
class CamViewData: public wxPanel //!< Camera view panel
{
public:
	//! constructor
    CamViewData( wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size);
	//! destructor
	~CamViewData();
	//! init settings
	int InitViewData();

	/* Opening and closing */
	void CloseView(wxCommandEvent& WXUNUSED(event)); //!< Closes the display
	void StopView(wxCommandEvent& WXUNUSED(event));  //!< Disables the display

	/* Drawing bitmap */
	void Draw( wxDC& dc );

	/* Mapping ushort data*/
	int MapUshort2rgb();

	/* Mapping data limits*/
	void SetDispMin(double val);
	void SetDispMax(double val);
	double GetDispMin(){return m_dDispMin;};
	double GetDispMax(){return m_dDispMax;};
	/* */
	void TextChangedDispMin(wxCommandEvent& WXUNUSED(event));/* */
	void TextChangedDispMax(wxCommandEvent& WXUNUSED(event));/* */
	
	/* Copying data to display */
	int SetUshortData( unsigned short * buf, int numPix);

private:

	/* Buttons */
	wxButton* m_buttonCloseView; //!< Close view
	wxButton* m_buttonAcquire; //!< Acquire image camera
	wxButton* m_buttonStopView;  //!< Freeze display

	/* Buttons */
	wxRadioBox* m_radioboxDtype;	//!< SR data type

	/* Text zones */
	wxTextCtrl* m_textMin;
	wxTextCtrl* m_textMax;
	bool m_bTextInit;
	wxPanel* m_DrawPanel;

	// Protected data
protected:
	wxImage		m_pWxImg;	// pointer to image  (Dev. Indep.)
	wxBitmap	m_pBitmap;	// wxBitmap to paint (Dev. Dep.)
  	bool	m_bDrawing;
	bool	m_bNewImage;
	int m_nWidth;			// window width
	int m_nHeight;			// window height

// private methods
private:
	void	OnPaint(wxPaintEvent& event);
	void	OnSize( wxSizeEvent& even );

	//
	unsigned char *m_pRGB;		// RGB buffer
	unsigned char *m_pAlpha;	// alpha buffer
	unsigned char *m_pLUT;		// color LUT
	int m_nLUTlen;				// color LUT length
	int m_nComp;				// number of components (RGB)
	int m_nDataWidth;	// data width
	int m_nDataHeight;	// data type
	int m_nDataBytes;	// data bytes per sample
	void *m_pDataArray;			// data array
	
	double m_dDispMin;	// min value
	double m_dDispMax;	// max value
	

// protected data
protected:
	DECLARE_EVENT_TABLE()
};


//! enum used by the CamFrame class
enum CamViewDataEnum
{
	IDB_CloseViewData   = 256 ,
	IDB_FreezeViewData  = 257 ,
	IDT_DispMin = 258,
	IDT_DispMax = 259,
	IDP_DrawPanel = 260,
	ID_ThisIsAStop2 = 512
};