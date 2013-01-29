#include "stdafx.h"
#include "TestGame.h"
#include "PhysicsAsteroidManager.h"
#include "Asteroid.h"

using namespace RocketCommander;
using namespace Ogre;

BOOST_AUTO_TEST_SUITE(PhysicsAsteroidManagerTest)

BOOST_AUTO_TEST_CASE(AsteroidPhysicsSmallSceneTest)
{
	class AsteroidPhysicsSmallSceneTestGame : public TestGame
	{
		class TestPhysicsAsteroidManager : public PhysicsAsteroidManager
		{
		public:
			TestPhysicsAsteroidManager(BaseGame &m_game, const Level &level)
			 : PhysicsAsteroidManager(m_game, level), m_sceneMgr(m_game.GetSceneManager())
			{
			}

			void SetupScene(int sceneNumber)
			{
				// Kill all
				for (std::vector<std::vector<Asteroids> >::iterator it1 = m_sectorAsteroids.begin(); it1 != m_sectorAsteroids.end(); ++it1)
				{
					for (std::vector<Asteroids>::iterator it2 = it1->begin(); it2 != it1->end(); ++it2)
					{
						for (Asteroids::iterator asteroid = it2->begin(); asteroid != it2->end(); ++asteroid)
						{
							delete *asteroid;
						}
						it2->clear();
					}
				}

				switch (sceneNumber)
				{
				case 0:
					// Just add 2 colliding asteroids
					AddAsteroid(Vector3(-50, 0, 0), 12.5f, Vector3(20, 0, 0));
					AddAsteroid(Vector3(+50, 0, 0), 12.5f, Vector3(-20, 0, 0));
					break;

				case 1:
					// Coolide fast moving asteroid with still one
					// Use same sizes and masses for 100% impulse transfer
					AddAsteroid(Vector3(-50, 0, 0), 12.5f, Vector3(40, 0, 0));
					AddAsteroid(Vector3(+50, 0, 0), 12.5f, Vector3(0, 0, 0));
					break;

				case 2:
					// Collide with carzy movement values
					AddAsteroid(Vector3(-35, +19, 8) * (-2), 12.5f, Vector3(-35, +19, 8));
					AddAsteroid(Vector3(+15, 40, 14) * (-2), 12.5f, Vector3(+15, 40, 14));
					break;

				case 3:
					// Collide with up/down values
					AddAsteroid(Vector3(0, 0, -50), 12.5f, Vector3(0, 0, 40));
					AddAsteroid(Vector3(0, 0, +70), 12.5f, Vector3(0, 0, -20));
					break;

				case 4:
					// Collide with up/down values
					AddAsteroid(Vector3(0, -50, 0), 12.5f, Vector3(0, 40, 0));
					AddAsteroid(Vector3(0, +70, 0), 12.5f, Vector3(0, -20, 0));
					break;

				case 5:
					// 2 colliding asteroids with different sizes
					AddAsteroid(Vector3(-50, 0, 0), 2.5f, Vector3(20, 0, 0));
					AddAsteroid(Vector3(+50, 0, 0), 12.5f, Vector3(-20, 0, 0));
					break;

				case 6:
					// 2 colliding asteroids with different sizes
					AddAsteroid(Vector3(-50, +100, 0), 2.5f, Vector3(20, -40, 0));
					AddAsteroid(Vector3(+50, +100, 0), 12.5f, Vector3(-20, -40, 0));
					break;
				}
			}

			void UpdateScene()
			{
				for (int z = 0; z < NumberOfSectors; ++z)
				{
					for (int x = 0; x < NumberOfSectors; ++x)
					{
						// Update all positions for this sector
						for (Asteroids::iterator asteroid = m_sectorAsteroids[z][x].begin(); asteroid != m_sectorAsteroids[z][x].end(); ++asteroid)
						{
							(*asteroid)->UpdateMovement(m_game.GetMoveFactorPerSecond());
						}

						HandleSectorPhysics(x, z, 0, 0);
					}
				}
			}

		private:
			void AddAsteroid(const Vector3 &pos, float size, const Vector3 &movement)
			{
				Asteroid *asteroid = new Asteroid(m_sceneMgr, 0, pos);
				asteroid->SetMovement(movement);
				asteroid->SetSize(size);
				asteroid->SetCollisionRadius(size * Asteroid::CollisionSizeCorrectionFactor);
				m_sectorAsteroids[MiddleSector][MiddleSector].push_back(asteroid);
			}

			SceneManager &m_sceneMgr;
		};

		virtual void InitTest()
		{
			EnableSkyBox();
			GetCamera().setPosition(0, 0, 50);
			SetLightDirection(GetCamera().getDirection());
			m_asteroidManager.reset(new TestPhysicsAsteroidManager(*this, Level::LoadAllLevels()[0]));

			m_asteroidManager->SetupScene(0);
		}

		virtual void UpdateTest(double timeSinceLastFrame)
		{
			m_asteroidManager->UpdateScene();
		}

		virtual bool keyPressed(const OIS::KeyEvent &evt)
		{
			if (evt.key >= OIS::KC_1 && evt.key <= OIS::KC_7)
			{
				m_asteroidManager->SetupScene(evt.key - OIS::KC_1);
			}
			return TestGame::keyPressed(evt);
		}

		boost::shared_ptr<TestPhysicsAsteroidManager> m_asteroidManager;
	};

	AsteroidPhysicsSmallSceneTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()
