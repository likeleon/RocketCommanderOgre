#include "stdafx.h"
#include "MissionSelectionScreen.h"
#include "CEGuiHelpers.h"
#include "RocketCommanderGame.h"
#include "Line2D.h"
#include "MissionScreen.h"

using namespace Ogre;

namespace RocketCommander
{
	MissionSelector::MissionSelector(const String &name, const Level &level, Ogre::SceneManager &sceneMgr, Sound &sound, MissionSelector::IEventListener &eventListener)
	: m_name(name), m_sound(sound), m_level(level), m_eventListener(eventListener), m_txtName(NULL)
	{
		CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

		// Get windows
		m_txtName = static_cast<CEGUI::DefaultWindow *>(winMgr.getWindow("MissionSelection/" + name + "Text"));
		assert(m_txtName);
		for (int i = 0; ; ++i)
		{
			const CEGUI::String btnName = "MissionSelection/" + name + "Button" + Ogre::StringConverter::toString(i);
			if (!winMgr.isWindowPresent(btnName))
				break;
			m_btnImages.push_back(static_cast<CEGUI::PushButton *>(winMgr.getWindow(btnName)));
		}

		// Draw lines around preview rect to mark highlighting
		const float heightScale = 698.0f / 768.0f;
		for (std::vector<CEGUI::PushButton *>::iterator btnImage = m_btnImages.begin(); btnImage != m_btnImages.end(); ++btnImage)
		{
			Ogre::Vector4 rcImage;
			rcImage.x = (*btnImage)->getXPosition().d_scale * 2 - 1;
			rcImage.y = 1 - (*btnImage)->getYPosition().d_scale * heightScale * 2;
			rcImage.z = rcImage.x + (*btnImage)->getWidth().d_scale * 2;
			rcImage.w = rcImage.y - (*btnImage)->getHeight().d_scale * heightScale * 2;
			m_lines.push_back(new Line2D(sceneMgr, Vector2(rcImage.x, rcImage.y), Vector2(rcImage.z, rcImage.y), ColourValue::White));
			m_lines.push_back(new Line2D(sceneMgr, Vector2(rcImage.z, rcImage.y), Vector2(rcImage.z, rcImage.w), ColourValue::White));
			m_lines.push_back(new Line2D(sceneMgr, Vector2(rcImage.z, rcImage.w), Vector2(rcImage.x, rcImage.w), ColourValue::White));
			m_lines.push_back(new Line2D(sceneMgr, Vector2(rcImage.x, rcImage.w), Vector2(rcImage.x, rcImage.y), ColourValue::White));
		}

		// Register UI events
		for (std::vector<CEGUI::PushButton *>::iterator btnImage = m_btnImages.begin(); btnImage != m_btnImages.end(); ++btnImage)
		{
			(*btnImage)->subscribeEvent(CEGUI::PushButton::EventMouseEnters, 
				CEGUI::Event::Subscriber(&MissionSelector::OnButtonMouseEnters, this));
			(*btnImage)->subscribeEvent(CEGUI::PushButton::EventMouseLeaves, 
				CEGUI::Event::Subscriber(&MissionSelector::OnButtonMouseLeaves, this));
			(*btnImage)->subscribeEvent(CEGUI::PushButton::EventMouseClick, 
				CEGUI::Event::Subscriber(&MissionSelector::OnButtonMouseClick, this));
		}
	}

	MissionSelector::~MissionSelector()
	{
		for (std::vector<CEGUI::PushButton *>::iterator btnImage = m_btnImages.begin(); btnImage != m_btnImages.end(); ++btnImage)
		{
			(*btnImage)->removeAllEvents();
		}

		for (std::vector<Line2D *>::iterator line = m_lines.begin(); line != m_lines.end(); ++line)
		{
			delete *line;
		}
	}

	bool MissionSelector::OnButtonMouseEnters(const CEGUI::EventArgs &e)
	{
		m_sound.Play(Sound::Sounds_Highlight);

		m_txtName->setProperty("TextColours", "tl:FFFF0000 tr:FFFF0000 bl:FFFF0000 br:FFFF0000");
		for (std::vector<Line2D *>::iterator line = m_lines.begin(); line != m_lines.end(); ++line)
			(*line)->SetColour(ColourValue::Red);
		return true;
	}

	bool MissionSelector::OnButtonMouseLeaves(const CEGUI::EventArgs &e)
	{
		m_txtName->setProperty("TextColours", "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");
		for (std::vector<Line2D *>::iterator line = m_lines.begin(); line != m_lines.end(); ++line)
			(*line)->SetColour(ColourValue::White);
		return true;
	}

	bool MissionSelector::OnButtonMouseClick(const CEGUI::EventArgs &e)
	{
		return m_eventListener.OnSelected(this);
	}

	MissionSelectionScreen::MissionSelectionScreen(RocketCommanderGame &game)
	: GameScreen(game, "MissionSelection.layout"), 
	  m_selectedLevel(NULL)
	{
	}

	void MissionSelectionScreen::Enter()
	{
		GameScreen::Enter();

		CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

		m_missionSelectors.push_back(new MissionSelector("EasyFlight", m_game.GetLevel(0), m_game.GetSceneManager(), m_game.GetSound(), *this));
		m_missionSelectors.push_back(new MissionSelector("LostCivilization", m_game.GetLevel(1), m_game.GetSceneManager(), m_game.GetSound(), *this));
		m_missionSelectors.push_back(new MissionSelector("ValleyofDeath", m_game.GetLevel(2), m_game.GetSceneManager(), m_game.GetSound(), *this));
		m_missionSelectors.push_back(new MissionSelector("TheRevenge", m_game.GetLevel(3), m_game.GetSceneManager(), m_game.GetSound(), *this));

		CEGuiHelpers::SubscribeEvent("MissionSelection/BackButton", CEGUI::PushButton::EventMouseEnters, 
			CEGUI::Event::Subscriber(&GameScreen::OnBackButtonMouseEnters, (GameScreen *)this));
		CEGuiHelpers::SubscribeEvent("MissionSelection/BackButton", CEGUI::PushButton::EventClicked, 
			CEGUI::Event::Subscriber(&GameScreen::OnBackButtonClicked, (GameScreen *)this));
	}

	void MissionSelectionScreen::Exit()
	{
		for (std::vector<MissionSelector *>::iterator selector = m_missionSelectors.begin();
			selector != m_missionSelectors.end(); ++selector)
		{
			delete *selector;
		}
		m_missionSelectors.clear();

		GameScreen::Exit();
	}

	bool MissionSelectionScreen::OnSelected(MissionSelector *selector)
	{
		m_selectedLevel = &selector->GetLevel();
		return true;
	}

	void MissionSelectionScreen::Run()
	{
		if (m_selectedLevel)
		{
			// Start mission with this level with closing this screen
			m_game.ChangeGameScreen(new MissionScreen(m_game, *m_selectedLevel));
		}
	}
}