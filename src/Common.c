#include <stdbool.h>
#include "Common.h"
#include "Defines.h"
#include "CWorldParts.h"
#include "CLevelPackFile.h"
#include "CSelector.h"
#include "pd_api.h"

LCDBitmap* IMGBackground=NULL,*IMGFloor=NULL,*IMGPlayer=NULL,*IMGBox=NULL,*IMGSpot=NULL,*IMGEmpty=NULL,*IMGWall=NULL,*IMGTitleScreen=NULL;
LCDFont* font,* BigFont,*MonoFont, *RobotoMedium;
int GameState = GSTitleScreenInit;
int Selection=0, MusicCount=0,SelectedMusic=0,InstalledLevelPacksCount=0,InstalledLevels=0,SelectedLevel=0,SelectedLevelPack=0,UnlockedLevels=1,GetStringId = -1, GetStringX = 0, GetStringY = 0;
bool LevelEditorMode=false,LevelHasChanged=false,StageReload=false,ReloadMusic=false;
CWorldParts* WorldParts;
CLevelPackFile *LevelPackFile;
char *LevelPackName = NULL;
char InstalledLevelPacks[MaxLevelPacks][MaxLevelPackNameLength];
bool isCustomnTitleScreen;
int WINDOW_WIDTH = 400;
int WINDOW_HEIGHT = 240;
char *FileName = NULL;
int AskingQuestionID = -1;
size_t MaxLenGetString = 0;
bool AskingQuestion = false;
bool AskingGetString = false;
bool showFPS = true;
char NormalCreateName[21];
bool paused = false;
unsigned long framecount = 0, frameUpStart = 0, frameDownStart = 0, frameLeftStart = 0, frameRightStart = 0, frameAStart = 0;
PDButtons currButtons, prevButtons;