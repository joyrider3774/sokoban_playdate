#include <string.h>
#include <stdbool.h>
#include "Common.h"
#include "GameFuncs.h"
#include "Pd_helperfuncs.h"
#include "Sound.h"
#include "pd_api.h"

#include "Menus.h"


CWorldPart *Player;

bool StageDone()
{
	int Teller,FilledSpots=0,Spots=0;
	for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
		if (WorldParts->Items[Teller]->Type == IDBox)
		{
			if (WorldParts->Items[Teller]->AnimPhase == 1)
                FilledSpots++;
		}
		else
            if (WorldParts->Items[Teller]->Type == IDSpot)
                Spots++;
    if (FilledSpots >= Spots)
        return true;
    else
        return false;
}

void RestartLevel()
{
	if (!AskingQuestion)
	{
		playMenuSelectSound();
		pd->graphics->drawBitmap(IMGBackground, 0, 0, kBitmapUnflipped);
		CWorldParts_Draw(WorldParts);
		AskQuestion(IDRestartLevel, "You are about to restart this level\nAre you sure you want to restart?\n\nPress (A) to Restart (B) to Cancel");
	}
}

void ReturnToLevelEditor()
{
	if(!AskingQuestion && LevelEditorMode)
	{
		GameState=GSLevelEditorInit;
		DestroyMenuItems();
		playMenuBackSound();
	}
}

void ShowGameInfo()
{
	if (!AskingQuestion)
	{
		char* Msg;
		playMenuSelectSound();
		pd->graphics->drawBitmap(IMGBackground, 0, 0, kBitmapUnflipped);
#ifdef SDL2API
		CWorldParts_DrawFloor(WorldParts, Player);
#endif
		CWorldParts_Draw(WorldParts);
		if (LevelPackFile->Loaded)
		{
			if (strlen(LevelPackFile->LevelsMeta[SelectedLevel - 1].author) > 0)
				pd->system->formatString(&Msg, "Level Pack: %s\nLevel: %d/%d - Moves: %d - Pushes: %d\nAuthor: %s\nComments: %s", LevelPackName, SelectedLevel, InstalledLevels, WorldParts->Moves, WorldParts->Pushes, LevelPackFile->LevelsMeta[SelectedLevel - 1].author, LevelPackFile->LevelsMeta[SelectedLevel - 1].comments);
			else
				pd->system->formatString(&Msg, "Level Pack: %s\nLevel: %d/%d - Moves: %d - Pushes: %d\nAuthor: %s\nComments: %s", LevelPackName, SelectedLevel, InstalledLevels, WorldParts->Moves, WorldParts->Pushes, LevelPackFile->author, LevelPackFile->LevelsMeta[SelectedLevel - 1].comments);
		}
		else
			pd->system->formatString(&Msg, "Level Pack: %s\nLevel: %d/%d - Moves: %d - Pushes: %d\nAuthor: %s", LevelPackName, SelectedLevel, InstalledLevels, WorldParts->Moves, WorldParts->Pushes, NormalCreateName);
		AskQuestion(IDLevelInfo, Msg);
		pd->system->realloc(Msg, 0);
	}
}

void GameInit()
{
	int Teller;
	SelectMusic(musGame1 + (rand() % 3));
	CreateGameMenuItems();
	if (LevelEditorMode && StageReload)
	{
		CWorldParts_Load(WorldParts, "sokoban_temp.lev", true);
	}
	Player=NULL;
	for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
	{
		if (WorldParts->Items[Teller]->Type == IDPlayer)
		{
			Player = WorldParts->Items[Teller];
			break;
		}
	}
	//should never happen
	if(!Player)
	{
		Player = CWorldPart_Create(0,0, true, IDPlayer);
		CWorldParts_Add(WorldParts,Player);
	}

}

void Game()
{
	int Teller;
	char* Msg;
	if(GameState == GSGameInit)
	{
		GameInit();
		GameState = GSGame;
	}
	bool response = false;
	int id = -1;
	if(!AskingQuestion)
	{
		if (!Player->IsMoving && (WorldParts->Moves >0) && StageDone())
		{
			playLevelDoneSound();
			if (LevelEditorMode)
			{
				pd->system->formatString(&Msg, "Congratulations !\nYou Succesfully Solved this level in\n%d moves and %d pushes\nDo you want to return to the\nlevel editor ?\n\n(A) Leveleditor (B) Play Again", WorldParts->Moves, WorldParts->Pushes);
				AskQuestion(IDSolvedLevelLevelEditorMode, Msg);
				pd->system->realloc(Msg, 0);
			}
			else
			{
				if (SelectedLevel == UnlockedLevels)
				{
					if ( UnlockedLevels < InstalledLevels)
					{
						pd->system->formatString(&Msg,"Congratulations !\nYou Succesfully Solved Level %d/%d in\n%d moves and %d pushes\nThe next level has now been unlocked!\n\nPress (A) to continue",SelectedLevel,InstalledLevels,WorldParts->Moves, WorldParts->Pushes);
						AskQuestion(IDSolvedLevelNextUnlocked, Msg);
						pd->system->realloc(Msg, 0);
					}
					else
					{
						pd->system->formatString(&Msg,"Congratulations !\nYou Succesfully Solved Level %d/%d in\n%d moves and %d pushes\nlevelpack %s\nis now finished, try out another one!\n\nPress (A) to continue",SelectedLevel,InstalledLevels,WorldParts->Moves, WorldParts->Pushes,LevelPackName);
						AskQuestion(IDSolvedLastLevel, Msg);
						pd->system->realloc(Msg, 0);
							
					}
				}
				else
				{
					pd->system->formatString(&Msg,"Congratulations !\nYou Succesfully Solved Level %d/%d in\n%d moves and %d pushes\n\n\nPress (A) to continue",SelectedLevel,InstalledLevels,WorldParts->Moves, WorldParts->Pushes);
					AskQuestion(IDSolvedEarlierLevel, Msg);
					pd->system->realloc(Msg, 0);
				}
			}
		}
		
		if((currButtons & kButtonB) && !(prevButtons & kButtonB))
		{
			if(!LevelEditorMode)
			{
				GameState = GSStageSelectInit;

				playMenuBackSound();
			}
		}

		if (!Player->IsMoving)
		{
			if (currButtons & kButtonA)
			{
				CWorldParts_HistoryGoBack(WorldParts);
			}

			if (!(currButtons & kButtonA) && (currButtons & kButtonRight))
			{
				if (CWorldPart_CanMoveTo(Player, Player->PlayFieldX + 1, Player->PlayFieldY))
				{
					CWorldParts_HistoryAdd(WorldParts);
				}
				CWorldPart_MoveTo(Player, Player->PlayFieldX + 1, Player->PlayFieldY, false);
			}
			else
			{
				if (!(currButtons & kButtonA) && (currButtons & kButtonLeft))
				{
					if (CWorldPart_CanMoveTo(Player, Player->PlayFieldX - 1, Player->PlayFieldY))
					{
						CWorldParts_HistoryAdd(WorldParts);
					}
					CWorldPart_MoveTo(Player, Player->PlayFieldX - 1, Player->PlayFieldY, false);
				}
				else
				{
					if (!(currButtons & kButtonA) && (currButtons & kButtonUp))
					{
						if (CWorldPart_CanMoveTo(Player, Player->PlayFieldX, Player->PlayFieldY - 1))
						{
							CWorldParts_HistoryAdd(WorldParts);
						}
						CWorldPart_MoveTo(Player, Player->PlayFieldX, Player->PlayFieldY - 1, false);
					}
					else
					{
						if (!(currButtons & kButtonA) && (currButtons & kButtonDown))
						{
							if (CWorldPart_CanMoveTo(Player, Player->PlayFieldX, Player->PlayFieldY + 1))
							{
								CWorldParts_HistoryAdd(WorldParts);
							}
							CWorldPart_MoveTo(Player, Player->PlayFieldX, Player->PlayFieldY + 1, false);
						}
					}
				}
			}
		}

		

		if(!AskingQuestion)
		{
			pd->graphics->drawBitmap(IMGBackground,0,0,kBitmapUnflipped);
			CWorldParts_Move(WorldParts);
#ifdef SDL2API
			CWorldParts_DrawFloor(WorldParts,WorldParts->Player);
#endif
			CWorldParts_Draw(WorldParts);
		}
	}
	else
	{
		if(AskQuestionUpdate(&id, &response, false))
		{
			if(id == IDSolvedLevelLevelEditorMode)
			{
				if (response)
				{
					GameState = GSLevelEditorInit;
				}
				else
				{
					CWorldParts_RemoveAll(WorldParts);
					CWorldParts_Load(WorldParts,"sokoban_temp.lev", false);
					for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
					{
						if (WorldParts->Items[Teller]->Type == IDPlayer)
						{
							Player = WorldParts->Items[Teller];
							break;
						}
					}
				}
			}

			if(id == IDSolvedLevelNextUnlocked)
			{
				UnlockedLevels++;
				SelectedLevel++;
				SaveUnlockData();
				GameState = GSStageSelectInit;
			}

			if(id == IDSolvedLastLevel)
			{
				GameState = GSTitleScreenInit;
			}

			if (id == IDSolvedEarlierLevel)
			{
				GameState = GSStageSelectInit;
			}

			if (id == IDRestartLevel)
			{
				if(response)
				{
					if (LevelEditorMode && StageReload)
					{
						CWorldParts_Load(WorldParts, "sokoban_temp.lev", true);
					}
					else
					{
						pd->system->formatString(&FileName, "levelpacks/%s/level%d.lev", LevelPackName, SelectedLevel);
						if (FileExists(FileName, true))
							CWorldParts_Load(WorldParts, FileName, true);
						else
							CWorldParts_LoadFromLevelPackFile(WorldParts, LevelPackFile, SelectedLevel, true);
						pd->system->realloc(FileName, 0);
					}
					Player = NULL;
					for (Teller = 0; Teller < WorldParts->ItemCount; Teller++)
					{
						if (WorldParts->Items[Teller]->Type == IDPlayer)
						{
							Player = WorldParts->Items[Teller];
							break;
						}
					}
					Player = NULL;
					for (Teller = 0; Teller < WorldParts->ItemCount; Teller++)
					{
						if (WorldParts->Items[Teller]->Type == IDPlayer)
						{
							Player = WorldParts->Items[Teller];
							break;
						}
					}

					//should never happen
					if (!Player)
					{
						Player = CWorldPart_Create(0, 0, true, IDPlayer);
						CWorldParts_Add(WorldParts, Player);
					}
				}
			}
		}
	}

	if (GameState != GSGame)
	{
		DestroyMenuItems();
	}	
}
