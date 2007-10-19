/**
 * wxSRparlab.h 
 * Top level header file for wxSRparlab
 *
 * @author: James Mure-Dubois
 * @version: 2007.10.19
 */

#pragma once

#define wxUSE_GUI 1		//!< using wxWidgets GUI components
#define __WXDEBUG__		//!< using debug version of wxWidgets

/** the environment variable WXWIN should point to a valid wxWidget 
 *  installation \n
 *  - "$(WXWIN)/include"  should be added to the include path \n
 *  - config specific: "$(WXWIN)/include/vc_lib/msvc/" should 
 *    be added to the include path for "setup.h"
 */
#include "wx/wx.h"    //!< base for wxWindows


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

