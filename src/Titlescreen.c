#include <string.h>
#include <stdbool.h>
#include "Defines.h"
#include "Titlescreen.h"
#include "Common.h"
#include "GameFuncs.h"
#include "Pd_helperfuncs.h"
#include "Sound.h"
#include "pd_api.h"
#include "Crank.h"

void TitleScreenInit()
{
	setCrankMoveThreshold(180);
	Selection = 1;
	SelectMusic(musTitle);
	if (InstalledLevelPacksCount > 0)
	{
		pd->system->formatString(&FileName, "levelpacks/%s", LevelPackName);
		CLevelPackFile_loadFile(LevelPackFile,FileName, NrOfCols, NrOfRows, true);
		pd->system->realloc(FileName, 0);
		LoadNormalCreatorName();
	}
	int total = 0;
	if (InstalledLevelPacksCount > 0)
	{
		for (SelectedLevelPack = 0; SelectedLevelPack < InstalledLevelPacksCount; SelectedLevelPack++)
		{
			pd->system->formatString(&FileName, "levelpacks/%s", InstalledLevelPacks[SelectedLevelPack]);
			if (!FileExists(FileName, false))
			{
				pd->system->realloc(FileName, 0);
				pd->system->formatString(&FileName, "levelpacks/%s", InstalledLevelPacks[SelectedLevelPack]);
			}
			CLevelPackFile_loadFile(LevelPackFile, FileName, NrOfCols, NrOfRows, false);
			//WorldParts->LoadFromLevelPackFile(LevelPackFile, SelectedLevel, true);
			pd->system->realloc(FileName, 0);
			total += LevelPackFile->LevelCount;
		}
	}
	pd->system->logToConsole("total levels:%d\n", total);
}


void TitleScreen()
{
	uint8_t * frame = pd->graphics->getFrame();
    int Teller;
	char* Tekst;
	


	if (GameState == GSTitleScreenInit)
	{
		TitleScreenInit();
		GameState = GSTitleScreen;
	}
	int CrankMove = crankUpdate();
	int id = -1;
	bool response = false;
	if(!AskingQuestion)
	{
		pd->graphics->drawBitmap(IMGTitleScreen,0,0,kBitmapUnflipped);
		printTitleInfo();

		pd->graphics->fillRect(40*UI_WIDTH_SCALE,70*UI_HEIGHT_SCALE,240*UI_WIDTH_SCALE,100*UI_HEIGHT_SCALE,kColorWhite);
		pd->graphics->drawRect(40*UI_WIDTH_SCALE,70*UI_HEIGHT_SCALE,240*UI_WIDTH_SCALE,100*UI_HEIGHT_SCALE,kColorBlack);
		pd->graphics->drawRect(41*UI_WIDTH_SCALE,71.5*UI_HEIGHT_SCALE,238*UI_WIDTH_SCALE,97.5*UI_HEIGHT_SCALE,kColorBlack);
		pd->system->formatString(&Tekst,"Play Selected LevelPack\nLevel Editor\n<%s>\nCredits",LevelPackName);
		drawText(BigFont,Tekst,strlen(Tekst),kASCIIEncoding,70*UI_WIDTH_SCALE,77*UI_HEIGHT_SCALE);
		pd->system->realloc(Tekst, 0);
		Tekst = pd->system->realloc(NULL, 100);
		if (Selection > 1)
		{			
			strcpy(Tekst," \n");
			for(Teller=2;Teller<Selection;Teller++)
				strcat(Tekst,"\n");
			strcat(Tekst,">>");
		}
		else
			strcpy(Tekst,">>");
		drawText(BigFont,Tekst,strlen(Tekst),kASCIIEncoding, 45*UI_WIDTH_SCALE,77*UI_HEIGHT_SCALE);
		pd->system->realloc(Tekst, 0);
		
		if (currButtons & kButtonLeft)
		{
			if (!(prevButtons & kButtonLeft))
				frameLeftStart = framecount;
			if ((framecount - frameLeftStart) % MenuUpdateTicks == 0)
			{
				if (Selection == 3)
					if (InstalledLevelPacksCount > 0)
					{
						SelectedLevelPack--;
						if (SelectedLevelPack < 0)
							SelectedLevelPack = InstalledLevelPacksCount - 1;
						if (LevelPackName)
							pd->system->realloc(LevelPackName, 0);
						pd->system->formatString(&LevelPackName, "%s", InstalledLevelPacks[SelectedLevelPack]);
						pd->system->formatString(&FileName, "levelpacks/%s", LevelPackName);
						CLevelPackFile_loadFile(LevelPackFile, FileName, NrOfCols, NrOfRows, true);
						pd->system->realloc(FileName, 0);
						LoadNormalCreatorName();
						playMenuSound();
						SaveSettings();
					}
			}
		}
		
		if (currButtons & kButtonRight)
		{
			if (!(prevButtons & kButtonRight))
				frameRightStart = framecount;
			if ((framecount - frameRightStart) % MenuUpdateTicks == 0)
			{
				if (Selection == 3)
					if (InstalledLevelPacksCount > 0)
					{
						SelectedLevelPack++;
						if (SelectedLevelPack > InstalledLevelPacksCount - 1)
							SelectedLevelPack = 0;
						if (LevelPackName)
							pd->system->realloc(LevelPackName, 0);
						pd->system->formatString(&LevelPackName, "%s", InstalledLevelPacks[SelectedLevelPack]);
						pd->system->formatString(&FileName, "levelpacks/%s", LevelPackName);
						CLevelPackFile_loadFile(LevelPackFile, FileName, NrOfCols, NrOfRows, true);
						pd->system->realloc(FileName, 0);
						LoadNormalCreatorName();
						playMenuSound();
						SaveSettings();
					}
			}
		}
		if ((currButtons & kButtonUp) || (CrankMove == CRANKMOVELEFT))
		{
			if (!(prevButtons & kButtonUp) && !(CrankMove == CRANKMOVELEFT))
				frameUpStart = framecount;
			if (((framecount - frameUpStart) % MenuUpdateTicks == 0) || (CrankMove == CRANKMOVELEFT))
			{
				if (Selection > 1)
				{
					Selection--;
					playMenuSound();
				}
			}
		}

		if ((currButtons & kButtonDown) || (CrankMove == CRANKMOVERIGHT))
		{
			if (!(prevButtons & kButtonDown) && !(CrankMove == CRANKMOVERIGHT))
				frameDownStart = framecount;
			if (((framecount - frameDownStart) % MenuUpdateTicks == 0) || (CrankMove == CRANKMOVERIGHT))
			{
				if (Selection < 4)
				{
					Selection++;
					playMenuSound();
				}
			}
		}

		if ((currButtons & kButtonA) && !(prevButtons & kButtonA))
		{
			switch(Selection)
			{
				case 1:
					if (InstalledLevelPacksCount >0)
					{
						pd->system->formatString(&FileName, "levelpacks/%s", LevelPackName);
						CLevelPackFile_loadFile(LevelPackFile,FileName, NrOfCols, NrOfRows, false);
						pd->system->realloc(FileName, 0);
						LoadNormalCreatorName();
						FindLevels();
						if(InstalledLevels > 0)
						{										
							LoadUnlockData();
							SelectedLevel=UnlockedLevels;
							LevelEditorMode=false;
							GameState=GSStageSelectInit;
							playMenuSelectSound();
						}
						else
						{
							playMenuSelectSound();
							pd->system->formatString(&Tekst,"There are no levels found in levelpack\n%s\n\nPlease create a level for this level pack\nfirst!",LevelPackName);
							AskQuestion(IDNoLevelsInPack, Tekst);
							pd->system->realloc(Tekst, 0);
						}
					}
					break;
				case 2:
					GameState=GSLevelEditorMenuInit;
					LevelEditorMode=true;
					playMenuSelectSound();
					break;
				case 3:
					if (InstalledLevelPacksCount > 0)
					{
						SelectedLevelPack++;
						if(SelectedLevelPack > InstalledLevelPacksCount-1)
							SelectedLevelPack = 0;
						if(LevelPackName)
							pd->system->realloc(LevelPackName, 0);
						pd->system->formatString(&LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
						pd->system->formatString(&FileName, "levelpacks/%s", LevelPackName);
						CLevelPackFile_loadFile(LevelPackFile, FileName, NrOfCols, NrOfRows, true);
						pd->system->realloc(FileName, 0);
						LoadNormalCreatorName();
						playMenuSelectSound();
						SaveSettings();
					}		
					break;
				case 4:
					GameState=GSCreditsInit;
					playMenuSelectSound();
					break;
			}
		}
		
	}
	AskQuestionUpdate(&id, &response, false);
}
