project.name = "wxSRparlabPREM"
project.guid = "8BC9CEB8-8B4A-11D0-8D11-00A0C91BC940"

addoption("use-vtk", "Enable building against VTK(tm) for rendering")

if (options["use-vtk"]) then
	project.configs = {"DebugVTK", "ReleaseVTK"}
		--tinsert(project.config,   {"DebugVTK", "ReleaseVTK"})
	project.config["DebugVTK"].bindir = string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/bin")
	project.config["ReleaseVTK"].bindir = string.format('%s%s',os.getenv("JMU_BUILDS"), "/Release/bin")

	dopackage("wxSRparlab/wxSRparlabVTK")
else
	project.config["Debug"].bindir = string.format('%s%s',os.getenv("JMU_BUILDS"), "/Debug/bin")
	project.config["Release"].bindir = string.format('%s%s',os.getenv("JMU_BUILDS"), "/Release/bin")

	dopackage("wxSRparlab/wxSRparlab")	
end
