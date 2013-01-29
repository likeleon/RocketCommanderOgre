#pragma once

#include "Level.h"
#include "BaseGame.h"

namespace RocketCommander
{
	class Asteroid;
	class SmallAsteroid;

	class BaseAsteroidManager
	{
	public:
		BaseAsteroidManager(BaseGame &m_game, const Level &level);
		virtual ~BaseAsteroidManager();

		static const float SectorWidth;
		static const float SectorHeight;
		static const float SectorDepth;

		static const float MinAsteroidSize;
		static const float MaxAsteroidSize;

		static const float SmallAsteroidSize;

		static float GetMinViewDepth();
		static float GetMaxViewDepth();
		static float GetHalfViewDepth();

		const Level& GetLevel() const { return m_currentLevel; }
		const Ogre::Vector3& GetTargetPosition() const { return m_targetPos; }
		virtual void SetLevel(const Level &level);

		void Update();

		static const int NumberOfSectors = 19;
		static const int MiddleSector = NumberOfSectors / 2;
		static const int MinSector = -MiddleSector;
		static const int MaxSector = +MiddleSector;

		static const int NumberOfSmallSectors = 7;
		static const int SmallMiddleSector = NumberOfSmallSectors / 2;
		static const int SmallSectorAdd = MiddleSector - SmallMiddleSector;
		static const int MinSmallSector = -SmallMiddleSector;
		static const int MaxSmallSector = +SmallMiddleSector;

		//
		// NOTE: These are only for tests. Do not use these interfaces in real code.
		//
		const std::vector<std::vector<bool> >& GetSectorIsVisible() const { return m_sectorIsVisible; }

	protected:
		virtual void HandleSectorPhysics(int checkX, int checkZ, int cameraSectorPosX, int cameraSectorPosZ) {}
		virtual void ShowAllItems() {}
		
		BaseGame		&m_game;
		Ogre::Vector3	m_targetPos;

		typedef std::list<Asteroid *>				Asteroids;
		std::vector<std::vector<Asteroids> >		m_sectorAsteroids;
		std::vector<std::vector<bool> >				m_sectorIsVisible;

		typedef std::list<SmallAsteroid *>			SmallAsteroids;
		std::vector<std::vector<SmallAsteroids> >	m_sectorSmallerAsteroids;

	private:
		static const float MinViewDepth;
		static float MaxViewDepth;
		static float HalfViewDepth;

		void GenerateSector(Asteroids &asteroids, int x, int z);
		void GenerateSmallerAsteroidsSector(SmallAsteroids &smallAsteroids, int numOfAsteroids, int x, int z);
		void CalculateSectors();
		void CalculateSectorMovement();
		void CalculateSectorVisibility();
		Ogre::Radian GetViewableFieldOfView() const;

		Level			m_currentLevel;
		std::vector<std::vector<bool> >				m_sectorVisibleInRange;
		std::vector<std::vector<Ogre::Vector3> >	m_sectorDirection;
		int m_centerSectorPositionX;
		int m_centerSectorPositionZ;
		int m_numAsteroidModels;
		int m_numSmallAsteroidModels;
	};
}