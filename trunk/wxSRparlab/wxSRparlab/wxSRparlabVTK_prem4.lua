-- premake4 default script file to automate building of wxSRparlabVTK
--
--    Copyright 2009 James Mure-Dubois, Heinz Hügli and Institute of Microtechnology of EPFL.
--      http://parlab.epfl.ch/
--
--    This program is free software: you can redistribute it and/or modify
--    it under the terms of the GNU General Public License as published by
--    the Free Software Foundation, either version 3 of the License, or
--    (at your option) any later version.
--
--    This program is distributed in the hope that it will be useful,
--    but WITHOUT ANY WARRANTY; without even the implied warranty of
--    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--    GNU General Public License for more details.
--
--    You should have received a copy of the GNU General Public License
--    along with this program.  If not, see <http://www.gnu.org/licenses/>.
--
-- $Rev$:
-- $Author$:
-- $Date$:
-- Set package name.
project "wxSRparlabVTKPREM"
	-- if you want a different name for your target than the projects's name.
	outName						= "wxSRparlabVTKPREM"
	uuid						"F0174772-C16D-4CED-8E0E-3B1348DB3631"
	-- Set the files to include/exclude.
	files						{ "*.cpp", "*.h" }
	--excludes					{ "" }
	-- Setup the output directory options.
	--		Note: Use 'libdir' for "lib" kind only.
	--bindir					"../lib"
	--libdir					"../lib"
	-- Set the defines.
	--defines						{ "_SRPL_API_DLL" }
	implibprefix 				""
	targetprefix 				""
	-- Common setup
	language					"C++"
	flags						{ "NoPCH" }
	--******* GENAERAL SETUP **********
	--*	Settings that are not dependant
	--*	on the operating system.
	--*********************************
	kind						"WindowedApp"
	excludes					{"camTranMats*.cpp"}

-- OPTIONS ---------------------------------------------------------------------

	-- -- Dynamic Runtime
	if _OPTIONS["dynamic-runtime"] then
		flags					{ "StaticRuntime" }
	end
	
	includedirs				{string.format('%s',os.getenv("JMU_TICPP"))} --TICPP includes
	libdirs					{string.format('%s%s',os.getenv("JMU_TICPP"), "/lib")} -- TICPP libs
	includedirs				{string.format('%s%s',os.getenv("JMU_SVNSANDBOX_TRUNK"), "/libSRPLscat")}
	includedirs				{string.format('%s%s',os.getenv("JMU_SVNSANDBOX_TRUNK"), "/libSRPLavg")}
	includedirs				{string.format('%s%s',os.getenv("JMU_SVNSANDBOX_TRUNK"), "/libSRPLcoordTrf")}
	includedirs				{string.format('%s%s',os.getenv("JMU_SVNSANDBOX_TRUNK"), "/libSRPLsegm")}
	includedirs				{string.format('%s%s',os.getenv("JMU_SVNSANDBOX_TRUNK"), "/libSRPLransac")}
    includedirs				{string.format('%s%s',os.getenv("JMU_SVNSANDBOX_TRUNK"), "/libSRPLalign")}
	
	
	includedirs				{
    string.format('%s',os.getenv("JMU_VTKBINBASE")),
    string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "/Graphics"),
	string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "/Rendering"),
	string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "/Common"),
	string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "/Filtering"),
	string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "/IO"),
	string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "/Hybrid"),
    string.format('%s%s',os.getenv("JMU_VTKBINBASE"), "/Utilities")
  							}
  	links					{"vtkGraphics", "vtkRendering", "vtkCommon", "vtkFiltering", "vtkIO", "vtkHybrid"}
  	libdirs				{string.format('%s',os.getenv("JMU_VTKBINBASE"))}
  	defines					{"JMU_USE_VTK", "JMU_TGTFOLLOW"}
	

-- CONFIGURATIONS -------------------------------------------------------------
--
	-- -- Unicode
	configuration "unicode"
		flags					{ "Unicode" }
		defines					{ "UNICODE", "_UNICODE" }

	-- -- GCC compilers
	configuration { "code*", "gmake" }
		objdir					".obj"
		buildoptions			{ "-O0" }

	-- -- Visual Studio
	configuration "vs*"
		-- Windows and Visual C++ 2005/2008
		defines					{ "_CRT_SECURE_NO_DEPRECATE" }
        implibprefix 				"lib"

	-- Release/Debug
	-- Set the default targetName if none is specified.
	if #outName <= 0 then outName = project().name end
	print( outName )
	print( project().name )
	-- Set the targets.
	configuration "Release"
		targetname				( outName )
		targetdir				(string.format('%s%s',os.getenv("JMU_BUILDS"), "/Release/bin"))
		objdir					(string.format('%s%s',os.getenv("JMU_BUILDS"), "/Release/obj"))
		implibdir				(string.format('%s%s',os.getenv("JMU_BUILDS"), "/Release/lib")) 
		defines					{ "NDEBUG" }
		flags					{ "OptimizeSpeed" }
		links					{ "ticpp" }	-- link TICPP
		--links					{"SRPLscat", "SRPLavg", "SRPLcoordTrf", "SRPLsegm", "SRPLransac", "SRPLalign"}
		libdirs					{(string.format('%s%s',os.getenv("JMU_BUILDS"), "/Release/bin"))}
        libdirs					{(string.format('%s%s',os.getenv("JMU_BUILDS"), "/Release/lib"))}

	configuration "Debug"
		targetname 				( outName.."d" )
		targetdir				(string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/bin"))
		objdir					(string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/obj"))
		implibdir				(string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/lib"))
		defines					{ "DEBUG", "_DEBUG" }
		flags					{ "Symbols" }
		links					{ "ticppd" }	-- link TICPP
		--links					{"SRPLscatd", "SRPLavgd", "SRPLcoordTrfd", "SRPLsegmd", "SRPLransacd","SRPLalignd"}
		libdirs					{(string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/bin"))}
        libdirs					{(string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/lib"))}

	-- -- Operating Systems specific
	if configuration "windows" then
        flags                   { "WinMain" } 
		defines					{ "WIN32", "_WINDOWS" }
		includedirs				{string.format('%s',os.getenv("JMU_FFTW3"))} --FFTW3 includes
		libdirs				{string.format('%s',os.getenv("JMU_FFTW3"))} --FFTW3 binaries
		links					{ "libfftw3-3" } -- FFTW3 lib name
		includedirs      		{string.format('%s',os.getenv("JMU_MESA_LIBUSBSR"))} -- setup of libUSBSR
		libdirs      		{string.format('%s',os.getenv("JMU_MESA_LIBUSBSR"))} -- setup of libUSBSR
        includedirs				{(string.format('%s%s',os.getenv("WXWIN"), "/include"))} --wxWidgets include
        libdirs				{(string.format('%s%s',os.getenv("WXWIN"), "/lib/vc_lib"))} --wxWidgets include
        links       {"winmm", "comctl32", "rpcrt4", "wsock32", "odbc32" } --wxWidgets links
        if configuration "vs*" then
            includedirs				{(string.format('%s%s',os.getenv("WXWIN"), "/include/msvc"))} --wxWidgets include
            configuration "Debug"
                includedirs				{(string.format('%s%s',os.getenv("WXWIN"), "/lib/vc_lib/mswd"))} --wxWidgets include
                libdirs				{(string.format('%s%s',os.getenv("JMU_VTKBINBASE"), "/bin/debug"))} -- VTK libs
                links					{"libSRPLscatd", "libSRPLavgd", "libSRPLcoordTrfd", "libSRPLsegmd", "libSRPLransacd","libSRPLalignd"}
                links       {"wxbase28d", "wxbase28d_net", "wxbase28d_xml", "wxexpatd", "wxjpegd", "wxmsw28d_adv",  "wxmsw28d_aui",  "wxmsw28d_core", "wxmsw28d_html", "wxmsw28d_media", "wxmsw28d_qa",  "wxmsw28d_richtext", "wxmsw28d_xrc", "wxpngd", "wxregexd", "wxtiffd", "wxzlibd" }
            configuration "Release"
                includedirs				{(string.format('%s%s',os.getenv("WXWIN"), "/lib/vc_lib/msw"))} --wxWidgets include
                libdirs				{(string.format('%s%s',os.getenv("JMU_VTKBINBASE"), "/bin/release"))} -- VTK libs
                links					{"libSRPLscat", "libSRPLavg", "libSRPLcoordTrf", "libSRPLsegm", "libSRPLransac", "libSRPLalign"}
                links       {"wxbase28", "wxbase28_net", "wxbase28_xml", "wxexpat", "wxjpeg", "wxmsw28_adv",  "wxmsw28_aui",  "wxmsw28_core", "wxmsw28_html", "wxmsw28_media", "wxmsw28_qa",  "wxmsw28_richtext", "wxmsw28_xrc", "wxpng", "wxregex", "wxtiff", "wxzlib" }
        end
	else
		excludes				{ "**.rc" }		-- Ignore resource files in Linux.
		buildoptions			{ "-fPIC" }
        configuration "Debug"
            links					{"SRPLscatd", "SRPLavgd", "SRPLcoordTrfd", "SRPLsegmd", "SRPLransacd","SRPLalignd"}
        configuration "Release"
            links					{"SRPLscat", "SRPLavg", "SRPLcoordTrf", "SRPLsegm", "SRPLransac", "SRPLalign"}
	end

	configuration{"macosx", "codeblocks"}
		libdirs					(string.format('%s', "/usr/local/lib"))
		
	configuration "macosx"
		includedirs				{string.format('%s%s',os.getenv("JMU_FFTW3"), "/include")} --FFTW3 includes
		libdirs				{string.format('%s%s',os.getenv("JMU_FFTW3"), "/lib")} --FFTW3 binaries
		includedirs				{string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "")} --FFTW3 includes
		libdirs				{string.format('%s%s',os.getenv("JMU_FFTW3"), "/lib")} --FFTW3 binaries
		links					{ "fftw3" } -- FFTW3 lib name
		buildoptions			{"`wx-config --cxxflags` -W -Wall -ansi"} -- wxWidgets include
		linkoptions				{"`wx-config --libs`"} --wxWdigets link
		libdirs					{string.format('%s', "/opt/local/var/macports/software/wxWidgets/2.8.10_0/opt/local/lib")}
		










