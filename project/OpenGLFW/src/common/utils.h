#pragma once

#include <string>
#include <direct.h>

static const char*s_pwd;

std::string getFullPath(const char *path)
{
	if (s_pwd == NULL)
	{
		s_pwd = _getcwd(NULL, 0);
		printf("work path:%s\n", s_pwd);
	}

	std::string fullPath(s_pwd);
	fullPath.append("/");
	fullPath.append(path);

	

	return fullPath;
}
