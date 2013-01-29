#include "stdafx.h"
#include "CreditsScreen.h"
#include "CEGuiHelpers.h"
#include "RocketCommanderGame.h"


namespace RocketCommander
{
	CreditsScreen::CreditsScreen(RocketCommanderGame &game)
	: GameScreen(game, "Credits.layout")
	{
	}

	void CreditsScreen::Enter()
	{
		GameScreen::Enter();

		CEGuiHelpers::SubscribeEvent("Credits/BackButton", CEGUI::PushButton::EventMouseEnters, 
			CEGUI::Event::Subscriber(&GameScreen::OnBackButtonMouseEnters, (GameScreen *)this));
		CEGuiHelpers::SubscribeEvent("Credits/BackButton", CEGUI::PushButton::EventClicked, 
			CEGUI::Event::Subscriber(&GameScreen::OnBackButtonClicked, (GameScreen *)this));
	}
}