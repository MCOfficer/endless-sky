/* TestStep.cpp
Copyright (c) 2019 by Peter van der Meer

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "ConditionSet.h"
#include "DataNode.h"
#include "Files.h"
#include "Panel.h"
#include "PlanetPanel.h"
#include "PlayerInfo.h"
#include "TestStep.h"
#include "UI.h"
#include <string>


using namespace std;


TestStep::TestStep()
{
	// Initialize with some sensible default values.
	stepType = 0;
	frameWait = 20;
	saveGameName = "";
}



TestStep::~TestStep()
{
}



int TestStep::StepType()
{
	return stepType;
}



const string TestStep::SaveGameName()
{
	return saveGameName;
}



bool TestStep::PlayerIsFlyingAround(UI &menuPanels, UI &gamePanels, PlayerInfo &player)
{
	// Code borrowed from main.cpp
	bool inFlight = (menuPanels.IsEmpty() && gamePanels.Root() == gamePanels.Top());
	return inFlight;
}



bool TestStep::PlayerMenuIsActive(UI &menuPanels)
{
	return !menuPanels.IsEmpty();
}



bool TestStep::PlayerOnPlanetMainScreen(UI &menuPanels, UI &gamePanels, PlayerInfo &player)
{
	if (! menuPanels.IsEmpty())
		return false;
	if (gamePanels.Root() == gamePanels.Top())
		return false;

	PlanetPanel *topPlanetPanel = GetPlanetPanelIfAvailable(gamePanels);
	return topPlanetPanel != nullptr;
}



PlanetPanel * TestStep::GetPlanetPanelIfAvailable(UI &gamePanels)
{
	if (gamePanels.IsEmpty()){
		return nullptr;
	}
	Panel *topPanel = gamePanels.Top().get();
	// If we can cast the topPanel from the gamePanels to planetpanel,
	// then we have landed and are on a planet.
	return dynamic_cast<PlanetPanel*>(topPanel);
}






void TestStep::Load(const DataNode &node)
{
	if (node.Token(0) == "load" and node.Size() > 1)
	{
		stepType = LOAD_GAME;
		saveGameName = node.Token(1);
	}
	else if (node.Token(0) == "assert")
	{
		stepType = ASSERT;
		checkedCondition.Load(node);
	}
	else if (node.Size() > 1 and node.Token(0) == "wait" and node.Token(1) == "for")
	{
		stepType = WAITFOR;
		checkedCondition.Load(node);
	}
	else if (node.Token(0) == "land")
	{
		stepType = LAND;
	}
	else if (node.Token(0) == "launch")
	{
		stepType = LAUNCH;
	}
	else
		node.PrintTrace("Skipping unrecognized test-step: " + node.Token(0));
}

int TestStep::DoStep(UI &menuPanels, UI &gamePanels, PlayerInfo &player)
{
	if (frameWait > 0){
		frameWait--;
		return RESULT_RETRY;
	}

	string saveGamePath = Files::Saves() + SaveGameName();
	switch (stepType)
	{
		case TestStep::ASSERT:
		case TestStep::WAITFOR:
			//TODO: implement the ASSERT and WAITFOR condition checkers.
			return RESULT_FAIL;
			break;
		case TestStep::LAUNCH:
			// If flying around, then launching the ship succesfully happened
			if (PlayerIsFlyingAround(menuPanels, gamePanels, player))
				return RESULT_DONE;
			// Should implement some function to close this menu. But
			// fail for now if the player/game menu is active.
			if (PlayerMenuIsActive(menuPanels))
				return RESULT_FAIL;
			if (PlayerOnPlanetMainScreen(menuPanels, gamePanels, player)){
				// Launch using the conversation mechanism. Not the most
				// appropriate way to launch, but works for now.
				player.BasicCallback(Conversation::LAUNCH);
				return RESULT_RETRY;
			}
			// No idea where we are and what we are doing. But we are not
			// in a position to launch, so fail this teststep.
			return RESULT_FAIL;
			break;
		case TestStep::LAND:
			// If the player/game menu is active, then we are not in a state
			// where land makes sense.
			if (PlayerMenuIsActive(menuPanels))
				return RESULT_FAIL;
			// If we are still flying around, then we are not on a planet.
			if (PlayerIsFlyingAround(menuPanels, gamePanels, player))
			{
				//TODO: implement sending of LAND command here
				return RESULT_RETRY;
			}
			if (PlayerOnPlanetMainScreen(menuPanels, gamePanels, player))
				return RESULT_DONE;

			return RESULT_FAIL;
			break;
		case TestStep::LOAD_GAME:
			player.Load(Files::Saves() + SaveGameName());
			return RESULT_DONE;
			break;
		default:
			// ERROR, unknown test-step-type
			// TODO: report error and exit with non-zero return-code
			return RESULT_FAIL;
			break;
	}
}


