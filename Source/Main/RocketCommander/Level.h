#pragma once

#include "Item.h"

namespace RocketCommander
{
	class Level
	{
	public:
		Level() {}
		Level(const std::string &fullFileName);

		const std::string& GetName() const { return m_name; }
		int GetLength() const { return m_length; }
		int GetWidth() const { return m_width; }
		float GetDensity(int x, int z) const { return m_density[x][z]; }
		const Ogre::ColourValue& GetSunColour(int levelPos) const { return m_sunColor[levelPos]; }

		typedef std::list<Ogre::Vector3> ItemPositions;
		typedef std::map<Item::ItemTypes, ItemPositions> Items;
		const Items& GetItems() const { return m_items; }

		static std::vector<Level> LoadAllLevels();

	private:
		static const int DefaultLevelWidth = 20;
		static const Ogre::ColourValue FuelItemColor;
		static const Ogre::ColourValue HealthItemColor;
		static const Ogre::ColourValue ExtraLifeItemColor;
		static const Ogre::ColourValue SpeedItemColor;
		static const Ogre::ColourValue BombItemColor;

		Ogre::Vector3 GenerateItemPosition(int xPos, int zPos);

		std::string m_name;
		int m_width;
		int m_length;
		std::vector<std::vector<float> > m_density;
		std::vector<Ogre::ColourValue> m_sunColor;
		Items m_items;
	};
}