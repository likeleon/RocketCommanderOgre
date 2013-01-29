#include "stdafx.h"
#include "Directories.h"
#include <shlobj.h>

namespace RocketCommander
{
	std::string Directories::m_homePath = "";
	bool Directories::m_homePathPrepared = false;

	std::string Directories::GetContentDirectory()
	{
		return "../Content";
	}

	std::string Directories::GetLevelsDirectory()
	{
		return GetContentDirectory() + "/Levels";
	}

	std::string Directories::GetMusicsDirectory()
	{
		return GetContentDirectory() + "/Sounds";
	}

	std::string Directories::GetWritablePath(const std::string &fileName)
	{
		if (m_homePathPrepared == false)
		{
			char path[MAX_PATH];
			if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL|CSIDL_FLAG_CREATE, NULL, 0, path)))
			{
				m_homePath = path;
				// create Ogre subdir
				m_homePath += "\\RocketCommander\\";
				if (!CreateDirectoryA(m_homePath.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
				{
					// couldn't create directory, fall back to current working dir
					m_homePath.clear();
				}
				else
				{
					const std::string subdir = "ver1_0";
					m_homePath += subdir + '\\';
					// create release subdir
					if (!CreateDirectoryA(m_homePath.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
					{
						// couldn't create directory, fall back to current working dir
						m_homePath.clear();
					}
				}
			}

			if (m_homePath.empty())
			{
				// couldn't create dir in home directory, fall back to cwd
				m_homePath = "";
			}

			m_homePathPrepared = true;
		}
		return m_homePath + fileName;
	}
}