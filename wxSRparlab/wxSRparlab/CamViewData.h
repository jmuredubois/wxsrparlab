/*
 * CamViewData.h
 * Header of the camera settings panel for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.11.08
 */

#pragma once
#include "wxSRparlab.h" //!< top-level header file
#include "wxSrApp.h"	//!< application header file
#include "CMainWnd.h"	//!< main window header file
#include "CamFrame.h"	//!< cam  window header file



/**
 * Camera settings panel class \n
 * This class: \n
 * - displays GUI elements allowing to modify camera settings \n
 */
class CamViewData: public wxPanel //!< Camera frame class
{
public:
	//! constructor
    CamViewData( wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size);
	//! destructor
	~CamViewData();
	//! init settings
	int InitViewRange();

	/* Opening and closing */
	void CloseView(wxCommandEvent& WXUNUSED(event)); //!< Closes the display
	void StopView(wxCommandEvent& WXUNUSED(event));  //!< Disables the display
	void Draw( wxDC& dc );
	

private:

	/* Buttons */
	wxButton* m_buttonCloseView; //!< Close camera
	wxButton* m_buttonStopView;  //!< Freeze display

	/* Buttons */
	wxRadioBox* m_radioboxDtype;	//!< SR data type

	/* Text zones */
	wxStaticText* m_textMin;
	wxStaticText* m_textMax;
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
	unsigned char *m_pRGB;	// RGB buffer
	unsigned char *m_pAlpha;	// alpha buffer
	int m_nDataWidth;
	int m_nDataHeight;

// protected data
protected:
	DECLARE_EVENT_TABLE()
};


//! enum used by the CamFrame class
enum CamViewDataEnum
{
	IDB_CloseViewRange   = 256 ,
	IDB_FreezeViewRange  = 257 ,
	IDS_TextMinDisp = 258,
	IDS_TextMaxDisp = 259,
	IDP_DrawPanel = 260,
	ID_ThisIsAStop2 = 512
};