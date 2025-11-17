#include <string.h>
#include <stdbool.h>
#include "GameFuncs.h"
#include "CWorldParts.h"
#include "Common.h"
#include "Defines.h"
#include "Game.h"
#include "LevelEditor.h"
#include "LevelEditorMenu.h"
#include "StageSelect.h"
#include "Credits.h"
#include "Titlescreen.h"
#include "Pd_helperfuncs.h"
#include "Sound.h"
#include "Common.h"
#include "pd_api.h"



void setupGame()
{
	//Reset Globals first

	GameState = GSTitleScreenInit;
	Selection = 0;
	InstalledLevelPacksCount = 0;
	InstalledLevels = 0;
	SelectedLevel = 0;
	SelectedLevelPack = 0;
	UnlockedLevels = 1;
	GetStringId = -1;
	GetStringX = 0;
	GetStringY = 0;
	bool GlobalSoundEnabled = true;
	LevelEditorMode = false;
	LevelHasChanged = false;
	StageReload = false;
	ReloadMusic = false;
	LevelPackName = NULL;
	FileName = NULL;
	isCustomnTitleScreen = false;
	AskingQuestionID = -1;
	MaxLenGetString = 0;
	AskingQuestion = false;
	AskingGetString = false;
	showFPS = false;
	paused = false;
	framecount = 0;
	//normal game stuff

	srand(pd->system->getCurrentTimeMilliseconds());
	font = loadFontAtPath("fonts/Roboto-Black-12");
	BigFont = loadFontAtPath("fonts/Roboto-Black-16");
	RobotoMedium = loadFontAtPath("fonts/Roboto-Black-24");
	WorldParts = CWorldParts_Create();

	SearchForLevelPacks();
	LoadSettings();
	initSound();
	initMusic();
	LoadGraphics();
	setMusicOn(true);
	setSoundOn(true);
	LevelPackFile = CLevelPackFile_Create();

}

void terminateGame()
{
	CWorldParts_RemoveAll(WorldParts);
	CWorldParts_Destroy(WorldParts);
	CLevelPackFile_Destroy(LevelPackFile);
	SaveSettings();
	UnLoadGraphics();
	deInitSound();
	deInitMusic();
	font = NULL;
	BigFont = NULL;
	RobotoMedium = NULL;
}


int mainLoop(void* ud)
{
	if (paused)
		return 1;
	prevButtons = currButtons;
	pd->system->getButtonState(&currButtons, NULL, NULL);
	
	switch(GameState)
	{
		case GSTitleScreenInit:
		case GSTitleScreen :
			TitleScreen();
			break;
		case GSCreditsInit:
		case GSCredits :
			Credits();
			break;
		case GSGameInit:
		case GSGame :
			Game();
			break;
		case GSStageSelectInit:
		case GSStageSelect:
			StageSelect();
			break;
		case GSLevelEditorInit:
		case GSLevelEditor:
			LevelEditor();
			break;
		case GSLevelEditorMenuInit:
		case GSLevelEditorMenu:
			LevelEditorMode = true;
			LevelEditorMenu();
			break;
	}
	if (showFPS)
		pd->system->drawFPS(0, 0);
	framecount++;
	return 1;
}
