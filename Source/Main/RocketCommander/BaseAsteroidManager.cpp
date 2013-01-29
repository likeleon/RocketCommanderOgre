#include "stdafx.h"
#include "BaseAsteroidManager.h"
#include "Directories.h"
#include "RandomHelper.h"
#include "MathHelper.h"
#include "ColourHelper.h"
#include "LensFlare.h"
#include "Player.h"
#include "Asteroid.h"
#include "SmallAsteroid.h"

using namespace Ogre;

namespace RocketCommander
{
	const float BaseAsteroidManager::SectorWidth = 200.0f;
	const float BaseAsteroidManager::SectorHeight = 200.0f;
	const float BaseAsteroidManager::SectorDepth = 200.0f;

	const float BaseAsteroidManager::MinAsteroidSize = 32.0f;
	const float BaseAsteroidManager::MaxAsteroidSize = 62.0f;

	const float BaseAsteroidManager::SmallAsteroidSize = 129.0f;

	const float BaseAsteroidManager::MinViewDepth = 0.33f;
	float BaseAsteroidManager::MaxViewDepth = -1.0f;
	float BaseAsteroidManager::HalfViewDepth = -1.0f;

	float BaseAsteroidManager::GetMinViewDepth()
	{
		return MinViewDepth;
	}
	
	float BaseAsteroidManager::GetMaxViewDepth()
	{
		if (MaxViewDepth == -1.0f)
		{
			MaxViewDepth = SectorDepth * NumberOfSectors;// - SectorDepth / 2.0f;
		}
		return MaxViewDepth;
	}

	float BaseAsteroidManager::GetHalfViewDepth()
	{
		if (HalfViewDepth == -1.0f)
		{
			HalfViewDepth = GetMaxViewDepth() * 0.4f;
		}
		return HalfViewDepth;
	}

	BaseAsteroidManager::BaseAsteroidManager(BaseGame &game, const Level &level)
		: m_game(game),
		  m_targetPos(0, 0, 10000),
		  m_sectorAsteroids(NumberOfSectors, std::vector<Asteroids>(NumberOfSectors)),
		  m_sectorIsVisible(NumberOfSectors, std::vector<bool>(NumberOfSectors, false)),
		  m_sectorSmallerAsteroids(NumberOfSmallSectors, std::vector<SmallAsteroids>(NumberOfSmallSectors)),
		  m_sectorVisibleInRange(NumberOfSectors, std::vector<bool>(NumberOfSectors)),
		  m_sectorDirection(NumberOfSectors, std::vector<Ogre::Vector3>(NumberOfSectors, Ogre::Vector3::ZERO)),
		  m_centerSectorPositionX(0),
		  m_centerSectorPositionZ(0),
		  m_numAsteroidModels(0),
		  m_numSmallAsteroidModels(0)
	{
		// Set level
		SetLevel(level);

		// Load all available asteroids
		while (1)
		{
			const std::string meshName = "asteroid" + StringConverter::toString(m_numAsteroidModels + 1);
			const std::string meshFile = meshName + ".mesh";
			const std::string lowMeshFile = meshName + "Low.mesh";
			const std::string meshFilePath = Directories::GetContentDirectory() + "/" + meshName + ".mesh";

			if (!boost::filesystem::exists(meshFilePath))
				break;

			MeshPtr meshPtr = MeshManager::getSingleton().load(meshFile, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			MeshPtr lowMeshPtr = MeshManager::getSingleton().load(lowMeshFile, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			meshPtr->createManualLodLevel(GetHalfViewDepth(), lowMeshPtr->getName());

			m_numAsteroidModels++;
		}

		if (m_numAsteroidModels == 0)
			throw new std::exception("Unable to start game, no asteroid meshes were found,"
				"please check the content directory!");

		// Load all smaller asteroids
		while (1)
		{
			const std::string meshName = "SmallAsteroid" + StringConverter::toString(m_numSmallAsteroidModels + 1);
			const std::string meshFilePath = Directories::GetContentDirectory() + "/" + meshName + ".mesh";
			if (!boost::filesystem::exists(meshFilePath))
				break;
			m_numSmallAsteroidModels++;
		}

		// Create all asteroids
		for (int z = MinSector; z <= MaxSector; ++z)
		{
			for (int x = MinSector; x <= MaxSector; ++x)
			{
				const int iz = z + MiddleSector;
				const int ix = x + MiddleSector;
				GenerateSector(m_sectorAsteroids[iz][ix], x, z);
			}
		}

		// Create smaller asteroids
		for (int z = MinSmallSector; z <= MaxSmallSector; ++z)
		{
			for (int x = MinSmallSector; x <= MaxSmallSector; ++x)
			{
				const int iz = z + SmallMiddleSector;
				const int ix = x + SmallMiddleSector;
				GenerateSmallerAsteroidsSector(m_sectorSmallerAsteroids[iz][ix],
					m_sectorAsteroids[iz + SmallSectorAdd][ix + SmallSectorAdd].size(), x, z);
			}
		}

		// Precalculate visible sector stuff
		for (int z = MinSector; z <= MaxSector; ++z)
		{
			for (int x = MinSector; x <= MaxSector; ++x)
			{
				const int iz = z + MiddleSector;
				const int ix = x + MiddleSector;

				// Check if distance is smaller than the max view depth and
				// add a small offset to include nearly visible sectors.
				m_sectorVisibleInRange[iz][ix] = Math::Sqrt((Real)(x * x + z * z)) < MiddleSector + 0.25f;

				// Calculate direction (just normalize relative position)
				m_sectorDirection[iz][ix] = Ogre::Vector3((Real)x, 0, (Real)z);
				m_sectorDirection[iz][ix].normalise();
			}
		}

		// Calculate sectors and visibility
		CalculateSectors();
	}

	BaseAsteroidManager::~BaseAsteroidManager()
	{
		for (std::vector<std::vector<Asteroids> >::iterator it1 = m_sectorAsteroids.begin(); it1 != m_sectorAsteroids.end(); ++it1)
		{
			for (std::vector<Asteroids>::iterator it2 = it1->begin(); it2 != it1->end(); ++it2)
			{
				for (Asteroids::iterator asteroid = it2->begin(); asteroid != it2->end(); ++asteroid)
				{
					delete *asteroid;
				}
			}
		}

		for (std::vector<std::vector<SmallAsteroids> >::iterator it1 = m_sectorSmallerAsteroids.begin(); it1 != m_sectorSmallerAsteroids.end(); ++it1)
		{
			for (std::vector<SmallAsteroids>::iterator it2 = it1->begin(); it2 != it1->end(); ++it2)
			{
				for (SmallAsteroids::iterator smallAsteroid = it2->begin(); smallAsteroid != it2->end(); ++smallAsteroid)
				{
					delete *smallAsteroid;
				}
			}
		}
	}

	void BaseAsteroidManager::SetLevel(const Level &level)
	{
		m_currentLevel = level;
		
		// Update target position
		m_targetPos = Vector3(0, 0, (level.GetLength() + 1) * SectorDepth);
	}

	void BaseAsteroidManager::GenerateSector(Asteroids &asteroids, int x, int z)
	{
		// Make sure there are no asteroids in sector 0, 0, 0 and the surrounding sectors
		if (std::abs(x) < 2 && std::abs(z) < 2)
			return;

		// Check out how much asteroids we got in this sector.
		// If this is not a valid sector, use a medium density.
		float density = 0.1f;
		const int levelX = x + m_currentLevel.GetWidth() / 2;
		const int levelZ = z;
		if (levelX >= 0 && levelX < m_currentLevel.GetWidth() &&
			levelZ >= 0 && levelZ < m_currentLevel.GetLength())
		{
			density += m_currentLevel.GetDensity(levelX, levelZ);
		}

		const int numOfAsteroids = RandomHelper::GetRandomInt(0, (int)(2 + density * 10));

		for (int num = 0; num < numOfAsteroids; ++num)
		{
			const int type = RandomHelper::GetRandomInt(0, m_numAsteroidModels - 1);
			Vector3 position(x * SectorWidth, 
				m_game.GetCamera().getPosition().y + Math::RangeRandom(-SectorWidth * 3.15f, +SectorWidth * 3.15f),
				z * SectorDepth);
			position += Vector3(RandomHelper::GetRandomVector3(-SectorWidth * 0.42f, +SectorWidth * 0.42f));
			asteroids.push_back(new Asteroid(m_game.GetSceneManager(), type, position));
		}
	}

	void BaseAsteroidManager::GenerateSmallerAsteroidsSector(SmallAsteroids &smallAsteroids, int numOfAsteroids, int x, int z)
	{
		// Always create at least 1 smaller asteroid instance per sector
		const int numOfSmallerAsteroids = 2 + RandomHelper::GetRandomInt(0, 4 + numOfAsteroids);

		for (int num = 0; num < numOfSmallerAsteroids; ++num)
		{
			const int type = RandomHelper::GetRandomInt(0, m_numSmallAsteroidModels - 1);
			Vector3 position(x * SectorWidth, 0, z * SectorDepth);
			position += Vector3(Math::RangeRandom(-SectorWidth / 2, +SectorWidth / 2),
				Math::RangeRandom(-SectorWidth * 2.1f, +SectorWidth * 2.1f),
				Math::RangeRandom(-SectorWidth / 2, +SectorWidth / 2));
			smallAsteroids.push_back(new SmallAsteroid(m_game.GetSceneManager(), type, position));
		}
	}

	void BaseAsteroidManager::CalculateSectors()
	{
		CalculateSectorMovement();
		CalculateSectorVisibility();
	}

	void BaseAsteroidManager::CalculateSectorMovement()
	{
		const Vector3 &cameraPos = m_game.GetCamera().getPosition();
		const int cameraSectorPosX = (int)MathHelper::Round(cameraPos.x / SectorWidth);
		const int cameraSectorPosZ = (int)MathHelper::Round(cameraPos.z / SectorWidth);

		// Changed sector?
		if (m_centerSectorPositionX != cameraSectorPosX || m_centerSectorPositionZ != cameraSectorPosZ)
		{
			// Move sectors over, check out how much we are moving
			const int movedXSectors = cameraSectorPosX - m_centerSectorPositionX;
			const int movedZSectors = cameraSectorPosZ - m_centerSectorPositionZ;

			//
			// Normal asteroids
			//
			std::vector<std::vector<Asteroids> > helperCopyAsteroids(m_sectorAsteroids);
			std::vector<std::vector<bool> > helperCopyUsed(NumberOfSectors, std::vector<bool>(NumberOfSectors, false));

			for (int z = 0; z < NumberOfSectors; ++z)
			{
				for (int x = 0; x < NumberOfSectors; ++x)
				{
					if (x >= -movedXSectors && x < NumberOfSectors - movedXSectors &&
						z >= -movedZSectors && z < NumberOfSectors - movedZSectors)
					{
						m_sectorAsteroids[z][x] = helperCopyAsteroids[z + movedZSectors][x + movedXSectors];
						helperCopyUsed[z + movedZSectors][x + movedXSectors] = true;
					}
					else
					{
						m_sectorAsteroids[z][x].clear();
						GenerateSector(m_sectorAsteroids[z][x], 
							x - MiddleSector + cameraSectorPosX, 
							z - MiddleSector + cameraSectorPosZ);
					}
				}
			}

			for (int z = 0; z < NumberOfSectors; ++z)
			{
				for (int x = 0; x < NumberOfSectors; ++x)
				{
					if (helperCopyUsed[z][x])
						continue;

					for (Asteroids::iterator asteroid = helperCopyAsteroids[z][x].begin(); asteroid != helperCopyAsteroids[z][x].end(); ++asteroid)
					{
						delete *asteroid;
					}
				}
			}

			//
			// Smaller asteroids
			//
			std::vector<std::vector<SmallAsteroids> > helperCopySmallerAsteroids(m_sectorSmallerAsteroids);
			std::vector<std::vector<bool> > helperCopyUsedSmaller(NumberOfSmallSectors, std::vector<bool>(NumberOfSmallSectors, false));

			for (int z = 0; z < NumberOfSmallSectors; ++z)
			{
				for (int x = 0; x < NumberOfSmallSectors; ++x)
				{
					if (x >= -movedXSectors && x < NumberOfSmallSectors - movedXSectors &&
						z >= -movedZSectors && z < NumberOfSmallSectors - movedZSectors)
					{
						m_sectorSmallerAsteroids[z][x] = helperCopySmallerAsteroids[z + movedZSectors][x + movedXSectors];
						helperCopyUsedSmaller[z + movedZSectors][x + movedXSectors] = true;
					}
					else
					{
						m_sectorSmallerAsteroids[z][x].clear();
						GenerateSmallerAsteroidsSector(m_sectorSmallerAsteroids[z][x],
							m_sectorAsteroids[z + SmallSectorAdd][x + SmallSectorAdd].size(),
							x - SmallMiddleSector + cameraSectorPosX,
							z - SmallMiddleSector + cameraSectorPosZ);
					}
				}
			}

			for (int z = 0; z < NumberOfSmallSectors; ++z)
			{
				for (int x = 0; x < NumberOfSmallSectors; ++x)
				{
					if (helperCopyUsedSmaller[z][x])
						continue;

					for (SmallAsteroids::iterator smallAsteroid = helperCopySmallerAsteroids[z][x].begin(); smallAsteroid != helperCopySmallerAsteroids[z][x].end(); ++smallAsteroid)
					{
						delete *smallAsteroid;
					}
				}
			}

			m_centerSectorPositionX = cameraSectorPosX;
			m_centerSectorPositionZ = cameraSectorPosZ;
		}
	}

	void BaseAsteroidManager::CalculateSectorVisibility()
	{
		for (int z = MinSector; z <= MaxSector; ++z)
		{
			for (int x = MinSector; x <= MaxSector; ++x)
			{
				const int iz = z + MiddleSector;
				const int ix = x + MiddleSector;

				bool isVisible = m_sectorVisibleInRange[iz][ix];

				// Allow all if looking up or down now.
				Vector3 camDir = m_game.GetCamera().getDirection();
				if (isVisible && std::abs(camDir.y) < 0.75f)
				{
					Radian angle = camDir.angleBetween(m_sectorDirection[iz][ix]);
					isVisible = camDir.angleBetween(m_sectorDirection[iz][ix]) < GetViewableFieldOfView();
				}

				if (std::abs(x) + std::abs(z) <= 2)
					isVisible = true;

				m_sectorIsVisible[iz][ix] = isVisible;
			}
		}
	}

	void BaseAsteroidManager::Update()
	{
		//
		// Initialize and cacluate sectors
		//

		//Get current sector we are in.
		const Vector3 &cameraPos = m_game.GetCamera().getPosition();
		const int cameraSectorPosX = (int)MathHelper::Round(cameraPos.x / SectorWidth);
		const int cameraSectorPosZ = (int)MathHelper::Round(cameraPos.z / SectorWidth);

		CalculateSectors();

		//
		// Prepare level position and colors
		//

		// Get level position for sun color
		int levelPos = cameraSectorPosZ;
		if (levelPos < 0)
			levelPos = 0;
		if (levelPos >= m_currentLevel.GetLength())
			levelPos = m_currentLevel.GetLength() - 1;

		// Update sun color, also for lighting and lens flare.
		const ColourValue sunColour = ColourHelper::InterpolateColour(ColourValue::White, m_currentLevel.GetSunColour(levelPos), 0.75f);
		m_game.SetLensFlareColour(sunColour);

		// Set colour to lighting
		//m_game.SetLightColour(sunColour);
		m_game.SetLightColour(ColourValue::White);

		// Also update sun position
		const Vector3 sunPos = LensFlare::RotateSun(Radian(Math::PI / 4.0f + m_game.GetPlayer().GetGameTimeMs() / 50000.0f));
		LensFlare::SetOrigin3D(sunPos);
		m_game.SetLightDirection(-sunPos);

		//
		// Update asteroids
		//
		for (int z = 0; z < NumberOfSectors; ++z)
		{
			for (int x = 0; x < NumberOfSectors; ++x)
			{
				if (!m_sectorIsVisible[z][x])
					continue;

				// Update rotation and movement for all asteroids
				for (Asteroids::iterator asteroid = m_sectorAsteroids[z][x].begin(); asteroid != m_sectorAsteroids[z][x].end(); ++asteroid)
				{
					// Update rotation and position
					(*asteroid)->UpdateMovement(m_game.GetMoveFactorPerSecond());
				}

				if ((z + x) % 2 == m_game.GetTotalFrames() % 2)
					HandleSectorPhysics(x, z, cameraSectorPosX, cameraSectorPosZ);
			}
		}

		// Show all items
		ShowAllItems();
	}

	Radian BaseAsteroidManager::GetViewableFieldOfView() const
	{
		return BaseGame::FieldOfView * 0.75f;
	}
}