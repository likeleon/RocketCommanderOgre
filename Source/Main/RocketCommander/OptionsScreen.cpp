#include "stdafx.h"
#include "OptionsScreen.h"
#include "CEGuiHelpers.h"
#include "RocketCommanderGame.h"
#include "Sound.h"

using namespace Ogre;

namespace RocketCommander
{
	OptionsScreen::RendererOption::RendererOption(const std::string &name, const CEGUI::String &windowNamePrefix)
	: m_name(name),
	  m_isAvailable(true)
	{
		CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

		m_txtCaption = static_cast<CEGUI::Window *>(winMgr.getWindow(windowNamePrefix + "Caption"));
		assert(m_txtCaption);

		m_txtValue = static_cast<CEGUI::Window *>(winMgr.getWindow(windowNamePrefix + "Value"));
		assert(m_txtValue);

		m_txtValue->subscribeEvent(CEGUI::Window::EventMouseEnters, 
				CEGUI::Event::Subscriber(&OptionsScreen::RendererOption::OnMouseEnters, this));

		m_txtValue->subscribeEvent(CEGUI::Window::EventMouseLeaves, 
				CEGUI::Event::Subscriber(&OptionsScreen::RendererOption::OnMouseLeaves, this));

		m_txtValue->subscribeEvent(CEGUI::Window::EventMouseClick, 
				CEGUI::Event::Subscriber(&OptionsScreen::RendererOption::OnMouseClick, this));
	}

	OptionsScreen::RendererOption::~RendererOption()
	{
		assert(m_txtValue);
		m_txtValue->removeAllEvents();
	}

	void OptionsScreen::RendererOption::UpdateValue(const RenderSystem &renderer)
	{
		assert(m_txtValue);
		ConfigOptionMap& options = const_cast<RenderSystem &>(renderer).getConfigOptions();
		if (options.find(m_name) == options.end())
		{
			m_isAvailable = false;
			m_possibleValues.clear();
			m_txtValue->setText("Option not available");
			m_txtCaption->setProperty("TextColours", CEGuiHelpers::GetSolidColourString(CEGuiHelpers::GrayColour));
			m_txtValue->setProperty("TextColours", CEGuiHelpers::GetSolidColourString(CEGuiHelpers::GrayColour));
		}
		else
		{
			m_isAvailable = true;
			m_possibleValues = options[m_name].possibleValues;
			m_txtValue->setText(options[m_name].currentValue);
			m_txtCaption->setProperty("TextColours", CEGuiHelpers::GetSolidColourString(CEGuiHelpers::WhiteColour));
			m_txtValue->setProperty("TextColours", CEGuiHelpers::GetSolidColourString(CEGuiHelpers::WhiteColour));
		}
	}

	const std::string& OptionsScreen::RendererOption::GetName() const
	{
		return m_name;
	}

	std::string OptionsScreen::RendererOption::GetValue() const
	{
		assert(IsAvailable() && m_txtValue);
		return m_txtValue->getText().c_str();
	}

	bool OptionsScreen::RendererOption::OnMouseClick(const CEGUI::EventArgs &e)
	{
		if (!IsAvailable())
			return true;

		const Ogre::StringVector::const_iterator nowValueIter = 
			std::find(m_possibleValues.begin(), m_possibleValues.end(), GetValue());
		assert(nowValueIter != m_possibleValues.end());

		Ogre::StringVector::const_iterator newValueIter = nowValueIter + 1;
		if (newValueIter == m_possibleValues.end())
			newValueIter = m_possibleValues.begin();
		m_txtValue->setText(*newValueIter);
		return true;
	}
	
	bool OptionsScreen::RendererOption::IsAvailable() const
	{
		return m_isAvailable;
	}

	bool OptionsScreen::RendererOption::OnMouseEnters(const CEGUI::EventArgs &e)
	{
		if (!IsAvailable())
			return true;

		m_txtValue->setProperty("TextColours", CEGuiHelpers::GetSolidColourString(CEGuiHelpers::RedColour));
		return true;
	}

	bool OptionsScreen::RendererOption::OnMouseLeaves(const CEGUI::EventArgs &e)
	{
		if (!IsAvailable())
			return true;

		m_txtValue->setProperty("TextColours", CEGuiHelpers::GetSolidColourString(CEGuiHelpers::WhiteColour));
		return true;
	}

	OptionsScreen::~OptionsScreen()
	{
		for (RendererOptions::iterator option = m_rendererOptions.begin(); option != m_rendererOptions.end(); option++)
			delete *option;
		m_rendererOptions.clear();
	}

	OptionsScreen::OptionsScreen(RocketCommanderGame &game)
	: GameScreen(game, "Options.layout"),
	  m_txtRenderer(NULL)
	{
	}

	void OptionsScreen::Enter()
	{
		GameScreen::Enter();

		//
		// GUI
		//
		CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();
		
		m_txtRenderer = static_cast<CEGUI::ProgressBar *>(winMgr.getWindow("Options/RenderSystemValue"));
		assert(m_txtRenderer);

		RenderSystem * const renderer = m_game.GetRoot().getRenderSystem();

		m_rendererOptions.push_back(new RendererOption("Video Mode", "Options/VideoMode"));
		m_rendererOptions.push_back(new RendererOption("Full Screen", "Options/FullScreen"));
		m_rendererOptions.push_back(new RendererOption("VSync", "Options/VSync"));
		m_rendererOptions.push_back(new RendererOption("FSAA", "Options/FSAA"));
		m_rendererOptions.push_back(new RendererOption("Rendering Device", "Options/RenderingDevice"));

		UpdateOptionValues(*renderer);

		// Register UI event handlers
		CEGuiHelpers::SubscribeEvent("Options/RenderSystemValue", CEGUI::Window::EventMouseClick, 
			CEGUI::Event::Subscriber(&OptionsScreen::OnRendererMouseClick, this));
		
		CEGuiHelpers::SubscribeEvent("Options/RenderSystemValue", CEGUI::Window::EventMouseEnters, 
			CEGUI::Event::Subscriber(&OptionsScreen::OnRendererMouseEnters, this));

		CEGuiHelpers::SubscribeEvent("Options/RenderSystemValue", CEGUI::Window::EventMouseLeaves, 
			CEGUI::Event::Subscriber(&OptionsScreen::OnRendererMouseLeaves, this));

		CEGuiHelpers::SubscribeEvent("Options/ApplyChanges", CEGUI::Window::EventMouseClick, 
			CEGUI::Event::Subscriber(&OptionsScreen::OnApplyChangesMouseClick, this));

		CEGuiHelpers::SubscribeEvent("Options/ApplyChanges", CEGUI::Window::EventMouseEnters, 
			CEGUI::Event::Subscriber(&OptionsScreen::OnApplyChangesMouseEnters, this));

		CEGuiHelpers::SubscribeEvent("Options/ApplyChanges", CEGUI::Window::EventMouseLeaves, 
			CEGUI::Event::Subscriber(&OptionsScreen::OnApplyChangesMouseLeaves, this));

		CEGuiHelpers::SubscribeEvent("Options/BackButton", CEGUI::PushButton::EventMouseEnters, 
			CEGUI::Event::Subscriber(&GameScreen::OnBackButtonMouseEnters, (GameScreen *)this));
		CEGuiHelpers::SubscribeEvent("Options/BackButton", CEGUI::PushButton::EventClicked, 
			CEGUI::Event::Subscriber(&GameScreen::OnBackButtonClicked, (GameScreen *)this));
	}

	void OptionsScreen::UpdateOptionValues(const RenderSystem &renderer)
	{
		assert(m_txtRenderer);
		m_txtRenderer->setText(renderer.getName());

		for (RendererOptions::iterator option = m_rendererOptions.begin(); option != m_rendererOptions.end(); option++)
		{
			(*option)->UpdateValue(renderer);
		}
	}

	bool OptionsScreen::OnApplyChangesMouseClick(const CEGUI::EventArgs &e)
	{
		NameValuePairList newOptions;
		for (RendererOptions::iterator option = m_rendererOptions.begin(); option != m_rendererOptions.end(); option++)
		{
			if (!(*option)->IsAvailable())
				continue;
			newOptions[(*option)->GetName()] = (*option)->GetValue();
		}

		m_game.Reconfigure(m_txtRenderer->getText().c_str(), newOptions);
		return true;
	}

	bool OptionsScreen::OnApplyChangesMouseEnters(const CEGUI::EventArgs &e)
	{
		const CEGUI::MouseEventArgs &mouseEventArgs = static_cast<const CEGUI::MouseEventArgs &>(e);
		mouseEventArgs.window->setProperty("TextColours", CEGuiHelpers::GetSolidColourString(CEGuiHelpers::RedColour));
		m_game.GetSound().Play(Sound::Sounds_Highlight);
		return true;
	}

	bool OptionsScreen::OnApplyChangesMouseLeaves(const CEGUI::EventArgs &e)
	{
		const CEGUI::MouseEventArgs &mouseEventArgs = static_cast<const CEGUI::MouseEventArgs &>(e);
		mouseEventArgs.window->setProperty("TextColours", CEGuiHelpers::GetSolidColourString(CEGuiHelpers::DarkGreenColour));
		return true;
	}

	bool OptionsScreen::OnRendererMouseClick(const CEGUI::EventArgs &e)
	{
		const RenderSystemList &renderers = m_game.GetRoot().getAvailableRenderers();
		const RenderSystem *nowRenderer = m_game.GetRoot().getRenderSystemByName(m_txtRenderer->getText().c_str());
		assert(nowRenderer);

		RenderSystemList::const_iterator nowRendererIter = 
			std::find(renderers.begin(), renderers.end(), nowRenderer);
		assert(nowRendererIter != renderers.end());

		RenderSystemList::const_iterator newRenderer = nowRendererIter + 1;

		if (newRenderer == renderers.end())
			newRenderer = renderers.begin();

		UpdateOptionValues(**newRenderer);
		return true;
	}

	bool OptionsScreen::OnRendererMouseEnters(const CEGUI::EventArgs &e)
	{
		const CEGUI::MouseEventArgs &mouseEventArgs = static_cast<const CEGUI::MouseEventArgs &>(e);
		mouseEventArgs.window->setProperty("TextColours", CEGuiHelpers::GetSolidColourString(CEGuiHelpers::RedColour));
		return true;
	}

	bool OptionsScreen::OnRendererMouseLeaves(const CEGUI::EventArgs &e)
	{
		const CEGUI::MouseEventArgs &mouseEventArgs = static_cast<const CEGUI::MouseEventArgs &>(e);
		mouseEventArgs.window->setProperty("TextColours", CEGuiHelpers::GetSolidColourString(CEGuiHelpers::WhiteColour));
		return true;
	}
}