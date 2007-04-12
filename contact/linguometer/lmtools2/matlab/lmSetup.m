path_pwd = pwd;
if (isunix())
	warning('off');
	path(path,[path_pwd '/include/tapadm/']);
	path(path,[path_pwd '/include/lmmatlab/']);
	path(path,[path_pwd '/include/lmstuff/']);
	path(path,[path_pwd '/include/mtmatlab/']);
	warning('on');
else
	warning('off');
	path(path,[path_pwd '\include\tapadm\']);
	path(path,[path_pwd '\include\lmmatlab\']);
	path(path,[path_pwd '\include\lmstuff/']);
	path(path,[path_pwd '\include\mtmatlab\']);
	warning('on');
end

