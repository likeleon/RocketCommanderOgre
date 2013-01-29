#include "stdafx.h"
#include "Sound.h"
#include "SoundManager.h"
#include "Utils.h"
#include "RandomHelper.h"

namespace RocketCommander
{
	const Sound::SoundSetting Sound::SoundSettings[MaxSounds] =
	{
		Sound::SoundSetting(Sounds_Bomb,		"Bomb.wav",			false),
		Sound::SoundSetting(Sounds_Click,		"Click.wav",		false),
		Sound::SoundSetting(Sounds_Defeat,		"Defeat.wav",		false),
		Sound::SoundSetting(Sounds_Explosion,	"Explosion.wav",	false),
		Sound::SoundSetting(Sounds_ExtraLife,	"ExtraLife.wav",	false),
		Sound::SoundSetting(Sounds_Fuel,		"Fuel.wav",			false),
		Sound::SoundSetting(Sounds_Health,		"Health.wav",		false),
		Sound::SoundSetting(Sounds_Highlight,	"Highlight.wav",	false),
		Sound::SoundSetting(Sounds_GameMusic,	"Rocket Commander Game Music.wav", true),
		Sound::SoundSetting(Sounds_MenuMusic,	"Rocket Commander Menu Music.wav", true),
		Sound::SoundSetting(Sounds_RocketMotor,	"RocketMotor.wav",	true),
		Sound::SoundSetting(Sounds_SideHit,		"SideHit.wav",		false),
		Sound::SoundSetting(Sounds_Speed,		"Speed.wav",		false),
		Sound::SoundSetting(Sounds_Victory,		"Victory.wav",		false),
		Sound::SoundSetting(Sounds_Whosh1,		"Whosh1.wav",		false),
		Sound::SoundSetting(Sounds_Whosh2,		"Whosh2.wav",		false),
		Sound::SoundSetting(Sounds_Whosh3,		"Whosh3.wav",		false),
		Sound::SoundSetting(Sounds_Whosh4,		"Whosh4.wav",		false)
	};

	const float Sound::BaseVolume = 0.4f;

	Sound::Sound()
	: m_soundMgr(NULL)
	{
		m_soundMgr = new SoundManager();
		m_soundMgr->Initialize();

		for (size_t i = 0; i < ArraySize(SoundSettings); ++i)
		{
			const SoundSetting &SoundSetting = SoundSettings[i];
			if (SoundSetting.m_looped)
				m_soundDatas[SoundSetting.m_sound].m_soundIndex = m_soundMgr->CreateLoopedStream(SoundSetting.m_fileName);
			else
				m_soundDatas[SoundSetting.m_sound].m_soundIndex = m_soundMgr->CreateStream(SoundSetting.m_fileName);
			m_soundDatas[SoundSetting.m_sound].m_channelIndex = INVALID_SOUND_CHANNEL;
		}
	}

	Sound::~Sound()
	{
		m_soundMgr->StopAllSounds();
		delete m_soundMgr;
	}

	void Sound::Play(Sounds sound)
	{
		if (SoundSettings[sound].m_looped)
		{
			// Music
			FMOD::Channel *channel = m_soundMgr->GetSoundChannel(m_soundDatas[sound].m_channelIndex);
			bool isPlaying = false;
			if (channel)
				channel->isPlaying(&isPlaying);
			if (channel && isPlaying)
				return;

			// Stop current music
			StopLooped();
		}

		assert(m_soundMgr);
		m_soundMgr->PlaySound(m_soundDatas[sound].m_soundIndex, NULL, &m_soundDatas[sound].m_channelIndex, BaseVolume);
	}

	void Sound::StopLooped()
	{
		for (size_t i = 0; i < ArraySize(SoundSettings); ++i)
		{
			const SoundSetting &SoundSetting = SoundSettings[i];
			if (!SoundSetting.m_looped)
				continue;
	
			m_soundMgr->StopSound(&m_soundDatas[SoundSetting.m_sound].m_channelIndex);
		}
	}

	void Sound::PlayRocketMotorSound(float volume)
	{
		assert(m_soundMgr);
		m_soundMgr->PlaySound(m_soundDatas[Sound::Sounds_RocketMotor].m_soundIndex, 
			NULL, 
			&m_soundDatas[Sound::Sounds_RocketMotor].m_channelIndex, 
			BaseVolume * volume);
	}

	void Sound::ChangeRocketMotorPitchEffect(float pitchFactor)
	{
		// Not yet supported
	}
	
	void Sound::StopRocketMotorSound()
	{
		m_soundMgr->StopSound(&m_soundDatas[Sound::Sounds_RocketMotor].m_channelIndex);
	}

	void Sound::Update()
	{
		m_soundMgr->Update();
	}

	void Sound::PlayWhosh(float volume)
	{
		assert(m_soundMgr);
		
		const Sounds whoshType = static_cast<Sounds>(static_cast<int>(Sounds_Whosh1) +
			RandomHelper::GetRandomInt(0, Sounds_Whosh4 - Sounds_Whosh1));
		
		m_soundMgr->PlaySound(m_soundDatas[whoshType].m_soundIndex, 
			NULL, 
			&m_soundDatas[whoshType].m_channelIndex, 
			BaseVolume * volume);
	}

	void Sound::PlayItemSound(Item::ItemTypes itemType)
	{
		Play(itemType == Item::ItemType_Fuel? Sound::Sounds_Fuel :
			itemType == Item::ItemType_Health? Sound::Sounds_Health :
			itemType == Item::ItemType_ExtraLife? Sound::Sounds_ExtraLife :
			itemType == Item::ItemType_Speed? Sound::Sounds_Speed :
			Sound::Sounds_Bomb);
	}

	void Sound::PlayExplosionSound()
	{
		Play(Sound::Sounds_Explosion);
	}

	void Sound::PlayDefeatSound()
	{
		Play(Sound::Sounds_Defeat);
	}

	void Sound::PlayVictorySound()
	{
		Play(Sound::Sounds_Victory);
	}

	void Sound::SetCurrentMusicMode(bool inGame)
	{
		Play(inGame? Sound::Sounds_GameMusic : Sound::Sounds_MenuMusic);
	}

}