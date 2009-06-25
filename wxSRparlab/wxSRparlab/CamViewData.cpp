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

#ifdef JMU_ALIGNGUI
// implement message map
BEGIN_EVENT_TABLE(jmuDrawPanel, wxPanel)
	EVT_RIGHT_DCLICK(jmuDrawPanel::OnRightDclick)
END_EVENT_TABLE()
#endif

// implement message map
BEGIN_EVENT_TABLE(CamViewData, wxPanel)
	EVT_BUTTON(IDB_CloseViewData,  CamViewData::CloseView)
	EVT_SIZE( CamViewData::OnSize ) 
	EVT_PAINT( CamViewData::OnPaint)
	EVT_TEXT( IDT_DispMin , CamViewData::TextChangedDispMin)
	EVT_TEXT( IDT_DispMax , CamViewData::TextChangedDispMax)
END_EVENT_TABLE()

#ifdef JMU_ALIGNGUI
jmuDrawPanel::jmuDrawPanel( CamViewData* parent) : wxPanel(parent, IDP_DrawPanel)
{
	_parent = parent;
}
jmuDrawPanel::~jmuDrawPanel()
{
}
void jmuDrawPanel::OnRightDclick(wxMouseEvent& event)
{
	wxPoint pos = event.GetPosition();
	/*m_DrawPanel->GetSize(&wP, &hP);
		if( (wP<1) || (hP<1)) { errMutex = m_mutexBitmap->Unlock(); return res;};
		double scFact = ((double) wP / (double) m_nCols);
		if (((double) hP / (double) m_nRows) < scFact) {
				scFact = ((double) hP / (double) m_nRows);};
		int wD = (int) floor(m_nCols * scFact);
		int hD = (int) floor(m_nRows * scFact);

	
		delete(m_pBitmap);
		m_pBitmap = new wxBitmap( m_pWxImg->Scale(wD, hD) );*/
	int nCols = _parent->GetNcols(); int nRows = _parent->GetNrows();
	int cP, rP;
	this->GetSize( &cP, &rP);
	double scFact = ((double) cP / (double) nCols);
	if (((double) rP / (double) nRows) < scFact) { scFact = ((double) rP / (double) nRows);};
	int cD = (int) floor(nCols * scFact);
	int rD = (int) floor(nRows * scFact);
	int cSR =(int) floor( (double)pos.x * (double)nCols / (double)cD);
	int rSR =(int) floor( (double)pos.y * (double)nRows / (double)rD);
	wxString text; 
	text.Printf(wxT("Right double click -  pos: (%04i - %04i) - SRpos: (%04i - %04i)"),
		pos.x, pos.y, cSR, rSR);
	_parent->SetTxtInfo(text);
	if((cSR>=nCols) || (rSR>=nRows))
	{
		text.Printf(wxT("OUTSIDE SR SENSOR -  pos: (%04i - %04i) - SRpos: (%04i - %04i)"),
		pos.x, pos.y, cSR, rSR);
		_parent->SetTxtInfo(text);
	}
	else
	{
		_parent->GetParentCamFrame()->AddAlignPt(cSR, rSR);
	}
}
#endif

/**
 * Camera data panel class constructor \n
 * Each instance must have a parent wnd (usually a notebook) \n
 */
CamViewData::CamViewData(wxWindow* parent, CamFrame* pCamFrame, const wxString& title, const wxPoint& pos, const wxSize& size)
: wxPanel(parent, wxID_ANY, pos, size, wxBORDER_NONE, title)
{
	int res = 0;
	m_bNewImage = false;
	m_bTextInit = false;
	_pCamFrame = pCamFrame;

	m_nComp = NCOMP;
	m_nLUTlen = LUTLEN;

	m_pWxImg = NULL;
	m_pBitmap = NULL;
	m_DrawPanel = NULL;

	res += AllocLUT(); //Allocs the LUT
	
	m_nCols = 176;
	m_nRows = 144;
	
	res += AllocRGBA();
	res += AllocDataArray();

	res += AllocWxImg();
	res += AllocWxBitmap();

	m_dDispMin = 25344.0;
	m_dDispMax = 0.0;

	res += Map2rgb<unsigned short>();
}
/**
 * Camera view panel class destructor \n
 */
CamViewData::~CamViewData()
{
	if(m_pRGB   != NULL) { free((void*) m_pRGB  ); m_pRGB   = NULL; };
	if(m_pAlpha != NULL) { free((void*) m_pAlpha); m_pAlpha = NULL; };
	if(m_pLUT   != NULL) { free((void*) m_pLUT  ); m_pLUT   = NULL; };
	if(m_mutexDataArray != NULL) { delete(m_mutexDataArray); m_mutexDataArray = NULL;};
	if(m_mutexBitmap != NULL) { delete(m_mutexBitmap); m_mutexBitmap = NULL;};
	if(m_pDataArray != NULL) { free((void*) m_pDataArray ); m_pDataArray = NULL; };
	if(m_pWxImg != NULL) { delete( m_pWxImg ); m_pWxImg = NULL; };
	if(m_pBitmap != NULL) { delete( m_pBitmap ); m_pBitmap = NULL; };
}
/**
 * Allocates the LUT onto which data is mapped
 *  - free(m_pLUT) should be called before view termination
 */
int CamViewData::AllocLUT() //! Allocate LUT
{
	m_pLUT = (unsigned char*) malloc(m_nComp* m_nLUTlen *sizeof(unsigned char));
	if(m_pLUT == NULL) { return -1; }; // return if malloc fails

	memset((void*) m_pLUT, 0x77, m_nComp* m_nLUTlen *sizeof(unsigned char)); // init LUT memory zone

	#include "lutDef.h"
	return 0;
}
/**
 * Allocates the RGB buffer for data display
 *  - free(m_pRGB)   should be called before view termination
 *  - free(m_pAlpha) should be called before view termination
 */
int CamViewData::AllocRGBA() //! Allocate RGB and alpha buffer
{
	if( (m_nComp < 0) || (m_nCols < 0) || (m_nRows <0 ) ){return -2;} ; // fail if size not assigned
	m_pRGB = (unsigned char*) malloc(m_nComp* m_nCols * m_nRows *sizeof(unsigned char));
	if(m_pRGB == NULL) { return -1; }; // return if malloc fails
	memset((void*) m_pRGB, 0x77, m_nComp* m_nCols * m_nRows *sizeof(unsigned char));
	m_pAlpha = (unsigned char*) malloc( m_nCols * m_nRows *sizeof(unsigned char));
	if(m_pAlpha == NULL) { return -1; }; // return if malloc fails
	memset((void*) m_pAlpha, 0x77, m_nCols * m_nRows *sizeof(unsigned char));

	int i = 0;
	/* Debug: 3ramps /
	int c = 0; int r = 0; unsigned char R, G, B;
	R = 0; G = 0; B = 0;
	// ENDOF Debug: 3ramps */
	for(i = 0 ; i <(m_nCols * m_nRows); i++)
	{
		/* Debug: 3ramps /
		m_pRGB[m_nComp*i+0] = R;
		m_pRGB[m_nComp*i+1] = G;
		m_pRGB[m_nComp*i+2] = B;
		c +=1;
		if (c >= m_nCols)
		{
			c = 0;
			r +=1;
		}
		if(r < (m_nRows /3)) {R=c;G=0;B=0;}
		else{if(r < (m_nRows *2/3)) {R=0;G=c;B=0;}
		else{R=0;G=0;B=c;} } ;
		// ENDOF Debug: 3ramps */
		/* Debug: blueScreen /
		m_pRGB[3*i+0] = (unsigned char) 0x00; 
		m_pRGB[3*i+1] = (unsigned char) 0x00;
		m_pRGB[3*i+2] = (unsigned char) 0xFF;  
		// ENDOF Debug: blueScreen */ 

		m_pAlpha[i] = (unsigned char) 0xFF; // alpha to 1.0
	}
	return 0;
}
/**
 * Allocates the data array
 *  - free(m_pDataArray)   should be called before view termination
 */
int CamViewData::AllocDataArray() //! Allocate dataArray buffer
{
	if( (m_nCols < 0) || (m_nRows <0 ) ){return -2;} ; // fail if size not assigned
	m_pDataArray = (void*) malloc( m_nCols * m_nRows *sizeof(double));
	if(m_pDataArray == NULL) { return -1; }; // return if malloc fails
	memset((void*) m_pDataArray, 0x77, m_nCols * m_nRows *sizeof(double));

	m_mutexDataArray = new wxMutex(wxMUTEX_DEFAULT);
	m_mutexBitmap = new wxMutex(wxMUTEX_DEFAULT);
	int i, c; // used in loops
	/* double *data = (double*) m_pDataArray; */
	unsigned short *data = (unsigned short*) m_pDataArray;

	/* Debug: Gray ramps */
	c= 0;
	for(i = 0 ; i <(m_nCols * m_nRows); i++)
	{
		data[i] = c;
		c +=1;
		if (c >= 25344)
		{
			c = 0;
		}
	}
	return 0;
}
/**
 * Allocates the wxImg
 */
int CamViewData::AllocWxImg() //! Allocate wxImg buffer
{
	int res = 0;
	if( (m_nCols < 0) || (m_nRows <0 ) ){return -2;} ; // fail if size not assigned
	if(m_pRGB == NULL) { return -1; }; // return the RGB buffer is null
	// convert data from raw image to wxImg 
	m_pWxImg = new wxImage( m_nCols, m_nRows, m_pRGB, TRUE );

	if(m_pAlpha == NULL) { return -3; }; // return the RGB buffer is null
	// convert data from raw image to wxImg 
	m_pWxImg->SetAlpha( m_pAlpha, TRUE);

	return res;
}
/**
 * Allocates the wxBitmap
 */
int CamViewData::AllocWxBitmap() //! Allocate wxBitmap buffer
{
	int res = 0;
	if( (m_nCols < 0) || (m_nRows <0 ) ){return -2;} ; // fail if size not assigned
	if(m_pWxImg == NULL) { return -1; }; // return the RGB buffer is null
	m_pBitmap = new wxBitmap( m_pWxImg->Scale(m_nCols , m_nRows) );
	return res;
}
/**
 * 
 */
int CamViewData::InitViewData()
{
	int res = 0;
	wxString types[] = { wxT("Range"), wxT("Amp"), wxT("z"),
        wxT("xy") };

    m_radioboxDtype = new wxRadioBox(this, wxID_ANY, wxT("DataType"),
        wxDefaultPosition, wxDefaultSize, 4, types, 4, wxRA_SPECIFY_COLS);

	
	m_textMin = new wxTextCtrl( this, IDT_DispMin, wxT("25344.0"));
	m_textMax = new wxTextCtrl( this, IDT_DispMax, wxT("0.0"));
	m_bTextInit = true; m_textMin->SetModified(true); m_textMax->SetModified(true);	
#ifdef JMU_ALIGNGUI
	m_DrawPanel = new jmuDrawPanel(this);
#else
	m_DrawPanel = new wxPanel(this, IDP_DrawPanel);//, wxPoint(-1, -1), wxSize(176, 144));
#endif
	m_DrawPanel->SetMinSize(wxSize(176, 144));
	    wxBoxSizer *sizerDraw = new wxBoxSizer(wxHORIZONTAL);
		sizerDraw->Add(m_DrawPanel, 1, wxEXPAND);
	  wxBoxSizer *sizerText = new wxBoxSizer(wxHORIZONTAL);
	    sizerText->Add(m_textMin, 0, wxEXPAND);
		sizerText->AddStretchSpacer();
	    sizerText->Add(m_textMax, 0, wxEXPAND);
	  wxBoxSizer *sizerDrawText = new wxBoxSizer(wxVERTICAL);
		sizerDrawText->Add(sizerText, 0, wxEXPAND);
		sizerDrawText->Add(sizerDraw, 4, wxEXPAND);

	m_textInfo = new wxStaticText(this, IDT_DispTxtInfo, wxT("Info"));

	/* sizer stuff  ...*/
    wxBoxSizer *sizerPanel = new wxBoxSizer(wxVERTICAL);

    sizerPanel->Add(m_radioboxDtype, 0, wxEXPAND);
	sizerPanel->Add(sizerDrawText, 4, wxEXPAND);

	sizerPanel->Add(m_textInfo, 0, wxEXPAND);


    this->SetSizerAndFit(sizerPanel);

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
	//int nWidth = even.GetSize().GetWidth();
	//int nHeight = even.GetSize().GetHeight();
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
	if(m_DrawPanel->IsShownOnScreen())
	{
		wxPaintDC dc(m_DrawPanel);
		Draw( dc );
	}
	event.Skip();
}

////////////////////////////////////////////////////////////////////
// Method:	Draw
// Class:	CamViewData
// Purose:	camera drawing
// Input:	reference to dc
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CamViewData::Draw( wxDC& dc )
{
	// check if dc available
	if( !dc.Ok() ){ return; }
	wxMutexError errMutex= m_mutexBitmap->Lock();
	if(errMutex == wxMUTEX_NO_ERROR)
	{


		//wxMutexGUIEnter();
		int x,y,w,h;
		dc.GetClippingBox( &x, &y, &w, &h );
		if( (m_pBitmap!=NULL) )
		{
			this->SetBitmap();
			dc.DrawBitmap( m_pBitmap[0] , x, y );
		}
		//wxMutexGUILeave();

		errMutex = m_mutexBitmap->Unlock();
	} //{errMutex == wxMUTEX_NO_ERROR)

	return;
}

/* Setting display min value*/
void CamViewData::SetNewImage()
{
	m_bNewImage = true;
}

/* Setting display min value*/
void CamViewData::SetDispMin(double val)
{
	m_dDispMin = val;
	m_textMin->GetValue().Printf(wxT("%d"), val);
	m_textMin->SetModified(true);
}
/* Setting display min value*/
void CamViewData::SetDispMax(double val)
{
	m_dDispMax = val;
	m_textMax->GetValue().Printf(wxT("%d"), val);
	m_textMax->SetModified(true);
}

/* acting on chaged text value */
void CamViewData::TextChangedDispMin(wxCommandEvent &)
{
	double val = 0;
	if( !m_bTextInit){return ;};
	wxString strVal = m_textMin->GetValue();
	if( strVal.ToDouble(& val) ) /* read value as double*/
	{
		SetDispMin(val);
	}
	else
	{
		m_textMin->DiscardEdits();
		m_textMax->GetValue().Printf(wxT("%d"), m_dDispMin);
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
		SetDispMax(val);
	}
	else
	{
		m_textMax->DiscardEdits();
		m_textMax->GetValue().Printf(wxT("%d"), m_dDispMax);
	}
}

/* sets the bitmap */
int CamViewData::SetBitmap()
{
	int res = 0;
	if(m_DrawPanel == NULL) { return -1;};
	if( (m_nCols < 1) || (m_nRows < 1)) { return -2;};
	if(m_pWxImg == NULL) { return -3;};
	int wP= 0; int hP = 0;
	wxMutexError errMutex= m_mutexBitmap->Lock();
	if(errMutex == wxMUTEX_NO_ERROR)
	{
		m_DrawPanel->GetSize(&wP, &hP);
		if( (wP<1) || (hP<1)) { errMutex = m_mutexBitmap->Unlock(); return res;};
		double scFact = ((double) wP / (double) m_nCols);
		if (((double) hP / (double) m_nRows) < scFact) {
				scFact = ((double) hP / (double) m_nRows);};
		int wD = (int) floor(m_nCols * scFact);
		int hD = (int) floor(m_nRows * scFact);

	
		delete(m_pBitmap);
		m_pBitmap = new wxBitmap( m_pWxImg->Scale(wD, hD) );
		errMutex = m_mutexBitmap->Unlock();
	} //{errMutex == wxMUTEX_NO_ERROR)

	return res;
};

/* Changing info text */
void CamViewData::SetTxtInfo(wxString text)
{
	if(!m_textInfo){return;};
	m_textInfo->SetLabel(text);
	return;
}
