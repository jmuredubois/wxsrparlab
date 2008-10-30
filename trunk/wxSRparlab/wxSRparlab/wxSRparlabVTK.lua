package.name = "wxSRparlabVTKPREM"
package.guid = "F0174772-C16D-4CED-8E0E-3B1348DB3631"
package.kind = "winexe"
package.language = "c++"

package.configs = {"DebugVTK", "ReleaseVTK"}

package.targetprefix = ""
--package.config["DebugVTK"].target = string.format('/%s%s%s',os.getenv("JMU_BUILDS"), "/Debug/bin",package.name)
--package.config["ReleaseVTK"].target = string.format('%s%s%s',os.getenv("JMU_BUILDS"), "/Release/bin",package.name)

package.config["DebugVTK"].bindir = string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/bin")
package.config["ReleaseVTK"].bindir = string.format('%s%s',os.getenv("JMU_BUILDS"), "/Release/bin")
package.config["DebugVTK"].objdir = string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/obj")
package.config["ReleaseVTK"].objdir = string.format('%s%s',os.getenv("JMU_BUILDS"), "/Release/obj")

package.files = {
  matchfiles("*.h", "*.cpp")
}
---[[
package.excludes = {
  "lutDef.h",
  "definesSR.h",
  "libusbSR.h",
  "camTranMats.cpp",
  "CamScattering.h",
  "CamScattering.cpp",
  "CamFlagNaN.h",
  "CamFlagNaN.cpp",
  "srBuf.h",
  "imgPadder.h",
  "imgPadder.cpp",
  "myConvolveHfloat.h",
  "myConvolveHfloat.cpp"
}
--]]

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
        string.format('%s%s',os.getenv("WXWIN"), "/lib/vc_dll"),
        string.format('%s%s',os.getenv("WXWIN"), "/lib/vc_lib")
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

--package includes for TICPP
tinsert(package.config["DebugVTK"].links,   { "ticppd"})
tinsert(package.config["ReleaseVTK"].links,   { "ticpp"})
tinsert(package.includepaths,
  {
    string.format('%s',os.getenv("JMU_TICPP"))
  }
)
tinsert( package.libpaths, 
  {
    string.format('%s%s',os.getenv("JMU_TICPP"), "/lib")
  }
)
--END OF package includes for TICPP

--package includes for FFTW
if (OS == "windows") then
  tinsert(package.config["DebugVTK"].links,   { "libfftw3-3"})
  tinsert(package.config["ReleaseVTK"].links,   { "libfftw3-3"})
  tinsert(package.includepaths,
  {
    string.format('%s',os.getenv("JMU_FFTW3"))
  }
  )
  tinsert( package.libpaths, 
  {
    string.format('%s',os.getenv("JMU_FFTW3"))
  }
  )
  --tinsert(package.config["DebugVTK"].links,   { "libSRPLscat"})
  --tinsert(package.config["ReleaseVTK"].links,   { "libSRPLscat"})
  tinsert(package.includepaths,
  {
	string.format('%s%s',os.getenv("JMU_SVNSANDBOX_TRUNK"), "/libSRPLscat")
  }
  )
  tinsert( package.config["DebugVTK"].libpaths, 
  {
    string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/lib")
  }
  tinsert( package.config["ReleaseVTK"].libpaths, 
  {
    string.format('%s%s',os.getenv("JMU_BUILDS"), "/Release/lib")
  }
  )
end
if (OS == "macosx") then
  tinsert(package.config["DebugVTK"].links,   { "libfftw3"})
  tinsert(package.config["ReleaseVTK"].links,   { "libfftw3"})
  tinsert(package.includepaths,
  {
    string.format('%s%s',os.getenv("JMU_FFTW3"), "/include")
  }
  )
  tinsert( package.libpaths, 
  {
    string.format('%s%s',os.getenv("JMU_FFTW3"), "/lib")
  }
  )
end


--END OF package includes for FFTW

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