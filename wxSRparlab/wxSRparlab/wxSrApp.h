/*
 * CMainWnd.h
 * Header of the main window for wxSRparlab
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
#include "wxSRparlab.h" // top-level header file
#include "CMainWnd.h"	// main window class header
#include "CamFrame.h"	// camera frame header file

/*
 * Required class declarations
 */
class SrApp;		// main application
class MainWnd;		// main window

/**
 * Main application class
 */
class SrApp: public wxApp	//! Main application class
{
public:
	//SrApp::SrApp();		//!< constructor
    virtual bool OnInit();	//!< method called on initialization
	virtual int  OnExit();	//!< method called on exit

private:
	MainWnd		*_mainWnd;		//!< pointer to main window
};