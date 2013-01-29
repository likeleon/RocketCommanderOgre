#include "stdafx.h"
#include "TextRenderer.h"
#include <stdio.h>
#include <stdarg.h>

template<> RocketCommander::TextRenderer *Ogre::Singleton<RocketCommander::TextRenderer>::ms_Singleton = 0;

namespace RocketCommander
{
	TextRenderer::TextRenderer()
	: m_overlayMgr(NULL), m_overlay(NULL), m_panel(NULL)
	{
		m_overlayMgr = Ogre::OverlayManager::getSingletonPtr();

		m_overlay = m_overlayMgr->create("overlay1");
		m_panel = static_cast<Ogre::OverlayContainer*>(m_overlayMgr->createOverlayElement("Panel", "container1"));
		m_panel->setDimensions(1, 1);
		m_panel->setPosition(0, 0);

		m_overlay->add2D(m_panel);

		m_overlay->show();
	}

	TextRenderer::~TextRenderer()
	{
		if (m_overlayMgr)
		{
			if (m_panel)
				m_overlayMgr->destroyOverlayElement(m_panel);
			if (m_overlay)
				m_overlayMgr->destroy(m_overlay);
		}
	}

	void TextRenderer::AddTextBox(const std::string &id,
		const std::string &text,
		Ogre::Real x, Ogre::Real y,
		Ogre::Real width, Ogre::Real height,
		const Ogre::ColourValue &color)
	{
		Ogre::OverlayElement *textBox = m_overlayMgr->createOverlayElement("TextArea", id);
		textBox->setDimensions(width, height);
		textBox->setMetricsMode(Ogre::GMM_PIXELS);
		textBox->setPosition(x, y);
		textBox->setWidth(width);
		textBox->setHeight(height);
		textBox->setParameter("font_name", "DebugFont");
		textBox->setParameter("char_height", "16");
		textBox->setColour(color);

		textBox->setCaption(text);

		m_panel->addChild(textBox);
	}

	void TextRenderer::RemoveTextBox(const std::string &id)
	{
		m_panel->removeChild(id);
		m_overlayMgr->destroyOverlayElement(id);
	}

	void TextRenderer::SetText(const std::string &id, const std::string &text)
	{
		Ogre::OverlayElement* textBox = m_overlayMgr->getOverlayElement(id);
		textBox->setCaption(text);
	}

	void TextRenderer::SetText(const std::string &id, const char *fmt, /* args */ ...)
	{
		char       text[256] = {'\0'};
		va_list    ap;

		if (fmt)
		{
			va_start(ap, fmt);
			vsprintf(text, fmt, ap);
			va_end(ap);
		}

		Ogre::OverlayElement *textBox = m_overlayMgr->getOverlayElement(id);
		textBox->setCaption(text);
	}
}