/*
 * wxSRparlab.h
 * Top level header file for wxSRparlab
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

//#define DUMMYSR //!< use this flag to disable SR fcts

#ifndef DUMMYSR
    #define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
	#define _CRT_SECURE_NO_DEPRECATE
	// Windows Header Files:
	#include <windows.h>
	#include "libMesaSR.h"
	#pragma comment( lib, "libMesaSR" )
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
#include "wx/timer.h"		//!< for timer interface (used in rendering for example);
#include "wx/stopwatch.h"	//!< for stopwatch interface;
#include "wx/combobox.h"		//!< for listbox

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

#define NUMCAMS 4	//!< number of SR cams

#define SAFE_FREE(p)       { if(p) { delete (p);     (p)=0; } }

//#define JMU_TGTFOLLOW // WATCH OUT: JMU_USE_VTK must be defined to use this //added to project options
#ifdef JMU_USE_VTK
	#include "wxSRvtkView.h"
#endif

#define JMU_RANSAC // testing experimental C++ RANSAC
#define JMU_ICPVTK // testing ICP from VTK
#define JMU_KDTREEVTK // testing KDTREE from VTK
#define JMU_KDSAVE // saving kdTree distances.
#define JMU_ALIGNGUI // testing alignment GUI

#include "srBuf.h"
