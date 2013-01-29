#include "stdafx.h"
#include "MissionScreen.h"
#include "GameAsteroidManager.h"
#include "Player.h"
#include "Rocket.h"
#include "Texts.h"
#include "CEGuiHelpers.h"
#include "SpaceCamera.h"
#include "Sound.h"

using namespace Ogre;

namespace RocketCommander
{
	MissionScreen::MissionScreen(RocketCommanderGame &game, const Level &level)
	: GameScreen(game, "Mission.layout", false),
	  m_rocketEndGameScale(0.0f),
	  m_fuelBar(NULL),
	  m_healthBar(NULL),
	  m_speedBar(NULL),
	  m_lifeBar(NULL),
	  m_posText(NULL),
	  m_rankText(NULL),
	  m_timeText(NULL),
	  m_scoreText(NULL),
	  m_messageText(NULL),
	  m_targetIcon(NULL),
	  m_targetIconSize(Vector2::ZERO),
	  m_arrowRight(NULL),
	  m_arrowLeft(NULL),
	  m_arrowUp(NULL),
	  m_arrowDown(NULL),
	  m_arrowSize(Vector2::ZERO)
	{
		// Set level for asteroid manager
		m_game.GetAsteroidManager().SetLevel(level);

		// Start new game
		m_game.GetPlayer().Reset(level.GetName());
	}

	void MissionScreen::Enter()
	{
		GameScreen::Enter();

		//
		// GUI
		//
		CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();
		
		m_fuelBar = static_cast<CEGUI::ProgressBar *>(winMgr.getWindow("Mission/FuelBar"));
		assert(m_fuelBar);

		m_healthBar = static_cast<CEGUI::ProgressBar *>(winMgr.getWindow("Mission/HealthBar"));
		assert(m_healthBar);

		m_speedBar = static_cast<CEGUI::ProgressBar *>(winMgr.getWindow("Mission/SpeedBar"));
		assert(m_speedBar);

		m_lifeBar = static_cast<CEGUI::ProgressBar *>(winMgr.getWindow("Mission/RocketLife"));
		assert(m_lifeBar);

		m_posText = static_cast<CEGUI::Window *>(winMgr.getWindow("Mission/Pos"));
		assert(m_posText);

		m_rankText = static_cast<CEGUI::Window *>(winMgr.getWindow("Mission/Rank"));
		assert(m_rankText);

		m_timeText = static_cast<CEGUI::Window *>(winMgr.getWindow("Mission/Time"));
		assert(m_timeText);

		m_scoreText = static_cast<CEGUI::Window *>(winMgr.getWindow("Mission/Score"));
		assert(m_scoreText);

		m_messageText = static_cast<CEGUI::Window *>(winMgr.getWindow("Mission/Message"));
		assert(m_messageText);

		m_targetIcon = static_cast<CEGUI::Window *>(winMgr.getWindow("Mission/TargetIcon"));
		assert(m_targetIcon);
		m_targetIconSize.x = m_targetIcon->getWidth().d_scale;
		m_targetIconSize.y = m_targetIcon->getHeight().d_scale;

		m_arrowRight = static_cast<CEGUI::Window *>(winMgr.getWindow("Mission/ArrowRight"));
		m_arrowLeft = static_cast<CEGUI::Window *>(winMgr.getWindow("Mission/ArrowLeft"));
		m_arrowUp = static_cast<CEGUI::Window *>(winMgr.getWindow("Mission/ArrowUp"));
		m_arrowDown = static_cast<CEGUI::Window *>(winMgr.getWindow("Mission/ArrowDown"));
		assert(m_arrowRight && m_arrowLeft && m_arrowUp && m_arrowDown);
		m_arrowSize.x = m_arrowLeft->getWidth().d_scale;
		m_arrowSize.y = m_arrowLeft->getHeight().d_scale;
	}

	void MissionScreen::Run()
	{
		// Zoom into rocket for countdown time
		ZoomIntoRocket();

		// Show target in z direction
		ShowTarget(m_game.GetAsteroidManager().GetLevel().GetLength());

		// Show all hud controls
		ShowHudContorls();

		// Handle game stuff
		Player &player = m_game.GetPlayer();
		player.HandleGameLogic();

		// Show on screen helper messages
		ShowScreenMessages();

		// End game if escape was pressed or game is over and space or mouse button was pressed
		const bool quitWithGameOver = player.IsGameOver() &&
			(m_game.GetKeyboard().isKeyDown(OIS::KC_SPACE) || 
			 m_game.GetMouse().getMouseState().buttonDown(OIS::MB_Left));

		if (m_game.GetKeyboard().isKeyDown(OIS::KC_ESCAPE) || quitWithGameOver)
		{
			player.SetGameOverAndUploadHighscore();

			m_game.GetSound().StopRocketMotorSound();

			m_game.GetCamera().setPosition(Vector3::ZERO);
			m_game.GetSpaceCamera().SetInGame(false);

			m_quit = true;
		}
	}

	void MissionScreen::ZoomIntoRocket()
	{
		const Player &player = m_game.GetPlayer();
		if (!player.IsGameOver())
			return;

		// Only start showing rocket after all explosions are nearly over
		if (player.GetExplosionTimeoutMs() < 400 &&
			player.GetExplosionTimeoutMs2() < 400 &&
			player.GetExplosionTimeoutMs3() < 400)
		{
			Rocket &rocket = m_game.GetRocket();

			// Scale in rocket
			m_rocketEndGameScale += m_game.GetMoveFactorPerSecond() * 5.0f;
			m_rocketEndGameScale = std::min(m_rocketEndGameScale, 10.0f);
			rocket.SetScale(Vector3(m_rocketEndGameScale, m_rocketEndGameScale, m_rocketEndGameScale));

			// Show rocket in middle of screen
			Vector3 inFrontOfCameraPos = Vector3(0, -0.3f, -1.75f) * 10.0f;
			inFrontOfCameraPos = m_game.GetCamera().getViewMatrix().inverse() * inFrontOfCameraPos;
			rocket.SetPosition(inFrontOfCameraPos);

			const Quaternion &rocketOrient = Quaternion(Radian(Math::PI / 4.0f), Vector3::UNIT_Z) *
				Quaternion(Radian(-Math::PI), Vector3::UNIT_X) *
				Quaternion(Radian(m_game.GetTotalTimeMs() / 2293.0f), Vector3::UNIT_Y) *
				Quaternion(Radian(Math::PI), Vector3::UNIT_X);
			rocket.SetOrientation(rocketOrient);
		}
		else
		{
			m_rocketEndGameScale = 0.0f;
		}
	}

	void MissionScreen::ShowHudContorls()
	{
		const Player &player = m_game.GetPlayer();

		// Fuel, health and speed
		assert(m_fuelBar && m_healthBar && m_speedBar);
		m_fuelBar->setProgress(player.GetFuel());
		m_healthBar->setProgress(player.GetHealth());
		m_speedBar->setProgress(player.GetSpeed());
		m_lifeBar->setProgress(player.GetLifes() / 10.0f);

		// Show level position, score, rank ang game time
		assert(m_posText && m_rankText && m_scoreText && m_timeText);
		int pos = static_cast<int>(101 * m_game.GetCamera().getPosition().z / 
			(m_game.GetAsteroidManager().GetLevel().GetLength() * GameAsteroidManager::SectorDepth));
		pos = std::min(100, std::max(0, pos));
		m_posText->setText("Pos: " + StringConverter::toString(pos) + "%");

		m_rankText->setText("Rank: #1");

		m_scoreText->setText("Score: " + StringConverter::toString(player.GetScore()));

		m_timeText->setText("Time: " + 
			StringConverter::toString(static_cast<int>((player.GetGameTimeMs() / 1000) / 60), 2, '0') + ":" +
			StringConverter::toString(static_cast<int>(player.GetGameTimeMs() / 1000) % 60, 2, '0'));
	}

	void MissionScreen::ShowScreenMessages()
	{
		using namespace OIS;

		assert(m_messageText);
		m_messageText->setAlpha(1.0f);
		m_messageText->setText("");
		m_messageText->setProperty("TextColours", CEGuiHelpers::GetSolidColourString(CEGUI::colour(1.0f, 1.0f, 1.0f)));

		Player &player = m_game.GetPlayer();
		const Ogre::Vector3 &pos = m_game.GetCamera().getPosition();

		// If game is over, show end screen
		if (player.IsGameOver())
		{
			m_messageText->setText(player.IsVictory() ? Texts::Victory : Texts::GameOver + "\n" +
				Texts::YourHighscore + " " + StringConverter::toString(player.GetScore()) + " " +
				"(#1)");
		}

		// If we just lost a life, display message
		else if (player.GetExplosionTimeoutMs() >= 0)
		{
			m_messageText->setText(Texts::YouLostALife + "\n" +
				Texts::ClickToContinue);

			const MouseState &mouseState = m_game.GetMouse().getMouseState();
			if (mouseState.buttonDown(MB_Left))
				player.SetExplosionTimeoutMs(-1);
		}

		// Show countdown if a new life started
		else if (player.GetLifeTimeMs() < Player::LifeTimeZoomAndAccelerateMs)
		{
			float alpha = 1.0f - ((static_cast<int>(player.GetLifeTimeMs()) % 1000) / 1000.0f);
			alpha *= 2.0f;
			alpha = std::min(1.0f, alpha);
			m_messageText->setAlpha(alpha);
			
			if (player.GetLifeTimeMs() < 3000)
			{
				m_messageText->setText(Texts::GetReady + "\n" +
					StringConverter::toString(3 - static_cast<int>(player.GetLifeTimeMs() / 1000)));
			}
			else
			{
				m_messageText->setText(Texts::Go);
			}
		}

		// Show item helper messages
		else if (player.GetItemMessageTimeoutMs() > 0 &&
			player.GetCurrentItemMessage().empty() == false)
		{
			player.SetItemMessageTimeoutMs(std::max(0.0f, player.GetItemMessageTimeoutMs() - m_game.GetElapsedTimeThisFrameInMs()));

			float alpha = 1.0f;
			if (player.GetItemMessageTimeoutMs() < 1000)
				alpha = player.GetItemMessageTimeoutMs() / 1000.0f;
			m_messageText->setAlpha(alpha);
			
			m_messageText->setProperty("TextColours", CEGuiHelpers::GetSolidColourString(CEGUI::colour(0.83f, 0.83f, 0.83f)));
			m_messageText->setText(player.GetCurrentItemMessage());
		}

		// Warning if nearly dead
		else if (player.GetShowHealthWarningTimeoutMs() > 0)
		{
			float alpha = 1.0f;
			if (player.GetShowHealthWarningTimeoutMs() < 1000)
				alpha = player.GetShowHealthWarningTimeoutMs() / 1000.0f;

			m_messageText->setProperty("TextColours", CEGuiHelpers::GetSolidColourString(CEGUI::colour(1.0f, 0.0f, 0.0f)));
			m_messageText->setText(Texts::WarningLowHealth);
		}

		// Warning if going out of fuel
		else if (player.GetFuel() < 0.1f)
		{
			m_messageText->setProperty("TextColours", CEGuiHelpers::GetSolidColourString(CEGUI::colour(1.0f, 0.0f, 0.0f)));
			m_messageText->setText(Texts::WarningOutOfFuel);
		}

		// Warning if out of playable area
		else if (pos.x / GameAsteroidManager::SectorWidth < 
			-m_game.GetAsteroidManager().GetLevel().GetWidth() / 2 ||
			pos.x / GameAsteroidManager::SectorWidth > 
			+m_game.GetAsteroidManager().GetLevel().GetWidth() / 2 ||
			pos.y / GameAsteroidManager::SectorHeight <
			-m_game.GetAsteroidManager().GetLevel().GetWidth() / 2 ||
			pos.y / GameAsteroidManager::SectorHeight >
			+m_game.GetAsteroidManager().GetLevel().GetWidth() / 2 ||
			pos.z < 0)
		{
			m_messageText->setProperty("TextColours", CEGuiHelpers::GetSolidColourString(CEGUI::colour(1.0f, 0.0f, 0.0f)));
			m_messageText->setText(Texts::WarningOutOfLevel);
		}
	}

	void MissionScreen::ShowTarget(int levelLength)
	{
		m_targetIcon->hide();
		m_arrowRight->hide();
		m_arrowLeft->hide();
		m_arrowUp->hide();
		m_arrowDown->hide();

		const Vector3 &targetPosition = m_game.GetAsteroidManager().GetTargetPosition();
		const float targetDistance = (targetPosition - m_game.GetCamera().getPosition()).length();
		const Vector2 screenPos = m_game.Convert3DPointTo2D(targetPosition);
		const bool isVisible = m_game.IsInFrontOfCamera(targetPosition);

		if (isVisible &&
			screenPos.x >= 0 && screenPos.x < m_game.GetWidth() &&
			screenPos.y >= 0 && screenPos.y < m_game.GetHeight())
		{
			assert(m_targetIcon);
			
			const Vector2 distanceVector = Vector2(screenPos.x - m_game.GetWidth() / 2, screenPos.y - m_game.GetHeight() / 2);
			float borderFactor = distanceVector.length() / static_cast<float>(m_game.GetHeight() / 2);
			borderFactor = std::min(1.0f, std::max(0.2f, borderFactor));
			m_targetIcon->setAlpha(borderFactor);

			const float posX = screenPos.x / m_game.GetWidth() - m_targetIconSize.x / 2;
			const float posY = screenPos.y / m_game.GetHeight() - m_targetIconSize.y / 2;
			m_targetIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(posX, 0), CEGUI::UDim(posY, 0)));

			m_targetIcon->show();
		}
		else
		{
			// Find out direction of the target by calculating 4 angles
			// for all 4 directions and then choosing the closest one
			Vector3 leftRotation = m_game.GetCamera().getRight() * -Vector3::UNIT_SCALE;
			const float leftAngle = leftRotation.angleBetween(targetPosition).valueRadians();
			
			Vector3 rightRotation = m_game.GetCamera().getRight();
			const float rightAngle = rightRotation.angleBetween(targetPosition).valueRadians();
			
			Vector3 upRotation = m_game.GetCamera().getUp();
			const float upAngle = upRotation.angleBetween(targetPosition).valueRadians();

			Vector3 downRotation = m_game.GetCamera().getUp() * -Vector3::UNIT_SCALE;
			const float downAngle = downRotation.angleBetween(targetPosition).valueRadians();

			// Find out direction and screen coordinates on the screen borders
			const float xPosPercent = 0.5f + (leftAngle - Math::PI / 2.0f) * 0.6f;
			const float yPosPercent = 0.5f + (upAngle - Math::PI / 2.0f) * 0.85f;
			float posX = xPosPercent - m_arrowSize.x;
			posX = std::min(1.0f, std::max(0.0f, posX));
			float posY = yPosPercent - m_arrowSize.y;
			posY = std::min(1.0f, std::max(0.0f, posY));

			// First try, just get the closest angle
			if (leftAngle < rightAngle &&
				leftAngle < upAngle &&
				leftAngle < downAngle)
			{
				m_arrowLeft->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(posY, 0)));
				m_arrowLeft->show();
			}
			else if (rightAngle < upAngle &&
				rightAngle < downAngle)
			{
				m_arrowRight->setPosition(CEGUI::UVector2(CEGUI::UDim(1.0f - m_arrowSize.x, 0), CEGUI::UDim(posY, 0)));
				m_arrowRight->show();
			}
			else if (upAngle < downAngle)
			{
				m_arrowUp->setPosition(CEGUI::UVector2(CEGUI::UDim(posX, 0), CEGUI::UDim(0, 0)));
				m_arrowUp->show();
			}
			else
			{
				m_arrowDown->setPosition(CEGUI::UVector2(CEGUI::UDim(posX, 0), CEGUI::UDim(1.0f - m_arrowSize.y, 0)));
				m_arrowDown->show();
			}
		} // if (isVisible...
	}
}