#include "stdafx.h"
#include "TestGameWithLight.h"
#include "Asteroid.h"
#include "BaseAsteroidManager.h"

using namespace RocketCommander;
using namespace Ogre;

BOOST_AUTO_TEST_SUITE(AsteroidTest)

BOOST_AUTO_TEST_CASE(RenderTest)
{
	class RenderTestGame : public TestGameWithLight
	{
		typedef std::vector<boost::shared_ptr<Asteroid> > Asteroids;

		virtual void InitTest()
		{
			TestGameWithLight::InitTest();
			ShowCameraInfo();

			const float asteroidSize = BaseAsteroidManager::MinAsteroidSize;
			GetCamera().setPosition(Vector3(0, 0, -asteroidSize * 2));
			GetCamera().lookAt(Vector3(0, 0, 0));

			m_asteroids.push_back(boost::shared_ptr<Asteroid>(new Asteroid(*m_sceneMgr, 0, Vector3::ZERO)));
			m_asteroids.push_back(boost::shared_ptr<Asteroid>(new Asteroid(*m_sceneMgr, 1, Vector3(asteroidSize * 3, 0, 0))));
			m_asteroids.push_back(boost::shared_ptr<Asteroid>(new Asteroid(*m_sceneMgr, 2, Vector3(0, asteroidSize * 3, 0))));
			m_asteroids.push_back(boost::shared_ptr<Asteroid>(new Asteroid(*m_sceneMgr, 3, Vector3(-asteroidSize * 3, 0, 0))));
			m_asteroids.push_back(boost::shared_ptr<Asteroid>(new Asteroid(*m_sceneMgr, 4, Vector3(0, -asteroidSize * 3, 0))));

			for (Asteroids::iterator asteroid = m_asteroids.begin(); asteroid != m_asteroids.end(); ++asteroid)
			{
				(*asteroid)->SetSize(asteroidSize);
			}
		}

		Asteroids m_asteroids;
	};
	
	RenderTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()
