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
#include "MainPanel.h"
#include "Panel.h"
#include "PlanetPanel.h"
#include "PlayerInfo.h"
#include "Ship.h"
#include "TestStep.h"
#include "UI.h"
#include <string>


using namespace std;


TestStep::TestStep()
{
	// Initialize with some sensible default values.
	stepType = 0;
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

int TestStep::DoStep(int stepAction, UI &menuPanels, UI &gamePanels, PlayerInfo &player)
{
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
				Ship * playerFlagShip = player.Flagship();
				if (playerFlagShip == nullptr)
					return RESULT_FAIL;
				if (stepAction == 0)
				{
					// Send the land command here
					// TODO: Player commands are handled in AI.cpp (at the moment this code was written), not handled on flagship. Change this code or change handling in AI.
					// TODO: Landing should also have a stellar target (just to reduce ambiguity)
					// TODO: Different command to start hovering over planet?
					Command command;
					command.Set(Command::LAND);
					playerFlagShip->SetCommands(command);
					stepAction++;
				}
				return RESULT_NEXTACTION;
			}
			if (PlayerOnPlanetMainScreen(menuPanels, gamePanels, player))
				return RESULT_DONE;

			// Unknown state/screen. Landing fails.
			return RESULT_FAIL;
			break;
		case TestStep::LOAD_GAME:
			if (stepAction == 0){
				// Check if the savegame actually exists
				if (! Files::Exists(Files::Saves() + SaveGameName()))
					return RESULT_FAIL;
				// Perform the load and verify that player is loaded.
				player.Load(Files::Saves() + SaveGameName());
				if (!player.IsLoaded())
					return RESULT_FAIL;
				// Enable testmode to prevent automatic saving.
				player.SetTestMode();
				// Actual load succeeded. Allow game to adopt to new
				// situation and then continue with enter/pilot step.
				return RESULT_NEXTACTION;
			}
			else if (stepAction == 1)
			{
				// Clear the menu entries and go to main game screen
				if (! menuPanels.IsEmpty())
					menuPanels.Pop(menuPanels.Top().get());
				// Transfer control to game before final check to allow
				// closing of menuPanel.
				return RESULT_NEXTACTION;
			}
			else if (stepAction == 2)
			{
				if (! menuPanels.IsEmpty())
					return RESULT_FAIL;
				// TODO: this should be called/loaded from LoadPanel
				gamePanels.Reset();
				return RESULT_NEXTACTION;
			}
			else if (stepAction == 3)
			{
				// TODO: this should be called/loaded from LoadPanel
				gamePanels.Push(new MainPanel(player));
				return RESULT_NEXTACTION;
			}
			else if (stepAction == 4)
			{
				if (gamePanels.IsEmpty())
					return RESULT_FAIL;
				return RESULT_DONE;
			}
			else
				return RESULT_FAIL;

			break;
		default:
			// ERROR, unknown test-step-type
			// TODO: report error and exit with non-zero return-code
			return RESULT_FAIL;
			break;
	}
}


