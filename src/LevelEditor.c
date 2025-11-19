#include <string.h>
#include <stdbool.h>
#include "LevelEditor.h"
#include "Common.h"
#include "GameFuncs.h"
#include "CSelector.h"
#include "Pd_helperfuncs.h"
#include "Crank.h"
#include "Menus.h"
#include "Sound.h"
#include "pd_api.h"

bool ShowPosition=true;
int buttonIgnoreFramesLevelEditor = 25;

CSelector *Selector;

bool LevelErrorsFound()
{
	int Teller,NumBoxes=0,NumSpots=0,NumPlayer=0,NumFilledSpots=0;
	for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
	{
		if(WorldParts->Items[Teller]->Type == IDBox)
		{
			NumBoxes++;
			if (WorldParts->Items[Teller]->AnimPhase == 1)
                NumFilledSpots++;
		}
		if(WorldParts->Items[Teller]->Type == IDSpot)
			NumSpots++;
		if(WorldParts->Items[Teller]->Type == IDPlayer)
			NumPlayer++;
	}
	if (NumPlayer==0)
	{
		playErrorSound();
		AskQuestion(IDNoPlayer, "Can not save this level because there's\nno player in the level! Please add a Player\nand try again or cancel saving.\n\nPress (A) to continue");
		return true;
	}
	if (NumBoxes < NumSpots)
	{
		playErrorSound();
		AskQuestion(IDMoreSpotsThanBoxes, "Can not save this level because the number\nof spots is greater then the number of boxes!\nPlease Add some more boxes and try again\nor cancel saving.\nPress (A) to continue");
		return true;
	}
	return false;

}

void PlayIfNoErrorsFound()
{
	if(!AskingQuestion && !LevelErrorsFound())
	{
		playMenuSelectSound();
		CWorldParts_Save(WorldParts, "sokoban_temp.lev");
		DestroyMenuItems();
		StageReload = true;
		GameState=GSGameInit;
	}
}

void DeleteAllParts()
{
		
	if (!AskingQuestion && (WorldParts->ItemCount > 0))
	{
		playMenuSelectSound();
		AskQuestion(IDDeleteAllParts, "You are about to delete all parts\nin this level, are you sure\nyou want to do this?\n\nPress (A) to delete, (B) to cancel");
		
	}
}

void CenterLevel()
{
	if (!AskingQuestion)
		LevelHasChanged |= CWorldParts_CenterLevel(WorldParts);
}


void LevelEditorInit()
{
	setCrankMoveThreshold(90);
	Selector = CSelector_Create();
	SelectMusic(musTitle);
	CreateLevelEditorMenuItems();
	if (StageReload)
	{
		CWorldParts_Load(WorldParts, "sokoban_temp.lev", true);
		pd->file->unlink("sokoban_temp.lev", false);
		StageReload=false;
	}
	buttonIgnoreFramesLevelEditor = 50;
}

void LevelEditor()
{
    int Teller,Teller2;
	bool AnotherPartFound,SamePartFound,SpotFound;
	char* Tekst;
	if (GameState == GSLevelEditorInit)
	{
		LevelEditorInit();
		GameState = GSLevelEditor;
	}

	bool response;
	int id;
	if(AskingQuestion)
		buttonIgnoreFramesLevelEditor = 25;
	if (!AskingQuestion)
	{
		pd->graphics->drawBitmap(IMGBackground,0,0,kBitmapUnflipped);
		CWorldParts_Move(WorldParts);
		CWorldParts_Draw(WorldParts);
		CSelector_Draw(Selector);
		if (ShowPosition)
		{			
			pd->system->formatString(&Tekst,"X: %d - Y: %d",CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector));
			pd->graphics->fillRect(ORIG_WINDOW_WIDTH-60*UI_WIDTH_SCALE,0, 60 * UI_WIDTH_SCALE, 17*UI_HEIGHT_SCALE,kColorWhite);
			pd->graphics->drawRect(ORIG_WINDOW_WIDTH-60*UI_WIDTH_SCALE,-1, 61 * UI_WIDTH_SCALE, 18*UI_HEIGHT_SCALE,kColorBlack);
			drawText(font,Tekst,strlen(Tekst),kASCIIEncoding, 262*UI_WIDTH_SCALE,2*UI_HEIGHT_SCALE);
			pd->system->realloc(Tekst, 0);
		}

		int crankMove = crankUpdate();
		buttonIgnoreFramesLevelEditor--;
		if (buttonIgnoreFramesLevelEditor < 0)
			buttonIgnoreFramesLevelEditor = 0;
		if ((buttonIgnoreFramesLevelEditor <= 0) && (currButtons & kButtonA))
		{
			if (!(prevButtons & kButtonA))
				frameAStart = framecount;
			if ((framecount - frameAStart) % LevelEditorUpdateTicks == 0)
			{
				SamePartFound = false;
				for (Teller = 0; Teller < WorldParts->ItemCount; Teller++)
				{
					if ((WorldParts->Items[Teller]->PlayFieldX == CSelector_GetPlayFieldX(Selector)) &&
						(WorldParts->Items[Teller]->PlayFieldY == CSelector_GetPlayFieldY(Selector)))
					{
						if (WorldParts->Items[Teller]->Type == CSelector_GetSelection(Selector))
						{
							SamePartFound = true;
						}
						if (CSelector_GetSelection(Selector) == IDEmpty)
						{
							LevelHasChanged = true;
							break;
						}
					}
				}
				if (CSelector_GetSelection(Selector) != IDEmpty)
					if (!LevelHasChanged)
						LevelHasChanged = !SamePartFound;
				switch (CSelector_GetSelection(Selector))
				{
				case IDEmpty:
					for (Teller = 0; Teller < WorldParts->ItemCount; Teller++)
					{
						if ((WorldParts->Items[Teller]->PlayFieldX == CSelector_GetPlayFieldX(Selector)) &&
							(WorldParts->Items[Teller]->PlayFieldY == CSelector_GetPlayFieldY(Selector)))
						{
							if (WorldParts->Items[Teller]->Type == IDFloor)
							{
								AnotherPartFound = false;
								SpotFound = false;
								for (Teller2 = Teller + 1; Teller2 < WorldParts->ItemCount; Teller2++)
								{
									if ((WorldParts->Items[Teller2]->PlayFieldX == CSelector_GetPlayFieldX(Selector)) &&
										(WorldParts->Items[Teller2]->PlayFieldY == CSelector_GetPlayFieldY(Selector)))
									{
										if (WorldParts->Items[Teller2]->Type == IDSpot)
											SpotFound = true;
										else
										{
											CWorldParts_RemoveType(WorldParts, WorldParts->Items[Teller2]->PlayFieldX, WorldParts->Items[Teller2]->PlayFieldY, WorldParts->Items[Teller2]->Type);
											AnotherPartFound = true;
											break;
										}

									}
								}
								if (SpotFound && !AnotherPartFound)
								{
									CWorldParts_RemoveType(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDSpot);
								}
								else
								{
									if (!SpotFound && !AnotherPartFound)
										CWorldParts_RemoveType(WorldParts, WorldParts->Items[Teller]->PlayFieldX, WorldParts->Items[Teller]->PlayFieldY, IDFloor);
								}
								break;
							}
							else
							{
								if (WorldParts->Items[Teller]->Type == IDSpot)
								{
									AnotherPartFound = false;
									for (Teller2 = Teller + 1; Teller2 < WorldParts->ItemCount; Teller2++)
									{
										if ((WorldParts->Items[Teller2]->PlayFieldX == CSelector_GetPlayFieldX(Selector)) &&
											(WorldParts->Items[Teller2]->PlayFieldY == CSelector_GetPlayFieldY(Selector)) &&
											(WorldParts->Items[Teller2]->Type != IDFloor))
										{
											CWorldParts_RemoveType(WorldParts, WorldParts->Items[Teller2]->PlayFieldX, WorldParts->Items[Teller2]->PlayFieldY, WorldParts->Items[Teller2]->Type);
											AnotherPartFound = true;
											break;
										}
									}
									if (!AnotherPartFound)
										CWorldParts_RemoveType(WorldParts, WorldParts->Items[Teller]->PlayFieldX, WorldParts->Items[Teller]->PlayFieldY, IDSpot);
									break;
								}
								else
								{
									if (WorldParts->Items[Teller]->Type == IDBox)
									{
										CWorldParts_RemoveType(WorldParts, WorldParts->Items[Teller]->PlayFieldX, WorldParts->Items[Teller]->PlayFieldY, IDBox);
										break;
									}
									else
									{
										if (WorldParts->Items[Teller]->Type == IDPlayer)
										{
											CWorldParts_RemoveType(WorldParts, WorldParts->Items[Teller]->PlayFieldX, WorldParts->Items[Teller]->PlayFieldY, IDPlayer);
											break;
										}
										else
										{
											CWorldParts_Remove(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector));
											break;
										}
									}
								}
							}
						}
					}

					break;
				case IDBox:
					CWorldParts_RemoveType(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWall);
					CWorldParts_RemoveType(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox);
					CWorldParts_RemoveType(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer);
					CWorldParts_Add(WorldParts, CWorldPart_Create(CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), false, IDBox));
					break;
				case IDPlayer:
					CWorldParts_RemoveType(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWall);
					CWorldParts_RemoveType(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox);
					for (Teller = 0; Teller < WorldParts->ItemCount; Teller++)
					{
						if (WorldParts->Items[Teller]->Type == IDPlayer)
							CWorldParts_RemoveType(WorldParts, WorldParts->Items[Teller]->PlayFieldX, WorldParts->Items[Teller]->PlayFieldY, IDPlayer);
					}
					CWorldParts_Add(WorldParts, CWorldPart_Create(CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), false, IDPlayer));
					break;
				case IDWall:
					CWorldParts_Remove(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector));
					CWorldParts_Add(WorldParts, CWorldPart_Create(CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), false, IDWall));
					break;
				case IDSpot:
					CWorldParts_RemoveType(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWall);
					CWorldParts_RemoveType(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDSpot);
					CWorldParts_Add(WorldParts, CWorldPart_Create(CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), false, IDSpot));
					break;
				case IDFloor:
					CWorldParts_RemoveType(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDFloor);
					CWorldParts_RemoveType(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWall);
					CWorldParts_Add(WorldParts, CWorldPart_Create(CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), false, IDFloor));
					break;
				}
			}
		}

		if (currButtons & kButtonLeft)
		{
			if (!(prevButtons & kButtonLeft))
				frameLeftStart = framecount;
			if ((framecount - frameLeftStart) % LevelEditorUpdateTicks == 0)
			{
				buttonIgnoreFramesLevelEditor = 0;
				CSelector_MoveLeft(Selector);
			}
		}

		if (currButtons & kButtonRight)
		{
			if (!(prevButtons & kButtonRight))
				frameRightStart = framecount;
			if ((framecount - frameRightStart) % LevelEditorUpdateTicks == 0)
			{
				buttonIgnoreFramesLevelEditor = 0;
				CSelector_MoveRight(Selector);
			}
		}

		if (currButtons & kButtonUp)
		{
			if (!(prevButtons & kButtonUp))
				frameUpStart = framecount;
			if ((framecount - frameUpStart) % LevelEditorUpdateTicks == 0)
			{
				buttonIgnoreFramesLevelEditor = 0;
				CSelector_MoveUp(Selector);
			}
		}

		if (currButtons & kButtonDown)
		{
			if (!(prevButtons & kButtonDown))
				frameDownStart = framecount;
			if ((framecount - frameDownStart) % LevelEditorUpdateTicks == 0)
			{
				buttonIgnoreFramesLevelEditor = 0;
				CSelector_MoveDown(Selector);
			}
		}

		if (crankMove == CRANKMOVELEFT)
		{
			buttonIgnoreFramesLevelEditor = 0;
			CSelector_DecSelection(Selector);
		}

		if (crankMove == CRANKMOVERIGHT)
		{
			buttonIgnoreFramesLevelEditor = 0;
			CSelector_IncSelection(Selector);
		}


		if ((currButtons & kButtonB) && !(prevButtons & kButtonB))
		{
			if (LevelHasChanged)
			{
				playMenuSelectSound();
				AskQuestion(IDCurrentLevelNotSaved, "The current level isn't saved yet!\nDo you want to save it now ?\n\nPress (A) to save, (B) to cancel saving");
			}
			else
			{
				GameState = GSStageSelectInit;
				playMenuBackSound();
			}
		}		
	}
	else
	{
		if(AskQuestionUpdate(&id, &response, false))
		{
			if (id == IDDeleteAllParts)
			{
				if(response)
				{
					LevelHasChanged = true;
					CWorldParts_RemoveAll(WorldParts);
				}
			}

			if (id == IDCurrentLevelNotSaved)
			{
				if(response)
				{
					pd->graphics->drawBitmap(IMGBackground,0,0,kBitmapUnflipped);
					CWorldParts_Move(WorldParts);
					CWorldParts_Draw(WorldParts);
					CSelector_Draw(Selector);
					if (ShowPosition)
					{
						pd->system->formatString(&Tekst, "X: %d - Y: %d", CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector));
						pd->graphics->fillRect(ORIG_WINDOW_WIDTH - 50 * UI_WIDTH_SCALE, 0, 50 * UI_WIDTH_SCALE, 13 * UI_HEIGHT_SCALE, kColorWhite);
						pd->graphics->drawRect(ORIG_WINDOW_WIDTH - 50 * UI_WIDTH_SCALE, -1, 51 * UI_WIDTH_SCALE, 14 * UI_HEIGHT_SCALE, kColorBlack);
						drawText(font, Tekst, strlen(Tekst), kASCIIEncoding, 272 * UI_WIDTH_SCALE, 2 * UI_HEIGHT_SCALE);
						pd->system->realloc(Tekst, 0);
					}
					if (!LevelErrorsFound())
					{						
						pd->file->mkdir("levelpacks");
						pd->system->formatString(&FileName,"levelpacks/%s._lev", LevelPackName);
						pd->file->mkdir(FileName);
						pd->system->realloc(FileName, 0);
						if (SelectedLevel==0)
							pd->system->formatString(&FileName,"levelpacks/%s._lev/level%d.lev",LevelPackName,InstalledLevels+1);
						else
							pd->system->formatString(&FileName,"levelpacks/%s._lev/level%d.lev",LevelPackName,SelectedLevel);

						CWorldParts_Save(WorldParts, FileName);
						pd->system->realloc(FileName, 0);
						FindLevels();
						if (SelectedLevel==0)
							SelectedLevel = InstalledLevels;
						LevelHasChanged=false;
						GameState = GSStageSelectInit;									
					}

				}
				else
					GameState = GSStageSelectInit;
			}
		}
	}

	if (GameState != GSLevelEditor)
	{
		DestroyMenuItems();
		CSelector_Destroy(Selector);
	}
}
