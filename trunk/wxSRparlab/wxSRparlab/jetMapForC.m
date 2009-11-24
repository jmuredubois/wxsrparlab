function res = jetMapForC(nColors)
% jetMapForC.m
%  helper function allowing to reproduce the colors of Matlab(r) jet colormap inorder to use them in a c program
%
%    Copyright 2009 James Mure-Dubois, Heinz Hügli and Institute of Microtechnology of EPFL.
%      http://parlab.epfl.ch/
%
%    This program is free software: you can redistribute it and/or modify
%    it under the terms of the GNU General Public License as published by
%    the Free Software Foundation, either version 3 of the License, or
%    (at your option) any later version.
%
%    This program is distributed in the hope that it will be useful,
%    but WITHOUT ANY WARRANTY; without even the implied warranty of
%    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%    GNU General Public License for more details.
%
%    You should have received a copy of the GNU General Public License
%    along with this program.  If not, see <http://www.gnu.org/licenses/>.
%
% $Rev$:
% $Author$:
% $Date$:

if( (exist('nColors')~=1) || (nColors<2))
    nColors = 256;
end;

cmap = [0 0 0; jet(nColors-1)];

%% export in C++ code
for m=1:1:nColors
    disp( sprintf('m_pLUT[%u] = %u;', 3*(m-1)+0, uint8(cmap(m,1)*255)));
    disp( sprintf('m_pLUT[%u] = %u;', 3*(m-1)+1, uint8(cmap(m,2)*255)));
    disp( sprintf('m_pLUT[%u] = %u;', 3*(m-1)+2, uint8(cmap(m,3)*255)));
end;
