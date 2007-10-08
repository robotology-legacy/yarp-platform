% Copyright (C) 2007 Michele Tavella <michele@liralab.it>
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program; if not, write to the Free Software
% Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

path_pwd = pwd;
if (isunix())
	warning('off');
	path(path,[path_pwd '/include/']);
	path(path,[path_pwd '/include/dtw']);
	path(path,[path_pwd '/include/dvdec']);
	path(path,[path_pwd '/include/lmmatlab/']);
	path(path,[path_pwd '/include/lmpackages']);
	path(path,[path_pwd '/include/lmstuff/']);
	path(path,[path_pwd '/include/mtdevel/']);
	path(path,[path_pwd '/include/mtmatlab/']);
	path(path,[path_pwd '/include/rastamat']);
	path(path,[path_pwd '/include/tapadm/']);
	path(path,[path_pwd '/include/voicebox']);
	warning('on');
else
	warning('off');
	path(path,[path_pwd '\include\']);
	path(path,[path_pwd '\include\dtw']);
	path(path,[path_pwd '\include\dvdec']);
	path(path,[path_pwd '\include\lmmatlab\']);
	path(path,[path_pwd '\include\lmpackages']);
	path(path,[path_pwd '\include\lmstuff/']);
	path(path,[path_pwd '\include\mtdevel\']);
	path(path,[path_pwd '\include\mtmatlab\']);
	path(path,[path_pwd '\include\rastamat']);
	path(path,[path_pwd '\include\tapadm\']);
	path(path,[path_pwd '\include\voicebox']);
	warning('on');
end
savepath
