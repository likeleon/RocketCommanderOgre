#include "stdafx.h"
#include "Level.h"
#include "Utils.h"
#include "Directories.h"

using namespace RocketCommander;

BOOST_AUTO_TEST_SUITE(LevelTest)

struct LevelData
{
	const std::string levelFile;
	const int length;
	const int width;

	LevelData(const std::string &levelFile, int length, int width)
	: levelFile(levelFile), length(length), width(width)
	{
	}
};

static const LevelData levelDatas[] = 
{
	LevelData(Directories::GetLevelsDirectory() + "/Easy Flight.png", 256, 20),
	LevelData(Directories::GetLevelsDirectory() + "/Lost Civilization.png", 512, 20),
	LevelData(Directories::GetLevelsDirectory() + "/Valley of Death.png", 1024, 20),
	LevelData(Directories::GetLevelsDirectory() + "/The Revenge.png", 1024, 20)
};

BOOST_AUTO_TEST_CASE(ConstructorTest)
{
	for (size_t i = 0; i < ArraySize(levelDatas); ++i)
	{
		const LevelData &levelData = levelDatas[i];
		Level level(levelData.levelFile);
		BOOST_CHECK_EQUAL(boost::filesystem::path(levelData.levelFile).replace_extension("").filename(), level.GetName());
		BOOST_CHECK_EQUAL(levelData.length, level.GetLength());
		BOOST_CHECK_EQUAL(levelData.width, level.GetWidth());
	}
}

BOOST_AUTO_TEST_CASE(LoadAllLevelsTest)
{
	const std::vector<Level> levels = Level::LoadAllLevels();
	BOOST_CHECK_EQUAL(ArraySize(levelDatas), levels.size());
	for (size_t i = 0; i < ArraySize(levelDatas); ++i)
	{
		BOOST_CHECK_EQUAL(boost::filesystem::path(levelDatas[i].levelFile).replace_extension("").filename(), levels[i].GetName());
	}
}

BOOST_AUTO_TEST_SUITE_END()
