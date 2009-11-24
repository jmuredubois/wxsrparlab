/*
 * CamViewData.h
 * Header of the camera view panel for wxSRparlab
 *
 *    Copyright 2009 James Mure-Dubois, Heinz Hügli and Institute of Microtechnology of EPFL.
 *      http://parlab.epfl.ch/
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $Rev$:
 * $Author$:
 * $Date$:
 */

#pragma once
#include "wxSRparlab.h" //!< top-level header file
#include "wxSrApp.h"	//!< application header file
#include "CMainWnd.h"	//!< main window header file
#include "CamFrame.h"	//!< cam  window header file
#define JMU_ALIGNGUI // testing alignment GUI

#define LUTLEN 256	// color LUT length
#define NCOMP 3		// number of components (RGB)

#ifdef JMU_ALIGNGUI
class CamViewData;
class jmuDrawPanel: public wxPanel
{
public:
	//! constructor
    jmuDrawPanel( CamViewData* parent);
	//! destructor
	~jmuDrawPanel();
	void OnRightDclick(wxMouseEvent& event);
private:
	CamViewData* _parent;
// protected data
protected:
	DECLARE_EVENT_TABLE()
};
#endif

/**
 * Camera settings panel class \n
 * This class: \n
 * - displays GUI elements allowing to modify camera settings \n
 */
class CamFrame;
class CamViewData: public wxPanel //!< Camera view panel
{
public:
	//! constructor
    CamViewData( wxWindow* parent, CamFrame* pCamFrame, const wxString& title, const wxPoint& pos, const wxSize& size);
	//! destructor
	~CamViewData();

	//! init settings
	int InitViewData();
	wxMutex* m_mutexDataArray;
	wxMutex* m_mutexBitmap;
	CamFrame* GetParentCamFrame(){ return _pCamFrame;};

private:
	CamFrame* _pCamFrame;
	int AllocLUT(); /* initialize LUT */
	int AllocRGBA(); /* initialize RGB and Alpha buffer */
	int AllocDataArray(); /* initialize data buffer */
	int AllocWxImg(); /* initialize WxImage buffer */
	int AllocWxBitmap(); /* initialize WxBitmap buffer */
	
public:
	/* Opening and closing */
	void CloseView(wxCommandEvent& WXUNUSED(event)); //!< Closes the display
	void StopView(wxCommandEvent& WXUNUSED(event));  //!< Disables the display
	int GetNcols(){return m_nCols;};
	int GetNrows(){return m_nRows;};

	/* Changing interface buttons text */

	void SetTxtInfo(wxString text);

	/* Drawing bitmap */
	void Draw( wxDC& dc );
	void SetNewImage();

	/* Mapping data limits*/
	void SetDispMin(double val);
	void SetDispMax(double val);
	double GetDispMin(){return m_dDispMin;};
	double GetDispMax(){return m_dDispMax;};
	/* */
	void TextChangedDispMin(wxCommandEvent& WXUNUSED(event));/* */
	void TextChangedDispMax(wxCommandEvent& WXUNUSED(event));/* */
	
	/* Copying data to display */
	template<typename T> int SetDataArray( T* buf, int numPix)
	{
		int res= 0;
		if(buf == NULL){return -1;};
		if(numPix*sizeof(T) > m_nCols * m_nRows *sizeof(T)){return -2;};
		if(m_pDataArray == NULL){return -3;};

		wxMutexError errMutex= m_mutexDataArray->Lock();
		if(errMutex == wxMUTEX_NO_ERROR)
		{
			memcpy( (void*) m_pDataArray, (const void*) buf, numPix*sizeof(T));
			errMutex = m_mutexDataArray->Unlock();
		}
		this->Map2rgb<T>();
		m_DrawPanel->Refresh();
		if(this->IsShownOnScreen()){ this->Refresh();};

		return res;
	};
//template int CamViewData::SetDataArray<unsigned short>( unsigned short* buf, int numPix);


	/* Mapping data to RGB buffer*/
	template<typename T> int Map2rgb()
{
	int res = 0;
	if(m_pLUT       == NULL){return -1;};
	if(m_pRGB       == NULL){return -2;};
	if(m_pDataArray == NULL){return -3;};

	wxMutexError errMutex= m_mutexDataArray->Lock();
	if(errMutex == wxMUTEX_NO_ERROR)
	{
		T* data = (T*) m_pDataArray;
		double invDyn = 1.0/ (m_dDispMax - m_dDispMin) * ( (double) m_nLUTlen);

		int val = 0;
		unsigned char *curPix, *curCol;
		for(int i = 0 ; i <(m_nCols * m_nRows); i++)
		{
			val = (int)floor(( ((double)data[i]) - m_dDispMin ) * invDyn );
			if(val <0){ val = 0;}
			if(val > (m_nLUTlen-1) ) {val = (m_nLUTlen-1) ;}


			curPix = &m_pRGB[m_nComp*i+0];
			curCol = &m_pLUT[m_nComp*val];
			memcpy( (void*) curPix, (const void*) curCol, m_nComp);
		}
		errMutex = m_mutexDataArray->Unlock();
	} //{errMutex == wxMUTEX_NO_ERROR)

	return res;
};



	int SetBitmap();
private:

	/* RadioBoxes */
	wxRadioBox* m_radioboxDtype;	//!< SR data type

	/* Text zones */
	wxTextCtrl* m_textMin;
	wxTextCtrl* m_textMax;
	bool m_bTextInit;
#ifdef JMU_ALIGNGUI
	jmuDrawPanel* m_DrawPanel;
#else
	wxPanel* m_DrawPanel;
#endif
	wxStaticText* m_textInfo;

	// Private data
private:
	wxImage*	m_pWxImg;	// pointer to image  (Dev. Indep.)
	wxBitmap*	m_pBitmap;	// wxBitmap to paint (Dev. Dep.)
	bool	m_bNewImage;

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
	int m_nCols;	// data width
	int m_nRows;	// data type
	int m_nDispWidth;	// data width
	int m_nDispHeight;	// data type
	int m_nDataBytes;	// data bytes per sample
	void *m_pDataArray;			// data array
	
	double m_dDispMin;	// min value
	double m_dDispMax;	// max value
	

// protected data
protected:
	DECLARE_EVENT_TABLE()
};


//! enum used by the CamViewData class
enum CamViewDataEnum
{
	IDB_CloseViewData   = 256 ,
	IDB_FreezeViewData  = 257 ,
	IDT_DispMin = 258,
	IDT_DispMax = 259,
	IDP_DrawPanel = 260,
	IDT_DispTxtInfo = 261,
	ID_ThisIsAStop2 = 512
};