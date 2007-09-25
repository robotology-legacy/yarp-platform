path_pwd = pwd;
if (isunix())
	warning('off');
	path(path,[path_pwd '/include/']);
	path(path,[path_pwd '/include/tapadm/']);
	path(path,[path_pwd '/include/lmmatlab/']);
	path(path,[path_pwd '/include/lmstuff/']);
	path(path,[path_pwd '/include/mtdevel/']);
	path(path,[path_pwd '/include/mtmatlab/']);
	path(path,[path_pwd '/include/lmpackages']);
	path(path,[path_pwd '/include/dtw']);
	path(path,[path_pwd '/include/dvdec']);
	path(path,[path_pwd '/include/rastamat']);
	warning('on');
else
	warning('off');
	path(path,[path_pwd '\include\']);
	path(path,[path_pwd '\include\tapadm\']);
	path(path,[path_pwd '\include\lmmatlab\']);
	path(path,[path_pwd '\include\lmstuff/']);
	path(path,[path_pwd '\include\mtmatlab\']);
	path(path,[path_pwd '\include\mtdevel\']);
	path(path,[path_pwd '\include\lmpackages']);
	path(path,[path_pwd '\include\dtw']);
	path(path,[path_pwd '\include\dvdec']);
	path(path,[path_pwd '\include\rastamat']);
	warning('on');
end
savepath
