/*
 * CMainWnd.h
 * Header of the main window for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.10.19
 */

#pragma once
#include "wxSRparlab.h" //!< top-level header file

/**
 * Main application class
 */
class wxSrApp: public wxApp	//! Main application class
{
	//! method called on initialization
    virtual bool OnInit();
};