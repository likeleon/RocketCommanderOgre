#include "stdafx.h"
#include "Sound.h"
#include "TestGame.h"

BOOST_AUTO_TEST_SUITE(SoundTest)

using namespace RocketCommander;

BOOST_AUTO_TEST_CASE(PlaySoundsTest)
{
	class PlaySoundsTestGame : public TestGame
	{
		virtual void InitTest()
		{
			m_nowSound = Sound::Sounds_Bomb;

			m_leftTopDebug->setText("1: Play game music\n"
				"2: Play menu music\n"
				"3: Stop music\n"
				"4: Play next sound\n"
				"5: Play rocket motor sound\n"
				"6: Stop rocket motor sound\n"
				"7: Play whosh sound\n");
		}

		virtual bool keyPressed(const OIS::KeyEvent &evt)
		{
			if (evt.key == OIS::KC_1)
				m_sound->Play(Sound::Sounds_GameMusic);
			else if (evt.key == OIS::KC_2)
				m_sound->Play(Sound::Sounds_MenuMusic);
			else if (evt.key == OIS::KC_3)
				m_sound->StopLooped();
			else if (evt.key == OIS::KC_4)
			{
				while (1)
				{
					m_nowSound = (++m_nowSound) % Sound::MaxSounds;
					if (m_nowSound != Sound::Sounds_GameMusic &&
						m_nowSound != Sound::Sounds_MenuMusic &&
						m_nowSound != Sound::Sounds_RocketMotor)
						break;
				}
				m_sound->Play(static_cast<Sound::Sounds>(m_nowSound));
			}
			else if (evt.key == OIS::KC_5)
			{
				m_sound->PlayRocketMotorSound(0.75f);
				m_sound->ChangeRocketMotorPitchEffect(0.5f);
			}
			else if (evt.key == OIS::KC_6)
			{
				m_sound->StopRocketMotorSound();
			}
			else if (evt.key == OIS::KC_7)
			{
				m_sound->PlayWhosh(1.0f);
			}
			return TestGame::keyPressed(evt);
		}

		int m_nowSound;
	};

	PlaySoundsTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()
