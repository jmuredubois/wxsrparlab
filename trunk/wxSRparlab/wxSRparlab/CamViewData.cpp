/*
 * CamViewData.cpp
 * Implementation of the camera settings panel for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.11.08
 */

#include "wxSRparlab.h" //!< top-level header file
#include "CMainWnd.h"	//!< main window header file
#include "wxSrApp.h"	//!< application header file
#include "CamFrame.h"	//!< camera frame header file
#include "CamViewData.h" //!< camera settings panel header file


// implement message map
BEGIN_EVENT_TABLE(CamViewData, wxPanel)
	EVT_BUTTON(IDB_CloseViewRange,  CamViewData::CloseView)
	EVT_SIZE( CamViewData::OnSize ) 
	EVT_PAINT( CamViewData::OnPaint)
END_EVENT_TABLE()


/**
 * Camera data panel class constructor \n
 * Each instance must have a parent wnd (usually a notebook) \n
 */
CamViewData::CamViewData(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size)
: wxPanel(parent, wxID_ANY, pos, size, wxBORDER_NONE, title)
{
	// set my canvas width/height
	m_nWidth = size.GetWidth( );
	m_nHeight = size.GetHeight( );
	m_bDrawing = false;
	m_bNewImage = true;

	m_nDataWidth = 176;
	m_nDataHeight = 144;
	int nComp = 3;
	m_pRGB = (unsigned char*) malloc(nComp* m_nDataWidth * m_nDataHeight *sizeof(unsigned char));
	memset((void*) m_pRGB, 0x77, nComp* m_nDataWidth * m_nDataHeight *sizeof(unsigned char));
	m_pAlpha = (unsigned char*) malloc( m_nDataWidth * m_nDataHeight *sizeof(unsigned char));
	memset((void*) m_pAlpha, 0x77, m_nDataWidth * m_nDataHeight *sizeof(unsigned char));

	int c = 0; int r = 0; unsigned char R, G, B;
	R = 0; G = 0; B = 0;
	for(int i = 0 ; i <(m_nDataWidth * m_nDataHeight); i++)
	{
		m_pRGB[3*i+0] = R;
		m_pRGB[3*i+1] = G;
		m_pRGB[3*i+2] = B;
		c +=1;
		if (c >= m_nDataWidth)
		{
			c = 0;
			r +=1;
		}
		if(r < (m_nDataHeight /3)) {R=c;G=0;B=0;}
		else{if(r < (m_nDataHeight *2/3)) {R=0;G=c;B=0;}
		else{R=0;G=0;B=c;} } ;
		/*m_pRGB[3*i+0] = (unsigned char) 0x00; 
		m_pRGB[3*i+1] = (unsigned char) 0x00;
		m_pRGB[3*i+2] = (unsigned char) 0xFF; */ /* Debug: blueScreen */ 
		m_pAlpha[i] = (unsigned char) 0xFF; // alpha to 1.0
	}


	bool toto; // DEBUG variable
	// convert data from raw image to wxImg 
	m_pWxImg = wxImage( m_nDataWidth, m_nDataHeight, m_pRGB, TRUE );
	m_pWxImg.SetAlpha( m_pAlpha, TRUE);
	// convert to bitmap to be used by the window to draw
	m_pBitmap = wxBitmap( m_pWxImg.Scale(m_nDataWidth, m_nDataHeight) );
}
/**
 * Camera settings panel class constructor \n
 * Each instance must have a parent wnd (usually a notebook) \n
 */
CamViewData::~CamViewData()
{
	if(m_pRGB   != NULL) { free((void*) m_pRGB  ); m_pRGB   = NULL; };
	if(m_pAlpha != NULL) { free((void*) m_pAlpha); m_pAlpha = NULL; };
}

/**
 * 
 */
int CamViewData::InitViewRange()
{
	int res = 0;

	m_buttonCloseView = new wxButton(this, IDB_CloseViewRange, wxT("Close"));
	m_buttonStopView = new wxButton(this, IDB_FreezeViewRange, wxT("Freeze"));
	m_buttonStopView->SetFocus();
	  wxBoxSizer *sizerButtons = new wxBoxSizer(wxHORIZONTAL);
	    sizerButtons->Add(m_buttonCloseView, 1, wxEXPAND);
	    sizerButtons->Add(m_buttonStopView, 1, wxEXPAND);

	//Connect(IDB_CloseViewRange, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CamViewData::CloseView));


	wxString types[] = { wxT("Range"), wxT("Amp"), wxT("z"),
        wxT("xy") };

    m_radioboxDtype = new wxRadioBox(this, wxID_ANY, wxT("DataType"),
        wxDefaultPosition, wxDefaultSize, 4, types, 4, wxRA_SPECIFY_COLS);

	m_textMin = new wxStaticText( this, IDS_TextMinDisp, wxT("0"));
	m_textMax = new wxStaticText( this, IDS_TextMaxDisp, wxT("7500"));
	m_DrawPanel = new wxPanel(this, IDP_DrawPanel, wxPoint(-1, -1), wxSize(176, 144));
	  wxBoxSizer *sizerText = new wxBoxSizer(wxHORIZONTAL);
	    sizerText->Add(m_textMin, 1, wxEXPAND);
		sizerText->Add(m_DrawPanel);
	    sizerText->Add(m_textMax, 1, wxEXPAND);

	/* sizer stuff  ...*/
    wxBoxSizer *sizerPanel = new wxBoxSizer(wxVERTICAL);

	sizerPanel->Add(sizerButtons, 1, wxEXPAND);
    sizerPanel->Add(m_radioboxDtype, 1, wxEXPAND);
	sizerPanel->Add(sizerText, 1, wxEXPAND);


    this->SetSizer(sizerPanel);

	return res;
}



//! Closes the display
void CamViewData::CloseView(wxCommandEvent& WXUNUSED(event))
{
	Close(TRUE);
};

//! Disables the display
void CamViewData::StopView(wxCommandEvent& WXUNUSED(event))
{
	//
};

/**
 * Method:	OnSize
 * Purpose:	adjust on windows resize
 * Input:	reference to size event
 * Output:	nothing
 */
void CamViewData::OnSize( wxSizeEvent& even )
{
	int nWidth = even.GetSize().GetWidth();
	int nHeight = even.GetSize().GetHeight();

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	even.Skip();		// allow automatic handling of event

}

////////////////////////////////////////////////////////////////////
// Method:	OnPaint
// Purose:	on paint event
// Input:	reference to paint event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CamViewData::OnPaint( wxPaintEvent& event )
{
	wxPaintDC dc(m_DrawPanel);
	Draw( dc );
	event.Skip();
}

////////////////////////////////////////////////////////////////////
// Method:	Draw
// Class:	CCamView
// Purose:	camera drawing
// Input:	reference to dc
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CamViewData::Draw( wxDC& dc )
{
	// check if dc available
	if( !dc.Ok( ) || m_bDrawing == true ){ return; }

		m_bDrawing = true;

		int x,y,w,h;
		dc.GetClippingBox( &x, &y, &w, &h );
		// if there is a new image to draw
		if( m_bNewImage )
		{
			dc.DrawBitmap( m_pBitmap, x, y );
			//m_bNewImage = false;
		} else
		{
			// draw inter frame ?
		}

		m_bDrawing = false;

	return;
}