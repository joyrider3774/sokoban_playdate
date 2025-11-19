#include <stdbool.h>
#include <string.h>
#include "GameFuncs.h"
#include "Common.h"
#include "Sound.h"
#include "Pd_helperfuncs.h"
#include "pd_api.h"


typedef struct {
	char SelectedLevelPack[101];
} SettingsData;

void LoadSettings()
{
	SDFile *Fp;
	Fp = pd->file->open("sokoban_settings.dat",kFileReadData);
	if (Fp)
	{
		SettingsData data;
		memset(&data, 0, sizeof(data));
		pd->file->read(Fp, &data, sizeof(data));		
		for (int i = 0; i < InstalledLevelPacksCount; i++)
			if(strcmp(data.SelectedLevelPack, InstalledLevelPacks[i]) == 0)
			{
				SelectedLevelPack = i;
				if (LevelPackName)
					pd->system->realloc(LevelPackName, 0);
				pd->system->formatString(&LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
				break;
			}
		pd->file->close(Fp);
	}
	else
	{
		//default to SokWhole
		for (int i = 0; i < InstalledLevelPacksCount; i++)
			if (strcmp("SokWhole.sok", InstalledLevelPacks[i]) == 0)
			{
				SelectedLevelPack = i;
				if (LevelPackName)
					pd->system->realloc(LevelPackName, 0);
				pd->system->formatString(&LevelPackName, "%s", InstalledLevelPacks[SelectedLevelPack]);
				break;
			}
	}

}

void SaveSettings()
{
	SDFile *Fp;
	Fp = pd->file->open("sokoban_settings.dat", kFileWrite);
	if (Fp)
	{
		SettingsData data;
		memset(&data, 0, sizeof(data));
		strncpy(data.SelectedLevelPack, LevelPackName, MaxLevelPackNameLength);
		pd->file->write(Fp, &data, sizeof(data));
		pd->file->close(Fp);
	}
}

void SearchForLevelPacksListFiles(const char* path, void* userdata)
{
	if (path[strlen(path)-1] == '/') //directory
	{
		if ((InstalledLevelPacksCount < MaxLevelPacks) && (strlen(path)-1 < MaxLevelPackNameLength))
		{
			char Name[MaxLevelPackNameLength+1];
			memset(Name, 0, sizeof(Name));
			strncpy(Name, path, strlen(path)-1);
			Name[strlen(path)] = '\0';
			char* ext = strstr(Name, "._lev");
			if (ext)
				*ext = 0;
			bool found = false;
			for (int i = 0; i < InstalledLevelPacksCount; i++)
			{
				if (strcmp(Name, InstalledLevelPacks[i]) == 0)
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				strcpy(InstalledLevelPacks[InstalledLevelPacksCount], Name);
				InstalledLevelPacksCount++;
			}
		}
	}
	//levelpackfiles
	else
	{
		if ((InstalledLevelPacksCount < MaxLevelPacks) && (strlen(path) < MaxLevelPackNameLength))
		{
			char* ext;
			char* dot = strrchr(path, '.');
			if (dot && !(dot == path))
			{
				ext = dot + 1;
				if ((strcmp(ext, "sok") == 0) || (strcmp(ext, "txt") == 0) || (strcmp(ext, "SOK") == 0) || (strcmp(ext, "TXT") == 0))
				{					
					bool found = false;
					for (int i = 0; i < InstalledLevelPacksCount; i++)
					{
						if (strcmp(path, InstalledLevelPacks[i]) == 0)
						{
							found = true;
							break;
						}
					}

					if (!found)
					{
						strcpy(InstalledLevelPacks[InstalledLevelPacksCount], path);
						InstalledLevelPacksCount++;
					}
				}
			}
		}
	}

	if (InstalledLevelPacksCount > 0)
	{
		if (LevelPackName)
			pd->system->realloc(LevelPackName, 0);
		pd->system->formatString(&LevelPackName, "%s", InstalledLevelPacks[SelectedLevelPack]);
	}
}

void SearchForLevelPacks()
{
	InstalledLevelPacksCount = 0;
	SelectedLevelPack = 0;
	if(LevelPackName)
		pd->system->realloc(LevelPackName, 0);
	pd->system->formatString(&LevelPackName, "%s", "");
	pd->file->listfiles("levelpacks", &SearchForLevelPacksListFiles, NULL, 0);
}


bool AskQuestionUpdate(int* Id, bool* Answer, bool MustBeAButton)
{
	*Answer = false;
	*Id = AskingQuestionID;

	if (AskingQuestionID > -1)
	{
		if ((currButtons & kButtonA) && !(prevButtons & kButtonA))
		{
			*Answer = true;
			AskingQuestion = false;
			AskingQuestionID = -1;
			prevButtons = currButtons;
			playMenuSelectSound();
			return true;
		}

		if ((currButtons & kButtonB) && !(prevButtons & kButtonB))
		{
			*Answer = false;
			AskingQuestion = false;
			AskingQuestionID = -1;
			prevButtons = currButtons;
			playMenuBackSound();
			return true;
		}
	}
	return false;
}

void AskQuestion(int Id, char* Msg)
{
	int fh = pd->graphics->getFontHeight(font);
	int count = 0;
	char* s = Msg;
	while (*s) {
		if (*s == '\n')
			count++;
		s++;
	}
	int diffy = 0;
	//add some lowering as title box is not centered
	if ((GameState == GSTitleScreen) || (GameState == GSLevelEditorMenu))
		diffy = 12;
	int MsgHeight = (count+1) * fh;
	int MsgWidth = pd->graphics->getTextWidth(font, Msg, strlen(Msg), kUTF8Encoding, 0);
	int MsgX = (ORIG_WINDOW_WIDTH - MsgWidth) >> 1;
	int MsgY = (ORIG_WINDOW_HEIGHT - MsgHeight) >> 1;
	pd->graphics->fillRect(MsgX -5 ,MsgY - 5 + diffy,MsgWidth + 10, MsgHeight + 10,kColorWhite);
	pd->graphics->drawRect(MsgX - 5, MsgY - 5 + diffy, MsgWidth + 10, MsgHeight + 10, kColorBlack);
	pd->graphics->drawRect(MsgX - 3, MsgY - 3 + diffy, MsgWidth + 6, MsgHeight + 6, kColorBlack);
	drawText(font, Msg, strlen(Msg), kUTF8Encoding, MsgX, MsgY + diffy);

	AskingQuestionID = Id;
	AskingQuestion = true;
	prevButtons = currButtons;
}

void LoadNormalCreatorName()
{
	memset(NormalCreateName, 0, sizeof(NormalCreateName));
	if (InstalledLevelPacksCount > 0)
	{
		pd->system->formatString(&FileName, "levelpacks/%s._lev/credits.dat", LevelPackName);
		if (!FileExists(FileName, true))
		{
			pd->system->realloc(FileName, 0);
			pd->system->formatString(&FileName, "levelpacks/%s/credits.dat", LevelPackName);
		}
		SDFile* Fp = pd->file->open(FileName, kFileReadData);
		if(!Fp)
			Fp = pd->file->open(FileName, kFileRead);
		if (Fp)
		{

			pd->file->seek(Fp, 0, SEEK_END);
			int size = pd->file->tell(Fp);
			char *LevelPackCreator = pd->system->realloc(NULL, size + 1);
			memset(LevelPackCreator, 0, sizeof(LevelPackCreator));
			pd->file->seek(Fp, 0, SEEK_SET);
			pd->file->read(Fp, LevelPackCreator, size);
			pd->file->close(Fp);
			strncpy(NormalCreateName, LevelPackCreator, 20);
			pd->system->realloc(LevelPackCreator,0);
		}
		pd->system->realloc(FileName, 0);
	}
}

void SaveUnlockData()
{
	SDFile *Fp;
	pd->system->formatString(&FileName,"%s.dat",LevelPackName);
	Fp = pd->file->open(FileName,kFileWrite);
	if (Fp)
	{
		pd->file->write(Fp, &UnlockedLevels,sizeof(UnlockedLevels));
		pd->file->close(Fp);
	}
	pd->system->realloc(FileName, 0);
}

void LoadUnlockData()
{
	SDFile *Fp;
 	UnlockedLevels = 1;
	pd->system->formatString(&FileName, "%s.dat", LevelPackName);
	Fp = pd->file->open(FileName, kFileReadData);
	if (Fp)
	{
		pd->file->read(Fp, &UnlockedLevels,sizeof(UnlockedLevels));
		pd->file->close(Fp);
		if ((UnlockedLevels > InstalledLevels) || (UnlockedLevels < 1))
			UnlockedLevels = 1;
	}
	pd->system->realloc(FileName, 0);
}

char* GetString(int Id, int x, int y,char* Msg, size_t MaxLen)
{
	char* Result = NULL;
	if (MaxLen > 0)
	{
		GetStringX = x;
		GetStringY = y;
		AskingGetString = true;
		GetStringId = Id;

		pd->graphics->fillRect(40 * UI_WIDTH_SCALE, 70 * UI_HEIGHT_SCALE, 240 * UI_WIDTH_SCALE, 100 * UI_HEIGHT_SCALE, kColorWhite);
		pd->graphics->drawRect(40 * UI_WIDTH_SCALE, 70 * UI_HEIGHT_SCALE, 240 * UI_WIDTH_SCALE, 100 * UI_HEIGHT_SCALE, kColorBlack);
		pd->graphics->drawRect(42 * UI_WIDTH_SCALE, 72 * UI_HEIGHT_SCALE, 237 * UI_WIDTH_SCALE, 97 * UI_HEIGHT_SCALE, kColorBlack);
		drawText(BigFont, Msg, strlen(Msg), kASCIIEncoding, 65 * UI_WIDTH_SCALE, 75 * UI_HEIGHT_SCALE);

		char* Tekst;
		pd->system->formatString(&Tekst,"    Use Up, Down, Left, Right\n      A=Ok           B=Cancel");
		drawText(BigFont, Tekst, strlen(Tekst), kASCIIEncoding, 65 * UI_WIDTH_SCALE, 125 * UI_HEIGHT_SCALE);
		pd->system->realloc(Tekst, 0);
    	printTitleInfo();
		MaxLenGetString = MaxLen;
		Result = pd->system->realloc(NULL, MaxLen+1);
		if(Result)
			memset(Result, 0, MaxLen + 1);
	}
	else
	{
		GetStringId = -1;
		AskingGetString = false;
		GetStringX = 0;
		GetStringY = 0;
		MaxLenGetString = 0;
	}
	return Result;
}

void getStringDraw(char* StringBuffer, bool ErasingPrevious)
{
	char* textBuffer;
	size_t len = strlen(StringBuffer);
	textBuffer = pd->system->realloc(NULL, len + 3);
	memset(textBuffer, 0, len + 3);
	strcpy(textBuffer, StringBuffer);
	textBuffer[len - 1] = '[';
	textBuffer[len] = StringBuffer[len - 1];
	textBuffer[len + 1] = ']';
	textBuffer[len + 2] = '\0';
	
	pd->graphics->fillRect(51*UI_WIDTH_SCALE,102*UI_HEIGHT_SCALE,218*UI_WIDTH_SCALE,18*UI_HEIGHT_SCALE,kColorWhite);
	drawText(BigFont,textBuffer,strlen(textBuffer), kASCIIEncoding, 75*UI_WIDTH_SCALE,102*UI_HEIGHT_SCALE);	
	pd->system->realloc(textBuffer, 0);
}

bool getStringUpdate(int *Id, bool* Answered, char* StringBuffer)
{
	*Id = GetStringId;
	*Answered = false;

	if (GetStringId == -1)
		return false;

	if (StringBuffer == NULL)
		return false;
	
	size_t len = strlen(StringBuffer);
	if (len == 0)
	{
		StringBuffer[0] = 'a';
		getStringDraw(StringBuffer, false);
	}
	
	if ((currButtons & kButtonB) && !(prevButtons & kButtonB))
	{
		memset(StringBuffer, 0, len);
		*Answered = false;
		GetStringId = -1;
		AskingGetString = false;
		prevButtons = currButtons;
		playMenuBackSound();
		return true;
	}

	if ((currButtons & kButtonA) && !(prevButtons & kButtonA))
	{
		*Answered = true;
		GetStringId = -1;
		AskingGetString = false;
		prevButtons = currButtons;
		playMenuSelectSound();
		return true;
	}

	if ((currButtons & kButtonUp) && !(prevButtons & kButtonUp))
	{
		getStringDraw(StringBuffer, true);
		char val = StringBuffer[len - 1];
		val++;
		if ((val == 'z' + 1))
		{
			val = '0';
		}
		else
		{
			if (val == '9' + 1)
			{
				val = 'a';
			}
		}
		StringBuffer[len - 1] = val;
		getStringDraw(StringBuffer, false);
		playMenuSelectSound();
	}

	if ((currButtons & kButtonDown) && !(prevButtons & kButtonDown))
	{
		getStringDraw(StringBuffer, true);
		char val = StringBuffer[len - 1];
		val--;
		if ((val == 'a' - 1))
		{
			val = '9';
		}
		else
		{
			if (val == '0' - 1)
			{
				val = 'z';
			}
		}
		StringBuffer[len - 1] = val;
		getStringDraw(StringBuffer, false);
		playMenuSelectSound();
	}

	if ((currButtons & kButtonLeft) && !(prevButtons & kButtonLeft))
	{
		if (len > 1)
		{
			getStringDraw(StringBuffer, true);
			StringBuffer[len - 1] = '\0';
			getStringDraw(StringBuffer, false);
			playMenuSelectSound();
		}
	}

	if ((currButtons & kButtonRight) && !(prevButtons & kButtonRight))
	{
		if ((len < MaxLenGetString))
		{
			getStringDraw(StringBuffer, true);
			StringBuffer[len] = StringBuffer[len-1];
			getStringDraw(StringBuffer, false);
			playMenuSelectSound();
		}
	}

	return false;
}



void FindLevels()
{
	int InstalledLevelsFile = LevelPackFile->LevelCount;
	int Teller=InstalledLevelsFile + 1;
	char* FileName2;
	char* FileName3;
	
	pd->system->formatString(&FileName2,"levelpacks/%s/level%d.lev",LevelPackName,Teller);
	pd->system->formatString(&FileName3,"levelpacks/%s._lev/level%d.lev",LevelPackName, Teller);
	while (FileExists(FileName2, true) || FileExists(FileName2, false) || FileExists(FileName3, true))
	{
		pd->system->realloc(FileName2, 0);
		pd->system->realloc(FileName3, 0);
		Teller+=30;
		pd->system->formatString(&FileName2, "levelpacks/%s/level%d.lev", LevelPackName, Teller);
		pd->system->formatString(&FileName3, "levelpacks/%s._lev/level%d.lev", LevelPackName, Teller);
	}
	while (!FileExists(FileName2, true) && !FileExists(FileName2, false) && !FileExists(FileName3, true) && (Teller > InstalledLevelsFile))
	{
		pd->system->realloc(FileName2, 0);
		pd->system->realloc(FileName3, 0);
		Teller--;
		pd->system->formatString(&FileName2, "levelpacks/%s/level%d.lev", LevelPackName, Teller);
		pd->system->formatString(&FileName3, "levelpacks/%s._lev/level%d.lev", LevelPackName, Teller);
	}
	pd->system->realloc(FileName2, 0);
	pd->system->realloc(FileName3, 0);
	InstalledLevels=Teller;
}

void LoadGraphics()
{
	if(IMGBackground)
		pd->graphics->freeBitmap(IMGBackground);
	if(IMGFloor)
		pd->graphics->freeBitmap(IMGFloor);
	if(IMGPlayer)
		pd->graphics->freeBitmap(IMGPlayer);
	if(IMGBox)
		pd->graphics->freeBitmap(IMGBox);
	if(IMGSpot)
		pd->graphics->freeBitmap(IMGSpot);
	if(IMGEmpty)
		pd->graphics->freeBitmap(IMGEmpty);
	if(IMGWall)
		pd->graphics->freeBitmap(IMGWall);
	if(IMGTitleScreen)
		pd->graphics->freeBitmap(IMGTitleScreen);

	IMGFloor = loadImageAtPath("graphics/floor.png");
	IMGWall = loadImageAtPath("graphics/wall.png");
	IMGBox = loadImageAtPath("graphics/box.png");
	IMGSpot = loadImageAtPath("graphics/spot.png");
	IMGPlayer = loadImageAtPath("graphics/player.png");
	IMGEmpty = loadImageAtPath("graphics/empty.png");
	IMGBackground = loadImageAtPath("graphics/background.png");
	IMGTitleScreen = loadImageAtPath("graphics/titlescreen.png");
}

void UnLoadGraphics()
{
	if(IMGBackground)
		pd->graphics->freeBitmap(IMGBackground);
	if(IMGFloor)
		pd->graphics->freeBitmap(IMGFloor);
	if(IMGPlayer)
		pd->graphics->freeBitmap(IMGPlayer);
	if(IMGBox)
		pd->graphics->freeBitmap(IMGBox);
	if(IMGSpot)
		pd->graphics->freeBitmap(IMGSpot);
	if(IMGEmpty)
		pd->graphics->freeBitmap(IMGEmpty);
	if(IMGWall)
		pd->graphics->freeBitmap(IMGWall);
	if(IMGTitleScreen)
		pd->graphics->freeBitmap(IMGTitleScreen);

	IMGBackground = NULL;
	IMGFloor = NULL;
	IMGPlayer = NULL;
	IMGBox = NULL;
	IMGSpot = NULL;
	IMGEmpty = NULL;
	IMGWall = NULL;
	IMGTitleScreen = NULL;
}

void printTitleInfo()
{
	char Tekst[250];
	int w;
	if(LevelPackFile->Loaded)
	{		
		if(strlen(LevelPackFile->author) > 0)
		{
			strcpy(Tekst, "Levels by ");
			strcat(Tekst, LevelPackFile->author);
			w = pd->graphics->getTextWidth(RobotoMedium, Tekst, strlen(Tekst), kUTF8Encoding, 0);
			drawText(RobotoMedium, Tekst, strlen(Tekst), kUTF8Encoding, (ORIG_WINDOW_WIDTH - w) / 2, 200);
		}
	}
	else
	{
		if (strlen(NormalCreateName) > 0)
		{
			strcpy(Tekst, "Levels by ");
			strcat(Tekst, NormalCreateName);
			w = pd->graphics->getTextWidth(RobotoMedium, Tekst, strlen(Tekst), kUTF8Encoding, 0);
			drawText(RobotoMedium, Tekst, strlen(Tekst), kUTF8Encoding, (ORIG_WINDOW_WIDTH - w) / 2, 200);
		}
	}
}

char chr(int ascii)
{
	return((char)ascii);
}

int ord(char chr)
{
	return((int)chr);
}