/*
 * wxSRapp.cpp
 * Implementation of the main window for wxSRparlab
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

#include "wxSRparlab.h" //!< top-level header file
#include "wxSrApp.h"	//!< this class header file
#include "CMainWnd.h"	//!< main window class header
#include "CamFrame.h"	//!< camera frame header file


/**
 * Main application construction \n
 * - the main app is derived from wxApp \n
 * - a pointer to the main wnd is initialized as null \n
 * - a pointer^2 to camFrames is initialized as null \n
 * - the _numCams variable is initialized to the global value NUMCAMS \n
 *
 */
//SrApp::SrApp()
//: wxApp()
//{
//    _mainWnd = NULL;
//	_numCams = NUMCAMS;
//}

/**
 * This method is called on startup \n
 * - new instances of GUI classes are created here \n \n
 */
bool SrApp::OnInit()
{
	//! - inits private vars to safe defaults \n
	_mainWnd = NULL;

	//! - creates a new main window \n
    _mainWnd = new MainWnd( _T("wxSRparlab"), wxPoint(05,50), wxSize(300,300) );
	if(_mainWnd != NULL){
		_mainWnd->Init();
		_mainWnd->Show(TRUE); // show the main window
	}
	else{
		return FALSE; // return false if creation fails
	} // ENDOF if(_mainWnd != NULL)

	_mainWnd->AddChildren();
    SetTopWindow(_mainWnd); //! - declares main wnd as top wnd \n
	// success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
	//_mainWnd->SendSizeEvent();
	_mainWnd->Layout();
	int wdt, hgt;
	_mainWnd->GetSize(&wdt, &hgt);
	_mainWnd->SetSize(wdt+1, hgt+1);
	_mainWnd->Refresh();
	_mainWnd->Raise();
    return TRUE;
} 


/**
 * This method is called on exit \n
 * - instances of GUI classes are closed by wxWidget heritance \n
 */
int SrApp::OnExit()
{
	int res = 0;
	return res;
}