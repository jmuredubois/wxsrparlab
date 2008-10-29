/*
 * wxSRparlab.h
 * Top level header file for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.11.07
 */

#pragma once

#define DUMMYSR //!< use this flag to disable SR fcts

#ifndef DUMMYSR
    #define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
	#define _CRT_SECURE_NO_DEPRECATE
	// Windows Header Files:
	#include <windows.h>
	#include "libusbSR.h"
	#pragma comment( lib, "libusbSR" )
#else
    #include "srDummy.h"
#endif

#define wxUSE_GUI 1		//!< using wxWidgets GUI components

/** the environment variable WXWIN should point to a valid wxWidget
 *  installation \n
 *  - "$(WXWIN)/include"  should be added to the include path \n
 *  - config specific: "$(WXWIN)/include/vc_lib/msvc/" should
 *    be added to the include path for "setup.h"
 */
//#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include "wx/wx.h"			//!< base for wxWindows
#include "wx/mdi.h"			//!< used for MDI interface
#include "wx/panel.h"		//!< for panels (used in tabbed interface)
#include "wx/notebook.h"	//!< for notebook (tabbed interface)
#include "wx/aui/auibook.h" //!< AUI notebook (DEBUG)
#include "wx/datetime.h"	//!< for date/time
#include "wx/stattext.h"	//!< for static text
#include "wx/textctrl.h"	//!< for text control
#include "wx/image.h"		//!< for images
#include "wx/checkbox.h"	//!< for checkboxes
#include "wx/ffile.h"		//!< for files
#include "wx/thread.h"		//!< for threads
#include "wx/gbsizer.h"		//!< for grid bag sizer
#include "wx/event.h"		//!< for command events
#include "wx/app.h"			//!< for posting events

#include <list>		//STL list
#include <vector>		//STL list


/** on win32 machine, wxWidgets needs those 5 libs, \n
 *  comctl32.lib rpcrt4.lib winmm.lib advapi32.lib wsock32.lib
 *  according to http://www.wxwidgets.org/wiki/index.php/MSVC
 *  (accessed on 2007.10.19)
 */

/** wxWidgets needs 2 more libs, depending on conf \n
 * - Directory 	        Basic wxWidgets libraries 	        Description \n
 * - lib\vc_lib\msw 	wxmsw28_core.lib wxbase28.lib 	    release ANSI static \n
 * - lib\vc_lib\mswd 	wxmsw28d_core.lib wxbase28d.lib 	debug ANSI static \n
 * - lib\vc_lib\mswu 	wxmsw28u_core.lib wxbase28u.lib 	release Unicode static \n
 * - lib\vc_lib\mswud 	wxmsw28ud_core.lib wxbase28ud.lib 	debug Unicode static \n
 *  according to http://www.wxwidgets.org/wiki/index.php/MSVC
 *  (accessed on 2007.10.19)
 */

#include <math.h>

#define TIXML_USE_TICPP
#include "ticpp.h" //Open source XML parser

#define NUMCAMS 2	//!< number of SR cams

#define SAFE_FREE(p)       { if(p) { delete (p);     (p)=0; } }

//#define JMU_TGTFOLLOW // WATCH OUT: JMU_USE_VTK must be defined to use this //added to project options
#ifdef JMU_USE_VTK
	#include "wxSRvtkView.h"
#endif

#include "srBuf.h"
