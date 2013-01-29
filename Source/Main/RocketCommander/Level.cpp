#include "stdafx.h"
#include "Level.h"
#include "GameAsteroidManager.h"
#include "RandomHelper.h"
#include "Directories.h"
#include "Utils.h"

using namespace Ogre;

namespace RocketCommander
{
	const ColourValue Level::FuelItemColor			= ColourValue(1.0f, 1.0f, 0.0f);
	const ColourValue Level::HealthItemColor		= ColourValue(0.0f, 1.0f, 0.0f);
	const ColourValue Level::ExtraLifeItemColor		= ColourValue(1.0f, 0.0f, 1.0f);
	const ColourValue Level::SpeedItemColor			= ColourValue(0.0f, 0.0f, 1.0f);
	const ColourValue Level::BombItemColor			= ColourValue(1.0f, 0.0f, 0.0f);

	Level::Level(const std::string &fullFileName)
		: m_name(boost::filesystem::path(fullFileName).replace_extension("").filename()),
		m_width(0), m_length(0)
	{
		// Load level
		const std::string fileName = boost::filesystem::path(fullFileName).replace_extension("level").file_string();
		std::ifstream file(fileName.c_str(), std::ios::binary);
		assert(file && file.is_open());

		file.read((char *)&m_width, sizeof(m_width));
		int height = 0;
		file.read((char *)&height, sizeof(height));
		std::vector<std::vector<ColourValue> > levelColors(m_width, std::vector<ColourValue>(height, ColourValue::Black));
		for (int x = 0; x < m_width; ++x)
		{
			for (int y = 0; y < height; ++y)
			{
				unsigned char r, g, b;
				file.read((char *)&r, sizeof(r)).read((char *)&g, sizeof(g)).read((char *)&b, sizeof(b));
				levelColors[x][y] = ColourValue(r / 255.0f, g / 255.0f, b / 255.0f);
			}
		}
		file.close();

		// Copy over level height as length, but always use 20 as the levelWidth.
		// We will use all pixels at 0-39 and scale them down to 0-19.
		m_width = DefaultLevelWidth;
		m_length = height;

		// Initialize density array, used to generate sectors
		m_density.resize(m_width, std::vector<float>(m_length));

		// Also load sun colors
		m_sunColor.resize(m_length);

		// Remember last density value in case we hit a item color,
		// this way we can set both the density and put the item into its list.
		float lastDensityValue = 0.0f;

		// Load everyting in as 0-1 density values
		for (int y = 0; y < m_length; ++y)
		{
			// Use inverted y position for the level, we start at the bottom.
			const int yPos = m_length - (y + 1);

			// Load level data
			for (int x = 0; x < m_width; ++x)
			{
				const ColourValue loadedColor = levelColors[(m_width * 2 - 1) - (x * 2)][y];
				const ColourValue loadedColor2 = levelColors[(m_width * 2 - 1) - (x * 2 + 1)][y];

				float densityValue = lastDensityValue;

				// Check if it is any item color
				if (loadedColor == FuelItemColor ||	loadedColor2 == FuelItemColor)
					m_items[Item::ItemType_Fuel].push_back(GenerateItemPosition(x, yPos));
				else if (loadedColor == HealthItemColor || loadedColor2 == HealthItemColor)
					m_items[Item::ItemType_Health].push_back(GenerateItemPosition(x, yPos));
				else if (loadedColor == ExtraLifeItemColor || loadedColor2 == ExtraLifeItemColor)
					m_items[Item::ItemType_ExtraLife].push_back(GenerateItemPosition(x, yPos));
				else if (loadedColor == SpeedItemColor || loadedColor2 == SpeedItemColor)
					m_items[Item::ItemType_Speed].push_back(GenerateItemPosition(x, yPos));
				else if (loadedColor == BombItemColor || loadedColor2 == BombItemColor)
					m_items[Item::ItemType_Bomb].push_back(GenerateItemPosition(x, yPos));
				else
					// Just use the red component, should be a gray value anyway.
					densityValue = loadedColor.r;

				// Apply density
				m_density[x][yPos] = densityValue;
			}

			// Load sun color at this level position (just read pixel at x pos 50)
			m_sunColor[yPos] = levelColors[50][y];
		}

		// Finished!
	}

	Vector3 Level::GenerateItemPosition(int xPos, int zPos)
	{
		// First generate sector position (randomize y a bit)
		const Real x = (xPos - m_width / 2) * GameAsteroidManager::SectorWidth;
		const Real y = Ogre::Math::RangeRandom(
			-GameAsteroidManager::SectorHeight * 1.8f,
			+GameAsteroidManager::SectorHeight * 1.9f);
		const Real z = GameAsteroidManager::SectorDepth * zPos;

		const Vector3 randomness = RandomHelper::GetRandomVector3(
			-GameAsteroidManager::SectorWidth / 2,
			+GameAsteroidManager::SectorWidth  / 2);

		return Vector3(x, y, z) + randomness;
	}

	std::vector<Level> Level::LoadAllLevels()
	{
		std::vector<Level> levels;

		const std::string defaultMaps[] = 
		{
			Directories::GetLevelsDirectory() + "/Easy Flight.png",
			Directories::GetLevelsDirectory() + "/Lost Civilization.png",
			Directories::GetLevelsDirectory() + "/Valley of Death.png",
			Directories::GetLevelsDirectory() + "/The Revenge.png",
		};

		for (size_t i = 0; i < ArraySize(defaultMaps); ++i)
			levels.push_back(Level(defaultMaps[i]));
		return levels;
	}
}