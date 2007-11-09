/*
 * CamViewData.cpp
 * Implementation of the camera settings panel for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.11.09
 */

#include "wxSRparlab.h" //!< top-level header file
#include "CMainWnd.h"	//!< main window header file
#include "wxSrApp.h"	//!< application header file
#include "CamFrame.h"	//!< camera frame header file
#include "CamViewData.h" //!< camera settings panel header file


// implement message map
BEGIN_EVENT_TABLE(CamViewData, wxPanel)
	EVT_BUTTON(IDB_CloseViewData,  CamViewData::CloseView)
	EVT_SIZE( CamViewData::OnSize ) 
	EVT_PAINT( CamViewData::OnPaint)
	EVT_TEXT( IDT_DispMin , CamViewData::TextChangedDispMin)
	EVT_TEXT( IDT_DispMax , CamViewData::TextChangedDispMax)
END_EVENT_TABLE()


/**
 * Camera data panel class constructor \n
 * Each instance must have a parent wnd (usually a notebook) \n
 */
CamViewData::CamViewData(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size)
: wxPanel(parent, wxID_ANY, pos, size, wxBORDER_NONE, title)
{
	// set my canvas width/height
	/*m_nWidth = size.GetWidth( );
	m_nHeight = size.GetHeight( );*/
	m_bDrawing = false;
	m_bNewImage = true;
	m_bTextInit = false;

	m_nComp = NCOMP;
	m_nLUTlen = LUTLEN;

	m_pLUT = (unsigned char*) malloc(m_nComp* m_nLUTlen *sizeof(unsigned char));
	memset((void*) m_pLUT, 0x77, m_nComp* m_nLUTlen *sizeof(unsigned char));

	int i, c; // variables used in loops

	/* Debug: gray ramp */
	c = 0; unsigned char R, G, B;
	R = 0; G = 0; B = 0;
	for(i = 0 ; i <(m_nLUTlen); i++)
	{
		m_pLUT[m_nComp*i+0] = R;
		m_pLUT[m_nComp*i+1] = G;
		m_pLUT[m_nComp*i+2] = B;
		c +=1;
		if (c >= m_nLUTlen)
		{
			c = 0;
		}
		R=c;G=c;B=c;
	}
	/* ENDOF Debug: gray ramp */
	
	m_nDataWidth = 176;
	m_nDataHeight = 144;
	
	m_pRGB = (unsigned char*) malloc(m_nComp* m_nDataWidth * m_nDataHeight *sizeof(unsigned char));
	memset((void*) m_pRGB, 0x77, m_nComp* m_nDataWidth * m_nDataHeight *sizeof(unsigned char));
	m_pAlpha = (unsigned char*) malloc( m_nDataWidth * m_nDataHeight *sizeof(unsigned char));
	memset((void*) m_pAlpha, 0x77, m_nDataWidth * m_nDataHeight *sizeof(unsigned char));

	/* Debug: 3ramps /
	int c = 0; int r = 0; unsigned char R, G, B;
	R = 0; G = 0; B = 0;
	/* ENDOF Debug: 3ramps */
	for(i = 0 ; i <(m_nDataWidth * m_nDataHeight); i++)
	{
		/* Debug: 3ramps /
		m_pRGB[m_nComp*i+0] = R;
		m_pRGB[m_nComp*i+1] = G;
		m_pRGB[m_nComp*i+2] = B;
		c +=1;
		if (c >= m_nDataWidth)
		{
			c = 0;
			r +=1;
		}
		if(r < (m_nDataHeight /3)) {R=c;G=0;B=0;}
		else{if(r < (m_nDataHeight *2/3)) {R=0;G=c;B=0;}
		else{R=0;G=0;B=c;} } ;
		/* ENDOF Debug: 3ramps */
		/* Debug: blueScreen /
		m_pRGB[3*i+0] = (unsigned char) 0x00; 
		m_pRGB[3*i+1] = (unsigned char) 0x00;
		m_pRGB[3*i+2] = (unsigned char) 0xFF;  
		/* ENDOF Debug: blueScreen */ 

		m_pAlpha[i] = (unsigned char) 0xFF; // alpha to 1.0
	}

	m_pDataArray = (void*) malloc( m_nDataWidth * m_nDataHeight *sizeof(unsigned short));
	memset((void*) m_pDataArray, 0x77, m_nDataWidth * m_nDataHeight *sizeof(unsigned short));

	unsigned short *data = (unsigned short*) m_pDataArray;

	/* Debug: Gray ramps */
	c = 0;
	for(i = 0 ; i <(m_nDataWidth * m_nDataHeight); i++)
	{
		data[i] = c;
		c +=1;
		if (c >= 25344)
		{
			c = 0;
		}
	}
	m_dDispMin = 0.0;
	m_dDispMax = 25344.0;
	int toto = MapUshort2rgb();
	/* Endof: Gray ramps */


	//// convert data from raw image to wxImg 
	//m_pWxImg = wxImage( m_nDataWidth, m_nDataHeight, m_pRGB, TRUE );
	//m_pWxImg.SetAlpha( m_pAlpha, TRUE);
	//// convert to bitmap to be used by the window to draw
	//m_pBitmap = wxBitmap( m_pWxImg.Scale(m_nDataWidth, m_nDataHeight) );
}
/**
 * Camera view panel class destructor \n
 */
CamViewData::~CamViewData()
{
	if(m_pRGB   != NULL) { free((void*) m_pRGB  ); m_pRGB   = NULL; };
	if(m_pAlpha != NULL) { free((void*) m_pAlpha); m_pAlpha = NULL; };
	if(m_pLUT   != NULL) { free((void*) m_pLUT  ); m_pLUT   = NULL; };
	if(m_pDataArray != NULL) { free((void*) m_pDataArray ); m_pDataArray = NULL; };
}

/**
 * 
 */
int CamViewData::InitViewData()
{
	int res = 0;

	m_buttonCloseView = new wxButton(this, IDB_CloseViewData, wxT("Close"));
	m_buttonAcquire = new wxButton(this, IDB_Acquire, wxT("Acquire"));
	m_buttonStopView = new wxButton(this, IDB_FreezeViewData, wxT("Freeze"));
	m_buttonStopView->SetFocus();
	  wxBoxSizer *sizerButtons = new wxBoxSizer(wxHORIZONTAL);
		sizerButtons->Add(m_buttonAcquire, 1, wxEXPAND);
	    sizerButtons->Add(m_buttonCloseView, 1, wxEXPAND);
	    sizerButtons->Add(m_buttonStopView, 1, wxEXPAND);

	//Connect(IDB_CloseViewData, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CamViewData::CloseView));


	wxString types[] = { wxT("Range"), wxT("Amp"), wxT("z"),
        wxT("xy") };

    m_radioboxDtype = new wxRadioBox(this, wxID_ANY, wxT("DataType"),
        wxDefaultPosition, wxDefaultSize, 4, types, 4, wxRA_SPECIFY_COLS);

	m_textMin = new wxTextCtrl( this, IDT_DispMin, wxT("0") );
	m_textMax = new wxTextCtrl( this, IDT_DispMax, wxT("7500"));
	m_bTextInit = true;
	m_DrawPanel = new wxPanel(this, IDP_DrawPanel, wxPoint(-1, -1), wxSize(176, 144));
	  wxBoxSizer *sizerText = new wxBoxSizer(wxHORIZONTAL);
	    sizerText->Add(m_textMin, 1, wxEXPAND);
		sizerText->Add(m_DrawPanel, 1, wxEXPAND);
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

	/*m_nWidth = nWidth;
	m_nHeight = nHeight;*/
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

//! maps to RGB
int CamViewData::MapUshort2rgb()
{
	int res = 0;
	if(m_pLUT       == NULL){return -1;};
	if(m_pRGB       == NULL){return -2;};
	if(m_pDataArray == NULL){return -3;};

	unsigned short* data = (unsigned short*) m_pDataArray;
	double invDyn = 1.0/ (m_dDispMax - m_dDispMin) * ( (double) m_nLUTlen);

	int val = 0;
	unsigned char *curPix, *curCol;
	for(int i = 0 ; i <(m_nDataWidth * m_nDataHeight); i++)
	{
		val = (int)floor(( ((double)data[i]) - m_dDispMin ) * invDyn );
		if(val <0){ val = 0;}
		if(val > (m_nLUTlen-1) ) {val = (m_nLUTlen-1) ;}


		curPix = &m_pRGB[m_nComp*i+0];
		curCol = &m_pLUT[m_nComp*val];
		memcpy( (void*) curPix, (const void*) curCol, m_nComp);
	}

	// convert data from raw image to wxImg 
	m_pWxImg = wxImage( m_nDataWidth, m_nDataHeight, m_pRGB, TRUE );
	m_pWxImg.SetAlpha( m_pAlpha, TRUE);
	// convert to bitmap to be used by the window to draw
	m_pBitmap = wxBitmap( m_pWxImg.Scale(m_nDataWidth, m_nDataHeight) );

	return res;
};


/* Copying data to display */
int CamViewData::SetUshortData( unsigned short * buf, int numPix)
{
	int res= 0;
	if(buf == NULL){return -1;};
	if(numPix*sizeof(unsigned short) > m_nDataWidth * m_nDataHeight *sizeof(unsigned short)){return -2;};
	if(m_pDataArray == NULL){return -3;};

	memcpy( (void*) m_pDataArray, (const void*) buf, numPix*sizeof(unsigned short));
	MapUshort2rgb();
	m_DrawPanel->Refresh();
	m_DrawPanel->Update();
	Refresh();

	return res;
}

/* Setting display min value*/
void CamViewData::SetDispMin(double val)
{
	m_dDispMin = val;
}
/* Setting display min value*/
void CamViewData::SetDispMax(double val)
{
	m_dDispMax = val;
}

/* acting on chaged text value */
void CamViewData::TextChangedDispMin(wxCommandEvent &)
{
	double val = 0;
	if( !m_bTextInit){return ;};
	wxString strVal = m_textMin->GetValue();
	if( strVal.ToDouble(& val) ) /* read value as double*/
	{
		m_dDispMin = val;
	}
	else
	{
		strVal.Printf(wxT("%d"),m_dDispMin);
		m_textMin->ChangeValue(strVal); 
	}
}

/* acting on chaged text value */
void CamViewData::TextChangedDispMax(wxCommandEvent &)
{
	double val = 0;
	if( !m_bTextInit){return ;};
	wxString strVal = m_textMax->GetValue();
	if( strVal.ToDouble(& val) ) /* read value as double*/
	{
		m_dDispMax = val;
	}
	else
	{
		strVal.Printf(wxT("%d"),m_dDispMax);
		m_textMax->ChangeValue(strVal); 
	}
}