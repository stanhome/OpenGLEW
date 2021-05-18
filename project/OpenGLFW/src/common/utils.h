#pragma once

#include <string>

#ifdef __APPLE__
#include <unistd.h>
#else
#include <direct.h>
//char* (&getcwd)(char* _DstBuf, int _SizeInBytes) = _getcwd;
#endif

static const char*s_pwd;

static std::string getFullPath(const char *path)
{
	if (s_pwd == NULL)
	{
#ifdef __APPLE__
		s_pwd = getcwd(NULL, 0);
#else
		s_pwd = _getcwd(NULL, 0);
#endif
		printf("work path:%s\n", s_pwd);
	}

	std::string fullPath(s_pwd);
	fullPath.append("/");
	fullPath.append(path);

	return fullPath;
}
