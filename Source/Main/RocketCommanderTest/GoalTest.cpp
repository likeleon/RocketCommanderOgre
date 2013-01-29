#include "stdafx.h"
#include "TestGameWithLight.h"
#include "Goal.h"

using namespace RocketCommander;
using namespace Ogre;

BOOST_AUTO_TEST_SUITE(GoalTest)

BOOST_AUTO_TEST_CASE(RenderTest)
{
	class RenderTestGame : public TestGameWithLight
	{
		virtual void InitTest()
		{
			TestGameWithLight::InitTest();
			ShowCameraInfo();

			GetCamera().setPosition(Vector3(0, 0, -5));
			GetCamera().lookAt(Vector3(0, 0, 0));

			m_goal.reset(new Goal(*m_sceneMgr, Vector3::ZERO));
		}

		boost::shared_ptr<Goal> m_goal;
	};

	RenderTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()