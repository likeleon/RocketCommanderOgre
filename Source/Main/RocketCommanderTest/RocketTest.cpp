#include "stdafx.h"
#include "TestGameWithLight.h"
#include "Model.h"
#include "Rocket.h"

using namespace RocketCommander;
using namespace Ogre;

BOOST_AUTO_TEST_SUITE(RocketTest)

BOOST_AUTO_TEST_CASE(RenderTest)
{
	class RenderTestGame : public TestGameWithLight
	{
		virtual void InitTest()
		{
			TestGameWithLight::InitTest();

			// Create "rocket" model
			m_rocket.reset(new Rocket(*m_sceneMgr));
			m_rocket->SetPosition(Vector3(0, 0, 0));
		}
		boost::shared_ptr<Rocket> m_rocket;
	};

	RenderTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_CASE(InFrontOfCameraTest)
{
	class InFrontOfCameraTest : public TestGameWithLight
	{
		virtual void InitTest()
		{
			TestGameWithLight::InitTest();
			ShowCameraInfo();
		
			rocket.reset(new Rocket(*m_sceneMgr));
		}

		virtual void UpdateTest(double timeSinceLastFrame)
		{
			// Set position
			Vector3 inFrontOfCameraPos = Vector3(0, -1.33f, -2.5f);
			inFrontOfCameraPos = GetCamera().getViewMatrix().inverse() * inFrontOfCameraPos;
			rocket->SetPosition(inFrontOfCameraPos);

			// Set orientation
			Quaternion rocketOrient = GetCamera().getOrientation() *
				Quaternion(Radian(GetTotalTimeMs() / 8500.0f), Vector3::UNIT_Z) *
				Quaternion(-Radian(Math::PI / 2.0f), Vector3::UNIT_X);
			rocket->SetOrientation(rocketOrient);

			TestGameWithLight::UpdateTest(timeSinceLastFrame);
		}

		boost::shared_ptr<Rocket> rocket;
	};

	InFrontOfCameraTest game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()