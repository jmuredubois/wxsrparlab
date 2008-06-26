package.name = "wxSRparlabPREM"
package.guid = "F0174772-C16D-4CED-8E0E-3B1348DB3630"
package.kind = "exe"
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

package.buildflags = {"no-pch"}
package.defines = {""}
package.includepaths = {
  string.format('%s%s',os.getenv("WXWIN"), "/include"),
  string.format('%s%s',os.getenv("WXWIN"), "/include/msvc")
}
package.libpaths = {
  string.format('%s%s',os.getenv("WXWIN"), "/lib/vc_dl"),
  string.format('%s%s',os.getenv("WXWIN"), "/lib/vc_lib"),
}

tinsert(package.config["Release"].buildflags, {"optimize-speed"})

tinsert(package.config["Debug"].defines , "DEBUG")
tinsert(package.config["Release"].defines , "NDEBUG")