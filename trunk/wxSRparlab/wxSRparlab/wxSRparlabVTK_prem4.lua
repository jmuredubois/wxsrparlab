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
	if not _OPTIONS["dynamic-runtime"] then
		flags					{ "StaticRuntime" }
	end
	
	includedirs				{string.format('%s',os.getenv("JMU_TICPP"))} --TICPP includes
	libdirs					{string.format('%s%s',os.getenv("JMU_TICPP"), "/lib")} -- TICPP libs
	includedirs				{string.format('%s%s',os.getenv("JMU_SVNSANDBOX_TRUNK"), "/libSRPLscat")}
	includedirs				{string.format('%s%s',os.getenv("JMU_SVNSANDBOX_TRUNK"), "/libSRPLavg")}
	includedirs				{string.format('%s%s',os.getenv("JMU_SVNSANDBOX_TRUNK"), "/libSRPLcoordTrf")}
	includedirs				{string.format('%s%s',os.getenv("JMU_SVNSANDBOX_TRUNK"), "/libSRPLsegm")}
	includedirs				{string.format('%s%s',os.getenv("JMU_SVNSANDBOX_TRUNK"), "/libSRPLransac")}
	
	
	includedirs				{
    string.format('%s',os.getenv("JMU_VTKBINBASE")),
    string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "/Graphics"),
	string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "/Rendering"),
	string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "/Common"),
	string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "/Filtering"),
	string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "/IO"),
	string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "/Hybrid")
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
		links					{"SRPLscat", "SRPLavg", "SRPLcoordTrf", "SRPLsegm", "SRPLransac"}
		libdirs					{(string.format('%s%s',os.getenv("JMU_BUILDS"), "/Release/bin"))}
	configuration "Debug"
		targetname 				( outName.."d" )
		targetdir				(string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/bin"))
		objdir					(string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/obj"))
		implibdir				(string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/lib"))
		defines					{ "DEBUG", "_DEBUG" }
		flags					{ "Symbols" }
		links					{ "ticppd" }	-- link TICPP
		links					{"SRPLscatd", "SRPLavgd", "SRPLcoordTrfd", "SRPLsegmd", "SRPLransacd"}
		libdirs					{(string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/bin"))}

	-- -- Operating Systems specific
	if configuration "windows" then
		defines					{ "WIN32", "_WINDOWS" }
		includedirs				{string.format('%s',os.getenv("JMU_FFTW3"))} --FFTW3 includes
		libdirs				{string.format('%s',os.getenv("JMU_FFTW3"))} --FFTW3 binaries
		links					{ "libfftw3-3" } -- FFTW3 lib name
		--includedirs      		{string.format('%s',os.getenv("JMU_MESA_LIBUSBSR"))} -- setup of libUSBSR
		--libdirs      		{string.format('%s',os.getenv("JMU_MESA_LIBUSBSR"))} -- setup of libUSBSR
	else
		excludes				{ "**.rc" }		-- Ignore resource files in Linux.
		buildoptions			{ "-fPIC" }
	end

	configuration{"macosx", "codeblocks"}
		libdirs					(string.format('%s', "/usr/local/lib"))
		
	configuration "macosx"
		targetextension		".dylib"
		linkoptions			"-dynamiclib"
		includedirs				{string.format('%s%s',os.getenv("JMU_FFTW3"), "/include")} --FFTW3 includes
		libdirs				{string.format('%s%s',os.getenv("JMU_FFTW3"), "/lib")} --FFTW3 binaries
		includedirs				{string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "")} --FFTW3 includes
		libdirs				{string.format('%s%s',os.getenv("JMU_FFTW3"), "/lib")} --FFTW3 binaries
		links					{ "fftw3" } -- FFTW3 lib name
		buildoptions			{"`wx-config --cxxflags` -W -Wall -ansi"} -- wxWidgets include
		linkoptions				{"`wx-config --libs`"} --wxWdigets link
		libdirs					{string.format('%s', "/opt/local/var/macports/software/wxWidgets/2.8.10_0/opt/local/lib")}
		










