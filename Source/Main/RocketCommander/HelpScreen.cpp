#include "stdafx.h"
#include "HelpScreen.h"
#include "CEGuiHelpers.h"
#include "RocketCommanderGame.h"

namespace RocketCommander
{
	HelpScreen::HelpScreen(RocketCommanderGame &game)
	: GameScreen(game, "Help.layout")
	{
	}

	void HelpScreen::Enter()
	{
		GameScreen::Enter();

		CEGuiHelpers::SubscribeEvent("Help/BackButton", CEGUI::PushButton::EventMouseEnters, 
			CEGUI::Event::Subscriber(&GameScreen::OnBackButtonMouseEnters, (GameScreen *)this));
		CEGuiHelpers::SubscribeEvent("Help/BackButton", CEGUI::PushButton::EventClicked, 
			CEGUI::Event::Subscriber(&GameScreen::OnBackButtonClicked, (GameScreen *)this));
	}
}