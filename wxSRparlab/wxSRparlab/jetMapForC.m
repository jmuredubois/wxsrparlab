function res = jetMapForC(nColors)

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
