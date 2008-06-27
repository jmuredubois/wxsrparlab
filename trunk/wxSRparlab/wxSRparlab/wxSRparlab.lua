package.name = "wxSRparlabPREM"
package.guid = "F0174772-C16D-4CED-8E0E-3B1348DB3630"
package.kind = "winexe"
package.language = "c++"

package.targetprefix = ""
--package.config["Debug"].target = string.format('%s%s%s',os.getenv("JMU_BUILDS"), "/Debug/bin",package.name)
--package.config["Release"].target = string.format('%s%s%s',os.getenv("JMU_BUILDS"), "/Release/bin",package.name)

package.config["Debug"].bindir = string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/bin")
package.config["Release"].bindir = string.format('%s%s',os.getenv("JMU_BUILDS"), "/Release/bin")
package.config["Debug"].objdir = string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/obj")
package.config["Release"].objdir = string.format('%s%s',os.getenv("JMU_BUILDS"), "/Release/obj")

package.files = {
  matchfiles("*.h", "*.cpp")
}
---[[
package.excludes = {
  "lutDef.h",
  "definesSR.h",
  "libusbSR.h",
  "wxSRvtkView.h",
  "wxSRvtkView.cpp",
  "camTranMats.cpp",
}
--]]

package.buildflags = {"no-pch","no-main"}
--package.defines = {""}
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
        string.format('%s%s',os.getenv("WXWIN"), "/lib/vc_lib"),
      }
	)
  end
end

tinsert(package.config["Release"].buildflags, {"optimize-speed"})


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
  tinsert(package.buildoptions, "`wx-config --cxxflags` -W -Wall -ansi")
  tinsert(package.linkoptions,  "`wx-config --libs`")
end

if (OS == "windows") then
  --tinsert(package.defines, { "WIN32", "_WINDOWS", "WINVER=0x400", "_MT", "wxUSE_GUI=1" })
  tinsert(package.links,   { "comctl32", "rpcrt4", "winmm", "advapi32", "wsock32"})

  --tinsert(package.config["Debug"].defines , "_DEBUG")
  tinsert(package.config["Release"].defines, "NDEBUG")

  --tinsert(package.defines, "WXUSINGDLL")
  tinsert(package.config["Debug"].links, {"wxmsw28d_core", "wxbase28d"})
  tinsert(package.config["Release"].links, {"wxmsw28_core", "wxbase28"})
end

---end

-- --http://wiki.wxwindows.org/WxMac_Issues
if (OS == "macosx") then
--  	string.format('%s%s',"cd ",string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/bin")
--),
  package.postbuildcommands = {
  	string.format('%s%s%s%s%s',"rm -f -r -v ",os.getenv("JMU_BUILDS"), "/Debug/bin",package.name, ".app"),
  	string.format('%s%s%s%s%s',"mkdir ",os.getenv("JMU_BUILDS"), "/Debug/bin/",package.name, ".app"),
  	string.format('%s%s%s%s%s',"mkdir ",os.getenv("JMU_BUILDS"), "/Debug/bin/",package.name, ".app/Contents"),
  	string.format('%s%s%s%s%s',"mkdir ",os.getenv("JMU_BUILDS"), "/Debug/bin/",package.name, ".app/Contents/MacOS"),
  	string.format('%s%s%s%s%s',"mkdir ",os.getenv("JMU_BUILDS"), "/Debug/bin/",package.name, ".app/Contents/Resources"),
  	string.format('%s%s%s%s%s',"mkdir ",os.getenv("JMU_BUILDS"), "/Debug/bin/",package.name, ".app/Contents/Resources/English.lproj"),
  	string.format('%s%s%s%s%s%s%s%s%s%s',"cp ",os.getenv("JMU_BUILDS"), "/Debug/bin/",package.name," ",os.getenv("JMU_BUILDS"), "/Debug/bin/", package.name, ".app/Contents/MacOS/", package.name)
  	}
end
