#include "stdafx.h"
#include "SoundManager.h"
#include "TestGameWithLight.h"

BOOST_AUTO_TEST_SUITE(SoundManagerTest)

using namespace RocketCommander;

BOOST_AUTO_TEST_CASE(PlaySoundsTest)
{
	class PlaySoundsTestGame : public TestGameWithLight
	{
	public:
		virtual ~PlaySoundsTestGame()
		{
			delete m_soundMgr;
		}

	private:
		virtual void InitTest()
		{
			TestGameWithLight::InitTest();

			m_soundMgr = new SoundManager();
			m_soundMgr->Initialize();

			const int soundIndex = m_soundMgr->CreateLoopedStream(Ogre::String("Rocket Commander Menu Music.wav"));
			m_soundMgr->PlaySound(soundIndex, NULL, NULL);
		}

		virtual bool keyPressed(const OIS::KeyEvent &evt)
		{
			if (evt.key == OIS::KC_1)
			{
				const int soundIndex = m_soundMgr->CreateStream(Ogre::String("Whosh1.wav"));
				m_soundMgr->PlaySound(soundIndex, NULL, NULL);
			}
			return TestGameWithLight::keyPressed(evt);
		}

		virtual void UpdateTest(double timeSinceLastFrame)
		{
			TestGameWithLight::UpdateTest(timeSinceLastFrame);
			m_soundMgr->Update();
		}

		SoundManager *m_soundMgr;
	};

	PlaySoundsTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()
