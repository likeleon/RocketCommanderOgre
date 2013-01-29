#include "stdafx.h"
#include "TestGameWithLight.h"
#include "SmallAsteroid.h"
#include "BaseAsteroidManager.h"

using namespace RocketCommander;
using namespace Ogre;

BOOST_AUTO_TEST_SUITE(SmallAsteroidTest)

BOOST_AUTO_TEST_CASE(RenderTest)
{
	class RenderTestGame : public TestGameWithLight
	{
		typedef std::vector<boost::shared_ptr<SmallAsteroid> > SmallAsteroids;

		virtual void InitTest()
		{
			TestGameWithLight::InitTest();
			ShowCameraInfo();

			const float size = BaseAsteroidManager::SmallAsteroidSize;
			GetCamera().setPosition(Vector3(0, 0, -size * 2));
			GetCamera().lookAt(Vector3(0, 0, 0));

			m_smallAsteroids.push_back(boost::shared_ptr<SmallAsteroid>(new SmallAsteroid(*m_sceneMgr, 0, Vector3::ZERO)));
			m_smallAsteroids.push_back(boost::shared_ptr<SmallAsteroid>(new SmallAsteroid(*m_sceneMgr, 1, Vector3(size * 3, 0, 0))));
			m_smallAsteroids.push_back(boost::shared_ptr<SmallAsteroid>(new SmallAsteroid(*m_sceneMgr, 2, Vector3(0, size * 3, 0))));

			for (SmallAsteroids::iterator smallAsteroid = m_smallAsteroids.begin(); smallAsteroid != m_smallAsteroids.end(); ++smallAsteroid)
			{
				(*smallAsteroid)->SetSize(size);
			}
		}

		virtual void UpdateTest(double timeSinceLastFrame)
		{
			TestGameWithLight::UpdateTest(timeSinceLastFrame);
		}

		SmallAsteroids m_smallAsteroids;
	};
	
	RenderTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()
