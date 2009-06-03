-- Set package name.
project "wxSRparlabVTKPREM"
	-- if you want a different name for your target than the projects's name.
	outName						= "libSRPLscat"
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

-- OPTIONS ---------------------------------------------------------------------

	-- -- Dynamic Runtime
	if not _OPTIONS["dynamic-runtime"] then
		flags					{ "StaticRuntime" }
	end
	
	includedirs				{string.format('%s',os.getenv("JMU_TICPP"))} --TICPP includes
	libdirs					{string.format('%s%s',os.getenv("JMU_TICPP"), "/lib")} -- TICPP libs
	

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
	configuration "Debug"
		targetname 				( outName.."d" )
		targetdir				(string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/bin"))
		objdir					(string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/obj"))
		implibdir				(string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/lib"))
		defines					{ "DEBUG", "_DEBUG" }
		flags					{ "Symbols" }
		links					{ "ticppd" }	-- link TICPP

	-- -- Operating Systems specific
	if configuration "windows" then
		defines					{ "WIN32", "_WINDOWS" }
		includedirs				{string.format('%s',os.getenv("JMU_FFTW3"))} --FFTW3 includes
		libdirs				{string.format('%s',os.getenv("JMU_FFTW3"))} --FFTW3 binaries
		links					{ "libfftw3-3" } -- FFTW3 lib name
		includedirs      		{string.format('%s',os.getenv("JMU_MESA_LIBUSBSR"))} -- setup of libUSBSR
		libdirs      		{string.format('%s',os.getenv("JMU_MESA_LIBUSBSR"))} -- setup of libUSBSR
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
		links					{ "fftw3" } -- FFTW3 lib name
		












package.buildflags = {"no-pch","no-main"}
--package.defines = {""}



--- setup of wxWidgets build options
if (OS == "windows") then
  tinsert(package.includepaths,
    {
      string.format('%s%s',os.getenv("WXWIN"), "/include")
    }
  )
  if (target =="vs2005") then
	tinsert(package.includepaths, 
	  {
		string.format('%s%s',os.getenv("WXWIN"), "/include/msvc")
	  }
	)
	tinsert( package.libpaths, 
      {
        string.format('%s%s',os.getenv("WXWIN"), "/lib/vc_lib")--,
		--string.format('%s%s',os.getenv("WXWIN"), "/lib/vc_dll") --causes LINK ERRORS on PARLAB
      }
	)
  end
end
if (OS == "macosx") then
  tinsert(package.includepaths,
    {
      string.format('%s', "/usr/local/lib/wx/include/mac-ansi-release-2.8")
      --string.format('%s%s',os.getenv("WXWIN"), "/include")
    }
  )
  if (target =="cb-gcc") then
	tinsert( package.libpaths, 
      {
      	string.format('%s', "/usr/local/lib")
        --string.format('%s%s',os.getenv("WXWIN"), "/build-release")
      }
	)
  end
end

tinsert(package.config["ReleaseVTK"].buildflags, {"optimize-speed"})

--addoption("--with-shared-wxwin", "link wxWidgets as a shared library")
-- --Configure a C/C++ package to use wxWidgets
--function configureWxWidgets(package)

if (package.kind == "dll") then
  tinsert(package.defines, "WXMAKINGDLL")
end

if (OS == "linux") then
  tinsert(package.buildoptions, "`wx-config --cxxflags` -W -Wall -ansi -pedantic")
  tinsert(package.linkoptions,  "`wx-config --libs`")
end
if (OS == "macosx") then
  tinsert(package.buildoptions, "`wx-config --cxxflags --unicode=no` -W -Wall -ansi")
  tinsert(package.linkoptions,  "`wx-config --libs --unicode=no`")
  tinsert(package.buildoptions, "-I/usr/local/lib/wx/include/mac-ansi-release-2.8 -I/usr/local/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXMAC__");
  tinsert(package.linkoptions,  "-L/usr/local/lib   -framework IOKit -framework Carbon -framework Cocoa -framework System -framework QuickTime  -lwx_mac_richtext-2.8 -lwx_mac_aui-2.8 -lwx_mac_xrc-2.8 -lwx_mac_qa-2.8 -lwx_mac_html-2.8 -lwx_mac_adv-2.8 -lwx_mac_core-2.8 -lwx_base_carbon_xml-2.8 -lwx_base_carbon_net-2.8 -lwx_base_carbon-2.8")
end

if (OS == "windows") then
  --tinsert(package.defines, { "WIN32", "_WINDOWS", "WINVER=0x400", "_MT", "wxUSE_GUI=1" })
  tinsert(package.links,   { "comctl32", "rpcrt4", "winmm", "advapi32", "wsock32"})

  --tinsert(package.config["Debug"].defines , "_DEBUG")
  tinsert(package.config["ReleaseVTK"].defines, "NDEBUG")

  --tinsert(package.defines, "WXUSINGDLL")
  tinsert(package.config["DebugVTK"].links, {"wxmsw28d_core", "wxbase28d"})
  tinsert(package.config["ReleaseVTK"].links, {"wxmsw28_core", "wxbase28"})
end

---END OF package includes for wxWidgets


--package includes for VTK
tinsert(package.includepaths,
  {
    string.format('%s',os.getenv("JMU_VTKBINBASE")),
    string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "/Graphics"),
	string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "/Rendering"),
	string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "/Common"),
	string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "/Filtering"),
	string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "/IO"),
	string.format('%s%s',os.getenv("JMU_VTKSRCBASE"), "/Hybrid")
  }
)
tinsert(package.libpaths,
  {
    string.format('%s%s',os.getenv("JMU_VTKBINBASE"), "/bin"), --for MacOSX version
	string.format('%s%s',os.getenv("JMU_VTKBINBASE"), "/bin/release") --for WinXP version
  }
)
tinsert(package.links,
  {
    "vtkGraphics", "vtkRendering", "vtkCommon", "vtkFiltering", "vtkIO", "vtkHybrid"
  }
)
tinsert(package.defines, { "JMU_USE_VTK" } )
tinsert(package.defines, { "JMU_TGTFOLLOW" } )
--END OF package includes for VTK


--package includes for libSRPLscat (uses FFTW)
  tinsert(package.links,   { "libSRPLscat"})
  tinsert(package.includepaths,
  {
	string.format('%s%s',os.getenv("JMU_SVNSANDBOX_TRUNK"), "/libSRPLscat")
  }
  )
--END OF package includes for libSRPLscat (uses FFTW)
--package includes for libSRPLavg
  tinsert(package.links,   { "libSRPLavg"})
  tinsert(package.includepaths,
  {
	string.format('%s%s',os.getenv("JMU_SVNSANDBOX_TRUNK"), "/libSRPLavg")
  }
  )
--END OF package includes for libSRPLavg
--package includes for libSRPLcoordTrf
  tinsert(package.links,   { "libSRPLcoordTrf"})
  tinsert(package.includepaths,
  {
	string.format('%s%s',os.getenv("JMU_SVNSANDBOX_TRUNK"), "/libSRPLcoordTrf")
  }
  )
--END OF package includes for libSRPLcoordTrf
--package includes for libSRPLsegm
  tinsert(package.links,   { "libSRPLsegm"})
  tinsert(package.includepaths,
  {
	string.format('%s%s',os.getenv("JMU_SVNSANDBOX_TRUNK"), "/libSRPLsegm")
  }
  )
--END OF package includes for libSRPLsegm
--package includes for libSRPLransac
  tinsert(package.links,   { "libSRPLransac"})
  tinsert(package.includepaths,
  {
	string.format('%s%s',os.getenv("JMU_SVNSANDBOX_TRUNK"), "/libSRPLransac")
  }
  )
--END OF package includes for libSRPLransac

  tinsert( package.config["DebugVTK"].libpaths, 
  {
    string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/lib")
  }
  )
  tinsert( package.config["ReleaseVTK"].libpaths, 
  {
    string.format('%s%s',os.getenv("JMU_BUILDS"), "/Release/lib")
  }
  )
  
-- --http://wiki.wxwindows.org/WxMac_Issues
--[
if (OS == "macosx") then
--  	string.format('%s%s',"cd ",string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/bin")
--),
tinsert(
  package.config["DebugVTK"].postbuildcommands, 
  {
  	"pwd",
  	string.format('%s%s%s%s%s',"rm -f -r -v ",os.getenv("JMU_BUILDS"), "/Debug/bin/",package.name, ".app"),
  	string.format('%s%s%s%s%s',"mkdir ",os.getenv("JMU_BUILDS"), "/Debug/bin/",package.name, ".app"),
  	string.format('%s%s%s%s%s',"mkdir ",os.getenv("JMU_BUILDS"), "/Debug/bin/",package.name, ".app/Contents"),
  	string.format('%s%s%s%s%s',"mkdir ",os.getenv("JMU_BUILDS"), "/Debug/bin/",package.name, ".app/Contents/MacOS"),
  	string.format('%s%s%s%s%s',"mkdir ",os.getenv("JMU_BUILDS"), "/Debug/bin/",package.name, ".app/Contents/Resources"),
  	string.format('%s%s%s%s%s',"mkdir ",os.getenv("JMU_BUILDS"), "/Debug/bin/",package.name, ".app/Contents/Resources/English.lproj"),
  	string.format('%s%s%s%s%s%s%s%s%s%s',"cp ",os.getenv("JMU_BUILDS"), "/Debug/bin/",package.name," ",os.getenv("JMU_BUILDS"), "/Debug/bin/", package.name, ".app/Contents/MacOS/", package.name),
  	string.format('%s%s%s%s%s%s%s%s%s',"cp ",package.name,".icns ",os.getenv("JMU_BUILDS"), "/Debug/bin/", package.name, ".app/Contents/Resources/", package.name,".icns"),
  	string.format('%s%s%s%s%s',"cp Info.plist ",os.getenv("JMU_BUILDS"), "/Debug/bin/", package.name, ".app/Contents/Info.plist")
  }
  )
tinsert(
  package.config["ReleaseVTK"].postbuildcommands, 
  {
  	"pwd",
  	string.format('%s%s%s%s%s',"rm -f -r -v ",os.getenv("JMU_BUILDS"), "/Release/bin/",package.name, ".app"),
  	string.format('%s%s%s%s%s',"mkdir ",os.getenv("JMU_BUILDS"), "/Release/bin/",package.name, ".app"),
  	string.format('%s%s%s%s%s',"mkdir ",os.getenv("JMU_BUILDS"), "/Release/bin/",package.name, ".app/Contents"),
  	string.format('%s%s%s%s%s',"mkdir ",os.getenv("JMU_BUILDS"), "/Release/bin/",package.name, ".app/Contents/MacOS"),
  	string.format('%s%s%s%s%s',"mkdir ",os.getenv("JMU_BUILDS"), "/Release/bin/",package.name, ".app/Contents/Resources"),
  	string.format('%s%s%s%s%s',"mkdir ",os.getenv("JMU_BUILDS"), "/Release/bin/",package.name, ".app/Contents/Resources/English.lproj"),
  	string.format('%s%s%s%s%s%s%s%s%s%s',"cp ",os.getenv("JMU_BUILDS"), "/Release/bin/",package.name," ",os.getenv("JMU_BUILDS"), "/Release/bin/", package.name, ".app/Contents/MacOS/", package.name),
  	string.format('%s%s%s%s%s%s%s%s%s',"cp ",package.name,".icns ",os.getenv("JMU_BUILDS"), "/Release/bin/", package.name, ".app/Contents/Resources/", package.name,".icns"),
  	string.format('%s%s%s%s%s',"cp Info.plist ",os.getenv("JMU_BUILDS"), "/Release/bin/", package.name, ".app/Contents/Info.plist")
  }
  )

end
---]]