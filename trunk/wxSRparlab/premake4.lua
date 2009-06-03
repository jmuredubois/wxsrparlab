--*	NOTES:
--*		- use the '/' slash for all paths.
--*****************************************************************************
solution "wxSRparlabPREM"
	configurations { "Debug", "Release" }

-- This is for including other Premake scripts.
--dofile( "wxSRparlabPREM_prem4.lua" )

-- Add options here.
newoption
{
	trigger = "dynamic-runtime",
	description = "Use the dynamicly loadable version of the runtime."
}

newoption
{
	trigger = "unicode",
	description = "Use the Unicode character set"
}

newoption
{
	trigger = "use-vtk",
	description = "Enable building against VTK(tm) for rendering"
}

if _OPTIONS["use-vtk"] then
	dofile( "wxSRparlab/wxSRparlabVTK_prem4.lua" )
else
	dofile( "wxSRparlab/wxSRparlab_prem4.lua" )
end

