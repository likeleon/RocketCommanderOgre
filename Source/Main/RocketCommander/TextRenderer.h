#pragma once

namespace RocketCommander
{
	class TextRenderer : public Ogre::Singleton<TextRenderer>
	{
	public:
		TextRenderer();
		virtual ~TextRenderer();

		void AddTextBox(const std::string &id, const std::string &text,
			Ogre::Real x, Ogre::Real y, Ogre::Real width, Ogre::Real height,
			const Ogre::ColourValue& color = Ogre::ColourValue(1, 1, 1));

		void RemoveTextBox(const std::string &id);

		void SetText(const std::string &id, const std::string &text);
		void SetText(const std::string &id, const char *fmt, /* args */ ...);

	private:
		Ogre::OverlayManager	*m_overlayMgr;
		Ogre::Overlay			*m_overlay;
		Ogre::OverlayContainer	*m_panel;
	};
}