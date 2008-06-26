package.name = "wxSRparlabPREM"
package.guid = "F0174772-C16D-4CED-8E0E-3B1348DB3630"
package.kind = "winexe"
package.language = "c++"

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
package.includepaths = {
  string.format('%s%s',os.getenv("WXWIN"), "/include"),
  string.format('%s%s',os.getenv("WXWIN"), "/include/msvc")
}
package.libpaths = {
  string.format('%s%s',os.getenv("WXWIN"), "/lib/vc_dll"),
  string.format('%s%s',os.getenv("WXWIN"), "/lib/vc_lib"),
}

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
