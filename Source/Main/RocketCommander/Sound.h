#pragma once

#include "Item.h"

class SoundManager;

namespace RocketCommander
{
	class Sound
	{
	public:
		typedef enum
		{
			Sounds_Bomb = 0,
			Sounds_Click,
			Sounds_Defeat,
			Sounds_Explosion,
			Sounds_ExtraLife,
			Sounds_Fuel,
			Sounds_Health,
			Sounds_Highlight,
			Sounds_GameMusic,
			Sounds_MenuMusic,
			Sounds_RocketMotor,
			Sounds_SideHit,
			Sounds_Speed,
			Sounds_Victory,
			Sounds_Whosh1,
			Sounds_Whosh2,
			Sounds_Whosh3,
			Sounds_Whosh4,
			MaxSounds
		} Sounds;

		struct SoundSetting
		{
			SoundSetting(Sounds sound, const Ogre::String &fileName, bool looped)
				: m_sound(sound), m_fileName(fileName), m_looped(looped) {}

			Sounds			m_sound;
			Ogre::String	m_fileName;
			bool			m_looped;
		};

		struct SoundData
		{
			int m_soundIndex;
			int m_channelIndex;
		};

		Sound();
		~Sound();

		void Update();
		void Play(Sounds sound);
		void StopLooped();
		void PlayRocketMotorSound(float volume);
		void ChangeRocketMotorPitchEffect(float pitchFactor);
		void StopRocketMotorSound();
		void PlayWhosh(float volume);
		void PlayItemSound(Item::ItemTypes itemType);
		void PlayExplosionSound();
		void PlayDefeatSound();
		void PlayVictorySound();
		void SetCurrentMusicMode(bool inGame);

	private:
		static const SoundSetting SoundSettings[];
		static const float BaseVolume;

		SoundManager *m_soundMgr;
		std::map<Sounds, SoundData> m_soundDatas;
	};
}