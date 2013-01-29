#include "stdafx.h"
#include "BaseGame.h"

using namespace RocketCommander;

BOOST_AUTO_TEST_SUITE(BaseGameTest)

BOOST_AUTO_TEST_CASE(RunTest)
{
	BaseGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()