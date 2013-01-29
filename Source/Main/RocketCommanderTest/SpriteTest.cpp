#include "stdafx.h"
#include "TestGame.h"
#include "Sprite.h"

using namespace RocketCommander;

BOOST_AUTO_TEST_SUITE(SpriteTest)

BOOST_AUTO_TEST_CASE(DrawSpriteTest)
{
	class DrawSpriteTestGame : public TestGame
	{
	public:
		DrawSpriteTestGame() : m_textureName("Glow.dds"), m_showBackground(true) {}

	private:
		virtual void InitTest()
		{
			EnableSkyBox();
			m_sprite->LoadSprite(m_textureName);
			m_leftTopDebug->setText("From left to right: Original Alpha50% Alpha50%/Color\nBlending mode: AlphaBlend(Top) Additive(Bottom)\nB: Toggle background");
		}

		virtual void UpdateTest(double timeSinceLastFrame)
		{
			EnableSkyBox();

			m_sprite->DrawSprite(m_textureName, Sprite::Rect(0.0f, 0.0f, 0.333f, 0.5f), Sprite::FULL_SPRITE, Ogre::ColourValue::White, 1.0f);
			m_sprite->DrawSprite(m_textureName, Sprite::Rect(0.333f, 0.0f, 0.666f, 0.5f), Sprite::FULL_SPRITE, Ogre::ColourValue::White, 0.5f);
			m_sprite->DrawSprite(m_textureName, Sprite::Rect(0.666f, 0.0f, 1.0f, 0.5f), Sprite::FULL_SPRITE, Ogre::ColourValue::Green, 0.5f);

			m_sprite->DrawSprite(m_textureName, Sprite::Rect(0.0f, 0.5f, 0.333f, 1.0f), Sprite::FULL_SPRITE, Ogre::ColourValue::White, 1.0f, true);
			m_sprite->DrawSprite(m_textureName, Sprite::Rect(0.333f, 0.5f, 0.666f, 1.0f), Sprite::FULL_SPRITE, Ogre::ColourValue::White, 0.5f, true);
			m_sprite->DrawSprite(m_textureName, Sprite::Rect(0.666f, 0.5f, 1.0f, 1.0f), Sprite::FULL_SPRITE, Ogre::ColourValue::Green, 0.5f, true);
		}

		virtual bool keyPressed(const OIS::KeyEvent &evt)
		{
			if (evt.key == OIS::KC_B)
				m_showBackground = !m_showBackground;
			return true;
		}

		const Ogre::String m_textureName;
		bool m_showBackground;
	};

	DrawSpriteTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()