#pragma once

namespace RocketCommander
{
	class Directories
	{
	public:
		static std::string GetLevelsDirectory();
		static std::string GetContentDirectory();
		static std::string GetMusicsDirectory();
		static std::string GetWritablePath(const std::string &fileName);

	private:
		static std::string m_homePath;
		static bool m_homePathPrepared;
	};
}